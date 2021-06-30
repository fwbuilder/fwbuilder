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


#include "NATCompiler_pix.h"
#include "NamedObjectsAndGroupsSupport.h"
#include "NamedObjectsManager.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/FailoverClusterGroup.h"

#include <assert.h>

#include <QString>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;



/*
 * this processor uses slurp to make sure all previous processors ran before
 * it starts scanning rules. 
 */
bool NATCompiler_pix::mergeNATCmd::processNext()
{
    NATCompiler_pix *pix_comp = dynamic_cast<NATCompiler_pix*>(compiler);

    slurp();
    if (tmp_queue.size()==0) return false;

    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        NATRule *rule = NATRule::cast( *k );

        if (rule->getRuleType() == NATRule::DNAT)
        {
            StaticCmd *scmd = pix_comp->static_commands[rule->getInt("sc_cmd")];

            map<int,StaticCmd*>::iterator i1;
            for (i1=pix_comp->static_commands.begin();
                 i1!=pix_comp->static_commands.end(); ++i1) 
            {
                StaticCmd *sc = (*i1).second;
                if (scmd==sc) break;

                if ( *scmd == *sc)
                {
/*
 * rule 'sc' is above rule 'scmd', we need to print 'static' command
 * only in the last rule using the same access list. That's why we set
 * flag ignore_scmd_and_print acl in sc and not in scmd
 */
                    scmd->acl_name = sc->acl_name;
                    sc->ignore_scmd_and_print_acl = true;
                }
            }
        }

	if (rule->getRuleType()==NATRule::SNAT)
        { 
            NATCmd *natcmd = pix_comp->nat_commands[ rule->getInt("nat_cmd") ];

            map<int,NATCmd*>::iterator i1;
            for (i1 = pix_comp->nat_commands.begin();
                 i1 != pix_comp->nat_commands.end(); ++i1) 
            {
                NATCmd *nc = (*i1).second;
/* since map nat_commands is sorted by the key, we only have to scan it
 * until we hit natcmd 
 */

                if (natcmd==nc) break;

                const InetAddr *a1 = natcmd->t_addr->getAddressPtr();
                const InetAddr *a2 = nc->t_addr->getAddressPtr();

                Interface *int1 = natcmd->o_iface;
                Interface *int2 = nc->o_iface;

                if ((natcmd->t_addr == nc->t_addr ||
                     (a1 && a2 && *a1 == *a2)) &&
                    int1->getId() == int2->getId() ) 
                {
                    natcmd->ignore_global = true;
                    natcmd->nat_id = nc->nat_id;
                }
            }

            for (map<int,NATCmd*>::iterator i1=pix_comp->nat_commands.begin();
                 i1!=pix_comp->nat_commands.end(); ++i1) 
            {
                NATCmd *nc = (*i1).second;
/* since map nat_commands is sorted by the key, we only have to scan it
 * until we hit natcmd 
 */
                if (natcmd == nc) break;
                if (nc->ignore_nat) continue;

/* using operator==(const Address &o1,const Address &o2) here */

                if ( *(natcmd->o_src) == *(nc->o_src) &&
                     *(natcmd->o_dst) == *(nc->o_dst) &&
                     *(natcmd->o_srv) == *(nc->o_srv) &&
                     natcmd->i_iface->getId() == nc->i_iface->getId() )
                {
/*
 * there is another nat rule (rule #2) with the same "original"
 * addresses and the same interface. We can drop this nat rule, but need
 * to merge its global pool with pool of the rule #2.
 *
 * This nat rule could have been sharing a global pool with some other
 * nat rule; in this case we need to find this other rule and also
 * reassign it to the global pool of the rule #2.
 */
                    natcmd->ignore_nat = true;
                    map<int,NATCmd*>::iterator i2;
                    for (i2 = pix_comp->nat_commands.begin();
                         i2 != pix_comp->nat_commands.end(); ++i2) 
                    {
                        NATCmd *nc2 = i2->second;
                        if (natcmd->nat_id == nc2->nat_id)
                            nc2->nat_id = nc->nat_id;
                    }
                    natcmd->nat_id = nc->nat_id;
                }
            }

            if (!natcmd->use_nat_0_0) 
            {
                map<int,NATCmd*>::iterator i1;
                for (i1 = pix_comp->nat_commands.begin();
                     i1 != pix_comp->nat_commands.end(); ++i1) 
                {
                    NATCmd *nc=(*i1).second;
/* since map nat_commands is sorted by the key, we only have to scan it
 * until we hit natcmd 
 */
                    if (natcmd==nc) break;

/* ignore nat natcmd entries for rules where we won't print 'nat'
 * command or use 'nat 0' command since this means we won't print
 * access-list for those rules and hense can not merge lists
 */
                    if (nc->ignore_nat) continue;
                    if (nc->use_nat_0_0) continue;

                    if ( natcmd->nat_id == nc->nat_id &&
                         natcmd->t_addr == nc->t_addr &&
                         natcmd->i_iface->getId() == nc->i_iface->getId() )
                    {
/* two nat commands with the same id, the same interface and the same
 * translated address, but different osrc and odst. OSrc and ODst must
 * be different, otherwise these two commands would have been merged
 * in the previous cycle.  We can merge access lists and drop one of
 * these nat commands. We merge ACLs by assigning them the same name.
 */
                        natcmd->nat_acl_name = nc->nat_acl_name;
                        nc->ignore_nat_and_print_acl = true;
                    }
                }
            }
	}

    }
    return true;
}

