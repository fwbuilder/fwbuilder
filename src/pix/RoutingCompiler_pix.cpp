/*
 * Copyright (c) 2008 Steven Mestdagh
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "RoutingCompiler_pix.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Network.h"

#include <stack>

#include <iostream>
#if __GNUC__ > 3 || \
    (__GNUC__ == 3 && (__GNUC_MINOR__ > 2 || (__GNUC_MINOR__ == 2 ) ) ) || \
    _MSC_VER
#  include <streambuf>
#else
#  include <streambuf.h>
#endif
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;


string RoutingCompiler_pix::myPlatformName() { return "pix"; }

int RoutingCompiler_pix::prolog()
{
    int n = RoutingCompiler_cisco::prolog();    

    if (fw->getStr("platform")!="pix") 
	abort("Unsupported platform " + fw->getStr("platform") );

    return n;
}

void RoutingCompiler_pix::epilog()
{
}

/**
 *-----------------------------------------------------------------------
 */
void RoutingCompiler_pix::compile()
{
    printRule = new RoutingCompiler_pix::PrintRule("");

    cout << " Compiling routing rules for "
         << fw->getName() << " ..." <<  endl << flush;

    try 
    {
	Compiler::compile();

        add(new RoutingCompiler::Begin());
        add(new printTotalNumberOfRules());
        
        add(new recursiveGroupsInRDst("Check for recursive Groups in RDst"));
        add(new emptyGroupsInRDst("Check for empty Groups in RDst"));
        add(new emptyRDstAndRItf("Check if RDst and RItf are both empty"));
        add(new singleAdressInRGtw(
                "Check if RGtw object has exactly one IP adress"));
        add(new rItfChildOfFw("Check if RItf is an Iterface of this firewall"));
        add(new validateNetwork("Validate network addresses"));
        add(new reachableAddressInRGtw(
                "Check if RGtw is reachable via local networks"));
        add(new contradictionRGtwAndRItf(
                "Check if RGtw is in a network of RItf"));

        add(new ExpandGroups("Expand groups in DST"));
        add(new ExpandMultipleAddresses(
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
        
        add(new eliminateDuplicateRules(
                "Eliminate duplicate rules over the whole table"));
        
        add(new PrintRule("generate ip code"));
        add(new simplePrintProgress());

        runRuleProcessors();

    } catch (FWException &ex) {
	error(ex.toString());
	exit(1);
    }
}


