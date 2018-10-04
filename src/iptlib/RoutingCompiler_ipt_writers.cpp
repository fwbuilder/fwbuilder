/* 

                  Firewall Builder Routing add-on

                 Copyright (C) 2004 Compal GmbH, Germany

  Author:  Tidei Maurizio     <fwbuilder-routing at compal.de>
  Modified: Vadim Kurland     vadim@vk.crocodile.org

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
#include "Configlet.h"

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
#include "fwbuilder/Resources.h"

#include <QString>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;



/**
 *-----------------------------------------------------------------------
 *                    Methods for printing
 */
string RoutingCompiler_ipt::PrintRule::_printAddr(Address  *o)
{
    ostringstream  ostr;

    if (Interface::cast(o)!=nullptr)
    {
        Interface *iface=Interface::cast(o);
        if (iface->isDyn())
            ostr << "$interface_" << iface->getName() << " ";
        return ostr.str();
    }

    const InetAddr *addr;
    const InetAddr *mask;
    addr = o->getAddressPtr();
    mask = o->getNetmaskPtr();

    if (addr==nullptr)
    {
        FWObject *obj=o;
/*
 * check if this is object of class Address. since we want to
 * distinguish between Host, Interface and Address, and both Host and
 * Interface are inherited from Address, we can't use cast. Use isA
 * instead
 */
        while (obj!=nullptr && 
               !Host::isA(obj) && 
               !Firewall::isA(obj)  && 
               !Network::isA(obj))  obj=obj->getParent();

        compiler->abort(
            "Problem with address or netmask in the object or "
            "one of its interfaces: '" + obj->getName() + "'");
    }


    if (addr->isAny() && mask->isAny()) 
    {
        ostr << "default ";
    } else 
    {
        ostr << addr->toString();

        if (Interface::cast(o)==nullptr &&
            Address::cast(o)->dimension() > 1 &&
            !mask->isHostMask())
        {
            ostr << "/" << mask->getLength();
        }
        ostr << " ";
    }
    return ostr.str();
}



RoutingCompiler_ipt::PrintRule::PrintRule(const std::string &name) : RoutingRuleProcessor(name) 
{ 
    print_once_on_top=true;
}


bool RoutingCompiler_ipt::PrintRule::processNext()
{
    RoutingCompiler_ipt *ipt_comp = dynamic_cast<RoutingCompiler_ipt*>(compiler);
    RoutingRule *rule = getNext(); 
    if (rule==nullptr) return false;

    tmp_queue.push_back(rule);
    
    if (print_once_on_top && !compiler->inSingleRuleCompileMode())
    {
        // string os_family = Resources::os_res[compiler->fw->getStr("host_OS")]->
        //     getResourceStr("/FWBuilderResources/Target/family");
        Configlet routing_functions(compiler->fw, "linux24", "routing_functions");

        // we should delete default route if we have a new one to
        // install. IF user did not define any routes that look like
        // default (i.e. where destination is "any"), then we should
        // preserve default so that we won't leave machine with no
        // default at all.
        QString route_pattern = "";
        if (ipt_comp->have_default_route)
        {
            // If we will install default route, delete it now
            route_pattern = "'proto kernel'";
        } else
        {
            // do not delete default if we won't install new one
            route_pattern = "'\\( proto kernel \\)\\|\\(default via \\)'";
        }

        routing_functions.setVariable("proto_filter", route_pattern);
        compiler->output << routing_functions.expand().toStdString();

        ipt_comp->defined_restore_script_output = true;
        print_once_on_top = false;
    }
    
    // TODO: convert this into virtual function RoutingCompiler_ipt::printComment()
    string rl = rule->getLabel();
    string comm = rule->getComment();
    string::size_type c1,c2;
    c1=0;
    
    if (!compiler->inSingleRuleCompileMode() && rl!=current_rule_label)
    {
        compiler->output << "# " << endl;
        compiler->output << "# Rule " << rl << endl;
        //compiler->output << "# " << rule->getRuleTypeAsString() << endl;
        compiler->output << "# " << endl;
        compiler->output << "echo \"Routing rule " << rl << "\"" << endl;
        compiler->output << "# " << endl;
    }
    
    if( rule->getRuleType() != RoutingRule::MultiPath )
    {
        if (!compiler->inSingleRuleCompileMode() && rl!=current_rule_label)
        {
            while ( (c2=comm.find('\n',c1))!=string::npos )
            {
                compiler->output << "# " << comm.substr(c1,c2-c1) << endl;
                c1=c2+1;
            }
            compiler->output << "# " << comm.substr(c1) << endl;
            compiler->output << "# " << endl;

            string err = compiler->getErrorsForRule(rule, "# ");
            if (!err.empty()) compiler->output << err << endl;

            current_rule_label = rl;
        }
        
//        string err = rule->getCompilerMessage();
//        if (!err.empty()) compiler->output << "# " << err << endl;

        string  command_line = RoutingRuleToString(rule);
        compiler->output << command_line;
    
    } else
    {
        // the ecmp_id contains the table, the rule label and the metric. These are the properties the ecmp rules are distinguished
        string metric = rule->getMetricAsString();
        string ecmp_id = rule->getSortedDstIds() + "#" + metric;
                    
        if (rl!=current_rule_label)
        {
            compiler->output << "# Some sub rules belonging to an ECMP (Equal Cost Multi Path) rule were placed in the ECMP section below." << endl;
            current_rule_label=rl;
        }
        
        map< string, string>& ecmp_rules_buffer =
            ((RoutingCompiler_ipt*)compiler)->ecmp_rules_buffer;
        map< string, string>& ecmp_comments_buffer =
            ((RoutingCompiler_ipt*)compiler)->ecmp_comments_buffer;

        map< string, string>::iterator ecmp_rules_buffer_it;
        ecmp_rules_buffer_it = ecmp_rules_buffer.find(ecmp_id);
        if( ecmp_rules_buffer_it == ecmp_rules_buffer.end() )
        {
            // ECMP Dst not seen so far, add "ip route add x.x.x.x"
            // and comment's header
            ecmp_comments_buffer[ecmp_id] = "#\n# Multipath Rule "
                " derived from the following routing rules:\n#\n";
            
            if (rule->getMetricAsString() != "0")
            {
                ecmp_rules_buffer[ecmp_id] += "$IP route add " +
                    _printRDst(rule) + "metric " + metric;
            } else
            {
                ecmp_rules_buffer[ecmp_id] += "$IP route add " +
                    _printRDst(rule);
            }
        }
        
        ecmp_comments_buffer[ecmp_id] += "# Rule " + rl + "\n";
        
        while ( (c2=comm.find('\n',c1))!=string::npos )
        {
            ecmp_comments_buffer[ecmp_id] +=
                "#    " + comm.substr(c1,c2-c1) + "\n";
            c1=c2+1;
        }
        ecmp_comments_buffer[ecmp_id] += "#    " + comm.substr(c1) + "\n";
        
        // Collect the nexthop information for the multipath rules, indexed by the sorted dst ids
        ecmp_rules_buffer[ecmp_id] += " \\\nnexthop " ;
        ecmp_rules_buffer[ecmp_id] += _printRGtw(rule);
        ecmp_rules_buffer[ecmp_id] += _printRItf(rule);
        
        
    }
    return true;
}