/*
 * The goal of this processor is to find SNAT rules that could be
 * translated as "nat (interface) 0.0.0.0 0.0.0.0. These rules should
 * have the same network object in OSrc that is used to define
 * interface's network zone. The logic is simple: if network "A" is a
 * network zone for internal interface, then only packets from this
 * network can hit it and therefore there is no need to check source
 * address once more in the "nat" rule. 
 *
 * We also check for ODst and OSrv, because if the destination or the
 * service are defined, then this optimization can not be done.
 *
 * This optimization can be turned off using checkbutton in the
 * "Firewall" tab.
 *
 * call this processor really early, when groups have not been
 * expanded yet.  At this point both NAT rule type and interfaces it
 * is associated with are unknown yet. We have to partially repeat
 * algorithms used in other rule processors to determine NAT rule type
 * and interface.
 *
 * We do this optimization in two steps:
 *
 * 1. in this rule processor we replace object in OSrc with firewall's
 * interface. This way we can still use other rule processors that
 * determine rule type and assign it to interfaces, but rule won't be
 * split onto multiple rules because of objects in OSrc. We also set
 * boolean flags "clear_osrc" and "use_nat_0_0" on the rule.
 *
 * 2. further down in rule processor clearOSrc we check the flag and
 * clear OSrc if it is set.
 *
 * 3. flag "use_nat_0_0" is used in printRule processor.
 */
bool  NATCompiler_pix::optimizeDefaultNAT::processNext()
{
//    NATCompiler_pix *pix_comp=dynamic_cast<NATCompiler_pix*>(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    RuleElementOSrc  *osrc=rule->getOSrc();
    RuleElementOSrv  *osrv=rule->getOSrv();
    RuleElementODst  *odst=rule->getODst();
    RuleElementTSrc  *tsrc=rule->getTSrc();
    RuleElementTDst  *tdst=rule->getTDst();

    if (osrc->size()>1) return true;
    if (osrc->isAny())  return true;
    if (!osrv->isAny()) return true;
    if (!odst->isAny()) return true;

/*
 * can't use RuleElementOSrc::getFirst(bool dereference) because it
 * returns Address::cast(o), but child element of rule element may be
 * a group when this processor is called.
 */    
    FWObject *o=osrc->front();
    string osrc_id;
    if (FWReference::cast(o)!=nullptr)
        osrc_id = FWObjectDatabase::getStringId(FWReference::cast(o)->getPointerId());
    else
        osrc_id = FWObjectDatabase::getStringId(o->getId());

    if ( ( !tsrc->isAny() && tdst->isAny()) ||
         ( !osrc->isAny() && odst->isAny() && tsrc->isAny() && tdst->isAny() )
    )
    {
//  this rule type is SNAT or NONAT

        list<FWObject*> l2=compiler->fw->getByType(Interface::TYPENAME);
        for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
        {
            Interface *iface=Interface::cast(*i);

            if (iface->getStr("orig_netzone_id")==osrc_id )
            {
                rule->setBool("clear_osrc",true);
                rule->setBool("use_nat_0_0",true);
                osrc->clearChildren();
                osrc->addRef(iface);
                break;
            }
        }
    }

    return true;
}

