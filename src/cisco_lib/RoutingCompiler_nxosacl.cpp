/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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


#include "RoutingCompiler_nxosacl.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Network.h"

#include <string>
#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;


string RoutingCompiler_nxosacl::myPlatformName() { return "nxosacl"; }

int RoutingCompiler_nxosacl::prolog()
{
    int n = RoutingCompiler_cisco::prolog();    

    if (fw->getStr("platform")!="nxosacl") 
	abort("Unsupported platform " + fw->getStr("platform") );

    return n;
}

void RoutingCompiler_nxosacl::epilog()
{
}

/*
 * Replace objects in dst and gw with their ip addresses, except if
 * interface of the firewall is found in gw, it is left intact because
 * NXOS allows for using interface name as gateway in "ip route"
 * command.
 */
bool RoutingCompiler_nxosacl::ExpandMultipleAddressesExceptInterface::processNext()
{
    RoutingRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    RuleElementRDst *dst = rule->getRDst();    assert(dst);
    compiler->_expand_addr(rule, dst, true);

    RuleElementRGtw *gtwrel = rule->getRGtw();    assert(gtwrel);
    Address *gtw = Address::cast(
        FWReference::cast(gtwrel->front())->getPointer());
    if (gtw == nullptr)
        compiler->abort(rule, "Broken GTW");
    if (Interface::isA(gtw) && gtw->isChildOf(compiler->fw)) return true;
    compiler->_expand_addr(rule, gtwrel, false);
    return true;
}


bool RoutingCompiler_nxosacl::checkRItfAndGw::processNext()
{
    RoutingRule *rule=getNext(); if (rule==nullptr) return false;
    tmp_queue.push_back(rule);

    RuleElementRItf *itfrel = rule->getRItf();    assert(itfrel);
    RuleElementRGtw *gtwrel = rule->getRGtw();    assert(gtwrel);

    if (!itfrel->isAny() && !gtwrel->isAny())
        compiler->abort(rule, "Can not use both gateway address and interface in "
                        "NXOS routing rule");

    return true;
}



/**
 *-----------------------------------------------------------------------
 */
void RoutingCompiler_nxosacl::compile()
{
    printRule = new RoutingCompiler_nxosacl::PrintRule("");

    info(" Compiling routing rules for " + fw->getName());

    Compiler::compile();

    add(new RoutingCompiler::Begin());
    add(new printTotalNumberOfRules());
        
    add( new singleRuleFilter());

    add(new recursiveGroupsInRDst("Check for recursive Groups in RDst"));
    add(new emptyGroupsInRDst("Check for empty Groups in RDst"));
    add(new emptyRDstAndRItf("Check if RDst and RItf are both empty"));

    // add(new singleAdressInRGtw(
    //         "Check if RGtw object has exactly one IP adress"));

    add(new rItfChildOfFw("Check if RItf is an Iterface of this firewall"));
    add(new checkRItfAndGw("Both gateway and interface can not be used in the same rule"));

    add(new validateNetwork("Validate network addresses"));
    add(new reachableAddressInRGtw(
            "Check if RGtw is reachable via local networks"));
    //add(new contradictionRGtwAndRItf(
    //        "Check if RGtw is in a network of RItf"));

    add(new ExpandGroups("Expand groups in DST"));
    add(new ExpandMultipleAddressesExceptInterface(
            "Expand objects with multiple addresses in DST"));
    add(new eliminateDuplicatesInDST("Eliminate duplicates in DST"));
        
    add(new createSortedDstIdsLabel(
            "Create label with a sorted dst-id-list for 'competingRules'"));
    add(new competingRules("Check for competing rules"));

    add(new ConvertToAtomicForDST(
            "Convert to atomic rules by dst address elements"));
        
    add(new createSortedDstIdsLabel(
            "Create label with a sorted dst-id-list for 'classifyRoutingRules'"));
    add(new classifyRoutingRules(
            "Classify into single path or part of a multi path rule"));
        
    //add(new eliminateDuplicateRules(
    //        "Eliminate duplicate rules over the whole table"));
        
    add(new PrintRule("generate ip code"));
    add(new simplePrintProgress());

    runRuleProcessors();

}


