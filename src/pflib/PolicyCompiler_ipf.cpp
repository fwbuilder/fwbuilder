/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: PolicyCompiler_ipf.cpp 1303 2007-05-08 02:11:39Z vkurland $

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

#include "PolicyCompiler_ipf.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/AddressTable.h"

#include <iostream>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string PolicyCompiler_ipf::myPlatformName() { return "ipf"; }

int PolicyCompiler_ipf::prolog()
{
    int n= PolicyCompiler_pf::prolog();

//    FWObject    *grp;
    anytcp=TCPService::cast(dbcopy->create(TCPService::TYPENAME) );
    anytcp->setId(ANY_TCP_OBJ_ID);
//    grp=dbcopy->getById( dbcopy->std.TCPServicesId , true );
//    assert(grp!=NULL);
    dbcopy->add(anytcp,false);
    cacheObj(anytcp); // to keep cache consistent

    anyudp=UDPService::cast(dbcopy->create(UDPService::TYPENAME) );
    anyudp->setId(ANY_UDP_OBJ_ID);
//    grp=dbcopy->getById( dbcopy->std.UDPServicesId , true );
//    assert(grp!=NULL);
    dbcopy->add(anyudp,false);
    cacheObj(anyudp); // to keep cache consistent

    anyicmp=ICMPService::cast(dbcopy->create(ICMPService::TYPENAME) );
    anyicmp->setId(ANY_ICMP_OBJ_ID);
//    grp=dbcopy->getById( dbcopy->std.ICMPServicesId , true );
//    assert(grp!=NULL);
    dbcopy->add(anyicmp,false);
    cacheObj(anyicmp); // to keep cache consistent


    return n;
}

bool PolicyCompiler_ipf::expandAnyService::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *srv=rule->getSrv();
    FWOptions *ruleopt =rule->getOptionsObject();

    if (srv->isAny() && ! ruleopt->getBool("stateless") && rule->getAction()==PolicyRule::Accept) {

	PolicyRule *r= PolicyRule::cast(
	    compiler->dbcopy->create(PolicyRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	RuleElementSrv *nsrv=r->getSrv();
	nsrv->clearChildren();
	nsrv->addRef(compiler->dbcopy->findInIndex(ANY_ICMP_OBJ_ID));
	tmp_queue.push_back(r);

	r= PolicyRule::cast(
	    compiler->dbcopy->create(PolicyRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrv=r->getSrv();
	nsrv->clearChildren();
	nsrv->addRef(compiler->dbcopy->findInIndex(ANY_TCP_OBJ_ID));
	tmp_queue.push_back(r);

	r= PolicyRule::cast(
	    compiler->dbcopy->create(PolicyRule::TYPENAME) );
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrv=r->getSrv();
	nsrv->clearChildren();
	nsrv->addRef(compiler->dbcopy->findInIndex(ANY_UDP_OBJ_ID));
	tmp_queue.push_back(r);

	r= PolicyRule::cast(
	    compiler->dbcopy->create(PolicyRule::TYPENAME) );
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrc *src=rule->getSrc();

    if (src->getNeg()) {
        RuleElementSrc *nsrc;
	PolicyRule     *r;
        FWOptions *ruleopt;

	r= PolicyRule::cast(compiler->dbcopy->create(PolicyRule::TYPENAME) );
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

	r= PolicyRule::cast(compiler->dbcopy->create(PolicyRule::TYPENAME) );
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementDst *dst=rule->getDst();

    if (dst->getNeg()) {
        RuleElementDst *ndst;
	PolicyRule     *r;
        FWOptions *ruleopt;

	r= PolicyRule::cast(compiler->dbcopy->create(PolicyRule::TYPENAME) );
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

	r= PolicyRule::cast(compiler->dbcopy->create(PolicyRule::TYPENAME) );
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *srv=rule->getSrv();

    if (srv->getNeg()) {
	throw FWException(_("Negation in Srv is not implemented. Rule: ")+rule->getLabel());
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
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs   =Interface::cast( obj );

        if (ifs!=NULL && !ifs->isRegular()) continue;
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
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
    PolicyCompiler *pcomp=dynamic_cast<PolicyCompiler*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if ( ! rule->getBool("skip_check_for_duplicates"))
    {
        for (deque<PolicyRule*>::iterator i=rules_seen_so_far.begin(); i!=rules_seen_so_far.end(); ++i)
        {
            PolicyRule *r=(*i);
            if ( r->getBool("skip_check_for_duplicates") ) continue;
            if (r->getInterfaceId()==rule->getInterfaceId() &&
                r->getAction()==rule->getAction()           &&
                r->getAction()!=PolicyRule::Continue        &&
                r->getAction()!=PolicyRule::Skip            &&
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );

    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
        if (atrt!=NULL && atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            compiler->abort("Run-time AddressTable objects are not supported. Rule " + rule->getLabel());
    }

    tmp_queue.push_back(rule);
    return true;
}


void PolicyCompiler_ipf::compile()
{
    cout << " Compiling policy for " << fw->getName() << " ..." <<  endl << flush;

    try {

	Compiler::compile();

	addDefaultPolicyRule();
        bool check_for_recursive_groups=true;

        if ( fw->getOptionsObject()->getBool ("check_shading") ) 
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

            add( new ExpandMultipleAddressesInSRC(
                     "expand objects with multiple addresses in SRC" ) );
            add( new ExpandMultipleAddressesInDST(
                     "expand objects with multiple addresses in DST" ) );
            add( new ConvertToAtomic("convert to atomic rules" ) );
            add( new DetectShadowing("Detect shadowing" ) );
            add( new simplePrintProgress() );

            runRuleProcessors();
            deleteRuleProcessors();
        }


        add( new Begin());
        add( new printTotalNumberOfRules() );
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
        add( new SplitDirection("split rules with direction 'both'" ) );
        add( new ExpandMultipleAddresses(
                 "expand objects with multiple addresses") );
        add( new checkForDynamicInterfacesOfOtherObjects(
                 "check for dynamic interfaces of other hosts and firewalls" ));
        add( new MACFiltering("verify for MAC address filtering" ) );
        add( new checkForUnnumbered("check for unnumbered interfaces") );
        add( new specialCaseWithDynInterface(
                 "check for a special cases with dynamic interface") );
        add( new addressRanges("expand address range objects") );
        add( new splitServices("split rules with different protocols") );
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
        add( new convertInterfaceIdToStr("prepare interface assignments") );
        add( new PrintRule("generate ipf code") );
        add( new simplePrintProgress() );

        runRuleProcessors();


    } catch (FWException &ex) {
	error(ex.toString());
	exit(1);
    }
}

string PolicyCompiler_ipf::debugPrintRule(Rule *r)
{
    PolicyRule *rule=PolicyRule::cast(r);
//    FWOptions  *ruleopt =rule->getOptionsObject();

    string  iface = rule->getInterfaceId();
    if (iface!="") {
	Interface *rule_iface = getCachedFwInterface( iface );
        iface=" intf: "+rule_iface->getName();
    }
    ostringstream s;
    s << PolicyCompiler::debugPrintRule(rule)+" "+iface;
    if (r->getBool("skip_check_for_duplicates")) s << "skip_check_for_duplicates ";
    if (r->getStr("skip_label")!="")             s << "skip_label: " << r->getStr("skip_label") << " ";
    if (r->getStr("skip_to")!="")                s << "skip_to: "    << r->getStr("skip_to")    << " ";
    if (r->getInt("no_to_skip")!=-1)             s << "no_to_skip: " << r->getInt("no_to_skip");
    return s.str();
}


void PolicyCompiler_ipf::epilog()
{
}
