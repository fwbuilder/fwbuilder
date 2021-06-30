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


#include "PolicyCompiler_pf.h"
#include "NATCompiler_pf.h"

#include "fwbuilder/AddressTable.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>

#include <assert.h>
#include <QtDebug>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string PolicyCompiler_pf::myPlatformName() { return "pf"; }

int PolicyCompiler_pf::prolog()
{
    if (fw->getStr("platform")!=myPlatformName() ) 
	abort("Unsupported platform " + fw->getStr("platform") );

    list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
    {
        Interface *iface=dynamic_cast<Interface*>(*i);
        assert(iface);

        if ( iface->isDyn())  
        {
            list<FWObject*> l3=iface->getByType(IPv4::TYPENAME);
            if (l3.size()>0)
            {
                QString err(
                    "Dynamic interface %1 should not have an IP "
                    "address object attached to it. This IP address "
                    "object will be ignored.");
                warning(err.arg(iface->getName().c_str()).toStdString());
                for (list<FWObject*>::iterator j=l3.begin(); j!=l3.end(); ++j) 
                    iface->remove(*j);
            }
        }
    }

    if (tables)
    {
        tables->init(dbcopy);
        if (!getSourceRuleSet()->isTop())
            tables->setRuleSetName(getRuleSetName());
    }

    return  PolicyCompiler::prolog();
}

/*
 * this is very much like
 * Compiler::swapMultiAddressObjectsInRE::processNext() except it also
 * registers the table using registerTable()
 */
