/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: PolicyCompiler_pix.cpp,v 1.1 2008/03/06 06:49:00 vkurland Exp $

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

#include "Helper.h"

#include "PolicyCompiler_pix.h"
#include "NATCompiler_pix.h"
#include "PIXObjectGroup.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/AddressTable.h"

#include <iostream>
#if __GNUC__ > 3 || \
    (__GNUC__ == 3 && (__GNUC_MINOR__ > 2 || (__GNUC_MINOR__ == 2 ) ) ) || \
     _MSC_VER
#  include <streambuf>
#else
#  include <streambuf.h>
#endif
#include <iomanip>
#include <algorithm>
#include <functional>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string PolicyCompiler_pix::myPlatformName() { return "pix"; }

PolicyCompiler_pix::PolicyCompiler_pix(FWObjectDatabase *_db,
                                       const std::string &fwname,
                                       OSConfigurator *_oscnf,
                                       NATCompiler_pix *_natcmp) :
    PolicyCompiler_cisco(_db,fwname,_oscnf) 
{
    natcmp=_natcmp;
    resetinbound=false;
    fragguard=false;
}

int PolicyCompiler_pix::prolog()
{
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");
    string host_os = fw->getStr("host_OS");

    if (platform!="pix" && platform!="fwsm") 
	abort("Unsupported platform " + platform );

    object_groups=new Group();
    dbcopy->add( object_groups );

    output << "!################"  << endl;

    if (platform=="fwsm")
    {
        if (fw->getOptionsObject()->getBool("pix_use_manual_commit") )
            output << "access-list mode manual" << endl;
        else
            output << "access-list mode auto" << endl;
    }

    if ( fw->getOptionsObject()->getBool("pix_acl_substitution") )
    {
        /* Generate short temporary ACL and assign it to all
         * interfaces. This ACL permits IPSEC (IP proto 50 and UDP port 500)
         as well as ssh from given subnet to any.
        */

        string temp_acl = "tmp_acl";
        string temp_acl_addr = fw->getOptionsObject()->getStr("pix_acl_temp_addr");
        if (temp_acl_addr.empty())
        {
            abort("Missing address for management host or subnet for temporary ACL.\nPlease enter it in the tab 'Script options' in 'Firewall Settings' dialog");
        }

        string::size_type slash_idx = temp_acl_addr.find('/');
        string addr = temp_acl_addr;
        string netmask = "255.255.255.255";

        if (slash_idx!=string::npos)
        {
            addr = temp_acl_addr.substr(0,slash_idx);
            netmask = temp_acl_addr.substr(slash_idx+1);
            try
            {
                if (netmask.find(".")!=string::npos)
                {
                    Netmask nm(netmask);
                    nm.to32BitInt(); // to avoid warning abt unused var
                } else
                {
                    int nm_length;
                    istringstream  str(netmask);
                    str >> nm_length;
                    Netmask nm(nm_length);
                    netmask = nm.toString();
                }
            } catch(FWException &ex)
            {
                abort("Invalid netmask for management subnet: '"+netmask+"'");
            }
        }

        try
        {
            IPAddress a(addr);
            a.to32BitInt();
        } catch(FWException &ex)
        {
            abort("Invalid address for management subnet: '"+addr+"'");
        }

        string clearACLcmd = Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/")+
               "version_"+version+"/pix_commands/clear_acl");

        output << endl;

        output << clearACLcmd << " " << temp_acl << endl;
        output << "access-list " << temp_acl
               << " permit ip "
               << addr << " " << netmask
               << " any "
               << endl;
        output << "access-list " << temp_acl
               << " deny ip any any "
               << endl;
        if (platform=="fwsm" &&
            fw->getOptionsObject()->getBool("pix_use_manual_commit") )
            output << "access-list commit" << endl;

        output << endl;

        output << "access-group " << temp_acl
               << " in interface outside" << endl;
        output << "access-group " << temp_acl
               << " in interface inside" << endl;

        output << endl;
    }

    return PolicyCompiler::prolog();
}

bool PolicyCompiler_pix::checkVersionAndDynamicInterface::findDynamicInterface(
    PolicyRule *rule, RuleElement *rel)
{
    string vers=compiler->fw->getStr("version");
    for (list<FWObject*>::iterator i1=rel->begin(); i1!=rel->end(); ++i1) 
    {
	FWObject *o   = *i1;
	FWObject *obj = NULL;
	if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
        Interface *iface=Interface::cast(obj);
        if (iface!=NULL && iface->isDyn() && (vers=="6.1" || vers=="6.2"))
            compiler->abort("Dynamic interface can be used in the policy rule only in v6.3 or later. Rule "+rule->getLabel());
    }

    return true;
}

