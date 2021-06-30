/* 

                          Firewall Builder

                 Copyright (C) 2002-2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/


#include "PolicyCompiler_ipt.h"
#include "OSConfigurator_linux24.h"
#include "ipt_utils.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/ObjectMatcher.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/physAddress.h"

#include "combinedAddress.h"

#include <stack>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include <assert.h>
#include <cstdlib>
#include <climits>

#include <QString>
#include <QRegExp>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


static int chain_no=0;
static std::list<std::string> standard_chains;


const std::list<std::string>& PolicyCompiler_ipt::getStandardChains()
{
    if (standard_chains.size()==0)
    {
        standard_chains.push_back("INPUT");
        standard_chains.push_back("OUTPUT");
        standard_chains.push_back("FORWARD");
        standard_chains.push_back("PREROUTING");
        standard_chains.push_back("POSTROUTING");
        standard_chains.push_back("RETURN");
        standard_chains.push_back("LOG");
        standard_chains.push_back("ACCEPT");
        standard_chains.push_back("DROP");
        standard_chains.push_back("REJECT");
        standard_chains.push_back("MARK");
        standard_chains.push_back("CONNMARK");
        standard_chains.push_back("QUEUE");
        standard_chains.push_back("CLASSIFY");
        standard_chains.push_back("ROUTE");

        // pseudo-targets
        standard_chains.push_back(".CUSTOM");
        standard_chains.push_back(".CONTINUE");
    }
    return standard_chains;
}

PolicyCompiler_ipt::~PolicyCompiler_ipt()
{
//    if (printRule) delete printRule;
    while (chains.size() > 0)
    {
        map<string, chain_list*>::iterator i = chains.begin();
        chain_list *cl = i->second;
        chains.erase(i);
        delete cl;
    }
}

string PolicyCompiler_ipt::myPlatformName() { return "iptables"; }

void PolicyCompiler_ipt::registerChain(const string &chain_name)
{
    map<string, chain_list*>::iterator i = chains.find(chain_name);
    if (i==chains.end())
    {
        chain_list *cl = new chain_list();
        cl->push_back(chain_name);
        chains[chain_name] = cl;
    }
}

void PolicyCompiler_ipt::insertUpstreamChain(const string &chain_name,
                                             const string &before_chain)
{
    if (!chain_name.empty())
    {
        map<string, chain_list*>::iterator i = chains.find(before_chain);
        if (i==chains.end()) return;
        chain_list *cl = i->second;
        list<string>::iterator j = std::find(cl->begin(),
                                             cl->end(), before_chain);
        cl->insert(j, chain_name);
    }
}

string PolicyCompiler_ipt::findUpstreamChain(const string &chain_name)
{
    map<string, chain_list*>::iterator i = chains.find(chain_name);
    if (i==chains.end()) return "";
    return i->second->front();
}

void PolicyCompiler_ipt::setChain(PolicyRule *rule, const string &chain_name)
{
    rule->setStr("ipt_chain", chain_name);
    string target = rule->getStr("ipt_target");
    if (!target.empty())
    {
        registerChain(target);
        insertUpstreamChain(chain_name, target);
    }
}

string PolicyCompiler_ipt::printChains(PolicyRule *rule)
{
    string chain_name = rule->getStr("ipt_chain");
    map<string, chain_list*>::iterator i = chains.find(chain_name);
    if (i==chains.end() || i->second->size()==0) return chain_name;
    ostringstream res;
    for (list<string>::iterator j=i->second->begin(); j!=i->second->end(); ++j)
    {
        res << *j;
        if (*j == chain_name) res << "*";
        res << "[" 
            << chain_usage_counter[*j]
            << "],";
    }
    return res.str();
}

bool PolicyCompiler_ipt::isChainDescendantOfOutput(const string &chain_name)
{
    map<string, chain_list*>::iterator i = chains.find(chain_name);
    if (i==chains.end()) return false;
    for (list<string>::iterator j=i->second->begin(); j!=i->second->end(); ++j)
        if (*j=="OUTPUT") return true;
    return false;
}

bool PolicyCompiler_ipt::isChainDescendantOfInput(const string &chain_name)
{
    map<string, chain_list*>::iterator i = chains.find(chain_name);
    if (i==chains.end()) return false;
    for (list<string>::iterator j=i->second->begin(); j!=i->second->end(); ++j)
        if (*j=="INPUT") return true;
    return false;
}

/*
 * this function generates acceptable shell variable name from
 * interface name. Note that OSConfigurator_linux24::getInterfaceVarName()
 * does the same and these two functions should be identical.
 *
 *  TODO: really need to have one function for this instead of two in
 * two different classes.
 */
string PolicyCompiler_ipt::getInterfaceVarName(FWObject *iface, bool v6)
{
    ostringstream  ostr;
    string iname = iface->getName();
    string::size_type p1;
    while ( (p1=iname.find("."))!=string::npos)
        iname=iname.replace(p1,1,"_");
    while ( (p1=iname.find("-"))!=string::npos)
        iname=iname.replace(p1,1,"_");
    ostr << "i_" << iname;
    if (v6) ostr << "_v6";
    return ostr.str();
}

string PolicyCompiler_ipt::getAddressTableVarName(FWObject *at)
{
    ostringstream  ostr;
    string name = at->getName();
    string::size_type p1;
    const char *bad_shell_chars = " !#$&*()-+=\\|{}[]?<>,.:";
    for (const char *cptr=bad_shell_chars; *cptr; cptr++)
    {
        while ((p1 = name.find(*cptr)) != string::npos)
            name = name.replace(p1,1,"_");
    }
    ostr << "at_" << name;
    return ostr.str();
}

string PolicyCompiler_ipt::getNewTmpChainName(PolicyRule *rule)
{
    std::ostringstream str;
    string chain_id = rule->getUniqueId();
    int    n = tmp_chain_no[chain_id];

    str << "C" << chain_id;
    str << "." <<  setw(1) << setfill('0') << n;

    n++;
    tmp_chain_no[chain_id]=n;
    return str.str();


#if 0
    std::ostringstream str;
    str << "ptmp" << setw(3) << setfill('0') << chain_no;
    chain_no++;
    return str.str();
#endif
}

string PolicyCompiler_ipt::getNewChainName(PolicyRule *rule,
                                           Interface *rule_iface)
{
    std::ostringstream str;

/* if interface name ends with '*', this is a wildcard interface. We
 * do not want '*' to get incorporated into the chain name, so we
 * replace it with '_' */

    if (rule_iface)
    {
        string iface_name = rule_iface->getName();
        string::size_type n=iface_name.find("*");
        str << iface_name.substr(0,n) << "_";
    }

    switch (rule->getDirection())
    {
    case PolicyRule::Inbound:   str << "In_";  break;
    case PolicyRule::Outbound:  str << "Out_"; break;
    default: ;
    }
    int pos = rule->getPosition();

    string ruleset_name = getRuleSetName();

    if (ruleset_name != "Policy")
        str << ruleset_name << "_";
    else
        str << "RULE_";

    if (pos>=0)
        str << pos;
    else // special case: position == -1
        str << "000";

    string suffix = rule->getStr("subrule_suffix");
    if (!suffix.empty()) str << "_" << suffix;

    string chain_name = str.str();
    int n = rule_chain_no[chain_name];
    n++;
    rule_chain_no[chain_name] = n;

    // if (n > 1)
    // {
    //     str << "_" << n;
    // }

    string full_chain_name = str.str();

    chain_no++;

    return full_chain_name;
}

void PolicyCompiler_ipt::_expand_interface(Rule *rule,
                                           Interface *iface,
                                           std::list<FWObject*> &list_result,
                                           bool expand_cluster_interfaces_fully)
{
    std::list<FWObject*> ol1;

    Compiler::_expand_interface(
        rule, iface, ol1, expand_cluster_interfaces_fully);

    // see utils.cpp
    expand_interface_with_phys_address(this, rule, iface, ol1, list_result);
}

string PolicyCompiler_ipt::getActionOnReject(PolicyRule *rule)
{
    FWOptions  *ruleopt =rule->getOptionsObject();
    return ruleopt->getStr("action_on_reject");
}

bool   PolicyCompiler_ipt::isActionOnRejectTCPRST(PolicyRule *rule)
{
    string s = getActionOnReject(rule);
    return ( ! s.empty() && s.find("TCP ")!=string::npos );
}

/*
 * resets rule option "action_on_reject" so it won't be TCP RST
 * Algorithm:
 *
 *    if global option "action_on_reject" is not empty
 *      if global option is TCP RST
 *        set rule option value to "none"
 *      else
 *        copy value from global option to rule option
 *    else
 *      set rule option value to "none"
 *
 * 
 */
void PolicyCompiler_ipt::resetActionOnReject(PolicyRule *rule)
{
    FWOptions  *ruleopt =rule->getOptionsObject();            
    string go = getCachedFwOpt()->getStr("action_on_reject");

    if (!go.empty())
    {
        if ( go.find("TCP ")!=string::npos )
        {
            ruleopt->setStr("action_on_reject", "NOP"); // hack.
        } else
        {
            ruleopt->setStr("action_on_reject", go);
        }
    } else
        ruleopt->setStr("action_on_reject", "none"); // hack.
}

void PolicyCompiler_ipt::registerRuleSetChain(const std::string &chain_name)
{
    chain_usage_counter[chain_name] = 1;
}

void PolicyCompiler_ipt::verifyPlatform()
{
    string family = Resources::platform_res[fw->getStr("platform")]->
        getResourceStr("/FWBuilderResources/Target/family");

    if (family != myPlatformName()) 
	abort("Unsupported platform " + fw->getStr("platform") +
              " (family " + family + ")");
}

int PolicyCompiler_ipt::prolog()
{
    verifyPlatform(); 

    PolicyCompiler::prolog();

    addPredefinedPolicyRules();

    FWOptions *fwopt = getCachedFwOpt();

    // initialize counters for the standard chains
    for (list<string>::const_iterator i =
             PolicyCompiler_ipt::getStandardChains().begin();
         i != PolicyCompiler_ipt::getStandardChains().end(); ++i)
    {
        chain_usage_counter[*i] = 1;
    }

    Service     *anytcp, *anyudp, *anyicmp, *anyip;
    Address     *bcast255;
    TCPService  *tcpsyn;

    anytcp=dbcopy->createTCPService();
    anytcp->setId(FWObjectDatabase::registerStringId(ANY_TCP_OBJ_ID));
    anytcp->setName("AnyTCP");
    persistent_objects->add(anytcp);

    tcpsyn=dbcopy->createTCPService();
    tcpsyn->setId(FWObjectDatabase::registerStringId(TCP_SYN_OBJ_ID));
    tcpsyn->setName("tcpSYN");
    tcpsyn->setTCPFlag(TCPService::SYN,true);
    tcpsyn->setAllTCPFlagMasks();
    persistent_objects->add(tcpsyn);

    anyudp=dbcopy->createUDPService();
    anyudp->setId(FWObjectDatabase::registerStringId(ANY_UDP_OBJ_ID));
    anyudp->setName("AnyUDP");
    persistent_objects->add(anyudp);

    anyicmp=dbcopy->createICMPService();
    anyicmp->setId(FWObjectDatabase::registerStringId(ANY_ICMP_OBJ_ID));
    anyicmp->setName("AnyICMP");
    persistent_objects->add(anyicmp);

    anyip=dbcopy->createIPService();
    anyip->setId(FWObjectDatabase::registerStringId(ANY_IP_OBJ_ID));
    anyip->setName("AnyIP");
    persistent_objects->add(anyip);

    bcast255=dbcopy->createIPv4();
    bcast255->setId(FWObjectDatabase::registerStringId(BCAST_255_OBJ_ID));
    bcast255->setName("Broadcast_addr");
    bcast255->setAddress(InetAddr::getAllOnes());
    bcast255->setNetmask(InetAddr(InetAddr::getAllOnes()));
    persistent_objects->add(bcast255);

    bool global_afpa = fwopt->getBool("firewall_is_part_of_any_and_networks");
    int n = 0;
    for(FWObject::iterator i=source_ruleset->begin(); i!=source_ruleset->end(); i++)
    {
	Rule *r = Rule::cast( *i );
        if (r == nullptr) continue;
	if (r->isDisabled()) continue;

        FWOptions *ruleopt = r->getOptionsObject();
        string rule_afpa = ruleopt->getStr("firewall_is_part_of_any_and_networks");

        // in v3.0 rule options attribute "assume fw is part of any"
        // used to be a checkbox and therefore stored as boolean in
        // the rule options. Old "on" or "True" maps to "1", old "off"
        // or "False" maps to "use global" (it was impossible to turn
        // this option off for just one rule if it was on
        // globally). If this attribute has value of an empty string,
        // then we should use global setting from the firewall options
        // object.
        if (rule_afpa.empty())
            ruleopt->setInt("firewall_is_part_of_any_and_networks", global_afpa);
        if (rule_afpa == "True")
            ruleopt->setInt("firewall_is_part_of_any_and_networks", 1);
        if (rule_afpa == "False")
            ruleopt->setInt("firewall_is_part_of_any_and_networks", global_afpa);

        n++;
    }

    string version = fw->getStr("version");
    using_ipset = (XMLTools::version_compare(version, "1.4.1.1") >= 0 &&
                          fwopt->getBool("use_m_set"));
    actually_used_module_set = false;

    build_interface_groups(dbcopy, persistent_objects, fw, ipv6, regular_interfaces);

    // count bridge interfaces. We need this later in
    // PrintRule::_printDirectionAndInterface

    list<FWObject*> l2 = fw->getByTypeDeep(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
    {
        Interface *iface = Interface::cast(*i);
        assert(iface);

        string interface_type = iface->getOptionsObject()->getStr("type");
        if (interface_type == "bridge") bridge_count++;
    }

    return n;
}

void PolicyCompiler_ipt::addPredefinedPolicyRules()
{
    // if (getSourceRuleSet()->isTop() && !inSingleRuleCompileMode())
    // {
    //     insertConntrackRule();
    //     insertFailoverRule();
    // }
}

bool  PolicyCompiler_ipt::SkipActionContinueWithNoLogging::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if ( ! rule->getStr("ipt_target").empty() &&
         rule->getStr("ipt_target") == ".CONTINUE" &&
         ! rule->getLogging() &&
         ! rule->getTagging() &&
         ! rule->getClassification() &&
         ! rule->getRouting()) return true;  // skip this rule

    tmp_queue.push_back(rule);
    return true;
}

/*
 * by the time this processor is called all non-terminating rules should
 * be processed if terminating behevior needs to be emulated.
 */
bool  PolicyCompiler_ipt::dropTerminatingTargets::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    string tgt = rule->getStr("ipt_target");

    if (tgt=="CLASSIFY" || tgt=="MARK") tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::clearTagClassifyInFilter::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    if (ipt_comp->my_table != "mangle")
    {
        rule->setClassification(false);
        rule->setRouting(false);
        rule->setTagging(false);
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::clearActionInTagClassifyIfMangle::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    if (ipt_comp->my_table == "mangle" &&
        (rule->getTagging() || rule->getClassification())
    )
        rule->setAction(PolicyRule::Continue);

    tmp_queue.push_back(rule);
    return true;
}

/*
 * in a rule generates some code in both filter and mangle tables and
 * has logging turned on, we should log only once. Will log in filter.
 * However if the rule belongs to mangle-only rule set, we should log
 * in mangle.
 */
bool PolicyCompiler_ipt::clearLogInMangle::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    FWOptions *rulesetopts = ipt_comp->getSourceRuleSet()->getOptionsObject();
    if (rulesetopts->getBool("mangle_only_rule_set"))
    {
        tmp_queue.push_back(rule);
        return true;
    }

    if (ipt_comp->my_table == "mangle") rule->setLogging(false);
    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::splitIfTagClassifyOrRoute::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    FWOptions *ruleopt = rule->getOptionsObject();
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *r;
    Q_UNUSED(r);

    int number_of_options = 0;
    if (rule->getTagging()) number_of_options++;
    if (rule->getClassification()) number_of_options++;
    if (rule->getRouting()) number_of_options++;

    if (ipt_comp->my_table=="mangle" && number_of_options > 0)
    {
        RuleElementSrc *nsrc;
        RuleElementDst *ndst;
        RuleElementSrv *nsrv;
        RuleElementItf *nitfre;
        PolicyRule     *r, *r2;
        Q_UNUSED(r);
        Q_UNUSED(r2);

        string this_chain = rule->getStr("ipt_chain");
        string new_chain  = this_chain;

        nsrc = rule->getSrc();
        ndst = rule->getDst();
        nsrv = rule->getSrv();
        nitfre = rule->getItf();

        if (
            (! nsrc->isAny() || ! ndst->isAny() ||
             ! nsrv->isAny() || ! nitfre->isAny()) && number_of_options > 1
        )
        {
            new_chain = ipt_comp->getNewTmpChainName(rule);
            r = compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setStr("subrule_suffix", "ntt");
            r->setStr("ipt_target", new_chain);
            r->setClassification(false);
            r->setRouting(false);
            r->setTagging(false);
            r->setLogging(false);
            r->setAction(PolicyRule::Continue);
            tmp_queue.push_back(r);

            nsrc = rule->getSrc();   nsrc->reset();
            ndst = rule->getDst();   ndst->reset();
            nsrv = rule->getSrv();   nsrv->reset();
            nitfre = rule->getItf(); nitfre->reset();
            ruleopt = rule->getOptionsObject();
            ruleopt->setInt("limit_value",-1);
            ruleopt->setInt("limit_value",-1);
            ruleopt->setInt("connlimit_value",-1);
            ruleopt->setInt("hashlimit_value",-1);
            ruleopt->setBool("stateless",true);
            rule->setLogging(false);
        }

        if (rule->getTagging())
        {
            r = compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setClassification(false);
            r->setRouting(false);
            rule->setTagging(false);
            r->setStr("ipt_chain", new_chain);
            r->setStr("upstream_rule_chain", this_chain);
            r->setAction(PolicyRule::Continue);
            tmp_queue.push_back(r);
        }

        if (rule->getClassification())
        {
            r = compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            rule->setClassification(false);
            r->setRouting(false);
            r->setTagging(false);
            r->setStr("ipt_chain", new_chain);
            r->setStr("upstream_rule_chain", this_chain);
            r->setAction(PolicyRule::Continue);
            tmp_queue.push_back(r);
        }

        /*
         * Target ROUTE is terminating unless parameter "--continue"
         * is present. We add "--continue" if action is Continue,
         * otherwise the rule does not need to be split and we carry
         * action Accept further.
         */

        if (rule->getRouting() || rule->getAction() != PolicyRule::Continue)
        {
            rule->setClassification(false);
            rule->setTagging(false);
            rule->setStr("ipt_chain", new_chain);
            rule->setStr("upstream_rule_chain", this_chain);
            tmp_queue.push_back(rule);
        }
        
    } else
        tmp_queue.push_back(rule);

    return true;
}

