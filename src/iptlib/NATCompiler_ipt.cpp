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

#include "NATCompiler_ipt.h"
#include "OSConfigurator_linux24.h"
#include "ipt_utils.h"

#include "combinedAddress.h"

#include "fwcompiler/OSConfigurator.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/ObjectMatcher.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"


#include <QString>
#include <QRegExp>
#include <QtDebug>

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <cstring>

#include <assert.h>
#include <ctype.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

struct subnetInfo {
    Interface *iface;
    IPv4      *ipv4;
    int        nmlength;
    subnetInfo()                           { iface=nullptr; ipv4=nullptr; nmlength=0; }
    subnetInfo(Interface *i,IPv4 *a,int n) { iface=i;    ipv4=a;    nmlength=n; }
};


#if 0
static int chain_no=0;
#endif

static std::map<std::string,int> tmp_chain_no;
static std::list<std::string> standard_chains;

const std::list<std::string>& NATCompiler_ipt::getStandardChains()
{
    if (standard_chains.size()==0)
    {
        standard_chains.push_back("POSTROUTING");
        standard_chains.push_back("PREROUTING");
        standard_chains.push_back("SNAT");
        standard_chains.push_back("DNAT");
        standard_chains.push_back("MASQUERADE");
        standard_chains.push_back("REDIRECT");
        standard_chains.push_back("NETMAP");
        standard_chains.push_back("LOG");
        standard_chains.push_back("MARK");
        standard_chains.push_back("ACCEPT");
        standard_chains.push_back("REJECT");
        standard_chains.push_back("DROP");
        standard_chains.push_back("RETURN");
        standard_chains.push_back("OUTPUT");
    }
    return standard_chains;
}


string NATCompiler_ipt::myPlatformName() { return "iptables"; }

/*
 * this function generates acceptable shell variable name from
 * interface name. Note that
 * OSConfigurator_linux24::getInterfaceVarName() and
 * PolicyCompiler_ipt::getInterfaceVarName() do the same thing and
 * these functions should be identical.
 *
 *  TODO: really need to have one function for this instead of three in
 * three different classes.
 */
string NATCompiler_ipt::getInterfaceVarName(FWObject *iface, bool v6)
{
    ostringstream  ostr;
    string iname=iface->getName();
    string::size_type p1;
    while ( (p1=iname.find("."))!=string::npos) iname=iname.replace(p1,1,"_");
    while ( (p1=iname.find("-"))!=string::npos) iname=iname.replace(p1,1,"_");
    ostr << "i_" << iname;
    if (v6) ostr << "_v6";
    return ostr.str();
}

string NATCompiler_ipt::getAddressTableVarName(FWObject *at)
{
    ostringstream  ostr;
    string name=at->getName();
    string::size_type p1;
    const char *bad_shell_chars = " !#$&*()-+=\\|{}[]?<>,.:";
    for (const char *cptr=bad_shell_chars; *cptr; cptr++)
    {
        while ( (p1=name.find(*cptr))!=string::npos)
            name=name.replace(p1,1,"_");
    }
    ostr << "at_" << name;
    return ostr.str();
}

string NATCompiler_ipt::getNewTmpChainName(NATRule *rule)
{
    std::ostringstream str;
    string chain_id=rule->getUniqueId();
    int    n=tmp_chain_no[chain_id];

    str << "C" << chain_id;
    str << "." <<  setw(1) << setfill('0') << n;

    n++;
    tmp_chain_no[chain_id]=n;
    return str.str();

#if 0
    std::ostringstream str;
    str << "ntmp" << setw(3) << setfill('0') << chain_no;
    chain_no++;
    return str.str();
#endif
}

string NATCompiler_ipt::debugPrintRule(Rule *r)
{
    NATRule *rule = NATRule::cast(r);

    return NATCompiler::debugPrintRule(rule)+
        " c=" + rule->getStr("ipt_chain") +
        " t=" + rule->getStr("ipt_target") +
        " (type="+rule->getRuleTypeAsString()+")";

}

void NATCompiler_ipt::verifyPlatform()
{
    string family = Resources::platform_res[fw->getStr("platform")]->
        getResourceStr("/FWBuilderResources/Target/family");

    if (family != myPlatformName()) 
	abort("Unsupported platform " + fw->getStr("platform") +
              " (family " + family + ")");
}

int NATCompiler_ipt::prolog()
{
    verifyPlatform(); 

    // initialize counters for the standard chains
    for (list<string>::const_iterator i =
             NATCompiler_ipt::getStandardChains().begin();
         i != NATCompiler_ipt::getStandardChains().end(); ++i)
    {
        chain_usage_counter[*i] = 1;
    }

    int n = NATCompiler::prolog();

    if ( n>0 ) 
    {
	list<FWObject*> l2=fw->getByTypeDeep(Interface::TYPENAME);
	for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
        {
	    Interface *iface=dynamic_cast<Interface*>(*i);
	    assert(iface);

            if ( iface->isDyn())  iface->setBool("use_var_address",true);
	}

        build_interface_groups(dbcopy, persistent_objects, fw, ipv6, regular_interfaces);
    }

    string version = fw->getStr("version");
    using_ipset = (XMLTools::version_compare(version, "1.4.1.1") >= 0 &&
                          fwopt->getBool("use_m_set"));

    return n;
}


void NATCompiler_ipt::_expand_interface(Rule *rule,
                                        Interface *iface,
                                        std::list<FWObject*> &list_result,
                                        bool expand_cluster_interfaces_fully)
{
    std::list<FWObject*> ol1;

    Compiler::_expand_interface(
        rule, iface, ol1, expand_cluster_interfaces_fully);

    // see utils.cpp
    expand_interface_with_phys_address(this, rule, iface, ol1, list_result);
}

bool compare_addresses_ptr(const InetAddr* a1, const InetAddr* a2)
{
    return (*a1 < *a2);
}


/*
 * call this processor after classifyNATRules
 */
bool NATCompiler_ipt::ConvertLoadBalancingRules::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    if (rule->getRuleType()==NATRule::LB)
    {
        RuleElementTDst  *tdst=rule->getTDst();  assert(tdst);

        list<const InetAddr*> al;
        for(list<FWObject*>::iterator i=tdst->begin(); i!=tdst->end(); i++) 
        {
            FWObject *o= *i;
            FWObject *obj = nullptr;
            if (FWReference::cast(o)!=nullptr)
                obj=FWReference::cast(o)->getPointer();
            //const InetAddrMask *a = Address::cast(obj)->getAddressObjectInetAddrMask();
            const InetAddr *ip_addr = Address::cast(obj)->getAddressPtr();
            al.push_back( ip_addr );
        }

        al.sort(compare_addresses_ptr);

        const InetAddr* a1 = al.front();
        list<const InetAddr*>::iterator j=al.begin();
        j++;

        for ( ; j!=al.end(); j++)
        {
/* I use temporary AddressRange object here because it takes care of
 * big endian/little endian conversion for me 
 */
            AddressRange tar;
            tar.setRangeStart( *a1 );
            tar.setRangeEnd( *(*j) );
            if ( tar.dimension() != 2 )
            {
                compiler->abort(
                    rule,
                    "Non-contiguous address range in "
                    "Translated Destination in load balancing NAT rule");
            }
            a1 = *j;
        }

        AddressRange *ar = compiler->dbcopy->createAddressRange();
        ar->setRangeStart( *(al.front()) );
        ar->setRangeEnd( *(al.back()) );
        ar->setName(string("%")+al.front()->toString()
                    +"-"+al.back()->toString()+"%" );
        compiler->persistent_objects->add(ar,false);
        tdst->clearChildren();
        tdst->addRef(ar);

        rule->setRuleType(NATRule::DNAT);
    }

    return true;
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
bool NATCompiler_ipt::splitSDNATRule::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    if ( rule->getRuleType()==NATRule::SDNAT)
    {
//        RuleElementOSrc *osrc;
        RuleElementODst *odst;
        RuleElementOSrv *osrv;
        RuleElementTSrc *tsrc;
        RuleElementTDst *tdst;
        RuleElementTSrv *tsrv;

        bool tsrv_translates_src_port = false;
        bool tsrv_translates_dst_port = false;

        Service  *osrv_obj = compiler->getFirstOSrv(rule);
        Service  *tsrv_obj = compiler->getFirstTSrv(rule);
        
        if (TCPUDPService::cast(osrv_obj) && TCPUDPService::cast(tsrv_obj))
        {
            TCPUDPService *tu_osrv = TCPUDPService::cast(osrv_obj);
            TCPUDPService *tu_tsrv = TCPUDPService::cast(tsrv_obj);

            tsrv_translates_src_port =
                (tu_tsrv->getSrcRangeStart() != 0 && tu_tsrv->getDstRangeStart() == 0);
            tsrv_translates_dst_port =
                (tu_tsrv->getSrcRangeStart() == 0 && tu_tsrv->getDstRangeStart() != 0);

            if (tsrv_translates_dst_port &&
                tu_osrv->getDstRangeStart() == tu_tsrv->getDstRangeStart() &&
                tu_osrv->getDstRangeEnd() == tu_tsrv->getDstRangeEnd())
                tsrv_translates_dst_port = false;  // osrv and tsrv define the same ports

            if (tsrv_translates_src_port &&
                tu_osrv->getSrcRangeStart() == tu_tsrv->getSrcRangeStart() &&
                tu_osrv->getSrcRangeEnd() == tu_tsrv->getSrcRangeEnd())
                tsrv_translates_src_port = false;  // osrv and tsrv define the same ports
        }

/* first rule translates destination and may translate service (depends
 * on the original rule). Set type to Unknown because this may become
 * DNAT or DNetNat - we will decide later.
 */
        NATRule *r = compiler->dbcopy->createNATRule();
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);
        r->setRuleType(NATRule::Unknown);
                
        tsrc = r->getTSrc();
        tsrc->clearChildren();
        tsrc->setAnyElement();

