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


#include "NATCompiler_ipf.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"

#include <QString>

#include <iostream>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string NATCompiler_ipf::myPlatformName() { return "ipf"; }


int NATCompiler_ipf::prolog()
{

    int n=NATCompiler_pf::prolog();

    return n;
}

bool NATCompiler_ipf::VerifyRules::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    RuleElementOSrc  *osrc=rule->getOSrc();  assert(osrc);
    RuleElementODst  *odst=rule->getODst();  assert(odst);
    RuleElementOSrv  *osrv=rule->getOSrv();  assert(osrv);

    RuleElementTSrc  *tsrc=rule->getTSrc();  assert(tsrc);
#ifndef NDEBUG
    RuleElementTDst  *tdst=rule->getTDst();  assert(tdst);
#endif
    RuleElementTSrv  *tsrv=rule->getTSrv();  assert(tsrv);

    if (rule->getRuleType()==NATRule::DNAT && odst->size()!=1)
	compiler->abort(rule,
                        "There should be no more than one object in "
                        "original destination");

//    if (rule->getRuleType()==NATRule::SNAT && tsrc->size()!=1)
//	compiler->abort(rule, "There should be no more than one object in translated source in the rule "+rule->getLabel());

    if (rule->getRuleType()==NATRule::DNAT && osrv->isAny()) 
	compiler->abort(rule,
                        "Service must be specified for destination translation rule");

    if (tsrv->size()!=1) 
	compiler->abort(rule,
                        "Translated service should be 'Original' or should "
                        "contain single object");

    FWObject *o=tsrv->front();
    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

    if ( Group::cast(o)!=nullptr)
	compiler->abort(rule,
                        "Can not use group in translated service");

    if (rule->getRuleType()==NATRule::SNetnat && !tsrc->isAny() )
    {
        Network *a1=Network::cast(compiler->getFirstOSrc(rule));
        Network *a2=Network::cast(compiler->getFirstTSrc(rule));
        if ( a1==nullptr || a2==nullptr ||
             a1->getNetmaskPtr()->getLength()!=a2->getNetmaskPtr()->getLength() )
            compiler->abort(rule,
                            "Original and translated source should both "
                            "be networks of the same size");
    }

    if (rule->getRuleType()==NATRule::DNetnat && !tsrc->isAny() )
    {
        Network *a1=Network::cast(compiler->getFirstODst(rule));
        Network *a2=Network::cast(compiler->getFirstTDst(rule));
        if ( a1==nullptr || a2==nullptr ||
             a1->getNetmaskPtr()->getLength()!=a2->getNetmaskPtr()->getLength() )
            compiler->abort(rule,
                            "Original and translated destination should "
                            "both be networks of the same size");
    }

    if (osrc->getNeg() || odst->getNeg() || osrv->getNeg())
        compiler->abort(rule, "Negation in NAT rules is not supported");

    return true;
}

bool NATCompiler_ipf::ExpandPortRange::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    Service          *osrv=compiler->getFirstOSrv(rule);

    if (UDPService::isA(osrv) || TCPService::isA(osrv))
    {
	int rs = TCPUDPService::cast(osrv)->getDstRangeStart();
	int re = TCPUDPService::cast(osrv)->getDstRangeEnd();

        int numPorts = re-rs+1;
        if (numPorts==1)
        {
            tmp_queue.push_back(rule);
            return true;
        }

        if (numPorts > 20)
        {
            ostringstream ostr;
            ostr << string("Expanding port range ") << osrv->getName()
                 << " creates " << numPorts << " rules";
            compiler->warning(rule, ostr.str());
        }

        string newSrvType = TCPService::TYPENAME;
        if (UDPService::isA(osrv)) newSrvType = UDPService::TYPENAME;

        for (int p=rs; p<=re; ++p)
        {
            NATRule *r = compiler->dbcopy->createNATRule();
            r->duplicate(rule);

            FWObject *newSrv = compiler->dbcopy->create(newSrvType);
            newSrv->duplicate(osrv,true);
            TCPUDPService::cast(newSrv)->setDstRangeStart(p);
            TCPUDPService::cast(newSrv)->setDstRangeEnd(p);
            compiler->persistent_objects->add(newSrv,false);
            compiler->dbcopy->addToIndex(newSrv);

            RuleElementOSrv *nosrv = r->getOSrv();
            nosrv->clearChildren();
            nosrv->addRef(newSrv);

            compiler->temp_ruleset->add(r);
            tmp_queue.push_back(r);
        }
    } else
    {
        tmp_queue.push_back(rule);
    }
    return true;
}

