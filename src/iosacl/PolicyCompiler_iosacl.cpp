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

#include "config.h"

#include "PolicyCompiler_iosacl.h"

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

string PolicyCompiler_iosacl::myPlatformName() { return "iosacl"; }

PolicyCompiler_iosacl::PolicyCompiler_iosacl(FWObjectDatabase *_db,
                                             const std::string &fwname,
                                             bool ipv6_policy,
                                             OSConfigurator *_oscnf) :
    PolicyCompiler_cisco(_db, fwname, ipv6_policy, _oscnf)
{
    resetinbound=false;
    fragguard=false;
}

int PolicyCompiler_iosacl::prolog()
{
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");
    string host_os = fw->getStr("host_OS");

    if (platform!="iosacl")
	abort("Unsupported platform " + platform );

    object_groups = new Group();
    dbcopy->add( object_groups );

    output << "!################"  << endl;

    if ( fw->getOptionsObject()->getBool("iosacl_acl_substitution") )
    {
        /* Generate short temporary ACL and assign it to all
         * interfaces. This ACL permits IPSEC (IP proto 50 and UDP port 500)
         as well as ssh from given subnet to any.
        */

        string temp_acl = "tmp_acl";
        string temp_acl_addr = fw->getOptionsObject()->getStr("iosacl_acl_temp_addr");
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
                    InetAddr nm(netmask);
                    nm.getLength(); // to avoid warning abt unused var
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
            InetAddr a(addr);
            a.isAny();
        } catch(FWException &ex)
        {
            abort("Invalid address for management subnet: '"+addr+"'");
        }

        string clearACLcmd = Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/")+
               "version_"+version+"/iosacl_commands/clear_ip_acl");

        output << endl;


        // cisco uses "wildcards" instead of netmasks

        //long nm = InetAddr(netmask).to32BitInt();
        //struct in_addr na;
        //na.s_addr = ~nm;
        InetAddr nnm( ~(InetAddr(netmask)) );

        string addr_family_prefix = "ip";
        if (ipv6) addr_family_prefix = "ipv6";

        output << clearACLcmd << " " << temp_acl << endl;
        output << addr_family_prefix
               << " access-list extended " << temp_acl << endl;
        output << "  permit ip "
               << addr << " " << nnm.toString() << " any " << endl;
        output << "  deny " << addr_family_prefix
               << " any any " << endl;
        output << "exit" << endl;
        output << endl;

        // find management interface
        int nmi = 0;
        list<FWObject*> ll = fw->getByType(Interface::TYPENAME);
        for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
        {
            Interface *intf = Interface::cast( *i );
            if (intf->isManagement())
            {
                nmi++;
                output << "interface " << intf->getName() << endl;
                output << "  no " << addr_family_prefix
                       << " access-group in" << endl;
                output << "  no " << addr_family_prefix
                       << " access-group out" << endl;
                output << "  " << addr_family_prefix
                       << " access-group " << temp_acl << " in" << endl;
                output << "exit" << endl;
            }
        }
        if (nmi==0)
        {
            abort("One of the interfaces of the firewall must be marked as management interface.");
        }

        output << endl;
    }

    return PolicyCompiler::prolog();
}

bool PolicyCompiler_iosacl::checkForDynamicInterface::findDynamicInterface(
    PolicyRule *rule, RuleElement *rel)
{
    string vers=compiler->fw->getStr("version");
    for (list<FWObject*>::iterator i1=rel->begin(); i1!=rel->end(); ++i1) 
    {
	FWObject *o   = *i1;
	FWObject *obj = NULL;
	if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
        Interface *iface=Interface::cast(obj);
        if (iface!=NULL && iface->isDyn())
            compiler->abort("Dynamic interface can not be used in the IOS ACL rules. Rule "+rule->getLabel());
    }

    return true;
}

