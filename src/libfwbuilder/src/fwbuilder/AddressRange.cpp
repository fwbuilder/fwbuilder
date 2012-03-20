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

#include "config.h"
#include "fwbuilder/libfwbuilder-config.h"


#include "fwbuilder/AddressRange.h"
#include "fwbuilder/XMLTools.h"

using namespace libfwbuilder;

const char *AddressRange::TYPENAME={"AddressRange"};

/**
 * Empty constructor. Make sure start and end addresses are
 * initialized to 0.
 */
AddressRange::AddressRange() :
    Address(),
    start_address(InetAddr::getAny()), end_address(InetAddr::getAny())
{
    Address::setNetmask(InetAddr::getAny());
}

AddressRange::AddressRange(AddressRange &o) : Address() , 
                                              start_address(o.getRangeStart()), 
                                              end_address(o.getRangeEnd()) 
{
    FWObject::operator=(o);
}

const InetAddr* AddressRange::getAddressPtr() const
{
    return &start_address;
}

unsigned int AddressRange::dimension()  const
{
    return start_address.distance(end_address);
}

void AddressRange::setAddress(const InetAddr &a)  
{
    setRangeStart(a);
    setRangeEnd(a);
}

void AddressRange::setNetmask(const InetAddr& ) {}

FWObject& AddressRange::shallowDuplicate(const FWObject *o, bool preserve_id)
    throw(FWException)
{
    const AddressRange *n = dynamic_cast<const AddressRange *>(o);
    if (n==NULL)
        throw(FWException(
"Attempt to copy incompatible object to AddressRange: objectID="+o->getId()));

    start_address = n->getRangeStart();
    end_address   = n->getRangeEnd();

    return FWObject::shallowDuplicate(o, preserve_id);
}

bool AddressRange::cmp(const FWObject *obj, bool recursive) throw(FWException)
{
    if (AddressRange::constcast(obj)==NULL) return false;
    if (!FWObject::cmp(obj, recursive)) return false;

    InetAddr o1b;
    InetAddr o1e;
    InetAddr o2b;
    InetAddr o2e;

    o1b = getRangeStart();  
    o1e = getRangeEnd();

    o2b = AddressRange::constcast(obj)->getRangeStart();  
    o2e = AddressRange::constcast(obj)->getRangeEnd();

    return (o1b==o2b && o1e==o2e);
}

void AddressRange::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);
    
    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("start_address")));
    assert(n!=NULL);
    start_address = InetAddr(n);
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("end_address")));
    assert(n!=NULL);
    end_address = InetAddr(n);
    FREEXMLBUFF(n);
}

xmlNodePtr AddressRange::toXML(xmlNodePtr xml_parent_node) throw(FWException)
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



