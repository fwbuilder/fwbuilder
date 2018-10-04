/*

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include <assert.h>

#include "PolicyCompiler.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/MultiAddress.h"
#include "fwbuilder/FailoverClusterGroup.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace fwcompiler;
using namespace libfwbuilder;
using namespace std;

int PolicyCompiler::prolog()
{
    Compiler::prolog();

    Policy *policy = Policy::cast(fw->getFirstByType(Policy::TYPENAME));
    assert(policy);

    if (source_ruleset == nullptr) source_ruleset = policy;

    source_ruleset->renumberRules();

    temp_ruleset = new Policy();   // working copy of the policy
    fw->add( temp_ruleset );

    temp_ruleset->setName(source_ruleset->getName());

    int global_num = 0;

    string label_prefix = "";
    if (source_ruleset->getName() != "Policy") label_prefix = source_ruleset->getName();

    int rule_counter = 0;
    for (FWObject::iterator i=source_ruleset->begin(); i!=source_ruleset->end(); i++)
    {
	PolicyRule *r = PolicyRule::cast(*i);
        if (r == nullptr) continue; // skip RuleSetOptions object

        /*
         * do not remove disabled rules just yet because some
         * compilers might use RuleSet::insertRuleAtTop() and other
         * similar methods from prolog() or
         * addPredefinedPolicyRules()() and these methods renumber
         * rules (labels stop matching rule positions when this is
         * done because labels are configured in prolog() method of
         * the base class. See fwbuilder ticket 1173)
         */
	//if (r->isDisabled()) continue;

        if (r->getLabel().empty())
        {
            RuleElementItf *itfre = r->getItf();
            assert(itfre);

            if (itfre->isAny())
            {
                r->setLabel( createRuleLabel(label_prefix, 
                                             "global", r->getPosition()) );
            } else
            {
                string interfaces = "";
                for (FWObject::iterator i=itfre->begin(); i!=itfre->end(); ++i)
                {
                    FWObject *o = FWReference::getObject(*i);
                    if (interfaces!="") interfaces += ",";
                    interfaces += o->getName();
                }
                r->setLabel( createRuleLabel(label_prefix, 
                                             interfaces, r->getPosition()) );
            }
        }

	r->setAbsRuleNumber(global_num);
        global_num++;
        rule_counter++;
    }

    initialized = true;

    return rule_counter;
}



/*
 * detects if tule r2 shades rule r1.
 */
