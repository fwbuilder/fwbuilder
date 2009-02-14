/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id$

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

#include "config.h"

#include "PolicyCompiler_ipt.h"
#include "OSConfigurator_linux24.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/physAddress.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/XMLTools.h"

#include "combinedAddress.h"

#include <stack>

#include <iostream>
#if __GNUC__ > 3 || \
    (__GNUC__ == 3 && (__GNUC_MINOR__ > 2 || (__GNUC_MINOR__ == 2 ) ) ) || \
    _MSC_VER
#  include <streambuf>
#else
#  include <streambuf.h>
#endif
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include <assert.h>

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
        standard_chains.push_back("CUSTOM");
        standard_chains.push_back("ROUTE");
    }
    return standard_chains;
}

void join::operator()(std::string &s)
{
    if (!result->empty()) *result += separator;
    *result += s;
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

string PolicyCompiler_ipt::getInterfaceVarName(FWObject *iface, bool v6)
{
    ostringstream  ostr;
    string iname = iface->getName();
    string::size_type p1;
    while ( (p1=iname.find("."))!=string::npos)
        iname=iname.replace(p1,1,"_");
    ostr << "i_" << iname;
    if (v6) ostr << "_v6";
    return ostr.str();
}

string PolicyCompiler_ipt::getAddressTableVarName(FWObject *at)
{
    ostringstream  ostr;
    string name=at->getName();
    string::size_type p1;
    char *bad_shell_chars = " !#$&*()-+=\\|{}[]?<>,.";
    for (char *cptr=bad_shell_chars; *cptr; cptr++)
    {
        while ( (p1=name.find(*cptr))!=string::npos)
            name=name.replace(p1,1,"_");
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
        string iface_name=rule_iface->getName();
        string::size_type n=iface_name.find("*");
        str << iface_name.substr(0,n) << "_";
    }

    switch (rule->getDirection()) {
    case PolicyRule::Inbound:   str << "In_";  break;
    case PolicyRule::Outbound:  str << "Out_"; break;
    default: ;
    }
    int pos=rule->getPosition();

    string ruleset_name = getRuleSetName();

    if (ruleset_name != "Policy")
        str << ruleset_name << "_";
    else
        str << "RULE_";

    if (pos>=0)
        str << pos;
    else // special case: position == -1
        str << "000";

    string suffix=rule->getStr("subrule_suffix");
    if (!suffix.empty()) str << "_" << suffix;

    chain_no++;

    return str.str();
}

void PolicyCompiler_ipt::_expandInterface(Interface *iface,
                                          std::list<FWObject*> &ol)
{
    std::list<FWObject*>    ol1;

    std::list<FWObject*>    lipaddr;
    std::list<FWObject*>    lother;
    physAddress            *pa=NULL;

    Compiler::_expandInterface(iface,ol1);
    for (std::list<FWObject*>::iterator j=ol1.begin(); j!=ol1.end(); j++)
    {
        if ((*j)->getTypeName() == IPv4::TYPENAME)
        {
            lipaddr.push_back(*j);
            continue;
        }
        if (physAddress::cast(*j)!=NULL)
        {
            pa = physAddress::cast(*j);
            continue;
        }
        lother.push_back(*j);
    }

/* 
 * if pa==NULL then this is trivial case: there is no physical address
 */
    if (pa==NULL)
    {
        ol.insert(ol.end(),ol1.begin(),ol1.end());
        return;
    }

/* At this point we have physAddress object and have to deal with it
 *
 * Compiler::_expandInterface picks all IPv4 objects and physAddress
 * object under Interface; it can also add interface object(s) to
 * the list.
 *
 * We have two possibilities now: there could be IPv4 objects or
 * not. In either case list ol1 may contain also interface object(s).
 * If there are IPv4 objects, we replace them with combinedAddress
 * objects which store information about IPv4 address and physAddress pa.
 * If there were no IPv4 objects, then we pass physAddress along.
 * We always copy interface  objects to the output list.
 * 
 *
 *
 * we use physAddress only if Host option "use_mac_addr_filter" of the
 * parent Host object is true
 */
    FWObject  *p;
    FWOptions *hopt;
    p=iface->getParent();
    bool use_mac= (Host::cast(p)!=NULL && 
                   (hopt=Host::cast(p)->getOptionsObject())!=NULL &&
                   hopt->getBool("use_mac_addr_filter") ); 



    if (lipaddr.empty())    ol.push_back(pa);
    else
    {
        std::list<FWObject*>::iterator j=lipaddr.begin();
        for ( ; j!=lipaddr.end(); j++)
        {
            //const InetAddrMask *ipv4 = Address::cast(*j)->getAddressObjectInetAddrMask();
            const InetAddr *ip_addr = Address::cast(*j)->getAddressPtr();
            const InetAddr *ip_netm = Address::cast(*j)->getNetmaskPtr();
            if (use_mac)
            {
                combinedAddress *ca = new combinedAddress();
                dbcopy->add(ca);
                cacheObj(ca);
                ca->setName( "CA("+iface->getName()+")" );
                ca->setAddress( *ip_addr );
                ca->setNetmask( *ip_netm );
                ca->setPhysAddress( pa->getPhysAddress() );

                ol.push_back(ca);
            } else
                ol.push_back(*j);
        }
    }
    ol.insert(ol.end(),lother.begin(),lother.end());
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

int PolicyCompiler_ipt::prolog()
{
    if (fw->getStr("platform")!="iptables")
	abort(_("Unsupported platform ") + fw->getStr("platform") );

    int n = PolicyCompiler::prolog();

    FWOptions *fwopt = getCachedFwOpt();

    istringstream is(fwopt->getStr("ipt_mangle_only_rulesets"));
    std::copy(istream_iterator<string>(is),
              istream_iterator<string>(),
              back_inserter(mangle_only_rulesets));


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
    dbcopy->add(anytcp);
    cacheObj(anytcp); // to keep cache consistent

    tcpsyn=dbcopy->createTCPService();
    tcpsyn->setId(FWObjectDatabase::registerStringId(TCP_SYN_OBJ_ID));
    tcpsyn->setName("tcpSYN");
    tcpsyn->setTCPFlag(TCPService::SYN,true);
    tcpsyn->setAllTCPFlagMasks();
    dbcopy->add(tcpsyn);
    cacheObj(tcpsyn); // to keep cache consistent

    anyudp=dbcopy->createUDPService();
    anyudp->setId(FWObjectDatabase::registerStringId(ANY_UDP_OBJ_ID));
    anyudp->setName("AnyUDP");
    dbcopy->add(anyudp);
    cacheObj(anyudp); // to keep cache consistent

    anyicmp=dbcopy->createICMPService();
    anyicmp->setId(FWObjectDatabase::registerStringId(ANY_ICMP_OBJ_ID));
    anyicmp->setName("AnyICMP");
    dbcopy->add(anyicmp);
    cacheObj(anyicmp); // to keep cache consistent

    anyip=dbcopy->createIPService();
    anyip->setId(FWObjectDatabase::registerStringId(ANY_IP_OBJ_ID));
    anyip->setName("AnyIP");
    dbcopy->add(anyip);
    cacheObj(anyip); // to keep cache consistent

    bcast255=dbcopy->createIPv4();
    bcast255->setId(FWObjectDatabase::registerStringId(BCAST_255_OBJ_ID));
    bcast255->setName("Broadcast_addr");
    bcast255->setAddress(InetAddr::getAllOnes());
    bcast255->setNetmask(InetAddr(InetAddr::getAllOnes()));
    dbcopy->add(bcast255);
    cacheObj(bcast255);


    bool afpa = fwopt->getBool("firewall_is_part_of_any_and_networks");

    for(FWObject::iterator i=combined_ruleset->begin();
        i!=combined_ruleset->end(); i++)
    {
	Rule *r = Rule::cast( *i );
	if (r->isDisabled()) continue;

        FWOptions  *ruleopt = r->getOptionsObject();
        ruleopt->setBool("firewall_is_part_of_any_and_networks",
                         afpa | ruleopt->getBool("firewall_is_part_of_any_and_networks"));
    }

    return n;
}

void PolicyCompiler_ipt::addPredefinedPolicyRules()
{
}

bool  PolicyCompiler_ipt::SkipActionContinueWithNoLogging::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if ( ! rule->getStr("ipt_target").empty() &&
         rule->getStr("ipt_target") == "CONTINUE" &&
         ! rule->getLogging()) return true;  // skip this rule

    tmp_queue.push_back(rule);
    return true;
}

/*
 * by the time this processor is called all non-terminating rules should
 * be processed if terminating behevior needs to be emulated.
 */
bool  PolicyCompiler_ipt::dropTerminatingTargets::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    string tgt = rule->getStr("ipt_target");
    
    if (tgt=="CLASSIFY" || tgt=="MARK") tmp_queue.push_back(rule);
    return true;
}