bool PolicyCompiler_pix::checkVersionAndDynamicInterface::processNext()
{
    PolicyRule     *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    Service    *s=compiler->getFirstSrv(rule);

/* if service is ssh, telnet or icmp then we can use dynamic interface
 * even in earlier versions */
    if (ICMPService::isA(s)) return true;
    if (TCPService::isA(s))
    {
        if ( s->getInt("dst_range_start")==22 && 
             s->getInt("dst_range_end")==22) return true;
        if ( s->getInt("dst_range_start")==23 && 
             s->getInt("dst_range_end")==23) return true;
    }

    findDynamicInterface(rule,rule->getSrc());
    findDynamicInterface(rule,rule->getDst());

    return true;
}

bool PolicyCompiler_pix::SpecialServices::processNext()
{
    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    Service    *s=compiler->getFirstSrv(rule);

    if (IPService::cast(s)!=NULL) {
	if (s->getBool("short_fragm") ||
	    s->getBool("fragm") ) {

	    pix_comp->fragguard=true;
	    return true;   // do not copy the rule
	}
	if (s->getBool("rr")        ||
	    s->getBool("ssrr")      ||
	    s->getBool("ts") )
	    compiler->abort("PIX does not support checking for IP options in ACLs. Rule: "+rule->getLabel());
    }
    if (TCPService::cast(s)!=NULL) {
	if (s->getBool("ack_flag")  ||
	    s->getBool("fin_flag")  ||
	    s->getBool("rst_flag")  ||
	    s->getBool("syn_flag") )
	    compiler->abort("PIX does not support checking for TCP options in ACLs. Rule: "+rule->getLabel());
    }

    tmp_queue.push_back(rule);
    return true;
}

/*
 * if dst contains firewall, it must be a single object there.
 */
bool PolicyCompiler_pix::PrepareForICMPCmd::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    Address   *dst=compiler->getFirstDst(rule);
    Service   *srv=compiler->getFirstSrv(rule);

    if (ICMPService::isA(srv) &&
        compiler->complexMatch(dst,compiler->fw))
            rule->setBool("icmp_cmd",true);

    tmp_queue.push_back(rule);

    return true;
}


bool PolicyCompiler_pix::SplitSRCForICMPCmd::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if (rule->getBool("icmp_cmd"))
    {

        RuleElementSrc  *src=rule->getSrc();
        if (src->size()==1) 
        {
            tmp_queue.push_back(rule);
            return true;
        }

        for (FWObject::iterator i=src->begin(); i!=src->end(); ++i)
        {
            FWObject *o   = *i;
            FWObject *obj = NULL;
            if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
            Address *a=Address::cast(obj);
            assert(a!=NULL);

            PolicyRule *new_rule= PolicyRule::cast(compiler->dbcopy->create(PolicyRule::TYPENAME) );
            compiler->temp_ruleset->add(new_rule);
            new_rule->duplicate(rule);
            RuleElementSrc *new_re=new_rule->getSrc();
            new_re->clearChildren();
            new_re->addRef(a);

            tmp_queue.push_back(new_rule);
        }

    } else
        tmp_queue.push_back(rule);

    return true;
}

/*
 *  About "service resetinbound" command:
 *
 *  "The service command works with all inbound TCP connections to
 *   statics whose access lists or uauth (user authorization) do not
 *   allow inbound"
 */
bool PolicyCompiler_pix::RejectAction::processNext()
{
    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    if (rule->getAction()==PolicyRule::Reject) 
        pix_comp->resetinbound=true;
    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_pix::replaceNATtedObjects::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);
    Interface *rule_iface = compiler->getCachedFwInterface(rule->getInterfaceId());
//    string rule_iface_id=rule->getInterfaceId();

