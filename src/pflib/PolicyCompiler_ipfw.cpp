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


#include "PolicyCompiler_ipfw.h"
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

string PolicyCompiler_ipfw::myPlatformName() { return "ipfw"; }

int PolicyCompiler_ipfw::prolog()
{
    int n= PolicyCompiler_pf::prolog();

    anytcp=dbcopy->createTCPService();
    anytcp->setId(FWObjectDatabase::generateUniqueId()); // ANY_TCP_OBJ_ID);
    persistent_objects->add(anytcp,false);

    anyudp=dbcopy->createUDPService();
    anyudp->setId(FWObjectDatabase::generateUniqueId()); //ANY_UDP_OBJ_ID);
    persistent_objects->add(anyudp,false);

    anyicmp=dbcopy->createICMPService();
    anyicmp->setId(FWObjectDatabase::generateUniqueId()); //ANY_ICMP_OBJ_ID);
    persistent_objects->add(anyicmp,false);


    return n;
}

/*
 *  (this is a virtual method). We do not want to expand a firewall
 *  object that own the policy we are processing, because we can use
 *  address 'me' in ipfw rules.
 */
void PolicyCompiler_ipfw::_expand_addr(Rule *rule, FWObject *s, bool expand_cluster_interfaces_fully) 
{
    RuleElement *re=RuleElement::cast(s);

    if (re!=nullptr && re->size()==1 )
    {
        FWObject *o=re->front();
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

        if (o->getId()==fw->getId()) return;
    }
    Compiler::_expand_addr(rule, s, expand_cluster_interfaces_fully);
}

bool PolicyCompiler_ipfw::expandAnyService::processNext()
{
    PolicyCompiler_ipfw *pcomp=dynamic_cast<PolicyCompiler_ipfw*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrv *srv=rule->getSrv();
    FWOptions *ruleopt =rule->getOptionsObject();

    if (srv->isAny() && ! ruleopt->getBool("stateless") && rule->getAction()==PolicyRule::Accept) 
    {
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

bool PolicyCompiler_ipfw::SpecialRuleActionsForShadowing::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getAction()==PolicyRule::Pipe ||
        rule->getAction()==PolicyRule::Custom) 
        return true;

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_ipfw::doSrcNegation::processNext()
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

bool PolicyCompiler_ipfw::doDstNegation::processNext()
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

bool PolicyCompiler_ipfw::doSrvNegation::processNext()
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

bool PolicyCompiler_ipfw::separatePortRanges::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElementSrv *rel= rule->getSrv();

    if (rel->size()==1) 
    {
	tmp_queue.push_back(rule);
	return true;
    }

    list<Service*> services;   
    bool sawServiceWithPortRange=false;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
    {	    
	FWObject *o= *i;
	if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
	Service *s=Service::cast(o);
	assert(s!=nullptr);

	if ( TCPService::isA(s) || UDPService::isA(s) ) 
        {
            unsigned srs=TCPUDPService::cast(s)->getSrcRangeStart();
            unsigned sre=TCPUDPService::cast(s)->getSrcRangeEnd();
            unsigned drs=TCPUDPService::cast(s)->getDstRangeStart();
            unsigned dre=TCPUDPService::cast(s)->getDstRangeEnd();

            if (srs!=0 && sre==0) sre=srs;
            if (drs!=0 && dre==0) dre=drs;

            if (srs!=sre || drs!=dre)
            {
                /* leave the very first service with port range in this rule,
                 * split others into separate rules
                 */
                if (sawServiceWithPortRange)
                {
                    PolicyRule *r = compiler->dbcopy->createPolicyRule();
                    compiler->temp_ruleset->add(r);
                    r->duplicate(rule);
                    RuleElementSrv *nsrv=r->getSrv();
                    nsrv->clearChildren();
                    nsrv->addRef( s );
                    tmp_queue.push_back(r);
                    services.push_back(s);
                }
                sawServiceWithPortRange=true;
            } 
        }
    }
    for (list<Service*>::iterator i=services.begin(); i!=services.end(); i++) 
	rel->removeRef( (*i) );

    if (!rel->isAny())
	tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler_ipfw::sortTCPUDPServices::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElementSrv *rel= rule->getSrv();

    if (rel->size()==1) 
    {
	tmp_queue.push_back(rule);
	return true;
    }

    FWObject *o=rel->front();
    if (o && FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

    Service *s1= Service::cast(o);
    if ( !UDPService::isA(s1) && !TCPService::isA(s1))
    {
	tmp_queue.push_back(rule);
	return true;
    }

/*
 * we know that at this point if there the original rule had service
 * objects with port ranges, there is only one left. We just need to
 * move it to the front of the list.
 */
    Service *portRangeSvc=nullptr;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
    {	    
	FWObject *o= *i;
	if (FWReference::cast(o)!=nullptr)
            o=FWReference::cast(o)->getPointer();
	Service *s=Service::cast(o);
	assert(s!=nullptr);

        unsigned srs=TCPUDPService::cast(s)->getSrcRangeStart();
        unsigned sre=TCPUDPService::cast(s)->getSrcRangeEnd();
        unsigned drs=TCPUDPService::cast(s)->getDstRangeStart();
        unsigned dre=TCPUDPService::cast(s)->getDstRangeEnd();

        if (srs!=0 && sre==0) sre=srs;
        if (drs!=0 && dre==0) dre=drs;

        if (srs!=sre || drs!=dre)
        {
            portRangeSvc=s;
            break;
        } 
    }

    if (portRangeSvc)
    {
        rel->removeRef(portRangeSvc);

/* It certainly would have been better if we had FWObject::insertRef() */
        FWReference *oref = portRangeSvc->createRef();
        portRangeSvc->ref();

        rel->push_front(oref);
        oref->setParent(rel);
    }

    tmp_queue.push_back(rule);
    return true;
}

void PolicyCompiler_ipfw::specialCaseWithDynInterface::dropDynamicInterface(RuleElement *re)
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

bool PolicyCompiler_ipfw::specialCaseWithDynInterface::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    dropDynamicInterface( rule->getDst() );
    dropDynamicInterface( rule->getSrc() );
    tmp_queue.push_back(rule);
    return true;
}


