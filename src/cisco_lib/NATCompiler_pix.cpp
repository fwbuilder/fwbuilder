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


#include "NATCompiler_pix.h"
#include "NamedObjectsAndGroupsSupport.h"
#include "NamedObjectsManager.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <cstring>
#include <assert.h>

#include <QString>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string NATCompiler_pix::myPlatformName() { return "pix"; }

string _print_addr(const InetAddr* addr)
{
    if (addr) return addr->toString();
    return "NULL";
}

NATCompiler_pix::NATCompiler_pix(FWObjectDatabase *_db,
                                 Firewall *fw,
                                 bool ipv6_policy,
                                 OSConfigurator *_oscnf) : 
    NATCompiler(_db, fw, ipv6_policy, _oscnf) , helper(this)
{ 
}

NATCompiler_pix::~NATCompiler_pix()
{
    std::map<int,NATCmd*>::iterator it1;
    for (it1=nat_commands.begin(); it1!=nat_commands.end(); ++it1)
    {
        delete it1->second;
    }
    nat_commands.clear();

    std::map<int,StaticCmd*>::iterator it2;
    for (it2=static_commands.begin(); it2!=static_commands.end(); ++it2)
    {
        delete it2->second;
    }
    static_commands.clear();
    nonat_rules.clear();
    first_nonat_rule_id.clear();
}

bool StaticCmd::operator==(const StaticCmd &other)
{
    return (*oaddr == *(other.oaddr) &&
            *iaddr == *(other.iaddr) &&
            *osrv  == *(other.osrv)  &&
            *tsrv  == *(other.tsrv)  &&
            i_iface->getId() == other.i_iface->getId() &&
            o_iface->getId() == other.o_iface->getId());
}

/*
 * Do not expand interfaces in ODst and TSrc
 *
 */
void NATCompiler_pix::_expand_addr_recursive_pix(Rule *rule,
                                                 FWObject *re,
                                                 FWObject *s,
                                                 list<FWObject*> &ol,
                                                 bool expand_cluster_interfaces_fully)
{
    bool odst_or_tsrc = (re->getTypeName() == RuleElementODst::TYPENAME ||
                         re->getTypeName() == RuleElementTSrc::TYPENAME);

    list<FWObject*> addrlist;

    for (FWObject::iterator i1=s->begin(); i1!=s->end(); ++i1) 
    {
        FWObject *o = FWReference::getObject(*i1);
	assert(o);

        Address *addr = Address::cast(o);

        // this condition includes Host, Firewall and Interface
        if (addr && !addr->hasInetAddress())
        {
            addrlist.push_back(o);
            continue;
        }

        // IPv4, IPv6, Network, NetworkIPv6
        if (addr && addr->hasInetAddress() && MatchesAddressFamily(o))
        {            
            addrlist.push_back(o);
            continue;
        }

        if (o->getId() == FWObjectDatabase::ANY_ADDRESS_ID ||
            MultiAddress::cast(o)!=nullptr ||
            Interface::cast(o) ||
            physAddress::cast(o))
        {
            addrlist.push_back(o);
            continue;
        }
    }
 
    if (addrlist.empty())
    {
        if (RuleElement::cast(s)==nullptr) ol.push_back(s);
    }
    else
    {
        for (list<FWObject*>::iterator i2=addrlist.begin();
             i2!=addrlist.end(); ++i2)
        {
            Interface *i2itf = Interface::cast(*i2);
            if (i2itf)
            {
                // if this is ODst or TSrc, just use interface 
                if (odst_or_tsrc)
                {
                    ol.push_back(i2itf);
                    continue;
                }

                _expand_interface(rule, i2itf, ol, expand_cluster_interfaces_fully);
                continue;
            }
            _expand_addr_recursive_pix(rule, re, *i2, ol, expand_cluster_interfaces_fully);
        }
    }
}


void NATCompiler_pix::_expand_addr_recursive(Rule *rule, FWObject *re,
                                             list<FWObject*> &ol,
                                             bool expand_cluster_interfaces_fully)
{
    _expand_addr_recursive_pix(rule, re, re, ol, expand_cluster_interfaces_fully);
}


void NATCompiler_pix::_expand_interface(Rule *rule,
                                        Interface *iface,
                                        std::list<FWObject*> &ol,
                                        bool expand_cluster_interfaces_fully)
{
    Compiler::_expand_interface(rule, iface, ol, expand_cluster_interfaces_fully);
}

string NATCompiler_pix::getNATACLname(Rule *rule,int nat_id)
{
    int    n=-1;
    string res;
    do 
    {
        n++;
        ostringstream os;
        os << rule->getUniqueId() << "." << nat_id << "." << n;
        res=os.str();
    } while (nat_acl_names.count(res)!=0);
    return res;
}

string NATCompiler_pix::getNATACLname(Rule *rule,string suffix)
{
    int    n=-1;
    string res;
    do 
    {
        n++;
        ostringstream os;
        os << rule->getUniqueId();
        if (!suffix.empty()) os << "." << suffix;
        os << "." << n;
        res=os.str();
    } while (nat_acl_names.count(res)!=0);
    return res;
}