bool NATCompiler_ipf::AssignInterface::processNext()
{
    NATRule *rule = getNext(); if (rule==nullptr) return false;
    RuleElementItfOutb *itf_re = rule->getItfOutb();

    Address *a = nullptr;

    switch (rule->getRuleType() )
    {

    case NATRule::Continue:
    case NATRule::NONAT:
    {
/* use heuristic to assign nonat rule to interfaces */
        Interface *iface;

        a=compiler->getFirstODst(rule);
        iface=compiler->findInterfaceFor( compiler->getFirstODst(rule) , 
                                          compiler->fw);
        if (iface!=nullptr && !iface->isLoopback()) 
        { 
            if ( ! itf_re->hasRef(iface)) itf_re->addRef(iface);
            // rule->setInterfaceId( iface->getId() );
            tmp_queue.push_back( rule );
            return true;
        }
/* slip into Redirect case to assign rule to all interfaces */
    }
    /* FALLTHRU */
    case NATRule::Redirect: 
    case NATRule::DNAT:
    case NATRule::DNetnat:
    case NATRule::LB:
    {
/*
 * we do not have network zones here, so our ability to pick right
 * interfaces is rather limited. First, we try to find interface that
 * is connected to the subnet OSrc belongs to. If that does not work,
 * we assign rule to all interfaces, except loopback 
 */
        a = nullptr;
        if ( ! rule->getOSrc()->isAny() )  a = compiler->getFirstOSrc(rule);
        if ( a==nullptr && ! rule->getODst()->isAny() )
            a = compiler->getFirstODst(rule);

        if (a!=nullptr)
        {
            Interface *iface;
            iface = compiler->findInterfaceFor(a,compiler->fw);

            if (iface!=nullptr && !iface->isLoopback()) 
            { 
                if ( ! itf_re->hasRef(iface)) itf_re->addRef(iface);
                // rule->setInterfaceId( iface->getId() );
                tmp_queue.push_back(rule);
                return true;
            }
        }

        FWObjectTypedChildIterator j=compiler->fw->findByType(Interface::TYPENAME);
        for ( ; j!=j.end(); ++j ) 
        {
            Interface *iface = Interface::cast(*j);
            assert(iface);
            if ( iface->isUnnumbered() || 
                 iface->isBridgePort() || 
                 iface->isLoopback()) continue;

            NATRule *r = compiler->dbcopy->createNATRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            RuleElementItfOutb *itf_re = r->getItfOutb();
            if ( ! itf_re->hasRef(iface)) itf_re->addRef(iface);
            //r->setInterfaceId( iface->getId() );
            tmp_queue.push_back( r );
        }
        return true;
    }

    case NATRule::SNAT:
    case NATRule::SNetnat:
    {
        a=compiler->getFirstTSrc(rule);

        if ( (Interface::isA(a) || IPv4::isA(a)) && a->isChildOf(compiler->fw))
        {
            FWObject *p = a;
            while ( ! Interface::isA(p) ) p = p->getParent();
            if ( ! itf_re->hasRef(p)) itf_re->addRef(p);
            // rule->setInterfaceId( p->getId() );
            tmp_queue.push_back(rule);
            return true;
        }

/* if we appear here, then TSrc is not an interface or address of an
 * interface. 
 */
        int n=0;
        list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
        for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
        {
            Interface *iface=Interface::cast(*i);
            assert(iface);

            if (iface->isLoopback() ||
                iface->isUnnumbered() ||
                iface->isBridgePort()) continue;

            NATRule *r = compiler->dbcopy->createNATRule();
            r->duplicate(rule);
            compiler->temp_ruleset->add(r);

            RuleElementItfOutb *itf_re = r->getItfOutb();
            if ( ! itf_re->hasRef(iface)) itf_re->addRef(iface);
            // r->setInterfaceId( iface->getId() );
            
            tmp_queue.push_back(r);
            n++;
        }
        if (n==0) tmp_queue.push_back(rule);

        return true;
    }
    default: ;
    }

    compiler->abort(rule,
                    "Could not assign NAT rule to the interface. "
                    "Perhaps one of the objects has address which does not "
                    "belong to any subnet the firewall has interface on");
    
    return true;
}