// this version just splits rule so that each elementary rule is associated
// with one interface.

bool PolicyCompiler_ipt::InterfacePolicyRulesWithOptimization::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    RuleElementItf *itfre = rule->getItf(); assert(itfre);

    // sometimes for whatever reason itf rule element appears to be broken
    // (it is just empty, not even "any")
    if (itfre->isAny() || itfre->size()==0)
    {
        //rule->setInterfaceId(-1);
        tmp_queue.push_back(rule);
        return true;
    }

    for (FWObject::iterator i=itfre->begin(); i!=itfre->end(); ++i)
    {
        FWObject *o = FWObjectReference::getObject(*i);
        PolicyRule *r = compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        r->setStr("subrule_suffix", "i1");
        //r->setInterfaceId(o->getId());
        RuleElementItf *nitfre = r->getItf();
        nitfre->reset(); nitfre->addRef(o);
        tmp_queue.push_back(r);
    }
    return true;
}

/**
 * Deprecated beginning with 4.3.0. To be removed in future versions.
 */
bool PolicyCompiler_ipt::Route::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    FWOptions *ruleopt =rule->getOptionsObject();
    
    if (rule->getRouting())
    {
        string iif,oif,gw;
        iif = ruleopt->getStr("ipt_iif");
        oif = ruleopt->getStr("ipt_oif");
        gw  = ruleopt->getStr("ipt_gw");

        if (!iif.empty())
        {
            ipt_comp->setChain(rule, "PREROUTING");
        }

        if (!oif.empty() || !gw.empty())
        {
            ipt_comp->setChain(rule, "POSTROUTING");
        }

        if (ruleopt->getBool("ipt_tee"))
        {
            PolicyRule *r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            ipt_comp->setChain(rule, "PREROUTING");
            tmp_queue.push_back(r);

            r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            ipt_comp->setChain(rule, "POSTROUTING");
            tmp_queue.push_back(r);

            return true;
        }

    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::dropMangleTableRules::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);

    string ruleset_name = compiler->getRuleSetName();

    FWOptions *rulesetopts = ipt_comp->getSourceRuleSet()->getOptionsObject();
    if (rulesetopts->getBool("mangle_only_rule_set")) return true;

    if ( rule->getAction() == PolicyRule::Continue && ! rule->getLogging() &&
         (rule->getTagging() || rule->getRouting() || rule->getClassification()))
        return true;

    // Another special case (while working on #1415, although not
    // related directly): branching rule that has "branch in mangle table"
    // checkbox turned on and is branches to the "mangle only" rule set
    // does not need any iptables rules in the filter table
    FWOptions *ruleopt = rule->getOptionsObject();
    if (rule->getAction() == PolicyRule::Branch &&
        ruleopt->getBool("ipt_branch_in_mangle"))
    {
        RuleSet *ruleset = rule->getBranch();
        assert(ruleset!=nullptr);
        rulesetopts = ruleset->getOptionsObject();
        if (rulesetopts->getBool("mangle_only_rule_set")) return true;
    }

    tmp_queue.push_back(rule);

    return true;
}

/*
 *  Only call this rule processor if compiling for the mangle table
 */
bool PolicyCompiler_ipt::checkActionInMangleTable::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getAction() == PolicyRule::Reject)
    {
	compiler->abort(
            rule, 
            "Action Reject is not allowed in mangle table");
        return true;
    }

    tmp_queue.push_back(rule);

    return true;
}

/*
 * Rules with action Tag can only be in PREROUTING or OUTPUT chains,
 * rules with action Classify always go into POSTROUTING. This means
 * they can't conflict. But option Route can yield rules in PREROUTING
 * or POSTROUTING and can conflict. We'll flag combinations of Tag +
 * Route and Classify + Route if action is not Continue.
 */
bool PolicyCompiler_ipt::checkForUnsupportedCombinationsInMangle::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    if (ipt_comp->my_table=="mangle" &&
        rule->getAction() != PolicyRule::Continue &&
        rule->getRouting() &&
        (rule->getTagging() || rule->getClassification()))
    {
        QString err("Can not process option Route in combination with "
                    "options Tag or Classify and action %1");
	compiler->abort(
            rule,  err.arg(rule->getActionAsString().c_str()).toStdString());
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::deprecateOptionRoute::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getRouting())
    {
	compiler->abort(
            rule, 
            "Option Route is deprecated. You can use Custom Action "
            "to generate iptables command using '-j ROUTE' target "
            "if it is supported by your firewall OS");
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::Logging1::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if ( compiler->getCachedFwOpt()->getBool("log_all") )
        rule->setLogging(true);

    tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::storeAction::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    rule->setStr("stored_action", rule->getActionAsString() );

    rule->setBool("originated_from_a_rule_with_tagging", rule->getTagging());
    rule->setBool("originated_from_a_rule_with_classification",
                  rule->getClassification());
    rule->setBool("originated_from_a_rule_with_routing", rule->getRouting());

    tmp_queue.push_back(rule);
    return true;
}

/**
 * splits rule if logging is required and either src or dst is
 * not any
 */
bool PolicyCompiler_ipt::Logging2::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

#ifndef NDEBUG
    RuleElementItf *itf_re = rule->getItf(); assert(itf_re!=nullptr);
#endif

    RuleElementSrc      *nsrc;
    RuleElementDst      *ndst;
    RuleElementSrv      *nsrv;
    RuleElementInterval *nint;
    RuleElementItf      *nitfre;

    if (rule->getLogging()) 
    {
/*
 * see #2235 Rules with action Continue translate into iptables
 * commands without "-j TARGET" parameter, so we dont need to create new chain
 * for logging.
 */
        if (rule->getAction() == PolicyRule::Continue &&
            ( ! rule->getTagging() &&
              ! rule->getClassification() &&
              ! rule->getRouting()))
        {
            rule->setStr("ipt_target", "LOG");
            tmp_queue.push_back(rule);
            return true;
        }

/* 
 * chain could have been assigned if we split this rule before
 */
        string this_chain = rule->getStr("ipt_chain");
	string new_chain  = ipt_comp->getNewChainName(rule, nullptr); //rule_iface);

	PolicyRule *r;
        FWOptions  *ruleopt;

/*
 * if we are in the user-defined chain and src=dst=srv=int=any, then there is no
 * need to create a sub-chain. Otherwise, create new chain and handle logging
 * and actual original target there.
 */
        bool need_new_chain = true;

        if (this_chain==new_chain &&
            rule->getSrc()->isAny() &&
            rule->getDst()->isAny() &&
            rule->getSrv()->isAny() &&
            (rule->getWhen())!=nullptr && rule->getWhen()->isAny())
        {
            need_new_chain = false;
        }

/*
 * add copy of original rule, but turn off logging and set target
 * chain to new_chain.
 */
        if (need_new_chain) 
        {
            r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            ruleopt =r->getOptionsObject();
            r->setStr("ipt_target",new_chain);
            r->setClassification(false);
            r->setRouting(false);
            r->setTagging(false);
            r->setLogging(false);
            r->setAction(PolicyRule::Continue);    // ###
//        ruleopt->setInt("limit_value",-1);
            tmp_queue.push_back(r);
        }

/*
 * need to add two rules with the same rule_label, direction=both, no
 * interface, no src, no srv and no dst. One of these new rules should
 * have target = LOG and another should inherit action and therefore
 * target from original rule. Both new rules go into chain new_chain.
 * In both rules turn off stateful inspection.
 *
 * keep interface information in the first one to be able to process
 * %I in log prefix
 *
 */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        ruleopt =r->getOptionsObject();
	nsrc=r->getSrc();      nsrc->reset();
	ndst=r->getDst();      ndst->reset();
	nsrv=r->getSrv();      nsrv->reset();
        nitfre=r->getItf();    nitfre->reset();
	if ( (nint=r->getWhen())!=nullptr )  nint->reset();
	r->setStr("ipt_chain",new_chain);
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);

	r->setStr("ipt_target","LOG");
        r->setAction(PolicyRule::Continue);    // ###
	r->setDirection( PolicyRule::Both );
	r->setLogging(false);
        r->setClassification(false);
        r->setRouting(false);
        r->setTagging(false);
        ruleopt->setBool("stateless",true);
        r->setBool("force_state_check",false);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
 	tmp_queue.push_back(r);

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        ruleopt =r->getOptionsObject();
	nsrc=r->getSrc();   nsrc->reset();
	ndst=r->getDst();   ndst->reset();
	if ( (nint=r->getWhen())!=nullptr )  nint->reset();
        nitfre=r->getItf(); nitfre->reset();
/*
 * special case: need to preserve information about service protocol in case
 * action_on_reject is TCP RST
 */
        nsrv=r->getSrv();
        Service  *srv= compiler->getFirstSrv(r);
        if (TCPService::isA(srv))
        {
            nsrv->clearChildren();
            nsrv->addRef(compiler->dbcopy->findInIndex(
                             FWObjectDatabase::getIntId(ANY_TCP_OBJ_ID)));
        }
        else
        {
            nsrv->reset();
        }

	r->setStr("ipt_chain",new_chain);
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);

        r->setStr(".iface", "nil");
	//r->setInterfaceStr("nil");
	r->setDirection( PolicyRule::Both );
	r->setLogging(false);
        ruleopt->setBool("stateless",true);
        r->setBool("force_state_check",false);
        r->setBool("final",true);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);

	tmp_queue.push_back(r);
    } else
	tmp_queue.push_back(rule);

    return true;
}

string PolicyCompiler_ipt::printRuleElements::printRE(RuleElement *re)
{
    ostringstream str;
    if (re->size() == 0) return "<EMPTY>";
    FWObject *obj = FWReference::cast(re->front())->getPointer();
    str << "id=" << obj->getId()
        << " name=" << obj->getName()
        << " type=" << obj->getTypeName();
    return str.str();
}

bool PolicyCompiler_ipt::printRuleElements::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElementSrc *srcrel = rule->getSrc();
    RuleElementDst *dstrel = rule->getDst();
    RuleElementSrv *srvrel = rule->getSrv();

    cerr << "rule " << rule->getLabel()
         << "  src: " << printRE(srcrel)
         << "  dst: " << printRE(dstrel)
         << "  srv: " << printRE(srvrel)
         << endl;

    tmp_queue.push_back(rule);
    return true;
}

void PolicyCompiler_ipt::SingleRENegation::processSingleObjectNegationInRE(
    FWObject *obj, RuleElement *rel)
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);

    // We call singleSrcNegation before we replace AddressTable
    // objects with MultiAddressRunTime objects
    if (AddressTable::cast(obj) && AddressTable::cast(obj)->isRunTime() &&
        ipt_comp->using_ipset)
    {
        rel->setNeg(false);
        rel->setBool("single_object_negation", true);
        return;
    }

    Address *src = Address::cast(obj);
    // note: src can be nullptr if object in this rule element is a group
    // or MultiAddress
    if (src!=nullptr && src->countInetAddresses(true)==1 &&
        !compiler->complexMatch(src, compiler->fw)) 
    {
        rel->setNeg(false);
        rel->setBool("single_object_negation", true);
        return;
    }

    Service *srv = Service::cast(obj);
    // see comment in compile() where this rule processor is used for why
    // only some services can be processed here.
    if (TagService::isA(srv) || UserService::isA(srv))
    {
/*   A  B  ! C  ACTION  */
        rel->setNeg(false);
        rel->setBool("single_object_negation", true);
    }
}

bool PolicyCompiler_ipt::SingleRENegation::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    RuleElement *rel = RuleElement::cast(rule->getFirstByType(type_name));

/*   ! A  B  C  ACTION  */
    if (rel->getNeg() && rel->size()==1)
    {
        processSingleObjectNegationInRE(FWReference::getObject(rel->front()), rel);
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::SrcNegation::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule         *rule=getNext(); if (rule==nullptr) return false;
    FWOptions  *ruleopt =rule->getOptionsObject();
    string afpa = ruleopt->getStr("firewall_is_part_of_any_and_networks");

    RuleElementSrc *srcrel=rule->getSrc();

/*   ! A  B  C  D  ACTION  */

    if (srcrel->getNeg()) 
    {
	PolicyRule     *r;
	RuleElementSrc *nsrc;
	RuleElementDst *ndst;
	RuleElementSrv *nsrv;
	RuleElementInterval *nint;
        RuleElementItf *nitf;
        FWOptions      *ruleopt;

/*chain could have been assigned if we split this rule before */
        string this_chain  = rule->getStr("ipt_chain");
	string new_chain   = ipt_comp->getNewTmpChainName(rule);
	srcrel->setNeg(false);

        rule->setBool("upstream_rule_neg",true);

/*     any  B C D  TMP_CHAIN  */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setStr("subrule_suffix","1");
	nsrc=r->getSrc();  nsrc->reset();
        r->setClassification(false);
        r->setRouting(false);
        r->setTagging(false);
	r->setLogging(false);
	r->setStr("ipt_target",new_chain);
        ruleopt =r->getOptionsObject();
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
        ruleopt->setStr("firewall_is_part_of_any_and_networks", afpa);
	tmp_queue.push_back(r);

/* TMP_CHAIN   A  any any any  RETURN  */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setStr("subrule_suffix","2");

        if (!shadowing_mode)
        {
            ndst=r->getDst();    ndst->reset();
            nsrv=r->getSrv();    nsrv->reset();
            nitf=r->getItf();    nitf->reset();
            if ( (nint=r->getWhen())!=nullptr )  nint->reset();
        }

	r->setAction( PolicyRule::Return );
        r->setClassification(false);
        r->setRouting(false);
        r->setTagging(false);
	r->setLogging(false);
 	r->setStr("ipt_chain",new_chain);
	r->setStr("ipt_target","");
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);

        ruleopt =r->getOptionsObject();
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
        ruleopt->setBool("stateless",true);  // ###
	tmp_queue.push_back(r);

/* TMP_CHAIN   any any any any  ACTION  */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setStr("subrule_suffix","3");

        nsrc=r->getSrc();   nsrc->reset();

        if (!shadowing_mode)
        {
            ndst = r->getDst();   ndst->reset();
            nitf = r->getItf();   nitf->reset();
            if ( (nint=r->getWhen())!=nullptr )  nint->reset();
/*
 * special case: need to preserve information about service protocol in case
 * action_on_reject is TCP RST
 */
            nsrv=r->getSrv();
            Service  *srv= compiler->getFirstSrv(r);
            if (TCPService::isA(srv))
            {
                nsrv->clearChildren();
                nsrv->addRef(compiler->dbcopy->findInIndex(
                                 FWObjectDatabase::getIntId(ANY_TCP_OBJ_ID)));
            }
            else
            {
                nsrv->reset();
            }
        }
	r->setStr("ipt_chain",new_chain);
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);

	if ( ! rule->getStr("ipt_target").empty() )
	    r->setStr("ipt_target",rule->getStr("ipt_target"));

//	r->setInterfaceStr("nil");
        r->setBool("final",true);
        ruleopt =r->getOptionsObject();
        ruleopt->setBool("stateless",true);  // ###
	tmp_queue.push_back(r);

    } else
	tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::DstNegation::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    FWOptions  *ruleopt =rule->getOptionsObject();
    string afpa = ruleopt->getStr("firewall_is_part_of_any_and_networks");

    RuleElementDst *dstrel=rule->getDst();

/*   A  ! B  C  D  ACTION  */

    if (dstrel->getNeg()) 
    {
	PolicyRule     *r;
	RuleElementSrc *nsrc;
	RuleElementDst *ndst;
	RuleElementSrv *nsrv;
        RuleElementInterval *nint;
        RuleElementItf *nitf;
        FWOptions      *ruleopt;

/*chain could have been assigned if we split this rule before */
        string this_chain  = rule->getStr("ipt_chain");
	string new_chain   = ipt_comp->getNewTmpChainName(rule);
	dstrel->setNeg(false);

        rule->setBool("upstream_rule_neg",true);

/*     A any C D  TMP_CHAIN  */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setStr("subrule_suffix","1");
	ndst=r->getDst();  ndst->reset();
        r->setClassification(false);
        r->setRouting(false);
        r->setTagging(false);
	r->setLogging(false);
	r->setStr("ipt_target",new_chain);
        ruleopt =r->getOptionsObject();
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
        ruleopt->setStr("firewall_is_part_of_any_and_networks", afpa);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any B any any RETURN  */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setStr("subrule_suffix","2");

        if (!shadowing_mode)
        {
            nsrc=r->getSrc();   nsrc->reset();
            nsrv=r->getSrv();   nsrv->reset();
            nitf=r->getItf();   nitf->reset();
            if ( (nint=r->getWhen())!=nullptr )  nint->reset();
        }

	r->setAction( PolicyRule::Return );
        r->setClassification(false);
        r->setRouting(false);
        r->setTagging(false);
	r->setLogging(false);
	r->setStr("ipt_chain",new_chain);
	r->setStr("ipt_target","");
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);

        ruleopt =r->getOptionsObject();
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
        ruleopt->setBool("stateless",true);  // ###

