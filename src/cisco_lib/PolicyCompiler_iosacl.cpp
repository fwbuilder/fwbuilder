/* 

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

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


#include "PolicyCompiler_iosacl.h"
#include "NamedObjectsAndGroupsSupport.h"

#include "fwbuilder/AddressTable.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/ObjectMirror.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string PolicyCompiler_iosacl::myPlatformName() { return "iosacl"; }

PolicyCompiler_iosacl::PolicyCompiler_iosacl(FWObjectDatabase *_db,
                                             Firewall *fw,
                                             bool ipv6_policy,
                                             OSConfigurator *_oscnf) :
    PolicyCompiler_cisco(_db, fw, ipv6_policy, _oscnf)
{
    resetinbound = false;
    fragguard = false;
    comment_symbol = "!";
}

int PolicyCompiler_iosacl::prolog()
{
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");
    string host_os = fw->getStr("host_OS");

    if (platform!="iosacl")
	abort("Unsupported platform " + platform );

    fw->getOptionsObject()->setBool(
        "use_acl_remarks",
        fw->getOptionsObject()->getBool("iosacl_use_acl_remarks"));

    // object_groups = new Group();
    // persistent_objects->add( object_groups );

    setAllNetworkZonesToNone();

    return PolicyCompiler::prolog();
}

bool PolicyCompiler_iosacl::checkForDynamicInterface::findDynamicInterface(
    PolicyRule *rule, RuleElement *rel)
{
    string vers=compiler->fw->getStr("version");
    for (list<FWObject*>::iterator i1=rel->begin(); i1!=rel->end(); ++i1) 
    {
	FWObject *o   = *i1;
	FWObject *obj = nullptr;
	if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();
        Interface *iface=Interface::cast(obj);
        if (iface!=nullptr && iface->isDyn())
            compiler->abort(
                rule, 
                "Dynamic interface can not be used in the IOS ACL rules.");
    }

    return true;
}

bool PolicyCompiler_iosacl::checkForDynamicInterface::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    findDynamicInterface(rule,rule->getSrc());
    findDynamicInterface(rule,rule->getDst());

    tmp_queue.push_back(rule);
    return true;
}

/*
 * Copy all references from rule element re1 to rule element re2.
 */
void PolicyCompiler_iosacl::mirrorRule::duplicateRuleElement(
    RuleElement *re1, RuleElement *re2)
{
    re2->clearChildren();
    for (list<FWObject*>::iterator i1=re1->begin(); i1!=re1->end(); ++i1) 
    {
        FWObject *obj = FWReference::getObject(*i1);
        re2->addRef(obj);
    }
}

