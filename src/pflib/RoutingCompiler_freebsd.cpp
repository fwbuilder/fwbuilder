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


#include "RoutingCompiler_freebsd.h"

#include "fwbuilder/Firewall.h"

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

/**
 * Need to reimplement this function even though it looks exactly the same
 * as the one in RoutingCompiler_openbsd. The difference is that it picks up
 * class RoutingCompiler_freebsd::PrintRule 
 *
 */
void RoutingCompiler_freebsd::compile()
{
    string banner = " Compiling routing rules for " + fw->getName();
    info(banner);

    Compiler::compile();
    //bool check_for_recursive_groups=true;

    add(new RoutingCompiler::Begin());
    add(new printTotalNumberOfRules());
        
    add( new singleRuleFilter());

    add(new recursiveGroupsInRDst("Check for recursive Groups in RDst"));
    add(new emptyGroupsInRDst("Check for empty Groups in RDst"));
    add(new emptyRDstAndRItf("Check if RDst and RItf are both empty"));
    add(new singleAdressInRGtw(
            "Check if RGtw object has exactly one IP adress"));
    add(new rItfChildOfFw("Check if RItf is an Iterface of this firewall"));
    add(new interfaceOrGateway(
            "Check that the rule has either gw or interface but not both"));
    add(new validateNetwork("Validate network addresses"));
    add(new reachableAddressInRGtw(
            "Check if RGtw is reachable via local networks"));
    add(new contradictionRGtwAndRItf(
            "Check if RGtw is in a network of RItf"));

    add(new ExpandGroups("Expand groups in DST"));
    add(new ExpandMultipleAddresses(
            "Expand objects with multiple addresses in DST"));

    add(new addressRangesInDst("process address ranges"));

    //add(new eliminateDuplicatesInDST("Eliminate duplicates in DST"));

    add(new FindDefaultRoute("Find rules that install default route"));

#ifdef ECMP_SUPPORT_OLD_STYLE
    add(new createSortedDstIdsLabel(
            "Create label with a sorted dst-id-list for 'competingRules'"));
    add(new competingRules("Check for competing rules"));
#endif

    add(new ConvertToAtomicForDST(
            "Convert to atomic rules by dst address elements"));

    add(new sameDestinationDifferentGateways(
            "detect rules with the same destination but different gateways. We do not "
            "support ECMP at this time"));

    // add(new createSortedDstIdsLabel(
    //         "Create label with a sorted dst-id-list for 'classifyRoutingRules'"));
    // add(new classifyRoutingRules(
    //         "Classify into single path or part of a multi path rule"));
        
    add(new optimize3(
            "Eliminate duplicate rules generated from a single gui-rule"));
    add(new eliminateDuplicateRules(
            "Eliminate duplicate rules over the whole table"));
        
    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));

    add(printRule=new PrintRule("generate ip code"));
    add(new simplePrintProgress());

    runRuleProcessors();

}