//	r->setInterfaceStr("nil");
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  any any any  ACTION  */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setStr("subrule_suffix","3");

        ndst=r->getDst();   ndst->reset();

        if (!shadowing_mode)
        {
            nsrc=r->getSrc();   nsrc->reset();
            nitf=r->getItf();   nitf->reset();
            if ( (nint=r->getWhen())!=nullptr )  nint->reset();
/*
 * special case: need to preserve information about service protocol in case
 * action_on_reject is TCP RST
 */
            nsrv=r->getSrv();
            Service  *srv= compiler->getFirstSrv(r);
            if (TCPService::isA(srv))
            {
                nsrv->clearChildren();
                nsrv->addRef(compiler->dbcopy->findInIndex(
                                 FWObjectDatabase::getIntId(ANY_TCP_OBJ_ID)));
            }
            else
            {
                nsrv->reset();
            }
        }
	r->setStr("ipt_chain",new_chain);
	r->setStr("ipt_target","");
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);

	if ( ! rule->getStr("ipt_target").empty() )
	    r->setStr("ipt_target",rule->getStr("ipt_target"));

//	r->setInterfaceStr("nil");
        r->setBool("final",true);
        ruleopt =r->getOptionsObject();
        ruleopt->setBool("stateless",true);  // ###
	tmp_queue.push_back(r);

    } else
	tmp_queue.push_back(rule);

    return true;
}




bool PolicyCompiler_ipt::SrvNegation::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrv *srvrel=rule->getSrv();

/*   A  B  !C  D  ACTION  */

    if (srvrel->getNeg()) 
    {
	PolicyRule     *r;
	RuleElementSrc *nsrc;
	RuleElementDst *ndst;
	RuleElementSrv *nsrv;
        RuleElementInterval *nint;
        RuleElementItf *nitf;
        FWOptions      *ruleopt;

/*chain could have been assigned if we split this rule before */
        string this_chain = rule->getStr("ipt_chain");
	string new_chain  = ipt_comp->getNewTmpChainName(rule);
	srvrel->setNeg(false);


/*     A B any D TMP_CHAIN  */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setStr("subrule_suffix","1");
	nsrv=r->getSrv();  nsrv->reset();
        r->setClassification(false);
        r->setRouting(false);
        r->setTagging(false);
	r->setLogging(false);
	r->setStr("ipt_target",new_chain);
        ruleopt =r->getOptionsObject();
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  any  C any RETURN  */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setStr("subrule_suffix","2");

        if (!shadowing_mode)
        {
            nsrc=r->getSrc();   nsrc->reset();
            ndst=r->getDst();   ndst->reset();
            nitf=r->getItf();   nitf->reset();
            if ( (nint=r->getWhen())!=nullptr )  nint->reset();
        }

	r->setAction( PolicyRule::Return );
        r->setClassification(false);
        r->setRouting(false);
        r->setTagging(false);
	r->setLogging(false);
	r->setStr("ipt_chain",new_chain);
	r->setStr("ipt_target","");
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);

        ruleopt =r->getOptionsObject();
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
        ruleopt->setBool("stateless",true);  // ###

//	r->setInterfaceStr("nil");
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  any any any ACTION  */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setStr("subrule_suffix","3");

        nsrv=r->getSrv();   nsrv->reset();

        if (!shadowing_mode)
        {
            nsrc=r->getSrc();   nsrc->reset();
            ndst=r->getDst();   ndst->reset();
            nitf=r->getItf();   nitf->reset();
            if ( (nint=r->getWhen())!=nullptr )  nint->reset();
        }

	r->setStr("ipt_chain",new_chain);
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);

        r->setBool("upstream_rule_neg",true);
	if ( ! rule->getStr("ipt_target").empty() )
	    r->setStr("ipt_target",rule->getStr("ipt_target"));
        
//	r->setInterfaceStr("nil");
        r->setBool("final",true);
        ruleopt =r->getOptionsObject();
        ruleopt->setBool("stateless",true);  // ###
	tmp_queue.push_back(r);

    } else
	tmp_queue.push_back(rule);

    return true;
}


bool PolicyCompiler_ipt::TimeNegation::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    FWOptions  *ruleopt =rule->getOptionsObject();
    string afpa = ruleopt->getStr("firewall_is_part_of_any_and_networks");

    RuleElementInterval *intrel=rule->getWhen();

/*   A  B  C !D  ACTION  */

    if (intrel!=nullptr && intrel->getNeg()) 
    {
	PolicyRule          *r;
	RuleElementSrc      *nsrc;
	RuleElementDst      *ndst;
	RuleElementSrv      *nsrv;
	RuleElementInterval *nint;
        RuleElementItf *nitf;
        FWOptions           *ruleopt;

/*chain could have been assigned if we split this rule before */
        string this_chain = rule->getStr("ipt_chain");
	string new_chain  = ipt_comp->getNewTmpChainName(rule);
	intrel->setNeg(false);

        rule->setBool("upstream_rule_neg",true);

/*     A B C any  TMP_CHAIN  */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setStr("subrule_suffix","1");
	if ( (nint=r->getWhen())!=nullptr )  nint->reset();
        r->setClassification(false);
        r->setRouting(false);
        r->setTagging(false);
	r->setLogging(false);
	r->setStr("ipt_target",new_chain);
        ruleopt =r->getOptionsObject();
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
        ruleopt->setStr("firewall_is_part_of_any_and_networks", afpa);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  any any  D  RETURN  */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setStr("subrule_suffix","2");

        if (!shadowing_mode)
        {
            nsrc=r->getSrc();  nsrc->reset();
            ndst=r->getDst();  ndst->reset();
            nsrv=r->getSrv();  nsrv->reset();
            nitf=r->getItf();  nitf->reset();
        }

	r->setAction( PolicyRule::Return );
        r->setClassification(false);
        r->setRouting(false);
        r->setTagging(false);
	r->setLogging(false);
	r->setStr("ipt_chain",new_chain);
	r->setStr("ipt_target","");
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);

        ruleopt =r->getOptionsObject();
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
        ruleopt->setBool("stateless",true);  // ###
        
//	r->setInterfaceStr("nil");
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  any any any  ACTION  */
	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setStr("subrule_suffix","3");

        if ( (nint=r->getWhen())!=nullptr )  nint->reset();

        if (!shadowing_mode)
        {
            nsrc=r->getSrc();   nsrc->reset();
            ndst=r->getDst();   ndst->reset();
            nsrv=r->getSrv();   nsrv->reset();
            nitf=r->getItf();   nitf->reset();
/*
 * special case: need to preserve information about service protocol in case
 * action_on_reject is TCP RST
 */
            nsrv=r->getSrv();
            Service  *srv= compiler->getFirstSrv(r);
            if (TCPService::isA(srv))
            {
                nsrv->clearChildren();
                nsrv->addRef(compiler->dbcopy->findInIndex(
                                 FWObjectDatabase::getIntId(ANY_TCP_OBJ_ID)));
            }
            else
            {
                nsrv->reset();
            }
        }
	r->setStr("ipt_chain",new_chain);
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);

	if ( ! rule->getStr("ipt_target").empty() )
	    r->setStr("ipt_target",rule->getStr("ipt_target"));

//	r->setInterfaceStr("nil");
        r->setBool("final",true);
        ruleopt =r->getOptionsObject();
        ruleopt->setBool("stateless",true);  // ###
	tmp_queue.push_back(r);

    } else
	tmp_queue.push_back(rule);

    return true;
}








bool PolicyCompiler_ipt::InterfaceAndDirection::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    RuleElementItf *itfre = rule->getItf();   assert(itfre);
    
    if (rule->getDirection()==PolicyRule::Undefined) 
	rule->setDirection( PolicyRule::Both );

    if (itfre->isAny() && rule->getDirection()==PolicyRule::Both) 
    {
        rule->setStr(".iface", "nil");
	//rule->setInterfaceStr("nil");
	return true;
    }

    if (itfre->isAny() && (
	    rule->getDirection()==PolicyRule::Inbound ||
	    rule->getDirection()==PolicyRule::Outbound ))
    {
        itfre->addRef(ipt_comp->regular_interfaces["*"]);
	//rule->setInterfaceStr("*");
	return true;
    }

    return true;
}

bool PolicyCompiler_ipt::setChainPreroutingForTag::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    /*
     * About setting chain for rules with action Tag
     *
     * We tag in chains OUTPUT and PREROUTING. Here is why we need
     * OUTPUT: packets that originate on the firewall should be marked
     * in OUTPUT chain rather than in POSTROUTING because NAT
     * rerouting happens after OUTPUT hook but before POSTROUTING
     * hook. See diagram at
     * http://www.shorewall.net/NetfilterOverview.html
     *
     * Packet that traverse the firewall will be marked in PREROUTING
     * giving us a chance to match them later in other chains.
     *
     * Rule is split by the normal splitIfSrcAny rule processor if src
     * is any or chain is set to OUTPUT if src matches fw. In case
     * rule is split, the second copy won't have chain set when this
     * rule processor is called so it will place it in PREROUTING.
     *
     * This means this processor must be called after splitIfSrcAny but
     * before splitIfDstAny
     *
     * Chain is set by the rule processor setChainForMangle for all
     * rules in the table mangle if direction is set to Inbound or
     * Outbound
     */
           
    /*
     * set chain to PREROUTING if this is (was) a Tag rule, chain has
     * not been assigned yet, direction is Both and there is no
     * interface.
     */
    RuleElementItf *itf_re = rule->getItf(); assert(itf_re!=nullptr);

    if ( (rule->getTagging() || rule->getBool("originated_from_a_rule_with_tagging")) &&
          rule->getStr("ipt_chain").empty() &&
         (rule->getDirection()==PolicyRule::Both ||
          rule->getDirection()==PolicyRule::Inbound) &&
         itf_re->isAny())
    {
        ipt_comp->setChain(rule, "PREROUTING");
    }

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_ipt::setChainPostroutingForTag::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    RuleElementItf *itf_re = rule->getItf(); assert(itf_re!=nullptr);

    if ( (rule->getTagging()  || rule->getBool("originated_from_a_rule_with_tagging")) &&
          rule->getStr("ipt_chain").empty() &&
         (rule->getDirection()==PolicyRule::Both ||
          rule->getDirection()==PolicyRule::Outbound) &&
         itf_re->isAny())
        ipt_comp->setChain(rule, "POSTROUTING");

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::checkForRestoreMarkInOutput::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    FWOptions  *ruleopt = rule->getOptionsObject();

    if ( (rule->getTagging()  || rule->getBool("originated_from_a_rule_with_tagging")) &&
         ruleopt->getBool("ipt_mark_connections") &&
         rule->getStr("ipt_chain")=="OUTPUT")
        ipt_comp->have_connmark_in_output = true;

    tmp_queue.push_back(rule);
    return true;
}
 

bool PolicyCompiler_ipt::setChainForMangle::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (ipt_comp->my_table=="mangle" && rule->getStr("ipt_chain").empty())
    {
        if (rule->getDirection()==PolicyRule::Inbound)
            ipt_comp->setChain(rule, "PREROUTING");

        if (rule->getDirection()==PolicyRule::Outbound)
            ipt_comp->setChain(rule, "POSTROUTING");

/* if direction is "Outbound", chain can never be INPUT, but could be FORWARD */
        RuleElementSrc *srcrel = rule->getSrc();
        Address *src = compiler->getFirstSrc(rule);
        assert(src);

        if ( rule->getDirection()!=PolicyRule::Inbound &&
             !srcrel->isAny() && 
             compiler->complexMatch(src,compiler->fw,true,true))
        {
            ipt_comp->setChain(rule, "OUTPUT");
        }
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::splitIfTagAndConnmark::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    FWOptions *ruleopt = rule->getOptionsObject();

    if (rule->getTagging() && ruleopt->getBool("ipt_mark_connections"))
    {
	tmp_queue.push_back(rule);

	PolicyRule *r1 = compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r1);
	r1->duplicate(rule);
	r1->setStr("ipt_target", "CONNMARK");
        r1->setAction(PolicyRule::Continue);    // ###
        r1->setClassification(false);
        r1->setRouting(false);
        r1->setTagging(false);
        r1->setLogging(false);
        ruleopt = r1->getOptionsObject();
        ruleopt->setStr("CONNMARK_arg", "--save-mark");

	tmp_queue.push_back(r1);

        ipt_comp->have_connmark = true;
    } else
	tmp_queue.push_back(rule);

    return true;
}


bool PolicyCompiler_ipt::splitIfIfaceAndDirectionBoth::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementItf *itfre=rule->getItf();   assert(itfre);

    if ( !itfre->isAny() && rule->getDirection()==PolicyRule::Both)
    {
	PolicyRule *r;

        // If this rule has been assigned to chain POSTROUTING,
        // direction 'inbound' does not make sense for it.
        if (rule->getStr("ipt_chain") != "POSTROUTING")
        {
            r = compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setDirection( PolicyRule::Inbound );
            tmp_queue.push_back(r);
        }

        // If this rule has been assigned to chain PREROUTING,
        // direction 'Outbound' does not make sense for it.
        if (rule->getStr("ipt_chain") != "PREROUTING")
        {
            r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setDirection( PolicyRule::Outbound );
            tmp_queue.push_back(r);
        }
    } else
	tmp_queue.push_back(rule);

    return true;
}

/*
 * Check if ip address of the object passed as argument "addr" matches
 * broadcast address defined by an address/mask of one of the
 * interfaces of the firewall or is a broadcast or multicast address
 * itself, such as 255.255.255.255.
 */
bool PolicyCompiler_ipt::bridgingFw::checkForMatchingBroadcastAndMulticast(
    Address *addr)
{
    // addr can be interface, in which case it does not own ip address
    // and can not match broadcast or multicast
    if (!addr->hasInetAddress()) return false;

    const InetAddr *obj1_addr = addr->getAddressPtr();
    if (!obj1_addr->isAny() &&
        (obj1_addr->isBroadcast() || obj1_addr->isMulticast())
    ) return true;

    FWObjectTypedChildIterator j= compiler->fw->findByType(Interface::TYPENAME);
    for ( ; j!=j.end(); ++j )
    {
        Interface *iface = Interface::cast(*j);
        if ( iface->isRegular() )
        {
            FWObjectTypedChildIterator k = iface->findByType(IPv4::TYPENAME);
            for ( ; k!=k.end(); ++k )
            {
                //const InetAddrMask *ipv4 = Address::cast(*k)->getAddressObjectInetAddrMask();
                Address *addr = Address::cast(*k);
                const InetAddr *ip_netm = addr->getNetmaskPtr();
                const InetAddr *ip_net_addr = addr->getNetworkAddressPtr();
                const InetAddr *ip_bcast_addr = addr->getBroadcastAddressPtr();

/*
 * bug #780345: if interface has netmask 255.255.255.255, its own
 * address will be detected as broadcast. Of course interface address
 * should not be created with netmask 255.255.255.255, but even if it
 * is, we should not interpret its own address as a broadcast, so we
 * should just skip it here. Typical case when this happens is the
 * rule that uses firewall's interface in dst. If we compare an addres
 * found in dst against combination addr/netmask of the same
 * interface, and the netmask is 255.255.255.255, then we get positive
 * match because this routine interprets this address as a broadcast.
 */
                if (ip_netm->isHostMask())
                    continue;
/*
 * commented out to fix bug #637694 - "bridge enbaled / management"
 * Rule where firewall was in destination, and bridging option was on,
 * yielded code in FORWARD chain when this line was uncommented.  

   if ( ipv4->getAddress()==obj1_addr ) return true;

 */
                if (*ip_net_addr == *obj1_addr) return true; 
                if (*ip_bcast_addr == *obj1_addr) return true;
           }
        }
    }
    return false;
}

/*
 * call this after splitIfSrcMatchesFw and splitIfDstMatchesFw so that
 * we can count on firewall or broadcast/multicast being a single
 * object in src and dst.
 */
bool PolicyCompiler_ipt::bridgingFw::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

//    Address        *src=compiler->getFirstSrc(rule);
    Address *dst = compiler->getFirstDst(rule);

    if ( rule->getStr("ipt_chain")=="INPUT" )
    {
        if ( checkForMatchingBroadcastAndMulticast(dst) )
        {

/* bug #1101910: "Samba problem with Bridged Firewall" need to split
 * rule to take care of broadcasts forwarded by the bridge, as well as
 * broadcasts that are accepted by the firewall itself. Need to do
 * this only if the rule is not associated with any bridging
 * interfaces
 */

#ifndef NDEBUG
            RuleElementItf *itfre = rule->getItf();
            assert(itfre);
#endif

            RuleElementItf *itf_re = rule->getItf(); assert(itf_re!=nullptr);
            Interface *rule_iface = 
                Interface::cast(FWObjectReference::getObject(itf_re->front()));

//            Interface *rule_iface = Interface::cast(
//                compiler->dbcopy->findInIndex(rule->getInterfaceId()));

            if (rule_iface!=nullptr && 
                (rule_iface->isUnnumbered() || rule_iface->isBridgePort() ))
            {
                ipt_comp->setChain(rule, "FORWARD");
            } else
            {
                PolicyRule *r= compiler->dbcopy->createPolicyRule();
                compiler->temp_ruleset->add(r);
                r->duplicate(rule);
                ipt_comp->setChain(r, "FORWARD");
                tmp_queue.push_back(r);
            }
        }
    }

    tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::splitIfSrcNegAndFw::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if ( ! rule->getStr("ipt_chain").empty() ) 
    {
	tmp_queue.push_back(rule);
	return true;
    }

    RuleElementSrc *srcrel=rule->getSrc();
    RuleElementSrc *nsrc;