bool  NATCompiler_pix::SuppressDuplicateNONATStatics::processNext()
{
    Helper helper(compiler);
//    NATCompiler_pix *pix_comp=dynamic_cast<NATCompiler_pix*>(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getRuleType()== NATRule::NONAT && 
        rule->getInt("nonat_type")==NONAT_STATIC)
    {
	Address  *osrc=compiler->getFirstOSrc(rule);  assert(osrc);
	Address  *odst=compiler->getFirstODst(rule);  assert(odst);

        nonat_static_parameters  sp;
        sp.iface1 = helper.findInterfaceByNetzone(osrc );
        sp.iface2 = helper.findInterfaceByNetzone(odst );
        sp.addr = *(odst->getAddressPtr());
        sp.mask = *(odst->getNetmaskPtr());

        for (deque<nonat_static_parameters>::iterator i=all_nonat_statics.begin(); 
             i!=all_nonat_statics.end();  ++i )
        {
            if ( i->iface1==sp.iface1 &&
                 i->iface2==sp.iface2 &&
                 i->addr==sp.addr &&
                 i->mask==sp.mask ) return true;
        }
        all_nonat_statics.push_back(sp);
    }

    tmp_queue.push_back(rule);
    return true;
}

NATCompiler_pix::DetectOverlap::~DetectOverlap() {};

bool NATCompiler_pix::DetectOverlap::checkOverlapping(
    const libfwbuilder::Address  &addr1,
    const libfwbuilder::InetAddr &addr2)
{
    if (AddressRange::isA(&addr1))
    {
        const InetAddr a1 = AddressRange::constcast(&addr1)->getRangeStart();
        const InetAddr a2 = AddressRange::constcast(&addr1)->getRangeEnd();
        return (addr2==a1 || addr2==a2 || (addr2>a1 && addr2<a2));
    } else
    {
        return *(addr1.getAddressPtr()) == addr2 || addr1.belongs(addr2);
    }
}

string NATCompiler_pix::DetectOverlap::printGlobalPoolAddress(const Address &pool)
{
    if (AddressRange::isA(&pool))
    {
        const InetAddr a1=AddressRange::constcast(&pool)->getRangeStart();
        const InetAddr a2=AddressRange::constcast(&pool)->getRangeEnd();
        return a1.toString()+"-"+a2.toString();
    } else
    {
        return pool.getAddressPtr()->toString() + "/" +
            pool.getNetmaskPtr()->toString();
    }
}

