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


#include "NATCompiler_pf.h"

#include "fwcompiler/OSConfigurator.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"

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
	list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
	for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
        {
	    Interface *iface=dynamic_cast<Interface*>(*i);
	    assert(iface);

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
    }

/* pseudo-host with ip address 127.0.0.1  We'll use it for redirection
 * NAT rules
 */
    //FWObject    *grp;
    loopback_address = dbcopy->createIPv4();
    loopback_address->setName("__loopback_address__");
    loopback_address->setId(FWObjectDatabase::generateUniqueId()); // "__loopback_address_id__");

    IPv4::cast(loopback_address)->setAddress(InetAddr::getLoopbackAddr());

    persistent_objects->add(loopback_address,false);

    if (tables)
    {
        tables->init(dbcopy);
        if (!getSourceRuleSet()->isTop())
            tables->setRuleSetName(getRuleSetName());
    }

    return n;
}

string NATCompiler_pf::debugPrintRule(libfwbuilder::Rule *r)
{
    NATRule *rule = NATRule::cast(r);
    RuleElementItfOutb *itf_re = rule->getItfOutb();
    FWObject *rule_iface = FWObjectReference::getObject(itf_re->front());
    // FWObject *rule_iface = dbcopy->findInIndex(rule->getInterfaceId());

    return NATCompiler::debugPrintRule(rule) +
        " " + string( (rule_iface!=nullptr)?rule_iface->getName():"") +
        " (type=" + rule->getRuleTypeAsString() + ")";
}

void NATCompiler_pf::_expand_addr(Rule *rule,
                                  FWObject *s,
                                  bool expand_cluster_interfaces_fully)
{
    if (RuleElementTSrc::isA(s))
    {
        // do not replace interfaces with their ip addresses in TSrc
        // to be able to generate "nat ... -> (em0)" command later
        list<FWObject*> interfaces_in_re;
        for (FWObject::iterator i1=s->begin(); i1!=s->end(); ++i1) 
        {
            FWObject *o = FWReference::getObject(*i1);
            assert(o);
            if (Interface::isA(o))
                interfaces_in_re.push_back(o);
        }
        if (interfaces_in_re.size() > 1)
        {
            for (list<FWObject*>::iterator i=interfaces_in_re.begin();
                 i!=interfaces_in_re.end(); ++i) s->removeRef(*i);

            NATCompiler::_expand_addr(
                rule, s, expand_cluster_interfaces_fully);

            for (list<FWObject*>::iterator i=interfaces_in_re.begin();
                 i!=interfaces_in_re.end(); ++i) s->addRef(*i);

        }
    } else
        NATCompiler::_expand_addr(
            rule, s, expand_cluster_interfaces_fully);
}

