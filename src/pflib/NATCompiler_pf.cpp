/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: NATCompiler_pf.cpp 1372 2007-06-21 03:25:45Z vkurland $

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

#include "NATCompiler_pf.h"

#include "fwcompiler/OSConfigurator.h"

#include "fwbuilder/RuleElement.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/AddressTable.h"

#include <iostream>
#include <iomanip>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string NATCompiler_pf::myPlatformName() { return "pf"; }


int NATCompiler_pf::prolog()
{
    int n=NATCompiler::prolog();

    if ( n>0 ) 
    {
	bool found_ext=false;
	list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
	for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
        {
	    Interface *iface=dynamic_cast<Interface*>(*i);
	    assert(iface);

	    if ( iface->isExt() ) found_ext=true;
            if ( iface->isDyn())
            {
                iface->setBool("use_var_address",true);

/* dynamic interface should not have IPv4 child object(s). We issue a
 * warning if it does in a policy compiler, there is no need to repeat
 * it here
 */
                list<FWObject*> l3=iface->getByType(IPv4::TYPENAME);
                for (list<FWObject*>::iterator j=l3.begin(); j!=l3.end(); ++j) 
                    iface->remove(*j);
            }
	}

	if (!found_ext)
	    throw FWException(
                "At least one interface should be marked as external, "
                "can not configure NAT");
    }

/* pseudo-host with ip address 127.0.0.1  We'll use it for redirection
 * NAT rules
 */
    //FWObject    *grp;
    loopback_address = dbcopy->create(IPv4::TYPENAME);
    loopback_address->setName("__loopback_address__");
    loopback_address->setId("__loopback_address_id__");

    IPv4::cast(loopback_address)->setAddress(InetAddr::getLoopbackAddr());

    dbcopy->add(loopback_address,false);
    cacheObj(loopback_address);

    if (tables) tables->init(dbcopy);

    return n;
}

string NATCompiler_pf::debugPrintRule(libfwbuilder::Rule *r)
{
    NATRule   *rule=NATRule::cast(r);
    Interface *rule_iface = getCachedFwInterface(rule->getInterfaceId());

    return NATCompiler::debugPrintRule(rule)+
        " "+string( (rule_iface!=NULL)?rule_iface->getName():"") +
        " (type="+rule->getRuleTypeAsString()+")";
}


bool NATCompiler_pf::NATRuleType::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;
    tmp_queue.push_back(rule);

    if (rule->getRuleType()!=NATRule::Unknown) return true;

    RuleElementTDst *tdstre=rule->getTDst();

    //Address  *osrc=compiler->getFirstOSrc(rule);
    //Address  *odst=compiler->getFirstODst(rule);
                                             
    Address  *tsrc=compiler->getFirstTSrc(rule);
    Address  *tdst=compiler->getFirstTDst(rule);

    if (   tsrc->isAny() &&   tdst->isAny() ) {
	rule->setRuleType(NATRule::NONAT);
	return true;
    }

    if ( ! tsrc->isAny() && tdst->isAny() ) {
        rule->setRuleType(NATRule::SNAT);
	return true;
    }

    if (   tsrc->isAny() && ! tdst->isAny() ) {
/* this is load balancing rule if there are multiple objects in TDst */
        if ( tdstre->size()>1 ) rule->setRuleType(NATRule::LB);
        else
        {

            if ( compiler->complexMatch(tdst,compiler->fw) ) rule->setRuleType(NATRule::Redirect);
            else                                             rule->setRuleType(NATRule::DNAT);

//            if ( tdst->getId()==compiler->fw->getId() ) rule->setRuleType(NATRule::Redirect);
//            else                                        rule->setRuleType(NATRule::DNAT);
        }
        return true;
    }

    if ( ! tsrc->isAny() && ! tdst->isAny() ) 
    {
        rule->setRuleType(NATRule::SDNAT);
	return true;
    }

    throw FWException(_("Unsupported translation. Rule: ")+rule->getLabel());
   
    return false;
}

