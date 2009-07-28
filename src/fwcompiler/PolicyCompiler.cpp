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
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/CustomService.h"
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

    combined_ruleset = new Policy();   // combined ruleset (all interface policies and global policy)
    fw->add( combined_ruleset );

    temp_ruleset = new Policy();   // working copy of the policy
    fw->add( temp_ruleset );

    int global_num=0;

    RuleSet *ruleset = source_ruleset;
    if (ruleset == NULL)
    {
        source_ruleset = RuleSet::cast(policy);
        ruleset = policy;
    }
    ruleset->renumberRules();

    combined_ruleset->setName(ruleset->getName());
    temp_ruleset->setName(ruleset->getName());

    string label_prefix = "";
    if (ruleset->getName() != "Policy") label_prefix = ruleset->getName();

    for (FWObject::iterator i=ruleset->begin(); i!=ruleset->end(); i++)
    {
	PolicyRule *r = PolicyRule::cast(*i);

	if (r->isDisabled()) continue;

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
	r->setAbsRuleNumber(global_num); global_num++;
        r->setUniqueId( FWObjectDatabase::getStringId(r->getId()) );
	combined_ruleset->add( r );
    }

    initialized=true;

    return combined_ruleset->size();
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
     * TODO: actually, route rule may shadow other rules if it
     * translates into "final" target, that is stops processing. This
     * may or may not be so, depending on the platform and combination
     * of rule options.
     */
    if (r1_action==PolicyRule::Route  ||
        r2_action==PolicyRule::Route ) return false;

    /*
     * the problem with branching rules is that it is combination of
     * the head rule and rules in the branch rather than a single rule
     * that can shadow other rules below them. Our current mechanism for
     * shadowing detection does not support this so all we can do is
     * skip rules with action Branch.
     */
    if (r1_action==PolicyRule::Branch  ||
        r2_action==PolicyRule::Branch ) return false;

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

    if (src1==NULL || dst1==NULL || srv1==NULL)
        throw FWException("Can not compare rules because rule " + 
                          r1.getLabel()
                          + " has a group in one of its elements. Aborting.");

    if (src2==NULL || dst2==NULL || srv2==NULL)
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

    if (src1==NULL || dst1==NULL || srv1==NULL)
        throw FWException("Can not compare rules because rule " +
                          r1.getLabel() +
                          " has a group in one of its elements. Aborting.");

    if (src2==NULL || dst2==NULL || srv2==NULL)
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

bool PolicyCompiler::ItfNegation::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
//    FWOptions  *ruleopt =rule->getOptionsObject();

    list<FWObject*> allInterfaces=compiler->fw->getByType(Interface::TYPENAME);

    RuleElementItf *itfre = rule->getItf();
    if (itfre==NULL)
    {
        compiler->abort("Missing interface RE in rule '" +
                        rule->getLabel() +
                        "' id=" + FWObjectDatabase::getStringId(rule->getId())
        );
    }

    if (itfre->getNeg())
    {
        for (FWObject::iterator i=itfre->begin(); i!=itfre->end(); ++i)
        {
            FWObject *o = FWReference::getObject(*i);
            allInterfaces.remove(o);
        }
        itfre->reset();
        itfre->setNeg(false);
        for (FWObject::iterator i=allInterfaces.begin(); i!=allInterfaces.end(); ++i)
            itfre->addRef(*i);
    }

    tmp_queue.push_back(rule);
    return true;
}

bool  PolicyCompiler::InterfacePolicyRules::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementItf *itfre=rule->getItf();   assert(itfre);
    if (itfre->isAny())
    {
        rule->setInterfaceId(-1);
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
                    compiler->warning("Object '" + o1->getName() + "', which is not an interface, is a member of the group '" + o->getName() + "' used in 'Interface' element of a rule.   Rule: " + rule->getLabel());
                    continue;
                }
                PolicyRule *r= compiler->dbcopy->createPolicyRule();
                compiler->temp_ruleset->add(r);
                r->duplicate(rule);
                r->setInterfaceId(o1->getId());
                tmp_queue.push_back(r);
            }
        } else
        {

            PolicyRule *r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setInterfaceId(o->getId());
            tmp_queue.push_back(r);
        }
    }
    return true;
}


