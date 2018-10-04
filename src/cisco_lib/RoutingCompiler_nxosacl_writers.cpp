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
RoutingCompiler_nxosacl::PrintRule::PrintRule(const std::string &name) :
    RoutingCompiler_cisco::PrintRule(name)
{
}

bool RoutingCompiler_nxosacl::PrintRule::processNext()
{
    RoutingRule *rule = getNext(); if (rule == nullptr) return false;
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

string RoutingCompiler_nxosacl::PrintRule::_printRGtw(RoutingRule *rule)
{
    FWObject *ref;
    
    RuleElementRGtw *gtwrel = rule->getRGtw();
    ref = gtwrel->front();
    Address *gtw = Address::cast(FWReference::cast(ref)->getPointer());

    if (Interface::isA(gtw) && gtw->isChildOf(compiler->fw))
    {
        // gateway is interface of this firewall. Generate command
        // ip route A.B.C.D N.N.N.N interface metric
        return gtw->getName() + " ";
    }
        
    string gateway = _printAddr(gtw);
    if (gateway != "default ") return gateway;
    else return " ";
}
    
string RoutingCompiler_nxosacl::PrintRule::_printRItf(RoutingRule *rule)
{
    RuleElementRItf *itfrel = rule->getRItf();
    if (!itfrel->isAny())
    {
        Interface *itf =
            Interface::cast(FWObjectReference::getObject(itfrel->front()));
        if (itf != nullptr) return itf->getName() + " ";
    }
    return "";
}

string RoutingCompiler_nxosacl::PrintRule::RoutingRuleToString(RoutingRule *rule)
{
    FWObject *ref;

    RuleElementRDst *dstrel = rule->getRDst();
    ref = dstrel->front();
    Address *dst = Address::cast(FWReference::cast(ref)->getPointer());
    if(dst == nullptr) compiler->abort(rule, "Broken DST");

    std::ostringstream command_line;

    command_line << "ip route ";
    command_line << _printRDst(rule);
    command_line << _printRGtw(rule);
    command_line << _printRItf(rule);

    // default metric in NXOS is 1 (can't have metric 0)
    if (rule->getMetricAsString() == "0")
    {
        command_line << "1";
    } else {
        command_line << rule->getMetricAsString();
    }

    command_line << endl;

    return command_line.str();
}