/*
 * This processor should be called after classifyNATRule. Should call
 * classifyNATRule after this processor again.
 *
 * This algorithm is very much specific to iptables. Platforms where
 * this simple algorithm for SDNAT rules is not appropriate, should
 * either implement equivalent of this processor using different
 * algorithm, or should catch SDNAT rules and abort in their own
 * verifyNATRule processor.
 */
bool NATCompiler_pf::splitSDNATRule::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    if ( rule->getRuleType()==NATRule::SDNAT)
    {
        RuleElementODst *odst;
        RuleElementOSrv *osrv;
        RuleElementTSrc *tsrc;
        RuleElementTDst *tdst;

/* first rule translates destination and may translate service (depends
 * on the original rule) */
        NATRule *r = NATRule::cast( compiler->dbcopy->create(NATRule::TYPENAME) );
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);
        r->setRuleType(NATRule::Unknown);
                
        tsrc=r->getTSrc();
        tsrc->clearChildren();
        tsrc->setAnyElement();

        tmp_queue.push_back(r);

/* the second rule translates source and uses translated object in
 * ODst. Since the service could have been translated by the first
 * rule, we use TSrv in OSrv */
        r = NATRule::cast( compiler->dbcopy->create(NATRule::TYPENAME) );
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);
        r->setRuleType(NATRule::Unknown);
                
        odst=r->getODst();
        odst->clearChildren();
        for (FWObject::iterator i=rule->getTDst()->begin(); i!=rule->getTDst()->end(); i++)
            odst->add( *i );

        if ( ! rule->getTSrv()->isAny())
        {
            osrv=r->getOSrv();
            osrv->clearChildren();
            for (FWObject::iterator i=rule->getTSrv()->begin(); i!=rule->getTSrv()->end(); i++)
                osrv->add( *i );
        }

        tdst=r->getTDst();
        tdst->clearChildren();
        tdst->setAnyElement();

        tmp_queue.push_back(r);
    }
    else 
        tmp_queue.push_back(rule);

    return true;
}


bool NATCompiler_pf::VerifyRules::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;
    tmp_queue.push_back(rule);

    RuleElementOSrc  *osrc=rule->getOSrc();  assert(osrc);
    RuleElementODst  *odst=rule->getODst();  assert(odst);
    RuleElementOSrv  *osrv=rule->getOSrv();  assert(osrv);

    RuleElementTSrc  *tsrc=rule->getTSrc();  assert(tsrc);
    RuleElementTDst  *tdst=rule->getTDst();  assert(tdst);
    RuleElementTSrv  *tsrv=rule->getTSrv();  assert(tsrv);

//    if (rule->getRuleType()==NATRule::LB)
//        throw FWException(_("Load balancing rules are not supported. Rule ")+rule->getLabel());

    if (rule->getRuleType()==NATRule::DNAT && odst->size()!=1)
	throw FWException(_("There should be no more than one object in original destination in the rule ")+rule->getLabel());

//    if (rule->getRuleType()==NATRule::SNAT && tsrc->size()!=1)
//	throw FWException(_("There should be no more than one object in translated source in the rule ")+rule->getLabel());

    if (osrv->getNeg())
        throw FWException(_("Negation in original service is not supported. Rule ")+rule->getLabel());

    /* bug #1276083: "Destination NAT rules". this restriction is not
     * true at least as of OpenBSD 3.5
     *
    if (rule->getRuleType()==NATRule::DNAT && osrv->isAny()) 
	throw FWException(_("Service must be specified for destination translation rule. Rule ")+rule->getLabel());
    */

    if (rule->getRuleType()==NATRule::DNAT && osrv->isAny() && !tsrv->isAny())
	throw FWException(_("Can not translate 'any' into a specific service. Rule ")+rule->getLabel());

    if (tsrc->getNeg())
        throw FWException(_("Can not use negation in translated source. Rule ")+rule->getLabel());

    if (tdst->getNeg())
        throw FWException(_("Can not use negation in translated destination. Rule ")+rule->getLabel());

    if (tsrv->getNeg())
        throw FWException(_("Can not use negation in translated service. Rule ")+rule->getLabel());

    if (tsrv->size()!=1) 
	throw FWException(_("Translated service should be 'Original' or should contain single object. Rule: ")+rule->getLabel());

    FWObject *o=tsrv->front();
    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    if ( Group::cast(o)!=NULL)
	throw FWException(_("Can not use group in translated service. Rule ")+rule->getLabel());