bool PolicyCompiler::checkForShadowing(PolicyRule &r1, PolicyRule &r2)
{
    RuleElement *srcrel1;
    RuleElement *dstrel1;
    RuleElement *srvrel1;

    RuleElement *srcrel2;
    RuleElement *dstrel2;
    RuleElement *srvrel2;

    FWObject::iterator i1 = r1.begin();
    srcrel1 = RuleElement::cast(*i1);
    i1++;
    dstrel1 = RuleElement::cast(*i1);
    i1++;
    srvrel1 = RuleElement::cast(*i1);

    i1 = r2.begin();
    srcrel2 = RuleElement::cast(*i1);
    i1++;
    dstrel2 = RuleElement::cast(*i1);
    i1++;
    srvrel2 = RuleElement::cast(*i1);

    if (srcrel1->getNeg()) return false;
    if (dstrel1->getNeg()) return false;
    if (srvrel1->getNeg()) return false;
    if (srcrel2->getNeg()) return false;
    if (dstrel2->getNeg()) return false;
    if (srvrel2->getNeg()) return false;

    /*
     * TODO: actually, route rule may shadow other rules if it
     * translates into "final" target, that is stops processing. This
     * may or may not be so, depending on the platform and combination
     * of rule options.
     */
    if (r1.getRouting() || r2.getRouting()) return false;

    PolicyRule::Action r1_action = r1.getAction();
    PolicyRule::Action r2_action = r2.getAction();

    if (r1_action==PolicyRule::Accounting  ||
        r2_action==PolicyRule::Accounting ) return false;

    /*
     * this is delicate case: negation. We consider r2 to "shade" r1
     * only if r2 is above r1 in the policy; if r1 originally had
     * negation and has been split, such as for example done in
     * fwb_ipt, then some of the produced rules have action Return. If
     * r2 has action != Return and r1 has action Return, we ignore r1.
     */
    if (r1_action==PolicyRule::Return  ||
        r2_action==PolicyRule::Return ) return false;

    /*
     * the problem with branching rules is that it is combination of
     * the head rule and rules in the branch rather than a single rule
     * that can shadow other rules below them. Our current mechanism for
     * shadowing detection does not support this so all we can do is
     * skip rules with action Branch.
     */
    if (r1_action==PolicyRule::Branch  ||
        r2_action==PolicyRule::Branch ) return false;

    /*
     * rules with action continue do not make final decision and
     * therefore can not shadow other rules (but can be shadowed)
     */
    if (/* r1_action==PolicyRule::Continue  || */
        r2_action==PolicyRule::Continue ) return false;

    Address  *src1;
    Address  *dst1;
    Service  *srv1;

    Address  *src2;
    Address  *dst2;
    Service  *srv2;

    map<int, threeTuple*>::iterator it = rule_elements_cache.find(r1.getId());
    if (it!=rule_elements_cache.end())
    {
        threeTuple *tt = it->second;
        src1 = tt->src;
        dst1 = tt->dst;
        srv1 = tt->srv;
    } else
    {
        src1 = Address::cast(FWReference::cast(srcrel1->front())->getPointer());
        dst1 = Address::cast(FWReference::cast(dstrel1->front())->getPointer());
        srv1 = Service::cast(FWReference::cast(srvrel1->front())->getPointer());
        threeTuple *tt = new struct threeTuple;
        tt->src = src1;
        tt->dst = dst1;
        tt->srv = srv1;
        rule_elements_cache[r1.getId()] = tt;
    }
    
    it = rule_elements_cache.find(r2.getId());
    if (it!=rule_elements_cache.end())
    {
        threeTuple *tt = it->second;
        src2 = tt->src;
        dst2 = tt->dst;
        srv2 = tt->srv;
    } else
    {
        src2 = Address::cast(FWReference::cast(srcrel2->front())->getPointer());
        dst2 = Address::cast(FWReference::cast(dstrel2->front())->getPointer());
        srv2 = Service::cast(FWReference::cast(srvrel2->front())->getPointer());
        threeTuple *tt = new struct threeTuple;
        tt->src = src2;
        tt->dst = dst2;
        tt->srv = srv2;
        rule_elements_cache[r2.getId()] = tt;
    }

    if (src1==nullptr || dst1==nullptr || srv1==nullptr)
        throw FWException("Can not compare rules because rule " + 
                          r1.getLabel()
                          + " has a group in one of its elements. Aborting.");

    if (src2==nullptr || dst2==nullptr || srv2==nullptr)
        throw FWException("Can not compare rules because rule " + 
                          r2.getLabel() +
                          " has a group in one of its elements. Aborting.");

    if (MultiAddressRunTime::isA(src1) || MultiAddressRunTime::isA(dst1) ||
        MultiAddressRunTime::isA(src2) || MultiAddressRunTime::isA(dst2))
        return false;

    PolicyRule::Direction dir1 = r1.getDirection();
    PolicyRule::Direction dir2 = r2.getDirection();

    if (dir1 == PolicyRule::Both) dir1 = dir2;
    if (dir2 == PolicyRule::Both) dir2 = dir1;

    if (dir1 != dir2)     return false;

    return ( 
        Compiler::checkForShadowing(*src1, *src2) && 
        Compiler::checkForShadowing(*dst1, *dst2) && 
        Compiler::checkForShadowing(*srv1, *srv2) 
    );

// complete: 3'5"

    return false;
}

/**
 * compare interfaces of rules r1 and r2.
 *
 * Return true if r2 shadows r1 (only inetrface rule element is
 * checked)
 *
 * If interface element is "all" (empty), it shadows any specific
 * interface in the other rule, also "all" shadows "all". If neither
 * is "all", return true if both rules refer the same interface,
 * otherwise return false.
 */
