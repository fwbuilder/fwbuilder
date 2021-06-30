/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

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


#include "NATCompiler_asa8.h"
#include "NamedObject.h"
#include "ASA8ObjectGroup.h"
#include "NamedObjectsAndGroupsSupport.h"

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

#include <algorithm>
#include <functional>
#include <iostream>
#include <cstring>
#include <assert.h>

#include <QString>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


NATCompiler_asa8::NATCompiler_asa8(FWObjectDatabase *_db,
                                   Firewall *fw,
                                   bool ipv6_policy,
                                   OSConfigurator *_oscnf) : 
    NATCompiler_pix(_db, fw, ipv6_policy, _oscnf)
{ 
}

NATCompiler_asa8::~NATCompiler_asa8()
{
}
        
/*
 * Option "translate dns" can not be used if the rule has "destination"
 * part.
 */
bool NATCompiler_asa8::VerifyValidityOfDNSOption::processNext()
{
    NATRule *rule = getNext(); if (rule==nullptr) return false;
    FWOptions *ropt = rule->getOptionsObject();

    if (ropt->getBool("asa8_nat_dns"))
    {
        Address *odst = compiler->getFirstODst(rule);  assert(odst);
        if (!odst->isAny())
        {
            compiler->abort(rule,
                            "Option 'translate dns' can not be used in combination "
                            "with destination matching or translation");
        }

        Service  *osrv = compiler->getFirstOSrv(rule);  assert(osrv);
        if (!osrv->isAny())
        {
            compiler->abort(rule,
                            "Option 'translate dns' can not be used in combination "
                            "with service matching or translation");
        }

    }

    tmp_queue.push_back(rule);
    return true;
}

/*
 * After we call CreateObjectGroupsForTSrc to create object group for
 * TSrc, it can be one of the following:
 *
 *  - any
 *  - single address
 *  - single group (object group that was created by CreateObjectGroupsForTSrc)
 *  - an address and interface
 *  - a group and interface
 *
 * CreateObjectGroups::processNext() always puts interface first and group or
 * address second in TSrc
 */
bool NATCompiler_asa8::VerifyValidityOfTSrc::processNext()
{
    NATRule *rule = getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    RuleElementTSrc  *tsrc_re = rule->getTSrc();  assert(tsrc_re);
    if (tsrc_re->isAny()) return true;
    if (tsrc_re->size()==1) return true;
    if (tsrc_re->size()==2)
    {
        FWObject *obj1 = nullptr;
        FWObject *obj2 = nullptr;
        for (FWObject::iterator it=tsrc_re->begin(); it!=tsrc_re->end(); ++it)
        {
            if (obj1 == nullptr) obj1 = FWReference::getObject(*it);
            if (obj2 == nullptr) obj2 = FWReference::getObject(*it);
        }
        if (Interface::isA(obj1) && Address::cast(obj2)!=nullptr) return true;
        if (Interface::isA(obj1) && Group::cast(obj2)!=nullptr) return true;
        QString err("Invalid combination of objects in TSrc: %1 (%2) and %3 (%4) ");
        compiler->abort(
            rule,
            err.arg(obj1->getName().c_str()).arg(obj1->getTypeName().c_str())
            .arg(obj2->getName().c_str()).arg(obj2->getTypeName().c_str())
            .toStdString());
    }

    compiler->abort(rule, "TSrc has >2 objects");

    return true;
}

bool NATCompiler_asa8::VerifyRules::processNext()
{
    NATRule *rule = getNext(); if (rule==nullptr) return false;
    string version = compiler->fw->getStr("version");

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

    // if (rule->getRuleType()==NATRule::NONAT && (!osrv->isAny() || !tsrv->isAny()))
    // {
    //     compiler->abort(
    //             rule, 
    //             "'no nat' rules should have no services");
    //     return true;
    // }

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
            "Translated service should be 'Original' or should contain "
            "single object.");
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
                "Original and translated source should both be networks "
                "of the same size");
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
                "Original and translated destination should both be networks "
                "of the same size.");
            return true;
        }
    }

    if (rule->getRuleType()==NATRule::SNetnat) rule->setRuleType(NATRule::SNAT);
    if (rule->getRuleType()==NATRule::DNetnat) rule->setRuleType(NATRule::DNAT);

    if ((rule->getRuleType()==NATRule::DNAT || 
         rule->getRuleType()==NATRule::SDNAT) &&
        odst->isAny())
    {
            compiler->abort(
                rule, 
                "Oiginal destination can not be \"any\" in rules that translate "
                "destination");
            return true;
    }

    tmp_queue.push_back(rule);

    return true;
}