bool  PolicyCompiler::ExpandGroups::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    RuleElementSrc *src=rule->getSrc();   assert(src);
    RuleElementDst *dst=rule->getDst();   assert(dst);
    RuleElementSrv *srv=rule->getSrv();   assert(srv);

    compiler->expandGroupsInRuleElement(src);
    compiler->expandGroupsInRuleElement(dst);
    compiler->expandGroupsInRuleElement(srv);
    
    return true;
}


bool  PolicyCompiler::ExpandMultipleAddressesInSRC::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    RuleElementSrc *src=rule->getSrc();    assert(src);
    compiler->_expandAddr(rule,src);
    tmp_queue.push_back(rule);
    return true;
}

bool  PolicyCompiler::ExpandMultipleAddressesInDST::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    RuleElementDst *dst=rule->getDst();    assert(dst);
    compiler->_expandAddr(rule,dst);
    tmp_queue.push_back(rule);
    return true;
}

bool  PolicyCompiler::ExpandMultipleAddresses::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    RuleElementSrc *src=rule->getSrc();    assert(src);
    RuleElementDst *dst=rule->getDst();    assert(dst);
    compiler->_expandAddr(rule,src);
    compiler->_expandAddr(rule,dst);
    tmp_queue.push_back(rule);
    return true;
}

bool  PolicyCompiler::addressRanges::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    RuleElementSrc *src=rule->getSrc();    assert(src);
    RuleElementDst *dst=rule->getDst();    assert(dst);
    compiler->_expandAddressRanges(rule,src);
    compiler->_expandAddressRanges(rule,dst);
    tmp_queue.push_back(rule);
    return true;
}



Rule* PolicyCompiler::getDifference(PolicyRule &r1, PolicyRule &r2)
{
    PolicyRule *r = new PolicyRule();
    *r = r1;

    FWObject *nsrc=getFirstSrc(r);
    nsrc->clearChildren();

    FWObject *ndst=getFirstDst(r);
    ndst->clearChildren();

    Service  *nsrv=getFirstSrv(r);
    nsrv->clearChildren();

    if (r1.getInterfaceId()!=r2.getInterfaceId()) return r;

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
		    if (intersection!=NULL) *intersection=ir;
		    return j;
		}

	    } catch (FWException ex) {
		cerr << " *** Exception: " << ex.toString() << endl;
	    }  
	}
    }
    return j;
}

bool PolicyCompiler::splitServices::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *srv=rule->getSrv();

    if (srv->size()==1)
    {
        tmp_queue.push_back(rule);
        return true;
    }

    map<int, list<Service*> > services;

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);

        Service *s=Service::cast( o );
        assert(s);

        int proto = s->getProtocolNumber();
        services[proto].push_back(s);
    }

    for (map<int, list<Service*> >::iterator i1=services.begin();
         i1!=services.end(); i1++)
    {
        list<Service*> &sl=(*i1).second;

        PolicyRule *r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        RuleElementSrv *nsrv=r->getSrv();
        nsrv->clearChildren();

        for (list<Service*>::iterator j=sl.begin(); j!=sl.end(); j++)
        {
           nsrv->addRef( (*j) );
        }

        tmp_queue.push_back(r);
    }
    return true;
}

/*
 * processor splitServices should have been called eariler, so now all
 * services in Srv are of the same type
 */
bool PolicyCompiler::separateTCPWithFlags::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *rel= rule->getSrv();

    if (rel->size()==1)
    {
        tmp_queue.push_back(rule);
        return true;
    }