bool PolicyCompiler::checkInterfacesForShadowing(PolicyRule &r1, PolicyRule &r2)
{
    RuleElementItf *intf1_re = r1.getItf();
    FWObject *rule1_iface = FWObjectReference::getObject(intf1_re->front());

    RuleElementItf *intf2_re = r2.getItf();
    FWObject *rule2_iface = FWObjectReference::getObject(intf2_re->front());

    int intf1_id = rule1_iface->getId();
    int intf2_id = rule2_iface->getId();

    if (intf2_re->isAny()) return true;  // "eth0" -- "all" or "all" -- "all"
    return (intf1_id == intf2_id);
}


bool PolicyCompiler::cmpRules(PolicyRule &r1, PolicyRule &r2)
{
    if (r1.getSrc()->getNeg()!=r2.getSrc()->getNeg()) return false;
    if (r1.getDst()->getNeg()!=r2.getDst()->getNeg()) return false;
    if (r1.getSrv()->getNeg()!=r2.getSrv()->getNeg()) return false;
    if (r2.getSrc()->getNeg()!=r2.getSrc()->getNeg()) return false;
    if (r2.getDst()->getNeg()!=r2.getDst()->getNeg()) return false;
    if (r2.getSrv()->getNeg()!=r2.getSrv()->getNeg()) return false;

    Address  *src1=getFirstSrc(&r1);
    Address  *dst1=getFirstDst(&r1);
    Service  *srv1=getFirstSrv(&r1);

    Address  *src2=getFirstSrc(&r2);
    Address  *dst2=getFirstDst(&r2);
    Service  *srv2=getFirstSrv(&r2);

    if (src1==nullptr || dst1==nullptr || srv1==nullptr)
        throw FWException("Can not compare rules because rule " +
                          r1.getLabel() +
                          " has a group in one of its elements. Aborting.");

    if (src2==nullptr || dst2==nullptr || srv2==nullptr)
        throw FWException("Can not compare rules because rule " +
                          r2.getLabel() +
                          " has a group in one of its elements. Aborting.");

    PolicyRule::Direction dir1=r1.getDirection();
    PolicyRule::Direction dir2=r2.getDirection();

    if (dir1==PolicyRule::Both) dir1=dir2;
    if (dir2==PolicyRule::Both) dir2=dir1;

    if (dir1!=dir2)     return false;

    return ( (*src1 == *src2) && (*dst1 == *dst2) && (*srv1 == *srv2) );
}

bool PolicyCompiler::InterfacePolicyRules::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    RuleElementItf *itfre = rule->getItf(); assert(itfre);
    if (itfre->isAny())
    {
//        rule->setInterfaceId(-1);
        tmp_queue.push_back(rule);
        return true;
    }

    for (FWObject::iterator i=itfre->begin(); i!=itfre->end(); ++i)
    {
        FWObject *o = FWReference::getObject(*i);
        if (ObjectGroup::isA(o))
        {
            // a group in "interface" rule element. GUI checks that only
            // interfaces are allowed in such group, but we should check anyway.
            for (FWObject::iterator i=o->begin(); i!=o->end(); ++i)
            {
                FWObject *o1 = FWReference::getObject(*i);
                if (!Interface::isA(o1))
                {
                    compiler->warning(
                        "Object '" + o1->getName() +
                        "', which is not an interface, is a member of the group '" +
                        o->getName() +
                        "' used in 'Interface' element of a rule.");
                    continue;
                }
                PolicyRule *r= compiler->dbcopy->createPolicyRule();
                compiler->temp_ruleset->add(r);
                r->duplicate(rule);
                RuleElementItf *nitf = r->getItf();
                nitf->clearChildren();
                nitf->setAnyElement();
                nitf->addRef(o1);
                tmp_queue.push_back(r);
            }
        } else
        {
            PolicyRule *r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            RuleElementItf *nitf = r->getItf();
	    nitf->clearChildren();
	    nitf->setAnyElement();
            nitf->addRef(o);
            tmp_queue.push_back(r);
        }
    }
    return true;
}