#if 0
    if (rule->getRuleType()==NATRule::SNAT ) 
    {
        Address* o1=compiler->getFirstTSrc(rule);
        if ( Network::cast(o1)!=NULL || AddressRange::cast(o1)!=NULL )
            throw FWException(_("Can not use network or address range object in translated source. Rule ")+rule->getLabel());
    }
#endif

    if (rule->getRuleType()==NATRule::DNAT || rule->getRuleType()==NATRule::Redirect ) 
    {
        if ( tdst->size()!=1)
            throw FWException(_("There should be no more than one object in translated destination in the rule ")+rule->getLabel());

        Address* o1=compiler->getFirstTDst(rule);
        if ( Network::cast(o1)!=NULL || AddressRange::cast(o1)!=NULL )
            throw FWException(_("Can not use network or address range object in translated destination. Rule ")+rule->getLabel());
    }


    if (rule->getRuleType()==NATRule::SNetnat && !tsrc->isAny() ) 
    {
        Network *a1=Network::cast(compiler->getFirstOSrc(rule));
        Network *a2=Network::cast(compiler->getFirstTSrc(rule));
        if ( a1==NULL || a2==NULL ||
             a1->getNetmaskPtr()->getLength()!=a2->getNetmaskPtr()->getLength() )
            throw FWException(_("Original and translated source should both be networks of the same size . Rule ")+rule->getLabel());
    }

    if (rule->getRuleType()==NATRule::DNetnat && !tsrc->isAny() ) 
    {
        Network *a1=Network::cast(compiler->getFirstODst(rule));
        Network *a2=Network::cast(compiler->getFirstTDst(rule));
        if ( a1==NULL || a2==NULL ||
             a1->getNetmaskPtr()->getLength()!=a2->getNetmaskPtr()->getLength() )
            throw FWException(_("Original and translated destination should both be networks of the same size . Rule ")+rule->getLabel());
    }

    return true;
}

bool NATCompiler_pf::splitOnOSrv::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrv  *osrv=rule->getOSrv();  assert(osrv);
    if (osrv->size()!=1) {

	for(list<FWObject*>::iterator i=osrv->begin(); i!=osrv->end(); ++i) {
	    FWObject *o= *i;
//	    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	    Service *s=Service::cast( o );
	    assert(s);

	    NATRule *r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
	    compiler->temp_ruleset->add(r);
	    r->duplicate(rule);
	    RuleElementOSrv *nosrv=r->getOSrv();
	    nosrv->clearChildren();

	    nosrv->addRef( s );

	    tmp_queue.push_back( r );
	}
    } else
	tmp_queue.push_back(rule);
    
    return true;
}

bool NATCompiler_pf::fillTranslatedSrv::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;
    tmp_queue.push_back(rule);

    Service  *osrv_o=compiler->getFirstOSrv(rule);
    Service  *tsrv_o=compiler->getFirstTSrv(rule);

    if ( ! osrv_o->isAny()  && tsrv_o->isAny() ) 
    {
	RuleElementTSrv  *tsrv=rule->getTSrv();
	tsrv->addRef(osrv_o);
    }
    return true;
}

bool NATCompiler_pf::addVirtualAddress::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;
    tmp_queue.push_back(rule);

    Address *a=NULL;
    if (rule->getRuleType()==NATRule::SNAT) a=compiler->getFirstTSrc(rule);
    else
	if (rule->getRuleType()==NATRule::DNAT) a=compiler->getFirstODst(rule);
	else  return true;
    assert(a!=NULL);
    const InetAddr *a_addr = a->getAddressPtr();

    if ( ! a->isAny() && a->getId()!=compiler->getFwId() && a_addr)
    {
	list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
	for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
        {
	    Interface *iface=dynamic_cast<Interface*>(*i);
	    assert(iface);
            const InetAddr *iface_addr = iface->getAddressPtr();
	    if (iface_addr && *a_addr == *iface_addr )
                return true;
	}
	compiler->osconfigurator->addVirtualAddressForNAT( a );
    }

    return true;
}