/*
 * This rule processor converts non-terminating targets CLASSIFY and
 * MARK to terminating targets (equivalent) by splitting the rule and
 * adding one more rule with target ACCEPT.
 *
 * Call this rule processor at the very end of the chain when all
 * splits are done and target is set via "ipt_target"
 */
bool  PolicyCompiler_ipt::splitNonTerminatingTargets::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    string tgt = rule->getStr("ipt_target");
    FWOptions      *ruleopt = rule->getOptionsObject();
    
    if (compiler->fw->getOptionsObject()->getBool("classify_mark_terminating") &&
        !ruleopt->getBool("already_terminating_target") &&
        (tgt=="CLASSIFY" || tgt=="MARK"))
    {
        RuleElementSrc *nsrc;
        RuleElementDst *ndst;
        RuleElementSrv *nsrv;
        RuleElementItf *nitfre;
        PolicyRule     *r, *r2;

        string this_chain = rule->getStr("ipt_chain");
        string new_chain  = this_chain;

        nsrc = rule->getSrc();
        ndst = rule->getDst();
        nsrv = rule->getSrv();
        nitfre = rule->getItf();

        if (!nsrc->isAny() ||
            !ndst->isAny() ||
            !nsrv->isAny() ||
            !nitfre->isAny())
        {
            new_chain = ipt_comp->getNewTmpChainName(rule);
            r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setStr("subrule_suffix","ntt");
            r->setStr("ipt_target",new_chain);
            tmp_queue.push_back(r);
        }

        r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        nsrc = r->getSrc();   nsrc->reset();
        ndst = r->getDst();   ndst->reset();
        nsrv = r->getSrv();   nsrv->reset();
        nitfre = r->getItf(); nitfre->reset();
        r->setInterfaceId(-1);
        ruleopt = r->getOptionsObject();
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
        ruleopt->setBool("stateless",true);
        r->setLogging(false);
	r->setStr("ipt_chain",new_chain);
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);
        tmp_queue.push_back(r);

        r2= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r2);
        r2->duplicate(r);
        r2->setAction(PolicyRule::Accept);
        r2->setStr("ipt_target","ACCEPT");
        ruleopt = r2->getOptionsObject();
        ruleopt->setBool("stateless",true);
        tmp_queue.push_back(r2);

        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}

// this version just splits rule so that each elementary rule is associated
// with one interface.

bool  PolicyCompiler_ipt::InterfacePolicyRulesWithOptimization::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementItf *itfre=rule->getItf();   assert(itfre);
    // sometimes for whatever reason itf rule element appears to be broken
    // (it is just empty, not even "any")
    if (itfre->isAny() || itfre->size()==0)
    {
        rule->setInterfaceId(-1);
        tmp_queue.push_back(rule);
        return true;
    }

    RuleElementItf *nitfre;
    PolicyRule *r;

    for (FWObject::iterator i=itfre->begin(); i!=itfre->end(); ++i)
    {
        FWObject *o = *i;
        if (FWReference::cast(o)!=NULL) o = FWReference::cast(o)->getPointer();

        if (ObjectGroup::isA(o))
        {
            // a group in "interface" rule element. GUI checks that only
            // interfaces are allowed in such group, but we should check anyway.
            for (FWObject::iterator i=o->begin(); i!=o->end(); ++i)
            {
                FWObject *o1 = *i;
                if (FWReference::cast(o1)!=NULL)
                    o1=FWReference::cast(o1)->getPointer();
                if (!Interface::isA(o1))
                {
                    compiler->warning("Object '" + o1->getName() + "', which is not an interface, is a member of the group '" + o->getName() + "' used in 'Interface' element of a rule.   Rule: " + rule->getLabel());
                    continue;
                }
                r = compiler->dbcopy->createPolicyRule();
                compiler->temp_ruleset->add(r);
                r->duplicate(rule);
                r->setStr("subrule_suffix","i1");
                r->setInterfaceId(o1->getId());
                nitfre = r->getItf(); nitfre->reset(); nitfre->addRef(o1);
                tmp_queue.push_back(r);
            }
        } else
        {
            r = compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setStr("subrule_suffix", "i1");
            r->setInterfaceId(o->getId());
            nitfre = r->getItf(); nitfre->reset(); nitfre->addRef(o);
            tmp_queue.push_back(r);
        }
    }
    return true;
}

bool PolicyCompiler_ipt::Route::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    FWOptions *ruleopt =rule->getOptionsObject();
    
    if (rule->getAction() == PolicyRule::Route)
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


/*
 * A note about CLASSIFY target in iptables:
 *
 * CLASSIFY only works in mangle table in POSTROUTING chain.
 * the man page does not mention this, but module documentation
 * in p-o-m says so.
 *
 * per bug #1618329: "Wrong in-code comment" this comment is incorrect,
 * CLASSIFY target is valid in POSTROUTING, OUTPUT and FORWARD chains.
 */
bool PolicyCompiler_ipt::dropMangleTableRules::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);

    string ruleset_name = compiler->getRuleSetName();

    if (ipt_comp->isMangleOnlyRuleSet(ruleset_name)) return true;

    if (rule->getAction() == PolicyRule::Tag ||
        rule->getAction() == PolicyRule::Route ||
        rule->getAction() == PolicyRule::Classify) return true;

    tmp_queue.push_back(rule);

    return true;
}


bool PolicyCompiler_ipt::Logging1::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if ( compiler->getCachedFwOpt()->getBool("log_all") )
        rule->setLogging(true);

    tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::storeAction::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    rule->setStr("stored_action", rule->getActionAsString() );

    tmp_queue.push_back(rule);
    return true;
}

/**
 * splits rule if logging is required and either src or dst is
 * not any
 */