int NATCompiler_pix::prolog()
{
    global_pool_no = 1;

    NAT *final_ruleset = new NAT();
    final_ruleset->setName("Final NAT Rule Set");
    persistent_objects->add( final_ruleset );
    final_ruleset_id = final_ruleset->getId();

    return NATCompiler::prolog();
}

string NATCompiler_pix::debugPrintRule(Rule *r)
{
    NATRule *rule=NATRule::cast(r);

#ifndef NDEBUG
    RuleElementItfInb *itf_in_re = rule->getItfInb(); assert(itf_in_re!=nullptr);
    RuleElementItfOutb *itf_out_re = rule->getItfOutb(); assert(itf_out_re!=nullptr);
#endif

    ostringstream os;

    switch (rule->getRuleType()) 
    {
    case NATRule::NONAT:
        os << "NONAT Type: " << rule->getInt("nonat_type");
        break;

    case NATRule::SNAT:
    {
        if ( ! rule->exists("nat_cmd") ) break;
        NATCmd *natcmd = nat_commands[ rule->getInt("nat_cmd") ];
        if (natcmd != nullptr)
        {
            os <<" NATCmd: ";
            os << " rule=[" << natcmd->rule_label << "]";
            os << " id=" << natcmd->nat_id;
            os << " rule=" << natcmd->rule_label;
            os << " nat_acl_name=" << natcmd->nat_acl_name;
            os << " (" << nat_acl_names[natcmd->nat_acl_name] << ")";
            os << " o_src=" <<  _print_addr(natcmd->o_src->getAddressPtr());
            os << " o_dst=" <<  _print_addr(natcmd->o_dst->getAddressPtr());
            os << " o_srv=" <<  natcmd->o_srv->getName();
            os << " t_addr=" <<  _print_addr(natcmd->t_addr->getAddressPtr());
            os << " ignore_global=" << string((natcmd->ignore_global)?"1":"0");
            os << " ignore_nat=" << string((natcmd->ignore_nat)?"1":"0");
            os << " ignore_nat_and_print_acl="
               << string((natcmd->ignore_nat_and_print_acl)?"1":"0");
            os << " use_nat_0_0="
               << string((rule->getBool("use_nat_0_0"))?"1":"0");
        }
    }
    break;

    case NATRule::DNAT:
    {
        if ( ! rule->exists("sc_cmd") ) break;
        StaticCmd *scmd=static_commands[ rule->getInt("sc_cmd") ];
        if (scmd!=nullptr)
        {
            string iaddr_str = _print_addr(scmd->iaddr->getAddressPtr());
            string oaddr_str = _print_addr(scmd->oaddr->getAddressPtr());

            os << " StaticCmd:";
            os << " ignore=" << scmd->ignore_scmd_and_print_acl;
            os << " acl=" << scmd->acl_name;
            os << " (" << nat_acl_names[scmd->acl_name] << ")";
            os << " iaddr=" << iaddr_str;
            os << " oaddr=" << oaddr_str;
            os << " osrc=" << _print_addr(scmd->osrc->getAddressPtr());
            os << " osrv=" << scmd->osrv->getName();
            os << " tsrv=" << scmd->tsrv->getName();
        }
    }
    break;
    default: ;    // TODO: should actually be always_assert
    }

    return NATCompiler::debugPrintRule(rule) +
        " " +
        " (type=" + rule->getRuleTypeAsString() + ") " +
        "use_nat_0_0=" + string((rule->getBool("use_nat_0_0"))?"1":"0") + " " +
        os.str();
}

/*
 * store final nat rules in final rule set object in
 * persistent_obejcts. Note that we can't add the same rules since an
 * object can not be placed in two different places in the tree, so we
 * have to add copies.
 */
bool NATCompiler_pix::storeProcessedRules::processNext()
{
    NATCompiler_pix *pix_comp = dynamic_cast<NATCompiler_pix*>(compiler);

    FWObject *final_ruleset = compiler->persistent_objects->getRoot()->findInIndex(
        pix_comp->final_ruleset_id);

    slurp();
    if (tmp_queue.size()==0) return false;

    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        NATRule *rule = NATRule::cast( *k );

        NATRule *r = compiler->dbcopy->createNATRule();
        final_ruleset->add(r);
        r->duplicate(rule);
    }

    return true;
}

