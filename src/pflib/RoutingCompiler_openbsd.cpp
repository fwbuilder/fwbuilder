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


#include "RoutingCompiler_openbsd.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Network.h"

#include <stack>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

#include <assert.h>

#include <QObject>
#include <QStringList>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

static std::map<std::string,int> tmp_chain_no;

string RoutingCompiler_openbsd::myPlatformName() { return "pf"; }


void RoutingCompiler_openbsd::verifyOS()
{
    QStringList supported_os = QString(
        Resources::platform_res[fw->getStr("platform")]->
        getResourceStr("/FWBuilderResources/Target/supported_os").c_str())
        .split(",");

    QString host_os = fw->getStr("host_OS").c_str();
    if (!supported_os.contains(host_os)) 
	abort("Unsupported host OS " + host_os.toStdString());

}

int RoutingCompiler_openbsd::prolog()
{
    int n = RoutingCompiler::prolog();    

    verifyOS(); 

    return n;
}

bool RoutingCompiler_openbsd::addressRangesInDst::processNext()
{
    RoutingRule *rule;
    rule=getNext(); if (rule==nullptr) return false;

    RuleElementRDst *dstrel = rule->getRDst();
    compiler->_expandAddressRanges(rule, dstrel);

    tmp_queue.push_back(rule);
    return true;
}


bool RoutingCompiler_openbsd::FindDefaultRoute::processNext()
{
    RoutingCompiler_openbsd *bsd_comp = dynamic_cast<RoutingCompiler_openbsd*>(compiler);
    RoutingRule *rule;
    rule=getNext(); if (rule==nullptr) return false;

    RuleElementRDst *dstrel = rule->getRDst();
    FWObject *ref = dstrel->front();
    Address *dst = Address::cast(FWReference::cast(ref)->getPointer());
    if (dst->isAny()) bsd_comp->have_default_route = true;

    tmp_queue.push_back(rule);
    return true;
}

/*
 *  this processor eliminates duplicate routing rules, generated from the same
 *  rule in the GUI
 */
bool RoutingCompiler_openbsd::optimize3::processNext()
{
    RoutingCompiler_openbsd *bsd_comp = dynamic_cast<RoutingCompiler_openbsd*>(compiler);
    RoutingRule *rule = getNext(); if (rule==nullptr) return false;

    if (rule->isFallback() || rule->isHidden())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    assert (bsd_comp->printRule!=nullptr);
    
    string thisRule = rule->getLabel() + " " +
        bsd_comp->printRule->RoutingRuleToString(rule, false);
    
    if (rules_seen_so_far.count(thisRule)!=0) return true;

    tmp_queue.push_back(rule);
    rules_seen_so_far[thisRule] = true;

    return true;
}

/*
 *  this processor eliminates duplicate atomic routing rules in one routing table
 */
bool RoutingCompiler_openbsd::eliminateDuplicateRules::processNext()
{
    RoutingCompiler_openbsd *bsd_comp = dynamic_cast<RoutingCompiler_openbsd*>(compiler);
    RoutingRule *rule = getNext(); if (rule==nullptr) return false;

    if (rule->isFallback() || rule->isHidden())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    assert (bsd_comp->printRule!=nullptr);
    
    string thisRule = bsd_comp->printRule->RoutingRuleToString(rule, false);
    
    map<string, string>::iterator rules_it = rules_seen_so_far.find(thisRule);
            
    if (rules_it != rules_seen_so_far.end())
    {
        QString msg = QObject::tr("Two of the routing commands created from the gui "
                                  "routing rules %1 and %2 "
                                  "are identical, skipping the second. "
                                  "Revise them to avoid this warning");
        compiler->warning(
            rule,
            msg.arg(rules_it->second.c_str()).arg(rule->getLabel().c_str()).toStdString());
        return true;
    }

    tmp_queue.push_back(rule);
    rules_seen_so_far[thisRule] = rule->getLabel();
    return true;
}


/**
 *-----------------------------------------------------------------------
 */
void RoutingCompiler_openbsd::compile()
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

    add( new processMultiAddressObjectsInRDst(
             "process MultiAddress objects in RDst") );

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

string RoutingCompiler_openbsd::debugPrintRule(Rule *r)
{
    RoutingRule *rule = RoutingRule::cast(r);
    return RoutingCompiler::debugPrintRule(rule);
}

void RoutingCompiler_openbsd::epilog()
{
    if (!inSingleRuleCompileMode() && defined_restore_script_output)
    {
        // function restore_script_output may not be defined if we
        // have no rules or all rules are disabled
        output << endl;
        output << "restore_script_output" << endl;
        output << "echo \"...done.\"" << endl;
    }
}
