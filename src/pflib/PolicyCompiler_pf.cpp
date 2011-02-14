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

#include "PolicyCompiler_pf.h"
#include "NATCompiler_pf.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/StateSyncClusterGroup.h"

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
                char errstr[256];
                sprintf(errstr,
                        "Dynamic interface %s should not have an IP address object attached to it. This IP address object will be ignored.",
                        iface->getName().c_str() );
                warning(errstr );
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
    Rule *rule=prev_processor->getNextRule(); if (rule==NULL) return false;

    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );

    list<MultiAddress*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
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
        if (AddressTable::cast(o)!=NULL &&
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
            if (mart==NULL)
            {
                mart = new MultiAddressRunTime(atbl);

                // need to ensure stable ID for the runtime object, so
                // that when the same object is replaced in different
                // rulesets by different compiler passes, chosen
                // runtime object has the same ID and is identified as
                // the same by the compiler.

                mart->setId( mart_id );
                compiler->dbcopy->addToIndex(mart);
                compiler->dbcopy->add(mart);

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );
    bool neg = re->getNeg();

    list<FWObject*> maddr_runtime;

    try
    {
        for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
        {
            FWObject *o= *i;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

            MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
            if (atrt!=NULL &&
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    PolicyRule     *r;
    RuleElementSrc *src=rule->getSrc();    assert(src);

    if (src->size()==1 || src->getNeg())
    {
	tmp_queue.push_back(rule);
	return true;
    }
    FWObject       *fw_in_src=NULL;
    vector<FWObject*> cl;
    for (FWObject::iterator i1=src->begin(); i1!=src->end(); ++i1) {

	FWObject *o   = *i1;
	FWObject *obj = NULL;
//	if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
	if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
	if (obj==NULL)
            compiler->abort(rule, "Broken Src object");

	if (obj->getId()==compiler->getFwId()) {
	    fw_in_src=o;   // can not remove right now because remove invalidates iterator

	    RuleElementSrc *nsrc;

	    r= compiler->dbcopy->createPolicyRule();
	    compiler->temp_ruleset->add(r);
	    r->duplicate(rule);
	    nsrc=r->getSrc();
	    nsrc->clearChildren();
	    nsrc->setAnyElement();
	    nsrc->addRef( compiler->fw );
	    tmp_queue.push_back(r);
	}
    }
    if (fw_in_src!=NULL) src->remove( fw_in_src );

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_pf::splitIfFirewallInDst::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    PolicyRule     *r;
    RuleElementDst *dst=rule->getDst();    assert(dst);

    if (dst->size()==1 || dst->getNeg())
    {
	tmp_queue.push_back(rule);
	return true;
    }

    FWObject       *fw_in_dst=NULL;
    vector<FWObject*> cl;
    for (FWObject::iterator i1=dst->begin(); i1!=dst->end(); ++i1)
    {
	FWObject *o   = *i1;
	FWObject *obj = NULL;
//	if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
	if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
	if (obj==NULL)
            compiler->abort(rule, "Broken Dst");

	if (obj->getId()==compiler->getFwId()) {
	    fw_in_dst=o;   // can not remove right now because remove invalidates iterator

	    RuleElementDst *ndst;

	    r= compiler->dbcopy->createPolicyRule();
	    compiler->temp_ruleset->add(r);
	    r->duplicate(rule);
	    ndst=r->getDst();
	    ndst->clearChildren();
	    ndst->setAnyElement();
	    ndst->addRef( compiler->fw );
	    tmp_queue.push_back(r);
	}
    }
    if (fw_in_dst!=NULL) dst->remove( fw_in_dst );

    tmp_queue.push_back(rule);
    return true;
}



bool PolicyCompiler_pf::fillDirection::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
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

        if (src==NULL || dst==NULL)
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

/*
 * add default and auto-generated rules, except when we process rules
 * in a branch (i.e. anchor, since this is PF)
 */
void PolicyCompiler_pf::addDefaultPolicyRule()
{
    if (getSourceRuleSet()->isTop() && !inSingleRuleCompileMode())
    {
        if (getCachedFwOpt()->getBool("mgmt_ssh") &&
             !getCachedFwOpt()->getStr("mgmt_addr").empty())
        {
            PolicyRule *r;
            TCPService *ssh = dbcopy->createTCPService();
            ssh->setDstRangeStart(22);
            ssh->setDstRangeEnd(22);

            ssh->setName("mgmt_ssh");
            dbcopy->add(ssh,false);

            string mgmt_addr = getCachedFwOpt()->getStr("mgmt_addr");
            InetAddr  addr;
            InetAddr netmask(InetAddr::getAllOnes());
            try
            {
                addr = InetAddr(mgmt_addr);
                string::size_type sep = mgmt_addr.find("/");
                if (sep != string::npos)
                {
                    addr = InetAddr(mgmt_addr.substr(0,sep));
                    string nm = mgmt_addr.substr(sep+1);
                    int o1,o2,o3,o4;
                    if (sscanf(nm.c_str(),
                               "%3u.%3u.%3u.%3u", &o1, &o2, &o3, &o4)==4)
                    {
                        netmask = InetAddr(nm);
                    } else
                    {
                        sscanf(nm.c_str(),"%u",&o1);
                        netmask = InetAddr(o1);
                    }
                }
            } catch(FWException &ex)
            {
                char errstr[256];
                sprintf(errstr,
                        "Invalid address for the backup ssh access: '%s'",
                        mgmt_addr.c_str());
                abort(errstr);
            }

            Network *mgmt_workstation = dbcopy->createNetwork();
            mgmt_workstation->setName("mgmt_addr");
            mgmt_workstation->setAddress(addr);
            mgmt_workstation->setNetmask(netmask);
//        IPv4 *mgmt_workstation = IPv4::cast(dbcopy->create(IPv4::TYPENAME));
//        mgmt_workstation->setAddress(getCachedFwOpt()->getStr("mgmt_addr"));
            dbcopy->add(mgmt_workstation,false);

            r = dbcopy->createPolicyRule();
            temp_ruleset->add(r);
            r->setAction(PolicyRule::Accept);
            r->setLogging(false);
            r->setDirection(PolicyRule::Inbound);
            r->setPosition(9998);
            r->setComment("   backup ssh access rule ");
            r->setHidden(true);
            r->setFallback(false);
            r->setLabel("backup ssh access rule");
            r->setBool("needs_established",true);  // supported in ipfw

            RuleElement *src = r->getSrc();
            assert(src!=NULL);
            src->addRef(mgmt_workstation);

            RuleElement *dst = r->getDst();
            assert(dst!=NULL);
            dst->addRef(fw);

            RuleElement *srv = r->getSrv();
            assert(srv!=NULL);
            srv->addRef(ssh);

            combined_ruleset->push_front(r);

        }

        insertCarpRule();
        insertPfsyncRule();

        PolicyRule *r = dbcopy->createPolicyRule();
        FWOptions *ruleopt;
        temp_ruleset->add(r);
        r->setAction(PolicyRule::Deny);
        r->setLogging(getCachedFwOpt()->getBool("fallback_log"));
        r->setDirection(PolicyRule::Both);
        r->setPosition(10000);
        r->setComment("   fallback rule ");
        r->setHidden(true);
        r->setFallback(true);
        r->setLabel("fallback rule");
        ruleopt = r->getOptionsObject();
        ruleopt->setBool("stateless", true);
        combined_ruleset->push_back(r);
    }
}

bool PolicyCompiler_pf::SpecialServices::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    tmp_queue.push_back(rule);

    RuleElementSrv *srv=rule->getSrv();

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++)
    {
	FWObject *o= *i;
	if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	Service *s=Service::cast( o );
	assert(s);

	if (IPService::cast(s)!=NULL  && rule->getAction()==PolicyRule::Accept)
        {
            rule->setBool("allow_opts", IPService::constcast(s)->hasIpOptions());
	}
    }
    return true;
}

bool PolicyCompiler_pf::SplitDirection::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if (rule->getDirection()==PolicyRule::Both &&
        rule->getAction()==PolicyRule::Route)
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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    tmp_queue.push_back(rule);

    if ( rule->getAction()!=PolicyRule::Scrub &&
         rule->getAction()!=PolicyRule::Accounting &&
         rule->getAction()!=PolicyRule::Tag &&
         rule->getAction()!=PolicyRule::Branch
    ) rule->setBool("quick",true);

    return true;
}