//    Address *src=compiler->getFirstSrc(rule);
//    Service *srv=compiler->getFirstSrv(rule);

    RuleElementSrc *srcrel=rule->getSrc();
    RuleElementDst *dstrel=rule->getDst();
    RuleElementSrv *srvrel=rule->getSrv();

    list<PolicyRule*>    t_rules;
    list<PolicyRule*>    transformed_rules;

    for (list<FWObject*>::iterator i1=srcrel->begin(); i1!=srcrel->end(); ++i1) 
    {
        for (list<FWObject*>::iterator i2=dstrel->begin(); i2!=dstrel->end(); ++i2) 
        {
            for (list<FWObject*>::iterator i3=srvrel->begin(); i3!=srvrel->end(); ++i3) 
            {
                FWObject *o1  = *i1;
                FWObject *o2  = *i2;
                FWObject *o3  = *i3;
                FWObject *obj1 = NULL;
                FWObject *obj2 = NULL;
                FWObject *obj3 = NULL;

                if (FWReference::cast(o1)!=NULL) 
                    obj1=FWReference::cast(o1)->getPointer();
                Address *src=Address::cast(obj1);
                assert(src!=NULL);

                if (FWReference::cast(o2)!=NULL)
                    obj2=FWReference::cast(o2)->getPointer();
                Address *dst=Address::cast(obj2);
                assert(dst!=NULL);

                if (FWReference::cast(o3)!=NULL) 
                    obj3=FWReference::cast(o3)->getPointer();
                Service *srv=Service::cast(obj3);
                assert(srv!=NULL);

                list<triplet> tl = pix_comp->natcmp->findDNATForAddress(
                    src,dst,srv);

                for( list<triplet>::iterator t=tl.begin(); t!=tl.end(); ++t)
                {
                    FWObject *p = t->dst->getParent();
                    if (t->dst->getId()==rule_iface->getId() ||
                        p->getId()==rule_iface->getId())
                    {
                        PolicyRule  *r = PolicyRule::cast(
                            compiler->dbcopy->create(PolicyRule::TYPENAME) );
                        compiler->temp_ruleset->add(r);
                        r->duplicate(rule);

                        RuleElementSrc *nsrc=r->getSrc();
                        nsrc->clearChildren();
                        nsrc->addRef( src );

                        RuleElementDst *ndst=r->getDst();
                        ndst->clearChildren();
                        ndst->addRef( t->dst );

                        RuleElementSrv *nsrv=r->getSrv();
                        nsrv->clearChildren();
                        nsrv->addRef( t->srv );

                        t_rules.push_back(r);
                    }
                }
            }
        }
    }
/* list t_rules has all the atomic rules that have a
 * matching NAT rule, with dst and srv already converted. We just add them to
 * the policy on top of the original rule.
 */
    for (list<PolicyRule*>::iterator i1=t_rules.begin(); i1!=t_rules.end(); ++i1)
    {
        PolicyRule *r=PolicyRule::cast( *i1 );
        tmp_queue.push_back(r);
    }

    tmp_queue.push_back(rule);

    return true;
}




/*
 * processor splitIfDstMatchesFw should have made a firewall a single
 * object in dst
 */
bool PolicyCompiler_pix::splitIfTelnetSSHICMPtoFw::processNext()
{
    PolicyRule     *rule=getNext(); if (rule==NULL) return false;
//    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);

    Address *dst=compiler->getFirstDst(rule);
    RuleElement *re=rule->getSrc();
    if (re->size()!=1 && dst->getId()==compiler->getFwId())
    {
        for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
        {
            FWObject *o   = *i1;
            FWObject *obj = o;
            if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();

            PolicyRule  *r= PolicyRule::cast(compiler->dbcopy->create(PolicyRule::TYPENAME) );
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);

            RuleElementSrc *nsrc=r->getSrc();
            nsrc->clearChildren();
            nsrc->addRef( obj );
            tmp_queue.push_back(r);
        }
    } else 
        tmp_queue.push_back(rule);

    return true;
}

/*
 * this is probably not necessary. PIX prints all acl rules with
 * object-groups twice: first time as entered, with object-group, and
 * the second time it expands the group (for convenience ?). I thought
 * it does not print original rule for icmp but it looks like it it
 * does it for icmp just like for other protocols. PIX is ok, I made a
 * mistake. I keep with rule processor just in case, but comment out
 * the call to it.
 */