bool NATCompiler_pix::VerifyRules::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    string version = compiler->fw->getStr("version");

    if (rule->getRuleType()==NATRule::SDNAT) 
    {
        compiler->abort(
            rule, 
            "Rules that translate both source and destination are not supported.");
        return true;
    }

    bool  version_lt_63= (
        compiler->fw->getStr("platform")=="pix" &&
        libfwbuilder::XMLTools::version_compare(version, "6.3")<0);  // fwsm is always above 6.3 - its OS is based on 6.3

    RuleElementOSrc  *osrc=rule->getOSrc();  assert(osrc);
    RuleElementODst  *odst=rule->getODst();  assert(odst);
    RuleElementOSrv  *osrv=rule->getOSrv();  assert(osrv);

    RuleElementTSrc  *tsrc=rule->getTSrc();  assert(tsrc);
    RuleElementTDst  *tdst=rule->getTDst();  assert(tdst);
    RuleElementTSrv  *tsrv=rule->getTSrv();  assert(tsrv);

    if (rule->getRuleType()==NATRule::LB)
    {
        compiler->abort(
            rule, 
            "Load balancing rules are not supported.");
        return true;
    }

    if (rule->getRuleType()==NATRule::NONAT && (!osrv->isAny() || !tsrv->isAny()))
    {
        compiler->abort(
                rule, 
                "'no nat' rules should have no services");
        return true;
    }

    if (osrc->getNeg() ||
        odst->getNeg() ||
        osrv->getNeg() ||
        tsrc->getNeg() ||
        tdst->getNeg() ||
        tsrv->getNeg())
    {
        compiler->abort(
                rule, 
                "Negation is not supported in NAT rules.");
        return true;
    }

    if (rule->getRuleType()==NATRule::SNAT) 
    {
//        if ( tsrc->size()!=1)
//            compiler->abort("There should be no more than one object in translated source in the rule "+rule->getLabel());

        if ( ! odst->isAny() && version_lt_63)  // can do on fwsm
        {
            compiler->warning(
                    rule, 
                    "Original destination is ignored in 'nat' NAT rules "
                    "when compiling for PIX v6.2 and earlier.");
            odst->clearChildren();
            odst->setAnyElement();
        }
    }

    if (rule->getRuleType()==NATRule::DNAT) 
    {
        if ( odst->size()!=1 && version_lt_63)
        {
            compiler->abort(
                    rule, 
                    "There should be no more than one object in original destination");
            return true;
        }

        if ( ! osrc->isAny() && version_lt_63)
            compiler->warning(
                    rule, 
                    "Original source is ignored in 'static' NAT rules "
                    "when compiling for PIX v6.2 and earlier.");
    }

    if (osrv->size()!=1 && !tsrv->isAny())
    {
        compiler->abort(
                rule, 
                "Can not translate multiple services into one service in one rule. ");
        return true;
    }

    if (tsrv->size()!=1)
    {
        compiler->abort(
            rule, 
                "Translated service should be 'Original' or should contain single object.");
        return true;
    }

    if ( Group::cast( compiler->getFirstTSrv(rule) )!=nullptr)
    {
        compiler->abort(
                rule, 
                "Can not use group in translated service.");
        return true;
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
                    "Original and translated source should both be networks of the same size");
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

    if (rule->getRuleType()==NATRule::SNetnat) rule->setRuleType(NATRule::SNAT);
    if (rule->getRuleType()==NATRule::DNetnat) rule->setRuleType(NATRule::DNAT);

    tmp_queue.push_back(rule);

    return true;
}