bool PolicyCompiler_iosacl::mirrorRule::processNext()
{
    //PolicyCompiler_iosacl *iosacl_comp=dynamic_cast<PolicyCompiler_iosacl*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    if (rule->getOptionsObject()->getBool("iosacl_add_mirror_rule"))
    {
        PolicyRule *r= compiler->dbcopy->createPolicyRule();
        compiler->temp_ruleset->add(r);
        r->duplicate(rule);

        r->setAction(rule->getAction());

        switch (rule->getDirection())
        {
        case PolicyRule::Inbound: r->setDirection(PolicyRule::Outbound); break;
        case PolicyRule::Outbound: r->setDirection(PolicyRule::Inbound); break;
        default: r->setDirection(PolicyRule::Both); break;
        }

	RuleElementSrc *osrc = rule->getSrc();
	RuleElementDst *odst = rule->getDst();
	RuleElementSrv *osrv = rule->getSrv();
	RuleElementItf *oitf = rule->getItf();

	RuleElementSrc *nsrc = r->getSrc();
	RuleElementDst *ndst = r->getDst();
	RuleElementSrv *nsrv = r->getSrv();
	RuleElementItf *nitf = r->getItf();

        duplicateRuleElement(osrc, ndst);
        duplicateRuleElement(odst, nsrc);
        duplicateRuleElement(oitf, nitf);

        if (!osrv->isAny())
        {
            ObjectMirror mirror;
            nsrv->clearChildren();
            for (list<FWObject*>::iterator i1=osrv->begin(); i1!=osrv->end(); ++i1) 
            {
                Service *nobj = mirror.getMirroredService(
                    Service::cast(FWReference::getObject(*i1)));
                if (nobj->getParent() == nullptr)
                    compiler->persistent_objects->add(nobj, false);
                nsrv->addRef(nobj);
            }
        }

        tmp_queue.push_back(r);
    }
    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_iosacl::SpecialServices::processNext()
{
    //PolicyCompiler_iosacl *iosacl_comp=dynamic_cast<PolicyCompiler_iosacl*>(compiler);
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    Service *s = compiler->getFirstSrv(rule);

    if (IPService::cast(s)!=nullptr)
    {
	if (s->getBool("rr")        ||
	    s->getBool("ssrr")      ||
	    s->getBool("ts") )
	    compiler->abort(
                    rule, 
                    "IOS ACL does not support checking for IP options in ACLs.");
    }
    if (TCPService::cast(s)!=nullptr && TCPService::cast(s)->inspectFlags())
    {
        string version = compiler->fw->getStr("version");
        if (XMLTools::version_compare(version, "12.4")<0)
            compiler->abort(rule, "TCP flags match requires IOS v12.4 or later.");
    }

    tmp_queue.push_back(rule);
    return true;
}

/*
 * This rule processor is used to separate TCP service objects that
 * match tcp flags when generated config uses object-group clause
 */
bool PolicyCompiler_iosacl::splitTCPServiceWithFlags::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    RuleElementSrv *srv = rule->getSrv();

    if (srv->size() > 1)
    {
        std::list<FWObject*> cl;
        for (list<FWObject*>::iterator i1=srv->begin(); i1!=srv->end(); ++i1) 
        {
            FWObject *o   = *i1;
            FWObject *obj = nullptr;
            if (FWReference::cast(o)!=nullptr) obj=FWReference::cast(o)->getPointer();
            Service *s=Service::cast(obj);
            assert(s!=nullptr);

            TCPService *tcp_srv = TCPService::cast(s);
            if (tcp_srv && (tcp_srv->inspectFlags() || tcp_srv->getEstablished()))
                cl.push_back(s);
        }

        while (!cl.empty()) 
        {
            PolicyRule  *r = compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);

            RuleElementSrv *nsrv = r->getSrv();
            nsrv->clearChildren();
            nsrv->addRef( cl.front() );
            tmp_queue.push_back(r);

            srv->removeRef( cl.front() );
            cl.pop_front();
        }
        if (srv->size()>0) tmp_queue.push_back(rule);

    } else
        tmp_queue.push_back(rule);

    return true;
}