/* this rule translates destination and can't deal with source port
 * translation. Leave that to the second rule
 */
        if (tsrv_translates_src_port)
        {
            tsrv = r->getTSrv();
            tsrv->clearChildren();
            tsrv->setAnyElement();
        }

        tmp_queue.push_back(r);

/* the second rule translates source and uses translated object in
 * ODst. Since the service could have been translated by the first
 * rule, we use TSrv in OSrv
 */
        r = compiler->dbcopy->createNATRule();
        r->duplicate(rule);
        compiler->temp_ruleset->add(r);
        r->setRuleType(NATRule::Unknown);
                
/* if original rule involved negation in ODst, it should be processed
 * in the first of the two rules we create for SDNAT. Negation in OSrc
 * must be processed in both rules since the first rule does not
 * change OSrc
 */

        odst = r->getODst();
        odst->setNeg(false);

        odst->clearChildren();
        for (FWObject::iterator i=rule->getTDst()->begin(); i!=rule->getTDst()->end(); i++)
        {
            FWObject *obj = FWObjectReference::getObject(*i);
            odst->addRef(obj);
        }

        if ( ! rule->getTSrv()->isAny())
        {
            /*
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

        tdst = r->getTDst();
        tdst->clearChildren();
        tdst->setAnyElement();

        if (tsrv_translates_dst_port)
        {
            tsrv = r->getTSrv();
            tsrv->clearChildren();
            tsrv->setAnyElement();
        }

        tmp_queue.push_back(r);
    }
    else 
        tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::VerifyRules::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

#ifndef NDEBUG
    RuleElementOSrc  *osrc=rule->getOSrc();  assert(osrc);
    RuleElementODst  *odst=rule->getODst();  assert(odst);
    RuleElementOSrv  *osrv=rule->getOSrv();  assert(osrv);
#endif

    RuleElementTSrc  *tsrc=rule->getTSrc();  assert(tsrc);
    RuleElementTDst  *tdst=rule->getTDst();  assert(tdst);
    RuleElementTSrv  *tsrv=rule->getTSrv();  assert(tsrv);

    if (tsrc->getNeg())
    {
	compiler->abort(
            rule, 
            "Can not use negation in translated source ");
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

    if ( Group::cast( compiler->getFirstTSrv(rule) )!=nullptr)
    {
	compiler->abort(
            rule, 
            "Can not use group in translated service.");
        return true;
    }


    if (rule->getRuleType()==NATRule::LB)
    {
        compiler->abort(
            rule, 
            "Load balancing rules are not supported.");
        return true;
    }

    // Note that in -xt mode and in single rule compile compiler->abort
    // does not really abort processing
    if (rule->getRuleType()==NATRule::NATBranch)
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

    if (rule->getRuleType()==NATRule::SNAT ) 
    {
        FWObject *o1 = FWReference::getObject(tsrc->front());
        if ( ! tsrc->isAny() && Network::cast(o1)!=nullptr)
        {
            compiler->abort(
                rule, 
                "Can not use network object in translated source.");
            return true;
        }

        if (Interface::isA(o1) && Interface::cast(o1)->isUnnumbered())
        {
            compiler->abort(
                rule,
                "Can not use unnumbered interface in "
                "Translated Source of a Source translation rule.");
            return true;
        }
    }

    if (rule->getRuleType()==NATRule::SNetnat && !tsrc->isAny() ) 
    {
        Network *a1=Network::cast(compiler->getFirstOSrc(rule));
        Network *a2=Network::cast(compiler->getFirstTSrc(rule));
        if ( a1==nullptr || a2==nullptr ||
             a1->getNetmaskPtr()->getLength() != a2->getNetmaskPtr()->getLength() )
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
             a1->getNetmaskPtr()->getLength() != a2->getNetmaskPtr()->getLength() )
        {
            compiler->abort(
                rule, 
                "Original and translated destination should both be networks of the same size .");
            return true;
        }

    }

    tmp_queue.push_back(rule);

    return true;
}

/*
 * this should be called only after groupServicesByProtocol, so that we have
 * objects of the same type in OSrv and either "any" or a single
 * object in TSrv
 */
bool NATCompiler_ipt::VerifyRules2::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getRuleType()!= NATRule::Return)
    {
        RuleElementOSrv  *osrv=rule->getOSrv();  assert(osrv);
        RuleElementTSrv  *tsrv=rule->getTSrv();  assert(tsrv);

        Service *s1=compiler->getFirstOSrv(rule);
        Service *s2=compiler->getFirstTSrv(rule);

        if (osrv->isAny() && ! tsrv->isAny())
        {
            compiler->abort(
                rule, 
                "Can not use service object in Translated Service if Original Service is 'Any'.");
            return true;
        }

        if (!tsrv->isAny() && s1->getProtocolName()!=s2->getProtocolName())
        {
            compiler->abort(
                rule, 
                "Translated Service should be either 'Original' or should contain object of the same type as Original Service.");
            return true;
        }

    }

    tmp_queue.push_back(rule);
    return true;
}

/*
 * make sure combination of "-i" or "-o" interface spec and chosen chain
 * is allowed
 */
bool NATCompiler_ipt::VerifyRules3::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElement *itf_i_re = rule->getItfInb();
    assert(itf_i_re!=nullptr);
    RuleElement *itf_o_re = rule->getItfOutb();
    assert(itf_o_re!=nullptr);

    if (rule->getRuleType()==NATRule::SNAT &&  ! itf_i_re->isAny())
    {
        // iptables does not allow "-i" in POSTROUTING chain
        compiler->abort(
            rule,
            "Can not use inbound interface specification with "
            "rules that translate source because iptables does not "
            "allow \"-i\" in POSTROUTING chain");
        return true;
    }

    if (rule->getRuleType()==NATRule::DNAT &&  ! itf_o_re->isAny())
    {
        // iptables does not allow "-o" in PREROUTING chain
        compiler->abort(
            rule,
            "Can not use outbound interface specification with "
            "rules that translate destination because iptables does not "
            "allow \"-o\" in PREROUTING chain");
        return true;
    }

    string chain = rule->getStr("ipt_chain");

    if (chain == "OUTPUT" && ! itf_i_re->isAny())
    {
        // iptables does not allow "-i" in POSTROUTING chain
        compiler->abort(
            rule,
            "Can not use inbound interface specification with "
            "this rule  because iptables does not "
            "allow \"-i\" in OUTPUT chain");
        return true;
    }

    tmp_queue.push_back(rule);
    return true;
}


bool NATCompiler_ipt::convertToAtomicportForOSrv::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getOSrv()->size()>1 && ! rule->getTSrv()->isAny())
    {
        RuleElementOSrv *osrv=rule->getOSrv();    assert(osrv);

        for (FWObject::iterator i1=osrv->begin(); i1!=osrv->end(); ++i1) 
        {
            NATRule *r = compiler->dbcopy->createNATRule();
            r->duplicate(rule);
            compiler->temp_ruleset->add(r);

            FWObject *s;
        
            s = r->getOSrv();	assert(s);
            s->clearChildren();
            s->addRef(FWReference::getObject(*i1));
            
            tmp_queue.push_back(r);
        }
    }
    else
        tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::portTranslationRules::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    Address  *odst=compiler->getFirstODst(rule);
//    Service  *osrv=compiler->getFirstOSrv(rule);

    Address  *tsrc=compiler->getFirstTSrc(rule);
    Address  *tdst=compiler->getFirstTDst(rule);
    Service  *tsrv=compiler->getFirstTSrv(rule);

    if (rule->getRuleType() == NATRule::DNAT &&
        tsrc->isAny() &&   tdst->isAny() && ! tsrv->isAny() && odst->getId()==compiler->fw->getId() )
    {
        rule->getTDst()->addRef( odst );
    }

    tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::specialCaseWithRedirect::processNext()
{
    NATRule *rule = getNext(); if (rule==nullptr) return false;

    Address *tdst = compiler->getFirstTDst(rule);

/* we consider rule redirect only if TDst is a firewall object
 *
 */
    int fw_id = compiler->fw->getId();
    int cluster_id = -1;
    bool cluster_member = compiler->fw->getOptionsObject()->getBool("cluster_member");
    Cluster *cluster = nullptr;
    if (cluster_member)
    {
        cluster = Cluster::cast(
            compiler->dbcopy->findInIndex(compiler->fw->getInt("parent_cluster_id")));
        cluster_id = cluster->getId();
    }

    if (rule->getRuleType() == NATRule::DNAT && 
        (tdst->getId() == fw_id || tdst->getId() == cluster_id))
        rule->setRuleType(NATRule::Redirect);

    tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::splitOnODst::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementODst  *odst=rule->getODst();  assert(odst);
    if (rule->getRuleType()==NATRule::DNAT && odst->size()!=1)
    {
	for(list<FWObject*>::iterator i=odst->begin(); i!=odst->end(); ++i) 
        {
            FWObject *o= *i;
            if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
	    Address *a=Address::cast( o );
	    assert(a);

	    NATRule *r= compiler->dbcopy->createNATRule();
	    compiler->temp_ruleset->add(r);
	    r->duplicate(rule);
	    RuleElementODst *nodst=r->getODst();
	    nodst->clearChildren();

	    nodst->addRef( a );

	    tmp_queue.push_back( r );
        }
    } else
	tmp_queue.push_back(rule);

    return true;
}