bool NATCompiler_pix::AssignInterface::processNext()
{
    Helper helper(compiler);
    NATRule *rule = getNext(); if (rule==nullptr) return false;

    RuleElement *itf_re;

    Address  *a1 = nullptr;
    Address  *a2 = nullptr;

    if (rule->getRuleType()==NATRule::SNAT || rule->getRuleType()==NATRule::SDNAT)
    {
	a1 = compiler->getFirstOSrc(rule);
	a2 = compiler->getFirstTSrc(rule);
    }

    if (rule->getRuleType()==NATRule::DNAT)
    {
	a1 = compiler->getFirstODst(rule);
	a2 = compiler->getFirstTDst(rule);
    }

    if (rule->getRuleType()==NATRule::NONAT)
    {
	a1 = compiler->getFirstOSrc(rule);
	a2 = compiler->getFirstODst(rule);
    }

    assert(a1!=nullptr && a2!=nullptr);

    int org_intf_id = helper.findInterfaceByNetzone(a1);
    int trn_intf_id = helper.findInterfaceByNetzone(a2);

    FWObject *iface_org = compiler->dbcopy->findInIndex(org_intf_id);
    FWObject *iface_trn = compiler->dbcopy->findInIndex(trn_intf_id);

    if ( org_intf_id==-1 )
    {
        QString err("Object '%1' does not belong to any known network zone.");
	compiler->abort(rule, err.arg(a1->getName().c_str()).toStdString());
        return true;
    }

    if ( trn_intf_id==-1 )
    {
        QString err("Object '%1' does not belong to any known network zone.");
	compiler->abort(rule, err.arg(a2->getName().c_str()).toStdString());
        return true;
    }

    itf_re = rule->getItfInb(); assert(itf_re!=nullptr);
    if (itf_re->isAny() && ! itf_re->hasRef(iface_org)) itf_re->addRef(iface_org);

    itf_re = rule->getItfOutb(); assert(itf_re!=nullptr);
    if (itf_re->isAny() && ! itf_re->hasRef(iface_trn)) itf_re->addRef(iface_trn);

    if (org_intf_id == trn_intf_id)
    {
        QString err("Objects used in Original Source and Translated Source "
                    "of the rule dictate that the same interface '%1' is going "
                    "to be used as real and mapped interface in the generated "
                    "nat command.");
        compiler->warning(
            rule,
            err.arg(
                Interface::cast(iface_org)->getLabel().c_str()).toStdString());
    }

    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_pix::verifyInterfaces::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_pix::verifyRuleElements::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

#ifndef NDEBUG
    Address  *osrc=compiler->getFirstOSrc(rule);  assert(osrc);
#endif
    Address  *odst=compiler->getFirstODst(rule);  assert(odst);
    Service  *osrv=compiler->getFirstOSrv(rule);  assert(osrv);

#ifndef NDEBUG
    Address  *tsrc=compiler->getFirstTSrc(rule);  assert(tsrc);
#endif
    Address  *tdst=compiler->getFirstTDst(rule);  assert(tdst);
    Service  *tsrv=compiler->getFirstTSrv(rule);  assert(tsrv);

    string version = compiler->fw->getStr("version");

    if (rule->getRuleType()==NATRule::SNAT)
    {
	if ((! osrv->isAny() || ! tsrv->isAny()) &&
            libfwbuilder::XMLTools::version_compare(version, "6.3")<0)
        {
	    compiler->abort(
                    rule, 
                    "only PIX v6.3 and later recognizes services in global NAT.");
            return true;
        }
    }

    if (rule->getRuleType()==NATRule::DNAT)
    {
	if ((AddressRange::cast(odst) || AddressRange::cast(tdst)) &&
            libfwbuilder::XMLTools::version_compare(version, "8.3")<0)
        {
	    compiler->abort(
                    rule, 
                    "Address ranges are not supported in original destination or "
                    "translated destination ");
            return true;
        }

	if (Network::isA(odst) && Network::isA(tdst))
        {
            InetAddr n1 = (Interface::cast(odst)) ? 
                InetAddr(InetAddr::getAllOnes()) : (*(odst->getNetmaskPtr()));
            InetAddr n2 = (Interface::cast(tdst)) ? 
                InetAddr(InetAddr::getAllOnes()) : (*(tdst->getNetmaskPtr()));

            if ( !(n1==n2) )
            {
                compiler->abort(
                        rule, 
                        "Original and translated destination must be of the same "
                        "size");
                return true;
            }
        }


	if (osrv->getTypeName()!=tsrv->getTypeName()) 
        {
	    compiler->abort(
                    rule, 
                    "Original and translated services must be of "
                    "the same type.");
            return true;
        }

	if (ICMPService::isA(osrv))
        {
	    compiler->abort(
                    rule, 
                    "ICMP services are not supported in static NAT. ");
            return true;
        }

	if (TCPService::isA(osrv) || UDPService::isA(osrv))
        {
	    int drs=TCPUDPService::cast(osrv)->getDstRangeStart();
	    int dre=TCPUDPService::cast(osrv)->getDstRangeEnd();

	    if (drs!=dre)
            {
		compiler->abort(
                        rule, 
                        "TCP or UDP service with a port range is not "
                        "supported in NAT.");
                return true;
            }
	}
	if (TCPService::isA(tsrv) || UDPService::isA(tsrv))
        {
	    int drs=TCPUDPService::cast(tsrv)->getDstRangeStart();
	    int dre=TCPUDPService::cast(tsrv)->getDstRangeEnd();

	    if (drs!=dre)
            {
		compiler->abort(
                        rule, 
                        "TCP or UDP service with a port range is not "
                        "supported in NAT.");
                return true;
            }
	}
    }

    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_pix::fillTranslatedSrv::processNext()
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


/**
 * unlike standard inspector addressRanges in the base class NATCompiler,
 * this one does not expand address ranges in TSrc and TDst 
 */
bool NATCompiler_pix::ExpandAddressRanges::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    RuleElement *rel;

    rel=rule->getOSrc();    assert(rel);
    compiler->_expandAddressRanges(rule,rel);
    rel=rule->getODst();    assert(rel);
    compiler->_expandAddressRanges(rule,rel);

#if 0
// if we want to support NAT rules with address ranges. For example,
// could  compile these as a bunch of individual host translations

    switch (rule->getRuleType())
    {
    case NATRule::SNAT:
        rel=rule->getTSrc();    assert(rel);
        compiler->_expandAddressRanges(rule,rel);
        break;

    case NATRule::DNAT:
        rel=rule->getTDst();    assert(rel);
        compiler->_expandAddressRanges(rule,rel);
        break;
    }
#endif
    return true;
}

/*
 *  I assume that there is always only one object in ODst, TSrc and TDst
 *  rule elements. This should have been assured by inspector VerifyRules
 */
bool NATCompiler_pix::ReplaceFirewallObjectsODst::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    list<FWObject*> cl;
    RuleElementODst *rel;
    Address     *obj=nullptr;

    switch (rule->getRuleType()) {

    case NATRule::Masq:
//    case NATRule::Redirect: 
	return true;
    case NATRule::DNAT:
	rel=rule->getODst();      assert(rel);
	obj=compiler->getFirstODst(rule);  assert(obj!=nullptr);

	if (obj->getId()==compiler->getFwId() ) 
	{

	    list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
	    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
            {
		Interface *iface = Interface::cast(*i);

		if (! iface->isLoopback() && iface->getSecurityLevel()==0 )
                    cl.push_back(iface);
	    }
	    if ( ! cl.empty() )
            {
		// while (rel->size()) 
		//     rel->remove( rel->front()  );
                rel->clearChildren();

		for (FWObject::iterator i1=cl.begin(); i1!=cl.end(); ++i1) 
		{
		    rel->addRef( *i1 );
		}
	    }
	}
        default: ;    // TODO: should actually be always_assert
    }
    return true;
}