//    Address        *src=compiler->getFirstSrc(rule);

/* if there is negation in SRC, then we need to split the rule based on what
 * first rule generated as the result of processing negation would be:

     any  B C  TMP_CHAIN  

   in this case SRC will become Any even if it is not in the original
   rule. That is, we should split if srcrel is 'any' OR if it has negation.

   To avoid extra complexity in the generated code, this processor does it only
   if src contains more than 1 object and one of these objects is firewall. This
   is the only case when we need to split before processing negation. All other
   "normal" cases are handled by splitIfSrcAny

 */

    list<FWObject*> fwLikes;
    list<FWObject*> notFwLikes;

    if (rule->getDirection()!=PolicyRule::Inbound && srcrel->getNeg())
    {
        for (list<FWObject*>::iterator i1=srcrel->begin(); i1!=srcrel->end(); ++i1) 
        {
            FWObject *o   = *i1;
            if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

            Address *a = Address::cast(o);
            if (a && compiler->complexMatch(a,compiler->fw))
                fwLikes.push_back(o);
            else
                notFwLikes.push_back(o);
        }

        if (fwLikes.size() != 0)
        {
            PolicyRule *r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            ipt_comp->setChain(r,"OUTPUT");
            r->setDirection( PolicyRule::Outbound );
            nsrc=r->getSrc();
            nsrc->clearChildren();
            for (list<FWObject*>::iterator m=fwLikes.begin(); m!=fwLikes.end(); ++m) 
                nsrc->addRef(*m);
            tmp_queue.push_back(r);

//            rule->setStr("ipt_chain","FORWARD");
            nsrc=rule->getSrc();
            nsrc->reset();   // resets negation flag
            for (list<FWObject*>::iterator m=notFwLikes.begin(); m!=notFwLikes.end(); ++m) 
                nsrc->addRef(*m);
            if (!nsrc->isAny()) nsrc->setNeg(true);
            FWOptions  *ruleopt = rule->getOptionsObject();
            // so we do not put this rule in OUTPUT chain later
            ruleopt->setBool("no_output_chain",true);
            tmp_queue.push_back(rule);
            return true;
        }
    }
    tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::splitIfDstNegAndFw::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if ( ! rule->getStr("ipt_chain").empty() ) 
    {
	tmp_queue.push_back(rule);
	return true;
    }

    RuleElementDst *dstrel=rule->getDst();
    RuleElementDst *ndst;
//    Address        *dst=compiler->getFirstDst(rule);

/* if there is negation in DST, then we need to split the rule based on what
 * first rule generated as the result of processing negation would be:

     A  any C  TMP_CHAIN  

   in this case DST will become Any even if it is not in the original
   rule. That is, we should split if dstrel is 'any' OR if it has negation.

   To avoid extra complexity in the generated code, this processor does it only
   if dst contains more than 1 object and one of these objects is firewall. This
   is the only case when we need to split before processing negation. All other
   "normal" cases are handled by splitIfDstAny

 */

    list<FWObject*> fwLikes;
    list<FWObject*> notFwLikes;

    if (rule->getDirection()!=PolicyRule::Outbound && dstrel->getNeg())
    {
        for (list<FWObject*>::iterator i1=dstrel->begin(); i1!=dstrel->end(); ++i1) 
        {
            FWObject *o   = *i1;
            if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

            Address *a = Address::cast(o);
            if (a && compiler->complexMatch(a,compiler->fw))
                fwLikes.push_back(o);
            else
                notFwLikes.push_back(o);
        }

        if (fwLikes.size() != 0)
        {
            PolicyRule *r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            ipt_comp->setChain(r,"INPUT");
            r->setDirection( PolicyRule::Inbound );
            ndst=r->getDst();
            ndst->clearChildren();
            for (list<FWObject*>::iterator m=fwLikes.begin(); m!=fwLikes.end(); ++m) 
                ndst->addRef(*m);
            tmp_queue.push_back(r);

            // the second rule goes into FORWARD chain, but if source
            // is (or contains) firewall, we may also need OUTPUT chain

//            rule->setStr("ipt_chain","FORWARD");
            ndst=rule->getDst();
            ndst->reset();   // resets negation flag
            for (list<FWObject*>::iterator m=notFwLikes.begin(); m!=notFwLikes.end(); ++m) 
                ndst->addRef(*m);
            if (!ndst->isAny()) ndst->setNeg(true);
            FWOptions  *ruleopt = rule->getOptionsObject();
            // so we do not put this rule in INPUT chain later
            ruleopt->setBool("no_input_chain",true);
            tmp_queue.push_back(rule);
            return true;
        }
    }
    tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::splitIfSrcAny::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    FWOptions  *ruleopt = rule->getOptionsObject();
    FWOptions *fwopt = compiler->getCachedFwOpt();

    /* commented to fix bug #1112470
     * if fw is considered part of any, we should place rule in INPUT/OUTPUT
     * chains even if it is a bridging fw since fw itself may send or receive
     * packets
     */

    if ( /* fwopt->getBool("bridging_fw") || */
         ruleopt->getInt("firewall_is_part_of_any_and_networks")==0 ||
         ruleopt->getBool("no_output_chain")) 
    {
	tmp_queue.push_back(rule);
	return true;
    }

    /* See #2008. It appears "--physdev-out" is not allowed in OUTPUT
     * chain.
     */
#ifndef NDEBUG
    RuleElementItf *itfre = rule->getItf(); assert(itfre);
#endif
    Interface *itf = compiler->getFirstItf(rule);

    if (fwopt->getBool("bridging_fw") && itf && itf->isBridgePort())
    {
	tmp_queue.push_back(rule);
	return true;
    }

    if ( ! rule->getStr("ipt_chain").empty() ) 
    {
	tmp_queue.push_back(rule);
	return true;
    }

    RuleElementSrc *srcrel = rule->getSrc();
    Address *src = compiler->getFirstSrc(rule); 

    if ( rule->getDirection()!=PolicyRule::Inbound &&
         ( 
             srcrel->isAny() ||
             ( srcrel->size()==1 && src!=nullptr &&
               !compiler->complexMatch(src,compiler->fw) &&
               srcrel->getBool("single_object_negation")) 
         ) 
    )
    {
        PolicyRule *r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        ipt_comp->setChain(r,"OUTPUT");
        r->setDirection( PolicyRule::Outbound );
        tmp_queue.push_back(r);

        /*
         * A note about CLASSIFY target in iptables:
         *
         * CLASSIFY only works in mangle table in POSTROUTING chain.
         * the man page does not mention this, but module
         * documentation in p-o-m says so.
         */

        if (ipt_comp->my_table=="mangle" && rule->getClassification())
        {
            r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            ipt_comp->setChain(r,"POSTROUTING");
            r->setDirection( PolicyRule::Outbound );
            tmp_queue.push_back(r);
        }

    }
    tmp_queue.push_back(rule);  // add old rule anyway

    return true;
}

bool PolicyCompiler_ipt::splitIfDstAny::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

//    FWOptions *fwopt = compiler->getCachedFwOpt();
    FWOptions  *ruleopt = rule->getOptionsObject();
/* commented to fix bug #1112470
 * if fw is considered part of any, we should place rule in INPUT/OUTPUT
 * chains even if it is a bridging fw since fw itself may send or receive
 * packets
 */
    if ( /* fwopt->getBool("bridging_fw") || */
         ruleopt->getInt("firewall_is_part_of_any_and_networks")==0 ||
         ruleopt->getBool("no_input_chain")) 
    {
	tmp_queue.push_back(rule);
	return true;
    }

    if ( ! rule->getStr("ipt_chain").empty() ) 
    {
	tmp_queue.push_back(rule);
	return true;
    }

    RuleElementDst *dstrel = rule->getDst();
    Address *dst = compiler->getFirstDst(rule); 

    if ( rule->getDirection()!=PolicyRule::Outbound &&
         ( 
             dstrel->isAny() ||

             ( dstrel->size()==1 && dst!=nullptr &&
               !compiler->complexMatch(dst,compiler->fw) &&
               dstrel->getBool("single_object_negation")) 
         ) 
    )
    {
	PolicyRule *r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	ipt_comp->setChain(r,"INPUT");
	r->setDirection( PolicyRule::Inbound );
	tmp_queue.push_back(r);

        // if this rule is for mangle table, need to put it into
        // POSTROUTING chain as well because some targets that
        // work with mangle table can only go into POSTROUTING chain
        // such as CLASSIFY
        if (ipt_comp->my_table=="mangle" && rule->getClassification())
        {
            r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            ipt_comp->setChain(r,"PREROUTING");
            r->setDirection( PolicyRule::Inbound );
            tmp_queue.push_back(r);
        }

    }
    tmp_queue.push_back(rule); // add old rule in any case

    return true;
}

/**
 * If rule element RE (Src or Dst) has an AddressRange object that
 * represents single address, replace it with corresponding IPv4
 * object. Call this rule processor before splitIfSrcMatchingAddressRange
 *
 * #2650
 */
bool PolicyCompiler_ipt::specialCaseAddressRangeInRE::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    list<FWObject*> new_children;

    RuleElement *rel = RuleElement::cast( rule->getFirstByType(re_type) );

    for (list<FWObject*>::iterator i1=rel->begin(); i1!=rel->end(); ++i1) 
    {
        Address *addr_obj = Address::cast(FWReference::getObject(*i1));
        if (addr_obj == nullptr) continue;
/*
 * commented out for SF bug 3468358
 * Why did I need to replace cluster interface with member interface if
 * addresses of interfaces can not be AddressRange objects ?
 *        Address *addr_obj = compiler->correctForCluster(Address::cast(obj));
 */

        if (addr_obj && !addr_obj->isAny() && AddressRange::isA(addr_obj) &&
            addr_obj->dimension() == 1)
        {
            bool IPv4 = AddressRange::cast(addr_obj)->isV4();
            int address_family = IPv4 ? AF_INET : AF_INET6;

            Address *new_addr = IPv4
                    ? static_cast<Address*>(compiler->dbcopy->createIPv4())
                    : static_cast<Address*>(compiler->dbcopy->createIPv6());

            new_addr->setName(addr_obj->getName() + "_addr");
            new_addr->setAddress(AddressRange::cast(addr_obj)->getRangeStart());
            new_addr->setNetmask(InetAddr(InetAddr::getAllOnes(address_family)));
            compiler->persistent_objects->add(new_addr);
            new_children.push_back(new_addr);
        } else
            new_children.push_back(addr_obj);
    }

    if (new_children.size() > 0)
    {
        rel->clearChildren();
        for (list<FWObject*>::iterator i1=new_children.begin(); i1!=new_children.end(); ++i1) 
        {
            rel->addRef(*i1);
        }
    }

    tmp_queue.push_back(rule); // add old rule in any case

    return true;
}

/**
 * Split rule if src has addressRange object that matches the
 * firewall.  If some addresses inside the range match the firewall,
 * while others dont, the rule must be placed in both OUTPUT and
 * FORWARD chains.
 *
 * #2650
 */
bool PolicyCompiler_ipt::splitIfSrcMatchingAddressRange::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    Address *src = compiler->correctForCluster(compiler->getFirstSrc(rule));

    bool b, m;
    b=m= !( compiler->getCachedFwOpt()->getBool("bridging_fw") );

    /*
     * directions outbound or both: if src is an address range that
     * matches fw, we should split the rule to make sure we match both
     * in OUTPUT and FORWARD
     */
    if ( rule->getDirection() != PolicyRule::Inbound && 
         src && !src->isAny() && AddressRange::isA(src) &&
         ipt_comp->complexMatch(src, ipt_comp->fw, b, m))
    {
        PolicyRule *r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        ipt_comp->setChain(r, "OUTPUT");
        r->setDirection( PolicyRule::Outbound );
        tmp_queue.push_back(r);
    }

    tmp_queue.push_back(rule);

    return true;
}


/**
 * Split rule if dst has addressRange object that matches the
 * firewall.  If some addresses inside the range match the firewall,
 * while others dont, the rule must be placed in both INPUT and
 * FORWARD chains.
 *
 * #2650
 */
bool PolicyCompiler_ipt::splitIfDstMatchingAddressRange::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    Address *dst = compiler->correctForCluster(compiler->getFirstDst(rule));

    bool b, m;
    b=m= !( compiler->getCachedFwOpt()->getBool("bridging_fw") );

    /*
     * directions inbound or both: if src is an address range that
     * matches fw, we should split the rule to make sure we match both
     * in INPUT and FORWARD
     */
    if ( rule->getDirection() != PolicyRule::Outbound && 
         dst && !dst->isAny() && AddressRange::isA(dst) &&
         ipt_comp->complexMatch(dst, ipt_comp->fw, b, m))
    {
        PolicyRule *r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        ipt_comp->setChain(r, "INPUT");
        r->setDirection( PolicyRule::Inbound );
        tmp_queue.push_back(r);
    }

    tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::splitIfSrcAnyForShadowing::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getClassification())
    {
	tmp_queue.push_back(rule);
	return true;
    }

    RuleElementSrc *srcrel=rule->getSrc();
    FWOptions  *ruleopt = rule->getOptionsObject();

    if ( ruleopt->getInt("firewall_is_part_of_any_and_networks")==1 &&
         !ruleopt->getBool("no_output_chain") &&
         rule->getDirection()!=PolicyRule::Inbound && 
         srcrel->isAny() )
    {
        PolicyRule *r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        ipt_comp->setChain(r,"OUTPUT");
        r->setDirection( PolicyRule::Outbound );
        RuleElementSrc *nsrcrel=r->getSrc();
        nsrcrel->addRef(compiler->fw);
        tmp_queue.push_back(r);
    }
    tmp_queue.push_back(rule);  // add old rule anyway

    return true;
}

bool PolicyCompiler_ipt::splitIfDstAnyForShadowing::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getClassification())
    {
	tmp_queue.push_back(rule);
	return true;
    }

    RuleElementDst *dstrel=rule->getDst();
    FWOptions  *ruleopt = rule->getOptionsObject();

    if ( ruleopt->getInt("firewall_is_part_of_any_and_networks")==1 &&
         !ruleopt->getBool("no_input_chain") &&
         rule->getDirection()!=PolicyRule::Outbound && 
         dstrel->isAny() )
    {
        PolicyRule *r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        ipt_comp->setChain(r,"INPUT");
        r->setDirection( PolicyRule::Inbound );
        RuleElementDst *ndstrel=r->getDst();
        ndstrel->addRef(compiler->fw);
        tmp_queue.push_back(r);
    }
    tmp_queue.push_back(rule);  // add old rule anyway

    return true;
}


bool PolicyCompiler_ipt::splitIfSrcFWNetwork::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getClassification())
    {
	tmp_queue.push_back(rule);
	return true;
    }

    RuleElementSrc *srcrel=rule->getSrc();

    FWOptions *fwopt = compiler->getCachedFwOpt();
    FWOptions  *ruleopt = rule->getOptionsObject();
    if ( fwopt->getBool("bridging_fw") ||
         ruleopt->getInt("firewall_is_part_of_any_and_networks")==0 ||
         ruleopt->getBool("no_output_chain"))
    {
	tmp_queue.push_back(rule);
	return true;
    }

    if ( ! rule->getStr("ipt_chain").empty() || srcrel->isAny() ) 
    {
	tmp_queue.push_back(rule);
	return true;
    }

    if (rule->getDirection()!=PolicyRule::Inbound)
    {
        std::map<FWObject*,FWObject*> obj_subst;

        for (list<FWObject*>::iterator i1=srcrel->begin(); i1!=srcrel->end(); ++i1) 
        {
            FWObject *o   = *i1;
            if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
            Address *a = Address::cast(o);
            FWObject *na;
            if ((Network::isA(o) || NetworkIPv6::isA(o)) &&
                (na=compiler->findAddressFor(a, compiler->fw ))!=nullptr)
            {
                obj_subst[a]=na;
            }
        }

        if ( ! obj_subst.empty() )
        {
            PolicyRule *r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            ipt_comp->setChain(r,"OUTPUT");
            r->setDirection( PolicyRule::Outbound );

#if 0
/* I can't decide right now if I should replace network objects with firewall's addresses.
   I am going not to replace them for now */
            RuleElementSrc *nsrcrel=r->getSrc();
            for (std::map<FWObject*,FWObject*>::iterator i=obj_subst.begin(); i!=obj_subst.end(); i++)
            {
                nsrcrel->removeRef( i->first );
                nsrcrel->addRef( i->second );
            }
#endif
            tmp_queue.push_back(r);
        }
    }
    tmp_queue.push_back(rule);

    return true;
}


bool PolicyCompiler_ipt::splitIfDstFWNetwork::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getClassification())
    {
	tmp_queue.push_back(rule);
	return true;
    }

    RuleElementDst *dstrel=rule->getDst();

    FWOptions *fwopt = compiler->getCachedFwOpt();
    FWOptions  *ruleopt = rule->getOptionsObject();
    if ( fwopt->getBool("bridging_fw") ||
         ruleopt->getInt("firewall_is_part_of_any_and_networks")==0 ||
         ruleopt->getBool("no_input_chain"))
    {
	tmp_queue.push_back(rule);
	return true;
    }

    if ( ! rule->getStr("ipt_chain").empty() || dstrel->isAny() ) 
    {
	tmp_queue.push_back(rule);
	return true;
    }

    if (rule->getDirection()!=PolicyRule::Outbound)
    {
        std::map<FWObject*,FWObject*> obj_subst;

        for (list<FWObject*>::iterator i1=dstrel->begin(); i1!=dstrel->end(); ++i1) 
        {
            FWObject *o   = *i1;
            if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
            Address *a = Address::cast(o);
            FWObject *na;
            if ((Network::isA(a) || NetworkIPv6::isA(a)) &&
                (na=compiler->findAddressFor(a, compiler->fw))!=nullptr)
            {
                obj_subst[a]=na;
            }
        }

        if ( ! obj_subst.empty() )
        {
            PolicyRule *r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            ipt_comp->setChain(r,"INPUT");
            r->setDirection( PolicyRule::Inbound );

#if 0
/* I can't decide right now if I should replace network objects with firewall's addresses.
   I am going not to replace them for now */
            RuleElementDst *ndstrel=r->getDst();
            for (std::map<FWObject*,FWObject*>::iterator i=obj_subst.begin(); i!=obj_subst.end(); i++)
            {
                ndstrel->removeRef( i->first );
                ndstrel->addRef( i->second );
            }
#endif
            tmp_queue.push_back(r);
        }
    }
    tmp_queue.push_back(rule);

    return true;
}