/* separate TCP services with TCP flags (can't use those in combination
 * with others in multiport 
 */
    list<Service*> services;
    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);
	    
        TCPService *s=TCPService::cast( o );
        if (s==NULL) continue;

        if ( s->inspectFlags() )
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
    for (list<Service*>::iterator i1=services.begin(); i1!=services.end(); i1++)
        rel->removeRef( (*i1) );

    if (!rel->isAny())
        tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler::verifyCustomServices::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    RuleElementSrv *srv=rule->getSrv();

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);
	assert(o!=NULL);
	if (CustomService::isA(o) && 
	    CustomService::cast(o)->getCodeForPlatform(compiler->myPlatformName()).empty())
	    throw FWException("Custom service is not configured for the platform '"+compiler->myPlatformName()+"'. Rule "+rule->getLabel());
    }

    return true;
}

/*
 * checks if one of the children of RuleElement is a host, IPv4 or
 * network object with address 0.0.0.0 and netmask 0.0.0.0.
 *
 * Exceptions: 
 *   - object 'any'
 *   - interface with dynamic address.
 */
Address* PolicyCompiler::checkForZeroAddr::findZeroAddress(RuleElement *re)
{
    Address *a=NULL;

    for (FWObject::iterator i=re->begin(); i!=re->end(); i++) 
    {
        FWObject *o = FWReference::getObject(*i);
	assert(o!=NULL);

        MultiAddress *maddr = MultiAddress::cast(o);
        if (maddr && maddr->isRunTime()) continue;

        Address *addr = Address::cast(o);
        
        if (addr==NULL && o!=NULL)
            compiler->warning(
                string("findZeroAddress: Unknown object in rule element: ") +
                o->getName() +
                "  type=" + o->getTypeName());

        if (addr && addr->hasInetAddress())
        {
            if (Interface::cast(o)!=NULL && 
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
                if (ad->isAny() && nm!=NULL && nm->isAny())
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
    Address *a=NULL;

    for (FWObject::iterator i=re->begin(); i!=re->end(); i++) 
    {
        FWObject *o = FWReference::getObject(*i);
	assert(o!=NULL);
        Host *addr = Host::cast(o);
        // if host has child of type Interface, it must be first of the children
        if (addr!=NULL && addr->front()!=NULL && Interface::isA(addr->front()))
        {
            a=addr;
            break;
        }
    }

    return a;
}


/*
 * looks for objects with address 0.0.0.0 and aborts compilation if
 * finds such object
 */
bool PolicyCompiler::checkForZeroAddr::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    Address *a=NULL;

    a = findHostWithNoInterfaces( rule->getSrc() );
    if (a==NULL) a = findHostWithNoInterfaces( rule->getDst() );

    if (a!=NULL)
        compiler->abort("Object '"+a->getName()+
                        "' has no interfaces, therefore it does not have "
                        "address and can not be used in the rule."+
                        " Rule "+rule->getLabel());

    a = findZeroAddress( rule->getSrc() );
    if (a==NULL) a = findZeroAddress( rule->getDst() );

    if (a!=NULL)
    {
        string err="Object '"+a->getName()+"'";
        if (IPv4::cast(a)!=NULL) // || IPv6::cast(a)!=NULL
        {
            FWObject *p=a->getParent();
            Interface *iface = Interface::cast(p);
            if (iface!=NULL) 
            {
                err+=" (an address of interface ";
                if (iface->getLabel()!="") err+=iface->getLabel();
                else                       err+=iface->getName();
                err+=" )";
            }
        }
        err += " has address 0.0.0.0, which is equivalent to 'any'. "
            "This is most likely an error. Rule " + rule->getLabel();

        compiler->abort(err);
    }

    tmp_queue.push_back(rule);

    return true;
}


bool PolicyCompiler::checkForUnnumbered::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if ( compiler->catchUnnumberedIfaceInRE( rule->getSrc() ) || 
         compiler->catchUnnumberedIfaceInRE( rule->getDst() ) )
        compiler->abort("Can not use unnumbered interfaces in rules. Rule "+rule->getLabel());

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler::ConvertToAtomicForAddresses::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
	    s->add( *i1 );

	    s=r->getDst();	assert(s);
	    s->clearChildren();
	    s->add( *i2 );

	    tmp_queue.push_back(r);
	}
    }
    return true;
}