bool NATCompiler_asa8::verifyInterfacesInNatRule::processNext()
{
    NATRule *rule = getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);
    return true;
}



void NATCompiler_asa8::compile()
{
    info(" Compiling NAT rules for " + fw->getName());

    Compiler::compile();

    add( new Begin( "Begin processing"));
    add( new printTotalNumberOfRules());

    add( new singleRuleFilter());

/* REMOVE_OLD_OPTIMIZATIONS
   if (fw->getOptionsObject()->getBool( "pix_optimize_default_nat"))
   add (new optimizeDefaultNAT(
   "optimize commands 'nat (interface) 0.0.0.0 0.0.0.0'"));
*/

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
             "replace host objects with firewall's interfaces if "
             "the have the same address"));

    // ExpandMultipleAddresses acts on different rule elements
    // depending on the rule type.
    // Also using overloaded virtual function  _expand_interface
    add( new ExpandMultipleAddresses("expand multiple addresses"));
    add( new MACFiltering( "check for MAC address filtering"));

    // ASA8 nat commands support address range directly.
    // add( new ExpandAddressRanges("expand address range objects"));

    add( new checkForUnnumbered("check for unnumbered interfaces"));

    add( new splitByNetworkZonesForOSrc("split by netzone for OSrc"));

    //add( new groupServicesByProtocol("group services by protocol in OSrv"));
    add( new ConvertToAtomicForOSrv("convert to atomic for OSrv"));
    add( new ConvertToAtomicForTDst("convert to atomic for TDst"));
    add( new ConvertToAtomicForTSrv("convert to atomic for TSrv"));

    add( new AssignInterface("assign rules to interfaces" ));
    add( new verifyInterfacesInNatRule("verify assignment of interfaces"));
    add( new fillTranslatedSrv("fill translated service element" ));
    add( new verifyRuleElements(
             "verify rule elements for static NAT rules"));
    add( new processNONATRules("process NONAT" ));

    add( new VerifyValidityOfDNSOption(
             "Check validity of 'translate dns' option"));

    //add( new groupTCPUDP("split rules with TCP or UDP services"));

    add( new SpecialServicesOSrv( "check for special services" ));

    add( new CreateObjectGroupsForOSrc("create object groups for OSrc",
                                       named_objects_manager));
    add( new CreateObjectGroupsForODst("create object groups for ODst",
                                       named_objects_manager));
    add( new CreateObjectGroupsForOSrv("create object groups for OSrv",
                                       named_objects_manager));

    // need special rule processor to create object groups in TSrc
    // because of a special tratment that an Interface object gets in TSrc
    add( new CreateObjectGroupsForTSrc("create object groups for TSrc",
                                       named_objects_manager));

    add( new VerifyValidityOfTSrc("verify objects in TSrc"));

/* REMOVE_OLD_OPTIMIZATIONS
   if (fw->getOptionsObject()->getBool("pix_optimize_default_nat"))
   add (new clearOSrc ("clear OSrc" ));
*/

/* WE_DO_NOT_USE_NATCMD_FOR_ASA8
    add( new createNATCmd ("create NAT commands" ));
    add( new createStaticCmd ("create static commands" ));
*/

/* REMOVE_OLD_OPTIMIZATIONS
   add( new mergeNATCmd ("merge NAT commands" ));
   add( new SuppressDuplicateNONATStatics(
   "suppress duplicate NONAT statics" ));

   add( new checkForObjectsWithErrors(
   "check if we have objects with errors in rule elements"));
*/

    //add( new PrintClearCommands("Clear ACLs" ));
    add( new createNamedObjectsForNAT(
             "create named objects", named_objects_manager));
    //add( new printObjectGroups(
    //         "definitions of object groups", named_objects_manager));
    add( new PrintRule("generate PIX code" ));
    add( new storeProcessedRules ("store processed rules" ));
    add( new simplePrintProgress ());

/* REMOVE_OLD_OPTIMIZATIONS
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
*/
    runRuleProcessors();

}

string NATCompiler_asa8::printClearCommands()
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
            "version_" + version + "/pix_commands/clear_nat") << endl;
    }

    return output.str();
}