bool PolicyCompiler_pix::AvoidObjectGroup::processNext()
{
    PolicyRule     *rule=getNext(); if (rule==NULL) return false;
//    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);

    RuleElement    *srv=RuleElement::cast(rule->getFirstByType(RuleElementSrv::TYPENAME));
    if (srv->size()==1)  // no need to create object-group since there is single object in the rule element
    {
        tmp_queue.push_back(rule);
        return true;
    }

    FWObject *o = srv->front();
    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
    if (ICMPService::isA(o))
    {
/* we have a rule with multiple icmp services in Srv. We do not want
 * to use object-group for it because PIX 6.3(3) expands them anyway,
 * which breaks incremental installer.
 */
        for (FWObject::iterator i1=srv->begin(); i1!=srv->end(); ++i1)
        {
            PolicyRule *r = PolicyRule::cast(
                compiler->dbcopy->create(PolicyRule::TYPENAME) );
            r->duplicate(rule);
            compiler->temp_ruleset->add(r);

            FWObject *s;
            s=r->getSrv();      assert(s);
            s->clearChildren();
            s->add( *i1 );

            tmp_queue.push_back(r);
        }
        return true;
    }



    tmp_queue.push_back(rule);
    return true;
}

PIXGroup* PolicyCompiler_pix::CreateObjectGroups::findObjectGroup(RuleElement *re)
{
    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);

    list<FWObject*> relement;

    for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
        
        relement.push_back(obj);
    }


    for (FWObject::iterator i=pix_comp->object_groups->begin();
         i!=pix_comp->object_groups->end(); ++i)
    {
        PIXGroup *og=dynamic_cast<PIXGroup*>(*i);
        assert(og!=NULL);

        if (og->size()==0 || (og->size()!=re->size()) ) continue;

        bool match=true;
        for (FWObject::iterator i1=og->begin(); i1!=og->end(); ++i1) 
        {
            FWObject *o   = *i1;
            FWObject *obj = o;
            if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();

            if ( find(relement.begin(), relement.end(), obj)==relement.end() )
            {
                match=false;
                break;
            }
        }
        if (match) return og;
    }
    return NULL;
}

bool PolicyCompiler_pix::CreateObjectGroups::processNext()
{
    PolicyRule     *rule=getNext(); if (rule==NULL) return false;
    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);
    Interface      *rule_iface = compiler->getCachedFwInterface(rule->getInterfaceId());
    assert(rule_iface);

    RuleElement    *re=RuleElement::cast(rule->getFirstByType(re_type));
    if (re->size()==1)  // no need to create object-group since there is single object in the rule element
    {
        tmp_queue.push_back(rule);
        return true;
    }

    PIXGroup *obj_group=findObjectGroup(re);
    if (obj_group==NULL)
    {
        obj_group= new PIXGroup();

        FWObject *o=re->front();
        FWObject *obj = o;
        if (FWReference::cast(o)!=NULL)   obj=FWReference::cast(o)->getPointer();

        if (Address::cast(obj)!=NULL)     obj_group->setPIXGroupType(NETWORK);
        if (IPService::cast(obj)!=NULL)   obj_group->setPIXGroupType(PROTO);
        if (ICMPService::cast(obj)!=NULL) obj_group->setPIXGroupType(ICMP_TYPE);
        if (TCPService::cast(obj)!=NULL)  obj_group->setPIXGroupType(TCP_SERVICE);
        if (UDPService::cast(obj)!=NULL)  obj_group->setPIXGroupType(UDP_SERVICE);

        obj_group->setName(
            rule_iface->getLabel()+"."+rule->getUniqueId()+"."+name_suffix);

        pix_comp->object_groups->add(obj_group);
        pix_comp->cacheObj(obj_group);

        for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
        {
            FWObject *o   = *i1;
            FWObject *obj = o;
            if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
            obj_group->addRef( obj );
        }
    }
    re->clearChildren(false);   // do not want to destroy children objects

    re->addRef(obj_group);

    assert(re->size()==1);

    tmp_queue.push_back(rule);
    return true;
}