bool PolicyCompiler_pf::swapAddressTableObjectsInRE::processNext()
{
    PolicyCompiler_pf *pf_comp=dynamic_cast<PolicyCompiler_pf*>(compiler);
    Rule *rule=prev_processor->getNextRule(); if (rule==nullptr) return false;

    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );

    list<MultiAddress*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
        /* 
         * All addressTable objects will be run-time here because we
         * switch them in preprocessor. The difference is: if address
         * table was originally run-time, at this point it will have
         * no children, however if it was compile-time originally, it
         * will have children objects.  That is how we distinguish
         * them in this rule processor. Here we only deal with
         * AddressTable objects that originally used to be
         * compile-time because we need to create tables for them.
         */
        if (AddressTable::cast(o)!=nullptr &&
            AddressTable::cast(o)->isRunTime() &&
            o->size() > 0)
            cl.push_back(MultiAddress::cast(o));
    }

    if (!cl.empty())
    {
        for (list<MultiAddress*>::iterator i=cl.begin(); i!=cl.end(); i++)
        {
            MultiAddress *atbl = *i;

            // Need to make sure the ID of the MultiAddressRunTime
            // object created here is stable and is always the same
            // for the same MultiAddress object. In particular this
            // ensures that we reuse tables between policy and NAT rules
            string mart_id_str = FWObjectDatabase::getStringId(atbl->getId()) +
                "_runtime";
            int mart_id = FWObjectDatabase::registerStringId(mart_id_str);

            MultiAddressRunTime *mart = 
                MultiAddressRunTime::cast(compiler->dbcopy->findInIndex(mart_id));
            if (mart==nullptr)
            {
                mart = new MultiAddressRunTime(atbl);

                // need to ensure stable ID for the runtime object, so
                // that when the same object is replaced in different
                // rulesets by different compiler passes, chosen
                // runtime object has the same ID and is identified as
                // the same by the compiler.

                mart->setId( mart_id );
                compiler->dbcopy->addToIndex(mart);
                compiler->persistent_objects->add(mart);

// register this object as a table
                string tblname = atbl->getName();
                string tblID = tblname + "_addressTableObject";
                pf_comp->tables->registerTable(tblname,tblID,atbl);
            }

            re->removeRef(atbl);
            re->addRef(mart);
        }
        tmp_queue.push_back(rule);
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_pf::processMultiAddressObjectsInRE::processNext()
{
    PolicyCompiler_pf *pf_comp=dynamic_cast<PolicyCompiler_pf*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );
    bool neg = re->getNeg();

    list<FWObject*> maddr_runtime;

    try
    {
        for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
        {
            FWObject *o= *i;
            if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

            MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
            if (atrt!=nullptr &&
                atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            {
                if (re->size()>1 && neg)
                {
                    compiler->abort(rule,
                                    "AddressTable object can not be used "
                                    "with negation in combination with "
                                    "other objects in the same rule element.");
                }
                string tblname = o->getName();
                string tblID = tblname + "_addressTableObject";
                pf_comp->tables->registerTable(tblname,tblID,o);
                o->setBool("pf_table",true);
                maddr_runtime.push_back(o);
            }
        }
    } catch(FWException &ex)  // TableFactory::registerTable throws exception
    {
        string err;
        err = "Can not process MultiAddress object: " + ex.toString();
        compiler->abort(rule, err);
    }

    if (!maddr_runtime.empty())
    {
        RuleElement *nre;

        for (FWObject::iterator i=maddr_runtime.begin(); i!=maddr_runtime.end(); i++)
        {
            PolicyRule *r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            nre=RuleElement::cast( r->getFirstByType(re_type) );
            nre->clearChildren();
            nre->addRef( *i );
            tmp_queue.push_back(r);
        }

        for (FWObject::iterator i=maddr_runtime.begin(); i!=maddr_runtime.end(); i++)
            re->removeRef( *i );

        if (!re->isAny())
            tmp_queue.push_back(rule);

        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_pf::splitIfFirewallInSrc::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    PolicyRule *r;
    RuleElementSrc *src = rule->getSrc();
    assert(src);

    if (src->size()==1 || src->getNeg())
    {
	tmp_queue.push_back(rule);
	return true;
    }

    FWObject *fw_in_src = nullptr;
    vector<FWObject*> cl;
    for (FWObject::iterator i1=src->begin(); i1!=src->end(); ++i1)
    {
	FWObject *obj = FWReference::getObject(*i1);
	if (obj==nullptr)
            compiler->abort(rule, "Broken Src object");

	if (obj->getId()==compiler->getFwId())
        {
	    fw_in_src = obj;

	    RuleElementSrc *nsrc;

	    r = compiler->dbcopy->createPolicyRule();
	    compiler->temp_ruleset->add(r);
	    r->duplicate(rule);
	    nsrc = r->getSrc();
	    nsrc->clearChildren();
	    nsrc->setAnyElement();
	    nsrc->addRef( compiler->fw );
	    tmp_queue.push_back(r);
	}
    }
    if (fw_in_src!=nullptr) src->removeRef( fw_in_src );

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_pf::splitIfFirewallInDst::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    PolicyRule *r;
    RuleElementDst *dst = rule->getDst();    assert(dst);

    if (dst->size()==1 || dst->getNeg())
    {
	tmp_queue.push_back(rule);
	return true;
    }

    FWObject *fw_in_dst = nullptr;
    vector<FWObject*> cl;
    for (FWObject::iterator i1=dst->begin(); i1!=dst->end(); ++i1)
    {
	FWObject *obj = FWReference::getObject(*i1);
	if (obj==nullptr)
            compiler->abort(rule, "Broken Dst");

	if (obj->getId()==compiler->getFwId())
        {
	    fw_in_dst = obj;

	    RuleElementDst *ndst;

	    r = compiler->dbcopy->createPolicyRule();
	    compiler->temp_ruleset->add(r);
	    r->duplicate(rule);
	    ndst = r->getDst();
	    ndst->clearChildren();
	    ndst->setAnyElement();
	    ndst->addRef( compiler->fw );
	    tmp_queue.push_back(r);
	}
    }
    if (fw_in_dst!=nullptr) dst->removeRef( fw_in_dst );

    tmp_queue.push_back(rule);
    return true;
}



bool PolicyCompiler_pf::fillDirection::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

/* after interface policies have been merged with global policy, rules
 * with empty direction have disappeared.  In fact, xslt
 * transformation 2.1.1->2.1.2 leaves empty direction in the old
 * global policy rules, but the GUI promptly fixes that replacing it
 * with "Both" whenever user opens the policy. So, we have to handle
 * both the case of an empty direction and direction "Both". To
 * preserve old semantics as accurately as possible, I check for a
 * combination of empty interface and direction "Both", this is what
 * old global rules become after user opens the combined policy in the
 * GUI. In fact, it does not matter what direction is set in the rule
 * as long as it misses interface - we need to determine direction
 * again anyway.
 */
    if (rule->getDirection() == PolicyRule::Undefined)
        rule->setDirection( PolicyRule::Both );

/*
 * Correction for bug #2791950 "no way to generate "pass out" rule
 *  with no interface". Do not reset direction just because interface
 *  rule element is "any", otherwise we can not create rule with no
 *  interface spec:
 *
 * pass out quick inet from any to any
 *
 * If we reset direction here, instead of this one rule we get two,
 * one "pass out" and another "pass in". However it is still useful to
 * change direction if fw is in source or destination.
 */
    if (rule->getDirection() == PolicyRule::Both)
    {
        Address  *src = compiler->getFirstSrc(rule);
        Address  *dst = compiler->getFirstDst(rule);
        //int fwid = compiler->getFwId();

        if (src==nullptr || dst==nullptr)
            compiler->abort(rule, "Broken src or dst");

        if (!src->isAny() && !dst->isAny() &&
            compiler->complexMatch(compiler->fw, src) &&
            compiler->complexMatch(compiler->fw, dst)) return true;

        if (!src->isAny() && compiler->complexMatch(compiler->fw, src))
        {
            rule->setDirection( PolicyRule::Outbound );
            compiler->warning(
                rule, "Changing rule direction due to self reference");
        }
        if (!dst->isAny() && compiler->complexMatch(compiler->fw, dst))
        {
            rule->setDirection( PolicyRule::Inbound );
            compiler->warning(
                rule, "Changing rule direction due to self reference");
        }
    }
    return true;
}

bool PolicyCompiler_pf::SpecialServices::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    RuleElementSrv *srv=rule->getSrv();

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++)
    {
	FWObject *o= *i;
	if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
	Service *s=Service::cast( o );
	assert(s);

	if (IPService::cast(s)!=nullptr  && rule->getAction()==PolicyRule::Accept)
        {
            rule->setBool("allow_opts", IPService::constcast(s)->hasIpOptions());
	}
    }
    return true;
}