bool NATCompiler_pf::splitForTSrc::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;
    RuleElementTSrc  *tsrc=rule->getTSrc();  assert(tsrc);

    map<string,list<FWObject*> > interfaceGroups;

    for(list<FWObject*>::iterator i=tsrc->begin(); i!=tsrc->end(); ++i)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        Interface *iface = compiler->findInterfaceFor(Address::cast(o),
                                                      compiler->fw);
        if (iface!=NULL)
            interfaceGroups[iface->getId()].push_back(o);
    }

    if (interfaceGroups.size()<=1) tmp_queue.push_back(rule);
    else
    {
        map<string,list<FWObject*> >::iterator i;
        for (i=interfaceGroups.begin(); i!=interfaceGroups.end(); i++)
        {
            list<FWObject*> &objSubset = (*i).second;

            RuleElementTSrc  *ntsrc = NULL;
            NATRule *r = NATRule::cast(
                compiler->dbcopy->create(NATRule::TYPENAME) );
            r->duplicate(rule);
            compiler->temp_ruleset->add(r);
                
            ntsrc=r->getTSrc();
            ntsrc->clearChildren();
            ntsrc->setAnyElement();
            for (FWObject::iterator j=objSubset.begin(); j!=objSubset.end(); j++)
            {
                ntsrc->addRef(*j);
            }
            tmp_queue.push_back(r);
        }
    }
    return true;
}


bool NATCompiler_pf::AssignInterface::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    if (regular_interfaces.empty())
    {
        int n=0;
        list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
        for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
        {
            Interface *iface=Interface::cast(*i);
            assert(iface);

            if (iface->isLoopback() ||
                iface->isUnnumbered() ||
                iface->isBridgePort()) continue;

            if (n) regular_interfaces+=",";
            regular_interfaces+= iface->getName();
            n++;
        }
        if (n>1) regular_interfaces="{ "+regular_interfaces+" }";
    }

    switch ( rule->getRuleType() )
    {
    case NATRule::SNAT:
    {
	Address *a=compiler->getFirstTSrc(rule);
#if 0
        Interface *iface = compiler->findInterfaceFor(a,compiler->fw);
        if (iface!=NULL)
        {
            rule->setInterfaceId( iface->getId() );
            tmp_queue.push_back(rule);
            return true;
        }
#endif
        if ( (Interface::isA(a) || IPv4::isA(a)) && a->isChildOf(compiler->fw))
        {
            FWObject *p=a;
            while ( ! Interface::isA(p) ) p=p->getParent();
            rule->setInterfaceId( p->getId() );
            tmp_queue.push_back(rule);
            return true;
        }

/* if we appear here, then TSrc is not an interface or address of
 * an interface. Generate NAT rule without "on iface" clause
 */
        rule->setInterfaceStr("");
    }
    break;

    case NATRule::DNAT:
    {
	Address *a=compiler->getFirstODst(rule);

        if ( (Interface::isA(a) || IPv4::isA(a)) && a->isChildOf(compiler->fw))
        {
            FWObject *p=a;
            while ( ! Interface::isA(p) ) p=p->getParent();
            rule->setInterfaceId( p->getId() );
            tmp_queue.push_back(rule);
            return true;
        }

/* if we appear here, then ODst is not an interface or address of an
 * interface. If this is so, just do not specify interface for rdr
 * rule.
 */
        rule->setInterfaceStr("");
    }
    break;

    default: break;
    }

    tmp_queue.push_back(rule);
    return true;
}


/*
 *  I assume that there is always only one object in ODst, TSrc and TDst
 *  rule elements. This should have been assured by inspector VerifyRules
 */