bool PolicyCompiler_ipt::Logging2::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    Interface *rule_iface = compiler->getCachedFwInterface(rule->getInterfaceId());

    RuleElementSrc      *nsrc;
    RuleElementDst      *ndst;
    RuleElementSrv      *nsrv;
    RuleElementInterval *nint;
    RuleElementItf      *nitfre;

    if (rule->getLogging()) 
    {
/*chain could have been assigned if we split this rule before */
        string this_chain = rule->getStr("ipt_chain");
	string new_chain  = ipt_comp->getNewChainName(rule,rule_iface);

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
            (rule->getWhen())!=NULL && rule->getWhen()->isAny())
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
	if ( (nint=r->getWhen())!=NULL )  nint->reset();
	r->setStr("ipt_chain",new_chain);
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);

	r->setStr("ipt_target","LOG");
        r->setAction(PolicyRule::Continue);    // ###
	r->setDirection( PolicyRule::Both );
	r->setLogging(false);
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
	if ( (nint=r->getWhen())!=NULL )  nint->reset();
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

	r->setInterfaceStr("nil");
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
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

bool PolicyCompiler_ipt::singleSrcNegation::processNext()
{
    PolicyRule *rule = getNext(); if (rule==NULL) return false;
    RuleElementSrc *srcrel = rule->getSrc();

/*   ! A  B  C  ACTION  */
    if (srcrel->getNeg() && srcrel->size()==1)
    {
        Address *src = compiler->getFirstSrc(rule);  
        // note: src can be NULL if object in this rule element is a group
        // or MultiAddress
        if (src!=NULL && src->countInetAddresses(true)==1 &&
            !compiler->complexMatch(src, compiler->fw)) 
        {
            srcrel->setNeg(false);
            srcrel->setBool("single_object_negation", true);
        }
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::singleDstNegation::processNext()
{
    PolicyRule *rule = getNext(); if (rule==NULL) return false;
    RuleElementDst *dstrel = rule->getDst();

/*   A  ! B  C  ACTION  */
    if (dstrel->getNeg() && dstrel->size()==1)
    {
        Address *dst = compiler->getFirstDst(rule);  
        if (dst!=NULL && dst->countInetAddresses(true)==1 &&
            !compiler->complexMatch(dst, compiler->fw)) 
        {
            dstrel->setNeg(false);
            dstrel->setBool("single_object_negation", true);
        }
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::singleSrvNegation::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *srvrel=rule->getSrv();
    Service *srv=compiler->getFirstSrv(rule); // need to make sure it is not a group
    // see comment in compile() where this rule processor is used for why
    // only some services can be processed here.
    if (TagService::isA(srv) || UserService::isA(srv))
    {
/*   A  B  ! C  ACTION  */
        if (srvrel->getNeg() && srvrel->size()==1 && srv!=NULL ) 
        {
            srvrel->setNeg(false);
            srvrel->setBool("single_object_negation", true);
        }
    }

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_ipt::SrcNegation::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule         *rule=getNext(); if (rule==NULL) return false;
    FWOptions  *ruleopt =rule->getOptionsObject();
    bool afpa = ruleopt->getBool("firewall_is_part_of_any_and_networks");

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
	r->setLogging(false);
	r->setStr("ipt_target",new_chain);
        ruleopt =r->getOptionsObject();
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
        ruleopt->setBool("firewall_is_part_of_any_and_networks",afpa);
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
            if ( (nint=r->getWhen())!=NULL )  nint->reset();
        }

	r->setAction( PolicyRule::Return );
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
            ndst=r->getDst();   ndst->reset();
            nitf=r->getItf();   nitf->reset();
            if ( (nint=r->getWhen())!=NULL )  nint->reset();
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    FWOptions  *ruleopt =rule->getOptionsObject();
    bool afpa = ruleopt->getBool("firewall_is_part_of_any_and_networks");

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
	r->setLogging(false);
	r->setStr("ipt_target",new_chain);
        ruleopt =r->getOptionsObject();
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
        ruleopt->setBool("firewall_is_part_of_any_and_networks",afpa);
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
            if ( (nint=r->getWhen())!=NULL )  nint->reset();
        }

	r->setAction( PolicyRule::Return );
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
            if ( (nint=r->getWhen())!=NULL )  nint->reset();
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
            if ( (nint=r->getWhen())!=NULL )  nint->reset();
        }

	r->setAction( PolicyRule::Return );
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
            if ( (nint=r->getWhen())!=NULL )  nint->reset();
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    FWOptions  *ruleopt =rule->getOptionsObject();
    bool afpa = ruleopt->getBool("firewall_is_part_of_any_and_networks");

    RuleElementInterval *intrel=rule->getWhen();

/*   A  B  C !D  ACTION  */

    if (intrel!=NULL && intrel->getNeg()) 
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
	if ( (nint=r->getWhen())!=NULL )  nint->reset();
	r->setLogging(false);
	r->setStr("ipt_target",new_chain);
        ruleopt =r->getOptionsObject();
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
        ruleopt->setBool("firewall_is_part_of_any_and_networks",afpa);
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

        if ( (nint=r->getWhen())!=NULL )  nint->reset();

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    RuleElementItf *itfre=rule->getItf();   assert(itfre);
    
    if (rule->getDirection()==PolicyRule::Undefined) 
	rule->setDirection( PolicyRule::Both );

    if (itfre->isAny() && rule->getDirection()==PolicyRule::Both) 
    {
	rule->setInterfaceStr("nil");
	return true;
    }

    if (itfre->isAny() && (
	    rule->getDirection()==PolicyRule::Inbound ||
	    rule->getDirection()==PolicyRule::Outbound ))
    {
	rule->setInterfaceStr("*");
	return true;
    }


    return true;
}

bool PolicyCompiler_ipt::setChainPreroutingForTag::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
    if ( (rule->getAction() == PolicyRule::Tag ||
          rule->getStr("stored_action")=="Tag") && 
          rule->getStr("ipt_chain").empty() &&
         (rule->getDirection()==PolicyRule::Both ||
          rule->getDirection()==PolicyRule::Inbound) &&
         rule->getInterfaceId()==-1 )
        ipt_comp->setChain(rule, "PREROUTING");

    tmp_queue.push_back(rule);

    return true;
}


bool PolicyCompiler_ipt::setChainPostroutingForTag::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if ( (rule->getAction() == PolicyRule::Tag ||
          rule->getStr("stored_action")=="Tag") && 
          rule->getStr("ipt_chain").empty() &&
         (rule->getDirection()==PolicyRule::Both ||
          rule->getDirection()==PolicyRule::Outbound) &&
         rule->getInterfaceId()==-1 )
        ipt_comp->setChain(rule, "POSTROUTING");

    tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::checkForRestoreMarkInOutput::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule = getNext(); if (rule==NULL) return false;
    FWOptions  *ruleopt = rule->getOptionsObject();

    if ( (rule->getAction() == PolicyRule::Tag ||
          rule->getStr("stored_action")=="Tag") && 
         ruleopt->getBool("ipt_mark_connections") &&
         rule->getStr("ipt_chain")=="OUTPUT")
        ipt_comp->have_connmark_in_output = true;

    tmp_queue.push_back(rule);
    return true;
}
 

bool PolicyCompiler_ipt::setChainForMangle::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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

/*
 * couple of special cases for rules with action Tag
 *
 * option 'ipt_mark_connections' means we need to generate two rules:
 * one with target MARK and another with target CONNMARK. We place
 * these two new rules in a separate chain.
 *
 * if global option 'classify_mark_terminating' is also on, we place third rule in
 * the same chain, this time with action ACCEPT.
 *
 * Note that if option 'ipt_mark_connections' is off, we do not process
 * classify_mark_terminating option here. It will be processed later in 
 * splitNonTerminatingTargets
 */