void PolicyCompiler_pix::compile()
{
    cout << " Compiling policy for " << fw->getName() << " ..." <<  endl << flush;

    try 
    {
        string vers = fw->getStr("version");
        string platform = fw->getStr("platform");
        bool   outbound_acl_supported = Resources::platform_res[platform]->getResourceBool(
            string("/FWBuilderResources/Target/options/")+
            "version_"+vers+
            "/pix_outbound_acl_supported");
        bool generate_out_acl = fw->getOptionsObject()->getBool("pix_generate_out_acl");

        if (outbound_acl_supported && !generate_out_acl)
        {
            // behave like if outbound acls are not supported but are emulated
            outbound_acl_supported = false;
            fw->getOptionsObject()->setBool("pix_emulate_out_acl", true);
        }

	Compiler::compile();

	addDefaultPolicyRule();

        if ( fw->getOptionsObject()->getBool ("check_shading")) 
        {
            add( new Begin ("Detecting rule shadowing" ));
            add( new printTotalNumberOfRules ( ));

            add( new ItfNegation( "process negation in Itf" ));
            add( new InterfacePolicyRules("process interface policy rules and store interface ids"));

            add( new recursiveGroupsInSrc( "check for recursive groups in SRC" ));
            add( new recursiveGroupsInDst( "check for recursive groups in DST" ));
            add( new recursiveGroupsInSrv( "check for recursive groups in SRV" ));

            add( new ExpandGroups ("expand groups" ));
            add( new eliminateDuplicatesInSRC ("eliminate duplicates in SRC" ));
            add( new eliminateDuplicatesInDST ("eliminate duplicates in DST" ));
            add( new eliminateDuplicatesInSRV ("eliminate duplicates in SRV" ));
            add( new ExpandMultipleAddressesInSRC("expand objects with multiple addresses in SRC" ));
            add( new ExpandMultipleAddressesInDST("expand objects with multiple addresses in DST" ));
            add( new ConvertToAtomic ("convert to atomic rules" ));
            add( new DetectShadowing ("Detect shadowing" ));
            add( new simplePrintProgress ( ));

            runRuleProcessors();
            deleteRuleProcessors();
        }


        add( new Begin (" Start processing rules" ));
        add( new printTotalNumberOfRules ( ));
        add( new RejectAction ("check for action 'Reject'" ));

        add( new recursiveGroupsInSrc( "check for recursive groups in SRC" ));
        add( new recursiveGroupsInDst( "check for recursive groups in DST" ));
        add( new recursiveGroupsInSrv( "check for recursive groups in SRV" ));

        add( new emptyGroupsInSrc( "check for empty groups in SRC" ));
        add( new emptyGroupsInDst( "check for empty groups in DST" ));
        add( new emptyGroupsInSrv( "check for empty groups in SRV" ));

        add( new ExpandGroups ("expand groups" ));
        add( new eliminateDuplicatesInSRC( "eliminate duplicates in SRC" ));
        add( new eliminateDuplicatesInDST( "eliminate duplicates in DST" ));
        add( new eliminateDuplicatesInSRV( "eliminate duplicates in SRV" ));

        add( new processMultiAddressObjectsInSrc(
                 "process MultiAddress objects in Src"));
        add( new processMultiAddressObjectsInDst(
                 "process MultiAddress objects in Dst"));

        add( new ItfNegation( "process negation in Itf" ));
        add( new InterfacePolicyRules(
                 "process interface policy rules and store interface ids"));

	if ( fwopt->getBool("pix_assume_fw_part_of_any")) {
// add( new splitIfSrcAny( "split rule if src is any" ));
            add( new splitIfDstAny( "split rule if dst is any" ));
        }

        add( new splitIfSrcMatchesFw ("split rule if Src matches FW" ));
        add( new splitIfDstMatchesFw ("split rule if Dst matches FW" ));

// if ( !outbound_acl_supported )
// add( new fillDirection_v6 ("determine directions" ));

//	if ( fwopt->getBool("pix_replace_natted_objects"))
// add( new replaceNATtedObjects ("replace objects in DST that are TDst in DNAT translations" ));

        add( new telnetToFirewall(
                 "separate rules controlling telnet to firewall"));
        add( new sshToFirewall("separate rules controlling ssh to firewall" ));
        add( new splitServices("split rules with different protocols" ));
        add( new PrepareForICMPCmd("prepare for icmp command" ));

        add( new replaceFWinSRCInterfacePolicy(
                 "replace fw with its interface in SRC in interface policy rules"));
        add( new replaceFWinDSTInterfacePolicy(
                 "replace fw with its interface in DST in interface policy rules"));

        add( new ExpandMultipleAddressesInSRC(
                 "expand objects with multiple addresses in SRC" ));
        add( new MACFiltering("check for MAC address filtering" ));
        add( new splitByNetworkZonesForSrc(
                 "split rule if objects in Src belong to different network zones " ));
        add( new replaceFWinDSTPolicy(
                 "replace fw with its interface in DST in global policy rules"));

        add( new ExpandMultipleAddressesInDST(
                 "expand objects with multiple addresses in DST" ));
        add( new MACFiltering("check for MAC address filtering" ));
        add( new splitByNetworkZonesForDst(
                 "split rule if objects in Dst belong to different network zones " ));

        add( new checkForUnnumbered( "check for unnumbered interfaces" ));

        add( new addressRanges("process address ranges" ));

        if (outbound_acl_supported )
        {
            add( new setInterfaceAndDirectionBySrc(
                     "Set interface and direction for rules with interface 'all' using SRC; v7"));
            add( new setInterfaceAndDirectionByDst(
                     "Set interface and direction for rules with interface 'all' using DST; v7"));
            add(new setInterfaceAndDirectionIfInterfaceSet(
                    "Set direction for rules with interface not 'all'; v7"));
        } else
        {
            add( new SplitDirection_v6("split rules with direction 'both'" ));
// add( new assignRuleToInterface ("assign rules to interfaces" ));
            add( new EmulateOutboundACL_v6("emulate outbound ACL" ));
            add( new assignRuleToInterface_v6("assign rules to interfaces" ));
            add( new InterfaceAndDirection_v6(
                     "check for combinations of interface and direction"));
        }

        add( new specialCaseWithDynInterface(
                 "check for a special cases with dynamic interface" ));

        add( new SplitSRCForICMPCmd( "split SRC for icmp commands" ));

	if ( fwopt->getBool("pix_replace_natted_objects"))
            add( new replaceNATtedObjects(
                  "replace objects in DST that are TDst in DNAT translations"));


        if (outbound_acl_supported )
            // first arg is false because we are not using
            // "ip access-list" for PIX.
            add( new pickACL( false, "assign ACLs for v7" ));
        else
            add( new pickACL_v6( "assign ACLs for v6" ));

        add( new SpecialServices( "check for special services" ));
        add( new checkForZeroAddr( "check for zero addresses" ));
        add( new checkVersionAndDynamicInterface(
                 "check for dynamic interfaces in policy rule and verify version of PIX OS"));

        add( new splitIfTelnetSSHICMPtoFw(
                 "split rule if there are multiple objects in src and it controlls access to the firewall"));

        /* remove redundant objects only after all splits has been
         * done, right before object groups are created
         */
        add( new removeRedundantAddressesFromSrc(
                 "remove redundant addresses from Src"));
        add( new removeRedundantAddressesFromDst(
                 "remove redundant addresses from Dst"));
// add( new AvoidObjectGroup("avoid object groups for certain cases"));
        add( new CreateObjectGroupsForSrc("create object groups for Src"));
        add( new CreateObjectGroupsForDst("create object groups for Dst"));
        add( new CreateObjectGroupsForSrv("create object groups for Srv"));

        add( new simplePrintProgress());

        add( new createNewCompilerPass("Creating object groups and ACLs ..."));

        add( new PrintObjectGroupsAndClearCommands(
                 "Clear ACLs and generate code for object groups"));
        add( new PrintRule("generate code for ACLs"));
        add( new simplePrintProgress());

/*
  if ( fw->getOptionsObject()->getBool("pix_check_rule_shadowing")) 
  {
  add( new createNewCompilerPass (" Detecting rule shadowing ..." ));
  add( new ExpandGroups ("expand groups" ));
  add( new ConvertToAtomic ("convert to atomic rules" ));
  add( new DetectShadowing ("Detect shadowing" ));
  add( new simplePrintProgress ( ));
  }
*/
        runRuleProcessors();

    } catch (FWException &ex) {
	error(ex.toString());
        exit(1);
    }
}

string PolicyCompiler_pix::printAccessGroupCmd(ciscoACL *acl)
{
    string dir;
    if (acl->direction()=="in" || acl->direction()=="Inbound") dir="in";
    if (acl->direction()=="out" || acl->direction()=="Outbound") dir="out";
    return string("access-group ") + acl->workName() +
        " " + dir + 
        " interface " + acl->getInterface()->getLabel() + "\n";
}

void PolicyCompiler_pix::epilog()
{
    output << endl;
    if (resetinbound) output << "service resetinbound" << endl;

    output << endl;
    if (fw->getStr("platform")=="fwsm" && fw->getOptionsObject()->getBool("pix_use_manual_commit"))
    {
        output << "access-list commit" << endl;
        output << endl;
    }

    for (map<string,ciscoACL*>::iterator i=acls.begin(); i!=acls.end(); ++i) 
    {
        ciscoACL *acl=(*i).second;
        if (acl->size()!=0) output << printAccessGroupCmd(acl);
    }
    output << endl;

    if ( fw->getOptionsObject()->getBool("pix_regroup_commands")) 
    {
        cout << " Regrouping commands \n" << flush;
        regroup();
    }
}

