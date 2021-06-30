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

RoutingCompiler_cisco::PrintRule::PrintRule(const std::string &name) :
    RoutingRuleProcessor(name)
{
}

bool RoutingCompiler_cisco::PrintRule::processNext()
{
    RoutingRule *rule = getNext(); 
    if (rule == nullptr) return false;

    tmp_queue.push_back(rule);
    
    return true;
}

string RoutingCompiler_cisco::PrintRule::_printAddr(Address  *o)
{
    std::ostringstream  ostr;

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
            "Problem with address or netmask in the object or one "
            "of its interfaces: '" + obj->getName() + "'");
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
            ostr << " ";
            ostr << mask->toString();
        }

        ostr << " ";
    }
    return ostr.str();
}

string RoutingCompiler_cisco::PrintRule::RoutingRuleToString(RoutingRule*)
{
    return "";
}

string RoutingCompiler_cisco::PrintRule::_printRGtw(RoutingRule *rule)
{
    FWObject *ref;
    
    RuleElementRGtw *gtwrel = rule->getRGtw();
    ref = gtwrel->front();
    Address *gtw = Address::cast(FWReference::cast(ref)->getPointer());
    if (gtw == nullptr) compiler->abort(rule, "Broken GTW");

    string gateway = _printAddr(gtw);
    if (gateway != "default ") return gateway;
    else return " ";
}
    
string RoutingCompiler_cisco::PrintRule::_printRItf(RoutingRule *rule)
{
    FWObject *ref;
    
    RuleElementRItf *itfrel = rule->getRItf();
    ref = itfrel->front();
    Interface *itf = Interface::cast(FWReference::cast(ref)->getPointer());
    
    if (itf != nullptr) return itf->getLabel() + " ";
    else return "";
}

string RoutingCompiler_cisco::PrintRule::_printRDst(RoutingRule *rule)
{
    FWObject *ref;
    
    RuleElementRDst *dstrel = rule->getRDst();
    ref = dstrel->front();
    Address *dst = Address::cast(FWReference::cast(ref)->getPointer());
    if (dst==nullptr) compiler->abort(rule, "Broken DST");
    
    string dest = _printAddr(dst);

    if (dest != "default ") return dest;
    else return "0.0.0.0 0.0.0.0 ";
}
