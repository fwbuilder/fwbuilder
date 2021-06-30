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


#include "PolicyCompiler_ipf.h"
#include "fwcompiler/Compiler.h"

#include "fwbuilder/AddressTable.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"

#include <iostream>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string PolicyCompiler_ipf::myPlatformName() { return "ipf"; }

int PolicyCompiler_ipf::prolog()
{
    int n= PolicyCompiler_pf::prolog();

    anytcp = dbcopy->createTCPService();
    anytcp->setId(FWObjectDatabase::generateUniqueId()); //ANY_TCP_OBJ_ID);
    persistent_objects->add(anytcp,false);

    anyudp=dbcopy->createUDPService();
    anyudp->setId(FWObjectDatabase::generateUniqueId()); //ANY_UDP_OBJ_ID);
    persistent_objects->add(anyudp,false);

    anyicmp=dbcopy->createICMPService();
    anyicmp->setId(FWObjectDatabase::generateUniqueId()); //ANY_ICMP_OBJ_ID);
    persistent_objects->add(anyicmp,false);

    return n;
}

bool PolicyCompiler_ipf::expandAnyService::processNext()
{
    PolicyCompiler_ipf *pcomp=dynamic_cast<PolicyCompiler_ipf*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrv *srv=rule->getSrv();
    FWOptions *ruleopt =rule->getOptionsObject();

    if (srv->isAny() && ! ruleopt->getBool("stateless") && rule->getAction()==PolicyRule::Accept) {

	PolicyRule *r = compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	RuleElementSrv *nsrv=r->getSrv();
	nsrv->clearChildren();
	nsrv->addRef(pcomp->anyicmp); //compiler->dbcopy->findInIndex(ANY_ICMP_OBJ_ID));
	tmp_queue.push_back(r);

	r = compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrv=r->getSrv();
	nsrv->clearChildren();
	nsrv->addRef(pcomp->anytcp); //compiler->dbcopy->findInIndex(ANY_TCP_OBJ_ID));
	tmp_queue.push_back(r);

	r = compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrv=r->getSrv();
	nsrv->clearChildren();
	nsrv->addRef(pcomp->anyudp); //compiler->dbcopy->findInIndex(ANY_UDP_OBJ_ID));
	tmp_queue.push_back(r);

	r = compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	FWOptions *ruleopt =r->getOptionsObject();
	ruleopt->setBool("stateless",true);
	tmp_queue.push_back(r);

    } else
	tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipf::doSrcNegation::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrc *src=rule->getSrc();

    if (src->getNeg()) {
        RuleElementSrc *nsrc;
	PolicyRule     *r;
        FWOptions *ruleopt;

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setAction(PolicyRule::Continue);
	r->setLogging(false);
        nsrc=r->getSrc();
        nsrc->setNeg(false);
	r->setBool("quick",false);
        r->setBool("skip_check_for_duplicates",true);
        ruleopt = r->getOptionsObject();
        ruleopt->setBool("stateless", true);
	tmp_queue.push_back(r);

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        nsrc=r->getSrc();
        nsrc->setNeg(false);
	nsrc->clearChildren();
	nsrc->setAnyElement();
	r->setBool("quick",true);
        r->setBool("skip_check_for_duplicates",true);
	tmp_queue.push_back(r);

	return true;
    }
    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipf::doDstNegation::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementDst *dst=rule->getDst();

    if (dst->getNeg()) {
        RuleElementDst *ndst;
	PolicyRule     *r;
        FWOptions *ruleopt;

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setAction(PolicyRule::Continue);
	r->setLogging(false);
        ndst=r->getDst();
        ndst->setNeg(false);
	r->setBool("quick",false);
        r->setBool("skip_check_for_duplicates",true);
        ruleopt = r->getOptionsObject();
        ruleopt->setBool("stateless", true);
	tmp_queue.push_back(r);

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        ndst=r->getDst();
        ndst->setNeg(false);
	ndst->clearChildren();
	ndst->setAnyElement();
	r->setBool("quick",true);
        r->setBool("skip_check_for_duplicates",true);
	tmp_queue.push_back(r);

	return true;
    }
    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipf::doSrvNegation::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrv *srv=rule->getSrv();

    if (srv->getNeg())
    {
	compiler->abort(rule, "Negation in Srv is not implemented");
	return false;
    }
    tmp_queue.push_back(rule);
    return true;
}