bool NATCompiler_pf::ReplaceFirewallObjectsODst::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;
    tmp_queue.push_back(rule);

    list<FWObject*> cl;
    RuleElementODst *rel;
    Address         *obj=NULL;

    rel=rule->getODst();      assert(rel);
    obj=compiler->getFirstODst(rule);  assert(obj);

    if (obj->getId()==compiler->getFwId() ) {

	list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
	for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) {
	    Interface *interface_=Interface::cast(*i);
/*
 * update 03/20/03:
 *
 * generally we assume that if firewall object is used in the rule,
 * then any or all its interface will be used. This means that if
 * firewall is in ODst we should really use all of its interfaces, not
 * only external ones.
 */
            if (! interface_->isLoopback() ) cl.push_back(interface_);

//	    if (interface_->isExt()) cl.push_back(interface_);
	}
	if ( ! cl.empty() ) {
	    rel->clearChildren();

	    for (FWObject::iterator i1=cl.begin(); i1!=cl.end(); ++i1) 
	    {
		rel->addRef( *i1 );
	    }
	}
    }

    return true;
}

/*
 *  I assume that there is always only one object in ODst, TSrc and TDst
 *  rule elements. This should have been assured by inspector VerifyRules
 */
bool NATCompiler_pf::ReplaceFirewallObjectsTSrc::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;
    tmp_queue.push_back(rule);

    list<FWObject*> cl;
    RuleElementTSrc *rel;
    Address         *obj=NULL;

    switch (rule->getRuleType()) 
    {
    case NATRule::Masq:   return true;
    default:
	rel=rule->getTSrc();      assert(rel);
	obj=compiler->getFirstTSrc(rule);  assert(obj);

	if (obj->getId()==compiler->getFwId() ) 
        {
            Address *odst=compiler->getFirstODst(rule);

            rel->clearChildren();

            Interface *iface=compiler->findInterfaceFor(odst,compiler->fw);

            if (!odst->isAny() && !rule->getODst()->getNeg() && iface!=NULL)
                rel->addRef(iface);
            else  // else use all interfaces except loopback and unnumbered ones
            {
                list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
                for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
                {
                    Interface *iface=Interface::cast(*i);
                    if (! iface->isLoopback() &&
                        ! iface->isUnnumbered() &&
                        ! iface->isBridgePort()) 
                        rel->addRef( *i );
                }
                for (FWObject::iterator i1=cl.begin(); i1!=cl.end(); ++i1)
                    rel->addRef( *i1 );

/* it is an error if rule element is empty at this point. this could have
 * happened if all external interfaces are unnumbered */
                if (rel->size()==0)
                {
                    char errmsg[1024];
                    sprintf(errmsg,
_("Could not find suitable interface for the NAT rule %s. Perhaps all interfaces are unnumbered?"), 
                            rule->getLabel().c_str() );
                            compiler->abort(errmsg);
                }
            }
	}
    }
    return true;
}

/*
 *  I assume that there is always only one object in ODst, TSrc and TDst
 *  rule elements. This should have been assured by inspector VerifyRules
 */
bool NATCompiler_pf::ReplaceObjectsTDst::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;
    NATCompiler_pf *pf_comp=dynamic_cast<NATCompiler_pf*>(compiler);

    tmp_queue.push_back(rule);

    if (rule->getRuleType()==NATRule::Redirect) 
    {
        Service         *tsrv=compiler->getFirstTSrv(rule);
        RuleElementTDst *rel=rule->getTDst();          assert(rel);
        Address         *otdst=compiler->getFirstTDst(rule);
        Interface       *loopback=NULL;
        FWObject        *loopback_address=NULL;

/* if firewall is used in TDst in redirection rule, replace it with
 * its loopback interface
 */
        if (otdst->getId()==compiler->fw->getId())
        {
            std::list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
            for (std::list<FWObject*>::iterator i=l2.begin();
                 i!=l2.end(); ++i) 
            {
                Interface *iface = dynamic_cast<Interface*>(*i);
                assert(iface);
                if (iface->isLoopback()) 
                {
                    loopback = iface;
                    loopback_address = loopback->getFirstByType(IPv4::TYPENAME);
                }
            }

            if (loopback_address==NULL)
            {
                char errstr[1024];
                sprintf(errstr, _("Can not configure redirection NAT rule %s because loopback interface is missing.") ,
                        rule->getLabel().c_str() );
                compiler->abort(errstr);
            }

            rel->clearChildren();
            rel->addRef( loopback_address );

            pf_comp->redirect_rules.push_back(
                redirectRuleInfo( rule->getLabel(), otdst,
                                  loopback_address, tsrv ) );
        }
    }
    return true;
}


