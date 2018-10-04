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


#include "Helper.h"

#include "PolicyCompiler_pix.h"
#include "NATCompiler_pix.h"
#include "PIXObjectGroup.h"
#include "NamedObjectsAndGroupsSupport.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/FailoverClusterGroup.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>

#include <assert.h>

#include <QString>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string PolicyCompiler_pix::myPlatformName() { return "pix"; }

PolicyCompiler_pix::PolicyCompiler_pix(FWObjectDatabase *_db,
                                       Firewall *fw,
                                       bool ipv6_policy,
                                       OSConfigurator *_oscnf,
                                       NATCompiler_pix *_natcmp) :
    PolicyCompiler_cisco(_db, fw, ipv6_policy, _oscnf) 
{
    natcmp=_natcmp;
    resetinbound=false;
    fragguard=false;
}

int PolicyCompiler_pix::prolog()
{
    string platform = fw->getStr("platform");

    if (platform!="pix" && platform!="fwsm") 
	abort("Unsupported platform " + platform );

    return PolicyCompiler::prolog();
}

void PolicyCompiler_pix::_expand_interface(Rule *rule,
                                           Interface *iface,
                                           std::list<FWObject*> &ol,
                                           bool expand_cluster_interfaces_fully)
{
    Compiler::_expand_interface(rule, iface, ol, expand_cluster_interfaces_fully);
}


bool PolicyCompiler_pix::checkVersionAndDynamicInterface::findDynamicInterface(
    PolicyRule *rule, RuleElement *rel)
{
    string vers=compiler->fw->getStr("version");
    for (list<FWObject*>::iterator i1=rel->begin(); i1!=rel->end(); ++i1) 
    {
	FWObject *o   = *i1;
	FWObject *obj = nullptr;
	if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();
        Interface *iface=Interface::cast(obj);
        if (iface!=nullptr && iface->isDyn() && (vers=="6.1" || vers=="6.2"))
        {
            compiler->abort(
                rule, 
                "Dynamic interface can be used in the policy rule only "
                "in v6.3 or later.");
            return false;
        }
    }

    return true;
}

bool PolicyCompiler_pix::checkVersionAndDynamicInterface::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    Service *s = compiler->getFirstSrv(rule);

/* if service is ssh, telnet or icmp then we can use dynamic interface
 * even in earlier versions */
    if (ICMPService::isA(s))
    {
        tmp_queue.push_back(rule);
        return true;
    }

    if (TCPService::isA(s))
    {
        if ( s->getInt("dst_range_start")==22 && 
             s->getInt("dst_range_end")==22)
        {
            tmp_queue.push_back(rule);
            return true;
        }
        if ( s->getInt("dst_range_start")==23 && 
             s->getInt("dst_range_end")==23)
        {
            tmp_queue.push_back(rule);
            return true;
        }
    }

    if (findDynamicInterface(rule,rule->getSrc()) && 
        findDynamicInterface(rule,rule->getDst()))
        tmp_queue.push_back(rule);

    return true;
}

/*
 * if dst contains firewall, it must be a single object there.
 */
bool PolicyCompiler_pix::PrepareForICMPCmd::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

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
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

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
            FWObject *obj = nullptr;
            if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();
            Address *a=Address::cast(obj);
            assert(a!=nullptr);

            PolicyRule *new_rule= compiler->dbcopy->createPolicyRule();
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
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    if (rule->getAction()==PolicyRule::Reject) 
        pix_comp->resetinbound=true;
    tmp_queue.push_back(rule);
    return true;
}

/*
 * processor splitIfDstMatchesFw should have made a firewall a single
 * object in dst
 */