/*
 *  predicates that run before guarantee that when we call this one,
 *  firewall object, if it is in src or dst, is single object there
 */
bool PolicyCompiler_ipt::checkSrcAndDst1::processNext()
{
    PolicyRule *rule = getNext();
    if (rule==nullptr) return false;
    Address *src = compiler->getFirstSrc(rule);  assert(src);
    Address *dst = compiler->getFirstDst(rule);  assert(dst);

    if (!compiler->isFirewallOrCluster(src) &&
        compiler->isFirewallOrCluster(dst) &&
	rule->getDirection()==PolicyRule::Outbound )
    {
	compiler->abort(
            rule, 
            "direction can not be outbound when destination is firewall");
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::checkSrcAndDst2::processNext()
{
    PolicyRule *rule = getNext();
    if (rule==nullptr) return false;
    Address *src = compiler->getFirstSrc(rule);  assert(src);
    Address *dst = compiler->getFirstDst(rule);  assert(dst);

    if (compiler->isFirewallOrCluster(src) &&
        !compiler->isFirewallOrCluster(dst) &&
	rule->getDirection()==PolicyRule::Inbound )
    {
	compiler->abort(
            rule, 
            "direction can not be inbound when source is firewall");
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::specialCaseWithFW1::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getClassification())
    {
	tmp_queue.push_back(rule);
	return true;
    }

//    RuleElementSrc *srcrel=rule->getSrc();
    Address        *src = compiler->getFirstSrc(rule);  
    if(src==nullptr) compiler->abort(rule, "Broken SRC ");
//    RuleElementDst *dstrel=rule->getDst();
    Address        *dst = compiler->getFirstDst(rule);
    if(dst==nullptr) compiler->abort(rule, "Broken DST");

    if (!src->isAny() && !dst->isAny() &&
        compiler->complexMatch(src,compiler->fw) &&
        compiler->complexMatch(dst,compiler->fw) &&
	rule->getDirection()== PolicyRule::Both ) 
    {
	PolicyRule *r;

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setDirection( PolicyRule::Inbound );
	tmp_queue.push_back(r);

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setDirection( PolicyRule::Outbound );
	tmp_queue.push_back(r);
    } else
	tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::specialCaseWithFWInDstAndOutbound::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    Interface *itf = compiler->getFirstItf(rule);
    // note: itf can be nullptr if object in this rule element is a group
    RuleElementSrc *srcrel = rule->getSrc();
    Address *src =compiler->getFirstSrc(rule);
    Address *dst =compiler->getFirstDst(rule);
    string chain  = rule->getStr("ipt_chain");

    if (rule->getDirection()== PolicyRule::Outbound &&
        itf!=nullptr && itf->isChildOf(compiler->fw) &&
        chain!="OUTPUT")
    {
        // If dst is broadcast, drop the rule if this is not bridging
        // fw.  Bridging fw can forward broadcasts, so a rule with
        // this address in destination, direction Outbound and
        // non-empty interface is legit
        FWOptions *fwopt = compiler->getCachedFwOpt();
        const InetAddr *dst_addr = dst->getAddressPtr();
        if (dst_addr &&
            (dst_addr->isBroadcast() || dst_addr->isMulticast()) &&
            fwopt->getBool("bridging_fw"))
        {
            tmp_queue.push_back(rule);
            return true;
        }

        // if src does not match fw and dst matches firewall, skip the
        // rule.  The idea is that if src does not match fw, such
        // packet can only be forwarded by the firewall. If dst
        // matches firewall, it would go into INPUT chain and would
        // not be forwarded, so it can't cross interface in outbound
        // direction and the rule would never see this packet. If src
        // matches the firewall, the packet might be generated by it
        // and can cross interface in outbound direction, so the rule
        // must be preserved.
        // Also, if src is negated, the rule must be preserved too.
        if (srcrel->getNeg() || srcrel->getBool("single_object_negation"))
        {
            tmp_queue.push_back(rule);
            return true;
        }

        FWOptions *ruleopt = rule->getOptionsObject();
        bool rule_afpa = ruleopt->getBool("firewall_is_part_of_any_and_networks");

        bool src_matches = compiler->complexMatch(src, compiler->fw);
        bool dst_matches = compiler->complexMatch(dst, compiler->fw);

        // if "assume fw is part of any and networks" is turned off,
        // do not consider network objects matching. Except when such
        // network has netmask 255.255.255.255 and defines just a
        // single address

        if ((src->isAny() || Network::isA(src) || NetworkIPv6::isA(src)) &&
            !rule_afpa && ! src->getNetmaskPtr()->isHostMask()) src_matches = false;

        if ((dst->isAny() || Network::isA(dst) || NetworkIPv6::isA(dst)) &&
            !rule_afpa && ! dst->getNetmaskPtr()->isHostMask()) dst_matches = false;
        
        // there is still one case that this rule processor catches
        // and drop the rule, but I am not sure if it is right thing
        // to do. This is when src=some address on the subnet fw
        // intrface is on, but not the address of the firewall,
        // dst=broadcast or multicast, "assume fw is part of any" is
        // turned on, the firewall is not a bridge. A rule like this
        // passes all checks above and gets dropped by this rule
        // processor. It is hard ot say what should we really do in
        // this case.
        
        if (!src_matches && dst_matches)
        {
            // src does not match, dst matches: skipping the rule
            return true;
        }


        tmp_queue.push_back(rule);
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}


/*
 * this is basically the same as ExpandMultipleAddresses except it
 * does not skip loopback
 */
bool PolicyCompiler_ipt::specialCaseWithFW2::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrc *srcrel=rule->getSrc();
    Address        *src   =compiler->getFirstSrc(rule);  
    RuleElementDst *dstrel=rule->getDst();
    Address        *dst   =compiler->getFirstDst(rule);

    if (src->getId()==compiler->fw->getId() && dst->getId()==compiler->fw->getId() )
    {
	srcrel->reset();
	dstrel->reset();

        list<FWObject*> all_addresses;

        FWObjectTypedChildIterator i=compiler->fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface=Interface::cast(*i);
            if ( iface->isUnnumbered() || iface->isBridgePort() ) continue;

            FWObjectTypedChildIterator j=iface->findByType(IPv4::TYPENAME);
            for ( ; j!=j.end(); ++j ) all_addresses.push_back( *j);
        }
        for (list<FWObject*>::iterator i=all_addresses.begin(); i!=all_addresses.end(); ++i)
        {
            srcrel->addRef(*i);
            dstrel->addRef(*i);
        }
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::specialCaseWithUnnumberedInterface::dropUnnumberedInterface(RuleElement *re)
{
    if (re->isAny()) return true;
    list<FWObject*> cl;
    for (list<FWObject*>::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs   =Interface::cast( obj );

        if (ifs!=nullptr && 
            (ifs->isUnnumbered() || ifs->isBridgePort())) cl.push_back(obj);
    }

    if (!cl.empty())
    {
        for (list<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
            re->removeRef( (*i1) );
    }
    return (!re->isAny() );
}


/**
 * checks for the following situations: 
 *
 *  1. unnumbered interface is in source and direction is inbound
 *  (drop interface from src since source address is
 *  undertermined)
 *
 *  2. unnumbered interface is in source, direction is outbound
 *  and chain is  temporary (drop interface from the list, this
 *  rule has been created while processing negation. TODO: this
 *  is kludge, need to create separate temporary chain while
 *  doing negation in src if one of the objects is firewall)
 *
 *  3. unnumbered interface is in destination and chain is "OUTPUT"
 *  (drop interface since dest. address is undefined)
 * 
 *
 */
bool PolicyCompiler_ipt::specialCaseWithUnnumberedInterface::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    bool keep_rule=true;
    switch ( rule->getDirection() ) 
    {
    case PolicyRule::Inbound:
        keep_rule=dropUnnumberedInterface( rule->getSrc() );
        break;
    case PolicyRule::Outbound:
        if ( rule->getStr("ipt_chain")=="OUTPUT" ) 
            keep_rule=dropUnnumberedInterface( rule->getDst() );
        else
            keep_rule=dropUnnumberedInterface( rule->getSrc() );
        break;
    default: ;
    }

    if (keep_rule) tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::checkForDynamicInterfacesOfOtherObjects::findDynamicInterfaces(
    RuleElement *re, Rule *rule)
{
    if (re->isAny()) return true;
    list<FWObject*> cl;
    for (list<FWObject*>::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs = Interface::cast( obj );

        if (ifs != nullptr && ifs->isDyn())
        {
            if ( ! ifs->isChildOf(compiler->fw))        
            {
                // If this is dynamic failover interface, look at
                // corresponding member interface. If we can find one,
                // it is ok. Otherwise it is probably failover
                // interface of a cluster this firewall is not a
                // member of.
                if (ifs->isFailoverInterface())
                {
                    FailoverClusterGroup *fg = FailoverClusterGroup::cast(
                        ifs->getFirstByType(FailoverClusterGroup::TYPENAME));
                    if (fg &&
                        fg->getInterfaceForMemberFirewall(compiler->fw)!=nullptr)
                        continue;
                }
                QString err(
                    "Can not build rule using dynamic interface '%1' "
                    "of the object '%2' because its address in unknown.");
                compiler->abort(
                    rule,
                    err
                    .arg(ifs->getName().c_str())
                    .arg(ifs->getParent()->getName().c_str()).toStdString());
                return false;
            }
        }
    }
    return true;
}


bool PolicyCompiler_ipt::checkForDynamicInterfacesOfOtherObjects::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (findDynamicInterfaces( rule->getSrc() , rule ) &&
        findDynamicInterfaces( rule->getDst() , rule ))
        tmp_queue.push_back(rule);

    return true;
}

/*
 * remember, behavior of this processor has been changed in virtual
 * method _expand_interface
 */
bool PolicyCompiler_ipt::expandMultipleAddressesIfNotFWinSrc::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElementSrc *srcrel = rule->getSrc();
    Address *src =compiler->getFirstSrc(rule);
    assert(src);
    if (Firewall::cast(src)==nullptr)  compiler->_expand_addr(rule, srcrel, true);
    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::expandMultipleAddressesIfNotFWinDst::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElementDst *dstrel=rule->getDst();
    Address        *dst   =compiler->getFirstDst(rule);  assert(dst);
    if (Firewall::cast(dst)==nullptr)  compiler->_expand_addr(rule, dstrel, true);
    tmp_queue.push_back(rule);
    return true;
}

void PolicyCompiler_ipt::expandLoopbackInterfaceAddress::replaceLoopbackWithItsAddress(
    RuleElement *rel, Rule *rule)
{
    if (rel->isAny()) return;
    list<FWObject*> cl;
    for (list<FWObject*>::iterator i1=rel->begin(); i1!=rel->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();

        if (Interface::cast(obj)!=nullptr && Interface::cast(obj)->isLoopback())
        {
            FWObject *addr = obj->getFirstByType(IPv4::TYPENAME);
            if (addr==nullptr)
                compiler->abort(
                    rule, 
                    "Loopback interface of the firewall object does not "
                    "have IP address but is used in the rule");
            rel->removeRef(obj);
            rel->addRef(addr);
            break;  // I guess there can be only one loopback object in the rule, right ? 
        }
    }
}

bool PolicyCompiler_ipt::expandLoopbackInterfaceAddress::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrc *srcrel=rule->getSrc();
    RuleElementDst *dstrel=rule->getDst();

    replaceLoopbackWithItsAddress(srcrel,rule);
    replaceLoopbackWithItsAddress(dstrel,rule);

    tmp_queue.push_back(rule);

    return true;
}

/*
 *  This processor sets chain only if it is INPUT or OUTPUT. We will
 *  look at the rule if it goes into FORWARD chain in
 *  splitIfSrcFWNetwork / splitIfDstFWNetwork and possibly split it. We will
 *  set chain to FORWARD after that in decideOnChain
 */
bool PolicyCompiler_ipt::decideOnChainIfSrcFW::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if ( ! rule->getStr("ipt_chain").empty() || rule->getClassification())
    {
        tmp_queue.push_back(rule);
        return true;
    }

#ifdef DEBUG_FOR_DMZ
    cerr << endl;
    cerr << "PolicyCompiler_ipt::decideOnChainIfSrcFW "
         << " rule " << rule->getLabel()
         << endl;
#endif

    Address *src = compiler->correctForCluster(compiler->getFirstSrc(rule));
    assert(src);

/* Bug 811860: "IPTables Compiler Firewall IP to Input Chain".
 * on a bridging firewall rules not associated with interfaces should
 * go into INPUT/OUTPUT chain on interfaces that do routing and into
 * FORWARD chain on bridging interfaces. Sometimes bridging interfaces
 * are not created in the GUI, so to play it safe we will split the
 * rule and put it into both FORWARD and INPUT/OUTPUT chain.
 *
 * Bug #934949: "duplicate rules". Split the rule only if firewall is
 * in src or dst. Otherwise compiler produces duplicates.
 *
 */
    if ( compiler->getCachedFwOpt()->getBool("bridging_fw") &&
         compiler->complexMatch(src, compiler->fw, false, false) )
    {
        /* Correction for bug #1231 : as of fwbuilder v4.0 (and
         * really, probably as of 3.0), bridge ports must be created
         * in the GUI for the compiler to process policy of the
         * bridging firewall correctly.  Will split the rule if it is
         * not associated with any particular interface or associated
         * with an interface which is a bridge port.
         */

        Interface *rule_iface = compiler->getFirstItf(rule);
        if (rule_iface == nullptr || rule_iface->isBridgePort())
        {
            PolicyRule *r = compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            ipt_comp->setChain(r,"FORWARD");
            tmp_queue.push_back(r);
        }
    }

    bool b,m;
/* 
 * do not check for broadcasts and multicasts in bridging firewall because
 * they should go to FORWARD chain 
 *
 *    b=m= !( compiler->getCachedFwOpt()->getBool("bridging_fw") );
 *
 *
 *  Commented out the line above while working on the bug #811860:
 *  "IPTables Compiler Firewall IP to Input Chain".  No need to do it
 *  anymore since we now split the rule if we work with a bridging fw
 *  and the rule _always_ goes into FORWARD chain --vk 09/28/03
 */

    b=m=true;

    switch ( rule->getDirection() ) 
    {
    case PolicyRule::Outbound:
/* if direction is "Outbound", chain can never be INPUT, but could be FORWARD */
        if (! src->isAny() &&
            ! AddressRange::isA(src) &&   // #2650
            compiler->complexMatch(src, compiler->fw, b, m))
            ipt_comp->setChain(rule,"OUTPUT");
        break;

    case PolicyRule::Both:
/* direction == Both
 */
        if (! src->isAny() &&
            ! AddressRange::isA(src) &&   // #2650
            compiler->complexMatch(src, compiler->fw, b, m))
        {
            ipt_comp->setChain(rule,"OUTPUT");
            rule->setDirection( PolicyRule::Outbound );
        }
        break;
    default: break;
    }
    tmp_queue.push_back(rule);

    return true;
}

/*
 * Call this processor before InterfacePolicyRulesWithOptimization
 */
bool PolicyCompiler_ipt::decideOnChainIfDstFW::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if ( ! rule->getStr("ipt_chain").empty() || rule->getClassification())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    Address *dst = compiler->correctForCluster(compiler->getFirstDst(rule));
    assert(dst);

/*
 * Note: dst can be cluster object but not necessarily the same cluster
 * that is being compiled. Check if compiler->fw is a member of this cluster.
 */
    list<Firewall*> cluster_members;
    if (Cluster::isA(dst))
        Cluster::cast(dst)->getMembersList(cluster_members);


/* Bug 811860: "IPTables Compiler Firewall IP to Input Chain".
 * on a bridging firewall rules not associated with interfaces should
 * go into INPUT/OUTPUT chain on interfaces that do routing and into
 * FORWARD chain on bridging interfaces. Sometimes bridging interfaces
 * are not created in the GUI, so to play it safe we will split the
 * rule and put it into both FORWARD and INPUT/OUTPUT chain.
 *
 * Bug #934949: "duplicate rules". Split the rule only if firewall is
 * in src or dst. Otherwise compiler produces duplicates.
 */
    if ( compiler->getCachedFwOpt()->getBool("bridging_fw") &&
         compiler->complexMatch(dst,compiler->fw,false,false) )
    {
        /* Correction for bug #1231 : as of fwbuilder v4.0 (and
         * really, probably as of 3.0), bridge ports must be created
         * in the GUI for the compiler to process policy of the
         * bridging firewall correctly.  Will split the rule if it is
         * not associated with any particular interface or associated
         * with an interface which is a bridge port.
         */

        Interface *rule_iface = compiler->getFirstItf(rule);
        if (rule_iface == nullptr || rule_iface->isBridgePort())
        {
            PolicyRule *r = compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            ipt_comp->setChain(r,"FORWARD");
            tmp_queue.push_back(r);
        }
    }

    bool b,m;
