/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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

/*
  Class IPv6 serves two purposes:

   - it is used to describe configuration of an interface which consists
     of an address and netmask

   - it is used to describe a single standalone address object (in the tree,
     under Objects/Addresses)

  Even though class Network also has address and netmask, IPv6 objects are
  recognized by compilers as single addresses.

 */


#include <assert.h>
#include <iostream>
#include <sstream>
#include <cstring>

#include "fwbuilder/Inet6AddrMask.h"

#include "fwbuilder/IPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/XMLTools.h"

using namespace std;
using namespace libfwbuilder;

const char *IPv6::TYPENAME={"IPv6"};

IPv6::IPv6() : Address()
{
    delete inet_addr_mask;
    inet_addr_mask = new Inet6AddrMask();
}

IPv6::~IPv6()
{
}

FWObject& IPv6::shallowDuplicate(const FWObject *other,
                                 bool preserve_id)
{
    const IPv6* a_other = IPv6::constcast(other);
    assert(a_other);
    delete inet_addr_mask;
    inet_addr_mask = new Inet6AddrMask(
        *(dynamic_cast<Inet6AddrMask*>(a_other->inet_addr_mask)));
    return FWObject::shallowDuplicate(other, preserve_id);
}

void IPv6::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);

    const char* n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("address")));
    assert(n!=nullptr);
    setAddress(InetAddr(AF_INET6, n));
    XMLTools::FreeXmlBuff(n);

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("netmask")));
    assert(n!=nullptr);
    if (strlen(n))
    {
        if (string(n).find(":")!=string::npos)
        {
            setNetmask(InetAddr(AF_INET6, n));
        } else
        {
            istringstream str(n);
            int netm;
            str >> netm;
            setNetmask(InetAddr(AF_INET6, netm));
        }
    } else setNetmask(InetAddr(AF_INET6, 0));
    XMLTools::FreeXmlBuff(n);
}

xmlNodePtr IPv6::toXML(xmlNodePtr xml_parent_node)
{
    if (getName().empty()) setName(getTypeName());

    xmlNodePtr me = FWObject::toXML(xml_parent_node);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));
    
    xmlNewProp(me, 
               XMLTools::ToXmlCast("address"),
               XMLTools::StrToXmlCast(inet_addr_mask->getAddressPtr()->toString()));

    // Save netmask as bit length
    ostringstream str;
    str << inet_addr_mask->getNetmaskPtr()->getLength();
    xmlNewProp(me, XMLTools::ToXmlCast("netmask"), XMLTools::StrToXmlCast(str.str()));
    
    return me;
}

void IPv6::setAddress(const InetAddr &a)
{
    inet_addr_mask->setAddress(a);
}

void IPv6::setNetmask(const InetAddr &nm)
{
    inet_addr_mask->setNetmask(nm);
}

void IPv6::setAddressNetmask(const std::string& s)
{
    delete inet_addr_mask;
    inet_addr_mask = new Inet6AddrMask(s);
}

void IPv6::dump(std::ostream &f,bool recursive,bool brief,int offset) const
{
    FWObject::dump(f, recursive, brief, offset);
    f << inet_addr_mask->getAddressPtr()->toString() << endl;
}

bool IPv6::isPrimaryObject() const
{
    return (!Interface::isA(getParent()));
}