bool NATCompiler_ipt::splitOnOSrv::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrv  *osrv=rule->getOSrv();  assert(osrv);
    if (osrv->size()!=1) 
    {
	for(list<FWObject*>::iterator i=osrv->begin(); i!=osrv->end(); ++i) 
        {
            FWObject *o= *i;
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

bool NATCompiler_ipt::fillTranslatedSrv::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    Service  *osrv_o=compiler->getFirstOSrv(rule);
    Service  *tsrv_o=compiler->getFirstTSrv(rule);

    if ( ! osrv_o->isAny()  && tsrv_o->isAny() ) {
	RuleElementTSrv  *tsrv=rule->getTSrv();
	tsrv->addRef(osrv_o);
    }
    return true;
}

bool NATCompiler_ipt::addVirtualAddress::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    bool cluster_member = compiler->fw->getOptionsObject()->getBool("cluster_member");
    Cluster *cluster = nullptr;
    if (cluster_member)
        cluster = Cluster::cast(
            compiler->dbcopy->findInIndex(compiler->fw->getInt("parent_cluster_id")));

    Address *a = nullptr;

    if (rule->getRuleType()==NATRule::SNAT || rule->getRuleType()==NATRule::DNAT) 
    {
        if (rule->getRuleType()==NATRule::SNAT)   
            a = compiler->getFirstTSrc(rule);
        else
            a = compiler->getFirstODst(rule);

        Interface *iface = Interface::cast(a);
        if (iface && ! iface->isRegular()) return true;

        if ( ! a->isAny() &&
             ! compiler->complexMatch(a, compiler->fw) &&
             ! compiler->complexMatch(a, cluster))
        {
            if (AddressRange::cast(a)!=nullptr)
            {
                compiler->warning(
                        rule, 
                        string("Adding of virtual address for address range "
                               "is not implemented (object ") +
                        a->getName() + ")" );
            } else
                compiler->osconfigurator->addVirtualAddressForNAT( a );
        }
        return true;
    }


    if (rule->getRuleType()==NATRule::SNetnat || rule->getRuleType()==NATRule::DNetnat)
    {
        if (rule->getRuleType()==NATRule::SNetnat)   
            a=compiler->getFirstTSrc(rule);
        else
            a=compiler->getFirstODst(rule);

        if ( ! a->isAny() && Network::cast(a) )
            compiler->osconfigurator->addVirtualAddressForNAT( Network::constcast(a) );

        return true;
    }

    return true;
}

bool NATCompiler_ipt::splitRuleIfRuleElementIsDynamicInterface::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElement *re =RuleElement::cast(rule->getFirstByType(re_type));
    int nre = re->size();

    vector<FWObject*> cl;

    for(list<FWObject*>::iterator i=re->begin(); nre>1 && i!=re->end(); ++i) 
    {
        FWObject *o= *i;
	FWObject *obj = nullptr;
        if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();
        Interface *iface=Interface::cast(obj);
        if (iface!=nullptr && !iface->isRegular())
        {
	    cl.push_back(o);   // can not remove right now because remove invalidates iterator
            nre--;

	    NATRule  *new_rule= compiler->dbcopy->createNATRule();
	    compiler->temp_ruleset->add(new_rule);
	    new_rule->duplicate(rule);
            RuleElement *new_re=RuleElement::cast(new_rule->getFirstByType(re_type));
	    new_re->clearChildren();
	    new_re->setAnyElement();
	    new_re->addRef( iface );
	    tmp_queue.push_back(new_rule);
        }
    }

    if (!cl.empty())
    {
        for (vector<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
            re->remove( (*i1) );
    }

    tmp_queue.push_back(rule);

    return true;
}



bool NATCompiler_ipt::specialCaseWithUnnumberedInterface::dropUnnumberedInterface(RuleElement *re)
{
    if (re->isAny()) return true;
    list<FWObject*> cl;
    for (list<FWObject*>::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs   =Interface::cast( obj );

        if (ifs!=nullptr &&
            (ifs->isUnnumbered() || ifs->isBridgePort())
           ) cl.push_back(obj);
    }
    if (!cl.empty()) 
    {
        for (list<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
            re->removeRef( (*i1) );
    }
    return (!re->isAny());
}

bool NATCompiler_ipt::specialCaseWithUnnumberedInterface::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    bool keep_rule=true;
    switch (rule->getRuleType()) {
    case NATRule::Masq:
    case NATRule::SNAT:
        keep_rule=dropUnnumberedInterface( rule->getOSrc() );
        break;
    case NATRule::DNAT:
        keep_rule=dropUnnumberedInterface( rule->getODst() );
        break;
    default: ;
    }
    if (keep_rule) tmp_queue.push_back(rule);
    return true;
}


/*
 *  I assume that there is always only one object in ODst, TSrc and TDst
 *  rule elements. This should have been assured by inspector VerifyRules
 */
bool NATCompiler_ipt::ReplaceFirewallObjectsODst::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    bool cluster_member = compiler->fw->getOptionsObject()->getBool("cluster_member");
    Cluster *cluster = nullptr;
    if (cluster_member)
        cluster = Cluster::cast(
            compiler->dbcopy->findInIndex(compiler->fw->getInt("parent_cluster_id")));

    tmp_queue.push_back(rule);

    list<FWObject*> cl;
    RuleElementODst *rel;
    Address     *obj=nullptr;

    switch (rule->getRuleType()) {

    case NATRule::Masq:
//    case NATRule::Redirect: 
	return true;
    default:
	rel=rule->getODst();               assert(rel);
	obj=compiler->getFirstODst(rule);  assert(obj!=nullptr);

	if (obj->getId()==compiler->fw->getId() ||
            (cluster && obj->getId()==cluster->getId()))
	{

	    list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
	    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
            {
		Interface *interface_=Interface::cast(*i);
		if (interface_->isLoopback()) continue;
                if (cluster &&
                    ! interface_->getOptionsObject()->getBool("cluster_interface"))
                    continue;
                cl.push_back(interface_);
	    }
	    if ( ! cl.empty() )
            {
		while (rel->size()) 
		    rel->remove( rel->front()  );
		
		for (FWObject::iterator i1=cl.begin(); i1!=cl.end(); ++i1) 
		{
		    rel->addRef( *i1 );
		}
	    }
	}
    }
    return true;
}

/*
 * This processor works together with ConvertToAtomicRules and
 * AssignInterfaces. If firewall object is used in TSrc of SNAT rule,
 * it gets replaced with its interfaces. ConvertToAtomicRules slits
 * this rule onto atomic rules, each of which has one interface object
 * in TSrc. AssigInterfaces then assigns each atomic rule to
 * corresponding interface.
 *
 * it seems the simplest way is just to assign SNAT rule to all
 * interfaces if firewall is used in TSrc.  This automatically takes
 * care of weird cases where people use address that belongs to subnet
 * of one interface to do translation of packets going out through
 * another interface. Basically, compiler does not have information
 * about routing, so we have no choice but assume the routing can be
 * anything and assign the rule to all interfaces.
 */
bool NATCompiler_ipt::ReplaceFirewallObjectsTSrc::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    bool cluster_member = compiler->fw->getOptionsObject()->getBool("cluster_member");
    Cluster *cluster = nullptr;
    if (cluster_member)
        cluster = Cluster::cast(
            compiler->dbcopy->findInIndex(compiler->fw->getInt("parent_cluster_id")));

    tmp_queue.push_back(rule);

    list<FWObject*> cl;
    RuleElementTSrc *rel;
    Address     *obj=nullptr;

    switch (rule->getRuleType()) {

    case NATRule::Masq:
    case NATRule::Redirect: return true;

    default:
	rel=rule->getTSrc();               assert(rel);
	obj=compiler->getFirstTSrc(rule);  assert(obj!=nullptr);

	if (obj->getId()==compiler->fw->getId() ||
            (cluster && obj->getId()==cluster->getId()))
	{
            RuleElementODst *odstrel = rule->getODst();
            Address *osrc = compiler->getFirstOSrc(rule);
            Address *odst = compiler->getFirstODst(rule);

            rel->clearChildren();

            Interface *odst_iface = nullptr;
            if (cluster)
                odst_iface = compiler->findInterfaceFor(odst, cluster);
            else
                odst_iface = compiler->findInterfaceFor(odst, compiler->fw);

            Interface *osrc_iface = nullptr;
            if (cluster)
                osrc_iface = compiler->findInterfaceFor(osrc, compiler->fw);
            else
                osrc_iface = compiler->findInterfaceFor(osrc, compiler->fw);


            if (!odst->isAny() && odst_iface!=nullptr &&
                !odstrel->getBool("single_object_negation"))
            {
                rel->addRef(odst_iface);
            } else
            {
                /*
                 * else use all interfaces except loopback and
                 * unnumbered ones.  Skip interface connected to ODst
                 * if single object negation was detected in ODst. For
                 * cluster members use only copy of cluster interfaces
                 * (ticket #1185)
                 */


                list<FWObject*> l2 = compiler->fw->getByTypeDeep(Interface::TYPENAME);
                for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
                {
                    Interface *iface = Interface::cast(*i);
                    if (iface->isLoopback() ||
                        iface->isUnnumbered() ||
                        iface->isBridgePort() ) continue;

                    // Note: comparing osrc_iface and odst_iface by name
                    // because these objects are children of cluster
                    if (odstrel->getBool("single_object_negation") && odst_iface &&
                        odst_iface->getName()==iface->getName()) continue;

                    if (osrc_iface && osrc_iface->getName() == iface->getName())
                        continue;

                    if (cluster &&
                        ! iface->getOptionsObject()->getBool("cluster_interface"))
                        continue;

                    rel->addRef( *i );
                }

                for (FWObject::iterator i1=cl.begin(); i1!=cl.end(); ++i1)
                    rel->addRef( *i1 );

/* it is an error if rule element is empty at this point. this could have
 * happened if all external interfaces are unnumbered */
                if (rel->size()==0)
                {
                    compiler->abort(
                        rule, 
                        "Could not find suitable interface for the NAT rule. "
                        "Perhaps all interfaces are unnumbered?");
                }
            }
	}
    }
    return true;
}