void PolicyCompiler_ipf::specialCaseWithDynInterface::dropDynamicInterface(RuleElement *re)
{
    list<FWObject*> cl;
    for (list<FWObject*>::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs   =Interface::cast( obj );

        if (ifs!=nullptr && !ifs->isRegular()) continue;
        cl.push_back(obj);
    }
    if (!cl.empty()) 
    {
        re->clearChildren();
        for (list<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
            re->addRef( (*i1) );
    }
}

bool PolicyCompiler_ipf::specialCaseWithDynInterface::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    dropDynamicInterface( rule->getDst() );
    dropDynamicInterface( rule->getSrc() );
    tmp_queue.push_back(rule);
    return true;
}


PolicyCompiler_ipf::calculateSkip::calculateSkip(const std::string &n) : PolicyRuleProcessor(n)
{
}

bool PolicyCompiler_ipf::calculateSkip::processNext()
{
//    PolicyRule *rule;

    slurp();
    if (tmp_queue.size()==0) return false;

/*
 * first, we scan all rules and build a hash that maps attribute
 * "skip_label" to rule number. Attribute "skip_label" is set in
 * optimize1, after which we could have split some rules, so this
 * attrbiute may not be unique. We want to skip to the first rule
 * marked with the same skip label if there are few with the same
 * label. The simplest way to find the first one with the same label
 * is to scan rules in reverse order, that is from the bottom up.
 */
    int N=tmp_queue.size()-1;  // The last rule number is N
    for (deque<Rule*>::reverse_iterator k=tmp_queue.rbegin(); k!=tmp_queue.rend(); ++k) 
    {
        PolicyRule *r = PolicyRule::cast( *k );
        if (!r->getStr("skip_label").empty()) allrules[r->getStr("skip_label")]=N;
        r->setInt("rule_num",N);
        N--;
    }

    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        PolicyRule *r = PolicyRule::cast( *k );
        string rl=r->getLabel();
        int    current_position=r->getPosition();

        if (r->getAction()==PolicyRule::Skip)
        {
            assert(!r->getStr("skip_to").empty());

            int to=allrules[r->getStr("skip_to")];
            int n =r->getInt("rule_num");
            r->setInt("no_to_skip",to-n-1);
        }
/* Action 'Continue' means we need to jump to the next rule in the
 * GUI. We scan rules down from the current one, looking for the first
 * rule that corresponds to the next rule in the GUI.
 */
        if (r->getAction()==PolicyRule::Continue) 
        {
            r->setAction(PolicyRule::Skip);
            r->setBool("quick",false);
            
            deque<Rule*>::iterator j=k;
            ++j;
            int n=0;
            for ( ; j!=tmp_queue.end(); ++j) 
            {
                PolicyRule *r2 = PolicyRule::cast( *j );
                if (r2->getPosition()!=current_position) break;
/* 'skip' only skips rules with the same setting of 'in' or 'out',
 * that is the same direction
 */
                if (r2->getDirection()==r->getDirection()) ++n;
            }
            r->setInt("no_to_skip",n);
        }
    }
    return true;
}

bool PolicyCompiler_ipf::checkForKeepState::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    Service   *srv=compiler->getFirstSrv(rule);    assert(srv);
    FWOptions *ruleopt =rule->getOptionsObject();

    if (! ICMPService::isA(srv) && 
        ! UDPService::isA(srv)  &&
        ! TCPService::isA(srv) )    ruleopt->setBool("stateless",true);

    return true;
}

bool PolicyCompiler_ipf::eliminateDuplicateRules::processNext()
{
    PolicyCompiler *pcomp = dynamic_cast<PolicyCompiler*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    // Note that if rule has "any" in Interface column, it is
    // implemented as reference to the AnyNetwork object. In this case
    // Compiler::getFirstItf() returns nullptr.
    Interface *intf_rule = compiler->getFirstItf(rule);
    int intf_id_rule = (intf_rule) ? intf_rule->getId() : -1;

    if ( ! rule->getBool("skip_check_for_duplicates"))
    {
        for (deque<PolicyRule*>::iterator i=rules_seen_so_far.begin(); i!=rules_seen_so_far.end(); ++i)
        {
            PolicyRule *r=(*i);
            if (r->getBool("skip_check_for_duplicates") ) continue;
            if (r->getAction()==PolicyRule::Continue ||
                r->getAction()==PolicyRule::Skip) continue;

            Interface *intf_r = compiler->getFirstItf(r);
            int intf_id_r = (intf_r) ? intf_r->getId() : -1;

            if (intf_id_r==intf_id_rule &&
                r->getAction()==rule->getAction()           &&
                r->getLogging()==rule->getLogging()         &&
                pcomp->cmpRules(*r,*rule) ) 
            {
//                cout << "---------------------------------------" << endl;
//                cout << pcomp->debugPrintRule(r) << endl;
//                cout << pcomp->debugPrintRule(rule) <<  endl;
                return true;
            }
        }
    }
    tmp_queue.push_back(rule);
    rules_seen_so_far.push_back(rule);

    return true;
}