bool  PolicyCompiler::ExpandGroups::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    RuleElementSrc *src=rule->getSrc();   assert(src);
    RuleElementDst *dst=rule->getDst();   assert(dst);
    RuleElementSrv *srv=rule->getSrv();   assert(srv);

    compiler->expandGroupsInRuleElement(src);
    compiler->expandGroupsInRuleElement(dst);
    compiler->expandGroupsInRuleElement(srv);
    
    return true;
}

bool PolicyCompiler::expandGroupsInSrv::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    RuleElementSrv *srv = rule->getSrv();
    compiler->expandGroupsInRuleElement(srv);
    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler::expandGroupsInItf::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    RuleElementItf *itf = rule->getItf();
    compiler->expandGroupsInRuleElement(itf);
    tmp_queue.push_back(rule);
    return true;
}

bool  PolicyCompiler::ExpandMultipleAddresses::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElementSrc *src=rule->getSrc();    assert(src);
    RuleElementDst *dst=rule->getDst();    assert(dst);
    compiler->_expand_addr(rule, src, true);
    compiler->_expand_addr(rule, dst, true);
    tmp_queue.push_back(rule);
    return true;
}

void PolicyCompiler::addressRanges::expandAddressRangesInSrc(PolicyRule *rule)
{
    RuleElementSrc *src = rule->getSrc();    assert(src);
    compiler->_expandAddressRanges(rule, src);
}

void PolicyCompiler::addressRanges::expandAddressRangesInDst(PolicyRule *rule)
{
    RuleElementDst *dst = rule->getDst();    assert(dst);
    compiler->_expandAddressRanges(rule, dst);
}

bool PolicyCompiler::addressRanges::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    expandAddressRangesInSrc(rule);
    expandAddressRangesInDst(rule);
    tmp_queue.push_back(rule);
    return true;
}

Rule* PolicyCompiler::getDifference(PolicyRule &r1, PolicyRule &r2)
{
    PolicyRule *r = new PolicyRule();
    *r = r1;

    FWObject *nsrc = getFirstSrc(r);
    nsrc->clearChildren();

    FWObject *ndst = getFirstDst(r);
    ndst->clearChildren();

    Service  *nsrv = getFirstSrv(r);
    nsrv->clearChildren();

    RuleElementItf *intf_re = r1.getItf();
    FWObject *rule1_iface = FWObjectReference::getObject(intf_re->front());
    int iface1 = rule1_iface->getId();

    intf_re = r2.getItf();
    FWObject *rule2_iface = FWObjectReference::getObject(intf_re->front());
    int iface2 = rule2_iface->getId();

    if (iface1 != iface2) return r;

/*
    vector<FWObject*> v1=_substract_obj( r1.getSrc() , r2.getSrc() );
    vector<FWObject*> v2=_substract_obj( r1.getDst() , r2.getDst() );
    vector<FWObject*> v3=_substract_srv( r1.getSrv() , r2.getSrv() );

    for (vector<FWObject*>::iterator i=v1.begin(); i!=v1.end(); ++i)
	nsrc->addRef(*i);

    for (vector<FWObject*>::iterator i=v2.begin(); i!=v2.end(); ++i)
	ndst->addRef(*i);

    for (vector<FWObject*>::iterator i=v3.begin(); i!=v3.end(); ++i)
	nsrv->addRef(*i);
*/
    return r;
}



list<FWObject*>::iterator 
PolicyCompiler::find_more_specific_rule(
    PolicyRule *rule,
    bool check_interface,
    const list<FWObject*>::iterator &start_here,
    const list<FWObject*>::iterator &stop_here,
    PolicyRule **intersection)
{
    list<FWObject*>::iterator  j;
    for (j=start_here ; j!=stop_here; j++) {
	PolicyRule *r = PolicyRule::cast( *j );

	if (! check_interface || (rule->getStr("acl")==r->getStr("acl")) ) {

	    try {
		if (! intersect( *rule, *r )) continue;

		if (debug>=9) {
		    cerr << "*********  getIntersection: ------------------\n";
		    cerr << debugPrintRule(rule);
		    cerr << debugPrintRule(r);
		    cerr << "----------------------------------------------\n";
		}

		PolicyRule *ir=new PolicyRule();

/* need to place this rule into the tree somewhere so references will
 * get resolved */
		temp_ruleset->add( ir );

/* copy attributes from the current rule we are looking at. Do not change
 * this part of the algorithm as pix compiler relies upon it.
 */
		ir->duplicate(r);
		
		getIntersection(*rule , *r, *ir );

		if (! ir->isEmpty()) {
		    if (debug>=9) {
			cerr << debugPrintRule(ir);
			cerr << "------------------------------------------------\n";
		    }
		    if (intersection!=nullptr) *intersection=ir;
		    return j;
		}

	    } catch (FWException &ex) {
		cerr << " *** Exception: " << ex.toString() << endl;
	    }  
	}
    }
    return j;
}