bool PolicyCompiler_pf::doSrcNegation::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    PolicyCompiler_pf *pf_comp=dynamic_cast<PolicyCompiler_pf*>(compiler);

//    RuleElementSrc *src=rule->getSrc();
    RuleElementDst *dst=rule->getDst();
    RuleElementSrv *srv=rule->getSrv();

    if (pf_comp->redirect_rules_info==NULL)
        compiler->abort(
            rule, 
            "addLoopbackForRedirect needs a valid pointer to "
            "the list<NATCompiler_pf::redirectRuleInfo> object");

    tmp_queue.push_back(rule);

    //const list<NATCompiler_pf::redirectRuleInfo> lst = 
    //  pf_comp->natcmp->getRedirRulesInfo();

    if (pf_comp->redirect_rules_info->empty()) return true;

/*
 *  struct redirectRuleInfo {
 *    string   natrule_label;
 *    Address *tdst;
 *    Service *tsrv;
 *  };
 */

    for (FWObject::iterator i=srv->begin(); i!=srv->end(); i++) 
    {
	FWObject *o1= *i;
	if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
	Service *s=Service::cast( o1 );
	assert(s);

        for (FWObject::iterator j=dst->begin(); j!=dst->end(); j++) 
        {
            FWObject *o2= *j;
            if (FWReference::cast(o2)!=NULL) o2=FWReference::cast(o2)->getPointer();
            Address *a=Address::cast( o2 );
            assert(a);

            list<NATCompiler_pf::redirectRuleInfo>::const_iterator k;
            for (k=pf_comp->redirect_rules_info->begin();
                 k!=pf_comp->redirect_rules_info->end(); ++k)
            {
                if ( *a == *(k->old_tdst) &&  *s == *(k->tsrv) )
                {
// insert address used for redirection in the NAT rule.
                    dst->addRef( k->new_tdst );
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
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs   =Interface::cast( obj );

        if (ifs!=NULL    &&
            ifs->isDyn() &&
            ifs->getParent()->getId()!=compiler->fw->getId() &&
            ! ifs->getParent()->getBool("pf_table") )
        {
            char errstr[2048];
            sprintf(errstr,"Can not build rule using dynamic interface '%s' of the object '%s' because its address in unknown. Rule %s",
                    ifs->getName().c_str(), 
                    ifs->getParent()->getName().c_str(),
                    rule->getLabel().c_str() );

            compiler->abort(rule, errstr);
        }
    }
}


bool PolicyCompiler_pf::checkForDynamicInterfacesOfOtherObjects::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    findDynamicInterfaces( rule->getSrc() , rule );
    findDynamicInterfaces( rule->getDst() , rule );

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_pf::splitIfInterfaceInRE::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

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
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

        Interface *interface_=Interface::cast(o);
        if (interface_!=NULL && interface_->isDyn())
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
    PolicyCompiler_pf *pf_comp=dynamic_cast<PolicyCompiler_pf*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementSrc *src=rule->getSrc();
    RuleElementDst *dst=rule->getDst();
//    RuleElementSrv *srv=rule->getSrv();

    if (src->size()!=1) pf_comp->tables->createTablesForRE(src,rule);
    if (dst->size()!=1) pf_comp->tables->createTablesForRE(dst,rule);
//    if (srv->size()!=1) createTablesForRE(srv,rule);

    tmp_queue.push_back(rule);
    return true;
}


bool PolicyCompiler_pf::printScrubRule::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
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

    addDefaultPolicyRule();
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
            "replace cluster interfaces with member interfaces in the Interface rule element"));
    add(new ItfNegation("process negation in Itf"));

    //add(new InterfacePolicyRules(
    //    "process interface policy rules and store interface ids"));

    add(new splitIfFirewallInSrc("split rule if firewall is in Src"));
    add(new splitIfFirewallInDst("split rule if firewall is in Dst"));
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
    add(new convertInterfaceIdToStr("prepare interface assignments"));

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