bool NATCompiler_ipt::splitMultiSrcAndDst::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrv *osrv=rule->getOSrv();
    RuleElementOSrc *osrc=rule->getOSrc();
    RuleElementODst *odst=rule->getODst();
    RuleElementOSrc *rosrc;
    RuleElementODst *rodst;

    int nosrv=osrv->size();
    int nosrc=osrc->size();
    int nodst=odst->size();

/*
 * Return if service is set - svcs my introduce complications and I'm
 * treading carefully here.
 * We don't handle anything thats redirect, MASQ yet - just NONAT,SNAT & DNAT 
 * We also check we've got multiple rules to deal with - we can't optimize
 * 1 src with 1 dst ...
 */
 
    if ((nosrv>1  || !(osrv->isAny())) || 
        (nosrc<1 || osrc->isAny())  ||
        (nodst<1 || odst->isAny())  ||
        (nosrc==1 && nodst==1) )
    {
        tmp_queue.push_back(rule);
        return true;
    }
      
    switch (rule->getRuleType())
    {
    case NATRule::NONAT:     
    case NATRule::SNAT:     
    case NATRule::DNAT:
    { 
// get old chain name create new chain name
        string new_chain = NATCompiler_ipt::getNewTmpChainName(rule);
// create new rule
        NATRule *r = compiler->dbcopy->createNATRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule); 
// move existing rule onto new chain
        rule->setStr("ipt_chain", new_chain);
// we've already tested for interface ....
        rule->setStr(".iface_in", "nil");
        rule->setStr(".iface_out", "nil");
// new rule points to new chain, continues if no match
        r->setStr("ipt_target", new_chain);

// Now decide which way round would be best ...
        if (nosrc < nodst) 
        {
            rodst= r->getODst();
            rodst->clearChildren();
            rodst->setAnyElement();
            osrc->clearChildren();
            osrc->setAnyElement();
        } else
        {
            rosrc = r->getOSrc();
            rosrc->clearChildren();
            rosrc->setAnyElement();
            odst->clearChildren();
            odst->setAnyElement();
        }
               
        tmp_queue.push_back(r); 
        
    }
    break;
    default: ;
    }

    tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::dynamicInterfaceInODst::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    RuleElementODst  *odstrel=rule->getODst();  assert(odstrel);
    Address          *odst = compiler->getFirstODst(rule);
    if ( ! odstrel->isAny() )
    {
        Interface *iface = Interface::cast(odst);
        if (iface!=nullptr && iface->isDyn() && iface->isFailoverInterface())
        {
            Address *new_odst = compiler->correctForCluster(odst);
            RuleElementODst *odst_re = rule->getODst();  assert(odst_re);
            odst_re->removeRef(odst);
            odst_re->addRef(new_odst);
        }
    }
    return true;
}

bool NATCompiler_ipt::dynamicInterfaceInTSrc::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    FWOptions  *ruleopt =rule->getOptionsObject();
    bool use_snat = ruleopt->getBool("ipt_use_snat_instead_of_masq");

    tmp_queue.push_back(rule);

    Address *tsrc = compiler->getFirstTSrc(rule);

    if (rule->getRuleType()==NATRule::SNAT &&
        Interface::cast(tsrc)!=nullptr && !Interface::cast(tsrc)->isRegular())
    {
        Interface *iface = Interface::cast(tsrc);
        if (iface->isFailoverInterface())
        {
            Address *new_tsrc = compiler->correctForCluster(tsrc);
            RuleElementTSrc *tsrc_re = rule->getTSrc();  assert(tsrc);
            tsrc_re->removeRef(tsrc);
            tsrc_re->addRef(new_tsrc);
        }

        if (use_snat)
        {
            // Emulate SNAT with dynamic interface
            //tsrc->setBool("use_var_address", true);
        } else
        {
            rule->setRuleType(NATRule::Masq);
            if (rule->getStr("ipt_target")=="" || rule->getStr("ipt_target")=="SNAT")
                rule->setStr("ipt_target", "MASQUERADE");
        }
    }
    return true;
}

bool NATCompiler_ipt::alwaysUseMasquerading::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    FWOptions  *ruleopt =rule->getOptionsObject();
    bool use_masq = ruleopt->getBool("ipt_use_masq");

    tmp_queue.push_back(rule);

    if (use_masq && rule->getRuleType()==NATRule::SNAT)
    {
        rule->setRuleType(NATRule::Masq);
        if (rule->getStr("ipt_target")=="" || rule->getStr("ipt_target")=="SNAT")
            rule->setStr("ipt_target", "MASQUERADE");
    }
    return true;
}

/**
 * unlike standard inspector addressRanges in the base class NATCompiler,
 * this one does not expand address ranges in TSrc and TDst because
 * iptables supports ranges in those rule elements
 */
bool NATCompiler_ipt::ExpandAddressRanges::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    RuleElement *rel;

    rel=rule->getOSrc();    assert(rel);
    compiler->_expandAddressRanges(rule,rel);
    rel=rule->getODst();    assert(rel);
    compiler->_expandAddressRanges(rule,rel);
    return true;
}