void PolicyCompiler_iosacl::compile()
{
    string banner = " Compiling ruleset " + getSourceRuleSet()->getName();
    if (ipv6) banner += ", IPv6";
    info(banner);

    string version = fw->getStr("version");
    bool supports_object_groups = XMLTools::version_compare(version, "12.4")>=0 &&
        fw->getOptionsObject()->getBool("iosacl_use_object_groups") && ! ipv6;

    string vers = fw->getStr("version");
    string platform = fw->getStr("platform");

    Compiler::compile();

    if ( fw->getOptionsObject()->getBool ("check_shading") &&
         ! inSingleRuleCompileMode())
    {
        add( new Begin("Detecting rule shadowing"               ) );
        add( new printTotalNumberOfRules());

        add( new ItfNegation("process negation in Itf"  ) );
        add( new InterfacePolicyRules(
                 "process interface policy rules and store interface ids"));

        add( new recursiveGroupsInSrc("check for recursive groups in SRC"));
        add( new recursiveGroupsInDst("check for recursive groups in DST"));
        add( new recursiveGroupsInSrv("check for recursive groups in SRV"));

        add( new ExpandGroups("expand groups"));
        add( new dropRuleWithEmptyRE(
                 "drop rules with empty rule elements"));
        add( new eliminateDuplicatesInSRC("eliminate duplicates in SRC"));
        add( new eliminateDuplicatesInDST("eliminate duplicates in DST"));
        add( new eliminateDuplicatesInSRV("eliminate duplicates in SRV"));
        add( new ExpandMultipleAddressesInSrc(
                 "expand objects with multiple addresses in SRC" ) );
        add( new ExpandMultipleAddressesInDst(
                 "expand objects with multiple addresses in DST" ) );
        add( new dropRuleWithEmptyRE(
                 "drop rules with empty rule elements"));

        add( new mirrorRule("Add mirrored rules"));

        add( new ConvertToAtomic("convert to atomic rules"       ) );

        add( new checkForObjectsWithErrors(
                 "check if we have objects with errors in rule elements"));

        add( new DetectShadowing("Detect shadowing"              ) );
        add( new simplePrintProgress() );

        runRuleProcessors();
        deleteRuleProcessors();
    }


    add( new Begin (" Start processing rules" ) );
    add( new printTotalNumberOfRules ( ) );

    add( new singleRuleFilter());

    add( new recursiveGroupsInSrc( "check for recursive groups in SRC" ) );
    add( new recursiveGroupsInDst( "check for recursive groups in DST" ) );
    add( new recursiveGroupsInSrv( "check for recursive groups in SRV" ) );

    add( new emptyGroupsInSrc( "check for empty groups in SRC" ) );
    add( new emptyGroupsInDst( "check for empty groups in DST" ) );
    add( new emptyGroupsInSrv( "check for empty groups in SRV" ) );

    add( new ExpandGroups ("expand groups" ) );
    add( new dropRuleWithEmptyRE(
             "drop rules with empty rule elements"));
    add( new eliminateDuplicatesInSRC( "eliminate duplicates in SRC" ) );
    add( new eliminateDuplicatesInDST( "eliminate duplicates in DST" ) );
    add( new eliminateDuplicatesInSRV( "eliminate duplicates in SRV" ) );

    add( new processMultiAddressObjectsInSrc(
             "process MultiAddress objects in Src") );
    add( new processMultiAddressObjectsInDst(
             "process MultiAddress objects in Dst") );

    add( new expandGroupsInItf("expand groups in Interface" ));
    add( new replaceClusterInterfaceInItf(
             "replace cluster interfaces with member interfaces in the Interface rule element"));

    add( new ItfNegation( "process negation in Itf" ) );
    add( new InterfacePolicyRules(
             "process interface policy rules and store interface ids") );

    add( new groupServicesByProtocol ("split rules with different protocols" ) );

    add( new ExpandMultipleAddressesInSrc(
             "expand objects with multiple addresses in SRC" ) );
    add( new MACFiltering ("check for MAC address filtering" ) );
//        add( new splitByNetworkZonesForSrc ("split rule if objects in Src belong to different network zones " ) );
//        add( new replaceFWinDSTPolicy ("replace fw with its interface in DST in global policy rules") );

    add( new ExpandMultipleAddressesInDst(
             "expand objects with multiple addresses in DST" ) );
    add( new MACFiltering(
             "check for MAC address filtering" ) );
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

//        add( new splitByNetworkZonesForDst ("split rule if objects in Dst belong to different network zones " ) );

    if (ipv6)
        add( new DropIPv4Rules("drop ipv4 rules"));
    else
        add( new DropIPv6Rules("drop ipv6 rules"));
    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    add( new checkForUnnumbered("check for unnumbered interfaces"));

    if ( ! supports_object_groups)
        add( new addressRanges("process address ranges"));

    add( new mirrorRule("Add mirrored rules"));

    add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    add( new setInterfaceAndDirectionBySrc(
             "Set interface and direction for rules with interface 'all' using SRC"));
    add( new setInterfaceAndDirectionByDst(
             "Set interface and direction for rules with interface 'all' using DST"));
    add( new setInterfaceAndDirectionIfInterfaceSet(
             "Set direction for rules with interface not 'all'"));

    add( new specialCaseWithDynInterface(
             "check for a special cases with dynamic interface" ) );

    // first arg is true because we use "ip access-list" for IOS.
    add( new pickACL( true, "assign ACLs" ) );

    add( new SpecialServices( "check for special services" ) );
    add( new CheckForUnsupportedUserService("check for user service") );

    add( new checkForZeroAddr(    "check for zero addresses" ) );
    add( new checkForDynamicInterface("check for dynamic interfaces" ) );

    /* remove redundant objects only after all splits has been
     * done, right before object groups are created
     */
    add( new removeRedundantAddressesFromSrc(
             "remove redundant addresses from Src") );
    add( new removeRedundantAddressesFromDst(
             "remove redundant addresses from Dst") );

    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));

    if (supports_object_groups)
    {
        // "object-group service" does not seem to support
        // matching of tcp flags and "established". Need to
        // separate objects using these into separate rules to avoid
        // object-group

        add( new splitTCPServiceWithFlags(
                 "separate TCP service with tcp flags"));

        add( new CreateObjectGroupsForSrc("create object groups for Src",
                                          named_objects_manager));
        add( new CreateObjectGroupsForDst("create object groups for Dst",
                                          named_objects_manager));
        add( new CreateObjectGroupsForSrv("create object groups for Srv",
                                          named_objects_manager));
    } else
    {
        add( new ConvertToAtomic ("convert to atomic rules" ) );
    }

    add( new simplePrintProgress());
    add( new createNewCompilerPass("Creating object groups and ACLs"));

    // This processor prints each ACL separately in one block.
    // It adds comments inside to denote original rules.
    //
    add( new PrintCompleteACLs("Print ACLs"));
    add( new simplePrintProgress());

    runRuleProcessors();

}

