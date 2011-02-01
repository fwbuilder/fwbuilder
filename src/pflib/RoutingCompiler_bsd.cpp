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


#include "RoutingCompiler_bsd.h"

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

#include <QStringList>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

static std::map<std::string,int> tmp_chain_no;

string RoutingCompiler_bsd::myPlatformName() { return "pf"; }


void RoutingCompiler_bsd::verifyOS()
{
    QStringList supported_os = QString(
        Resources::platform_res[fw->getStr("platform")]->
        getResourceStr("/FWBuilderResources/Target/supported_os").c_str())
        .split(",");

    QString host_os = fw->getStr("host_OS").c_str();
    if (!supported_os.contains(host_os)) 
	abort("Unsupported host OS " + host_os.toStdString());

}

int RoutingCompiler_bsd::prolog()
{
    int n = RoutingCompiler::prolog();    

    verifyOS(); 

    return n;
}

bool RoutingCompiler_bsd::addressRangesInDst::processNext()
{
    RoutingRule *rule;
    rule=getNext(); if (rule==NULL) return false;

    RuleElementRDst *dstrel = rule->getRDst();
    compiler->_expandAddressRanges(rule, dstrel);

    tmp_queue.push_back(rule);
    return true;
}


bool RoutingCompiler_bsd::FindDefaultRoute::processNext()
{
    RoutingCompiler_bsd *bsd_comp = dynamic_cast<RoutingCompiler_bsd*>(compiler);
    RoutingRule *rule;
    rule=getNext(); if (rule==NULL) return false;

    RuleElementRDst *dstrel = rule->getRDst();
    FWObject *ref = dstrel->front();
    Address *dst = Address::cast(FWReference::cast(ref)->getPointer());
    if (dst->isAny()) bsd_comp->have_default_route = true;

    tmp_queue.push_back(rule);
    return true;
}


/**
 *-----------------------------------------------------------------------
 */
void RoutingCompiler_bsd::compile()
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
        
#ifdef ECMP_SUPPORT_OLD_STYLE
    add(new optimize3(
            "Eliminate duplicate rules generated from a single gui-rule"));
    add(new eliminateDuplicateRules(
            "Eliminate duplicate rules over the whole table"));
#endif
        
    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));

    add(new PrintRule("generate ip code"));
    add(new simplePrintProgress());

    runRuleProcessors();

}

string RoutingCompiler_bsd::debugPrintRule(Rule *r)
{
    RoutingRule *rule=RoutingRule::cast(r);

    string s= RoutingCompiler::debugPrintRule(rule);

    return s;
}

void RoutingCompiler_bsd::epilog()
{
    ///int total = ecmp_comments_buffer.size();
    int nb = 0;
    
    // ecmp roules can only be generated after all the rules have been
    // parsed, that is the reason for putting this code in the epilog
    // function
    if (ecmp_rules_buffer.size() > 0)
    {
        output << "\n#\n# ============== EQUAL COST MULTI PATH ============\n#"
               << endl;
        
        output << "echo \"Activating ecmp routing rules...\"" << endl;
                
        for (map<string,string>::iterator
                 ecmp_comments_buffer_it = ecmp_comments_buffer.begin();
             ecmp_comments_buffer_it != ecmp_comments_buffer.end();
             ++ecmp_comments_buffer_it)
        {
            output << ecmp_comments_buffer_it->second << "#\n" << flush;
    
            output << ecmp_rules_buffer[ecmp_comments_buffer_it->first] << flush;
           
            output << " \\\n|| route_command_error " << "\"" << ++nb << "\"" << endl;

            //echo \"Error: The ECMP routing rule #" << ++nb <<" couldn't be activated! Please make sure your kernel is compiled with the CONFIG_IP_ROUTE_MULTIPATH option.\"" << endl;
            
        }
    }
    
    if (!inSingleRuleCompileMode() && defined_restore_script_output)
    {
        // function restore_script_output may not be defined if we
        // have no rules or all rules are disabled
        output << endl;
        output << "restore_script_output" << endl;
        output << "echo \"...done.\"" << endl;
    }
}