void PolicyCompiler_pf::insertCarpRule()
{
    /* Add CARP-Service to database */
    IPService* carp_service = IPService::cast(dbcopy->create(IPService::TYPENAME));
    carp_service->setComment("CARP service");
    carp_service->setProtocolNumber(112);
    dbcopy->add(carp_service);

    FWObjectTypedChildIterator interfaces = fw->findByType(Interface::TYPENAME);
    for (; interfaces != interfaces.end(); ++interfaces) 
    {
        Interface *iface = Interface::cast(*interfaces);

        if (iface->isFailoverInterface())
        {
            FWObject *failover_group =
                iface->getFirstByType(FailoverClusterGroup::TYPENAME);
            if (failover_group->getStr("type") == "carp")
            {
                /* Add automatic rules for CARP 
                 * Rule should be associated with physical interface
                 */
                
                string phys_iface_name = iface->getOptionsObject()->getStr("base_device");
                Interface *phys_iface = Interface::cast(
                    fw->findObjectByName(Interface::TYPENAME, phys_iface_name));
                if (phys_iface)
                {
                    PolicyRule *rule = 
                        addMgmtRule(NULL, NULL, carp_service, phys_iface,
                                    PolicyRule::Both, PolicyRule::Accept, "CARP");
                    FWOptions *ruleopt = rule->getOptionsObject();
                    assert(ruleopt!=NULL);
                    ruleopt->setBool("firewall_is_part_of_any_and_networks", false);
                } else
                {
                    warning(
                        "Can not find interface " + phys_iface_name + 
                        " for the CARP interface " + iface->getName() +
                        " of the cluster");
                }
            }
        }
    }
}