/*
 * checks if one of the children of RuleElement is a host, IPv4 or
 * network object with address 0.0.0.0 and netmask 0.0.0.0.
 *
 * Exceptions: 
 *   - object 'any'
 *   - interface with dynamic address.
 *
 * In addition check for address A.B.C.D/0 which is most likely a
 * mistake if A.B.C.D != 0.0.0.0. See #475
 */
Address* PolicyCompiler::checkForZeroAddr::findZeroAddress(RuleElement *re)
{
    Address *a=nullptr;

    for (FWObject::iterator i=re->begin(); i!=re->end(); i++) 
    {
        FWObject *o = FWReference::getObject(*i);
	assert(o!=nullptr);

        MultiAddress *maddr = MultiAddress::cast(o);
        if (maddr && maddr->isRunTime()) continue;

        Address *addr = Address::cast(o);
        
        if (addr==nullptr && o!=nullptr)
            compiler->warning(
                    re->getParent(), 
                    string("findZeroAddress: Unknown object in rule element: ") +
                    o->getName() +
                    "  type=" + o->getTypeName());

        if (addr && addr->hasInetAddress())
        {
            if (Interface::cast(o)!=nullptr && 
                (Interface::cast(o)->isDyn() ||
                 Interface::cast(o)->isUnnumbered() ||
                 Interface::cast(o)->isBridgePort()))
                continue;

            if ( ! addr->isAny())
            {
                const InetAddr *ad = addr->getAddressPtr();
                const InetAddr *nm = addr->getNetmaskPtr();
                // AddressRange has address but not netmask
                // AddressRange with address 0.0.0.0 is acceptable
                // (not equivalent to "any")
                if (ad->isAny() && nm!=nullptr && nm->isAny())
                {
                    a = addr;
                    break;
                }
                // Address A.B.C.D/0 is most likely a mistake if
                // A.B.C.D != 0.0.0.0
                if ((Network::cast(addr) || NetworkIPv6::cast(addr)) &&
                    !ad->isAny() && nm!=nullptr && nm->isAny())
                {
                    a = addr;
                    break;
                }
            }
        }
    }

    return a;
}

Address* PolicyCompiler::checkForZeroAddr::findHostWithNoInterfaces(
    RuleElement *re)
{
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++) 
    {
        FWObject *o = FWReference::getObject(*i);
	assert(o!=nullptr);
        Host *addr = Host::cast(o);

        if (addr!=nullptr && addr->front()!=nullptr)
        {
            FWObject::iterator it;
            it=addr->begin();
            while (it!=addr->end() && !Interface::isA(*it)) ++it;
            if (it==addr->end()) return addr; // has no interfaces
        }
    }

    return nullptr;
}


/*
 * looks for objects with address 0.0.0.0 and aborts compilation if
 * finds such object
 */
bool PolicyCompiler::checkForZeroAddr::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    Address *a=nullptr;

    a = findHostWithNoInterfaces( rule->getSrc() );
    if (a==nullptr) a = findHostWithNoInterfaces( rule->getDst() );

    if (a!=nullptr)
        compiler->abort(
                rule, "Object '"+a->getName()+
                "' has no interfaces, therefore it does not have "
                "address and can not be used in the rule.");

    a = findZeroAddress( rule->getSrc() );
    if (a==nullptr) a = findZeroAddress( rule->getDst() );

    if (a!=nullptr)
    {
        string err="Object '"+a->getName()+"'";
        if (IPv4::cast(a)!=nullptr) // || IPv6::cast(a)!=nullptr
        {
            FWObject *p=a->getParent();
            Interface *iface = Interface::cast(p);
            if (iface!=nullptr) 
            {
                err+=" (an address of interface ";
                if (iface->getLabel()!="") err+=iface->getLabel();
                else                       err+=iface->getName();
                err+=" )";
            }
        }
        err += " has address or netmask 0.0.0.0, which is equivalent to 'any'. "
            "This is likely an error.";

        compiler->abort(rule, err);
    }

    tmp_queue.push_back(rule);

    return true;
}