/* 
 * do not check for broadcasts and multicasts in bridging firewall because
 * they should go to FORWARD chain 
 *
 *    b=m= !( compiler->getCachedFwOpt()->getBool("bridging_fw") );
 *
 *
 *  Commented out the line above while working on the bug #811860:
 *  "IPTables Compiler Firewall IP to Input Chain".  No need to do it
 *  anymore since we now split the rule if we work with a bridging fw
 *  and the rule _always_ goes into FORWARD chain --vk 09/28/03
 */

    b=m=true;

    switch ( rule->getDirection() ) 
    {
    case PolicyRule::Inbound:
/* if direction is "Inbound", chain can never be OUTPUT, but could be FORWARD */
        if (! dst->isAny() &&
            ! AddressRange::isA(dst) &&   // #2650
            (compiler->complexMatch(dst,compiler->fw,b,m) ||
             std::find(cluster_members.begin(),
                       cluster_members.end(),
                       compiler->fw) != cluster_members.end())
        )
            ipt_comp->setChain(rule,"INPUT");
        break;

    case PolicyRule::Both:
/* direction == Both
 */
        if (! dst->isAny() && 
            ! AddressRange::isA(dst) &&   // #2650
            (compiler->complexMatch(dst,compiler->fw,b,m) ||
             std::find(cluster_members.begin(),
                       cluster_members.end(),
                       compiler->fw) != cluster_members.end())
        )
        {
            ipt_comp->setChain(rule,"INPUT");
            rule->setDirection(PolicyRule::Inbound);
        }
        break;
    default: break;
    }
    tmp_queue.push_back(rule);

    return true;
}




/*
 * processor splitIfIfaceAndDirectionBoth splits interface rule if its
 * direction is "Both". This means that by the time when this
 * processor is called, original rule "any any any accept both" on the
 * loopback interface has already been converted to two rules :
 *
 * any any any accept inbound
 * any any any accept outbound
 * 
 * We do not have to split rule here, but rather just assign it to
 * INPUT/OUTPUT chains.
 *
 * We now call this rule processor after InterfacePolicyRulesWithOptimization
 * which means there is no more than one object in rule element 'Interface'
 */ 
bool PolicyCompiler_ipt::decideOnChainIfLoopback::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementItf *itfre=rule->getItf();
    assert(itfre);
    assert(itfre->size()<=1);

    // sometimes for whatever reason itf rule element appears to be broken
    // (it is just empty, not even "any")
    if (itfre->size()==0)
    {
        tmp_queue.push_back(rule);
        return true;
    }

    Interface *rule_iface = compiler->getFirstItf(rule);

    RuleElementSrc *srcrel=rule->getSrc();
    RuleElementDst *dstrel=rule->getDst();

    if (srcrel->isAny() && dstrel->isAny() && 
        rule->getStr("ipt_chain").empty()  &&
        rule_iface!=nullptr &&
        rule_iface->isLoopback() )
    {
        switch (rule->getDirection())
        {
        case PolicyRule::Inbound:   ipt_comp->setChain(rule,"INPUT");  break;
        case PolicyRule::Outbound:  ipt_comp->setChain(rule,"OUTPUT"); break;
        default: ;
        }
    }

    tmp_queue.push_back(rule);
    return true;
}

/**
 * target CLASSIFY is only valid in mangle table, chain POSTROUTING.
 * However if the same rule also has tagging option, it should be
 * split because we want to tag in PREROUTING
 */
bool PolicyCompiler_ipt::decideOnChainForClassify::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if ( ! rule->getClassification())
    {
	tmp_queue.push_back(rule);
	return true;
    }

    if (rule->getStr("ipt_chain").empty())
    {
        if (rule->getTagging())
        {
            PolicyRule *r = compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setClassification(false);
            r->setRouting(false);
            r->setAction(PolicyRule::Continue);
            tmp_queue.push_back(r);

            rule->setTagging(false);
        }

        ipt_comp->setChain(rule, "POSTROUTING");
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::finalizeChain::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

//    tmp_queue.push_back(rule);

    if ( ! rule->getStr("ipt_chain").empty() )
    {
        tmp_queue.push_back(rule);
        return true;
    }

    ipt_comp->setChain(rule, "FORWARD");

    if (ipt_comp->my_table=="mangle")
    {
        switch ( rule->getDirection() ) 
        {
        case PolicyRule::Inbound:
            ipt_comp->setChain(rule,"PREROUTING");
            break;
        case PolicyRule::Outbound:
            ipt_comp->setChain(rule,"POSTROUTING");
            break;
        default:
            ipt_comp->setChain(rule,"FORWARD");
            break;
        }

        if (rule->getAction() == PolicyRule::Accept)
            ipt_comp->setChain(rule,"PREROUTING");

    } else
    {

        Address *src = compiler->correctForCluster(compiler->getFirstSrc(rule));
        Address *dst = compiler->correctForCluster(compiler->getFirstDst(rule));

/*
 * Note that we deal with address ranges in splitIfSrcMatchingAddressRange and
 * splitIfDstMatchingAddressRange. At this point we treat ranges as always
 * not matching the firewall (so the go into FORWARD chain), except for ranges
 * that consist of 1 address. These should be treated as a single address. This
 * is for #2650
 */

        bool b,m;
/* 
 * do not check for broadcasts and multicasts in bridging firewall because
 * they should go to FORWARD chain 
 */
        b=m= !( compiler->getCachedFwOpt()->getBool("bridging_fw") );

        switch ( rule->getDirection() ) 
        {
        case PolicyRule::Inbound:
/* if direction is "Inbound", chain can never be OUTPUT, but could be FORWARD */
            if (dst && !dst->isAny() && ! AddressRange::isA(dst) &&   // #2650
                ipt_comp->complexMatch(dst, ipt_comp->fw, b, m))
            {
                ipt_comp->setChain(rule,"INPUT");
            }
            break;

        case PolicyRule::Outbound:
/* if direction is "Outbound", chain can never be INPUT, but could be FORWARD */
            if (src && !src->isAny() && ! AddressRange::isA(src) &&   // #2650
                ipt_comp->complexMatch(src, ipt_comp->fw, b, m))
            {
                ipt_comp->setChain(rule,"OUTPUT");
            }
            break;

        default:

/* direction == Both */

            if (dst && !dst->isAny() && ! AddressRange::isA(dst) &&   // #2650
                ipt_comp->complexMatch(dst, ipt_comp->fw, b, m))
            {
                ipt_comp->setChain(rule,"INPUT");
                break;
            }

            if (src && !src->isAny() && ! AddressRange::isA(src) &&   // #2650
                ipt_comp->complexMatch(src, ipt_comp->fw, b, m))
            {
                ipt_comp->setChain(rule,"OUTPUT");
                break;
            }
        }
    }

/*
 * bug #1040599: "unnecessary FORWARD rules".
 * If we haven't decided on INPUT/OUTPUT chain, it stays FORWARD.
 * However, if ip forwarding is turned off, we do not want any rules
 * in FORWARD chain, so we just drop it.
 *
 * If ip forwarding is set to "no change", assume it is on.
 */
    bool ipforw = true;
    string ip_forward_option;
    if (ipt_comp->ipv6)
        ip_forward_option =
            compiler->getCachedFwOpt()->getStr("linux24_ipv6_forward");
    else
        ip_forward_option =
            compiler->getCachedFwOpt()->getStr("linux24_ip_forward");

    if (!ip_forward_option.empty() &&
        (ip_forward_option=="0" ||
         ip_forward_option=="Off" ||
         ip_forward_option=="off")) ipforw = false;

    if (rule->getStr("ipt_chain")=="FORWARD" && !ipforw) return true;

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::decideOnTarget::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    if ( ! rule->getStr("ipt_target").empty() ) return true; // already defined

    // note that we use pseudo-target for action Continue
    switch (rule->getAction())
    {
    case PolicyRule::Accept:   rule->setStr("ipt_target", "ACCEPT");    break;
    case PolicyRule::Deny:     rule->setStr("ipt_target", "DROP");      break;
    case PolicyRule::Reject:   rule->setStr("ipt_target", "REJECT");    break;
    case PolicyRule::Return:   rule->setStr("ipt_target", "RETURN");    break;
//    case PolicyRule::Tag:      rule->setStr("ipt_target", "MARK");      break;
    case PolicyRule::Pipe:     rule->setStr("ipt_target", "QUEUE");     break;
//    case PolicyRule::Classify: rule->setStr("ipt_target", "CLASSIFY");  break;
//    case PolicyRule::Route:    rule->setStr("ipt_target", "ROUTE");     break;

    case PolicyRule::Continue: rule->setStr("ipt_target", ".CONTINUE"); break;
    case PolicyRule::Custom:   rule->setStr("ipt_target", ".CUSTOM");   break;

    case PolicyRule::Branch:
    {
        RuleSet *ruleset = rule->getBranch();
        if (ruleset==nullptr)
            compiler->abort(
                rule, 
                string("Branching rule ") + rule->getLabel() +
                " refers ruleset that does not exist");
        else
            rule->setStr("ipt_target", ruleset->getName());
        break;
    }
    default: ;
    }
    return true;
}

/*
 *  remove fw object from src or dst to simplify rules but only if:
 *
 *  original rule did not have negation and
 *  we do not add any virtual addresses for NAT.
 *
 * After removal the rule collapses to a simple command like this:
 *   iptables -A INPUT -p tcp --dport 22 -m state --state NEW -j ACCEPT
 *
 * this works fine except if we have added virtual addresses for
 * NAT. It is assumed that firewall object in rules represents
 * combination of addresses configured in its interfaces in the
 * GUI. Virtual addresses added for NAT are considered to be a side
 * effect and connections should not be implicitly permitted to them
 * by a rule with fw object in destination. The same applies to fw
 * object in source. See bug #685947
 *
 * To avoid inadvertently opening holes in the firewall by a rule like
 * that, we remove fw object only when it is safe to do so.
 */
bool PolicyCompiler_ipt::removeFW::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (compiler->osconfigurator->getNumOfVirtualAddressesForNat()==0 &&
        ! rule->getBool("upstream_rule_neg") )
    {
        RuleElementSrc *srcrel = rule->getSrc();
        Address        *src    = compiler->getFirstSrc(rule);
        if (src==nullptr)
        {
            compiler->abort(
            rule, "removeFW: Empty Source rule element in rule");
            return true;
        }

        RuleElementDst *dstrel = rule->getDst();
        Address        *dst    = compiler->getFirstDst(rule);
        if (dst==nullptr)
        {
            compiler->abort(
            rule, "removeFW: Empty Destination rule element in rule");
            return true;
        }

        string chain = rule->getStr("ipt_chain");

        if (( chain=="INPUT" || 
              ipt_comp->isChainDescendantOfInput(chain)) &&
            compiler->isFirewallOrCluster(dst))
        {
            dstrel->reset();
        }

        if (( chain=="OUTPUT" ||
              ipt_comp->isChainDescendantOfOutput(chain)) &&
            compiler->isFirewallOrCluster(src))
        {
            srcrel->reset();
        }
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::checkMACinOUTPUTChain::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if ( rule->getStr("ipt_chain")=="OUTPUT" )
    {
//        RuleElementSrc *srcrel=rule->getSrc();
        Address *src = compiler->getFirstSrc(rule);
        assert(src);

        if (physAddress::isA(src))
        {
            compiler->abort(
                rule, "Can not match MAC address of the firewall");
            return true;
        }

        if (combinedAddress::isA(src))
        {
            compiler->warning(
                    rule, 
                    "Can not match MAC address of the firewall "
                    "(chain OUTPUT) ");
            combinedAddress::cast(src)->setPhysAddress("");
        }
    }

    tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::checkUserServiceInWrongChains::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    Service *srv = compiler->getFirstSrv(rule);  assert(srv);
    string chain = rule->getStr("ipt_chain");

    if (UserService::cast(srv) != nullptr &&
        chain != "OUTPUT" &&
        !ipt_comp->isChainDescendantOfOutput(chain))
    {
        compiler->warning(
            
                rule, 
                "Iptables does not support module 'owner' in a chain "
                "other than OUTPUT");
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::fillActionOnReject::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    FWOptions  *ruleopt =rule->getOptionsObject();
    string s=ruleopt->getStr("action_on_reject");
    if (s.empty()) ruleopt->setStr("action_on_reject",
                     ipt_comp->getCachedFwOpt()->getStr("action_on_reject"));

    tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::splitRuleIfSrvAnyActionReject::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrv *srv= rule->getSrv();

    string s=ipt_comp->getActionOnReject(rule);
    if ( rule->getAction()==PolicyRule::Reject && s.empty() && srv->isAny() )
    {
        PolicyRule *r;
        RuleElementSrv *nsrv;

        r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        nsrv=r->getSrv();
        nsrv->addRef(compiler->dbcopy->findInIndex(
                         FWObjectDatabase::getIntId(ANY_TCP_OBJ_ID)));

        FWOptions  *ruleopt =r->getOptionsObject();
        ruleopt->setStr("action_on_reject","TCP RST");

        tmp_queue.push_back(r);
    }

    tmp_queue.push_back(rule);
    return true;
}

/**
 * objects in Srv must be of the same type by the time when we call
 * this rule processor
 */
bool PolicyCompiler_ipt::checkForStatefulICMP6Rules::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    FWOptions *ruleopt = rule->getOptionsObject();

    RuleElementSrv *srv = rule->getSrv();
    if (!srv->isAny())
    {
        Service *s = Service::cast(FWReference::getObject(srv->front()));
        assert(s);
        if (ICMP6Service::isA(s) && ! ruleopt->getBool("stateless"))
        {
            compiler->warning(
                rule,
                "Making rule stateless because it matches ICMPv6");
            ruleopt->setBool("stateless",true);
        }
    }

    tmp_queue.push_back(rule);
    return true;
}

/*
 * I am adding subrule suffix here, which I then use to generate
 * unique new chain name for this rule. The idea is to generate
 * meaningful chain name, which is associated with rule number (like
 * RULE_5), however since this processor runs very early and may split
 * the rule, subsequent processors that create new chains end up
 * creating chains with the same names. Need this suffix to create
 * different, yet meaningful chain names.
 *
 * TODO: add methods addRuleSuffix and getRuleSuffix to class Rule.
 * Define suffix automatically in a tree-like manner, so that when
 * original rule is split, its parts will get suffixes ".1" and
 * ".2". When these parts are split again, suffixes should become
 * ".1.1" and ".1.2" and so on.
 */
bool PolicyCompiler_ipt::splitServicesIfRejectWithTCPReset::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrv *srv= rule->getSrv();

    if ( rule->getAction()==PolicyRule::Reject &&
         ipt_comp->isActionOnRejectTCPRST(rule))
    {
        list<Service*> tcp;
        list<Service*> other;

        for (FWObject::iterator i=srv->begin(); i!=srv->end(); ++i) 
        {
            Service *s1 = Service::cast(FWReference::getObject(*i));
            assert(s1);
            // Protocol name is more reliable reference because CustomService
            // allows user to set protocol name
            if (s1->getProtocolName()=="tcp") tcp.push_back(s1);
            else other.push_back(s1);
        }

        if ( !other.empty() && tcp.empty() )
        {
            if (seen_rules[rule->getPosition()]==false) 
                compiler->warning(
                    
                        rule, 
                        "Rule action 'Reject' with TCP RST can be used "
                        "only with TCP services.");
            ipt_comp->resetActionOnReject(rule);
            tmp_queue.push_back(rule);
            seen_rules[rule->getPosition()]=true;
            return true;
        }

        if ( other.empty() && !tcp.empty() )
        {
            tmp_queue.push_back(rule);
            return true;
        }
/* if both are not empty */

        PolicyRule *r;
        RuleElementSrv *nsrv;

        r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        nsrv=r->getSrv();
        nsrv->clearChildren();

        for (list<Service*>::iterator j=other.begin(); j!=other.end(); j++)
            nsrv->addRef( (*j) );

        r->getOptionsObject()->setStr("action_on_reject","");
        r->setStr("subrule_suffix","1");
        tmp_queue.push_back(r);

        r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        nsrv=r->getSrv();
        nsrv->clearChildren();

        for (list<Service*>::iterator j=tcp.begin(); j!=tcp.end(); j++)
            nsrv->addRef( (*j) );

        r->setStr("subrule_suffix","2");
        tmp_queue.push_back(r);
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}

/*
 * processor groupServicesByProtocol should have been called eariler, so now all
 * services in Srv are of the same type
 */
bool PolicyCompiler_ipt::prepareForMultiport::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrv *rel= rule->getSrv();
    Service        *srv= compiler->getFirstSrv(rule);

    if (rel->size()==1)
    {
	tmp_queue.push_back(rule);
	return true;
    }

    if (IPService::isA(srv) || ICMPService::isA(srv) || ICMP6Service::isA(srv) ||
        CustomService::isA(srv) || TagService::isA(srv))
    {
/* multiport does  not support ip and icmp services, split the rule */
	for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
        {
	    FWObject *o= *i;
	    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
	    
	    Service *s=Service::cast( o );
	    assert(s);

	    PolicyRule *r= compiler->dbcopy->createPolicyRule();
	    compiler->temp_ruleset->add(r);
	    r->duplicate(rule);
	    RuleElementSrv *nsrv=r->getSrv();
	    nsrv->clearChildren();
	    nsrv->addRef( s );

	    tmp_queue.push_back(r);
	}
	return true;
    }

    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
        rule->setBool("ipt_multiport",true);
/* make sure we have no more than 15 ports */
        if (rel->size()>15) 
        {
            int n=0;
            PolicyRule *r;
            RuleElementSrv *nsrv = nullptr;
            for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
            {
                FWObject *o= *i;
                if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
                Service *s=Service::cast( o );
                assert(s);
                if (n==0)
                {
                    r= compiler->dbcopy->createPolicyRule();
                    compiler->temp_ruleset->add(r);
                    r->duplicate(rule);
                    nsrv=r->getSrv();
                    nsrv->clearChildren();
                    tmp_queue.push_back(r);
                }
                assert(nsrv!=nullptr);
                nsrv->addRef( s );
                if (++n>=15) n=0;
            }
        } else
            tmp_queue.push_back(rule);
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}