bool PolicyCompiler_pix::splitIfTelnetSSHICMPtoFw::processNext()
{
    PolicyRule     *rule=getNext(); if (rule==nullptr) return false;
//    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);

    Address *dst=compiler->getFirstDst(rule);
    RuleElement *re=rule->getSrc();
    if (re->size()!=1 && dst->getId()==compiler->getFwId())
    {
        for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
        {
            FWObject *o   = *i1;
            FWObject *obj = o;
            if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();

            PolicyRule  *r= compiler->dbcopy->createPolicyRule();
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
    PolicyRule     *rule=getNext(); if (rule==nullptr) return false;
//    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);

    RuleElement    *srv=RuleElement::cast(rule->getFirstByType(RuleElementSrv::TYPENAME));
    if (srv->size()==1)  // no need to create object-group since there is single object in the rule element
    {
        tmp_queue.push_back(rule);
        return true;
    }

    FWObject *o = srv->front();
    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
    if (ICMPService::isA(o))
    {
/* we have a rule with multiple icmp services in Srv. We do not want
 * to use object-group for it because PIX 6.3(3) expands them anyway,
 * which breaks incremental installer.
 */
        for (FWObject::iterator i1=srv->begin(); i1!=srv->end(); ++i1)
        {
            PolicyRule *r = compiler->dbcopy->createPolicyRule();
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

/*
 * See #2662: commands "ssh", "telnet" and "http" (those that control
 * access on the corresponding protocols to the firewall itself)
 * accept only ip address of a host or a network as their
 * argument. They do not accept address range, named object or object
 * group. This is so at least as of ASA 8.3. Since we expand address
 * ranges only for versions < 8.3 and use named object for 8.3 and
 * later, we need to make this additional check and still expand
 * address ranges in rules that will later convert to "ssh", "telnet"
 * or "http" command. Call this rule processor after telnetToFirewall,
 * sshToFirewall and httpToFirewall
 */
bool PolicyCompiler_pix::AddressRangesIfTcpServiceToFW::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    if (rule->getBool("tcp_service_to_fw"))
    {
        expandAddressRangesInSrc(rule);
    }

    tmp_queue.push_back(rule);
    return true;
}

void PolicyCompiler_pix::compile()
{
    string banner = " Compiling ruleset " + getSourceRuleSet()->getName();
    if (ipv6) banner += ", IPv6";
    info(banner);

    string vers = fw->getStr("version");
    string platform = fw->getStr("platform");
    bool outbound_acl_supported = Resources::platform_res[platform]->getResourceBool(
        string("/FWBuilderResources/Target/options/")+
        "version_"+vers+
        "/pix_outbound_acl_supported");
    bool generate_out_acl = fw->getOptionsObject()->getBool("pix_generate_out_acl");
    bool object_groups_supported = Resources::platform_res[platform]->getResourceBool(
        string("/FWBuilderResources/Target/options/")+
        "version_"+vers+
        "/pix_object_groups_supported");

    if (outbound_acl_supported && !generate_out_acl)
    {
        // behave like if outbound acls are not supported but are emulated
        outbound_acl_supported = false;
        fw->getOptionsObject()->setBool("pix_emulate_out_acl", true);
    }

    Compiler::compile();

    if ( fw->getOptionsObject()->getBool ("check_shading") &&
         ! inSingleRuleCompileMode())
    {
        add( new Begin ("Detecting rule shadowing" ));
        add( new printTotalNumberOfRules ( ));

        add( new expandGroupsInItf("expand groups in Interface" ));
        add( new replaceClusterInterfaceInItf(
                 "replace cluster interfaces with member interfaces in "
                 "the Interface rule element"));

        add( new ItfNegation( "process negation in Itf" ));
        add( new InterfacePolicyRules("process interface policy rules and "
                                      "store interface ids"));

        add( new recursiveGroupsInSrc( "check for recursive groups in SRC" ));
        add( new recursiveGroupsInDst( "check for recursive groups in DST" ));
        add( new recursiveGroupsInSrv( "check for recursive groups in SRV" ));

        add( new ExpandGroups ("expand groups" ));
        add( new eliminateDuplicatesInSRC ("eliminate duplicates in SRC" ));
        add( new eliminateDuplicatesInDST ("eliminate duplicates in DST" ));
        add( new eliminateDuplicatesInSRV ("eliminate duplicates in SRV" ));

        add( new processMultiAddressObjectsInSrc(
                 "process MultiAddress objects in Src"));
        add( new processMultiAddressObjectsInDst(
                 "process MultiAddress objects in Dst"));

        add( new ExpandMultipleAddressesInSrc(
                 "expand objects with multiple addresses in SRC" ));
        add( new ExpandMultipleAddressesInDst(
                 "expand objects with multiple addresses in DST" ));

        add( new ConvertToAtomic ("convert to atomic rules" ));

        add( new checkForObjectsWithErrors(
                 "check if we have objects with errors in rule elements"));

        add( new DetectShadowing ("Detect shadowing" ));
        add( new simplePrintProgress ( ));

        runRuleProcessors();
        deleteRuleProcessors();
    }


    add( new Begin (" Start processing rules" ));
    add( new printTotalNumberOfRules ( ));

    add( new singleRuleFilter());

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

    add( new expandGroupsInItf("expand groups in Interface" ));
    add( new replaceClusterInterfaceInItf(
             "replace cluster interfaces with member interfaces in "
             "the Interface rule element"));
    add( new ItfNegation( "process negation in Itf" ));
    add( new InterfacePolicyRules(
             "process interface policy rules and store interface ids"));

    if (XMLTools::version_compare(vers, "8.3")<0)
        add( new addressRanges("process address ranges" ));

    /*
     * We do not support ipv6 yet
     */
    add( new DropIPv6RulesWithWarning(
             "drop ipv6 rules",
             "Rule has been suppressed because it contains IPv6 objects and "
             "Firewall Builder does not support IPv6 for this platform"));

    if ( fwopt->getBool("pix_assume_fw_part_of_any"))
    {
        // Note that this splits the rule if Dst==any and one or more
        // icmp services are found in Srv. The name of this rule
        // processor needs to be more descriptive.
        add( new splitIfDstAny( "split rule if dst is any" ));
    }

    add( new splitIfSrcMatchesFw ("split rule if Src matches FW" ));
    add( new splitIfDstMatchesFw ("split rule if Dst matches FW" ));

    add( new telnetToFirewall(
             "separate rules controlling telnet to firewall"));
    add( new sshToFirewall(
             "separate rules controlling ssh to firewall" ));
    add( new httpToFirewall(
             "separate rules controlling http to firewall"));

    add( new AddressRangesIfTcpServiceToFW(
             "process address ranges in rules that control telnet/ssh/http to Fw"));

    add( new separateSrcPort("split rules matching source ports"));
    add( new separateCustom("split rules matching custom services"));

    add( new groupServicesByProtocol("split rules with different protocols"));

    add( new PrepareForICMPCmd("prepare for icmp command" ));
    

    add( new replaceFWinSRCInterfacePolicy(
             "replace fw with its interface in SRC in interface policy rules"));
    add( new replaceFWinDSTInterfacePolicy(
             "replace fw with its interface in DST in interface policy rules"));

    add( new ExpandMultipleAddressesInSrc(
             "expand objects with multiple addresses in SRC" ));
    add( new MACFiltering("check for MAC address filtering" ));
    add( new splitByNetworkZonesForSrc(
             "split rule if objects in Src belong to different network zones " ));
    add( new replaceFWinDSTPolicy(
             "replace fw with its interface in DST in global policy rules"));

    add( new ExpandMultipleAddressesInDst(
             "expand objects with multiple addresses in DST" ));
    add( new MACFiltering("check for MAC address filtering" ));
    add( new splitByNetworkZonesForDst(
             "split rule if objects in Dst belong to different network zones " ));

    add( new checkForUnnumbered( "check for unnumbered interfaces" ));

    if (outbound_acl_supported )
    {
        // Call these after splitIfSrcMatchesFw and splitIfDstMatchesFw
        add( new setInterfaceAndDirectionBySrc(
                 "Set interface and direction for rules with interface "
                 "'all' using SRC; v7"));
        add( new setInterfaceAndDirectionByDst(
                 "Set interface and direction for rules with interface "
                 "'all' using DST; v7"));
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

    if (XMLTools::version_compare(vers, "8.3")<0)
    {
        if ( fwopt->getBool("pix_replace_natted_objects"))
            add( new replaceTranslatedAddresses(
                     "replace objects in DST that are TDst in DNAT "
                     "translations"));
    } else
    {
        add( new warnWhenTranslatedAddressesAreUsed(
                 "warng when addresses that are ODst in DNAT translations "
                 "are used in DST"));
    }

    if (outbound_acl_supported )
        // first arg is false because we are not using
        // "ip access-list" for PIX.
        add( new pickACL( false, "assign ACLs for v7" ));
    else
        add( new pickACL_v6( "assign ACLs for v6" ));

    add( new SpecialServicesSrv( "check for special services" ));
    add( new CheckForUnsupportedUserService("check for user service") );
    add( new checkForZeroAddr( "check for zero addresses" ));
    add( new checkVersionAndDynamicInterface(
             "check for dynamic interfaces in policy rule and verify "
             "version of PIX OS"));

    add( new splitIfTelnetSSHICMPtoFw(
             "split rule if there are multiple objects in src and it "
             "controlls access to the firewall"));

    /* remove redundant objects only after all splits has been
     * done, right before object groups are created
     */
    add( new removeRedundantAddressesFromSrc(
             "remove redundant addresses from Src"));
    add( new removeRedundantAddressesFromDst(
             "remove redundant addresses from Dst"));

    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));

    if (object_groups_supported)
    {
// add( new AvoidObjectGroup("avoid object groups for certain cases"));
        add( new CreateObjectGroupsForSrc("create object groups for Src",
                                          named_objects_manager));
        add( new CreateObjectGroupsForDst("create object groups for Dst",
                                          named_objects_manager));
        add( new CreateObjectGroupsForSrv("create object groups for Srv",
                                          named_objects_manager));
    } else
    {
        add( new ConvertToAtomic ("convert to atomic rules" ));
    }


    add( new simplePrintProgress());

    add( new createNewCompilerPass("Creating object groups and ACLs ..."));

    if (XMLTools::version_compare(vers, "8.3")>=0)
    {
        add( new createNamedObjectsForPolicy(
                 "create named objects", named_objects_manager));
    }

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
}

string PolicyCompiler_pix::printAccessGroupCmd(ciscoACL *acl)
{
    if (getSourceRuleSet()->isTop())
    {
        string dir;
        if (acl->direction()=="in" || acl->direction()=="Inbound") dir="in";
        if (acl->direction()=="out" || acl->direction()=="Outbound") dir="out";
        return string("access-group ") + acl->workName() +
            " " + dir + 
            " interface " + acl->getInterface()->getLabel() + "\n";
    }
    return "";
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
        info(" Regrouping commands");
        regroup();
    }
}

string PolicyCompiler_pix::printClearCommands()
{
    ostringstream output;

    string vers = fw->getStr("version");
    string platform = fw->getStr("platform");

    string clearACLcmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/") +
        "version_" + vers + "/pix_commands/clear_acl");

    // string clearOGcmd = Resources::platform_res[platform]->getResourceStr(
    //     string("/FWBuilderResources/Target/options/") +
    //     "version_" + vers + "/pix_commands/clear_og");

    string clearICMPcmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/") +
        "version_" + vers + "/pix_commands/clear_icmp");

    string clearTelnetcmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/") +
        "version_" + vers + "/pix_commands/clear_telnet");

    if ( fw->getOptionsObject()->getBool("pix_acl_basic") )
    {
        output << clearACLcmd << endl;
        //output << clearOGcmd  << endl;
    }

    if (fw->getOptionsObject()->getBool("pix_acl_substitution"))
    {
        for (map<string,ciscoACL*>::iterator i=acls.begin();
             i!=acls.end(); ++i) 
        {
            ciscoACL *acl = (*i).second;
            output << clearACLcmd << " " << acl->workName() << endl;
        }
        //output << clearOGcmd << endl;
    }

    if ( !fw->getOptionsObject()->getBool("pix_acl_no_clear") )
    {
        output << clearICMPcmd    << endl;
        output << clearTelnetcmd  << endl;
    }

    // see #2322 If this is FWSM and if manual commit mode is used, we
    // need to commit after clearing ACLs before we clear object groups

    if (fw->getStr("platform")=="fwsm" && 
        fw->getOptionsObject()->getBool("pix_use_manual_commit") )
    {
        output << "access-list commit" << endl;
    }

    return output.str();
}