bool PolicyCompiler_ipt::splitIfTagAndConnmark::processNext()
{
    PolicyRule *rule = getNext(); if (rule==NULL) return false;
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    FWOptions *ruleopt = rule->getOptionsObject();
    Interface *rule_iface = compiler->getCachedFwInterface(rule->getInterfaceId());

    RuleElementSrc *nsrc;
    RuleElementDst *ndst;
    RuleElementSrv *nsrv;
    RuleElementInterval *nint;
    bool make_terminating =
        compiler->fw->getOptionsObject()->getBool("classify_mark_terminating");

    if (rule->getAction() == PolicyRule::Tag &&
        ruleopt->getBool("ipt_mark_connections"))
    {
	PolicyRule *r, *r1;

        if (make_terminating)
            ruleopt->setBool("already_terminating_target",true);

        string this_chain  = rule->getStr("ipt_chain");
	string new_chain=ipt_comp->getNewChainName(rule,rule_iface);

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setStr("ipt_target",new_chain);
	r->setLogging(false);
        r->setAction(PolicyRule::Continue);
	r->setLogging(false);
        ruleopt =r->getOptionsObject();
        if (make_terminating)
            ruleopt->setBool("already_terminating_target",true);

	tmp_queue.push_back(r);

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setStr("ipt_chain",new_chain);
        r->setStr("upstream_rule_chain",this_chain);
        ipt_comp->registerChain(new_chain);
        ipt_comp->insertUpstreamChain(this_chain, new_chain);

//	r->setDirection( PolicyRule::Both );
        ruleopt =r->getOptionsObject();
        ruleopt->setBool("stateless",true);
        r->setBool("force_state_check",false);
        ruleopt->setInt("limit_value",-1);
        ruleopt->setInt("connlimit_value",-1);
        ruleopt->setInt("hashlimit_value",-1);
	nsrc=r->getSrc();   nsrc->reset();
	ndst=r->getDst();   ndst->reset();
	nsrv=r->getSrv();   nsrv->reset();
	if ( (nint=r->getWhen())!=NULL )  nint->reset();
        if (make_terminating)
            ruleopt->setBool("already_terminating_target",true);

	tmp_queue.push_back(r);

	r1= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r1);
	r1->duplicate(r);
	r1->setStr("ipt_target","CONNMARK");
        r1->setAction(PolicyRule::Continue);    // ###
        r1->setLogging(false);
        ruleopt =r1->getOptionsObject();
        ruleopt->setStr("CONNMARK_arg","--save-mark");
        if (make_terminating)
            ruleopt->setBool("already_terminating_target",true);

	tmp_queue.push_back(r1);

        if (make_terminating)
        {
            r1 = compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r1);
            r1->duplicate(r);
            r1->setStr("ipt_target","ACCEPT");
            r1->setAction(PolicyRule::Accept);
            r1->setLogging(false);
            tmp_queue.push_back(r1);
        }

        ipt_comp->have_connmark = true;

    } else
	tmp_queue.push_back(rule);

    return true;
}


bool PolicyCompiler_ipt::splitIfIfaceAndDirectionBoth::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
            RuleElementItf *itfre = rule->getItf();
            assert(itfre);

            Interface *rule_iface =
                compiler->getCachedFwInterface(rule->getInterfaceId());

            if (rule_iface!=NULL && 
                (rule_iface->isUnnumbered() ||
                 rule_iface->isBridgePort() )
            ) ipt_comp->setChain(rule, "FORWARD");
            else
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    FWOptions  *ruleopt = rule->getOptionsObject();