bool NATCompiler_pf::NATRuleType::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    if (rule->getRuleType()!=NATRule::Unknown) return true;

    RuleElementTSrc *tsrcre = rule->getTSrc();
    RuleElementTDst *tdstre = rule->getTDst();
    RuleElementTSrv *tsrvre = rule->getTSrv();

    Service  *osrv=compiler->getFirstOSrv(rule);
                                             
    Address *tsrc = compiler->getFirstTSrc(rule);
    Address *tdst = compiler->getFirstTDst(rule);
    Service *tsrv=compiler->getFirstTSrv(rule);

    if (rule->getAction() == NATRule::Branch)
    {
	rule->setRuleType(NATRule::NATBranch);
        if (!tsrcre->isAny() || !tdstre->isAny() || !tsrvre->isAny())
        {
            tsrcre->clearChildren();
            tsrcre->setAnyElement();

            tdstre->clearChildren();
            tdstre->setAnyElement();

            tsrvre->clearChildren();
            tsrvre->setAnyElement();

            compiler->warning(
                    rule,
                    "Translated Src, Dst and Srv are ignored in the NAT "
                    "rule with action 'Branch'");
        }
        return true;
    }

    if (tsrc->isAny() && tdst->isAny() && 
        (tsrv->isAny() || (tsrv->getId() == osrv->getId()))
    )
    {
	rule->setRuleType(NATRule::NONAT);
	return true;
    }

    bool osrv_defines_src_port = false;
    Q_UNUSED(osrv_defines_src_port);
    bool osrv_defines_dst_port = false;
    Q_UNUSED(osrv_defines_dst_port);
    bool tsrv_translates_src_port = false;
    bool tsrv_translates_dst_port = false;

    if (TCPUDPService::cast(osrv))
    {
        TCPUDPService *tu_osrv = TCPUDPService::cast(osrv);

        osrv_defines_src_port =                                         \
            (tu_osrv->getSrcRangeStart() != 0 && tu_osrv->getDstRangeStart() == 0);
        osrv_defines_dst_port =                                         \
            (tu_osrv->getSrcRangeStart() == 0 && tu_osrv->getDstRangeStart() != 0);
    }

    if (TCPUDPService::cast(tsrv))
    {
        TCPUDPService *tu_tsrv = TCPUDPService::cast(tsrv);

        tsrv_translates_src_port =                                      \
            (tu_tsrv->getSrcRangeStart() != 0 && tu_tsrv->getDstRangeStart() == 0);
        tsrv_translates_dst_port =                                      \
            (tu_tsrv->getSrcRangeStart() == 0 && tu_tsrv->getDstRangeStart() != 0);
    }


    if (
        (! tsrc->isAny() && tdst->isAny()) ||
        (tsrc->isAny() && tdst->isAny() && tsrv_translates_src_port)
    )
    {
        rule->setRuleType(NATRule::SNAT);
	return true;
    }

    if (
        (tsrc->isAny() && ! tdst->isAny()) ||
        (tsrc->isAny() && tdst->isAny() && tsrv_translates_dst_port)
    )
    {
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

    if (
        ( ! tsrc->isAny() && ! tdst->isAny() ) ||
        ( ! tsrc->isAny() && tsrv_translates_dst_port) ||
        ( ! tdst->isAny() && tsrv_translates_src_port)
    )
    {
        rule->setRuleType(NATRule::SDNAT);
	return true;
    }

    compiler->abort(rule, "Unsupported translation.");
   
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
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    if ( rule->getRuleType()==NATRule::SDNAT)
    {
        RuleElementODst *odst;
        RuleElementOSrv *osrv;
        RuleElementTSrc *tsrc;
        RuleElementTDst *tdst;

/* first rule translates destination and may translate service (depends
 * on the original rule) */
        NATRule *r =  compiler->dbcopy->createNATRule();
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
        r = compiler->dbcopy->createNATRule();
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);
        r->setRuleType(NATRule::Unknown);
                
        odst=r->getODst();
        odst->clearChildren();
        for (FWObject::iterator i=rule->getTDst()->begin(); i!=rule->getTDst()->end(); i++)
        {
            FWObject *o = FWReference::getObject(*i);
            odst->addRef(o);
        }

        if ( ! rule->getTSrv()->isAny())
        {
            /*
             * See "pf flow diagram" at  http://homepage.mac.com/quension/pf/flow.png
             * rdr happens first, then nat. This means nat sees packet with
             * translated destination address and port.
             *
             * If the first rule in the pair translated service and
             * changed destination port, we need to match it in the
             * second rule to only trsnslate source in the packets
             * that have been processed by the first rule. However
             * this only applies to the case when destination port has
             * been translated because the first rule uses DNAT which
             * can only translate dest. port. So, if TSrv has zero
             * dest.  port range but non-zero source port range, we
             * should not match it here because in this case no
             * dest. port translation occurs.  If TSrv translates both
             * source and destination ports, we create new TCP(UDP)
             * service object with only dest. port part and use it to
             * match.
             */
            Service *tsrv = compiler->getFirstTSrv(rule);
            TCPUDPService *tu_tsrv = TCPUDPService::cast(tsrv);
            if (tu_tsrv && tu_tsrv->getDstRangeStart() != 0)
            {
                TCPUDPService *match_service = nullptr;
                if (tu_tsrv->getSrcRangeStart() == 0)
                {
                    // no source port tranlsation
                    match_service = tu_tsrv;
                } else
                {
                    // both source and dest port translation occurs
                    match_service = TCPUDPService::cast(
                        compiler->dbcopy->create(tsrv->getTypeName()));
                    match_service->setName(tsrv->getName() + "_dport");
                    compiler->persistent_objects->add(match_service);
                    match_service->setDstRangeStart(tu_tsrv->getDstRangeStart());
                    match_service->setDstRangeEnd(tu_tsrv->getDstRangeEnd());
                }
                osrv = r->getOSrv();
                osrv->clearChildren();
                osrv->addRef(match_service);
            }
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
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    string version = compiler->fw->getStr("version");

#ifndef NDEBUG
    RuleElementOSrc  *osrc=rule->getOSrc();  assert(osrc);
    RuleElementODst  *odst=rule->getODst();  assert(odst);
#endif
    RuleElementOSrv  *osrv=rule->getOSrv();  assert(osrv);

    RuleElementTSrc  *tsrc=rule->getTSrc();  assert(tsrc);
    RuleElementTDst  *tdst=rule->getTDst();  assert(tdst);
    RuleElementTSrv  *tsrv=rule->getTSrv();  assert(tsrv);

    /*
     * because of the change in the nat and rdr rules syntax in
     * 4.7, I can no longer implement no-nat rules correctly for
     * this version. They dropped the "no" keyword and their
     * examples suggest using "pass" to implement exclusions for
     * the nat rules. I need no-nat rule to just not translate but
     * not make a decision whether the packet should be passed or
     * dropped. In the new PF model, translation rules are just
     * options on the matching policy rules and they do not offer
     * any keyword or option to not translate.
     */
    if (rule->getRuleType()==NATRule::NONAT &&
        XMLTools::version_compare(version, "4.7")>=0)
    {
        compiler->abort(
            rule,
            "No translation rules are not supported for PF 4.7, "
            "use negation to implement exclusions");
        return true;
    }

    if (osrv->getNeg())
    {
        compiler->abort(
            rule, 
            "Negation in original service is not supported.");
        return true;
    }

    /* bug #1276083: "Destination NAT rules". this restriction is not
     * true at least as of OpenBSD 3.5
     *
    if (rule->getRuleType()==NATRule::DNAT && osrv->isAny()) 
	compiler->abort("Service must be specified for destination translation rule. Rule "+rule->getLabel());
    */

    if (rule->getRuleType()==NATRule::DNAT && osrv->isAny() && !tsrv->isAny())
    {
	compiler->abort(
            rule, 
            "Can not translate 'any' into a specific service.");
        return true;
    }

    if (tsrc->getNeg())
    {
        compiler->abort(
            rule, 
            "Can not use negation in translated source.");
        return true;
    }

    if (tdst->getNeg())
    {
        compiler->abort(
            rule, 
            "Can not use negation in translated destination.");
        return true;
    }

    if (tsrv->getNeg())
    {
        compiler->abort(
            rule, 
            "Can not use negation in translated service.");
        return true;
    }

    if (tsrv->size()!=1) 
    {
	compiler->abort(
            rule, 
            "Translated service should be 'Original' or should contain single object.");
        return true;
    }

    FWObject *o=tsrv->front();
    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

    if ( Group::cast(o)!=nullptr)
    {
	compiler->abort(
            rule, 
            "Can not use group in translated service.");
        return true;
    }

#if 0
    if (rule->getRuleType()==NATRule::SNAT ) 
    {
        Address* o1=compiler->getFirstTSrc(rule);
        if ( Network::cast(o1)!=nullptr || AddressRange::cast(o1)!=nullptr )
            compiler->abort("Can not use network or address range object in translated source. Rule "+rule->getLabel());
    }
#endif

    if (rule->getRuleType()==NATRule::SNAT ) 
    {
        if (tsrc->isAny())
        {
            compiler->abort(rule, 
                            "Source translation rule needs an address in "
                            "Translated Source.");
            return true;
        }

        FWObject *o = FWReference::getObject(tsrc->front());
        if (Interface::isA(o) && Interface::cast(o)->isUnnumbered())
        {
            compiler->abort(rule,
                            "Can not use unnumbered interface in "
                            "Translated Source of a Source translation rule.");
            return true;
        }

    }

    if (rule->getRuleType()==NATRule::DNAT || rule->getRuleType()==NATRule::Redirect ) 
    {
        if (tdst->isAny())
        {
            compiler->abort(
                rule, 
                "Destination translation rule needs an address in "
                "Translated Destination.");
            return true;
        }

        if ( tdst->size()!=1)
        {
            compiler->abort(
                rule, 
                "There should be no more than one object in translated destination");
            return true;
        }

        Address* o1=compiler->getFirstTDst(rule);
        if ( Network::cast(o1)!=nullptr || AddressRange::cast(o1)!=nullptr )
        {
            compiler->abort(
                rule, 
                "Can not use network or address range object in translated destination.");
            return true;
        }

    }


    if (rule->getRuleType()==NATRule::SNetnat && !tsrc->isAny() ) 
    {
        Network *a1=Network::cast(compiler->getFirstOSrc(rule));
        Network *a2=Network::cast(compiler->getFirstTSrc(rule));
        if ( a1==nullptr || a2==nullptr ||
             a1->getNetmaskPtr()->getLength()!=a2->getNetmaskPtr()->getLength() )
        {
            compiler->abort(
                rule, 
                "Original and translated source should both be networks of the same size.");
            return true;
        }

    }

    if (rule->getRuleType()==NATRule::DNetnat && !tsrc->isAny() ) 
    {
        Network *a1=Network::cast(compiler->getFirstODst(rule));
        Network *a2=Network::cast(compiler->getFirstTDst(rule));
        if ( a1==nullptr || a2==nullptr ||
             a1->getNetmaskPtr()->getLength()!=a2->getNetmaskPtr()->getLength() )
        {
            compiler->abort(
                rule, 
                "Original and translated destination should both be networks of the same size.");
            return true;
        }

    }

    if (rule->getRuleType()==NATRule::NATBranch ) 
    {
        RuleSet *branch = rule->getBranch();
        if (branch == nullptr)
        {
            compiler->abort(
                rule, 
                "Action 'Branch' needs NAT rule set to point to");
            return true;
        } else
        {
            if (!NAT::isA(branch))
            {
                compiler->abort(
                    rule, 
                    "Action 'Branch' must point to a NAT rule set "
                    "(points to " + branch->getTypeName() + ")");
                return true;
            }

        }
    }

    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_pf::splitOnOSrv::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrv  *osrv=rule->getOSrv();  assert(osrv);
    if (osrv->size()!=1)
    {
	for(list<FWObject*>::iterator i=osrv->begin(); i!=osrv->end(); ++i)
        {
	    FWObject *o= *i;
//	    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
	    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
	    Service *s=Service::cast( o );
	    assert(s);

	    NATRule *r= compiler->dbcopy->createNATRule();
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
    NATRule *rule=getNext(); if (rule==nullptr) return false;
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
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    Address *a=nullptr;
    if (rule->getRuleType()==NATRule::SNAT) a=compiler->getFirstTSrc(rule);
    else
	if (rule->getRuleType()==NATRule::DNAT) a=compiler->getFirstODst(rule);
	else  return true;
    assert(a!=nullptr);
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
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElementTSrc  *tsrc=rule->getTSrc();  assert(tsrc);

    map<int,list<FWObject*> > interfaceGroups;

    for(list<FWObject*>::iterator i=tsrc->begin(); i!=tsrc->end(); ++i)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
        Interface *iface = compiler->findInterfaceFor(Address::cast(o),
                                                      compiler->fw);
        if (iface!=nullptr)
            interfaceGroups[iface->getId()].push_back(o);
    }

    if (interfaceGroups.size()<=1) tmp_queue.push_back(rule);
    else
    {
        map<int,list<FWObject*> >::iterator i;
        for (i=interfaceGroups.begin(); i!=interfaceGroups.end(); i++)
        {
            list<FWObject*> &objSubset = (*i).second;

            RuleElementTSrc  *ntsrc = nullptr;
            NATRule *r = compiler->dbcopy->createNATRule();
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


bool NATCompiler_pf::assignInterfaceToNATRule(NATRule *rule, Address *addr)
{
    RuleElementItfOutb *itf_re = rule->getItfOutb();
    assert(itf_re!=nullptr);

    if (Interface::isA(addr) || IPv4::isA(addr))
    {
        FWObject *p = addr;
        while ( p && ! Interface::isA(p) ) p = p->getParent();
        Interface *intf = Interface::cast(p);

        if (intf && intf->isFailoverInterface())
        {
            FailoverClusterGroup *fg = FailoverClusterGroup::cast(
                intf->getFirstByType(FailoverClusterGroup::TYPENAME));
            if (fg)
                intf = fg->getInterfaceForMemberFirewall(fw);
        }

        if (intf && intf->isChildOf(fw))
        {
            if ( ! itf_re->hasRef(intf)) itf_re->addRef(intf);
            return true;
        }
    }
    return false;
}

bool NATCompiler_pf::AssignInterface::processNext()
{
    NATCompiler_pf *pf_comp = dynamic_cast<NATCompiler_pf*>(compiler);
    NATRule *rule = getNext(); if (rule==nullptr) return false;

    if (rule->getStr(".iface") == "nil")
    {
        tmp_queue.push_back(rule);
        return true;
    }

    RuleElementItfOutb *itf_re = rule->getItfOutb();
    assert(itf_re!=nullptr);

    if ( ! itf_re->isAny())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    switch ( rule->getRuleType() )
    {
    case NATRule::SNAT:
    {
        RuleElementTSrc *tsrc_re = rule->getTSrc();
        bool have_interface = false;
        for (FWObject::iterator i1=tsrc_re->begin(); i1!=tsrc_re->end(); ++i1)
        {
            Address *addr = Address::cast(FWObjectReference::getObject(*i1));
            have_interface |= pf_comp->assignInterfaceToNATRule(rule, addr);
        }

        if (have_interface)
        {
            tmp_queue.push_back(rule);
            return true;
        }

/* if we appear here, then TSrc is not an interface or address of
 * an interface. Generate NAT rule without "on iface" clause
 */
//        rule->setInterfaceStr("");
        itf_re->clearChildren();
        itf_re->setAnyElement();
    }
    break;

    case NATRule::DNAT:
    {
        RuleElementODst *odst_re = rule->getODst();
        bool have_interface = false;
        for (FWObject::iterator i1=odst_re->begin(); i1!=odst_re->end(); ++i1)
        {
            Address *addr = Address::cast(FWObjectReference::getObject(*i1));
            have_interface |= pf_comp->assignInterfaceToNATRule(rule, addr);
        }

        if (have_interface)
        {
            tmp_queue.push_back(rule);
            return true;
        }

/* if we appear here, then ODst is not an interface or address of an
 * interface. If this is so, just do not specify interface for rdr
 * rule.
 */
        itf_re->clearChildren();
        itf_re->setAnyElement();
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
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    list<FWObject*> cl;
    RuleElementODst *rel;
    Address *obj=nullptr;

    rel = rule->getODst();      assert(rel);
    obj =compiler->getFirstODst(rule);  assert(obj);
 
    if (obj->getId()==compiler->getFwId() )
    {
	list<FWObject*> l2 = compiler->fw->getByTypeDeep(Interface::TYPENAME);
	for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
        {
	    Interface *interface_ = Interface::cast(*i);
/*
 * update 03/20/03:
 *
 * generally we assume that if firewall object is used in the rule,
 * then any or all its interface will be used. This means that if
 * firewall is in ODst we should really use all of its interfaces, not
 * only external ones.
 */
            if (! interface_->isLoopback() ) cl.push_back(interface_);

	}
	if ( ! cl.empty() )
        {
	    rel->clearChildren();
	    for (FWObject::iterator i1=cl.begin(); i1!=cl.end(); ++i1) 
	    {
		rel->addRef( *i1 );
	    }
	}
/*
 * update for ticket 1397 If firewall object is in ODst, do not assign
 * the rule to any interface.  I use attribute ".iface" to signal
 * AssignInterface that it should not do anything.
 */
        rule->setStr(".iface", "nil");
    }

    return true;
}

/*
 *  I assume that there is always only one object in ODst, TSrc and TDst
 *  rule elements. This should have been assured by inspector VerifyRules
 */
bool NATCompiler_pf::ReplaceFirewallObjectsTSrc::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    list<FWObject*> cl;
    RuleElementTSrc *rel;
    Address         *obj=nullptr;

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

            if (!odst->isAny() && !rule->getODst()->getNeg() && iface!=nullptr)
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
                    QString err(
                        "Could not find suitable interface for the NAT rule %1. "
                        "Perhaps all interfaces are unnumbered?");
                    compiler->abort(
                        rule,
                        err.arg(rule->getLabel().c_str()).toStdString());
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
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    NATCompiler_pf *pf_comp=dynamic_cast<NATCompiler_pf*>(compiler);

    tmp_queue.push_back(rule);

    if (rule->getRuleType()==NATRule::Redirect) 
    {
        Service         *tsrv=compiler->getFirstTSrv(rule);
        RuleElementTDst *rel=rule->getTDst();          assert(rel);
        Address         *otdst=compiler->getFirstTDst(rule);
        Interface       *loopback=nullptr;
        FWObject        *loopback_address=nullptr;

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

            if (loopback_address==nullptr)
            {
                compiler->abort(rule, 
                                "Can not configure redirection for the NAT rule "
                                "because loopback interface is missing.");
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


bool NATCompiler_pf::processMultiAddressObjectsInRE::processNext()
{
    NATCompiler_pf *pf_comp=dynamic_cast<NATCompiler_pf*>(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );
    bool neg = re->getNeg();

    list<FWObject*> cl;

    try
    {
        for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
        {
            FWObject *o= *i;
            if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

            MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
            if (atrt!=nullptr && atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            {
                if (re->size()>1 && neg)
                {
                    string err = "AddressTable object can not be used with "
                        "negation in combination with other objects "
                        "in the same rule element.";
                    compiler->abort(rule, err);
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
            rule->getLabel() + ". Error: " + ex.toString();
        compiler->abort(rule,  err);
    }

    if (!cl.empty())
    {
        RuleElement *nre;

        for (FWObject::iterator i=cl.begin(); i!=cl.end(); i++)
        {
            NATRule *r= compiler->dbcopy->createNATRule();
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
        FWObject *o = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs = Interface::cast(obj);

        if (ifs && Cluster::isA(ifs->getParent()))
        {
            FailoverClusterGroup *failover_group =
                FailoverClusterGroup::cast(
                    ifs->getFirstByType(FailoverClusterGroup::TYPENAME));
            if (failover_group)
            {
                for (FWObjectTypedChildIterator it =
                         failover_group->findByType(FWObjectReference::TYPENAME);
                     it != it.end(); ++it)
                {
                    Interface *member_iface = Interface::cast(FWObjectReference::getObject(*it));
                    assert(member_iface);
                    if (member_iface->isChildOf(compiler->fw))
                    {
                        ifs = member_iface;
                        break;
                    }
                }
            }
        }

        if (ifs && ifs->isDyn() && ! ifs->isChildOf(compiler->fw))        
        {
            QString err(
                "Can not build rule using dynamic interface '%1' "
                "of the object '%2' because its address is unknown.");
            compiler->abort(
                rule, err
                .arg(ifs->getName().c_str())
                .arg(ifs->getParent()->getName().c_str()).toStdString());
        }
    }
}


bool NATCompiler_pf::checkForDynamicInterfacesOfOtherObjects::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

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
    NATRule *rule=getNext(); if (rule==nullptr) return false;

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

    string banner = " Compiling NAT rules for " + fw->getName();
    if (!getRuleSetName().empty())  banner += " ruleset " + getRuleSetName();
    if (ipv6) banner += ", IPv6";
    info(banner);

    Compiler::compile();

    add( new Begin());
    add( new printTotalNumberOfRules() );

    add( new singleRuleFilter());
    
    add(new expandGroupsInItfOutb("expand groups in Interface"));
    add(new replaceClusterInterfaceInItfOutb(
            "replace cluster interfaces with member interfaces in "
            "the Interface rule element"));
    add(new singleObjectNegationItfOutb(
            "process single object negation in inbound Itf"));
    add(new ItfOutbNegation("process negation in Itf"));

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

    if (fw->getOptionsObject()->getBool("preserve_group_names"))
    {
        add(new RegisterGroupsAndTablesInOSrc(
                "register object groups and tables in OSrc"));
        add(new RegisterGroupsAndTablesInODst(
                "register object groups and tables in ODst"));
    }

    add( new ExpandGroups( "expand groups" ) );
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));
    add( new eliminateDuplicatesInOSRC(  "eliminate duplicates in OSRC") );
    add( new eliminateDuplicatesInODST(  "eliminate duplicates in ODST") );
    add( new eliminateDuplicatesInOSRV(  "eliminate duplicates in OSRV") );

    add( new swapMultiAddressObjectsInOSrc(
             " swap MultiAddress -> MultiAddressRunTime in OSrc") );
    add( new swapMultiAddressObjectsInODst(
             " swap MultiAddress -> MultiAddressRunTime in ODst") );
    add( new swapMultiAddressObjectsInTSrc(
             " swap MultiAddress -> MultiAddressRunTime in TSrc") );
    add( new swapMultiAddressObjectsInTDst(
             " swap MultiAddress -> MultiAddressRunTime in TDst") );

    add( new swapAddressTableObjectsInOSrc(
             "AddressTable -> MultiAddressRunTime in OSrc") );
    add( new swapAddressTableObjectsInODst(
             "AddressTable -> MultiAddressRunTime in ODst") );
    add( new swapAddressTableObjectsInTSrc(
             "AddressTable -> MultiAddressRunTime in TSrc") );
    add( new swapAddressTableObjectsInTDst(
             "AddressTable -> MultiAddressRunTime in TDst") );

    add( new processMultiAddressObjectsInOSrc(
             "process MultiAddress objects in OSrc") );
    add( new processMultiAddressObjectsInODst(
             "process MultiAddress objects in ODst") );
    add( new processMultiAddressObjectsInTSrc(
             "process MultiAddress objects in TSrc") );
    add( new processMultiAddressObjectsInTDst(
             "process MultiAddress objects in TDst") );

    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    add( new splitOnOSrv( "split rule on original service" ) );
    add( new fillTranslatedSrv( "fill translated service" ) );

    //add( new doOSrcNegation( "process negation in OSrc" ) );
    //add( new doODstNegation( "process negation in ODst" ) );
    //add( new doOSrvNegation( "process negation in OSrv" ) );

    add( new NATRuleType( "determine NAT rule types" ) );
    add( new splitSDNATRule("split SDNAT rules"      ) );
    add( new NATRuleType( "determine NAT rule types" ) );
    add( new VerifyRules( "verify NAT rules" ) );

    add( new ReplaceFirewallObjectsODst(
             "replace references to the firewall in ODst" ) );
    add( new ReplaceFirewallObjectsTSrc(
             "replace references to the firewall in TSrc" ) );

    add( new ReplaceObjectsTDst( "replace objects in TDst" ) );

    add( new ExpandMultipleAddresses( "expand multiple addresses" ) );

    // we might get empty RE after expanding multiple addresses,
    // for example when unnumbered interface is used in TSRC. Note
    // that VerifyRules should not allow this, but we may still
    // get here in the test mode. Calling dropRuleWithEmptyRE works
    // as a fail-safe and prevents crash.
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    if ( manage_virtual_addr )
        add( new addVirtualAddress("add virtual addresses for NAT rules"));

    add( new checkForUnnumbered("check for unnumbered interfaces" ) );
    add( new checkForDynamicInterfacesOfOtherObjects(
             "check for dynamic interfaces of other hosts and firewalls"));
    add( new ExpandAddressRanges( "expand address range objects" ) );

    add( new splitForTSrc(
             "split if addresses in TSrc belong to different networks" ));

    add( new AssignInterface( "assign rules to interfaces" ) );

    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));

    add( new createTables("create tables"));
//        add( new PrintTables(       "print tables"     ) );

    add( new PrintRule("generate pf code") );
    add( new simplePrintProgress() );

    runRuleProcessors();

}


void NATCompiler_pf::epilog()
{
}

NATCompiler_pf::~NATCompiler_pf()
{
    //if (tables) tables->detach();
}

