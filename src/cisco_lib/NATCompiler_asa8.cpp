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

#include "config.h"

#include "NATCompiler_asa8.h"

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

void NATCompiler_asa8::compile()
{
    info(" Compiling NAT rules for " + fw->getName());

    try {

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

        add( new dropRuleWithEmptyRE("drop rules with empty rule elements"));

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

/* REMOVE_OLD_OPTIMIZATIONS
        if (fw->getOptionsObject()->getBool("pix_optimize_default_nat"))
            add (new clearOSrc ("clear OSrc" ));
*/

        add( new createNATCmd ("create NAT commands" ));
        add( new createStaticCmd ("create static commands" ));

/* REMOVE_OLD_OPTIMIZATIONS
        add( new mergeNATCmd ("merge NAT commands" ));
        add( new SuppressDuplicateNONATStatics(
                 "suppress duplicate NONAT statics" ));

        add( new checkForObjectsWithErrors(
                 "check if we have objects with errors in rule elements"));
*/

        add( new PrintClearCommands("Clear ACLs" ));
        add( new PrintObjectsForNat("generate objects for nat commands"));
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

    } catch (FWException &ex)
    {
	error(ex.toString());
        exit(1);
    }
}

