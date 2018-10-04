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

#include "RoutingCompiler_cisco.h"
#include "NamedObjectsAndGroupsSupport.h"
#include "NamedObjectsManager.h"

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
using namespace std;


int RoutingCompiler_cisco::prolog()
{
    return RoutingCompiler::prolog();    
}

/*
 *  this processor eliminates duplicate atomic routing rules in one
 *  routing table
 */
bool RoutingCompiler_cisco::eliminateDuplicateRules::processNext()
{
    RoutingCompiler_cisco *cisco_comp =
        dynamic_cast<RoutingCompiler_cisco*>(compiler);

    RoutingRule *rule = getNext();
    if (rule == nullptr) return false;

    if (rule->isFallback() || rule->isHidden())
    {
        tmp_queue.push_back(rule);
        return true;
    }

    string label = rule->getLabel();
    int bracepos = label.find("(");
    label.erase(0, bracepos);
    
    string thisRule = label + " " +
        cisco_comp->printRule->RoutingRuleToString(rule);
    
    rules_it = rules_seen_so_far.find(thisRule);
            
    if (rules_it != rules_seen_so_far.end())
    {
        string msg;
        msg = "Two of the sub rules created from the gui routing rules " +
            rules_it->second + " and " + rule->getLabel() +
            " are identical, skipping the second. " +
            "Revise them to avoid this warning";
        compiler->warning(rule,  msg.c_str() );
        return true;
    }

    tmp_queue.push_back(rule);
    rules_seen_so_far[thisRule] = rule->getLabel();

    return true;
}

void RoutingCompiler_cisco::compile()
{
    printRule = new RoutingCompiler_cisco::PrintRule("");
}

string RoutingCompiler_cisco::debugPrintRule(Rule *r)
{
    RoutingRule *rule = RoutingRule::cast(r);

    string s = RoutingCompiler::debugPrintRule(rule);

    return s;
}

void RoutingCompiler_cisco::setNamedObjectsManager(NamedObjectsManager *mgr)
{
    named_objects_manager = mgr;
}