bool PolicyCompiler::ConvertToAtomicForIntervals::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementInterval *ivl=rule->getWhen();

    if (ivl==NULL || ivl->isAny()) {
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
        s->add( *i1 );

        tmp_queue.push_back(r);
    }
    return true;
}

bool  PolicyCompiler::ConvertToAtomic::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
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
                s->add( *i1 );

                s=r->getDst();	assert(s);
                s->clearChildren();
                s->add( *i2 );

                s=r->getSrv();	assert(s);
                s->clearChildren();
                s->add( *i3 );

                tmp_queue.push_back(r);
	    }
	}
    }
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

	if (! check_interface || 
	    (rule->getInterfaceId()==r->getInterfaceId()) ) 
        {
            bool cr = false;
            if (reverse)
                cr = pcomp->checkForShadowing( *r , *rule );
            else
                cr = pcomp->checkForShadowing( *rule , *r );
            if ( cr ) 
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
    rule=getNext(); if (rule==NULL) return false;

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
            compiler->abort("Rule '" + r->getLabel() +
             "' shadows rule '" + rule->getLabel() + "'  below it");
        }
    }

    rules_seen_so_far.push_back(rule);

    return true;
}

bool PolicyCompiler::DetectShadowingForNonTerminatingRules::processNext()
{
    PolicyRule *rule;
    rule=getNext(); if (rule==NULL) return false;

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
            compiler->abort("Non-terminating rule '" + rule->getLabel() +
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    tmp_queue.push_back(rule);

    RuleElement *src=rule->getSrc();
    RuleElement *dst=rule->getDst();

    string lbl=rule->getLabel();

    if ( ! checkRuleElement(src) )
    {
        if (last_rule_lbl!=lbl)
            compiler->warning( "MAC address matching is not supported. One or several MAC addresses removed from source in the rule "+lbl);

        if (src->empty() || src->isAny())
            compiler->abort("Source becomes 'Any' after all MAC addresses have been removed in the rule "+lbl);

        last_rule_lbl=lbl;
    }


    if ( ! checkRuleElement(dst) )
    {
        if (last_rule_lbl!=lbl)
            compiler->warning("MAC address matching is not supported. One or several MAC addresses removed from destination in the rule "+lbl);

        if (dst->empty() || dst->isAny())
            compiler->abort("Destination becomes 'Any' after all MAC addresses have been removed in the rule "+lbl);

        last_rule_lbl=lbl;
    }

    return true;
}


bool PolicyCompiler::CheckForTCPEstablished::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *srv=rule->getSrv();

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);

        TCPService *s = TCPService::cast( o );
        if (s==NULL) continue;

        if (s->getEstablished())
            compiler->abort(string("TCPService object with option \"established\" is not supported by firewall platform \"") + compiler->myPlatformName() + string("\". Use stateful rule instead."));
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler::CheckForUnsupportedUserService::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrv *srv=rule->getSrv();

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++)
    {
        FWObject *o = FWReference::getObject(*i);

        if (UserService::isA(o))
            compiler->abort(string("UserService object is not supported by ") +
                            compiler->myPlatformName());
    }

    tmp_queue.push_back(rule);
    return true;
}

/* keep only rules that have ipv4 addresses in src and dst
 * 
 * TODO: figure out what to do with rules that have mix of ipv4 and ipv6
 * addresses in different rule elements (such as ipv4 address in odst
 * and ipv6 address in tdst or similar)
 */