bool PolicyCompiler_ipf::processMultiAddressObjectsInRE::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );

    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
        MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
        if (atrt!=nullptr && atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            compiler->abort(
                
                    rule, 
                    "Run-time AddressTable objects are not supported.");
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipf::SplitDirectionIpfilter::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getDirection()==PolicyRule::Both)
    {
	PolicyRule *r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setDirection(PolicyRule::Inbound);
	tmp_queue.push_back(r);

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setDirection(PolicyRule::Outbound);
	tmp_queue.push_back(r);

    } else 
	tmp_queue.push_back(rule);

    return true;
}



void PolicyCompiler_ipf::compile()
{
    info(" Compiling policy for " + fw->getName());

    Compiler::compile();

    bool check_for_recursive_groups=true;

    if ( fw->getOptionsObject()->getBool ("check_shading") &&
         ! inSingleRuleCompileMode())
    {
        add( new Begin ("Detecting rule shadowing"              ) );
        add( new printTotalNumberOfRules( ) );

        add( new ItfNegation("process negation in Itf"  ) );
        add( new InterfacePolicyRules(
                 "process interface policy rules and store interface ids"));

        add( new recursiveGroupsInSrc("check for recursive groups in SRC"));
        add( new recursiveGroupsInDst("check for recursive groups in DST"));
        add( new recursiveGroupsInSrv("check for recursive groups in SRV"));
        check_for_recursive_groups=false;

        add( new ExpandGroups("expand groups") );
        add( new eliminateDuplicatesInSRC("eliminate duplicates in SRC") );
        add( new eliminateDuplicatesInDST("eliminate duplicates in DST") );
        add( new eliminateDuplicatesInSRV("eliminate duplicates in SRV") );

        add( new swapMultiAddressObjectsInSrc(
                 " swap MultiAddress -> MultiAddressRunTime in Src") );
        add( new swapMultiAddressObjectsInDst(
                 " swap MultiAddress -> MultiAddressRunTime in Dst") );

        add( new ExpandMultipleAddressesInSrc(
                 "expand objects with multiple addresses in SRC" ) );
        add( new ExpandMultipleAddressesInDst(
                 "expand objects with multiple addresses in DST" ) );
        add( new ConvertToAtomic("convert to atomic rules" ) );

        add( new checkForObjectsWithErrors(
                 "check if we have objects with errors in rule elements"));

        add( new DetectShadowing("Detect shadowing" ) );
        add( new simplePrintProgress() );

        runRuleProcessors();
        deleteRuleProcessors();
    }


    add( new Begin());
    add( new printTotalNumberOfRules() );

    add( new singleRuleFilter());

//        add( new MACFiltering(                "verify for MAC address filtering"                ) );
    add( new setQuickFlag("set 'quick' flag") );

    if (check_for_recursive_groups)
    {
        add( new recursiveGroupsInSrc("check for recursive groups in SRC"));
        add( new recursiveGroupsInDst("check for recursive groups in DST"));
        add( new recursiveGroupsInSrv("check for recursive groups in SRV"));
    }

    add( new emptyGroupsInSrc("check for empty groups in SRC") );
    add( new emptyGroupsInDst("check for empty groups in DST") );
    add( new emptyGroupsInSrv("check for empty groups in SRV") );

    add( new ItfNegation("process negation in Itf"  ) );
    add( new InterfacePolicyRules(
             "process interface policy rules and store interface ids") );

    add( new doSrcNegation("process negation in Src") );
    add( new doDstNegation("process negation in Dst") );
    add( new doSrvNegation("process negation in Srv") );
    add( new ExpandGroups( "expand groups") );

    add( new CheckForTCPEstablished(
             "check for TCPService objects with flag \"established\"") );
    add( new CheckForUnsupportedUserService("check for user service") );
            
    add( new eliminateDuplicatesInSRC("eliminate duplicates in SRC") );
    add( new eliminateDuplicatesInDST("eliminate duplicates in DST") );
    add( new eliminateDuplicatesInSRV("eliminate duplicates in SRV") );

    add( new swapMultiAddressObjectsInSrc(
             " swap MultiAddress -> MultiAddressRunTime in Src") );
    add( new swapMultiAddressObjectsInDst(
             " swap MultiAddress -> MultiAddressRunTime in Dst") );

    add( new processMultiAddressObjectsInSrc(
             "process MultiAddress objects in Src") );
    add( new processMultiAddressObjectsInDst(
             "process MultiAddress objects in Dst") );

    add( new splitIfFirewallInSrc("split rule if firewall is in Src") );
    add( new splitIfFirewallInDst("split rule if firewall is in Dst") );
    add( new fillDirection("determine directions") );
    add( new SplitDirectionIpfilter("split rules with direction 'both'" ) );
    add( new ExpandMultipleAddresses(
             "expand objects with multiple addresses") );
    add( new checkForDynamicInterfacesOfOtherObjects(
             "check for dynamic interfaces of other hosts and firewalls" ));
    add( new MACFiltering("verify for MAC address filtering" ) );
    add( new checkForUnnumbered("check for unnumbered interfaces") );
    add( new specialCaseWithDynInterface(
             "check for a special cases with dynamic interface") );
    add( new addressRanges("expand address range objects") );
    add( new groupServicesByProtocol("split rules with different protocols") );
    add( new separateTCPWithFlags("separate TCP services with flags" ) );
    add( new separateSrcPort("split on TCP and UDP with source ports"));
    add( new verifyCustomServices(
             "verify custom services for this platform") );
    add( new SpecialServices("check for special services" ) );
    add( new expandAnyService("expand ANY service for stateful rules") );
/*
 * it may make sense to do optimization even before we expand groups
 * (before ExpandGroups). Need to test this idea.
 */
    if ( fw->getOptionsObject()->getBool ("optimize") )
    {
        add( new optimizeSrc("optimization in SRC") );
        add( new optimizeDst("optimization in DST") );
        add( new optimizeSrv("optimization in SRV") );
    }

    add( new ConvertToAtomic("convert to atomic rules") );
    add( new checkForZeroAddr("check for zero addresses" ) );

    if ( fw->getOptionsObject()->getBool ("eliminate_duplicates") ) 
        add( new eliminateDuplicateRules("eliminate duplicate rules" ) );

    add( new calculateSkip("calculate argument for skip") );
    add( new checkForKeepState("check for 'keep state'") );

    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));

    add( new PrintRule("generate ipf code") );
    add( new simplePrintProgress() );

    runRuleProcessors();

}