string RoutingCompiler_ipt::PrintRule::RoutingRuleToString(RoutingRule *rule)
{
    FWObject    *ref;

    RuleElementRDst *dstrel=rule->getRDst();
    ref=dstrel->front();
    Address *dst=Address::cast(FWReference::cast(ref)->getPointer());
    if(dst==nullptr)
        compiler->abort(rule, "Broken DST");
        
    ostringstream command_line;

    command_line << "$IP route add ";
    command_line << _printRDst(rule);
    if (rule->getMetricAsString() != "0")
    {
        command_line << " metric " << rule->getMetricAsString() << " ";
    }
    command_line << _printRGtw(rule);
    command_line << _printRItf(rule);

    // to make generated script more readable in single rule compile mode,
    // skip the part that rolls back in case of an error
    if (!compiler->inSingleRuleCompileMode())
    {
        command_line << "\\\n|| ";

        FWObject *opt_dummy = rule->getFirstByType(RoutingRuleOptions::TYPENAME);
        RoutingRuleOptions *opt = opt_dummy ? RoutingRuleOptions::cast(opt_dummy) : 0;
        if ( opt && opt->getBool("no_fail") )
        {
            command_line << "echo \"*** Warning: routing rule "
                         << rule->getLabel() << " failed. ignored. ***\"\n";
        } else
        {
            command_line << "route_command_error "
                         << "\"" << rule->getLabel() << "\"" << endl;;
        }
    }
    command_line << endl;

    return command_line.str();
}

string RoutingCompiler_ipt::PrintRule::_printRGtw(RoutingRule *rule)
{
    FWObject    *ref;
    
    RuleElementRGtw *gtwrel = rule->getRGtw();
    ref = gtwrel->front();
    Address *gtw = Address::cast(FWReference::cast(ref)->getPointer());
    if(gtw==nullptr)
        compiler->abort(rule, "Broken GTW");
    
    string gateway = _printAddr(gtw);
    
    if( gateway != "default ") return "via " + gateway;
    else return "";
}
    
string RoutingCompiler_ipt::PrintRule::_printRItf(RoutingRule *rule)
{
    FWObject    *ref;
    
    RuleElementRItf *itfrel=rule->getRItf();
    ref=itfrel->front();
    Interface *itf=Interface::cast(FWReference::cast(ref)->getPointer());
    
    if(itf != nullptr) return "dev " + itf->getName() + " ";
    else return "";
}

string RoutingCompiler_ipt::PrintRule::_printRDst(RoutingRule *rule)
{
    FWObject    *ref;
    
    RuleElementRDst *dstrel=rule->getRDst();
    ref=dstrel->front();
    Address *dst=Address::cast(FWReference::cast(ref)->getPointer());
    if(dst==nullptr)
        compiler->abort(rule, "Broken DST");
    
    return _printAddr(dst);
}