bool NATCompiler_pix::ReplaceFirewallObjectsTSrc::processNext()
{
    Helper helper(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    list<FWObject*> cl;
    RuleElementTSrc *rel;
    Address     *obj=nullptr;

    switch (rule->getRuleType()) {

    case NATRule::Masq:
    case NATRule::Redirect:
    {
        tmp_queue.push_back(rule);
        return true;
    }

    case NATRule::SNAT:
    {
        int        osrc_level=100;
        Address   *osrc=nullptr;
        Interface *osrc_iface=nullptr;

        if ( ! rule->getOSrc()->isAny())
        {
            osrc=compiler->getFirstOSrc(rule); assert(osrc!=nullptr);
            osrc_iface = Interface::cast(
                compiler->dbcopy->findInIndex( helper.findInterfaceByNetzone(osrc)));
            osrc_level = osrc_iface->getSecurityLevel();
        }

	rel = rule->getTSrc();
        assert(rel);

        if (rel->size() == 0)
        {
            compiler->abort(rule, "Empty TSrc");
            return true;
        }

	obj = compiler->getFirstTSrc(rule);
        assert(obj!=nullptr);

	if (obj->getId()==compiler->getFwId() ) 
	{

/* if ODst is 'any', pick all interfaces with security level _less_ than
 * level of the interface OSrc is associated with. If ODst is not 'any', 
 * find interface it is associated with and use only it.
 */
            if (rule->getODst()->isAny()) 
            {
                list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
                for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
                {
                    Interface *iface = Interface::cast(*i);
                    if (iface->getSecurityLevel()<osrc_level )  
                        cl.push_back(iface);
                }
            } else
            {
                Address *odst = compiler->getFirstODst(rule); assert(odst!=nullptr);
                FWObject *odst_iface =
                    compiler->dbcopy->findInIndex(
                        helper.findInterfaceByNetzone(odst ) );
                if (odst_iface!=nullptr) cl.push_back(odst_iface);
            }

	    if ( ! cl.empty() )
            {
		// while (rel->size()) 
		//     rel->remove( rel->front() );
                rel->clearChildren();

		for (FWObject::iterator i1=cl.begin(); i1!=cl.end(); ++i1) 
		{
		    rel->addRef( *i1 );
		}
	    }
	}
    }
    break;
    default: ;    // TODO: should actually be always_assert
    }

    tmp_queue.push_back(rule);
    return true;
}

void NATCompiler_pix::UseFirewallInterfaces::scanInterfaces(RuleElement *rel)
{
    FWObject *o= rel->front();
    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
    Address *obj=Address::cast(o);
    if(obj==nullptr)
    {
        compiler->abort(rel->getParent(), 
                        "Broken rule element "+
                        rel->getTypeName()+
                        " in rule "+
                        NATRule::cast(rel->getParent())->getLabel()+
                        " ( found object with type "+
                        string((o!=nullptr)?o->getTypeName():"<nullptr>") +
                        ")");
        return;
    }

    const InetAddr *obj_addr = obj->getAddressPtr();
    if (obj_addr==nullptr) return;

    list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
    {
        Interface *iface=Interface::cast(*i);
        const InetAddr *iface_addr = iface->getAddressPtr();
        if (iface_addr == nullptr) continue;
        if (*iface_addr == *obj_addr)
        {
            rel->removeRef(obj);
            rel->addRef(iface);
            return;
        }        
    }
}

bool NATCompiler_pix::UseFirewallInterfaces::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    RuleElement *rel;

    rel=rule->getODst();      assert(rel);
    if (!rel->isAny()) scanInterfaces(rel);

    rel=rule->getTSrc();      assert(rel);
    if (!rel->isAny()) scanInterfaces(rel);

    return true;
}