bool PolicyCompiler_pf::SplitDirection::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getDirection()==PolicyRule::Both && rule->getRouting())
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

bool PolicyCompiler_pf::ProcessScrubOption::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    FWOptions *ruleopt =rule->getOptionsObject();


    if ( ruleopt->getBool("scrub") ) {

	if (rule->getAction()!=PolicyRule::Accept) {
	    ruleopt->setBool("scrub",false);
	    tmp_queue.push_back(rule);

	    compiler->abort(rule,
                            "Rule option 'scrub' is supported only for rules "
                            "with action 'Accept'");

	    return true;
	}

	PolicyRule *r = compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setAction(PolicyRule::Scrub);
	r->getOptionsObject()->setBool("scrub",false);
	tmp_queue.push_back(r);

	ruleopt->setBool("scrub",false);
	tmp_queue.push_back(rule);

	return true;
    }

/* if service is ip_fragment and action is 'Deny', then add rule with scrub */

    Service *srv=compiler->getFirstSrv(rule);    assert(srv);

    if ( (srv->getBool("short_fragm") || srv->getBool("fragm")) &&
	 ( rule->getAction()==PolicyRule::Deny || rule->getAction()==PolicyRule::Reject) ) {

	PolicyRule *r = compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	r->setAction(PolicyRule::Scrub);
	r->getOptionsObject()->setBool("scrub",false);
	tmp_queue.push_back(r);

	return true;
    }

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_pf::setQuickFlag::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    FWOptions *ropt = rule->getOptionsObject();
    Q_UNUSED(ropt);

    switch (rule->getAction())
    {
    case PolicyRule::Scrub:
    case PolicyRule::Accounting:
    case PolicyRule::Branch:
    case PolicyRule::Continue:
        break;

    default:
        rule->setBool("quick", true);
        break;
    }

    // as of 4.2.0 build 3477 we provide checkboxes to make Tag and
    // Classify actions (PF) terminating or non-terminating on
    // per-rule basis. Old behavior: Tag was non-terminating and
    // Classify was terminating. Set options accordingly if they are
    // not set.
    // 
    // TODO #2367: now instead of checkboxes, user should use actions Accept
    // or Continue

    return true;
}

