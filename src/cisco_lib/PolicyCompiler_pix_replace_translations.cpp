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


#include "Helper.h"

#include "PolicyCompiler_pix.h"
#include "NATCompiler_pix.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"

#include <QString>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


bool PolicyCompiler_pix::matchTranslatedAddresses::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    string version = compiler->fw->getStr("version");

    transformed_rules.clear();

    RuleElementSrc *srcrel = rule->getSrc();
    RuleElementDst *dstrel = rule->getDst();
    RuleElementSrv *srvrel = rule->getSrv();

    for (list<FWObject*>::iterator i1=srcrel->begin(); i1!=srcrel->end(); ++i1) 
    {
        for (list<FWObject*>::iterator i2=dstrel->begin(); i2!=dstrel->end(); ++i2) 
        {
            for (list<FWObject*>::iterator i3=srvrel->begin(); i3!=srvrel->end(); ++i3) 
            {
                FWObject *o1  = *i1;
                FWObject *o2  = *i2;
                FWObject *o3  = *i3;
                FWObject *obj1 = nullptr;
                FWObject *obj2 = nullptr;
                FWObject *obj3 = nullptr;

                obj1 = FWReference::getObject(o1);
                Address *src = Address::cast(obj1);
                assert(src!=nullptr);

                obj2 = FWReference::getObject(o2);
                Address *dst = Address::cast(obj2);
                assert(dst!=nullptr);

                obj3 = FWReference::getObject(o3);
                Service *srv = Service::cast(obj3);
                assert(srv!=nullptr);

                list<NATRule*> tl = findMatchingNATRules(src, dst, srv);

                for( list<NATRule*>::iterator t=tl.begin(); t!=tl.end(); ++t)
                    action(rule, *t, src, dst, srv);

            }
        }
    }
/*
 *list transformed_rules has all the atomic rules that have a matching
 * NAT rule, with dst and srv already converted. We just add them to
 * the policy on top of the original rule.
 */
    list<PolicyRule*>::iterator i1;
    for (i1=transformed_rules.begin(); i1!=transformed_rules.end(); ++i1)
    {
        PolicyRule *r=PolicyRule::cast( *i1 );
        tmp_queue.push_back(r);
    }

    tmp_queue.push_back(rule);

    return true;
}


list<NATRule*> PolicyCompiler_pix::matchTranslatedAddresses::findMatchingNATRules(
    Address*, Address*, Service*)
{
    return list<NATRule*>();
}

void PolicyCompiler_pix::matchTranslatedAddresses::action(
    PolicyRule* , NATRule* , Address*, Address*, Service*)
{
}


list<NATRule*> PolicyCompiler_pix::replaceTranslatedAddresses::findMatchingNATRules(
    Address *src, Address *dst, Service *srv)
{
    PolicyCompiler_pix *pix_comp = dynamic_cast<PolicyCompiler_pix*>(compiler);
    return pix_comp->natcmp->findMatchingDNATRules(
        src, dst, srv, RuleElementTDst::TYPENAME);
}

void PolicyCompiler_pix::replaceTranslatedAddresses::action(
    PolicyRule* policy_rule,
    NATRule* nat_rule, Address *src, Address*, Service *srv)
{

//    FWObject *rule_iface = compiler->dbcopy->findInIndex(
//        policy_rule->getInterfaceId());

    RuleElementItf *intf_re = policy_rule->getItf();
    FWObject *rule_iface = FWObjectReference::getObject(intf_re->front());

    RuleElement *re = nat_rule->getOSrc();

    FWObject *o = FWReference::getObject(re->front());
#ifndef NDEBUG
    Address  *osrc = Address::cast(o); assert(osrc);
#endif

    re = nat_rule->getODst();
    o = FWReference::getObject(re->front());
    Address  *odst = Address::cast(o); assert(odst);

    re = nat_rule->getOSrv();
    o = FWReference::getObject(re->front());
    Service  *osrv = Service::cast(o); assert(osrv);

#ifndef NDEBUG
    re = nat_rule->getTSrc();
    o = FWReference::getObject(re->front());
    Address  *tsrc = Address::cast(o); assert(tsrc);

    re = nat_rule->getTDst();
    o = FWReference::getObject(re->front());
    Address  *tdst = Address::cast(o); assert(tdst);

    re = nat_rule->getTSrv();
    o = FWReference::getObject(re->front());
    Service  *tsrv = Service::cast(o); assert(tsrv);
#endif

    FWObject *p = odst->getParent();

    if (odst->getId() == rule_iface->getId() ||
        p->getId() == rule_iface->getId())
    {

        PolicyRule  *r = compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(policy_rule);

        RuleElementSrc *nsrc = r->getSrc();
        nsrc->clearChildren();
        nsrc->addRef( src );

        RuleElementDst *ndst = r->getDst();
        ndst->clearChildren();
        ndst->addRef( odst );

        RuleElementSrv *nsrv = r->getSrv();
        nsrv->clearChildren();

        if (osrv->isAny())
            nsrv->addRef( srv );
        else
            nsrv->addRef( osrv );

        transformed_rules.push_back(r);
    }

}


list<NATRule*> PolicyCompiler_pix::warnWhenTranslatedAddressesAreUsed::findMatchingNATRules(
    Address *src, Address *dst, Service *srv)
{
    PolicyCompiler_pix *pix_comp = dynamic_cast<PolicyCompiler_pix*>(compiler);
    return pix_comp->natcmp->findMatchingDNATRules(
        src, dst, srv, RuleElementODst::TYPENAME);
}

void PolicyCompiler_pix::warnWhenTranslatedAddressesAreUsed::action(
    PolicyRule* policy_rule,
    NATRule* nat_rule, Address*, Address *dst, Service*)
{
//    FWObject *rule_iface = compiler->dbcopy->findInIndex(
//        policy_rule->getInterfaceId());

    RuleElementItf *intf_re = policy_rule->getItf();
    FWObject *rule_iface = FWObjectReference::getObject(intf_re->front());

    string version = compiler->fw->getStr("version");

    RuleElement *re;
    FWObject *o;

    re = nat_rule->getODst();
    o = FWReference::getObject(re->front());
    Address  *odst = Address::cast(o); assert(odst);

    FWObject *p = odst->getParent();

    if (odst->getId() == rule_iface->getId() ||
        p->getId() == rule_iface->getId())
    {
        QString err("Object %1 that represents translated address in a NAT rule %2 "
                    "is used in a policy rule of ASA v%3 firewall. "
                    "Starting with v8.3, ASA requires using real IP addresses "
                    "in the firewall policy rules. ");

        compiler->warning(
            policy_rule,
            err.arg(QString::fromUtf8(dst->getName().c_str()))
            .arg(nat_rule->getLabel().c_str())
            .arg(version.c_str()).toStdString());
    }
}