/* commented to fix bug #1112470
 * if fw is considered part of any, we should place rule in INPUT/OUTPUT
 * chains even if it is a bridging fw since fw itself may send or receive
 * packets
 */
    if ( /* fwopt->getBool("bridging_fw") || */
         ! ruleopt->getBool("firewall_is_part_of_any_and_networks") ||
         ruleopt->getBool("no_output_chain")) 
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
             ( srcrel->size()==1 && src!=NULL &&
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

        // if this rule is for mangle table, need to put it into
        // POSTROUTING chain as well because some targets that
        // work with mangle table can only go into POSTROUTING chain
        // such as CLASSIFY
        if (ipt_comp->my_table=="mangle" &&
            rule->getAction()==PolicyRule::Classify)
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

//    FWOptions *fwopt = compiler->getCachedFwOpt();
    FWOptions  *ruleopt = rule->getOptionsObject();
/* commented to fix bug #1112470
 * if fw is considered part of any, we should place rule in INPUT/OUTPUT
 * chains even if it is a bridging fw since fw itself may send or receive
 * packets
 */
    if ( /* fwopt->getBool("bridging_fw") || */
         ! ruleopt->getBool("firewall_is_part_of_any_and_networks") ||
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

             ( dstrel->size()==1 && dst!=NULL &&
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
        if (ipt_comp->my_table=="mangle" && rule->getAction()==PolicyRule::Classify)
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

bool PolicyCompiler_ipt::splitIfSrcAnyForShadowing::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if (rule->getAction() == PolicyRule::Classify)
    {
	tmp_queue.push_back(rule);
	return true;
    }

    RuleElementSrc *srcrel=rule->getSrc();
    FWOptions  *ruleopt = rule->getOptionsObject();

    if ( ruleopt->getBool("firewall_is_part_of_any_and_networks") &&
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if (rule->getAction() == PolicyRule::Classify)
    {
	tmp_queue.push_back(rule);
	return true;
    }

    RuleElementDst *dstrel=rule->getDst();
    FWOptions  *ruleopt = rule->getOptionsObject();

    if ( ruleopt->getBool("firewall_is_part_of_any_and_networks") &&
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if (rule->getAction() == PolicyRule::Classify)
    {
	tmp_queue.push_back(rule);
	return true;
    }

    RuleElementSrc *srcrel=rule->getSrc();

    FWOptions *fwopt = compiler->getCachedFwOpt();
    FWOptions  *ruleopt = rule->getOptionsObject();
    if ( fwopt->getBool("bridging_fw") ||
         ! ruleopt->getBool("firewall_is_part_of_any_and_networks") ||
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
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            Address *a = Address::cast(o);
            FWObject *na;
            if ((Network::isA(o) || NetworkIPv6::isA(o)) &&
                (na=compiler->findAddressFor(a, compiler->fw ))!=NULL)
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if (rule->getAction() == PolicyRule::Classify)
    {
	tmp_queue.push_back(rule);
	return true;
    }

    RuleElementDst *dstrel=rule->getDst();

    FWOptions *fwopt = compiler->getCachedFwOpt();
    FWOptions  *ruleopt = rule->getOptionsObject();
    if ( fwopt->getBool("bridging_fw") ||
         ! ruleopt->getBool("firewall_is_part_of_any_and_networks") ||
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
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            Address *a = Address::cast(o);
            FWObject *na;
            if ((Network::isA(a) || NetworkIPv6::isA(a)) &&
                (na=compiler->findAddressFor(a, compiler->fw))!=NULL)
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

//    RuleElementSrc *srcrel=rule->getSrc();
    Address        *src   =compiler->getFirstSrc(rule);  assert(src);
//    RuleElementDst *dstrel=rule->getDst();
    Address        *dst   =compiler->getFirstDst(rule);  assert(dst);

    if (src->getId()!=compiler->getFwId() &&
	dst->getId()==compiler->getFwId() &&
	rule->getDirection()==PolicyRule::Outbound )
	throw FWException(_("direction can not be outbound when destination is firewall, in rule ")+rule->getLabel());

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::checkSrcAndDst2::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

//    RuleElementSrc *srcrel=rule->getSrc();
    Address        *src   =compiler->getFirstSrc(rule);  assert(src);
//    RuleElementDst *dstrel=rule->getDst();
    Address        *dst   =compiler->getFirstDst(rule);  assert(dst);

    if (src->getId()==compiler->getFwId() &&
	dst->getId()!=compiler->getFwId() &&
	rule->getDirection()==PolicyRule::Inbound )
	throw FWException(_("direction can not be inbound when source is firewall, in rule ")+rule->getLabel());

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::specialCaseWithFW1::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if (rule->getAction() == PolicyRule::Classify)
    {
	tmp_queue.push_back(rule);
	return true;
    }

//    RuleElementSrc *srcrel=rule->getSrc();
    Address        *src   =compiler->getFirstSrc(rule);  
    if(src==NULL) throw(_("Broken SRC in rule ")+rule->getLabel());
//    RuleElementDst *dstrel=rule->getDst();
    Address        *dst   =compiler->getFirstDst(rule);
    if(dst==NULL) throw(_("Broken DST in rule ")+rule->getLabel());

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

/*
 * this is basically the same as ExpandMultipleAddresses except it
 * does not skip loopback
 */
bool PolicyCompiler_ipt::specialCaseWithFW2::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs   =Interface::cast( obj );

        if (ifs!=NULL && 
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
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

void PolicyCompiler_ipt::checkForDynamicInterfacesOfOtherObjects::findDynamicInterfaces(RuleElement *re,
                                                                                        Rule        *rule)
{
    if (re->isAny()) return;
    list<FWObject*> cl;
    for (list<FWObject*>::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs   =Interface::cast( obj );

        if (ifs!=NULL && ifs->isDyn() && ! ifs->isChildOf(compiler->fw))        
        {
            char errstr[2048];
            sprintf(errstr,_("Can not build rule using dynamic interface '%s' of the object '%s' because its address in unknown. Rule %s"),
                    ifs->getName().c_str(), 
                    ifs->getParent()->getName().c_str(),
                    rule->getLabel().c_str() );

            throw FWException(errstr);
        }
    }
}


bool PolicyCompiler_ipt::checkForDynamicInterfacesOfOtherObjects::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    findDynamicInterfaces( rule->getSrc() , rule );
    findDynamicInterfaces( rule->getDst() , rule );

    tmp_queue.push_back(rule);
    return true;
}

/*
 * remember, behavior of this processor has been changed in virtual
 * method _expandInterface
 */
bool PolicyCompiler_ipt::expandMultipleAddressesIfNotFWinSrc::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    RuleElementSrc *srcrel = rule->getSrc();
    Address *src =compiler->getFirstSrc(rule);
    assert(src);
    if (Firewall::cast(src)==NULL)  compiler->_expandAddr(rule, srcrel);
    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::expandMultipleAddressesIfNotFWinDst::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    RuleElementDst *dstrel=rule->getDst();
    Address        *dst   =compiler->getFirstDst(rule);  assert(dst);
    if (Firewall::cast(dst)==NULL)  compiler->_expandAddr(rule, dstrel);
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
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();

        if (Interface::cast(obj)!=NULL && Interface::cast(obj)->isLoopback())
        {
            FWObject *addr = obj->getFirstByType(IPv4::TYPENAME);
            if (addr==NULL)
                compiler->abort(_("Loopback interface of the firewall object does not have IP address but is used in the rule ")+rule->getLabel());
            rel->removeRef(obj);
            rel->addRef(addr);
            break;  // I guess there can be only one loopback object in the rule, right ? 
        }
    }
}

bool PolicyCompiler_ipt::expandLoopbackInterfaceAddress::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if ( ! rule->getStr("ipt_chain").empty() ||
         rule->getAction() == PolicyRule::Classify)
    {
        tmp_queue.push_back(rule);
        return true;
    }

    Address *src = compiler->getFirstSrc(rule);
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
         compiler->complexMatch(src,compiler->fw,false,false) )
    {
	PolicyRule *r;

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        ipt_comp->setChain(r,"FORWARD");

	tmp_queue.push_back(r);
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
        if (!src->isAny() && compiler->complexMatch(src, compiler->fw, b, m))
            ipt_comp->setChain(rule,"OUTPUT");
        break;

    case PolicyRule::Both:
/* direction == Both
 */
        if (!src->isAny() && compiler->complexMatch(src, compiler->fw, b, m)) 
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

bool PolicyCompiler_ipt::decideOnChainIfDstFW::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if ( ! rule->getStr("ipt_chain").empty() || 
         rule->getAction() == PolicyRule::Classify)
    {
        tmp_queue.push_back(rule);
        return true;
    }

    Address        *dst   =compiler->getFirstDst(rule);  assert(dst);

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
         compiler->complexMatch(dst,compiler->fw,false,false) )
    {
	PolicyRule *r;

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        ipt_comp->setChain(r,"FORWARD");

	tmp_queue.push_back(r);
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
        if (!dst->isAny() && compiler->complexMatch(dst,compiler->fw,b,m))
            ipt_comp->setChain(rule,"INPUT");
        break;

    case PolicyRule::Both:
/* direction == Both
 */
        if (!dst->isAny() && compiler->complexMatch(dst,compiler->fw,b,m)) 
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
        rule_iface!=NULL &&
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
 * target CLASSIFY is only valid in mangle table, chain POSTROUTING
 */
bool PolicyCompiler_ipt::decideOnChainForClassify::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if (rule->getAction() != PolicyRule::Classify)
    {
	tmp_queue.push_back(rule);
	return true;
    }

    if (rule->getStr("ipt_chain").empty())
        ipt_comp->setChain(rule,"POSTROUTING");

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::finalizeChain::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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

//    RuleElementSrc *srcrel=rule->getSrc();
      Address *src = compiler->getFirstSrc(rule);
      if (src==NULL)
          compiler->abort(
              string("finalizeChain: Empty Source rule element in rule ") +
              rule->getLabel());

//    RuleElementDst *dstrel=rule->getDst();
      Address        *dst   =compiler->getFirstDst(rule);
      if (dst==NULL)
          compiler->abort(
              string("finalizeChain: Empty Destination rule element in rule ") +
              rule->getLabel());

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
          if (!dst->isAny() && ipt_comp->complexMatch(dst,ipt_comp->fw,b,m))
              ipt_comp->setChain(rule,"INPUT");

          break;

      case PolicyRule::Outbound:
/* if direction is "Outbound", chain can never be INPUT, but could be FORWARD */
          if (!src->isAny() && ipt_comp->complexMatch(src,ipt_comp->fw,b,m))
              ipt_comp->setChain(rule,"OUTPUT");

          break;

      default:

/* direction == Both */
          if (!dst->isAny() && ipt_comp->complexMatch(dst,ipt_comp->fw,b,m)) 
          {
              ipt_comp->setChain(rule,"INPUT");
              break;
          }
          if (!src->isAny() && ipt_comp->complexMatch(src,ipt_comp->fw,b,m)) 
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    if ( ! rule->getStr("ipt_target").empty() ) return true; // already defined

    switch (rule->getAction()) {
    case PolicyRule::Accept:   rule->setStr("ipt_target","ACCEPT");    break;
    case PolicyRule::Deny:     rule->setStr("ipt_target","DROP");      break;
    case PolicyRule::Reject:   rule->setStr("ipt_target","REJECT");    break;
    case PolicyRule::Return:   rule->setStr("ipt_target","RETURN");    break;
    case PolicyRule::Tag:      rule->setStr("ipt_target","MARK");      break;
    case PolicyRule::Pipe:     rule->setStr("ipt_target","QUEUE");     break;
    case PolicyRule::Classify: rule->setStr("ipt_target","CLASSIFY");  break;
    case PolicyRule::Continue: rule->setStr("ipt_target","CONTINUE");  break;
    case PolicyRule::Custom:   rule->setStr("ipt_target","CUSTOM");    break;
    case PolicyRule::Route:    rule->setStr("ipt_target","ROUTE");     break;
    case PolicyRule::Branch:
    {
        RuleSet *ruleset = rule->getBranch();
        if (ruleset==NULL)
            compiler->abort(string("Branching rule ") + rule->getLabel() +
                            " refers ruleset that does not exist");
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    if (compiler->osconfigurator->getNumOfVirtualAddressesForNat()==0 &&
        ! rule->getBool("upstream_rule_neg") )
    {
        RuleElementSrc *srcrel = rule->getSrc();
        Address        *src    = compiler->getFirstSrc(rule);
        if (src==NULL) compiler->abort(
            string("removeFW: Empty Source rule element in rule ") +
            rule->getLabel());

        RuleElementDst *dstrel = rule->getDst();
        Address        *dst    = compiler->getFirstDst(rule);
        if (dst==NULL) compiler->abort(
            string("removeFW: Empty Destination rule element in rule ") +
            rule->getLabel());

        string chain = rule->getStr("ipt_chain");
        if (( chain=="INPUT" || 
              ipt_comp->isChainDescendantOfInput(chain)) &&
            dst->getId()==compiler->getFwId() ) 
        {
            dstrel->reset();
        }

        if (( chain=="OUTPUT" ||
              ipt_comp->isChainDescendantOfOutput(chain)) &&
            src->getId()==compiler->getFwId() ) 
        {
            srcrel->reset();
        }
    }
    return true;
}

bool PolicyCompiler_ipt::checkMACinOUTPUTChain::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    if ( rule->getStr("ipt_chain")=="OUTPUT" )
    {
//        RuleElementSrc *srcrel=rule->getSrc();
        Address        *src   =compiler->getFirstSrc(rule);  assert(src);

        if (physAddress::isA(src))
            compiler->abort("Can not match on MAC address of the firewall "
                            "in rule " + rule->getLabel());

        if (combinedAddress::isA(src))
        {
            compiler->warning("Can not match on MAC address of the firewall "
                              "(chain OUTPUT) "
                              "in rule " + rule->getLabel());
            combinedAddress::cast(src)->setPhysAddress("");
        }
    }

    return true;
}

bool PolicyCompiler_ipt::checkUserServiceInWrongChains::processNext()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    Service *srv = compiler->getFirstSrv(rule);  assert(srv);
    string chain = rule->getStr("ipt_chain");

    if (UserService::cast(srv) != NULL &&
        chain != "OUTPUT" &&
        !ipt_comp->isChainDescendantOfOutput(chain))
    {
        compiler->warning("Iptables does not support module 'owner' in a chain "
                          "other than OUTPUT; Rule " + rule->getLabel());
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipt::separateUserServices::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *rel= rule->getSrv();

    if (rel->size()==1) 
    {
	tmp_queue.push_back(rule);
	return true;
    }

    list<Service*> services;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
    {	    
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	Service *s=Service::cast(o);
	assert(s!=NULL);

	if (UserService::isA(s))
        {
            PolicyRule *r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            RuleElementSrv *nsrv=r->getSrv();
            nsrv->clearChildren();
            nsrv->addRef( s );
            tmp_queue.push_back(r);
            services.push_back(s);
        }
    }
    for (list<Service*>::iterator i=services.begin(); i!=services.end(); i++) 
	rel->removeRef( (*i) );

    if (!rel->isAny())
	tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::separatePortRanges::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *rel= rule->getSrv();

    if (rel->size()==1) 
    {
	tmp_queue.push_back(rule);
	return true;
    }

    list<Service*> services;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
    {	    
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	Service *s=Service::cast(o);
	assert(s!=NULL);

	if ( TCPService::isA(s) || UDPService::isA(s) ) 
        {
            unsigned srs=TCPUDPService::cast(s)->getSrcRangeStart();
            unsigned sre=TCPUDPService::cast(s)->getSrcRangeEnd();
            unsigned drs=TCPUDPService::cast(s)->getDstRangeStart();
            unsigned dre=TCPUDPService::cast(s)->getDstRangeEnd();

            if (srs!=0 && sre==0) sre=srs;
            if (drs!=0 && dre==0) dre=drs;

/*
 * I also need to separate rules that use "Any UDP" and "Any TCP"
 * objects. These objects have all ports set to zero and iptables code
 * for them should just have "-p udp" or "-p tcp" without any
 * "--source-port" or "--destination-port" specification. Commands
 * like this do not combine with commands that do specify port because
 * they lose their "any udp"/"any tcp" meaning as soon as
 * "--source-port"/"--destination-port" is added.
 */
            if (srs==0 && sre==0 && drs==0 && dre==0) { sre=65535; dre=65535; }

            if (srs!=sre || drs!=dre) 
            {
                PolicyRule *r= compiler->dbcopy->createPolicyRule();
                compiler->temp_ruleset->add(r);
                r->duplicate(rule);
                RuleElementSrv *nsrv=r->getSrv();
                nsrv->clearChildren();
                nsrv->addRef( s );
                tmp_queue.push_back(r);
                services.push_back(s);
            } 
        }
    }
    for (list<Service*>::iterator i=services.begin(); i!=services.end(); i++) 
	rel->removeRef( (*i) );

    if (!rel->isAny())
	tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::separateSrcPort::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *rel= rule->getSrv();

    if (rel->size()==1) {
	tmp_queue.push_back(rule);
	return true;
    }

    list<Service*> services;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) {
	    
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	Service *s=Service::cast(o);
	assert(s!=NULL);

	if ( TCPService::isA(s) || UDPService::isA(s) )
        {
            int srs=TCPUDPService::cast(s)->getSrcRangeStart();
            int sre=TCPUDPService::cast(s)->getSrcRangeEnd();

            compiler->normalizePortRange(srs,sre);

            if (srs!=0 || sre!=0) {
                PolicyRule *r= compiler->dbcopy->createPolicyRule();
                compiler->temp_ruleset->add(r);
                r->duplicate(rule);
                RuleElementSrv *nsrv=r->getSrv();
                nsrv->clearChildren();
                nsrv->addRef( s );
                tmp_queue.push_back(r);
                services.push_back(s);
            } 
        }
    }
    for (list<Service*>::iterator i=services.begin(); i!=services.end(); i++) 
	rel->removeRef( (*i) );

    if (!rel->isAny())
	tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::fillActionOnReject::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    FWOptions  *ruleopt =rule->getOptionsObject();
    string s=ruleopt->getStr("action_on_reject");
    if (s.empty()) ruleopt->setStr("action_on_reject",
                     ipt_comp->getCachedFwOpt()->getStr("action_on_reject"));

    tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipt::expandGroupsInSrv::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *srv= rule->getSrv();

    compiler->expandGroupsInRuleElement(srv);

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_ipt::splitRuleIfSrvAnyActionReject::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
                    "Rule action 'Reject' with TCP RST can be used only with TCP services. Rule " + rule->getLabel());
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
 * processor splitServices should have been called eariler, so now all
 * services in Srv are of the same type
 */
bool PolicyCompiler_ipt::prepareForMultiport::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *rel= rule->getSrv();
    Service        *srv= compiler->getFirstSrv(rule);

    if (rel->size()==1) {
	tmp_queue.push_back(rule);
	return true;
    }

    if (IPService::isA(srv) ||
        ICMPService::isA(srv) ||
        ICMP6Service::isA(srv) ||
        CustomService::isA(srv) ||
        TagService::isA(srv))
    {
/* multiport does  not support ip and icmp services, split the rule */
	for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
        {
	    FWObject *o= *i;
	    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	    
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
	    RuleElementSrv *nsrv = NULL;
	    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
            {
		FWObject *o= *i;
		if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	    
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
                assert(nsrv!=NULL);
		nsrv->addRef( s );
		if (++n>=15) n=0;
	    }

	} else {
	    tmp_queue.push_back(rule);
	}
    }

    return true;
}

/*
 *  processor splitServices should have been called before, it makes sure
 *  all objects in Service are of the same type.
 *
 *  One special case is custom service "ESTABLISHED". This processor
 *  splits rule if it finds this service and turns off stateful
 *  inspection on the rule.
 *
 */
bool PolicyCompiler_ipt::specialCasesWithCustomServices::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        assert(o!=NULL);
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    FWOptions  *ruleopt =rule->getOptionsObject();
    PolicyRule *r;

    if ( ! ruleopt->getBool("firewall_is_part_of_any_and_networks") )
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    OSConfigurator_linux24 *osconf = 
        dynamic_cast<OSConfigurator_linux24*>(compiler->osconfigurator);

    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );

    if (re->size()==1) 
    {
        FWObject *o = re->front();
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
        if (atrt!=NULL)
        {
            // we have just one object in RE and this object is MutiAddressRunTime
            if (atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            {
                rule->setStr("address_table_file",atrt->getSourceName());
                osconf->registerMultiAddressObject(atrt);
            }
            if (atrt->getSubstitutionTypeName()==DNSName::TYPENAME)
            {
                // this is DNSName converted to its run-time counterpart,
                // we do not need to touch it at all
            }
            tmp_queue.push_back(rule);
            return true;
        }
    }

    list<MultiAddressRunTime*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
        if (atrt!=NULL && atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            cl.push_back(atrt);
    }

    if (!cl.empty())
    {
        RuleElement *nre;
        RuleElement *ore=RuleElement::cast( rule->getFirstByType(re_type) );
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
            r->setStr("address_table_file",atrt->getSourceName());
            osconf->registerMultiAddressObject(atrt);
            tmp_queue.push_back(r);

            ore->removeRef( *i );
        }
    }

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    Interface  *rule_iface = compiler->getCachedFwInterface(rule->getInterfaceId());
    FWOptions  *ruleopt =rule->getOptionsObject();

    if (rule->getAction()==PolicyRule::Accounting &&
        rule->getStr("ipt_target").empty())
    {
        string this_chain = rule->getStr("ipt_chain");
	string new_chain  = ipt_comp->getNewChainName(rule,rule_iface);
        string rule_name_accounting = ruleopt->getStr("rule_name_accounting");
        if (!rule_name_accounting.empty())
            new_chain = rule_name_accounting;

        if (new_chain==this_chain)
        {
            rule->setStr("ipt_target","RETURN");
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

            r->setStr("ipt_target","RETURN");
            r->setLogging(false);
            r->setAction(PolicyRule::Continue);
            tmp_queue.push_back(r);

            rule->setStr("ipt_target", new_chain);
            rule->setLogging(false);
            ruleopt =rule->getOptionsObject();
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



bool PolicyCompiler_ipt::addPredefinedRules::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
    printRule=NULL;
    string version = fw->getStr("version");

    cout << " Compiling ruleset " << getRuleSetName() << " for '" << my_table << "' table";
    if (ipv6) cout << ", IPv6";
    cout <<  endl << flush;


    Compiler::compile();
    bool check_for_recursive_groups=true;

    addPredefinedPolicyRules();

    if ( fw->getOptionsObject()->getBool ("check_shading") ) 
    {
        add( new Begin("Detecting rule shadowing"));

        addRuleFilter();

        add( new printTotalNumberOfRules());

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
 * virtual method _expandInterface  */
        add( new ExpandMultipleAddressesInSRC(
                 "expand objects with multiple addresses in SRC"));
        add( new ExpandMultipleAddressesInDST(
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
            
        if (my_table=="mangle" &&
            !fw->getOptionsObject()->getBool("classify_mark_terminating")
        )
        {
            add( new dropTerminatingTargets(
                     "Drop rules with terminating targets") );
            add( new DetectShadowingForNonTerminatingRules(
                     "Detect shadowing for non-terminating rules" ) );
        } else
            add( new DetectShadowing("Detect shadowing" ) );

        add( new simplePrintProgress() );

        runRuleProcessors();
        deleteRuleProcessors();
    }


    add( new PolicyCompiler::Begin());
    add( new addPredefinedRules("Add some predefined rules"));

//    add(new printRuleElements("print"));

    addRuleFilter();

    add( new printTotalNumberOfRules());

    add( new Route("process route rules"));
    add( new storeAction("store original action of this rule"));

    add( new splitIfTagAndConnmark("Tag+CONNMARK combo"));
    //add( new setChainForMangle("set chain for other rules in mangle"));

    add( new Logging1("check global logging override option"));
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
    add( new singleSrvNegation("negation in Srv if it holds 1 object"));
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


    add( new singleSrcNegation("negation in Src if it holds single object"));
    add( new singleDstNegation("negation in Dst if it holds single object"));

    add( new splitIfSrcNegAndFw("split rule if src has negation and fw"));
    add( new splitIfDstNegAndFw("split rule if dst has negation and fw"));

    add( new SrcNegation(false, "process negation in Src"  ));
    add( new DstNegation(false, "process negation in Dst"  ));
    add( new TimeNegation(false, "process negation in Time" ));

    add( new Logging2("process logging"));

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

    add( new splitIfSrcAny("split rule if src is any") );
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
    }

    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    add( new splitIfSrcMatchesFw("split rule if src matches FW"));
    add( new splitIfDstMatchesFw("split rule if dst matches FW"));

/*  at this point in all rules where firewall is in either src or dst,
 *  firewall is a single object in that rule element. Other rule
 *  elements may contain multiple objects yet
 */

    add( new specialCaseWithFW1(  "special case with firewall"      ) );

    add( new decideOnChainIfDstFW( "decide on chain if Dst has fw" ) );
    add( new splitIfSrcFWNetwork(
             "split rule if src has a net fw has interface on" ) );

    add( new decideOnChainIfSrcFW( "decide on chain if Src has fw" ) );
    add( new splitIfDstFWNetwork(
             "split rule if dst has a net fw has interface on" ) );

    add( new specialCaseWithFW2(
             "replace fw with its interfaces if src==dst==fw" ) );

/* behavior of processors ExpandMultiple... has been changed in the
 * virtual method expandInterface 
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

    if (ipv6)
        add( new DropIPv4Rules("drop ipv4 rules"));
    else
        add( new DropIPv6Rules("drop ipv6 rules"));
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

// trying process rules with multiple interfaces as late as possible
    add( new InterfacePolicyRulesWithOptimization(
             "process interface policy rules and store interface ids") );

/* this is just a patch for those who do not understand how does
 * "assume firewall is part of any" work. It also eliminates redundant
 * and useless rules in the FORWARD chain for rules assigned to a
 * loopback interface.
 */
    add( new decideOnChainIfLoopback("any-any rule on loopback"     ) );

//      add( new decideOnChainForClassify("set chain if action is Classify"));
    add( new finalizeChain( "decide on chain"   ) );
    add( new decideOnTarget( "decide on target"  ) );

    add( new checkForRestoreMarkInOutput(
             "check if we need -A OUTPUT -j CONNMARK --restore-mark"));

/*
 * removed call to processor removeFW to make changes for bug #685947:
 * "Rules with firewall object allow too much. "
 */
    add( new removeFW(                   "remove fw"              ) );

    add( new ExpandMultipleAddresses("expand multiple addresses"  ) );
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    add( new checkForUnnumbered("check for unnumbered interfaces" ) );
    add( new checkForDynamicInterfacesOfOtherObjects(
             "check for dynamic interfaces of other hosts and firewalls"));

    if ( fwopt->getBool("bridging_fw") ) 
        add( new bridgingFw("handle bridging firewall cases"));

    add( new specialCaseWithUnnumberedInterface(
             "check for a special cases with unnumbered interface" ) );

//        add( new splitServices(              "split on services"       ) );
//        add( new prepareForMultiport("prepare for multiport"           ) );

    add( new optimize1(        "optimization 1, pass 1"              ) );
    add( new optimize1(        "optimization 1, pass 2"              ) );
    add( new optimize1(        "optimization 1, pass 3"              ) );


    add( new splitServices("split on services"));
    add( new separateTCPWithFlags("split on TCP services with flags"));
    add( new verifyCustomServices("verify custom services"));
    add( new specialCasesWithCustomServices(
             "scpecial cases with some custom services"));
    add( new separatePortRanges("separate port ranges"));
    add( new separateUserServices("separate user services"));
    add( new separateSrcPort("split on TCP and UDP with source ports"));

//        add( new optimize1(        "optimization 1, pass 1"              ) );
//        add( new optimize1(        "optimization 1, pass 2"              ) );
    add( new optimize2(        "optimization 2"                        ) );
    add( new accounting(       "Accounting"                            ) );
    add( new prepareForMultiport("prepare for multiport"               ) );

    add( new splitNonTerminatingTargets(
             "split rules using non-terminating targets" ) );

    add( new ConvertToAtomicForAddresses(
             "convert to atomic rules by address elements") );

    add( new checkForZeroAddr("check for zero addresses") );
    add( new checkMACinOUTPUTChain("check for MAC in OUTPUT chain") );
    add( new checkUserServiceInWrongChains(
             "Check for UserSErvice ojects in chains other than OUTPUT"));

    add( new ConvertToAtomicForIntervals(
             "convert to atomic rules by interval element") );

    add( new SkipActionContinueWithNoLogging(
             "drop rules with action Continue") );
    add( new convertInterfaceIdToStr("prepare interface assignments") );
    add( new optimize3("optimization 3") );

    add( new countChainUsage("Count chain usage"));

    add( createPrintRuleProcessor() );

    add( new simplePrintProgress());

    runRuleProcessors();

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

    int iface_id = rule->getInterfaceId();
//    Interface *rule_iface = fw_interfaces[iface_id];

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
        string itf=" ";

        if (srcrel->getNeg()) src << "!";
        if (dstrel->getNeg()) dst << "!";
        if (srvrel->getNeg()) srv  = "!";
        if (intrel->getNeg()) time = "!";
        if (itfrel->getNeg()) itf  = "!";

        if (i1!=srcrel->end()) {
            FWObject *o=*i1;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            src << o->getName();
            if (Group::cast(o)!=NULL)
                src << "[" << o->size() << "]";
        }

        if (i2!=dstrel->end()) {
            FWObject *o=*i2;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            dst << o->getName();
            if (Group::cast(o)!=NULL)
                dst << "[" << o->size() << "]";
        }

        if (i3!=srvrel->end()) {
            FWObject *o=*i3;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            srv+=o->getName();
        }

        if (i4!=intrel->end()) {
            FWObject *o=*i4;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            time+=o->getName();
        }

        if (i5!=itfrel->end()) {
            FWObject *o=*i5;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            itf+=o->getName();
        }

        int w=0;
        if (no==0) {
            str << rule->getLabel();
            w=rule->getLabel().length();
        }
        
        str <<  setw(15-w)  << setfill(' ') << " ";

        str <<  setw(18) << setfill(' ') << src.str();
        str <<  setw(18) << setfill(' ') << dst.str();
        str <<  setw(12) << setfill(' ') << srv.c_str();
        str <<  setw(10) << setfill(' ') << time.c_str();
        str <<  setw(8)  << setfill(' ') << itf.c_str();

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
        if ( i5!=intrel->end() ) ++i5;
    }

    str << " c=" << printChains(rule);

    str << " t=" << rule->getStr("ipt_target")
        << " intfId=" << rule->getInterfaceId()
        << " intfstr=" << rule->getInterfaceStr();

    if (rule->getAction()==PolicyRule::Reject)
        str << " " + ruleopt->getStr("action_on_reject");

    if (ruleopt!=NULL && ruleopt->getInt("limit_value")>0)
        str << " limit";

    if (ruleopt!=NULL && ruleopt->getInt("connlimit_value")>0)
        str << " connlimit";

    if (ruleopt!=NULL && ruleopt->getInt("hashlimit_value")>0)
        str << " hashlimit";

    if (ruleopt!=NULL && ruleopt->getBool("no_input_chain"))
        str << " no_input";

    if (ruleopt!=NULL && ruleopt->getBool("no_output_chain"))
        str << " no_output";

    return str.str();
}

void PolicyCompiler_ipt::epilog()
{
    if (fwopt->getBool("use_iptables_restore") && getCompiledScriptLength()>0)
    {
        output << "#" << endl;
    }
}


PolicyCompiler_ipt::PrintRule* PolicyCompiler_ipt::createPrintRuleProcessor()
{
    if(printRule==NULL)
    {
        if (fwopt->getBool("use_iptables_restore"))
        {
            // bug #1812295: we should use PrintRuleIptRstEcho not only
            // when we have dynamic interfaces, but also when we have
            // address tables expanded at run time. Instead of checking
            // for all these conditions, just always use PrintRuleIptRstEcho
            printRule = new PrintRuleIptRstEcho(
                "generate code for iptables-restore using echo");
        } else
        {
            printRule = new PrintRule("generate shell script");
        }
        printRule->setContext(this);
    }
    return printRule;
}

string PolicyCompiler_ipt::flushAndSetDefaultPolicy()
{
    string res="";

    createPrintRuleProcessor();
    res += printRule->_declareTable();
    res += printRule->_flushAndSetDefaultPolicy();

    return res;
}

std::string PolicyCompiler_ipt::printAutomaticRules()
{
    string res="";
    // iptables accepted TCPMSS target in filter table, FORWARD chain 
    // in the older versions, but requires it to be in mangle filter
    // starting somewhere 1.3.x
    string version = fw->getStr("version");
    if (XMLTools::version_compare(version, "1.3.0")<0)
        res += printRule->_clampTcpToMssRule();

    res += printRule->_printOptionalGlobalRules();
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

bool PolicyCompiler_ipt::isMangleOnlyRuleSet(const string &ruleset_name)
{
    return (std::find(mangle_only_rulesets.begin(),
                      mangle_only_rulesets.end(),
                      ruleset_name) != mangle_only_rulesets.end());
}