/*
 * This includes commands that should be added first, such as commit mode
 * for FWSM, setting up temporary access list etc.
 */
string PolicyCompiler_pix::printPreambleCommands()
{
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");

    ostringstream output;

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
            abort(
                "Missing address for management host or subnet for "
                "temporary ACL. Enter it in the tab 'Script "
                "options' in 'Firewall Settings' dialog");
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
                    InetAddr nm(netmask);
                    nm.isAny(); // to avoid warning abt unused var
                } else
                {
                    int nm_length;
                    istringstream  str(netmask);
                    str >> nm_length;
                    InetAddr nm(nm_length);
                    netmask = nm.toString();
                }
            } catch(FWException &ex)
            {
                abort("Invalid netmask for management subnet: '"+netmask+"'");
            }
        }

        try
        {
            (void) InetAddr(addr);
        } catch(FWException &ex)
        {
            abort("Invalid address for management subnet: '"+addr+"'");
        }

        string clearACLcmd = Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/")+
            "version_"+version+"/pix_commands/clear_acl");

        output << endl;

        output << clearACLcmd << " " << temp_acl << endl;

        if (fw->getStr("platform")=="fwsm" && 
            fw->getOptionsObject()->getBool("pix_use_manual_commit") )
        {
            output << "access-list commit" << endl;
        }

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

        // see #2347  attach temporary acl to all interfaces

        list<FWObject*> all_interfaces = fw->getByTypeDeep(Interface::TYPENAME);
        list<FWObject*>::iterator i;
        for (i=all_interfaces.begin(); i!=all_interfaces.end(); ++i)
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);

            output << "access-group "
                   << temp_acl
                   << " in interface "
                   << iface->getLabel()
                   << endl;
        }

        output << endl;
    }

    return output.str();
}