void NATCompiler_ipt::checkForDynamicInterfacesOfOtherObjects::findDynamicInterfaces(
    RuleElement *re, Rule *rule)
{
    if (re->isAny()) return;

    bool cluster_member = compiler->fw->getOptionsObject()->getBool("cluster_member");
    FWObject *cluster = nullptr;
    if (cluster_member)
        cluster = compiler->dbcopy->findInIndex(compiler->fw->getInt("parent_cluster_id"));

    list<FWObject*> cl;
    for (list<FWObject*>::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();
        Interface  *ifs   =Interface::cast( obj );

        if (ifs!=nullptr && ifs->isDyn() &&
            ! ifs->isChildOf(compiler->fw) &&
            ! ifs->isChildOf(cluster))
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

bool NATCompiler_ipt::checkForDynamicInterfacesOfOtherObjects::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    findDynamicInterfaces( rule->getOSrc() , rule );
    findDynamicInterfaces( rule->getODst() , rule );
    findDynamicInterfaces( rule->getTSrc() , rule );
    findDynamicInterfaces( rule->getTDst() , rule );

    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_ipt::prepareForMultiport::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrv *rel= rule->getOSrv();
    Service         *srv= compiler->getFirstOSrv(rule);

    if (rel->size()==1) {
	tmp_queue.push_back(rule);
	return true;
    }

/*
 * processor groupServicesByProtocol should have been called eariler, so now all
 * services in Srv are of the same type
 */
    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	rule->setBool("ipt_multiport",true);
/* make sure we have no more than 15 ports */
	if (rel->size()>15) 
        {
	    int n=0;
	    NATRule         *r;
	    RuleElementOSrv *nsrv=nullptr;
	    for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
            {
		FWObject *o= *i;
		if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
	    
		Service *s=Service::cast( o );
		assert(s);

		if (n==0) 
                {
		    r= compiler->dbcopy->createNATRule();
		    compiler->temp_ruleset->add(r);
		    r->duplicate(rule);
		    nsrv=r->getOSrv();
		    nsrv->clearChildren();
		    tmp_queue.push_back(r);
		}
                assert(nsrv!=nullptr);
		nsrv->addRef( s );
		if (++n>=15) n=0;
	    }
	} else {
	    tmp_queue.push_back(rule);
	}

//	tmp_queue.push_back(rule);
    } else
	tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_ipt::splitMultipleICMP::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrv *rel= rule->getOSrv();
    Service         *srv= compiler->getFirstOSrv(rule);

    if (rel->size()==1) {
	tmp_queue.push_back(rule);
	return true;
    }

    if (ICMPService::isA(srv))
    {
        NATRule         *r;
        RuleElementOSrv *nsrv;
        for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
        {
            FWObject *o= *i;
            if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
	    
            Service *s=Service::cast( o );
            assert(s);

            r= compiler->dbcopy->createNATRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            nsrv=r->getOSrv();
            nsrv->clearChildren();
            nsrv->addRef( s );
            tmp_queue.push_back(r);
        }
    } else
	tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_ipt::doOSrcNegation::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrc *osrcrel=rule->getOSrc();

/*   ! A  B  C    */

    if (osrcrel->getNeg()) {
	NATRule     *r;
	RuleElementOSrc *nsrc;
	RuleElementODst *ndst;
	RuleElementOSrv *nsrv;

	RuleElementTSrc *ntsrc;
	RuleElementTDst *ntdst;
	RuleElementTSrv *ntsrv;

	string new_chain = NATCompiler_ipt::getNewTmpChainName(rule);
	osrcrel->setNeg(false);
/*
 *   negation in OSrc :
 *
 *  CHAIN     !A    B    C    RULE_TYPE  TARGET
 *-----------------------------------------------
 * -----      any   B    C    SNAT/DNAT  TMP_CHAIN  
 * TMP_CHAIN   A   any  any   RETURN     RETURN
 * TMP_CHAIN  any  any   C   SNAT/DNAT  ---------
 */
	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrc=r->getOSrc();  nsrc->clearChildren();  nsrc->setAnyElement();
//	ntsrc=r->getTSrc();  ntsrc->clearChildren();  ntsrc->setAnyElement();
//	ntdst=r->getTDst();  ntdst->clearChildren();  ntdst->setAnyElement();
//	r->setRuleType(NATRule::Continue);
	r->setStr("ipt_target",new_chain);
//	r->setBool("rule_added_for_osrc_neg",true);
	tmp_queue.push_back(r);

/* TMP_CHAIN   A  any  any  RETURN  */
	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	ndst=r->getODst();   ndst->clearChildren();   ndst->setAnyElement();
	nsrv=r->getOSrv();   nsrv->clearChildren();   nsrv->setAnyElement();
	ntsrc=r->getTSrc();  ntsrc->clearChildren();  ntsrc->setAnyElement();
	ntdst=r->getTDst();  ntdst->clearChildren();  ntdst->setAnyElement();
	ntsrv=r->getTSrv();  ntsrv->clearChildren();  ntsrv->setAnyElement();
	ndst->setNeg(false);
	nsrv->setNeg(false);
	r->setRuleType(NATRule::Return);
	r->setStr("ipt_target","RETURN");
	r->setStr("ipt_chain",new_chain);
        r->setStr(".iface_in", "nil");
        r->setStr(".iface_out", "nil");
	//r->setInterfaceStr("nil");
	r->setBool("rule_added_for_osrc_neg",true);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any any C  ACTION  */
	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrc=r->getOSrc();  nsrc->clearChildren();  nsrc->setAnyElement();
	ndst=r->getODst();  ndst->clearChildren();  ndst->setAnyElement();
	nsrv=r->getOSrv();
	ndst->setNeg(false);
	nsrv->setNeg(false);
	r->setStr("ipt_chain",new_chain);
        r->setStr(".iface_in", "nil");
        r->setStr(".iface_out", "nil");
	//r->setInterfaceStr("nil");
	r->setBool("rule_added_for_osrc_neg",true);
	tmp_queue.push_back(r);

    } else
	tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::doODstNegation::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementODst *odstrel=rule->getODst();

/*   ! A  B  C    */

    if (odstrel->getNeg()) {
	NATRule     *r;
	RuleElementOSrc *nsrc;
	RuleElementODst *ndst;
	RuleElementOSrv *nsrv;

	RuleElementTSrc *ntsrc;
	RuleElementTDst *ntdst;
	RuleElementTSrv *ntsrv;

	string new_chain=NATCompiler_ipt::getNewTmpChainName(rule);
	odstrel->setNeg(false);
/*
 *   negation in Odst :
 *
 *  CHAIN      A    !B   C    RULE_TYPE  TARGET
 *-----------------------------------------------
 * -----       A   any   C    SNAT/DNAT  TMP_CHAIN  
 * TMP_CHAIN  any   B   any   RETURN     RETURN
 * TMP_CHAIN  any  any   C    SNAT/DNAT  ---------
 */
	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	ndst=r->getODst();   ndst->clearChildren();   ndst->setAnyElement();
//	ntsrc=r->getTSrc();  ntsrc->clearChildren();  ntsrc->setAnyElement();
//	ntdst=r->getTDst();  ntdst->clearChildren();  ntdst->setAnyElement();
//	r->setRuleType(NATRule::Continue);
	r->setStr("ipt_target",new_chain);
	r->setBool("rule_added_for_odst_neg",true);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  B  any  RETURN  */
	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrc=r->getOSrc();   nsrc->clearChildren();   nsrc->setAnyElement();
	nsrv=r->getOSrv();   nsrv->clearChildren();   nsrv->setAnyElement();
	ntsrc=r->getTSrc();  ntsrc->clearChildren();  ntsrc->setAnyElement();
	ntdst=r->getTDst();  ntdst->clearChildren();  ntdst->setAnyElement();
	ntsrv=r->getTSrv();  ntsrv->clearChildren();  ntsrv->setAnyElement();
	nsrc->setNeg(false);
	nsrv->setNeg(false);
	r->setRuleType(NATRule::Return);
	r->setStr("ipt_target","RETURN");
	r->setStr("ipt_chain",new_chain);
        r->setStr(".iface_in", "nil");
        r->setStr(".iface_out", "nil");
	//r->setInterfaceStr("nil");
//	r->setBool("rule_added_for_odst_neg",true);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  any  C  ACTION  */
	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrc=r->getOSrc();  nsrc->clearChildren();  nsrc->setAnyElement();
	ndst=r->getODst();  ndst->clearChildren();  ndst->setAnyElement();
	nsrv=r->getOSrv();
	nsrc->setNeg(false);
	nsrv->setNeg(false);
	r->setStr("ipt_chain",new_chain);
        r->setStr(".iface_in", "nil");
        r->setStr(".iface_out", "nil");
	//r->setInterfaceStr("nil");
	r->setBool("rule_added_for_odst_neg",true);
	tmp_queue.push_back(r);

    } else
	tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::doOSrvNegation::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrv *osrvrel=rule->getOSrv();

/*   A  B ! C    */

    if (osrvrel->getNeg()) {
	NATRule     *r;
	RuleElementOSrc *nsrc;
	RuleElementODst *ndst;
	RuleElementOSrv *nsrv;

	RuleElementTSrc *ntsrc;
	RuleElementTDst *ntdst;

	string new_chain=NATCompiler_ipt::getNewTmpChainName(rule);
	osrvrel->setNeg(false);
/*
 *   negation in OSrv :
 *
 *  CHAIN      A    B   !C    RULE_TYPE  TARGET
 *-----------------------------------------------
 * -----       A    B   any   SNAT/DNAT  TMP_CHAIN  
 * TMP_CHAIN  any  any   C    RETURN     RETURN
 * TMP_CHAIN  any  any  any   SNAT/DNAT  ---------
 */
	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrv=r->getOSrv();  nsrv->clearChildren();  nsrv->setAnyElement();
//	ntsrc=r->getTSrc();  ntsrc->clearChildren();  ntsrc->setAnyElement();
//	ntdst=r->getTDst();  ntdst->clearChildren();  ntdst->setAnyElement();
//	r->setRuleType(NATRule::Continue);
	r->setStr("ipt_target",new_chain);
	r->setBool("rule_added_for_osrv_neg",true);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  any  C  RETURN  */
	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrc=r->getOSrc();  nsrc->clearChildren();  nsrc->setAnyElement();
	ndst=r->getODst();  ndst->clearChildren();  ndst->setAnyElement();
	ntsrc=r->getTSrc();  ntsrc->clearChildren();  ntsrc->setAnyElement();
	ntdst=r->getTDst();  ntdst->clearChildren();  ntdst->setAnyElement();
	nsrc->setNeg(false);
	ndst->setNeg(false);
	r->setRuleType(NATRule::Return);
	r->setStr("ipt_target","RETURN");
	r->setStr("ipt_chain",new_chain);
        r->setStr(".iface_in", "nil");
        r->setStr(".iface_out", "nil");
	//r->setInterfaceStr("nil");
	r->setBool("rule_added_for_osrv_neg",true);
	tmp_queue.push_back(r);

/* TMP_CHAIN   any  any  any  ACTION  */
	r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
	nsrc=r->getOSrc();  nsrc->clearChildren();  nsrc->setAnyElement();
	ndst=r->getODst();  ndst->clearChildren();  ndst->setAnyElement();
	nsrv=r->getOSrv();  nsrv->clearChildren();  nsrv->setAnyElement();
	nsrc->setNeg(false);
	ndst->setNeg(false);
	r->setStr("ipt_chain",new_chain);
        r->setStr(".iface_in", "nil");
        r->setStr(".iface_out", "nil");
	//r->setInterfaceStr("nil");
//	r->setBool("rule_added_for_osrv_neg",true);
	tmp_queue.push_back(r);

    } else
	tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::splitNONATRule::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    if ( rule->getStr("ipt_chain").empty() && rule->getRuleType()==NATRule::NONAT)
    {
        Address *osrc=compiler->getFirstOSrc(rule);
        bool osrcfw= compiler->complexMatch(osrc,compiler->fw);
/*
 * NONAT is special if OSrc matches firewall. It is not sufficient to
 * only put this rule in the OUTPUT chain because packets originating
 * on the firewall actually cross both OUTPUT and POSTROUTING chains
 * (I tested this). So, we need to make sure we _do not_ translate in
 * both these chains because there may be other rules in POSTROUTING
 * chain that may accidentally match the packet and translate it.
 */
        NATRule *r= compiler->dbcopy->createNATRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);
        r->setStr("ipt_chain","POSTROUTING");  
        tmp_queue.push_back(r);

        if (osrcfw)
        {
            rule->setStr("ipt_chain","OUTPUT");
            if (osrc->getId()==compiler->fw->getId())
            {
                RuleElementOSrc *src;
                src=rule->getOSrc();
                src->clearChildren();
                src->setAnyElement();
            }
        } else          rule->setStr("ipt_chain","PREROUTING");  

        tmp_queue.push_back(rule);

    } else
        tmp_queue.push_back(rule);
    return true;
}

/**
 * Branch rule in NAT rule sets should go into PREROUTING or
 * POSTROUTING chain depending on the target of the rules in the
 * branch. Iptables verifies this when a command that passes control
 * (the one with "-j <branch_ruleset_name>") is entered. If branch
 * ruleset has -j SNAT, the command that sends control to the branch
 * should be in POSTROUTING. Attempt to place it in PREROUTING ends
 * with an error "iptables: Invalid argument".
 *
 * Note that if branch rule set contains a mix of rules that use both
 * SNAT and DNAT targets, the branching rule (that should pass control
 * to the branch) can not be added to PREROUTING and POSTROUTING
 * chains, it just gives an error "iptables: Invalid argument" for both.
 * Tested with iptables 1.4.1.1 10/20/2009
 */