bool PolicyCompiler::checkForUnnumbered::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if ( compiler->catchUnnumberedIfaceInRE( rule->getSrc() ) || 
         compiler->catchUnnumberedIfaceInRE( rule->getDst() ) )
        compiler->abort(
            
                rule, "Can not use unnumbered interfaces in rules.");

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler::ConvertToAtomicForAddresses::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrc *src=rule->getSrc();    assert(src);
    RuleElementDst *dst=rule->getDst();    assert(dst);

    for (FWObject::iterator i1=src->begin(); i1!=src->end(); ++i1) {
	for (FWObject::iterator i2=dst->begin(); i2!=dst->end(); ++i2) {

	    PolicyRule *r = compiler->dbcopy->createPolicyRule();
	    r->duplicate(rule);
	    compiler->temp_ruleset->add(r);

	    FWObject *s;
	    s=r->getSrc();	assert(s);
	    s->clearChildren();
	    s->addCopyOf( *i1 );

	    s=r->getDst();	assert(s);
	    s->clearChildren();
	    s->addCopyOf( *i2 );

	    tmp_queue.push_back(r);
	}
    }
    return true;
}

bool PolicyCompiler::ConvertToAtomicForIntervals::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementInterval *ivl=rule->getWhen();

    if (ivl==nullptr || ivl->isAny()) {
        tmp_queue.push_back(rule);
        return true;
    }

    for (FWObject::iterator i1=ivl->begin(); i1!=ivl->end(); ++i1) {

        PolicyRule *r = compiler->dbcopy->createPolicyRule();
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);
        
        FWObject *s;

        s=r->getWhen();	assert(s);
        s->clearChildren();
        s->addCopyOf( *i1 );

        tmp_queue.push_back(r);
    }
    return true;
}

bool  PolicyCompiler::ConvertToAtomic::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElementSrc *src=rule->getSrc();  assert(src);
    RuleElementDst *dst=rule->getDst();  assert(dst);
    RuleElementSrv *srv=rule->getSrv();  assert(srv);

    for (FWObject::iterator i1=src->begin(); i1!=src->end(); i1++)
    {
	for (FWObject::iterator i2=dst->begin(); i2!=dst->end(); i2++)
        {
	    for (FWObject::iterator i3=srv->begin(); i3!=srv->end(); i3++)
            {
                PolicyRule *r = compiler->dbcopy->createPolicyRule();
                r->duplicate(rule);
                compiler->temp_ruleset->add(r);

                FWObject *s;
                s=r->getSrc();	assert(s);
                s->clearChildren();
                s->addCopyOf( *i1 );

                s=r->getDst();	assert(s);
                s->clearChildren();
                s->addCopyOf( *i2 );

                s=r->getSrv();	assert(s);
                s->clearChildren();
                s->addCopyOf( *i3 );

                tmp_queue.push_back(r);
	    }
	}
    }
    return true;
}


bool PolicyCompiler::checkForShadowingPlatformSpecific(PolicyRule * /*UNUSED r1 */,
                                                       PolicyRule * /*UNUSED r2 */)
{
    return true;
}