/*
 *  processor groupServicesByProtocol should have been called before, it makes sure
 *  all objects in Service are of the same type.
 *
 *  One special case is custom service "ESTABLISHED". This processor
 *  splits rule if it finds this service and turns off stateful
 *  inspection on the rule.
 *
 */
bool PolicyCompiler_ipt::specialCasesWithCustomServices::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrv *srv= rule->getSrv();

    if (srv->isAny())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    stack<FWObject*> cl;
    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++) 
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
        assert(o!=nullptr);
        if (CustomService::isA(o)) 
        {
            string code=CustomService::cast(o)->getCodeForPlatform(compiler->myPlatformName());
            if (code.find("ESTABLISHED")!=string::npos ||
                code.find("RELATED")!=string::npos) 
            {
                PolicyRule *r= compiler->dbcopy->createPolicyRule();
                compiler->temp_ruleset->add(r);
                r->duplicate(rule);
                RuleElementSrv *nsrv=r->getSrv();
                nsrv->clearChildren();
                nsrv->addRef(o);
                r->getOptionsObject()->setBool("stateless",true);
                tmp_queue.push_back(r);

                cl.push(o);
            }
        }
    }
    while (!cl.empty()) {
        srv->removeRef( cl.top() );
        cl.pop();
    }
/*
 * if srv is 'any' at this point, then it had only single object at
 * the beginning and that object was CustomService which we've split
 * into a new rule. There is nothing left in the original srv so we
 * can simply drop the old rule.
 */
    if (!srv->isAny())
        tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::convertAnyToNotFWForShadowing::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    FWOptions  *ruleopt =rule->getOptionsObject();
    PolicyRule *r;

    if ( ruleopt->getInt("firewall_is_part_of_any_and_networks")==0 )
    {
        RuleElementSrc *srcrel=rule->getSrc();
        RuleElementDst *dstrel=rule->getDst();

        if (srcrel->isAny())
        {
//            srcrel->addRef(compiler->fw);
//            srcrel->setNeg(true);

            r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setAction( PolicyRule::Return );
            RuleElementSrc *nsrc=r->getSrc();
            nsrc->clearChildren();
            nsrc->addRef( compiler->fw );
            tmp_queue.push_back(r);
        }

        if (dstrel->isAny())
        {
//            dstrel->addRef(compiler->fw);
//            dstrel->setNeg(true);

            r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setAction( PolicyRule::Return );
            RuleElementDst *ndst=r->getDst();
            ndst->clearChildren();
            ndst->addRef( compiler->fw );
            tmp_queue.push_back(r);
        }
    }
    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::processMultiAddressObjectsInRE::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    OSConfigurator_linux24 *osconf = 
        dynamic_cast<OSConfigurator_linux24*>(compiler->osconfigurator);

    RuleElement *re = RuleElement::cast( rule->getFirstByType(re_type) );

    if (re->size()==1) 
    {
        FWObject *o = re->front();
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
        MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
        if (atrt!=nullptr)
        {
            // we have just one object in RE and this object is MutiAddressRunTime
            if (atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            {
                string path =
                    atrt->getSourceNameAsPath(compiler->getCachedFwOpt());
                if (path.empty() && !atrt->getSourceName().empty()) {
                    compiler->abort(rule, "Firewall's data directory not set for address table: " + atrt->getName());
                    return true;
                }
                rule->setStr("address_table_file", path);
                osconf->registerMultiAddressObject(atrt);
            }
            if (atrt->getSubstitutionTypeName()==DNSName::TYPENAME)
            {
                // this is DNSName converted to its run-time counterpart,
                // we do not need to touch it at all
            }
        }
        tmp_queue.push_back(rule);
        return true;
    }

    list<MultiAddressRunTime*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
        MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
        if (atrt!=nullptr && atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            cl.push_back(atrt);
    }

    if (cl.empty())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    RuleElement *nre;
    RuleElement *ore = re;
    PolicyRule *r;
    for (list<MultiAddressRunTime*>::iterator i=cl.begin(); i!=cl.end(); i++) 
    {
        MultiAddressRunTime *atrt = *i;
        r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        nre=RuleElement::cast( r->getFirstByType(re_type) );
        nre->clearChildren();
        nre->addRef( atrt );

        string path = atrt->getSourceNameAsPath(compiler->getCachedFwOpt());
        if (path.empty() && !atrt->getSourceName().empty()) {
            compiler->abort(rule, "Firewall's data directory not set for address table: " + atrt->getName());
            return true;
        }
        r->setStr("address_table_file", path);

        osconf->registerMultiAddressObject(atrt);
        tmp_queue.push_back(r);

        ore->removeRef( *i );
    }

    // if rule element contained only run-time address tables, it should
    // be empty by now. There is no need to continue with this rule then.
    if ( ! re->isAny())
        tmp_queue.push_back(rule);

    return true;
}

/*
 * iptables does not have target that would do nothing and would not
 * terminate processing of the packet (like NOP), so we create a new
 * user chain with target RETURN.
 */
bool PolicyCompiler_ipt::accounting::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElementItf *itf_re = rule->getItf(); assert(itf_re!=nullptr);
    Interface *rule_iface = 
        Interface::cast(FWObjectReference::getObject(itf_re->front()));

    FWOptions  *ruleopt = rule->getOptionsObject();

    if (rule->getAction()==PolicyRule::Accounting &&
        rule->getStr("ipt_target").empty())
    {
        string this_chain = rule->getStr("ipt_chain");
	string new_chain  = ipt_comp->getNewChainName(rule, rule_iface);
        string rule_name_accounting = ruleopt->getStr("rule_name_accounting");
        if (!rule_name_accounting.empty())
            new_chain = rule_name_accounting;

        if (new_chain==this_chain)
        {
            rule->setStr("ipt_target", "RETURN");
            rule->setAction(PolicyRule::Continue);
        } else
        {
            PolicyRule     *r;
            FWOptions      *ruleopt;
            RuleElementSrc *nsrc;
            RuleElementDst *ndst;
            RuleElementSrv *nsrv;
/*
 * add copy of original rule, but turn off logging and set target
 * chain to new_chain.
 */
            r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            nsrc=r->getSrc();  nsrc->reset();
            ndst=r->getDst();  ndst->reset();
            nsrv=r->getSrv();  nsrv->reset();
            r->setStr("ipt_chain",new_chain);
            r->setStr("upstream_rule_chain",this_chain);
            ipt_comp->registerChain(new_chain);
            ipt_comp->insertUpstreamChain(this_chain, new_chain);

            r->setStr("ipt_target", "RETURN");
            r->setLogging(false);
            r->setAction(PolicyRule::Continue);
            tmp_queue.push_back(r);

            rule->setStr("ipt_target", new_chain);
            rule->setLogging(false);
            ruleopt = rule->getOptionsObject();
            ruleopt->setInt("limit_value",-1);
            ruleopt->setInt("connlimit_value",-1);
            ruleopt->setInt("hashlimit_value",-1);
        }
    } 
    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::countChainUsage::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);

    slurp();
    if (tmp_queue.size()==0) return false;

    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        PolicyRule *rule = PolicyRule::cast( *k );
        ipt_comp->chain_usage_counter[rule->getStr("ipt_target")] += 1;
    }

    // second pass: if chain the rule belongs to has never been used as a target
    // then the target chain of the rule will never be used as well
    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        PolicyRule *rule = PolicyRule::cast( *k );
        if (ipt_comp->chain_usage_counter[rule->getStr("ipt_chain")] == 0)
            ipt_comp->chain_usage_counter[rule->getStr("ipt_target")] = 0;
    }

    return true;
}

bool PolicyCompiler_ipt::checkInterfaceAgainstAddressFamily::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    /*
     * If interface is "regular", compiler expects its addresses to
     * match addresses on real firewall. If it does not have any
     * addresses that match address family of the rule set, drop the
     * rule. If interface is not "Regular", i.e. dynamic, unnumbered
     * or bridge port, then compiler assumes it gets its address(es)
     * at run time and therefore can have address that matches address
     * family of the rule set. Therefore we can not drop the rule.
     */

    Interface *rule_iface = compiler->getFirstItf(rule);

    if (rule_iface==nullptr || !rule_iface->isRegular()) 
    {
        tmp_queue.push_back(rule);
        return true;
    }

    string addr_type = IPv4::TYPENAME;
    if (ipt_comp->ipv6) addr_type = IPv6::TYPENAME;

    list<FWObject*> addr_list = rule_iface->getByType(addr_type);
    if (addr_list.size() != 0)
    {
        tmp_queue.push_back(rule);
        return true;
    }

    if (rule_iface->isFailoverInterface())
    {
        /*
         * for ticket #1172 : this is cluster interface that has no
         * address, check properties of the corresponding member
         */
        FailoverClusterGroup *fg = FailoverClusterGroup::cast(
            rule_iface->getFirstByType(FailoverClusterGroup::TYPENAME));
        Interface *other_iface = fg->getInterfaceForMemberFirewall(compiler->fw);

        if (other_iface == nullptr)
        {
            // if we get here, this cluster interface does not have
            // any corresponding interface of the firewall we are
            // compiling right now. What is the right thing to do in
            // this case? I suppose we can't check if this interface
            // matches address family. Dropping the rule.
            QString err("Cluster interface '%1' does not map onto any "
                        "interface of the firewall '%2' but is used "
                        "in the 'Interface' rule element. "
                        "The rule will be dropped because it can "
                        "not be associated with this interface.");
            compiler->warning(rule,
                              err.arg(rule_iface->getName().c_str())
                              .arg(compiler->fw->getName().c_str()).toStdString());
            return true;
        }

        if (other_iface->getByType(addr_type).size() != 0)
        {
            tmp_queue.push_back(rule);
            return true;
        } else
        {
            // member interface also has no addresses
            return true;
        }
    }

    // interface has no addresses and is not cluster failover interface
    // drop the rule
    return true;
}

bool PolicyCompiler_ipt::addPredefinedRules::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);
    return true;
}

void PolicyCompiler_ipt::addRuleFilter()
{
    add( new dropMangleTableRules(" remove rules that require mangle table") );
}

/**
 *-----------------------------------------------------------------------
 */
void PolicyCompiler_ipt::compile()
{
    string version = fw->getStr("version");
    string banner = " Compiling ruleset " + getRuleSetName() +
        " for '" + my_table + "' table";
    if (ipv6) banner += ", IPv6";
    info(banner);

    Compiler::compile();
    bool check_for_recursive_groups=true;

    if ( fw->getOptionsObject()->getBool ("check_shading") &&
         ! inSingleRuleCompileMode())
    {
        add( new Begin("Detecting rule shadowing"));

        addRuleFilter();

        add( new printTotalNumberOfRules());

        // use full negation rule processor in shadowing detection.
        // This rule processor replaces inetrface(s) object(s) with a
        // complimentary set of "other" interfaces of the firewall.
        //
        add( new ItfNegation("process negation in Itf"));

        add( new InterfacePolicyRules(
                 "process interface policy rules and store interface ids"));
        add( new convertAnyToNotFWForShadowing("convert 'any' to '!fw'"));
#if 0
        add( new splitIfSrcAnyForShadowing("split rule if src is any"));
        add( new splitIfDstAnyForShadowing("split rule if dst is any"));
        add( new SrcNegation(true, "process negation in Src"));
        add( new DstNegation(true, "process negation in Dst"));
#endif
        add( new recursiveGroupsInSrc("check for recursive groups in SRC"));
        add( new recursiveGroupsInDst("check for recursive groups in DST"));
        add( new recursiveGroupsInSrv("check for recursive groups in SRV"));
        check_for_recursive_groups=false;

        add( new ExpandGroups("expand groups"));
        add( new dropRuleWithEmptyRE(
                 "drop rules with empty rule elements"));
        add( new eliminateDuplicatesInSRC("eliminate duplicates in SRC"));
        add( new eliminateDuplicatesInDST("eliminate duplicates in DST"));
        add( new eliminateDuplicatesInSRV("eliminate duplicates in SRV"));

        add( new swapMultiAddressObjectsInSrc(
                 " swap MultiAddress -> MultiAddressRunTime in Src"));
        add( new swapMultiAddressObjectsInDst(
                 " swap MultiAddress -> MultiAddressRunTime in Dst"));

/* behavior of processors ExpandMultiple... has been changed in
 * virtual method _expand_interface  */
        add( new ExpandMultipleAddressesInSrc(
                 "expand objects with multiple addresses in SRC"));
        add( new ExpandMultipleAddressesInDst(
                 "expand objects with multiple addresses in DST"));
        add( new dropRuleWithEmptyRE(
                 "drop rules with empty rule elements"));

        add( new ConvertToAtomic("convert to atomic rules"));

/*
 * This assumes that all rules that go into the mangle table are
 * non-terminating. This is not necessarily correct because
 * non-termination is really an attribute of the target. However
 * targets that we support that go into mangle table (CLASSIFY and
 * MARK) are indeed non-terminating.
 */

        add( new SkipActionContinueWithNoLogging(
                 "drop rules with action Continue") );
            
        add( new checkForObjectsWithErrors(
                 "check if we have objects with errors in rule elements"));

        // #2367

        // if (my_table=="mangle" &&
        //     !fw->getOptionsObject()->getBool("classify_mark_terminating")
        // )
        // {
        //     add( new dropTerminatingTargets(
        //              "Drop rules with terminating targets") );
        //     add( new DetectShadowingForNonTerminatingRules(
        //              "Detect shadowing for non-terminating rules" ) );
        // } else

        add( new DetectShadowing("Detect shadowing" ) );

        add( new simplePrintProgress() );

        runRuleProcessors();
        deleteRuleProcessors();
    }


    add( new PolicyCompiler::Begin());

    add( new addPredefinedRules("Add some predefined rules"));

    addRuleFilter();

    add( new printTotalNumberOfRules());

    add( new singleRuleFilter());

    add( new deprecateOptionRoute("Deprecate option Route"));

    add( new checkForUnsupportedCombinationsInMangle(
             "Check for unsupported Tag+Route and Classify+Route combinations"));

    add( new clearTagClassifyInFilter(
             "Clear Tag and Classify options in filter table"));
    add( new clearLogInMangle("clear logging in rules in mangle table"));
    add( new clearActionInTagClassifyIfMangle(
             "clear action in rules with Tag and Classify in mangle"));

    add( new storeAction("store original action of this rule"));

    add( new Logging1("check global logging override option"));

    add( new emptyGroupsInItf("check for empty groups in Itf"));
    add( new expandGroupsInItf("expand groups in Interface" ));
    add( new replaceClusterInterfaceInItf(
             "replace cluster interfaces with member interfaces in the Interface rule element"));

    add( new singleObjectNegationItf(
             "negation in Itf if it holds single object"));
    add( new ItfNegation("process negation in Itf"));

    add( new decideOnChainForClassify("set chain for action is Classify"));

    add( new InterfaceAndDirection("fill in interface and direction"));

// if an action requires chain POSTROUTING (e.g. Classify), set chain
// BEFORE calling splitIfIfaceAndDirectionBoth
    add( new splitIfIfaceAndDirectionBoth(
             "split interface rule with direction 'both'"));

    if (check_for_recursive_groups)
    {
        add( new recursiveGroupsInSrc("check for recursive groups in SRC"));
        add( new recursiveGroupsInDst("check for recursive groups in DST"));
        add( new recursiveGroupsInSrv("check for recursive groups in SRV"));
    }

    add( new emptyGroupsInSrc("check for empty groups in SRC"));
    add( new emptyGroupsInDst("check for empty groups in DST"));
    add( new emptyGroupsInSrv("check for empty groups in SRV"));
/*
 * commented out to fix bug #727324. "-p tcp --destination-port ! 25"
 * means "all TCP with port != 25", which is not the same as "all
 * protocols except TCP port 25". We just can't use "!" with negation
 * in service even if there is only single object in that rule
 * element.
 *
 * Further correction: we CAN use single object negatiob with some types
 * of service objects, such as e.g. TagService or UserService
 */
    add( new SingleSrvNegation("negation in Srv if it holds 1 object"));
    add( new splitRuleIfSrvAnyActionReject(
             "split rule if action is reject and srv is any" ) );
    add( new SrvNegation( false, "process negation in Srv") );
    add( new expandGroupsInSrv("expand groups in Srv" ));

    add( new CheckForTCPEstablished("TCPService with \"established\"") );
            
//        add( new splitRuleIfSrvAnyActionReject(
//                 "split rule if action is reject and srv is any" ) );
    add( new fillActionOnReject("fill in action_on_reject"            ) );
    add( new splitServicesIfRejectWithTCPReset(
             "split if action on reject is TCP reset"));
    add( new fillActionOnReject("fill in action_on_reject 2"          ) );
    add( new splitServicesIfRejectWithTCPReset(
             "split if action on reject is TCP reset 2"));


    add( new SingleSrcNegation("negation in Src if it holds single object"));
    add( new SingleDstNegation("negation in Dst if it holds single object"));

    add( new splitIfSrcNegAndFw("split rule if src has negation and fw"));
    add( new splitIfDstNegAndFw("split rule if dst has negation and fw"));

    add( new SrcNegation(false, "process negation in Src"  ));
    add( new DstNegation(false, "process negation in Dst"  ));
    add( new TimeNegation(false, "process negation in Time" ));

    add( new Logging2("process logging"));

    // #2367 #2397
    add( new splitIfTagClassifyOrRoute(
             "Split rule if it uses tagging, classification or routing options"));
    add( new splitIfTagAndConnmark("Tag+CONNMARK combo"));
    add( new Route("process route rules"));


/*
 * this is just a patch for those who do not understand how does
 * "assume firewall is part of any" work. It also eliminates redundant
 * and useless rules in the FORWARD chain for rules assigned to a
 * loopback interface.
 */
//        add( new decideOnChainIfLoopback("any-any rule on loopback"     ) );


    add( new ExpandGroups("expand all groups"));
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));
    add( new eliminateDuplicatesInSRC("eliminate duplicates in SRC" ));
    add( new eliminateDuplicatesInDST("eliminate duplicates in DST" ));
    add( new eliminateDuplicatesInSRV("eliminate duplicates in SRV" ));
    add( new swapMultiAddressObjectsInSrc(
             " swap MultiAddress -> MultiAddressRunTime in Src"));
    add( new swapMultiAddressObjectsInDst(
             " swap MultiAddress -> MultiAddressRunTime in Dst"));

    add( new accounting("Accounting") );

    add( new splitIfSrcAny("split rule if src is any") );

    if (my_table == "mangle")
        add( new checkActionInMangleTable("check allowed actions in mangle table"));

    add( new setChainForMangle("set chain for other rules in mangle"));
    add( new setChainPreroutingForTag("chain PREROUTING for Tag"));
    add( new splitIfDstAny("split rule if dst is any") );
    add( new setChainPostroutingForTag("chain POSTROUTING for Tag"));


    add( new processMultiAddressObjectsInSrc(
             "process MultiAddress objects in Src"));
    add( new processMultiAddressObjectsInDst(
             "process MultiAddress objects in Dst"));

    if (XMLTools::version_compare(version, "1.2.11") < 0)
    {
        /* Use module iprange for iptables v1.2.11 and later.
         * should expand address range before splitIfSrcMatchesFw because some
         * addresses in the range may match firewall
         */
        add( new addressRanges("process address ranges"));
    } else
    {
        add( new specialCaseAddressRangeInSrc(
                 "replace single address range in Src"));
        add( new specialCaseAddressRangeInDst(
                 "replace single address range in Dst"));
        add( new splitIfSrcMatchingAddressRange(
                 "split rule if Src contains matching address range object"));
        add( new splitIfDstMatchingAddressRange(
                 "split rule if Dst contains matching address range object"));
    }

    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    add( new splitIfSrcMatchesFw("split rule if src matches FW"));
    add( new splitIfDstMatchesFw("split rule if dst matches FW"));

    add( new specialCaseWithFW1(  "special case with firewall"      ) );

    add( new decideOnChainIfDstFW( "decide on chain if Dst has fw" ) );
    add( new splitIfSrcFWNetwork(
             "split rule if src has a net fw has interface on" ) );

    add( new decideOnChainIfSrcFW( "decide on chain if Src has fw" ) );
    add( new splitIfDstFWNetwork(
             "split rule if dst has a net fw has interface on" ) );

    add( new specialCaseWithFW2(
             "replace fw with its interfaces if src==dst==fw" ) );

    /* behavior of processors ExpandMultiple... has been changed in
     * the virtual method expandInterface
     */
    add( new expandMultipleAddressesIfNotFWinSrc(
             "expand multiple addresses if not FW in Src") );
    add( new expandMultipleAddressesIfNotFWinDst(
             "expand multiple addresses if not FW in Dst") );
    add( new expandLoopbackInterfaceAddress(
             "check for loopback interface in the rule objects") );

    // processors that expand objects with multiple addresses
    // check addresses against current address family using member
    // ipv6. If all addresses do not match, we may end up with
    // empty rule element.
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