string PolicyCompiler_ipf::debugPrintRule(Rule *r)
{
    PolicyRule *rule=PolicyRule::cast(r);
//    FWOptions  *ruleopt =rule->getOptionsObject();

    ostringstream s;
    s << PolicyCompiler::debugPrintRule(rule) << " ";

    RuleElementItf *intf_re = rule->getItf();
    string rule_interfaces;
    int intf_count = 0;
    for (FWObject::iterator it=intf_re->begin(); it!=intf_re->end(); ++it)
    {
        FWObject *o   = *it;
        if (FWReference::cast(o)!=nullptr) o = FWReference::cast(o)->getPointer();
        rule_interfaces += " " + o->getName();
        intf_count++;
    }
    if (intf_count > 0)
    {
        s << " intf: ";
        if (intf_count > 1) s << "{ ";
        s << rule_interfaces;
        if (intf_count > 1) s << " }";
    }
    else
        s << " intf: ?";

    s << " ";

    if (r->getBool("skip_check_for_duplicates")) s << "skip_check_for_duplicates ";
    if (r->getStr("skip_label")!="")             s << "skip_label: " << r->getStr("skip_label") << " ";
    if (r->getStr("skip_to")!="")                s << "skip_to: "    << r->getStr("skip_to")    << " ";
    if (r->getInt("no_to_skip")!=-1)             s << "no_to_skip: " << r->getInt("no_to_skip");

    s << " " << FWObjectDatabase::getStringId(r->getId()) << " (" << r->getId() << ")";

    return s.str();
}


void PolicyCompiler_ipf::epilog()
{
}