bool  NATCompiler_pix::DetectGlobalPoolProblems::processNext()
{
    NATCompiler_pix *pix_comp=dynamic_cast<NATCompiler_pix*>(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    if (rule->getRuleType()== NATRule::SNAT )
    {
        NATCmd *natcmd = pix_comp->nat_commands[ rule->getInt("nat_cmd") ];

        if (natcmd->ignore_global) return true;

        if (natcmd->type != INTERFACE)
        {
            if (checkOverlapping(*(natcmd->t_addr),
                                 *(natcmd->o_iface->getAddressPtr())))
                compiler->abort(
                    rule, 
                    "Global pool "
                    + printGlobalPoolAddress(*(natcmd->t_addr)) 
                    + " overlaps with interface address.");

            if (checkOverlapping(*(natcmd->t_addr),
                                 *(natcmd->o_iface->getBroadcastAddressPtr()))
                ||
                checkOverlapping(*(natcmd->t_addr),
                                 *(natcmd->o_iface->getAddressPtr())) )
                compiler->warning(
                    rule, 
                    "Global pool "
                    + printGlobalPoolAddress(*(natcmd->t_addr)) 
                    + " overlaps with broadcast address.");
        }

        for (map<int,NATCmd*>::iterator i1=pix_comp->nat_commands.begin();
             i1!=pix_comp->nat_commands.end(); ++i1) 
        {
            NATCmd *nc = (*i1).second;
/* since map nat_commands is sorted by the key, we only have to scan it
 * until we hit natcmd 
 */
            if (nc->ignore_global) continue;
            if (natcmd==nc) break;

            Interface *int1 = natcmd->o_iface;
            Interface *int2 = nc->o_iface;

            if ( int1->getId()==int2->getId() )
            {
                if ( ! fwcompiler::_find_obj_intersection(natcmd->t_addr,nc->t_addr).empty() )
                {
                    compiler->abort(
                        rule, 
                        string("Global pool overlap: ")
                        + rule->getLabel() + " : "
                        + printGlobalPoolAddress(*(natcmd->t_addr)) 
                        + nc->rule_label + " : "
                        + printGlobalPoolAddress(*(nc->t_addr)) );
                }
            }

        }
    }
    return true;
}


bool  NATCompiler_pix::DetectOverlappingGlobalPoolsAndStaticRules::processNext()
{
    NATCompiler_pix *pix_comp=dynamic_cast<NATCompiler_pix*>(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    if (rule->getRuleType()== NATRule::DNAT )
    {
        Address  *outa=compiler->getFirstODst(rule);  assert(outa);
#ifndef NDEBUG
        Address  *insa=compiler->getFirstTDst(rule);  assert(insa);
#endif

        for (map<int,NATCmd*>::iterator i=pix_comp->nat_commands.begin(); 
             i!=pix_comp->nat_commands.end(); ++i)
        {
            NATCmd *natcmd=(*i).second;

            if (natcmd->ignore_global) return true;

            /* in this case natcmd->t_addr is interface. Interface creates
             * single-address global pool, but since it has netmask,
             * method checkOverlapping would treat it as network. I create
             * temporary substitution Address object to avoid this .
             *
             * If interface is used for a global pool (SNAT rule) and
             * for a static (DNAT rule), then this is ok even though
             * such global pool overlaps with such static (added 10/17/03)
             *
             * But first I need to check if this interface has dynamic
             * address, in which case I can not really do this check
             * at all.
             */
            IPv4 addr;

            Interface *iface=Interface::cast(natcmd->t_addr);
            if (iface!=nullptr && iface->isDyn()) return true;

            if (iface!=nullptr && iface->getId()==outa->getId()) return true;

            addr.setAddress(*(natcmd->t_addr->getAddressPtr()));
            addr.setNetmask(*(natcmd->t_addr->getNetmaskPtr()));

            if (natcmd->type== INTERFACE) 
            {
                addr.setNetmask(InetAddr(InetAddr::getAllOnes()));
            }

            if ( checkOverlapping(  addr, *(outa->getAddressPtr())) ||
                 checkOverlapping( *outa, *(addr.getAddressPtr())) )
                compiler->abort(
                    
                        rule, 
                        "Global pool "
                        +printGlobalPoolAddress(addr)
                        +" from rule "
                        +natcmd->rule_label
                        +" overlaps with static translation address in rule "
                        +rule->getLabel());
        }
    }
    return true;
}

bool  NATCompiler_pix::DetectDuplicateNAT::processNext()
{
    NATCompiler_pix *pix_comp=dynamic_cast<NATCompiler_pix*>(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    if (rule->getRuleType()== NATRule::SNAT)
    {
        NATCmd *natcmd=pix_comp->nat_commands[ rule->getInt("nat_cmd") ];

        if (natcmd->ignore_nat) return true;

        for (map<int,NATCmd*>::iterator i1=pix_comp->nat_commands.begin();
             i1!=pix_comp->nat_commands.end(); ++i1) 
        {
            NATCmd *nc = (*i1).second;
/* since map nat_commands is sorted by the key, we only have to scan it
 * until we hit natcmd 
 */
            if (nc->ignore_nat) continue;
            if (natcmd==nc) break;

            Interface *int1 = natcmd->o_iface;
            Interface *int2 = nc->o_iface;

//            InetAddr a1=natcmd->o_addr->getAddress();
//            InetAddr a2=nc->o_addr->getAddress();
//
//            InetAddr   m1=natcmd->o_addr->getInetAddr();
//            InetAddr   m2=nc->o_addr->getNetmask();

            if ( int1->getId()==int2->getId() &&
                 natcmd->o_src==nc->o_src &&
                 natcmd->o_dst==nc->o_dst &&
                 *(natcmd->o_srv)==*(nc->o_srv)
            )
            {
                ostringstream str;
                str << "Duplicate NAT detected: rules "
                    << rule->getLabel()
                    << " and "<< nc->rule_label
                    << " : "<< natcmd->o_src->getAddressPtr()->toString()
                    << "/"<< natcmd->o_src->getNetmaskPtr()->toString()
                    <<  " "
                    <<  natcmd->o_srv->getProtocolName()
                    << " "
                    <<  TCPUDPService::cast(natcmd->o_srv)->getSrcRangeStart()
                    << ":"
                    <<  TCPUDPService::cast(natcmd->o_srv)->getSrcRangeEnd()
                    << " "
                    << "->"<< natcmd->o_dst->getAddressPtr()->toString()
                    << "/"<< natcmd->o_dst->getNetmaskPtr()->toString()
                    <<  " "
                    <<  TCPUDPService::cast(natcmd->o_srv)->getDstRangeStart()
                    << "/"
                    <<  TCPUDPService::cast(natcmd->o_srv)->getDstRangeEnd();

                compiler->abort(rule, str.str());
        }
    }
}
return true;
}

bool  NATCompiler_pix::DetectOverlappingStatics::processNext()
{
    NATCompiler_pix *pix_comp=dynamic_cast<NATCompiler_pix*>(compiler);
    NATRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    if (rule->getRuleType()== NATRule::DNAT )
    {
        StaticCmd *scmd=pix_comp->static_commands[ rule->getInt("sc_cmd") ];

        for (map<int,StaticCmd*>::iterator i1=pix_comp->static_commands.begin();
             i1!=pix_comp->static_commands.end();  i1++ )
        {
//            int        scid=i1->first;
            StaticCmd *sc=  i1->second;
            if (sc->ignore_scmd_and_print_acl) continue;
            if (sc==scmd) break;

            if (Interface::isA(scmd->oaddr) && Interface::isA(sc->oaddr))
            {
                if ( *(sc->osrv) == *(scmd->osrv) && 
                     *(sc->tsrv) == *(scmd->tsrv) && 
                     *(sc->osrc) == *(scmd->osrc) &&
                     sc->oaddr->getId() == scmd->oaddr->getId())
                    compiler->abort(
                        
                            rule, 
                            "Static NAT rules overlap or are redundant : rules "+
                            sc->rule+" and "+scmd->rule+" : "+
                            "outside address: "+
                            "interface "+Interface::cast(scmd->oaddr)->getLabel()+
                            " inside address: "+
                            scmd->iaddr->getAddressPtr()->toString()+"/"+
                            scmd->iaddr->getNetmaskPtr()->toString());
            } else
            {
                if ( *(sc->osrv) == *(scmd->osrv) && 
                     *(sc->tsrv) == *(scmd->tsrv) && 
                     *(sc->osrc) == *(scmd->osrc))
                {
                    const InetAddrMask *ia1 =
                        scmd->iaddr->getInetAddrMaskObjectPtr();
                    const InetAddrMask *ia2 =
                        sc->iaddr->getInetAddrMaskObjectPtr();

                    const InetAddrMask *oa1 =
                        scmd->oaddr->getInetAddrMaskObjectPtr();
                    const InetAddrMask *oa2 =
                        sc->oaddr->getInetAddrMaskObjectPtr();

                    if ( ! getOverlap(*(ia1), *(ia2)).empty() ||
                         ! getOverlap(*(oa1), *(oa2)).empty() )
                        compiler->abort(
                            
                                rule, 
                                "Static NAT rules overlap or are redundant: rules "+
                                sc->rule+" and "+scmd->rule+" : "+
                                "outside address: "+
                                scmd->oaddr->getAddressPtr()->toString()+"/"+
                                scmd->oaddr->getNetmaskPtr()->toString()+
                                " inside address: "+
                                scmd->iaddr->getAddressPtr()->toString()+"/"+
                                scmd->iaddr->getNetmaskPtr()->toString());
                }
            }
        }
    }

    return true;
}

