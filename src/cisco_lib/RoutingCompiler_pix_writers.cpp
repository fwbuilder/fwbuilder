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

#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Network.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


/**
 *-----------------------------------------------------------------------
 *                    Methods for printing
 */
RoutingCompiler_pix::PrintRule::PrintRule(const std::string &name) :
    RoutingCompiler_cisco::PrintRule(name)
{
}

bool RoutingCompiler_pix::PrintRule::processNext()
{
    RoutingRule *rule = getNext(); 
    if (rule == nullptr) return false;

    tmp_queue.push_back(rule);
    
    string rl = rule->getLabel();
    string comm = rule->getComment();
    string::size_type c1, c2;
    c1 = 0;
    
    if (!compiler->inSingleRuleCompileMode() && rl != current_rule_label)
    {
        compiler->output << "! " << endl;
        compiler->output << "! Rule " << rl << endl;
        compiler->output << "! " << endl;
        compiler->output << "! \"Routing rule " << rl << "\"" << endl;
        compiler->output << "! " << endl;
    }
    
//    string err = rule->getCompilerMessage();
//    if (!err.empty()) compiler->output << "# " << err << endl;

    if( rule->getRuleType() != RoutingRule::MultiPath )
    {
        if (!compiler->inSingleRuleCompileMode() && rl != current_rule_label)
        {
            while ( (c2 = comm.find('\n',c1)) != string::npos )
            {
                compiler->output << "! " << comm.substr(c1,c2-c1) << endl;
                c1 = c2 + 1;
            }
            compiler->output << "! " << comm.substr(c1) << endl;
            compiler->output << "! " << endl;

            string err = compiler->getErrorsForRule(rule, "! ");
            if (!err.empty()) compiler->output << err << endl;

            current_rule_label = rl;
        }
        
        string command_line = RoutingRuleToString(rule);
        compiler->output << command_line;
    
    } else
    {
        string err = compiler->getErrorsForRule(rule, "! ");
        if (!err.empty()) compiler->output << err << endl;
            
        compiler->abort(rule, "MultiPath routing not supported by platform");
    }
    return true;
}

string RoutingCompiler_pix::PrintRule::RoutingRuleToString(RoutingRule *rule)
{
    FWObject *ref;

    RuleElementRDst *dstrel = rule->getRDst();
    ref = dstrel->front();
    Address *dst = Address::cast(FWReference::cast(ref)->getPointer());
    if(dst == nullptr) compiler->abort(rule, "Broken DST");

    std::ostringstream command_line;

    command_line << "route ";
    command_line << _printRItf(rule);
    command_line << _printRDst(rule);
    command_line << _printRGtw(rule);
    // default metric in PIX is 1
    if (rule->getMetricAsString() == "0") {
        command_line << "1";
    } else {
        command_line << rule->getMetricAsString();
    }

    command_line << endl;

    return command_line.str();
}