bool PolicyCompiler_pf::doSrcNegation::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrc *src=rule->getSrc();

    if (src->getNeg()) {
        RuleElementSrc *nsrc;
	PolicyRule     *r;

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	if (rule->getAction()==PolicyRule::Accept)
            r->setAction(PolicyRule::Deny);
	else
            r->setAction(PolicyRule::Accept);
        nsrc=r->getSrc();
        nsrc->setNeg(false);
	r->setBool("quick",true);
        r->setLogging(false);
	tmp_queue.push_back(r);

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        nsrc=r->getSrc();
        nsrc->setNeg(false);
	nsrc->clearChildren();
	nsrc->setAnyElement();
	r->setBool("quick",true);
	tmp_queue.push_back(r);

	return true;
    }
    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_pf::doDstNegation::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementDst *dst=rule->getDst();

    if (dst->getNeg()) {
        RuleElementDst *ndst;
	PolicyRule     *r;

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	if (rule->getAction()==PolicyRule::Accept)
            r->setAction(PolicyRule::Deny);
	else
            r->setAction(PolicyRule::Accept);
        ndst=r->getDst();
        ndst->setNeg(false);
	r->setBool("quick",true);
        r->setLogging(false);
	tmp_queue.push_back(r);

	r= compiler->dbcopy->createPolicyRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        ndst=r->getDst();
        ndst->setNeg(false);
	ndst->clearChildren();
	ndst->setAnyElement();
	r->setBool("quick",true);
	tmp_queue.push_back(r);

	return true;
    }
    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_pf::doSrvNegation::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementSrv *srv=rule->getSrv();

    if (srv->getNeg())
    {
	compiler->abort(rule,
                        "Negation in Srv is not implemented");
	return true;
    }
    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_pf::addLoopbackForRedirect::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    PolicyCompiler_pf *pf_comp = dynamic_cast<PolicyCompiler_pf*>(compiler);

    RuleElementDst *dst = rule->getDst();
    RuleElementSrv *srv = rule->getSrv();

    if (pf_comp->redirect_rules_info==nullptr)
        compiler->abort(
            rule, 
            "addLoopbackForRedirect needs a valid pointer to "
            "the list<NATCompiler_pf::redirectRuleInfo> object");

    tmp_queue.push_back(rule);

    if (pf_comp->redirect_rules_info->empty()) return true;

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++) 
    {
	FWObject *o1 = FWReference::getObject(*i);
	Service *s = Service::cast( o1 );
	assert(s);

        for (FWObject::iterator j=dst->begin(); j!=dst->end(); j++) 
        {
            FWObject *o2 = FWReference::getObject(*j);
            if (o2->getName() == "self" && DNSName::isA(o2)) continue;

            Address *a = Address::cast( o2 );
            assert(a);

            list<NATCompiler_pf::redirectRuleInfo>::const_iterator k;
            for (k=pf_comp->redirect_rules_info->begin();
                 k!=pf_comp->redirect_rules_info->end(); ++k)
            {
                Address *old_tdst_obj = Address::cast(
                    compiler->dbcopy->findInIndex(k->old_tdst));
                Service *tsrv_obj = Service::cast(
                    compiler->dbcopy->findInIndex(k->tsrv));

                if ( *a == *(old_tdst_obj) &&  *s == *(tsrv_obj) )
                {
// insert address used for redirection in the NAT rule.
                    FWObject *new_tdst_obj = compiler->dbcopy->findInIndex(k->new_tdst);
                    dst->addRef(new_tdst_obj);
                    return true;
                }
            }
        }
    }

    return true;
}