bool NATCompiler_ipf::prepareForLB::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getRuleType()==NATRule::LB ) 
    {
        RuleElementTDst  *tdst=rule->getTDst();  assert(tdst);
        if (tdst->size()>2)
        {
            std::vector<FWObject*> cl;
            for(list<FWObject*>::iterator i=tdst->begin(); i!=tdst->end(); ++i) 
            {
                FWObject *o= *i;
                if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

                cl.push_back(o);
                if (cl.size()==2)
                {
                    NATRule *r= compiler->dbcopy->createNATRule();
                    compiler->temp_ruleset->add(r);
                    r->duplicate(rule);
                    RuleElementTDst *notdst=r->getTDst();
                    notdst->clearChildren();
                    notdst->addRef( cl[0] );
                    notdst->addRef( cl[1] );
                    cl.clear();
                    tmp_queue.push_back( r );
                }
            }

            if (cl.size()!=0)
            {
                NATRule *r= compiler->dbcopy->createNATRule();
                compiler->temp_ruleset->add(r);
                r->duplicate(rule);
                RuleElementTDst *notdst=r->getTDst();
                notdst->clearChildren();
                notdst->addRef( cl[0] );
//                notdst->addRef( cl[1] );
                cl.clear();
                tmp_queue.push_back( r );
            }

        } else  tmp_queue.push_back(rule);
    } else  tmp_queue.push_back(rule);

    return true;
}

/*
 * by now the rule should have already been assigned to interface. 
 *
 * TODO: We should also take into account a situation when interface has
 * multiple addresses...
 */
bool NATCompiler_ipf::RedirectRules::processNext()
{
    NATRule   *rule = getNext(); if (rule==nullptr) return false;
    RuleElementItfOutb *itf_re = rule->getItfOutb();
    Interface *rule_iface =
        Interface::cast(FWObjectReference::getObject(itf_re->front()));
    //  Interface::cast( rule->getRoot()->getById(rule->getInterfaceId() ,true) );

    tmp_queue.push_back(rule);

    RuleElementTDst *rel=rule->getTDst();          assert(rel);
    Address         *otdst=compiler->getFirstTDst(rule);

    if (rule->getRuleType()==NATRule::Redirect && rule_iface!=nullptr &&
        otdst->getId()==compiler->fw->getId())
    {
        rel->clearChildren();
        rel->addRef( rule_iface );
    }

    return true;
}