void PolicyCompiler_pf::insertPfsyncRule()
{
    /* Add pfsync service to database */
    IPService* pfsync_service = IPService::cast(dbcopy->create(IPService::TYPENAME));
    pfsync_service->setComment("pfsync service");
    pfsync_service->setProtocolNumber(240);
    dbcopy->add(pfsync_service);

    FWObjectTypedChildIterator interfaces = fw->findByType(Interface::TYPENAME);
    for (; interfaces != interfaces.end(); ++interfaces) 
    {
        Interface *iface = Interface::cast(*interfaces);
        if (iface->getOptionsObject()->getBool("state_sync_group_member"))
        {
            FWObject *state_sync_group = dbcopy->findInIndex(
                dbcopy->getIntId(
                    iface->getOptionsObject()->getStr("state_sync_group_id")));
            assert(state_sync_group!=NULL);
            if (state_sync_group && state_sync_group->getStr("type") == "pfsync")
            {
                PolicyRule *rule = addMgmtRule(NULL, NULL, pfsync_service, iface,
                                               PolicyRule::Both,
                                               PolicyRule::Accept, "pfsync");
                FWOptions *ruleopt = rule->getOptionsObject();
                assert(ruleopt!=NULL);
                ruleopt->setBool("firewall_is_part_of_any_and_networks", false);
            }
        }
    }
}

/**
 * virtual method to let policy compiler check rules using
 * options specific for the given fw platform. Base class
 * PolicyCompiler has no visibility into platform-specific
 * options and can not do this.
 */
bool PolicyCompiler_pf::checkForShadowingPlatformSpecific(PolicyRule *r1,
                                                          PolicyRule *r2)
{
    PolicyRule::Action r2_action = r2->getAction();

    // Tag action is non-terminating and does not shadow anything
    if (r2_action == PolicyRule::Tag) return false;

    return true;
}