PolicyCompiler_ipfw::calculateNum::calculateNum(const std::string &n) :
    PolicyRuleProcessor(n) {}

bool PolicyCompiler_ipfw::calculateNum::processNext()
{
    PolicyCompiler_ipfw *pcomp = dynamic_cast<PolicyCompiler_ipfw*>(compiler);

    slurp();
    if (tmp_queue.size()==0) return false;

    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        PolicyRule *r = PolicyRule::cast( *k );
        pcomp->ipfw_num += 10;
        r->setInt("ipfw_num", pcomp->ipfw_num );
    }

    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        PolicyRule *r = PolicyRule::cast( *k );
        int current_position = r->getPosition();
        if (r->getAction()==PolicyRule::Continue) 
        {
            r->setAction(PolicyRule::Skip);
            
            deque<Rule*>::iterator j = k;
            ++j;
            PolicyRule *r2;
            for ( ; j!=tmp_queue.end(); ++j) 
            {
                r2 = PolicyRule::cast( *j );

                if (r2->getPosition()!=current_position)
                {
                    r->setInt("skip_to", r2->getInt("ipfw_num") );
                    break;
                }
            }
        }
    }

    return true;
}

bool PolicyCompiler_ipfw::checkForKeepState::processNext()
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

bool PolicyCompiler_ipfw::eliminateDuplicateRules::processNext()
{
    PolicyCompiler *pcomp=dynamic_cast<PolicyCompiler*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

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
            if ( r->getBool("skip_check_for_duplicates") ) continue;

            Interface *intf_r = compiler->getFirstItf(r);
            int intf_id_r = (intf_r) ? intf_r->getId() : -1;

            if (intf_id_r==intf_id_rule &&
                r->getAction()==rule->getAction()   &&
                r->getLogging()==rule->getLogging() &&
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

/*
 * this processor is the same as in PolicyCompiler_ipf
 */
bool PolicyCompiler_ipfw::processMultiAddressObjectsInRE::processNext()
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


void PolicyCompiler_ipfw::compile()
{
    string banner = " Compiling " + fw->getName();
    if (!getRuleSetName().empty())  banner += " ruleset " + getRuleSetName();
    if (ipv6) banner += ", IPv6";
    info(banner);

    Compiler::compile();

    bool check_for_recursive_groups=true;

    if ( fw->getOptionsObject()->getBool("check_shading") &&
         ! inSingleRuleCompileMode())
    {
        add( new Begin("Detecting rule shadowing"));
        add( new printTotalNumberOfRules());

        add( new SpecialRuleActionsForShadowing(
                 "disable rules with action Pipe and Custom") );
        add( new ItfNegation("process negation in Itf"  ) );
        add( new InterfacePolicyRules(
                 "process interface policy rules and store interface ids"));

        add( new recursiveGroupsInSrc("check for recursive grps in SRC"));
        add( new recursiveGroupsInDst("check for recursive grps in DST"));
        add( new recursiveGroupsInSrv("check for recursive grps in SRV"));
        check_for_recursive_groups=false;

        add( new ExpandGroups("expand groups"));
        add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));
        add( new eliminateDuplicatesInSRC("eliminate duplicates in SRC"));
        add( new eliminateDuplicatesInDST("eliminate duplicates in DST"));
        add( new eliminateDuplicatesInSRV("eliminate duplicates in SRV"));

        add( new swapMultiAddressObjectsInSrc(
                 " swap MultiAddress -> MultiAddressRunTime in Src") );
        add( new swapMultiAddressObjectsInDst(
                 " swap MultiAddress -> MultiAddressRunTime in Dst") );

        add( new ExpandMultipleAddressesInSrc(
                 "expand objects with multiple addresses in SRC"));
        add( new ExpandMultipleAddressesInDst(
                 "expand objects with multiple addresses in DST"));
        add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));
        add( new ConvertToAtomic("convert to atomic rules"));

        add( new checkForObjectsWithErrors(
                 "check if we have objects with errors in rule elements"));

        add( new DetectShadowing("Detect shadowing"));
        add( new simplePrintProgress());

        runRuleProcessors();
        deleteRuleProcessors();
    }


    add( new Begin());
    add( new printTotalNumberOfRules());

    add( new singleRuleFilter());

    if (check_for_recursive_groups)
    {
        add( new recursiveGroupsInSrc("check for recursive grps in SRC"));
        add( new recursiveGroupsInDst("check for recursive grps in DST"));
        add( new recursiveGroupsInSrv("check for recursive grps in SRV"));
    }

    add( new emptyGroupsInSrc("check for empty grps in SRC"));
    add( new emptyGroupsInDst("check for empty grps in DST"));
    add( new emptyGroupsInSrv("check for empty grps in SRV"));

    add( new ItfNegation("process negation in Itf"));
    add( new InterfacePolicyRules(
             "process interface policy rules and store interface ids"));

    add( new doSrcNegation("process negation in Src"));
    add( new doDstNegation("process negation in Dst"));
    add( new doSrvNegation("process negation in Srv"));
    add( new ExpandGroups("expand groups"));
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));
    add( new eliminateDuplicatesInSRC("eliminate duplicates in SRC"));
    add( new eliminateDuplicatesInDST("eliminate duplicates in DST"));
    add( new eliminateDuplicatesInSRV("eliminate duplicates in SRV"));

    add( new swapMultiAddressObjectsInSrc(
             " swap MultiAddress -> MultiAddressRunTime in Src") );
    add( new swapMultiAddressObjectsInDst(
             " swap MultiAddress -> MultiAddressRunTime in Dst") );

    add( new processMultiAddressObjectsInSrc(
             "process MultiAddress objects in Src") );
    add( new processMultiAddressObjectsInDst(
             "process MultiAddress objects in Dst") );

    add( new splitIfFirewallInSrc("split rule if firewall is in Src"));
    add( new splitIfFirewallInDst("split rule if firewall is in Dst"));
    add( new fillDirection("determine directions"));
    add( new ExpandMultipleAddresses(
             "expand objects with multiple addresses"));
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));
    add( new checkForDynamicInterfacesOfOtherObjects(
             "check for dynamic interfaces of other hosts and firewalls"));
    add( new MACFiltering("verify for MAC address filtering"));
    add( new checkForUnnumbered("check for unnumbered interfaces"));
    add( new specialCaseWithDynInterface(
             "check for a special cases with dynamic interface"));
    add( new addressRanges("expand address range objects"));
    add( new groupServicesByProtocol("split rules with different protocols"));
    add( new splitIpOptions("split rules with multiple IPService objects with options"));
    add( new separateTCPWithFlags("separate TCP services with flags"));
    add( new separateSrcPort("split on TCP and UDP with source ports"));
    add( new separatePortRanges("split services with port ranges"));
    add( new sortTCPUDPServices("move port ranges to the front of ports"));
    add( new verifyCustomServices(
             "verify custom services for this platform"));
    add( new SpecialServices("check for special services"));
//        add( new expandAnyService("expand ANY service for stateful rules"));
    add( new ConvertToAtomicForAddresses(
             "convert to atomic rules in SRC and DST"));
    add( new checkForZeroAddr("check for zero addresses"));

    add( new calculateNum("calculate rule numbers "));

    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));

    add( new PrintRule("generate ipf code"));
    add( new simplePrintProgress());

    runRuleProcessors();

}

string PolicyCompiler_ipfw::debugPrintRule(Rule *r)
{
    PolicyRule *rule=PolicyRule::cast(r);
    ostringstream s;

    s << PolicyCompiler::debugPrintRule(rule);

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

    return s.str();
}


void PolicyCompiler_ipfw::epilog()
{
}

