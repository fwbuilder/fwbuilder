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

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


/**
 *-----------------------------------------------------------------------
 *                    Methods for printing
 */

string RoutingCompiler_pix::PrintRule::_printAddr(Address  *o)
{
    std::ostringstream  ostr;

    if (Interface::cast(o)!=NULL)
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

    if (addr==NULL)
    {
        FWObject *obj=o;
/*
 * check if this is object of class Address. since we want to
 * distinguish between Host, Interface and Address, and both Host and
 * Interface are inherited from Address, we can't use cast. Use isA
 * instead
 */
        while (obj!=NULL && 
               !Host::isA(obj) && 
               !Firewall::isA(obj)  && 
               !Network::isA(obj))  obj=obj->getParent();

        compiler->abort(
            "Problem with address or netmask in the object or one "
            "of its interfaces: '" + obj->getName() + "'");
    }

    if (addr->isAny() && mask->isAny()) 
    {
        ostr << "default ";
    } else 
    {
        ostr << addr->toString();

        if (Interface::cast(o)==NULL &&
            Address::cast(o)->dimension() > 1 &&
            !mask->isHostMask())
        {
            ostr << " ";
            ostr << mask->toString();
        }

        ostr << " ";
    }
    return ostr.str();
}

RoutingCompiler_pix::PrintRule::PrintRule(const std::string &name) :
    RoutingRuleProcessor(name)
{
}

bool RoutingCompiler_pix::PrintRule::processNext()
{
    RoutingRule *rule = getNext(); 
    if (rule == NULL)
	return false;

    tmp_queue.push_back(rule);
    
    string rl = rule->getLabel();
    string comm = rule->getComment();
    string::size_type c1, c2;
    c1 = 0;
    
    if (rl != current_rule_label)
    {
            compiler->output << "! " << endl;
            compiler->output << "! Rule " << rl << endl;
            compiler->output << "! " << endl;
            compiler->output << "! \"Routing rule " << rl << "\"" << endl;
            compiler->output << "! " << endl;
    }
    
    if( rule->getRuleType() != RoutingRule::MultiPath )
    {
        if (rl != current_rule_label)
        {
            while ( (c2 = comm.find('\n',c1)) != string::npos ) {
                compiler->output << "! " << comm.substr(c1,c2-c1) << endl;
                c1 = c2 + 1;
            }
            compiler->output << "! " << comm.substr(c1) << endl;
            
            compiler->output << "! " << endl;
            
            current_rule_label=rl;
        }
        
        string command_line = RoutingRuleToString(rule);
        compiler->output << command_line;
    
    } else
    {
        throw FWException("MultiPath routing not supported by platform");
    }
    return true;
}

string RoutingCompiler_pix::PrintRule::RoutingRuleToString(RoutingRule *rule)
{
    FWObject *ref;

    RuleElementRDst *dstrel = rule->getRDst();
    ref = dstrel->front();
    Address *dst = Address::cast(FWReference::cast(ref)->getPointer());
    if(dst == NULL)
	throw FWException("Broken DST in " + rule->getLabel());
        
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

string RoutingCompiler_pix::PrintRule::_printRGtw(RoutingRule *rule)
{
    FWObject *ref;
    
    RuleElementRGtw *gtwrel = rule->getRGtw();
    ref = gtwrel->front();
    Address *gtw = Address::cast(FWReference::cast(ref)->getPointer());
    if (gtw == NULL) throw FWException("Broken GTW in " + rule->getLabel());

    string gateway = _printAddr(gtw);
    if (gateway != "default ") return gateway;
    else return " ";
}
    
string RoutingCompiler_pix::PrintRule::_printRItf(RoutingRule *rule)
{
    FWObject *ref;
    
    RuleElementRItf *itfrel = rule->getRItf();
    ref = itfrel->front();
    Interface *itf = Interface::cast(FWReference::cast(ref)->getPointer());
    
    if (itf != NULL) return itf->getLabel() + " ";
    else return "";
}

string RoutingCompiler_pix::PrintRule::_printRDst(RoutingRule *rule)
{
    FWObject *ref;
    
    RuleElementRDst *dstrel = rule->getRDst();
    ref = dstrel->front();
    Address *dst = Address::cast(FWReference::cast(ref)->getPointer());
    if (dst==NULL) throw FWException("Broken DST in " + rule->getLabel());
    
    string dest = _printAddr(dst);

    if (dest != "default ") return dest;
    else return "0.0.0.0 0.0.0.0 ";
}