bool NATCompiler_pf::swapAddressTableObjectsInRE::processNext()
{
    NATCompiler_pf *pf_comp=dynamic_cast<NATCompiler_pf*>(compiler);
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

            string mart_id = atbl->getId()+"_runtime";
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


bool NATCompiler_pf::processMultiAddressObjectsInRE::processNext()
{
    NATCompiler_pf *pf_comp=dynamic_cast<NATCompiler_pf*>(compiler);
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );
    bool neg = re->getNeg();

    list<FWObject*> cl;

    try
    {
        for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
        {
            FWObject *o= *i;
            if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

            MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
            if (atrt!=NULL && atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            {
                if (re->size()>1 && neg)
                {
                    string err = "AddressTable object can not be used with negation in combination with other objects in the same rule element. Rule ";
                    err += rule->getLabel();
                    compiler->abort(err);
                }
                o->setBool("pf_table",true);
                string tblname = o->getName();
                string tblID = tblname + "_addressTableObject";
                pf_comp->tables->registerTable(tblname,tblID,o);
                cl.push_back(o);
            }
        }
    } catch(FWException &ex)  // TableFactory::registerTable throws exception
    {
        string err;
        err = "Can not process MultiAddress object in rule " +
            rule->getLabel() + " : " + ex.toString();
        compiler->abort( err );
    }

    if (!cl.empty())
    {
        RuleElement *nre;

        for (FWObject::iterator i=cl.begin(); i!=cl.end(); i++)
        {
            NATRule *r= NATRule::cast(compiler->dbcopy->create(NATRule::TYPENAME) );
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            nre=RuleElement::cast( r->getFirstByType(re_type) );
            nre->clearChildren();
            nre->addRef( *i );
            tmp_queue.push_back(r);
        }

        for (FWObject::iterator i=cl.begin(); i!=cl.end(); i++)
            re->removeRef( *i );

        if (!re->isAny())
            tmp_queue.push_back(rule);

        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}



void NATCompiler_pf::checkForDynamicInterfacesOfOtherObjects::findDynamicInterfaces(RuleElement *re,
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


bool NATCompiler_pf::checkForDynamicInterfacesOfOtherObjects::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    findDynamicInterfaces( rule->getOSrc() , rule );
    findDynamicInterfaces( rule->getODst() , rule );
    findDynamicInterfaces( rule->getTSrc() , rule );
    findDynamicInterfaces( rule->getTDst() , rule );

    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_pf::createTables::processNext()
{
    NATCompiler_pf *pf_comp=dynamic_cast<NATCompiler_pf*>(compiler);
    NATRule *rule=getNext(); if (rule==NULL) return false;

    RuleElementOSrc *osrc=rule->getOSrc();
    RuleElementODst *odst=rule->getODst();

    if (osrc->size()!=1) pf_comp->tables->createTablesForRE(osrc,rule);
    if (odst->size()!=1) pf_comp->tables->createTablesForRE(odst,rule);

#if 0
    RuleElementTSrc *tsrc=rule->getTSrc();
    RuleElementTDst *tdst=rule->getTDst();

    if (tsrc->size()!=1) pf_comp->tables->createTablesForRE(tsrc,rule);
    if (tdst->size()!=1) pf_comp->tables->createTablesForRE(tdst,rule);
#endif

    tmp_queue.push_back(rule);
    return true;
}


void NATCompiler_pf::compile()
{
    bool manage_virtual_addr=fwopt->getBool("manage_virtual_addr");


    cout << _(" Compiling NAT rules for ") << fw->getName() << " ..." << endl << flush;

    try {

	Compiler::compile();

        add( new Begin());
        add( new printTotalNumberOfRules() );

        add( new recursiveGroupsInOSrc("check for recursive groups in OSRC") );
        add( new recursiveGroupsInODst("check for recursive groups in ODST") );
        add( new recursiveGroupsInOSrv("check for recursive groups in OSRV") );

        add( new recursiveGroupsInTSrc("check for recursive groups in TSRC") );
        add( new recursiveGroupsInTDst("check for recursive groups in TDST") );
        add( new recursiveGroupsInTSrv("check for recursive groups in TSRV") );

        add( new emptyGroupsInOSrc(    "check for empty groups in OSRC"    ) );
        add( new emptyGroupsInODst(    "check for empty groups in ODST"    ) );
        add( new emptyGroupsInOSrv(    "check for empty groups in OSRV"    ) );

        add( new emptyGroupsInTSrc(    "check for empty groups in TSRC"    ) );
        add( new emptyGroupsInTDst(    "check for empty groups in TDST"    ) );
        add( new emptyGroupsInTSrv(    "check for empty groups in TSRV"    ) );

        add( new ExpandGroups( "expand groups" ) );
        add( new eliminateDuplicatesInOSRC(  "eliminate duplicates in OSRC") );
        add( new eliminateDuplicatesInODST(  "eliminate duplicates in ODST") );
        add( new eliminateDuplicatesInOSRV(  "eliminate duplicates in OSRV") );

        add( new swapMultiAddressObjectsInOSrc(
                 " swap MultiAddress -> MultiAddressRunTime in OSrc") );
        add( new swapMultiAddressObjectsInODst(
                 " swap MultiAddress -> MultiAddressRunTime in ODst") );

        add( new swapAddressTableObjectsInOSrc(
                 "AddressTable -> MultiAddressRunTime in OSrc") );
        add( new swapAddressTableObjectsInODst(
                 "AddressTable -> MultiAddressRunTime in ODst") );
        add( new swapAddressTableObjectsInTDst(
                 "AddressTable -> MultiAddressRunTime in TDst") );

        add( new processMultiAddressObjectsInOSrc(
                 "process MultiAddress objects in OSrc") );
        add( new processMultiAddressObjectsInODst(
                 "process MultiAddress objects in ODst") );
        add( new processMultiAddressObjectsInTDst(
                 "process MultiAddress objects in TDst") );

        add( new splitOnOSrv( "split rule on original service" ) );
        add( new fillTranslatedSrv( "fill translated service" ) );

        //add( new doOSrcNegation( "process negation in OSrc" ) );
        //add( new doODstNegation( "process negation in ODst" ) );
        //add( new doOSrvNegation( "process negation in OSrv" ) );

        add( new NATRuleType( "determine NAT rule types" ) );
        add( new splitSDNATRule("split SDNAT rules"      ) );
        add( new NATRuleType( "determine NAT rule types" ) );
        add( new VerifyRules( "verify NAT rules" ) );
        //add( new splitODstForSNAT(
        //       "split rule if objects in ODst belong to different subnets"));
        add( new ReplaceFirewallObjectsODst(
                 "replace references to the firewall in ODst" ) );
        add( new ReplaceFirewallObjectsTSrc(
                 "replace references to the firewall in TSrc" ) );
        add( new ReplaceObjectsTDst( "replace objects in TDst" ) );

        add( new ExpandMultipleAddresses( "expand multiple addresses" ) );

	if ( manage_virtual_addr )
            add( new addVirtualAddress("add virtual addresses for NAT rules"));

        add( new checkForUnnumbered("check for unnumbered interfaces" ) );
        add( new checkForDynamicInterfacesOfOtherObjects(
                 "check for dynamic interfaces of other hosts and firewalls"));
        add( new ExpandAddressRanges( "expand address range objects" ) );
        //add( new ConvertToAtomicForTSrc( "convert to atomic rules" ) );
        add( new splitForTSrc(
                 "split if addresses in TSrc belong to different networks" ));
        add( new AssignInterface( "assign rules to interfaces" ) );
        add( new convertInterfaceIdToStr("prepare interface assignments") );

        add( new createTables(      "create tables"    ) );
//        add( new PrintTables(       "print tables"     ) );

        add( new PrintRule("generate pf code") );
        add( new simplePrintProgress() );

        runRuleProcessors();


    } catch (FWException &ex) {
	error(ex.toString());
	exit(1);
    }
}


void NATCompiler_pf::epilog()
{
}