bool NATCompiler_ipf::appProxy::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    bool ftp_proxy    = compiler->getCachedFwOpt()->getBool("ipf_nat_ftp_proxy");
    bool rcmd_proxy   = compiler->getCachedFwOpt()->getBool("ipf_nat_rcmd_proxy");
    bool krcmd_proxy  = compiler->getCachedFwOpt()->getBool("ipf_nat_krcmd_proxy");
    bool ekshell_proxy  = compiler->getCachedFwOpt()->getBool("ipf_nat_ekshell_proxy");
    bool raudio_proxy = compiler->getCachedFwOpt()->getBool("ipf_nat_raudio_proxy");
    bool h323_proxy   = compiler->getCachedFwOpt()->getBool("ipf_nat_h323_proxy");
    bool ipsec_proxy  = compiler->getCachedFwOpt()->getBool("ipf_nat_ipsec_proxy");
    bool pptp_proxy  = compiler->getCachedFwOpt()->getBool("ipf_nat_pptp_proxy");
    bool irc_proxy  = compiler->getCachedFwOpt()->getBool("ipf_nat_irc_proxy");

    QString ipsec_proxy_str = QString("proxy port %1 ipsec/udp ").arg(ISAKMP_PORT);
    QString ftp_proxy_str = QString("proxy port %1 ftp/tcp ").arg(FTP_PORT);
    QString rcmd_proxy_str = QString("proxy port %1 rcmd/tcp ").arg(RCMD_PORT);
    QString krcmd_proxy_str = QString("proxy port %1 rcmd/tcp ").arg(KRCMD_PORT);
    QString ekshell_proxy_str = QString("proxy port %1 rcmd/tcp ").arg(EKSHELL_PORT);
    QString raudio_proxy_str = QString("proxy port %1 raudio/tcp ").arg(RAUDIO_PORT);
    QString h323_proxy_str = QString("proxy port %1 h323/tcp ").arg(H323_PORT);
    QString pptp_proxy_str = QString("proxy port %1 pptp/tcp ").arg(PPTP_PORT);
    QString irc_proxy_str = QString("proxy port %1 irc/tcp ").arg(IRC_PORT);


    if (rule->getRuleType()==NATRule::SNAT || 
        rule->getRuleType()==NATRule::NONAT) 
    {
        Service *osrv = compiler->getFirstOSrv(rule);

        if (UDPService::isA(osrv))
        {
            UDPService *s=UDPService::cast(osrv);
            if (ipsec_proxy && 
                s->getDstRangeStart()==ISAKMP_PORT && s->getDstRangeEnd()==ISAKMP_PORT)  
                rule->setStr("nat_rule_proxy", ipsec_proxy_str.toStdString());
        }

        if (TCPService::isA(osrv))
        {
            TCPService *s = TCPService::cast(osrv);
            int range_start = s->getDstRangeStart();
            int range_end = s->getDstRangeEnd();
            if (ftp_proxy && range_start==FTP_PORT && range_end==FTP_PORT)
                rule->setStr("nat_rule_proxy", ftp_proxy_str.toStdString());
            
            if (rcmd_proxy && range_start==RCMD_PORT && range_end==RCMD_PORT)
                rule->setStr("nat_rule_proxy", rcmd_proxy_str.toStdString());

            if (krcmd_proxy && range_start==KRCMD_PORT && range_end==KRCMD_PORT )   
                rule->setStr("nat_rule_proxy", krcmd_proxy_str.toStdString());

            if (ekshell_proxy && range_start==EKSHELL_PORT && range_end==EKSHELL_PORT )   
                rule->setStr("nat_rule_proxy", ekshell_proxy_str.toStdString());

            if (raudio_proxy && range_start==RAUDIO_PORT && range_end==RAUDIO_PORT ) 
                rule->setStr("nat_rule_proxy", raudio_proxy_str.toStdString());

            if (h323_proxy && range_start==H323_PORT && range_end==H323_PORT )   
                rule->setStr("nat_rule_proxy", h323_proxy_str.toStdString());

            if (pptp_proxy && range_start==PPTP_PORT && range_end==PPTP_PORT )   
                rule->setStr("nat_rule_proxy", pptp_proxy_str.toStdString());

            if (irc_proxy && range_start==IRC_PORT && range_end==IRC_PORT )   
                rule->setStr("nat_rule_proxy", irc_proxy_str.toStdString());
        }
    }
    tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipf::expandAnyService::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElementOSrv *srv=rule->getOSrv();

    if (rule->getRuleType()==NATRule::SNAT && srv->isAny())
    {
	NATRule *r= compiler->dbcopy->createNATRule();
	compiler->temp_ruleset->add(r);
	r->duplicate(rule);
        r->setBool("needs_portmap",true);
	tmp_queue.push_back(r);
    }
    tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_ipf::processMultiAddressObjectsInRE::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;
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