bool NATCompiler_pix::processNONATRules::processNext()
{
    Helper helper(compiler);
    NATCompiler_pix *pix_comp = dynamic_cast<NATCompiler_pix*>(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    if (rule->getRuleType()==NATRule::NONAT)
    {
	Address  *osrc=compiler->getFirstOSrc(rule);  assert(osrc);
	Address  *odst=compiler->getFirstODst(rule);  assert(odst);

        Interface *osrc_iface = Interface::cast(
            compiler->dbcopy->findInIndex(helper.findInterfaceByNetzone(osrc)));
        Interface *odst_iface = Interface::cast(
            compiler->dbcopy->findInIndex(helper.findInterfaceByNetzone(odst)));

        int osrc_level = osrc_iface->getSecurityLevel();
        int odst_level = odst_iface->getSecurityLevel();

/*
 * PIX has two types of NONAT rules, one is when connection goes from
 * low security interface to the high security interface and another
 * for the opposite
 */
        if (osrc_level>odst_level)
        {
            rule->setInt("nonat_type", NONAT_NAT0);
            nonat n0;
            // n0.i_iface = osrc_iface;
            // n0.o_iface = odst_iface;

            RuleElement *itf_re = rule->getItfInb();
            assert(itf_re!=nullptr);
            if ( ! itf_re->hasRef(osrc_iface)) itf_re->addRef(osrc_iface);

            itf_re = rule->getItfOutb();
            assert(itf_re!=nullptr);
            if ( ! itf_re->hasRef(odst_iface)) itf_re->addRef(odst_iface);

            n0.src = osrc;
            n0.dst = odst;
            n0.acl_name = "nat0."+osrc_iface->getLabel();
            n0.last = true;

            pix_comp->nonat_rules[rule->getId()] = n0;
            pix_comp->registerACL(n0.acl_name);

            if (pix_comp->first_nonat_rule_id.count(osrc_iface->getId()) == 0)
                pix_comp->first_nonat_rule_id[osrc_iface->getId()] =
                    rule->getId();
            
        } else
        {
            rule->setInt("nonat_type", NONAT_STATIC);
            Interface *osrc_iface = Interface::cast(
                compiler->dbcopy->findInIndex(helper.findInterfaceByNetzone(osrc)));
            Interface *odst_iface = Interface::cast(
                compiler->dbcopy->findInIndex(helper.findInterfaceByNetzone(odst)));

            RuleElement *itf_re = rule->getItfInb();
            assert(itf_re!=nullptr);
            if ( ! itf_re->hasRef(osrc_iface)) itf_re->addRef(osrc_iface);

            itf_re = rule->getItfOutb();
            assert(itf_re!=nullptr);
            if ( ! itf_re->hasRef(odst_iface)) itf_re->addRef(odst_iface);

        }
    }

    return true;
}

bool NATCompiler_pix::createNATCmd::processNext()
{
//    Helper helper(compiler);
    NATCompiler_pix *pix_comp = dynamic_cast<NATCompiler_pix*>(compiler);
    NATRule *rule = getNext(); if (rule==nullptr) return false;
    string version = compiler->fw->getStr("version");

    if (rule->getRuleType()==NATRule::SNAT) 
    {
	Address  *osrc = compiler->getFirstOSrc(rule);  assert(osrc);
	Address  *odst = compiler->getFirstODst(rule);  assert(osrc);
	Service  *osrv = compiler->getFirstOSrv(rule);  assert(osrv);
	Address  *tsrc = compiler->getFirstTSrc(rule);  assert(tsrc);

        RuleElementItfInb *itf_in_re = rule->getItfInb();
        RuleElementItfOutb *itf_out_re = rule->getItfOutb();

        Interface *i_iface = Interface::cast(
            FWObjectReference::getObject(itf_in_re->front()));
        Interface *o_iface = Interface::cast(
            FWObjectReference::getObject(itf_out_re->front()));

	NATCmd *natcmd = new NATCmd();

        natcmd->nat_id = nat_id_counter;
        natcmd->rule_label = rule->getLabel();

        natcmd->o_src   = osrc;
        natcmd->o_dst   = odst;
        natcmd->o_srv   = osrv;
        natcmd->t_addr  = tsrc;
        natcmd->i_iface = i_iface; // inbound interface
        natcmd->o_iface = o_iface; // outbound interface

        natcmd->nat_acl_name = pix_comp->getNATACLname(rule,"");
        pix_comp->registerACL(natcmd->nat_acl_name);

        if (Interface::cast(tsrc)!=nullptr || o_iface->isDyn())
        {
            natcmd->type = INTERFACE;
        } else
        {
            if (Network::cast(tsrc)) 
            {
                natcmd->type = NETWORK_ADDRESS;
            } else {
                if (AddressRange::cast(tsrc)) natcmd->type = ADDRESS_RANGE;
                else                          natcmd->type = SINGLE_ADDRESS;
            }
        }

        natcmd->ignore_nat = natcmd->ignore_nat_and_print_acl =
            natcmd->ignore_global = false;
        natcmd->use_nat_0_0 = rule->getBool("use_nat_0_0");
        
/*
 * "nat ... outside" is only supported in PIX 6.2
 */

        natcmd->outside =
            ( i_iface->getSecurityLevel() < o_iface->getSecurityLevel());

        if (natcmd->outside && compiler->fw->getStr("platform")=="pix" &&
            libfwbuilder::XMLTools::version_compare(version, "6.2")<0 )
        {
            compiler->abort(
                    rule, 
                    "Bi-Directional NAT of source addresses is only "
                    "supported in  PIX 6.2 and newer.");
            return true;
        }
/* 
 * map is sorted container, this means that objects are going to be arranged
 * in nat_commands in the order of the key.
 */
        pix_comp->nat_commands[nat_id_counter]= natcmd;
        rule->setInt("nat_cmd",nat_id_counter);
        nat_id_counter++;
    }

    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_pix::createStaticCmd::processNext()
{
    NATCompiler_pix *pix_comp=dynamic_cast<NATCompiler_pix*>(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    if (rule->getRuleType()==NATRule::DNAT)
    {
	Address  *osrc = compiler->getFirstOSrc(rule);  assert(osrc);
	Address  *odst = compiler->getFirstODst(rule);  assert(odst);
	Service  *osrv = compiler->getFirstOSrv(rule);  assert(osrv);
	Address  *tdst = compiler->getFirstTDst(rule);  assert(tdst);
	Service  *tsrv = compiler->getFirstTSrv(rule);  assert(tsrv);

        RuleElementItfInb *itf_in_re = rule->getItfInb();
        RuleElementItfOutb *itf_out_re = rule->getItfOutb();

        Interface *i_iface = Interface::cast(
            FWObjectReference::getObject(itf_in_re->front()));
        Interface *o_iface = Interface::cast(
            FWObjectReference::getObject(itf_out_re->front()));

	StaticCmd *scmd = new StaticCmd();

        scmd->acl_name = pix_comp->getNATACLname(rule,"");
        pix_comp->registerACL(scmd->acl_name);
        scmd->rule=rule->getLabel();
        // source and destination addresses are swapped here because
        // access lists used for NAT should have 'real' addresses in source
        scmd->iaddr=tdst;
        scmd->oaddr=odst;
        scmd->osrc= osrc;
        scmd->osrv= osrv;
        scmd->tsrv= tsrv;
        scmd->ignore_scmd_and_print_acl=false;
        scmd->i_iface = i_iface;
        scmd->o_iface = o_iface;

        pix_comp->static_commands[sc_id_counter]=scmd;
        rule->setInt("sc_cmd",sc_id_counter);

        sc_id_counter++;
    }

    return true;
}


bool  NATCompiler_pix::clearOSrc::processNext()
{
//    NATCompiler_pix *pix_comp=dynamic_cast<NATCompiler_pix*>(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getBool("clear_osrc"))
    {
        RuleElementOSrc  *osrc=rule->getOSrc();
        osrc->clearChildren();
        osrc->setAnyElement();
    }

    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_pix::processMultiAddressObjectsInRE::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );

    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
        MultiAddress *atrt = MultiAddress::cast(o);
        if (atrt!=nullptr && atrt->isRunTime())
        {
            compiler->abort(
                    rule, 
                    "Run-time AddressTable and DNSName objects are not supported.");
            return true;
        }
    }

    tmp_queue.push_back(rule);
    return true;
}



void NATCompiler_pix::compile()
{
    info(" Compiling NAT rules for " + fw->getName());

    Compiler::compile();

    add( new Begin( "Begin processing"));
    add( new printTotalNumberOfRules());

    add( new singleRuleFilter());

    add(new expandGroupsInItfInb("expand groups in inbound Interface"));
    add(new replaceClusterInterfaceInItfInb(
            "replace cluster interfaces with member interfaces in "
            "the inbound Interface rule element"));
    add(new ItfInbNegation("process negation in inbound Itf"));

    add(new expandGroupsInItfOutb("expand groups in outbound Interface"));
    add(new replaceClusterInterfaceInItfOutb(
            "replace cluster interfaces with member interfaces in "
            "the outbound Interface rule element"));
    add(new ItfOutbNegation("process negation in outbound Itf"));

    add( new ConvertToAtomicForItfInb("convert to atomic for inbound interface") );
    add( new ConvertToAtomicForItfOutb("convert to atomic for outbound interface"));

    if (fw->getOptionsObject()->getBool( "pix_optimize_default_nat"))
        add (new optimizeDefaultNAT(
                 "optimize commands 'nat (interface) 0.0.0.0 0.0.0.0'"));

    add( new recursiveGroupsInOSrc("check for recursive groups in OSRC"));
    add( new recursiveGroupsInODst("check for recursive groups in ODST"));
    add( new recursiveGroupsInOSrv("check for recursive groups in OSRV"));

    add( new recursiveGroupsInTSrc("check for recursive groups in TSRC"));
    add( new recursiveGroupsInTDst("check for recursive groups in TDST"));
    add( new recursiveGroupsInTSrv("check for recursive groups in TSRV"));


    add( new emptyGroupsInOSrc("check for empty groups in OSRC"));
    add( new emptyGroupsInODst("check for empty groups in ODST"));
    add( new emptyGroupsInOSrv("check for empty groups in OSRV"));

    add( new emptyGroupsInTSrc("check for empty groups in TSRC"));
    add( new emptyGroupsInTDst("check for empty groups in TDST"));
    add( new emptyGroupsInTSrv("check for empty groups in TSRV"));

    add( new ExpandGroups("expand groups"));

    /*
     * We do not support ipv6 yet
     */
    add( new DropIPv6RulesWithWarning(
             "drop ipv6 rules",
             "Rule has been suppressed because it contains IPv6 objects and "
             "Firewall Builder does not support IPv6 for this platform"));

    add( new eliminateDuplicatesInOSRC("eliminate duplicates in OSRC"));
    add( new eliminateDuplicatesInODST("eliminate duplicates in ODST"));
    add( new eliminateDuplicatesInOSRV("eliminate duplicates in OSRV"));

    add( new processMultiAddressObjectsInOSrc(
             "process MultiAddress objects in OSrc"));
    add( new processMultiAddressObjectsInODst(
             "process MultiAddress objects in ODst"));

    add( new classifyNATRule("determine NAT rule types"));
    add( new VerifyRules("verify rules" ));

    // ReplaceFirewallObjectsODst, ReplaceFirewallObjectsODst and
    // UseFirewallInterfaces assume there is one object in ODst,
    // TSrc and TDst rule elements. This should have been assured
    // by inspector VerifyRules
    add( new ReplaceFirewallObjectsODst("replace fw object in ODst" ));
    add( new ReplaceFirewallObjectsTSrc("replace fw object in TSrc" ));
    add( new UseFirewallInterfaces(
             "replace host objects with firewall's interfaces if the have the same address"));

    // ExpandMultipleAddresses acts on different rule elements
    // depending on the rule type.
    // Also using overloaded virtual function  _expand_interface
    add( new ExpandMultipleAddresses("expand multiple addresses"));
    add( new MACFiltering( "check for MAC address filtering"));
    add( new ExpandAddressRanges("expand address range objects"));
    add( new checkForUnnumbered("check for unnumbered interfaces"));

    add( new ConvertToAtomic("convert to atomic rules" ));
    add( new AssignInterface("assign rules to interfaces" ));
    add( new verifyInterfaces("verify interfaces assignment" ));
    add( new fillTranslatedSrv("fill translated service element" ));
    add( new verifyRuleElements(
             "verify rule elements for static NAT rules"));
    add( new processNONATRules("process NONAT" ));

    if (fw->getOptionsObject()->getBool("pix_optimize_default_nat"))
        add (new clearOSrc ("clear OSrc" ));

    add( new SpecialServicesOSrv( "check for special services" ));

    add( new createNATCmd ("create NAT commands" ));
    add( new createStaticCmd ("create static commands" ));
    add( new mergeNATCmd ("merge NAT commands" ));
    add( new SuppressDuplicateNONATStatics(
             "suppress duplicate NONAT statics" ));

    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));

    //add( new PrintClearCommands( "Clear ACLs" ));

    add( new PrintRule ("generate PIX code" ));
    add( new storeProcessedRules ("store processed rules" ));
    add( new simplePrintProgress ());

    bool pix_check_duplicate_nat = 
        fw->getOptionsObject()->getBool("pix_check_duplicate_nat");
    bool pix_check_overlapping_global_pools =
        fw->getOptionsObject()->getBool("pix_check_overlapping_global_pools");
    bool pix_check_overlapping_statics =
        fw->getOptionsObject()->getBool("pix_check_overlapping_statics");
    bool pix_check_overlapping_global_statics =
        fw->getOptionsObject()->getBool("pix_check_overlapping_global_statics");

    if ( pix_check_duplicate_nat || 
         pix_check_overlapping_global_pools ||
         pix_check_overlapping_statics ||
         pix_check_overlapping_global_statics )
    {
        add( new createNewCompilerPass(" Detecting nat problems  ..."));

        if ( pix_check_duplicate_nat )
            add( new DetectDuplicateNAT(" Detect duplicate nat entries"));

        if ( pix_check_overlapping_global_pools )
            add( new DetectGlobalPoolProblems(
                     " Detect global pool overlapping" ));

        if ( pix_check_overlapping_statics )
            add( new DetectOverlappingStatics(
                     " Detect overlapping statics" ));

        if ( pix_check_overlapping_global_statics )
            add( new DetectOverlappingGlobalPoolsAndStaticRules(
                     " Detect overlapping global pools and statics" ));

        add( new simplePrintProgress ( ));
    }

    runRuleProcessors();

}


