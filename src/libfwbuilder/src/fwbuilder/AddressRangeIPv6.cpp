/* 

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include <assert.h>
#include <iostream>
#include <sstream>

#include "config.h"
#include "fwbuilder/libfwbuilder-config.h"

#include "fwbuilder/Inet6AddrMask.h"
#include "fwbuilder/AddressRangeIPv6.h"
#include "fwbuilder/XMLTools.h"

using namespace libfwbuilder;

const char *AddressRangeIPv6::TYPENAME={"AddressRangeIPv6"};

/**
 * Empty constructor. Make sure start and end addresses are
 * initialized to 0.
 */
AddressRangeIPv6::AddressRangeIPv6() : Address()
{
    delete inet_addr_mask;
    inet_addr_mask = new Inet6AddrMask();
    setAddress(InetAddr(AF_INET6, "::1"));
    setNetmask(InetAddr(AF_INET6, 128));
    Address::setNetmask(InetAddr(AF_INET6, 128));
}

AddressRangeIPv6::AddressRangeIPv6(AddressRangeIPv6 &o) : Address() , 
                                              start_address(o.getRangeStart()), 
                                              end_address(o.getRangeEnd()) 
{
    FWObject::operator=(o);
}

const InetAddr* AddressRangeIPv6::getAddressPtr() const
{
    return &start_address;
}

unsigned int AddressRangeIPv6::dimension()  const
{
    return start_address.distance(end_address);
}

void AddressRangeIPv6::setAddress(const InetAddr &a)  
{
    setRangeStart(a);
    setRangeEnd(a);
}

void AddressRangeIPv6::setNetmask(const InetAddr& ) {}

FWObject& AddressRangeIPv6::shallowDuplicate(const FWObject *o, bool preserve_id) throw(FWException)
{
    const AddressRangeIPv6 *n = dynamic_cast<const AddressRangeIPv6 *>(o);
    if (n==NULL) {

        std::ostringstream s;
        s << "Attempt to copy incompatible object to AddressRangeIPv6: objectID=";
        s << o->getId();

        throw(FWException(s.str()));
    }
    start_address = n->getRangeStart();
    end_address   = n->getRangeEnd();

    return FWObject::shallowDuplicate(o, preserve_id);
}

bool AddressRangeIPv6::cmp(const FWObject *obj, bool recursive) throw(FWException)
{
    if (AddressRangeIPv6::constcast(obj)==NULL) return false;
    if (!FWObject::cmp(obj, recursive)) return false;

    InetAddr o1b;
    InetAddr o1e;
    InetAddr o2b;
    InetAddr o2e;

    o1b = getRangeStart();  
    o1e = getRangeEnd();

    o2b = AddressRangeIPv6::constcast(obj)->getRangeStart();  
    o2e = AddressRangeIPv6::constcast(obj)->getRangeEnd();

    return (o1b==o2b && o1e==o2e);
}

void AddressRangeIPv6::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);
    
    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("start_address")));
    assert(n!=NULL);
    start_address = InetAddr(AF_INET6,n);
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("end_address")));
    assert(n!=NULL);
    end_address = InetAddr(AF_INET6,n);
    FREEXMLBUFF(n);
}

xmlNodePtr AddressRangeIPv6::toXML(xmlNodePtr xml_parent_node) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(xml_parent_node);

    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));
    
    xmlNewProp(me, 
               TOXMLCAST("start_address"),
               STRTOXMLCAST(start_address.toString()));
    
    xmlNewProp(me, 
               TOXMLCAST("end_address"),
               STRTOXMLCAST(end_address.toString()));
    
    return me;
}