std::deque<Rule*>::iterator 
PolicyCompiler::findMoreGeneralRule::find_more_general_rule(
    PolicyRule *rule,
    bool check_interface,
    const std::deque<Rule*>::iterator &start_here,
    const std::deque<Rule*>::iterator &stop_here,
    bool reverse)
{
    PolicyCompiler *pcomp=dynamic_cast<PolicyCompiler*>(compiler);
    if (compiler->debug>=9) 
    {
        cerr << "*********  searching for more general rule: -------------\n";
        cerr << compiler->debugPrintRule(rule);
        cerr << endl;
    }

    std::deque<Rule*>::iterator  j;
    for (j=start_here ; j!=stop_here; j++) 
    {
	PolicyRule *r = PolicyRule::cast( *j );

        bool intf_cr = false;
        if (reverse)
            intf_cr = pcomp->checkInterfacesForShadowing( *r , *rule );
        else
            intf_cr = pcomp->checkInterfacesForShadowing( *rule , *r );

	if (! check_interface || intf_cr)
        {
            bool cr = false;
            if (reverse)
                cr = pcomp->checkForShadowing( *r , *rule );
            else
                cr = pcomp->checkForShadowing( *rule , *r );

            if ( cr && pcomp->checkForShadowingPlatformSpecific(rule, r))
            {
                if (compiler->debug>=9) 
                {
                    cerr << r->getLabel() 
                         << ": FOUND more general rule:\n";
                    cerr << compiler->debugPrintRule(r);
                    cerr << endl;
                }
                return j;
            } else 
            {
                if (compiler->debug>=9)
                    cerr << r->getLabel() 
                         << ": rules do not intersect  \n";
                continue;
            }
	}
    }
    return j;
}

bool PolicyCompiler::DetectShadowing::processNext()
{
    PolicyRule *rule;
    rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);  // to pass it to the next processor, if any
    if (rule->isFallback()) return true; // do not check fallback  ..
    if (rule->isHidden())   return true; //  ... and hidden rules

    std::deque<Rule*>::iterator i =
        find_more_general_rule(rule,
                               true,
                               rules_seen_so_far.begin(),
                               rules_seen_so_far.end(),
                               false);
    if (i!=rules_seen_so_far.end()) 
    {
        Rule *r = *i;
/*
 * find_more_general finds more general _or_ equivalent rule
 */
        if (r && r->getAbsRuleNumber() != rule->getAbsRuleNumber() && 
            ! (*r == *rule) ) 
        {
            compiler->abort(
                                r, "Rule '" + r->getLabel() +
                                "' shadows rule '" + rule->getLabel() + "'  below it");
        }
    }

    rules_seen_so_far.push_back(rule);

    return true;
}

bool PolicyCompiler::DetectShadowingForNonTerminatingRules::processNext()
{
    PolicyRule *rule;
    rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);  // to pass it to the next processor, if any
    if (rule->isFallback()) return true; // do not check fallback  ..
    if (rule->isHidden())   return true; //  ... and hidden rules

    std::deque<Rule*>::iterator i = 
        find_more_general_rule(rule,
                               true,
                               rules_seen_so_far.begin(),
                               rules_seen_so_far.end(),
                               true);   // <<<<<<< NB!
    if (i!=rules_seen_so_far.end()) 
    {
        Rule *r = *i;
/*
 * find_more_general finds more general _or_ equivalent rule
 */
        if (r && r->getAbsRuleNumber() != rule->getAbsRuleNumber() && 
            ! (*r == *rule) ) 
        {
            compiler->abort(
                
                    rule, 
                    "Non-terminating rule '" + rule->getLabel() +
                    "' shadows rule '" + r->getLabel() + "'  above it");
        }
    }

    rules_seen_so_far.push_back(rule);
    return true;
}

bool PolicyCompiler::MACFiltering::checkRuleElement(RuleElement *re)
{
    bool  res=true;
    std::list<FWObject*> lst;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++) 
    {
        FWObject *o = FWReference::getObject(*i);
        if (physAddress::isA(o)) 
        {
            lst.push_back(o);
            res=false;
        }
    }
    for (FWObject::iterator i1=lst.begin(); i1!=lst.end(); i1++)   re->removeRef(*i1);
    return res;
}