void NATCompiler_pix::regroup()
{
    list<string>              commands;
    map<string,list<string> > script;

    commands.push_back("THE_REST");
    commands.push_back("access-list ");
    commands.push_back("global ");
    commands.push_back("nat ");
    commands.push_back("static ");

    string acl, agrp, icmp, telnet, ssh;
    string new_output;
    char   buf[1024];

    istringstream in(output.str());

    while (in)
    {
        in.getline(buf, 1023, '\n');

        strcat(buf,"\n");

        if (buf[0]=='!') continue;

        string slot="THE_REST";
        for (list<string>::iterator i=commands.begin(); i!=commands.end(); ++i)
        {
            if (strncmp(buf, (*i).c_str(), (*i).size())==0)
            {
                slot= *i;
                break;
            }
        }
        script[slot].push_back(buf);
    }

    output.str("");

    for (list<string>::iterator i=commands.begin(); i!=commands.end(); ++i)
    {

        for (list<string>::iterator j=script[*i].begin(); j!=script[*i].end(); ++j)
            output << *j;
        output << "! \n";
        output << "! \n";
    }
}


void NATCompiler_pix::epilog()
{
    if ( fw->getOptionsObject()->getBool("pix_regroup_commands")) 
    {
        info(" Regrouping commands");
        regroup();
    }
}

string NATCompiler_pix::printClearCommands()
{
    ostringstream output;
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");

    if ( !fw->getOptionsObject()->getBool("pix_acl_no_clear") &&
         !inSingleRuleCompileMode())
    {
        output << Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/") + 
            "version_" + version + "/pix_commands/clear_xlate") << endl;
        output << Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/") + 
            "version_" + version + "/pix_commands/clear_static") << endl;
        output << Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/") + 
            "version_" + version + "/pix_commands/clear_global") << endl;
        output << Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/") + 
            "version_" + version + "/pix_commands/clear_nat") << endl;
    }

    return output.str();
}

/*
 * This includes commands that should be added first, such as commit mode
 * for FWSM, setting up temporary access list etc.
 */
string NATCompiler_pix::printPreambleCommands()
{
    return "";
}

class MergeConflictRes : public FWObjectDatabase::ConflictResolutionPredicate
{
    public:
    MergeConflictRes() { }
    virtual bool askUser(FWObject*, FWObject*) {return false;}
};

void NATCompiler_pix::setNamedObjectsManager(NamedObjectsManager *mgr)
{
    named_objects_manager = mgr;
}

