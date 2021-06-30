/* 

                  Firewall Builder Routing add-on

                 Copyright (C) 2004 Compal GmbH, Germany

  Author:  Tidei Maurizio     <fwbuilder-routing at compal.de>
  
  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#include "RoutingCompiler_ipt.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Network.h"

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

static std::map<std::string,int> tmp_chain_no;

string RoutingCompiler_ipt::myPlatformName() { return "iptables"; }


void RoutingCompiler_ipt::verifyPlatform()
{
    //TODO: Routing based on the 'ip' command is independent from iptables
    string family = Resources::platform_res[fw->getStr("platform")]->
        getResourceStr("/FWBuilderResources/Target/family");

    if (family != myPlatformName()) 
	abort("Unsupported platform " + fw->getStr("platform") +
              " (family " + family + ")");
}

int RoutingCompiler_ipt::prolog()
{
    int n = RoutingCompiler::prolog();    

    verifyPlatform(); 

    return n;
}


/*
 *  this processor eliminates duplicate routing rules, generated from the same
 *  rule in the GUI
 */
bool RoutingCompiler_ipt::optimize3::processNext()
{
    RoutingRule *rule;
    rule=getNext(); if (rule==nullptr) return false;

    if (rule->isFallback() || rule->isHidden())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    if (printRule==nullptr)
    {
        printRule = new PrintRule("");
        printRule->setContext(compiler);
    }
    
    string thisRule = rule->getLabel() + " " + printRule->RoutingRuleToString(rule);
    
    if (rules_seen_so_far.count(thisRule)!=0) return true;

    tmp_queue.push_back(rule);
    rules_seen_so_far[thisRule] = true;

    return true;
}

/*
 *  this processor eliminates duplicate atomic routing rules in one routing table
 */
bool RoutingCompiler_ipt::eliminateDuplicateRules::processNext()
{
    RoutingRule *rule;
    rule = getNext(); if (rule==nullptr) return false;

    if (rule->isFallback() || rule->isHidden())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    if (printRule==nullptr)
    {
        printRule = new PrintRule("");
        printRule->setContext(compiler);
    }
    
    string label = rule->getLabel();
    int bracepos = label.find("(");
    label.erase(0, bracepos);
    
    string thisRule = label + " " + printRule->RoutingRuleToString(rule);
    
    rules_it = rules_seen_so_far.find(thisRule);
            
    if (rules_it != rules_seen_so_far.end())
    {
        string msg;
        msg = "Two of the sub rules created from the gui routing rules " +
            rules_it->second + " and " + rule->getLabel() +
            " are identical, skipping the second. " +
            "Revise them to avoid this warning";
        compiler->warning(rule,  msg.c_str());
        return true;
    }

    tmp_queue.push_back(rule);
    rules_seen_so_far[thisRule] = rule->getLabel();
    return true;
}

bool RoutingCompiler_ipt::addressRangesInDst::processNext()
{
    RoutingRule *rule;
    rule=getNext(); if (rule==nullptr) return false;

    RuleElementRDst *dstrel = rule->getRDst();
    compiler->_expandAddressRanges(rule, dstrel);

    tmp_queue.push_back(rule);
    return true;
}


bool RoutingCompiler_ipt::FindDefaultRoute::processNext()
{
    RoutingCompiler_ipt *ipt_comp = dynamic_cast<RoutingCompiler_ipt*>(compiler);
    RoutingRule *rule;
    rule=getNext(); if (rule==nullptr) return false;

    RuleElementRDst *dstrel = rule->getRDst();
    FWObject *ref = dstrel->front();
    Address *dst = Address::cast(FWReference::cast(ref)->getPointer());
    if (dst->isAny()) ipt_comp->have_default_route = true;

    tmp_queue.push_back(rule);
    return true;
}


/**
 *-----------------------------------------------------------------------
 */
void RoutingCompiler_ipt::compile()
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

    // unfortunately ExpandGroups also filters out objects that do not
    // match address family but does not issue a warning when that
    // happens. Since I need to show a warning when user places ipv6
    // object in a routing rule, I call DropIPv6RulesWithWarning()
    // before expanding groups. This has limited effect though: user
    // gets a warning when a single ipv6 address or network object
    // appears in the rule but gets no warning when it appears as a
    // member of a group.
    add(new DropIPv6RulesWithWarning(
            "drop ipv6 rules",
            "Rule has been suppressed because it contains IPv6 "
            "objects and Firewall Builder does not support IPv6 "
            "routing rules at this time"));

    add(new ExpandGroups("Expand groups in DST"));
    add(new ExpandMultipleAddresses(
            "Expand objects with multiple addresses in DST"));
    add(new dropRuleWithEmptyRE("drop rules with empty rule elements"));

    add(new DropIPv6RulesWithWarning(
            "drop ipv6 rules",
            "Rule has been suppressed because it contains IPv6 "
            "objects and Firewall Builder does not support IPv6 "
            "routing rules at this time"));


    add(new validateNetwork("Validate network addresses"));
    add(new reachableAddressInRGtw(
            "Check if RGtw is reachable via local networks"));
    add(new contradictionRGtwAndRItf(
            "Check if RGtw is in a network of RItf"));

    add(new addressRangesInDst("process address ranges"));

    add(new eliminateDuplicatesInDST("Eliminate duplicates in DST"));
        
    add(new FindDefaultRoute("Find rules that install default route"));

    add(new createSortedDstIdsLabel(
            "Create label with a sorted dst-id-list for 'competingRules'"));
    add(new competingRules("Check for competing rules"));

    add(new ConvertToAtomicForDST(
            "Convert to atomic rules by dst address elements"));

    add(new createSortedDstIdsLabel(
            "Create label with a sorted dst-id-list for 'classifyRoutingRules'"));
    add(new classifyRoutingRules(
            "Classify into single path or part of a multi path rule"));
        
    add(new optimize3(
            "Eliminate duplicate rules generated from a single gui-rule"));
    add(new eliminateDuplicateRules(
            "Eliminate duplicate rules over the whole table"));
        
    add( new checkForObjectsWithErrors(
             "check if we have objects with errors in rule elements"));

    add(new PrintRule("generate ip code"));
    add(new simplePrintProgress());

    runRuleProcessors();

}

string RoutingCompiler_ipt::debugPrintRule(Rule *r)
{
    RoutingRule *rule=RoutingRule::cast(r);

    string s= RoutingCompiler::debugPrintRule(rule);

    return s;
}

void RoutingCompiler_ipt::epilog()
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