bool NATCompiler_ipt::splitNATBranchRule::processNext()
{
    NATCompiler_ipt *ipt_comp = dynamic_cast<NATCompiler_ipt*>(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    if ( rule->getRuleType()==NATRule::NATBranch)
    {
        RuleSet *branch = rule->getBranch();
        if (branch) 
        {
            string branch_name  = branch->getName();

            if (ipt_comp->branch_ruleset_to_chain_mapping)
            {
                map<string, list<string> >::const_iterator lit = 
                    ipt_comp->branch_ruleset_to_chain_mapping->find(branch_name);
                if (lit!=ipt_comp->branch_ruleset_to_chain_mapping->end())
                {
                    list<string> chains = lit->second;
                    list<string>::iterator it;
                    for (it=chains.begin(); it!=chains.end(); ++it)
                    {
                        string branch_chain = *it;
                        // If chain in the branch rule set does not
                        // start with its own name plus "_", skip it
                        // because it is one of the standard chains
                        if (branch_chain.find(branch_name + "_") == 0)
                        {
                            // branch chain is <branch_ruleset_name> + "_" + <chain>
                            string my_chain = branch_chain.substr(branch_name.length()+1);
                            NATRule *r = compiler->dbcopy->createNATRule();
                            compiler->temp_ruleset->add(r);
                            r->duplicate(rule);
                            r->setStr("ipt_chain", my_chain);
                            r->setStr("ipt_target", *it);
                            tmp_queue.push_back(r);
                        }

                    }
                    return true;
                }
            }

            {
                NATRule *r;
                string prefix, new_chain, tgt_chain;
                string prepost[] = { "PRE", "POST" };
                int i;

                compiler->warning(rule,
                                  "NAT branching rule does not have information"
                                  " about targets used in the branch ruleset"
                                  " to choose proper chain in the nat table."
                                  " Will split the rule and place it in both"
                                  " PREROUTNING and POSTROUTING");

                prefix = (ipt_comp->getRuleSetName() + "_");
                if (prefix == string("NAT_")) prefix = "";

                for (i = 0; i < 2; i++) {
                   r = compiler->dbcopy->createNATRule();
                   compiler->temp_ruleset->add(r);
                   r->duplicate(rule);
                   new_chain = prefix + prepost[i] + "ROUTING";
                   tgt_chain = branch_name + "_" + prepost[i] + "ROUTING";

                   ipt_comp->registerRuleSetChain(new_chain);
                   ipt_comp->registerRuleSetChain(tgt_chain);
                   r->setStr("ipt_chain", new_chain);
                   r->setStr("ipt_target", tgt_chain);
                   tmp_queue.push_back(r);
                }
            }


            return true;
        }
        else 
        {
            compiler->abort(rule, "NAT branching rule misses branch rule set.");
            // in case we are in the test mode and abort() does not
            // really abort. Both the chain and the target are bogus
            // and are needed only to make the compiler continue and
            // produce some output, which will be shown to the user
            // together with the error in single-rule compile mode
            rule->setStr("ipt_chain", "PREROUTING");
            rule->setStr("ipt_target", "UNDEFINED");
            tmp_queue.push_back(rule);
        }
    } else
        tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::localNATRule::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

//    if ( rule->getStr("ipt_chain").empty())
//    {

    Address *osrc = compiler->getFirstOSrc(rule);
    bool osrcfw = compiler->complexMatch(osrc,compiler->fw);

    switch( rule->getRuleType())
    {
    case NATRule::DNAT:
    case NATRule::DNetnat:
    case NATRule::Redirect:
/* it should not be necessary to do anything if rule type is NONAT
 * since splitNONATRule takes care of NONAT rules
 *
 * is there any need to split the rule if it is SNAT or DNAT type ?  I
 * can't see any reason to do it.
 *
 * Can use OUTPUT chain only for DNAT rules and a like
 */
        if (osrcfw) rule->setStr("ipt_chain", "OUTPUT");
        if (osrcfw && osrc->getId()==compiler->fw->getId())
        {
            RuleElementOSrc *src;
            src=rule->getOSrc();
            src->clearChildren();
            src->setAnyElement();
        }
        break;
    default:
        break;
    }
//    }

    tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipt::splitIfOSrcAny::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

/* do not split if user nailed inbound interface */
    RuleElement *itf_re = rule->getItfInb();
    assert(itf_re!=nullptr);
    if (! itf_re->isAny()) return true;

/* do not split rules added to handle negation, these rules have "any"
 * in OSrc but get control only after OSrc is tested by another
 * rule */
    if (rule->getBool("rule_added_for_osrc_neg")) return true;
    if (rule->getBool("rule_added_for_odst_neg")) return true;
    if (rule->getBool("rule_added_for_osrv_neg")) return true;

    if (rule->getRuleType()==NATRule::DNAT)
    {
        RuleElementOSrc *osrcrel = rule->getOSrc();
        Address *osrc = compiler->getFirstOSrc(rule); 

        // split if osrc is any OR if it has a single object with negation
        if (osrc->isAny() || osrcrel->getBool("single_object_negation"))
        {
            NATRule *r = compiler->dbcopy->createNATRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            RuleElementOSrc *nosrcrel = r->getOSrc();
            nosrcrel->addRef(compiler->fw);
            tmp_queue.push_back(r);
        }
    }

    return true;
}


/*
 * we assume that splitIfOSrcMatchesFw was called before, so that if firewall
 * was in OSrc, it is now a single object in that rule element
 */
bool NATCompiler_ipt::DNATforFW::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    if (rule->getRuleType()==NATRule::DNAT)
    {
        Address *osrc=compiler->getFirstOSrc(rule);
        if ( compiler->complexMatch(osrc,compiler->fw) ) 
        {
            rule->setStr("ipt_chain","OUTPUT");  
            if (osrc->getId()==compiler->fw->getId())  
            {
                rule->getOSrc()->clearChildren();
                rule->getOSrc()->setAnyElement();
            }
        }
    }

    return true;
}


bool NATCompiler_ipt::decideOnChain::processNext()
{
    NATCompiler_ipt *ipt_comp = dynamic_cast<NATCompiler_ipt*>(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    string chain;
    switch (rule->getRuleType())
    {
    case NATRule::SNAT:     chain = "POSTROUTING"; break;
    case NATRule::SNetnat:  chain = "POSTROUTING"; break;
    case NATRule::Masq:     chain = "POSTROUTING"; break;
    case NATRule::DNAT:     chain = "PREROUTING";  break;
    case NATRule::DNetnat:  chain = "PREROUTING";  break;
    case NATRule::Redirect: chain = "PREROUTING";  break;
    case NATRule::NONAT:    
// processor splitNONATRule took care of NONAT rule
        break;
    case NATRule::NATBranch:
// processor splitNATBranchRule took care of NATBranch rule
        break;
    default: ;
    }

    if (!rule->getStr("ipt_chain").empty())
    {
        if (!compiler->getSourceRuleSet()->isTop() &&
            ipt_comp->getRuleSetName() == rule->getStr("ipt_chain"))
        {
            // this is a NAT branch. Need to rename the chain to add
            // information about the chain that would have been used
            // if this was top ruleset
            string new_chain = compiler->getRuleSetName() + "_" + chain;
            ipt_comp->registerRuleSetChain(new_chain);
            rule->setStr("ipt_chain", new_chain);
        }
        return true; // already defined
    }

    if (!chain.empty()) rule->setStr("ipt_chain", chain);
    return true;
}


bool NATCompiler_ipt::decideOnTarget::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    if ( ! rule->getStr("ipt_target").empty() ) return true; // already defined

    switch (rule->getRuleType())
    {
    case NATRule::NONAT:    rule->setStr("ipt_target","ACCEPT");   break;
    case NATRule::SNAT:     rule->setStr("ipt_target","SNAT");     break;
    case NATRule::SNetnat:  rule->setStr("ipt_target","NETMAP");   break;
    case NATRule::DNAT:     rule->setStr("ipt_target","DNAT");     break;
    case NATRule::DNetnat:  rule->setStr("ipt_target","NETMAP");   break;
    case NATRule::Masq:     rule->setStr("ipt_target","MASQUERADE"); break;
    case NATRule::Redirect: rule->setStr("ipt_target","REDIRECT"); break;
    case NATRule::Return:   rule->setStr("ipt_target","RETURN");   break;
    case NATRule::NATBranch:
        // this case has been taken care for in splitNATBranchRule()
        break;
    default: ;
    }
    return true;
}

/*
 * This rule processor chooses interfaces for the rule automatically
 * if user did not set them manually in "Inbound Interface" and
 * "Outbound Interface" columns.
 *
 * Automatic algorithm:
 *
 * this processor works together with ReplaceFirewallObjectsTSrc and
 * ConvertToAtomicRules. If the first two left interface object in
 * TSrc, AssignInterfaces assigns this rule to the corresponding
 * interface. Rule will be split and assigned to all interfaces here
 * if object in TSrc is not an interface or an address of interface.
 *
 * Summary: SNAT rules are now assigned to interfaces (using "-o
 * iface_name") as follows:
 *
 * - if firewall's interface or its address is in TSrc, the rule the
 * uses its address for "--to-source" and its name for "-o"
 *
 * - if firewall object is in TSrc, then it gets replaced with its
 * interfaces (except unnumbered and loopback interfaces) and rule is
 * processed using each interface as described above
 *
 * - if some other object is in TSrc, the rule is assigned to all
 * interfaces of the firewall (using notation with '+') and address of
 * this object is used for "--to-source". There are reasons why rule
 * has to be explicitly assigned to all interfaces using "-o" as
 * opposed to skipping "-o" all together. consider for example a
 * configuration with an unnumbred tunnel interface (e.g. ipsec0) used
 * for "road varrior" connections where IP address on the other end of
 * the tunnel is unknown. We can not add a "no nat" rule because we do
 * not know address of the net on the other side of the tunnel, but
 * fortunately ipsec0 is skipped in the assignment of SNAT rule
 * because it is unnumbered, so the firewall won't translate packets
 * going through this interface.
 * 
 *
 * NOTE: this rule processor may place groups of interfaces in inbound
 * and outbound interface rule elements. Names of these groups were
 * specifically constructed to match "wildcard" interface
 * specifications supported by iptables, such as "eth+". Do not call
 * rule processors that expand groups after AssignInterface.
 *
 */