string PolicyCompiler_iosacl::printAccessGroupCmd(ciscoACL *acl, bool neg)
{
    ostringstream str;

    string addr_family_prefix = "ip";
    if (ipv6) addr_family_prefix = "ipv6";

    if (getSourceRuleSet()->isTop())
    {
        string dir;
        if (acl->direction()=="in"  || acl->direction()=="Inbound")  dir="in";
        if (acl->direction()=="out" || acl->direction()=="Outbound") dir="out";

        str << "interface " << acl->getInterface()->getName() << endl;
        if (neg) str << "  no";
        str << "  " << addr_family_prefix << " ";
        str << getAccessGroupCommandForAddressFamily(ipv6);
        str << " " << acl->workName() << " " << dir << endl;
        str << "exit" << endl;
    }
    return str.str();
}

void PolicyCompiler_iosacl::epilog()
{
    output << endl;

    for (map<string,ciscoACL*>::iterator i=acls.begin(); i!=acls.end(); ++i) 
    {
        ciscoACL *acl=(*i).second;
        if (acl->size()!=0) output << printAccessGroupCmd(acl, false);
    }
    output << endl;

    if ( fw->getOptionsObject()->getBool("iosacl_regroup_commands") ) 
    {
        info(" Regrouping commands");
        regroup();
    }
}

string PolicyCompiler_iosacl::getAccessGroupCommandForAddressFamily(bool ipv6)
{
    if (ipv6) return "traffic-filter";
    return "access-group";
}

string PolicyCompiler_iosacl::printClearCommands()
{
    ostringstream output;

    string version = fw->getStr("version");
    string platform = fw->getStr("platform");

    string xml_element = "clear_ip_acl";
    if (ipv6) xml_element = "clear_ipv6_acl";

    string clearACLCmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/") +
        "version_" + version + "/iosacl_commands/" + xml_element);

    assert( !clearACLCmd.empty());

    // No need to output "clear" commands in single rule compile mode
    if ( fw->getOptionsObject()->getBool("iosacl_acl_basic") ||
         fw->getOptionsObject()->getBool("iosacl_acl_substitution"))
    {
        for (map<string,ciscoACL*>::iterator i=acls.begin(); i!=acls.end(); ++i)
        {
            ciscoACL *acl = (*i).second;
            output << clearACLCmd << " " << acl->workName() << endl;
        }
    }

    return output.str();
}