void NATCompiler_ipf::compile()
{
    bool manage_virtual_addr=getCachedFwOpt()->getBool("manage_virtual_addr");

    info(" Compiling NAT rules for " + fw->getName());

    Compiler::compile();

    add( new Begin());
    add( new printTotalNumberOfRules() );

    add( new singleRuleFilter());

    add( new recursiveGroupsInOSrc(      "check for recursive groups in OSRC"     ) );
    add( new recursiveGroupsInODst(      "check for recursive groups in ODST"     ) );
    add( new recursiveGroupsInOSrv(      "check for recursive groups in OSRV"     ) );

    add( new recursiveGroupsInTSrc(      "check for recursive groups in TSRC"     ) );
    add( new recursiveGroupsInTDst(      "check for recursive groups in TDST"     ) );
    add( new recursiveGroupsInTSrv(      "check for recursive groups in TSRV"     ) );

    add( new emptyGroupsInOSrc(          "check for empty groups in OSRC"         ) );
    add( new emptyGroupsInODst(          "check for empty groups in ODST"         ) );
    add( new emptyGroupsInOSrv(          "check for empty groups in OSRV"         ) );

    add( new emptyGroupsInTSrc(          "check for empty groups in TSRC"         ) );
    add( new emptyGroupsInTDst(          "check for empty groups in TDST"         ) );
    add( new emptyGroupsInTSrv(          "check for empty groups in TSRV"         ) );

    add( new ExpandGroups( "expand groups" ) );
    add( new eliminateDuplicatesInOSRC(  "eliminate duplicates in OSRC"              ) );
    add( new eliminateDuplicatesInODST(  "eliminate duplicates in ODST"              ) );
    add( new eliminateDuplicatesInOSRV(  "eliminate duplicates in OSRV"              ) );

    add( new swapMultiAddressObjectsInOSrc(" swap MultiAddress -> MultiAddressRunTime in OSrc") );
    add( new swapMultiAddressObjectsInODst(" swap MultiAddress -> MultiAddressRunTime in ODst") );

    add( new processMultiAddressObjectsInOSrc("process MultiAddress objects in OSrc") );
    add( new processMultiAddressObjectsInODst("process MultiAddress objects in ODst") );

    add( new splitOnOSrv( "split rule on original service" ) );
    add( new ExpandPortRange("expand port ranges") );
    add( new fillTranslatedSrv( "fill translated service" ) );
    add( new NATRuleType( "determine NAT rule types" ) );
    add( new VerifyRules( "verify NAT rules" ) );
    add( new splitODstForSNAT("split rule if objects in ODst belong to different subnets"    ) );
    add( new ReplaceFirewallObjectsODst( "replace references to the firewall in ODst" ) );
    add( new ReplaceFirewallObjectsTSrc( "replace references to the firewall in TSrc" ) );

    if ( manage_virtual_addr ) {
        add( new addVirtualAddress( "add virtual addresses for NAT rules" ) );
    }

    add( new ExpandMultipleAddresses( "expand multiple addresses" ) );
    add( new checkForUnnumbered(    "check for unnumbered interfaces"          ) );
    add( new checkForDynamicInterfacesOfOtherObjects( "check for dynamic interfaces of other hosts and firewalls" ) );
    add( new ExpandAddressRanges( "expand address range objects" ) );
    add( new ConvertToAtomicForOriginal( "convert to atomic rules in OSrc and ODst" ) );
    add( new ConvertToAtomicForTSrc( "convert to atomic rules in TSrc" ) );
    add( new prepareForLB( "prepare for load balancing rules" ) );
    add( new appProxy( "add application proxy code to map rules" ) );
    add( new expandAnyService("split NAT rules with ANY service"           ) );
    add( new AssignInterface( "assign rules to interfaces" ) );
    add( new RedirectRules( "replace objects in TDst for 'Redirect' rules" ) );

    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));

    add( new PrintRule("generate ipfilter code") );
    add( new simplePrintProgress() );

    runRuleProcessors();


}


void NATCompiler_ipf::epilog()
{
}