bool PolicyCompiler::MACFiltering::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    RuleElement *src=rule->getSrc();
    RuleElement *dst=rule->getDst();

    string lbl=rule->getLabel();

    if ( ! checkRuleElement(src) )
    {
        if (last_rule_lbl!=lbl)
            compiler->warning(
                
                    rule, "MAC address matching is not supported. "
                    "One or several MAC addresses removed from source in the rule");

        if (src->empty() || src->isAny())
            compiler->abort(
                
                    rule, 
                    "Source becomes 'Any' after all MAC addresses "
                    "have been removed in the rule");

        last_rule_lbl=lbl;
    }


    if ( ! checkRuleElement(dst) )
    {
        if (last_rule_lbl!=lbl)
            compiler->warning(
                
                    rule, 
                    "MAC address matching is not supported. "
                    "One or several MAC addresses removed from destination in the rule");

        if (dst->empty() || dst->isAny())
            compiler->abort(
                
                    rule, 
                    "Destination becomes 'Any' after all MAC addresses "
                    "have been removed in the rule ");

        last_rule_lbl=lbl;
    }

    return true;
}

string PolicyCompiler::debugPrintRule(Rule *r)
{
    PolicyRule *rule=PolicyRule::cast(r);

//    FWOptions *ruleopt =rule->getOptionsObject();

    RuleElementSrc *srcrel = rule->getSrc();
    RuleElementDst *dstrel = rule->getDst();
    RuleElementSrv *srvrel = rule->getSrv();
    RuleElementItf *itfrel = rule->getItf();

//    int iface_id = rule->getInterfaceId();
//    Interface *rule_iface = Interface::cast(dbcopy->findInIndex(iface_id));

    ostringstream str;

//    str << setw(70) << setfill('-') << "-";

    int no=0;
    FWObject::iterator i1=srcrel->begin();
    FWObject::iterator i2=dstrel->begin(); 
    FWObject::iterator i3=srvrel->begin();
    FWObject::iterator i4=itfrel->begin();

    while ( i1!=srcrel->end() || i2!=dstrel->end() || i3!=srvrel->end() ||
            i4!=itfrel->end())
    {
        str << endl;

        string src=" ";
        string dst=" ";
        string srv=" ";
        string itf=" ";

        int src_id = -1;
        int dst_id = -1;
        int srv_id = -1;

        if (srcrel->getNeg()) src = "!";
        if (dstrel->getNeg()) dst = "!";
        if (srvrel->getNeg()) srv = "!";
        if (itfrel->getNeg()) itf = "!";

        if (i1!=srcrel->end())
        {
            FWObject *o = FWReference::getObject(*i1);
            src += o->getName();
            src_id = o->getId();
        }

        if (i2!=dstrel->end())
        {
            FWObject *o = FWReference::getObject(*i2);
            dst += o->getName();
            dst_id = o->getId();
        }

        if (i3!=srvrel->end())
        {
            FWObject *o = FWReference::getObject(*i3);
            srv += o->getName();
            srv_id = o->getId();
        }

        if (i4!=itfrel->end())
        {
            ostringstream str;
            FWObject *o = FWReference::getObject(*i4);
            str << o->getName() << "(" << o->getId() << ")";
            itf += str.str();
        }

        int w = 0;
        if (no==0)
        {
            str << rule->getLabel();
            w = rule->getLabel().length();
        }
        
        str <<  setw(10-w)  << setfill(' ') << " ";

        str <<  setw(18) << setfill(' ') << src.c_str() << "(" << src_id << ")";
        str <<  setw(18) << setfill(' ') << dst.c_str() << "(" << dst_id << ")";
        str <<  setw(12) << setfill(' ') << srv.c_str() << "(" << srv_id << ")";
        str <<  setw(8)  << setfill(' ') << itf.c_str();

        if (no==0)
        {
            str <<  setw(9)  << setfill(' ') << rule->getActionAsString().c_str();
            str <<  setw(12)  << setfill(' ') << rule->getDirectionAsString().c_str();
            if (rule->getLogging()) str << " LOG";
        } else
            str <<  setw(18)  << setfill(' ') << " ";

        ++no;

        if ( i1!=srcrel->end() ) ++i1;
        if ( i2!=dstrel->end() ) ++i2;
        if ( i3!=srvrel->end() ) ++i3;
        if ( i4!=itfrel->end() ) ++i4;
    }
    return str.str();
}