bool PolicyCompiler_iosacl::checkForDynamicInterface::processNext()
{
    PolicyRule     *rule=getNext(); if (rule==NULL) return false;

    findDynamicInterface(rule,rule->getSrc());
    findDynamicInterface(rule,rule->getDst());

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_iosacl::SpecialServices::processNext()
{
    PolicyCompiler_iosacl *iosacl_comp=dynamic_cast<PolicyCompiler_iosacl*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    Service    *s=compiler->getFirstSrv(rule);

    if (IPService::cast(s)!=NULL)
    {
	if (s->getBool("rr")        ||
	    s->getBool("ssrr")      ||
	    s->getBool("ts") )
	    compiler->abort("IOS ACL does not support checking for IP options in ACLs. Rule: "+rule->getLabel());
    }
    if (TCPService::cast(s)!=NULL) {
	if (s->getBool("ack_flag")  ||
	    s->getBool("fin_flag")  ||
	    s->getBool("rst_flag")  ||
	    s->getBool("syn_flag") )
	    compiler->abort("IOS ACL does not support checking for TCP options in ACLs. Rule: "+rule->getLabel());
    }

    tmp_queue.push_back(rule);
    return true;
}

void PolicyCompiler_iosacl::compile()
{
    cout << endl;
    cout << " Compiling ruleset " << getSourceRuleSet()->getName();
    if (ipv6) cout << ", IPv6";
    cout <<  endl << flush;

    try 
    {
        string vers = fw->getStr("version");
        string platform = fw->getStr("platform");

	Compiler::compile();

	addDefaultPolicyRule();

        if ( fw->getOptionsObject()->getBool ("check_shading") ) 
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
            add( new ExpandMultipleAddressesInSRC(
                     "expand objects with multiple addresses in SRC" ) );
            add( new ExpandMultipleAddressesInDST(
                     "expand objects with multiple addresses in DST" ) );
            add( new dropRuleWithEmptyRE(
                     "drop rules with empty rule elements"));
            add( new ConvertToAtomic("convert to atomic rules"       ) );
            add( new DetectShadowing("Detect shadowing"              ) );
            add( new simplePrintProgress() );

            runRuleProcessors();
            deleteRuleProcessors();
        }


        add( new Begin (" Start processing rules" ) );
        add( new printTotalNumberOfRules ( ) );

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

        add( new ItfNegation( "process negation in Itf" ) );
        add( new InterfacePolicyRules(
                 "process interface policy rules and store interface ids") );

        add( new splitServices ("split rules with different protocols" ) );

        add( new ExpandMultipleAddressesInSRC(
                 "expand objects with multiple addresses in SRC" ) );
        add( new MACFiltering ("check for MAC address filtering" ) );
//        add( new splitByNetworkZonesForSrc ("split rule if objects in Src belong to different network zones " ) );
//        add( new replaceFWinDSTPolicy ("replace fw with its interface in DST in global policy rules") );

        add( new ExpandMultipleAddressesInDST(
                 "expand objects with multiple addresses in DST" ) );
        add( new MACFiltering(
                 "check for MAC address filtering" ) );
        add( new dropRuleWithEmptyRE(
                 "drop rules with empty rule elements"));
//        add( new splitByNetworkZonesForDst ("split rule if objects in Dst belong to different network zones " ) );

        add( new checkForUnnumbered(
                 "check for unnumbered interfaces" ) );

        add( new addressRanges ("process address ranges" ) );
        add( new dropRuleWithEmptyRE(
                 "drop rules with empty rule elements"));

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

        add( new ConvertToAtomic ("convert to atomic rules" ) );

        add( new simplePrintProgress());

        add( new createNewCompilerPass ("Creating ACLs ..."));

//        add( new ClearACLs("Clear ACLs"));

        // This processor prints each ACL separately in one block.
        // It adds comments inside to denote original rules.
        //
        add( new PrintCompleteACLs("Print ACLs"));

//        add( new PrintRule("generate code for ACLs"));
        add( new simplePrintProgress());

        runRuleProcessors();

    } catch (FWException &ex) {
	error(ex.toString());
        exit(1);
    }
}

string PolicyCompiler_iosacl::printAccessGroupCmd(ciscoACL *acl)
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
        str << "  " << addr_family_prefix
            << " access-group " << acl->workName() << " " << dir << endl;
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
        if (acl->size()!=0) output << printAccessGroupCmd(acl);
    }
    output << endl;

    if ( fw->getOptionsObject()->getBool("iosacl_regroup_commands") ) 
    {
        cout << " Regrouping commands \n" << flush;
        regroup();
    }
}