// trying process rules with multiple interfaces as late as possible
    add( new InterfacePolicyRulesWithOptimization(
             "process interface policy rules and store interface ids") );

    add( new checkInterfaceAgainstAddressFamily(
             "check if interface matches address family") );

    /* this is just a patch for those who do not understand how does
     * "assume firewall is part of any" work. It also eliminates
     * redundant and useless rules in the FORWARD chain for rules
     * assigned to a loopback interface.
     */
    add( new decideOnChainIfLoopback("any-any rule on loopback"     ) );

//      add( new decideOnChainForClassify("set chain if action is Classify"));

    add( new finalizeChain( "decide on chain"   ) );

    /*****************************************************************/
    /*  at this point in all rules where firewall is in either src or
     *  dst, firewall is a single object in that rule element. Other
     *  rule elements may contain multiple objects yet
     */
    add( new specialCaseWithFWInDstAndOutbound(
       "Drop rules in FORWARD chain with non-empty interface and dir Outbound"));

    add( new decideOnTarget( "decide on target"  ) );

    add( new checkForRestoreMarkInOutput(
             "check if we need -A OUTPUT -j CONNMARK --restore-mark"));

    /*
     * removed call to processor removeFW to make changes for bug
     * #685947: "Rules with firewall object allow too much. "
     */
    add( new removeFW("remove fw") );

    add( new ExpandMultipleAddresses("expand multiple addresses"  ) );
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    if (ipv6)
        add( new DropIPv4Rules("drop ipv4 rules"));
    else
        add( new DropIPv6Rules("drop ipv6 rules"));
    //add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    add( new checkForUnnumbered("check for unnumbered interfaces" ) );
    add( new checkForDynamicInterfacesOfOtherObjects(
             "check for dynamic interfaces of other hosts and firewalls"));

    if ( fwopt->getBool("bridging_fw") ) 
        add( new bridgingFw("handle bridging firewall cases"));

    add( new specialCaseWithUnnumberedInterface(
             "check for a special cases with unnumbered interface"));

//        add( new groupServicesByProtocol("split on services"));
//        add( new prepareForMultiport("prepare for multiport"));

    add( new optimize1("optimization 1, pass 1"));
    add( new optimize1("optimization 1, pass 2"));
    add( new optimize1("optimization 1, pass 3"));



    add( new groupServicesByProtocol("split on services"));
    add( new separateTCPWithFlags("split on TCP services with flags"));
    add( new verifyCustomServices("verify custom services"));
    add( new specialCasesWithCustomServices(
             "scpecial cases with some custom services"));
    add( new separatePortRanges("separate port ranges"));
    add( new separateUserServices("separate user services"));
    add( new separateSrcPort("split on TCP and UDP with source ports"));
    add( new checkForStatefulICMP6Rules(
             "Make sure rules that match icmpv6 are stateless"));

    add( new optimize2("optimization 2") );



    add( new prepareForMultiport("prepare for multiport") );

    add( new ConvertToAtomicForAddresses(
             "convert to atomic rules by address elements") );

    add( new checkForZeroAddr("check for zero addresses") );
    add( new checkMACinOUTPUTChain("check for MAC in OUTPUT chain") );
    add( new checkUserServiceInWrongChains(
             "Check for UserSErvice ojects in chains other than OUTPUT"));

    add( new ConvertToAtomicForIntervals(
             "convert to atomic rules by interval element") );

    // see #2235. ACtion Continue should generate iptables command
    // w/o "-j TARGET" parameter
    //
    // add( new SkipActionContinueWithNoLogging(
    //          "drop rules with action Continue") );

    add( new optimize3("optimization 3") );

    add( new optimizeForMinusIOPlus("optimize for '-i +' / '-o +'") );

    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));
    add( new countChainUsage("Count chain usage"));

    add( createPrintRuleProcessor() );

    add( new simplePrintProgress());

    runRuleProcessors();
    deleteRuleProcessors();

}

string PolicyCompiler_ipt::debugPrintRule(Rule *r)
{
    PolicyRule *rule=PolicyRule::cast(r);
    FWOptions  *ruleopt =rule->getOptionsObject();

    RuleElementSrc      *srcrel=rule->getSrc();
    RuleElementDst      *dstrel=rule->getDst();
    RuleElementSrv      *srvrel=rule->getSrv();
    RuleElementInterval *intrel=rule->getWhen();
    RuleElementItf      *itfrel=rule->getItf();

    ostringstream str;

//    str << setw(70) << setfill('-') << "-";

    int no=0;
    FWObject::iterator i1=srcrel->begin();
    FWObject::iterator i2=dstrel->begin(); 
    FWObject::iterator i3=srvrel->begin();
    FWObject::iterator i4=intrel->begin();
    FWObject::iterator i5=itfrel->begin();

    while ( i1!=srcrel->end() || i2!=dstrel->end() ||
            i3!=srvrel->end() || i4!=intrel->end() ||
            i5!=itfrel->end()) 
    {

        str << endl;

        ostringstream src;
        ostringstream dst;
        string srv=" ";
        string time=" ";
        ostringstream itf;

        if (srcrel->getNeg()) src << "!";
        if (dstrel->getNeg()) dst << "!";
        if (srvrel->getNeg()) srv  = "!";
        if (intrel->getNeg()) time = "!";
        if (itfrel->getNeg()) itf  << "!";

        if (i1!=srcrel->end())
        {
            FWObject *o = FWReference::getObject(*i1);
            src << o->getName();
            if (Group::cast(o)!=nullptr)
                src << "[" << o->size() << "]";
            if ( MultiAddress::cast(o)!=nullptr)
                src << string((MultiAddress::cast(o)->isRunTime()) ? "(r)" : "(c)");
        }

        if (i2!=dstrel->end())
        {
            FWObject *o = FWReference::getObject(*i2);
            dst << o->getName();
            if (Group::cast(o)!=nullptr)
                dst << "[" << o->size() << "]";
            if ( MultiAddress::cast(o)!=nullptr)
                dst << string((MultiAddress::cast(o)->isRunTime()) ? "(r)" : "(c)");
        }

        if (i3!=srvrel->end())
        {
            FWObject *o = FWReference::getObject(*i3);
            srv += o->getName();
        }

        if (i4!=intrel->end())
        {
            FWObject *o = FWReference::getObject(*i4);
            time += o->getName();
        }

        if (i5!=itfrel->end())
        {
            FWObject *o = FWReference::getObject(*i5);
            Interface *iface = Interface::cast(o);
            itf << o->getName() << "(" << o->getId() << ")";
            if (iface)
            {
                if (iface->isDyn()) itf << "D";
                if (iface->isUnnumbered()) itf << "U";
                if (iface->isFailoverInterface()) itf << "F";
            }
        }

        int w=0;
        if (no==0)
        {
            str << rule->getLabel();
            w = rule->getLabel().length();
        }
        
        str <<  setw(15-w)  << setfill(' ') << " ";

        str <<  setw(18) << setfill(' ') << src.str();
        str <<  setw(18) << setfill(' ') << dst.str();
        str <<  setw(12) << setfill(' ') << srv.c_str();
        str <<  setw(10) << setfill(' ') << time.c_str();
        str <<  setw(8)  << setfill(' ') << itf.str();

        if (no==0)
        {
            str <<  setw(9)  << setfill(' ') << rule->getActionAsString().c_str();
            str <<  setw(9)  << setfill(' ') << rule->getDirectionAsString().c_str();
            if (rule->getLogging()) str << " LOG";
        } else
            str <<  setw(18)  << setfill(' ') << " ";

        ++no;

        if ( i1!=srcrel->end() ) ++i1;
        if ( i2!=dstrel->end() ) ++i2;
        if ( i3!=srvrel->end() ) ++i3;
        if ( i4!=intrel->end() ) ++i4;
        if ( i5!=itfrel->end() ) ++i5;
    }

    str << " pos=" << rule->getPosition();

    str << " u=" << rule->getUniqueId();

    str << " c=" << printChains(rule);

    str << " t=" << rule->getStr("ipt_target");

    if ( ! rule->getStr(".iface").empty())
        str << " .iface=" << rule->getStr(".iface");

    if (rule->getTagging()) str << " (tag)";
    if (rule->getClassification()) str << " (class)";
    if (rule->getRouting()) str << " (route)";

    if (rule->getAction()==PolicyRule::Reject)
        str << " " + ruleopt->getStr("action_on_reject");

    if (ruleopt!=nullptr && ruleopt->getInt("limit_value")>0)
        str << " limit";

    if (ruleopt!=nullptr && ruleopt->getInt("connlimit_value")>0)
        str << " connlimit";

    if (ruleopt!=nullptr && ruleopt->getInt("hashlimit_value")>0)
        str << " hashlimit";

    if (ruleopt!=nullptr && ruleopt->getBool("no_input_chain"))
        str << " no_input";

    if (ruleopt!=nullptr && ruleopt->getBool("no_output_chain"))
        str << " no_output";

    //ruleopt->dump(str, false, false);

    return str.str();
}

void PolicyCompiler_ipt::epilog()
{
    if (fwopt->getBool("use_iptables_restore") &&
        getCompiledScriptLength()>0 &&
        ! inSingleRuleCompileMode())
    {
        output << "#" << endl;
    }
}

PolicyCompiler_ipt::PrintRule* PolicyCompiler_ipt::createPrintRuleProcessor()
{
    PolicyCompiler_ipt::PrintRule* print_rule = nullptr;
    if (fw->getOptionsObject()->getBool("use_iptables_restore"))
    {
        // bug #1812295: we should use PrintRuleIptRstEcho not only
        // when we have dynamic interfaces, but also when we have
        // address tables expanded at run time. Instead of checking
        // for all these conditions, just always use PrintRuleIptRstEcho
        print_rule = new PrintRuleIptRstEcho(
            "generate code for iptables-restore using echo");
    } else
    {
        print_rule = new PrintRule("generate shell script");
    }
    print_rule->setContext(this);
    print_rule->initialize();

    return print_rule;
}

string PolicyCompiler_ipt::flushAndSetDefaultPolicy()
{
    string res = "";

    if (!inSingleRuleCompileMode() && fwopt->getBool("use_iptables_restore"))
    {
        res += "echo :INPUT DROP [0:0]\n";
        res += "echo :FORWARD DROP [0:0]\n";
        res += "echo :OUTPUT DROP [0:0]\n";
    }
    return res;
}

std::string PolicyCompiler_ipt::printAutomaticRules()
{
    string res="";

    if (!inSingleRuleCompileMode())
    {
        PolicyCompiler_ipt::PrintRule* print_rule = createPrintRuleProcessor();
        // iptables accepted TCPMSS target in filter table, FORWARD chain 
        // in the older versions, but requires it to be in mangle filter
        // starting somewhere 1.3.x
        string version = fw->getStr("version");
        if (XMLTools::version_compare(version, "1.3.0")<0)
            res += print_rule->_clampTcpToMssRule();

        res += print_rule->_printOptionalGlobalRules();
        delete print_rule;
    }
    return res;
}

string PolicyCompiler_ipt::commit()
{
    return createPrintRuleProcessor()->_commit();
}

bool PolicyCompiler_ipt::newIptables(const string &version)
{
    return (version.empty() || version=="ge_1.2.6" ||
            XMLTools::version_compare(version, "1.2.6")>0);
}

list<string> PolicyCompiler_ipt::getUsedChains()
{
    list<string> res;
    for (map<string, int>::iterator it=chain_usage_counter.begin();
         it!=chain_usage_counter.end(); ++it)
        res.push_back(it->first);
    return res;
}

/*
 * see #1417 To policy rules with different module limit settings but
 * otherwise identical should not shadow each other.
 *
 * For all limit modules:
 * rule with rate "-1" (i.e. no rate limiting at all) shadows rule with
 * rate > 0
 * OR
 * rule with greater rate shadows rule with lower rate
 *
 * From man iptables: "A rule using this extension will match until
 * this limit is reached "
 *
 * consider for example two rules: rule 1 that matches 50 pkts/sec and
 * rule 2 that matches 30 pkts/sec
 *
 * rule 1 matches rates between 0 and 49 and rule 2 rates between 0
 * and 29.  This means rule 2 will never match any rate and rule with
 * greater limit value shadows the one with lower limit value
 *
 * we should return true if candidate_rule_2 shadows candidate_rule_1
 */
bool PolicyCompiler_ipt::checkForShadowingPlatformSpecific(PolicyRule *candidate_r1,
                                                           PolicyRule *candidate_r2)
{
    FWOptions *opt_1 = candidate_r1->getOptionsObject();
    FWOptions *opt_2 = candidate_r2->getOptionsObject();

    if (opt_1->getInt("limit_value")>0 || opt_2->getInt("limit_value")>0)
    {
        int rate_1 = opt_1->getInt("limit_value"); if (rate_1 == -1) rate_1 = INT_MAX;
        int rate_2 = opt_2->getInt("limit_value"); if (rate_2 == -1) rate_2 = INT_MAX;
        if (rate_1 > rate_2) return false;

        if (opt_1->getStr("limit_value_not") != opt_2->getStr("limit_value_not"))
            return false;
        if (opt_1->getStr("limit_suffix") != opt_2->getStr("limit_suffix"))
            return false;
    }

    if (opt_1->getInt("connlimit_value")>0 || opt_2->getInt("connlimit_value")>0)
    {
        int rate_1 = opt_1->getInt("connlimit_value"); if (rate_1 == -1) rate_1 = INT_MAX;
        int rate_2 = opt_2->getInt("connlimit_value"); if (rate_2 == -1) rate_2 = INT_MAX;
        if (rate_1 > rate_2) return false;

        if (opt_1->getStr("connlimit_value_not") != opt_2->getStr("connlimit_value_not"))
            return false;
        if (opt_1->getStr("connlimit_suffix") != opt_2->getStr("connlimit_suffix"))
            return false;
    }

    if (opt_1->getInt("hashlimit_value")>0 || opt_2->getInt("hashlimit_value")>0)
    {
        int rate_1 = opt_1->getInt("hashlimit_value"); if (rate_1 == -1) rate_1 = INT_MAX;
        int rate_2 = opt_2->getInt("hashlimit_value"); if (rate_2 == -1) rate_2 = INT_MAX;
        if (rate_1 > rate_2) return false;

        if (opt_1->getStr("hashlimit_suffix") != opt_2->getStr("hashlimit_suffix"))
            return false;
        if (opt_1->getStr("hashlimit_mode") != opt_2->getStr("hashlimit_mode"))
            return false;
        if (opt_1->getStr("hashlimit_name") != opt_2->getStr("hashlimit_name"))
            return false;
    }

    return true;
}