bool PolicyCompiler::DropRulesByAddressFamilyAndServiceType::processNext()
{
    PolicyRule *rule = getNext(); if (rule==NULL) return false;
    RuleElement *src = rule->getSrc();
    RuleElement *dst = rule->getDst();
    RuleElement *srv = rule->getSrv();

    bool orig_src_any = src->isAny();
    bool orig_dst_any = dst->isAny();
    bool orig_srv_any = srv->isAny();
    compiler->DropAddressFamilyInRE(src, drop_ipv6);
    compiler->DropAddressFamilyInRE(dst, drop_ipv6);
    compiler->DropByServiceTypeInRE(srv, drop_ipv6);

    if (!orig_src_any && src->isAny())
    {
        // removing all ipv6 addresses from source makes it 'any', drop
        // this rule
        return true;
    }

    if (!orig_dst_any && dst->isAny())
    {
        // removing all ipv6 addresses from destination makes it 'any', drop
        // this rule
        return true;
    }

    if (!orig_srv_any && srv->isAny())
    {
        // removing all ipv6 addresses from service makes it 'any', drop
        // this rule
        return true;
    }

    tmp_queue.push_back(rule);

    return true;
}

bool PolicyCompiler::dropRuleWithEmptyRE::processNext()
{
    PolicyRule *rule = getNext(); if (rule==NULL) return false;
    RuleElementSrc *srcrel=rule->getSrc();
    RuleElementDst *dstrel=rule->getDst();
    if ((srcrel->size() == 0) || (dstrel->size() == 0)) return true;
//    Address     *src = compiler->getFirstSrc(rule);
//    Address     *dst = compiler->getFirstDst(rule);
//    if (src!=NULL && dst!=NULL) tmp_queue.push_back(rule);
    tmp_queue.push_back(rule);
    return true;
}


string PolicyCompiler::debugPrintRule(Rule *r)
{
    PolicyRule *rule=PolicyRule::cast(r);

//    FWOptions *ruleopt =rule->getOptionsObject();

    RuleElementSrc *srcrel=rule->getSrc();
    RuleElementDst *dstrel=rule->getDst();
    RuleElementSrv *srvrel=rule->getSrv();
    RuleElementItf *itfrel=rule->getItf();

    int iface_id = rule->getInterfaceId();
    Interface *rule_iface = Interface::cast(dbcopy->findInIndex(iface_id));

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

        if (i4!=itfrel->end()) {
            FWObject *o=*i4;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
            itf+=o->getName();
        }

        int w=0;
        if (no==0) {
            str << rule->getLabel();
            w=rule->getLabel().length();
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
            if (rule_iface!=NULL) str << " " << rule_iface->getName();
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

PolicyRule* PolicyCompiler::addMgmtRule(Address* const src,
                                        Address* const dst,
                                        Service* const service,
                                        Interface* const iface,
                                        PolicyRule::Direction direction,
                                        PolicyRule::Action action,
                                        const string &label)
{
    assert(combined_ruleset != NULL);

    /* Insert PolicyRules at top so they do not get shadowed by other
     * rules. Call insertRuleAtTop() with hidden_rule argument true to
     * make sure this rule gets negative position number and does not
     * shift positions of other rules. See ticket #16. Also, hidden
     * rules are not considered for shadowing.
     */

    PolicyRule* rule = PolicyRule::cast(combined_ruleset->insertRuleAtTop(true));
    assert(rule != NULL);

    ostringstream str;
    str << rule->getPosition() << " " << label << " (automatic)" ;
    rule->setLabel(str.str());

    FWObject *re;
    re = rule->getSrc();  assert(re!=NULL);
    RuleElementSrc::cast(re)->reset();
    if(src != NULL)
        re->addRef(src);

    re = rule->getDst();  assert(re!=NULL);
    RuleElementDst::cast(re)->reset();
    if(dst != NULL)
        re->addRef(dst);

    re = rule->getSrv();  assert(re!=NULL);
    RuleElementSrv::cast(re)->reset();
    if(service != NULL)
        re->addRef(service);

    re = rule->getWhen(); assert(re!=NULL);
    RuleElementInterval::cast(re)->reset();

    re = rule->getItf(); assert(re!=NULL);
    RuleElementItf::cast(re)->reset();
    if(iface != NULL)
    {
        re->addRef(iface);
        rule->setInterfaceId(iface->getId());
    }

    rule->add(dbcopy->create(PolicyRuleOptions::TYPENAME));
    rule->setLogging(false);
    rule->enable();
    rule->setAction(action);
    rule->setDirection(direction);
    
    return rule;
}
