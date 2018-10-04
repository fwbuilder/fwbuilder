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

#include <QStringList>

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
string RoutingCompiler_openbsd::PrintRule::_printAddr(Address  *o)
{
    ostringstream  ostr;

    if (Interface::cast(o)!=nullptr)
    {
        Interface *iface = Interface::cast(o);
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



RoutingCompiler_openbsd::PrintRule::PrintRule(const std::string &name) :
    RoutingRuleProcessor(name) 
{ 
}


bool RoutingCompiler_openbsd::PrintRule::processNext()
{
    RoutingCompiler_openbsd *bsd_comp = 
        dynamic_cast<RoutingCompiler_openbsd*>(compiler);

    slurp();
    if (tmp_queue.size()==0) return false;


    if (!compiler->inSingleRuleCompileMode())
    {
        Configlet routing_functions(compiler->fw,
                                    compiler->fw->getStr("host_OS"),
                                    "routing_functions");

        // we should delete default route if we have a new one to
        // install. IF user did not define any routes that look like
        // default (i.e. where destination is "any"), then we should
        // preserve default so that we won't leave machine with no
        // default at all.
        QString route_pattern = "";
        if (bsd_comp->have_default_route)
        {
            // If we will install default route, delete it now
            route_pattern = "'lo0'";
        } else
        {
            // do not delete default if we won't install new one
            route_pattern = "'lo0|default'";
        }

        routing_functions.setVariable("route_filter", route_pattern);
        compiler->output << routing_functions.expand().toStdString();

        bsd_comp->defined_restore_script_output = true;
    }
    

    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        RoutingRule *rule = RoutingRule::cast( *k );

        string rl = rule->getLabel();
    
        if (!compiler->inSingleRuleCompileMode() && rl!=current_rule_label)
        {
            compiler->output << "# " << endl;
            compiler->output << "# Rule " << rl << endl;
            //compiler->output << "# " << rule->getRuleTypeAsString() << endl;
            compiler->output << "# " << endl;
            compiler->output << "echo \"Routing rule " << rl << "\"" << endl;
            compiler->output << "# " << endl;
        }
    
        if (rule->getRuleType() != RoutingRule::MultiPath )
        {
            if (!compiler->inSingleRuleCompileMode() && rl!=current_rule_label)
            {
                QStringList comment = QString::fromUtf8(
                    rule->getComment().c_str()).split("\n");
                int comment_lines = 0;
                foreach (QString str, comment)
                {
                    if (!str.isEmpty())
                    {
                        compiler->output << "# " << str.toUtf8().data() << endl;
                        ++comment_lines;
                    }
                }
                if (comment_lines) compiler->output << "#" << endl;

                string err = compiler->getErrorsForRule(rule, "# ");
                if (!err.empty()) compiler->output << err << endl;

                current_rule_label = rl;
            }
        
//            string err = rule->getCompilerMessage();
//            if (!err.empty()) compiler->output << "# " << err << endl;

            string  command_line = RoutingRuleToString(rule);
            compiler->output << command_line;
    
        }
    }
    return true;
}


string RoutingCompiler_openbsd::PrintRule::RoutingRuleToString(RoutingRule *rule,
                                                               bool add_decorations)
{
    RuleElementRDst *dstrel = rule->getRDst();
    Address *dst = Address::cast(FWReference::getObject(dstrel->front()));
    RuleElementRItf *itfrel = rule->getRItf();
    Interface *itf = Interface::cast(FWReference::getObject(itfrel->front()));
    RuleElementRGtw *gtwrel = rule->getRGtw();
    Address *gtw = Address::cast(FWReference::getObject(gtwrel->front()));

    if(dst==nullptr) compiler->abort(rule, "Broken DST");
        
    ostringstream command_line;

    command_line << "route add ";

    if (gtwrel->isAny() && itf != nullptr) command_line << "-interface ";

    command_line << _printRDst(rule);

    if (gtw != nullptr) command_line << _printRGtw(rule);
    if (itf != nullptr) command_line << _printRItf(rule);

    // to make generated script more readable in single rule compile mode,
    // skip the part that rolls back in case of an error
    if (!compiler->inSingleRuleCompileMode() && add_decorations)
    {
        command_line << "|| route_command_error "
                     << "\"" << rule->getLabel() << "\"" << endl;;
    }
    command_line << endl;

    return command_line.str();
}

string RoutingCompiler_openbsd::PrintRule::_printRGtw(RoutingRule *rule)
{
    RuleElementRGtw *gtwrel = rule->getRGtw();
    Address *gtw = Address::cast(FWReference::getObject(gtwrel->front()));
    if(gtw==nullptr)
        compiler->abort(rule, "Broken GTW");
    
    string gateway = _printAddr(gtw);
    
    if( gateway != "default ") return gateway;
    else return "";
}
    
string RoutingCompiler_openbsd::PrintRule::_printRItf(RoutingRule *rule)
{
    RuleElementRItf *itfrel = rule->getRItf();
    Interface *itf = Interface::cast(FWReference::getObject(itfrel->front()));
    if(itf != nullptr)
    {
        IPv4 *addr = IPv4::cast(itf->getFirstByType(IPv4::TYPENAME));
        if (addr == nullptr)
        {
            QString err("Can not configure static route via interface %1 "
                        "because its address is unknown");
            compiler->abort(rule, err.arg(itf->getName().c_str()).toStdString());
        }
        const InetAddr* ia = addr->getAddressPtr();
        return ia->toString();
    }
    else return "";
}

string RoutingCompiler_openbsd::PrintRule::_printRDst(RoutingRule *rule)
{
    RuleElementRDst *dstrel = rule->getRDst();
    Address *dst = Address::cast(FWReference::getObject(dstrel->front()));
    if(dst==nullptr)
        compiler->abort(rule, "Broken DST");
    
    return _printAddr(dst);
}