bool NATCompiler_ipt::AssignInterface::processNext()
{
    NATCompiler_ipt *ipt_comp = dynamic_cast<NATCompiler_ipt*>(compiler);
    NATRule *rule = getNext(); if (rule==nullptr) return false;

    RuleElement *itf_re;

    itf_re = rule->getItfInb();
    assert(itf_re!=nullptr);

    if ( ! itf_re->isAny())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    itf_re = rule->getItfOutb();
    assert(itf_re!=nullptr);

    if ( ! itf_re->isAny())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    switch (rule->getRuleType())
    {
    case NATRule::SNAT:
    case NATRule::Masq:
    {
        Address* tsrc = compiler->getFirstTSrc(rule);
        Interface *iface = Interface::cast(tsrc);

        if (IPv4::isA(tsrc) || IPv6::isA(tsrc))
        {
            iface = Interface::cast(tsrc->getParent());
        }

        if (iface)
        {
            FWObject *parent_host = Host::getParentHost(iface);
            if (Cluster::isA(parent_host))
            {

                if (iface->isFailoverInterface())
                {
                    FailoverClusterGroup *failover_group =
                        FailoverClusterGroup::cast(
                            iface->getFirstByType(FailoverClusterGroup::TYPENAME));

                    Interface *fw_iface =
                        failover_group->getInterfaceForMemberFirewall(compiler->fw);

                    if (fw_iface)
                    {
                        // this is a bit tricky: we assign rule to the
                        // member firewall's inteface but TSrc remains
                        // cluster interface or its address.
                        iface = fw_iface;
                        RuleElementItfOutb *itf_re = rule->getItfOutb();
                        assert(itf_re!=nullptr);
                        if ( ! itf_re->hasRef(iface)) itf_re->addRef(iface);
                        tmp_queue.push_back(rule);
                        return true;
                    }
                } else
                {
                    // parent is the cluster but there is no failover
                    // group.  This must be a copy of the member interface.
                    RuleElementItfOutb *itf_re = rule->getItfOutb();
                    assert(itf_re!=nullptr);
                    if ( ! itf_re->hasRef(iface)) itf_re->addRef(iface);
                    tmp_queue.push_back(rule);
                    return true;
                }

            } else
            {
                if (iface->isChildOf(compiler->fw))
                {
                    RuleElementItfOutb *itf_re = rule->getItfOutb();
                    assert(itf_re!=nullptr);
                    if ( ! itf_re->hasRef(iface)) itf_re->addRef(iface);
                    tmp_queue.push_back(rule);
                    return true;
                }
            }
        }

#if 0
        FWObject *p = iface->getParent();
        qDebug() << "Checkpoint #1"
                 << " iface=" << iface << " " << iface->getName().c_str()
                 << " parent=" << p << " " << p->getName().c_str()
                 << " " << p->getTypeName().c_str();
#endif

/* if we appear here, then TSrc is not an interface or address of an
 * interface. This processor will simply pass a rule along if firewall
 * has no interfaces at all.  I wonder if I really have to do this,
 * but I do it anyway.
 */
        int n = 0;
        QMap<QString, libfwbuilder::FWObject*>::iterator it;
        for (it=ipt_comp->regular_interfaces.begin();
             it!=ipt_comp->regular_interfaces.end(); ++it)
        {
            FWObject *itf_group = it.value();
            // group "*" holds all interfaces
            if (itf_group->getName() == "*") continue;
            NATRule *r = compiler->dbcopy->createNATRule();
            r->duplicate(rule);
            compiler->temp_ruleset->add(r);
            RuleElementItfOutb *itf_re = r->getItfOutb();
            assert(itf_re!=nullptr);
            if ( ! itf_re->hasRef(itf_group)) itf_re->addRef(itf_group);
            //r->setInterfaceStr(intf_name.toStdString());
            tmp_queue.push_back(r);
            n++;
        }
        if (n==0) tmp_queue.push_back(rule);
        return true;
    }
    break;
    default: ;
    }

    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_ipt::verifyRuleWithMAC::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    RuleElementOSrc *rel = rule->getOSrc();
    if (rel->isAny())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    string chain = rule->getStr("ipt_chain");

    if (chain!="PREROUTING" && chain!="FORWARD" && chain!="INPUT" )
    {
/* scan all objects in OSrc, look for physAddress or combinedAddress
 * with pa present.  Objects like that are not allowed in chain POSTROUTING.
 * Issue warning and remove physAddress from the list.
 */
        list<FWObject*> cl;
        FWObject       *pa=nullptr;
        for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
        {
            FWObject *o= *i;
            FWObject *o1= o;
            if (FWReference::cast(o)!=nullptr) o1=FWReference::cast(o)->getPointer();

            if (physAddress::isA(o1))
            {
                pa=o1;
                cl.push_back(o1);
            }
            combinedAddress *ca=combinedAddress::cast(o1);
            if (ca!=nullptr &&  ca->getPhysAddress()!="" )
            {                
/*  there are two possibilities:
 * 1 - combinedAddress consists of the IPv4 component and MAC address component
 * 2 - combinedAddress consists of an empty IPv4 component and MAC address .
 */
                pa=o1;
                if ( ca->isAny() ) cl.push_back(o1);
                else ca->setPhysAddress("");
            }
        }

        if (!cl.empty()) 
        {
            for (list<FWObject*>::iterator i1=cl.begin(); i1!=cl.end(); ++i1)  
                rel->removeRef( (*i1) );
        }

        if (pa!=nullptr)
        {
            if (rel->isAny())
            {
                QString err(
                    "SNAT rule can not match MAC address, however "
                    "after removing object %1 from OSrc it becomes 'Any'");
                compiler->abort(rule,
                                err.arg(pa->getName().c_str()).toStdString());
                return true;
            }
            else
            {
                QString err(
                    "SNAT rule can not match MAC address. Object %1 "
                    "removed from the rule");
                compiler->warning(rule,
                                  err.arg(pa->getName().c_str()).toStdString());
            }
        }
    }

    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_ipt::processMultiAddressObjectsInRE::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    OSConfigurator_linux24 *osconf = 
        dynamic_cast<OSConfigurator_linux24*>(compiler->osconfigurator);

    RuleElement *re=RuleElement::cast( rule->getFirstByType(re_type) );

    if (re->size()==1) 
    {
        FWObject *o = re->front();
        if (FWReference::cast(o)!=nullptr) o = FWReference::cast(o)->getPointer();

        MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
        if (atrt!=nullptr)
        {
            // we have just one object in RE and this object is MutiAddressRunTime
            if (atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            {
                string path =
                    atrt->getSourceNameAsPath(compiler->getCachedFwOpt());
                if (path.empty() && !atrt->getSourceName().empty()) {
                    compiler->abort(rule, "Firewall's data directory not set for address table: " + atrt->getName());
                    return true;
                }
                rule->setStr("address_table_file", path);
                osconf->registerMultiAddressObject(atrt);
            }
            if (atrt->getSubstitutionTypeName()==DNSName::TYPENAME)
            {
                // this is DNSName converted to its run-time counterpart,
                // we do not need to touch it at all
            }
            tmp_queue.push_back(rule);
            return true;
        }
    }

    list<MultiAddressRunTime*> cl;
    for (FWObject::iterator i=re->begin(); i!=re->end(); i++)
    {
        FWObject *o= *i;
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
        MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
        if (atrt!=nullptr && atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
            cl.push_back(atrt);
    }

    if (!cl.empty())
    {
        RuleElement *nre;
        RuleElement *ore=RuleElement::cast( rule->getFirstByType(re_type) );
        NATRule *r;
        for (list<MultiAddressRunTime*>::iterator i=cl.begin(); i!=cl.end(); i++)
        {
            MultiAddressRunTime *atrt = *i;
            r = compiler->dbcopy->createNATRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            nre=RuleElement::cast( r->getFirstByType(re_type) );
            nre->clearChildren();
            nre->addRef( atrt );

            string path = atrt->getSourceNameAsPath(compiler->getCachedFwOpt());
            if (path.empty() && !atrt->getSourceName().empty()) {
                compiler->abort(rule, "Firewall's data directory not set for address table: " + atrt->getName());
                return true;
            }
            r->setStr("address_table_file", path);

            osconf->registerMultiAddressObject(atrt);
            tmp_queue.push_back(r);

            ore->removeRef( *i );
        }
    }

    tmp_queue.push_back(rule);
    return true;
}

bool NATCompiler_ipt::countChainUsage::processNext()
{
    NATCompiler_ipt *ipt_comp = dynamic_cast<NATCompiler_ipt*>(compiler);

    slurp();
    if (tmp_queue.size()==0) return false;

    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        NATRule *rule = NATRule::cast( *k );
        ipt_comp->chain_usage_counter[rule->getStr("ipt_target")] += 1;
    }

    return true;
}

void NATCompiler_ipt::registerRuleSetChain(const std::string &chain_name)
{
    chain_usage_counter[chain_name] = 1;
}

void NATCompiler_ipt::compile()
{
    string banner = " Compiling ruleset " + getRuleSetName() +
        " for 'nat' table";
    if (ipv6) banner += ", IPv6";
    info(banner);

    Compiler::compile();

    add( new NATCompiler::Begin());

    add( new printTotalNumberOfRules());

    add( new singleRuleFilter());
    
    add(new expandGroupsInItfInb("expand groups in inbound Interface"));
    add(new replaceClusterInterfaceInItfInb(
            "replace cluster interfaces with member interfaces in "
            "the inbound Interface rule element"));
    add(new singleObjectNegationItfInb("process single object negation in inbound Itf"));
    add(new ItfInbNegation("process negation in inbound Itf"));

    add(new expandGroupsInItfOutb("expand groups in outbound Interface"));
    add(new replaceClusterInterfaceInItfOutb(
            "replace cluster interfaces with member interfaces in "
            "the outbound Interface rule element"));
    add(new singleObjectNegationItfOutb("process single object negation in outbound Itf"));
    add(new ItfOutbNegation("process negation in outbound Itf"));

    add( new recursiveGroupsInOSrc("check for recursive groups in OSRC"));
    add( new recursiveGroupsInODst("check for recursive groups in ODST"));
    add( new recursiveGroupsInOSrv("check for recursive groups in OSRV"));

    add( new recursiveGroupsInTSrc("check for recursive groups in TSRC"));
    add( new recursiveGroupsInTDst("check for recursive groups in TDST"));
    add( new recursiveGroupsInTSrv("check for recursive groups in TSRV"));

    add( new emptyGroupsInOSrc("check for empty groups in OSRC" ));
    add( new emptyGroupsInODst("check for empty groups in ODST" ));
    add( new emptyGroupsInOSrv("check for empty groups in OSRV" ));

    add( new emptyGroupsInTSrc("check for empty groups in TSRC" ));
    add( new emptyGroupsInTDst("check for empty groups in TDST" ));
    add( new emptyGroupsInTSrv("check for empty groups in TSRV"));

    add( new ExpandGroups("Expand groups"));

    // processors that expand objects with multiple addresses
    // check addresses against current address family using member
    // ipv6. If all addresses do not match, we may end up with
    // empty rule element.
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    if (ipv6)
        add( new DropIPv4Rules("drop ipv4 rules"));
    else
        add( new DropIPv6Rules("drop ipv6 rules"));

    add( new eliminateDuplicatesInOSRC("eliminate duplicates in OSRC"));
    add( new eliminateDuplicatesInODST("eliminate duplicates in ODST"));
    add( new eliminateDuplicatesInOSRV("eliminate duplicates in OSRV"));

    add( new swapMultiAddressObjectsInOSrc(
             " swap MultiAddress -> MultiAddressRunTime in OSrc") );
    add( new swapMultiAddressObjectsInODst(
             " swap MultiAddress -> MultiAddressRunTime in ODst") );
    add( new swapMultiAddressObjectsInTSrc(
             " swap MultiAddress -> MultiAddressRunTime in TSrc") );
    add( new swapMultiAddressObjectsInTDst(
             " swap MultiAddress -> MultiAddressRunTime in TDst") );

    add( new processMultiAddressObjectsInOSrc(
             "process MultiAddress objects in OSrc") );
    add( new processMultiAddressObjectsInODst(
             "process MultiAddress objects in ODst") );
    add( new processMultiAddressObjectsInTSrc(
             "process MultiAddress objects in TSrc") );
    add( new processMultiAddressObjectsInTDst(
             "process MultiAddress objects in TDst") );

    add( new doOSrvNegation( "process negation in OSrv" ));

    add( new convertToAtomicportForOSrv("convert to atomic rules in OSrv"));

    add( new classifyNATRule( "classify NAT rule" ));
    add( new splitSDNATRule( "split SDNAT rules" ));
    add( new classifyNATRule( "reclassify rules" ));
    add( new ConvertLoadBalancingRules( "convert load balancing rules"));
    add( new VerifyRules( "verify rules" ));

    add( new singleObjectNegationOSrc(
             "negation in OSrc if it holds single object"));
    add( new singleObjectNegationODst(
             "negation in ODst if it holds single object"));

    add( new doOSrcNegation( "process negation in OSrc" ));
    add( new doODstNegation( "process negation in ODst" ));

/* call splitOnODst after processing negation */
    add( new splitOnODst( "split on ODst" ));

    add( new portTranslationRules( "port translation rules" ));
    add( new specialCaseWithRedirect(
             "special case with redirecting port translation rules" ) );

    if (fwopt->getBool("local_nat") )
    {
        if ( fwopt->getBool("firewall_is_part_of_any_and_networks") )
            add( new splitIfOSrcAny( "split rule if OSrc is any" ));

        add( new splitIfOSrcMatchesFw("split rule if OSrc matches FW" ));
    }

    add( new splitNONATRule("NAT rules that request no translation"));
    add( new splitNATBranchRule("Split Branch rules to use all chains"));
    add( new localNATRule("process local NAT rules"));
// add( new DNATforFW("process DNAT rules for packets originated on the firewall"));
    add( new decideOnChain( "decide on chain" ) );
    add( new decideOnTarget( "decide on target" ) );


// ----------- 10/18/2008
    add( new splitODstForSNAT(
             "split rule if objects in ODst belong to different subnets") );
    add( new ReplaceFirewallObjectsODst("replace firewall in ODst" ) );
    add( new ReplaceFirewallObjectsTSrc("replace firewall in TSrc" ) );
    add( new splitOnDynamicInterfaceInODst(
             "split rule if ODst is dynamic interface" ) );
    add( new splitOnDynamicInterfaceInTSrc(
             "split rule if TSrc is dynamic interface" ) );

    add( new ExpandMultipleAddresses("expand multiple addresses") );
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));


    if (ipv6)
        add( new DropIPv4Rules("drop ipv4 rules"));
    else
        add( new DropIPv6Rules("drop ipv6 rules"));

    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    add( new specialCaseWithUnnumberedInterface(
             "special cases with dynamic and unnumbered interfaces" ) );
    add( new checkForDynamicInterfacesOfOtherObjects(
             "dynamic interfaces of other hosts and firewalls" ) );

    add( new verifyRuleWithMAC("verify rules using MAC address filtering"));
    add( new ExpandAddressRanges("expand address ranges") );
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    add( new splitMultiSrcAndDst(
             "split rules where multiple srcs and dsts are present" ) );

    add( new groupServicesByProtocol("group services by protocol"));
    add( new VerifyRules2("check correctness of TSrv"));
    add( new separatePortRanges("separate port ranges"));

    add( new separateSrcPort("separate objects with src ports") );

    add( new separateSrcAndDstPort("separate objects with src and dest ports"));

    add( new prepareForMultiport("prepare for multiport") );
    add( new splitMultipleICMP("split rule with multiple ICMP services") );

    add( new ConvertToAtomicForAddresses("convert to atomic rules") );

    add( new addVirtualAddress("add virtual addresses") );

    add( new AssignInterface("assign rules to interfaces") );
    add( new VerifyRules3("check combination of interface spec and chain"));
    add( new dynamicInterfaceInODst("split if dynamic interface in ODst") );
    add( new dynamicInterfaceInTSrc(
             "set target if dynamic interface in TSrc" ) );
    add( new alwaysUseMasquerading(
             "always use masquerading target instead of SNAT" ) );

    add( new ConvertToAtomicForItfInb("convert to atomic for inbound interface") );
    add( new ConvertToAtomicForItfOutb("convert to atomic for outbound interface"));

    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));

    add( new countChainUsage("Count chain usage"));

    if (fwopt->getBool("use_iptables_restore"))
    {
        // bug #1812295: we should use PrintRuleIptRstEcho not only
        // when we have dynamic interfaces, but also when we have
        // address tables expanded at run time. Instead of checking
        // for all these conditions, just always use PrintRuleIptRstEcho
        printRule=new PrintRuleIptRstEcho(
            "generate code for iptables-restore using echo");
    } else
        printRule=new PrintRule("generate iptables shell script");

    printRule->setContext(this);
    printRule->initialize();
    add( printRule );

    add( new simplePrintProgress() );

    runRuleProcessors();
}


void NATCompiler_ipt::epilog()
{
    if (fwopt->getBool("use_iptables_restore") &&
        getCompiledScriptLength()>0 &&
        ! inSingleRuleCompileMode())
    {
        output << "#" << endl;
    }
}

string NATCompiler_ipt::flushAndSetDefaultPolicy()
{
    string res="";

    if (fwopt->getBool("use_iptables_restore") && ! inSingleRuleCompileMode())
    {
        res += "echo :PREROUTING ACCEPT [0:0]\n";
        res += "echo :POSTROUTING ACCEPT [0:0]\n";
        res += "echo :OUTPUT ACCEPT [0:0]\n";
    }

    return res;
}

string NATCompiler_ipt::printAutomaticRules()
{
    return "";
}

string NATCompiler_ipt::commit()
{
    string res="";

    if(printRule!=nullptr)
    {
        res += printRule->_commit();
    }
    return res;
}

list<string> NATCompiler_ipt::getUsedChains()
{
    list<string> res;
    for (map<string, int>::iterator it=chain_usage_counter.begin();
         it!=chain_usage_counter.end(); ++it)
        res.push_back(it->first);
    return res;
}