void PolicyCompiler_pf::checkForDynamicInterfacesOfOtherObjects::findDynamicInterfaces(RuleElement *re,
                                                                                        Rule        *rule)
{
    if (re->isAny()) return;
    list<FWObject*> cl;
    for (list<FWObject*>::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs   =Interface::cast( obj );

        if (ifs!=nullptr    &&
            ifs->isDyn() &&
            ifs->getParent()->getId()!=compiler->fw->getId() &&
            ! ifs->getParent()->getBool("pf_table") )
        {
            QString err(
                "Can not build rule using dynamic interface '%1' "
                "of the object '%2' because its address in unknown.");
            compiler->abort(
                rule, 
                err
                .arg(ifs->getName().c_str())
                .arg(ifs->getParent()->getName().c_str()).toStdString());
        }
    }
}


bool PolicyCompiler_pf::checkForDynamicInterfacesOfOtherObjects::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    findDynamicInterfaces( rule->getSrc() , rule );
    findDynamicInterfaces( rule->getDst() , rule );

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_pf::splitIfInterfaceInRE::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );
    if (re->size()<=2)
    {
        tmp_queue.push_back(rule);
        return true;
    }

    list<FWObject*> cl;

    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

        Interface *interface_=Interface::cast(o);
        if (interface_!=nullptr && interface_->isDyn())
            cl.push_back(interface_);
    }

    if (!cl.empty())
    {
        RuleElement *nre;

        PolicyRule *r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        nre=RuleElement::cast( r->getFirstByType(re_type) );
        nre->clearChildren();
        for (FWObject::iterator i=cl.begin(); i!=cl.end(); i++)
            nre->addRef( *i );
        tmp_queue.push_back(r);

        r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        nre=RuleElement::cast( r->getFirstByType(re_type) );
        for (FWObject::iterator i=cl.begin(); i!=cl.end(); i++)
            nre->removeRef( *i );
        tmp_queue.push_back(r);

        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_pf::createTables::processNext()
{
    PolicyCompiler_pf *pf_comp = dynamic_cast<PolicyCompiler_pf*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    RuleElementSrc *src = rule->getSrc();
    RuleElementDst *dst = rule->getDst();

    if (!src->isAny()) pf_comp->tables->createTablesForRE(src, rule);
    if (!dst->isAny()) pf_comp->tables->createTablesForRE(dst, rule);

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_pf::printScrubRule::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    FWOptions* options=compiler->fw->getOptionsObject();

    if (!init && options->getBool("pf_do_scrub"))
    {
        compiler->output << "#" << endl;
        compiler->output << "# Defragmentation" << endl;
        compiler->output << "#" << endl;
        compiler->output << "scrub in from any to any" << endl << endl; 
        init=true;
    }

    tmp_queue.push_back(rule);
    return true;
}





void PolicyCompiler_pf::compile()
{
    string banner = " Compiling " + fw->getName();
    if (!getRuleSetName().empty())  banner += " ruleset " + getRuleSetName();
    if (ipv6) banner += ", IPv6";
    info(banner);

    Compiler::compile();

    bool check_for_recursive_groups=true;

    if (fw->getOptionsObject()->getBool("check_shading") &&
        ! inSingleRuleCompileMode())
    {
        add(new Begin("Detecting rule shadowing"));
        add(new printTotalNumberOfRules());

        add(new ItfNegation("process negation in Itf"));
        add(new InterfacePolicyRules(
                "process interface policy rules and store interface ids"));

        add(new recursiveGroupsInSrc("check for recursive groups in SRC"));
        add(new recursiveGroupsInDst("check for recursive groups in DST"));
        add(new recursiveGroupsInSrv("check for recursive groups in SRV"));
        check_for_recursive_groups=false;

        add(new ExpandGroups("expand groups"));
        add(new dropRuleWithEmptyRE("drop rules with empty rule elements"));
        add(new eliminateDuplicatesInSRC("eliminate duplicates in SRC"));
        add(new eliminateDuplicatesInDST("eliminate duplicates in DST"));
        add(new eliminateDuplicatesInSRV("eliminate duplicates in SRV"));

        add(new swapAddressTableObjectsInSrc(
                "AddressTable -> MultiAddressRunTime in Src"));
        add(new swapAddressTableObjectsInDst(
                "AddressTable -> MultiAddressRunTime in Dst"));

        add(new swapMultiAddressObjectsInSrc(
                "MultiAddress -> MultiAddressRunTime in Src"));
        add(new swapMultiAddressObjectsInDst(
                "MultiAddress -> MultiAddressRunTime in Dst"));

        add(new ExpandMultipleAddressesInSrc(
                "expand objects with multiple addresses in SRC"));
        add(new ExpandMultipleAddressesInDst(
                "expand objects with multiple addresses in DST"));
        add(new dropRuleWithEmptyRE(
                "drop rules with empty rule elements"));
        add(new ConvertToAtomic("convert to atomic rules"));

        add( new checkForObjectsWithErrors(
                 "check if we have objects with errors in rule elements"));

        add(new setQuickFlag("set 'quick' flag"));

        add(new DetectShadowing("Detect shadowing"));
        add(new simplePrintProgress());

        runRuleProcessors();
        deleteRuleProcessors();
    }

    add(new Begin());
    add(new printTotalNumberOfRules());

    add( new singleRuleFilter());

//        add(new printScrubRule(" Defragmentation"));
    if (check_for_recursive_groups)
    {
        add(new recursiveGroupsInSrc("check for recursive groups in SRC"));
        add(new recursiveGroupsInDst("check for recursive groups in DST"));
        add(new recursiveGroupsInSrv("check for recursive groups in SRV"));
    }

    add(new emptyGroupsInSrc("check for empty groups in SRC"));
    add(new emptyGroupsInDst("check for empty groups in DST"));
    add(new emptyGroupsInSrv("check for empty groups in SRV"));

//        add(new doSrcNegation("process negation in Src"));
//        add(new doDstNegation("process negation in Dst"));
    add(new doSrvNegation("process negation in Srv"));

    if (fw->getOptionsObject()->getBool("preserve_group_names"))
    {
        add(new RegisterGroupsAndTablesInSrc(
                "register object groups and tables in Src"));
        add(new RegisterGroupsAndTablesInDst(
                "register object groups and tables in Dst"));
    }

// ExpandGroups opens groups, as well as groups in groups etc.
    add(new ExpandGroups("expand groups"));
    add(new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    add(new CheckForTCPEstablished(
            "check for TCPService objects with flag \"established\""));
            
    add(new eliminateDuplicatesInSRC("eliminate duplicates in SRC"));
    add(new eliminateDuplicatesInDST("eliminate duplicates in DST"));
    add(new eliminateDuplicatesInSRV("eliminate duplicates in SRV"));

    add(new swapAddressTableObjectsInSrc(
            "AddressTable -> MultiAddressRunTime in Src"));
    add(new swapAddressTableObjectsInDst(
            "AddressTable -> MultiAddressRunTime in Dst"));

    add(new swapMultiAddressObjectsInSrc(
            "MultiAddress -> MultiAddressRunTime in Src"));
    add(new swapMultiAddressObjectsInDst(
            "MultiAddress -> MultiAddressRunTime in Dst"));

    add(new processMultiAddressObjectsInSrc(
            "process MultiAddress objects in Src"));
    add(new processMultiAddressObjectsInDst(
            "process MultiAddress objects in Dst"));

    add(new replaceFailoverInterfaceInItf("replace carp interfaces"));


    add(new expandGroupsInItf("expand groups in Interface"));
    add(new replaceClusterInterfaceInItf(
            "replace cluster interfaces with member interfaces in "
            "the Interface rule element"));
    add(new ItfNegation("process negation in Itf"));

    //add(new InterfacePolicyRules(
    //    "process interface policy rules and store interface ids"));

    add(new splitIfFirewallInSrc("split rule if firewall is in Src"));
    add(new ReplaceFirewallObjectWithSelfInSrc(
            "Replace firewall object with 'self' in Src"));

    add(new splitIfFirewallInDst("split rule if firewall is in Dst"));
    add(new ReplaceFirewallObjectWithSelfInDst(
            "Replace firewall object with 'self' in Dst"));

    // call these again since "self" is a MultiAddress object
    add( new swapMultiAddressObjectsInSrc(
             " swap MultiAddress -> MultiAddressRunTime in Src"));
    add( new swapMultiAddressObjectsInDst(
             " swap MultiAddress -> MultiAddressRunTime in Dst"));


    add(new fillDirection("determine directions"));

// commented out for bug #2828602
// ... and put back per #2844561
// both bug reports/patches are by Tom Judge (tomjudge on sourceforge)
    add( new SplitDirection("split rules with direction 'both'"  ));

    add(new addLoopbackForRedirect(
            "add loopback to rules that permit redirected services"));
    add(new ExpandMultipleAddresses(
            "expand objects with multiple addresses"));

    add(new dropRuleWithEmptyRE("drop rules with empty rule elements"));
    add(new checkForDynamicInterfacesOfOtherObjects(
            "check for dynamic interfaces of other hosts and firewalls"));
    add(new MACFiltering("verify for MAC address filtering"));
    add(new checkForUnnumbered("check for unnumbered interfaces"));
    add(new addressRanges("expand address range objects"));
    add(new groupServicesByProtocol("split rules with different protocols"));
    add(new separateTCPWithFlags("separate TCP services with flags"));
    add(new separateSrcPort("split on TCP and UDP with source ports"));
    add(new separateTagged("split on TagService"));
    add(new separateTOS("split on IPService with TOS"));

    if (ipv6)
        add( new DropIPv4Rules("drop ipv4 rules"));
    else
        add( new DropIPv6Rules("drop ipv6 rules"));

    add(new verifyCustomServices("verify custom services for this platform"));
//	add(new ProcessScrubOption("process 'scrub' option"));
    add(new SpecialServices("check for special services"));
    add(new setQuickFlag("set 'quick' flag"));
    add(new checkForZeroAddr("check for zero addresses"));

    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));

    add(new createTables("create tables"));
//        add(new PrintTables("print tables"));

    add(new PrintRule("generate pf code"));
    add(new simplePrintProgress());

    runRuleProcessors();

}


void PolicyCompiler_pf::epilog()
{
}

/**
 * virtual method to let policy compiler check rules using
 * options specific for the given fw platform. Base class
 * PolicyCompiler has no visibility into platform-specific
 * options and can not do this.
 */
bool PolicyCompiler_pf::checkForShadowingPlatformSpecific(PolicyRule *,
                                                          PolicyRule *r2)
{
    bool quick = r2->getBool("quick");
    // if quick == false, the rule is non-terminating
    if (!quick) return false;

    return true;
}

PolicyCompiler_pf::~PolicyCompiler_pf()
{
    // if (tables) tables->detach();
}
