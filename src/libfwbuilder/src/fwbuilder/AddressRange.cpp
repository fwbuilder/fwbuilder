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
{
    const AddressRange *n = dynamic_cast<const AddressRange *>(o);
    if (n==nullptr) {

        std::ostringstream s;
        s << "Attempt to copy incompatible object to AddressRange: objectID=";
        s << o->getId();

        throw(FWException(s.str()));
    }
    start_address = n->getRangeStart();
    end_address   = n->getRangeEnd();

    return FWObject::shallowDuplicate(o, preserve_id);
}

bool AddressRange::cmp(const FWObject *obj, bool recursive)
{
    if (AddressRange::constcast(obj)==nullptr) return false;
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

void AddressRange::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);
    
    const char *n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("start_address")));
    assert(n!=nullptr);
    start_address = InetAddr(AF_UNSPEC, n);
    XMLTools::FreeXmlBuff(n);

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("end_address")));
    assert(n!=nullptr);
    end_address = InetAddr(AF_UNSPEC, n);
    XMLTools::FreeXmlBuff(n);

    if (start_address.addressFamily() != end_address.addressFamily()) {

        std::ostringstream s;
        s << "AddressRange start and end address must be of same IP address family: ";
        s << "start_address: " << start_address.toString() << ", ";
        s << "end_address: " << end_address.toString();

        throw(FWException(s.str()));
    }
}

xmlNodePtr AddressRange::toXML(xmlNodePtr xml_parent_node)
{
    xmlNodePtr me = FWObject::toXML(xml_parent_node);

    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));
    
    xmlNewProp(me, 
               XMLTools::ToXmlCast("start_address"),
               XMLTools::StrToXmlCast(start_address.toString()));
    
    xmlNewProp(me, 
               XMLTools::ToXmlCast("end_address"),
               XMLTools::StrToXmlCast(end_address.toString()));
    
    return me;
}



