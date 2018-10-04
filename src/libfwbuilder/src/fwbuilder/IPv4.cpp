/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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
  Class IPv4 serves two purposes:

   - it is used to describe configuration of an interface which consists
     of an address and netmask

   - it is used to describe a single standalone address object (in the tree,
     under Objects/Addresses)

  Even though class Network also has address and netmask, IPv4 objects are
  recognized by compilers as single addresses.

 */


#include <assert.h>
#include <iostream>
#include <cstring>
#include <stdexcept>

#include "fwbuilder/IPv4.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/XMLTools.h"

using namespace std;
using namespace libfwbuilder;

const char *IPv4::TYPENAME={"IPv4"};

IPv4::IPv4() : Address()
{
}

IPv4::~IPv4()
{
}

void IPv4::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);

    const char* n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("address")));
    assert(n!=nullptr);

    // strip whitespace and other non-numeric characters at the beginng and end
    string addr(n);
    string::size_type first = addr.find_first_of("0123456789");
    string::size_type last = addr.find_last_of("0123456789");
    try {
        addr = addr.substr(first, last-first+1);
    } catch (std::out_of_range &ex)
    {
        cerr << "Object \"" << getName() << "\": Invalid address" << n << endl;
        addr = "0.0.0.0";
    }
    setAddress(InetAddr(addr));
    XMLTools::FreeXmlBuff(n);

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("netmask")));
    assert(n!=nullptr);

    string netm(n);
    first = netm.find_first_of("0123456789");
    last = netm.find_last_of("0123456789");
    try {
        netm = netm.substr(first, last-first+1);
    } catch (std::out_of_range &ex)
    {
        cerr << "Object \"" << getName() << "\": Invalid netmask" << n << endl;
        addr = "0.0.0.0";
    }

    if (!netm.empty()) setNetmask(InetAddr(netm));
    else               setNetmask(InetAddr(0));
    XMLTools::FreeXmlBuff(n);
}

xmlNodePtr IPv4::toXML(xmlNodePtr xml_parent_node)
{
    if (getName().empty()) setName(getTypeName());

    xmlNodePtr me = FWObject::toXML(xml_parent_node);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));
    
    xmlNewProp(me, 
               XMLTools::ToXmlCast("address"),
               XMLTools::StrToXmlCast(inet_addr_mask->getAddressPtr()->toString()));
    
    xmlNewProp(me, 
               XMLTools::ToXmlCast("netmask"),
               XMLTools::StrToXmlCast(inet_addr_mask->getNetmaskPtr()->toString()));
    
    return me;
}

void IPv4::setAddress(const InetAddr &a)
{
    inet_addr_mask->setAddress(a);
}

void IPv4::setNetmask(const InetAddr &nm)
{
    inet_addr_mask->setNetmask(nm);
}

void IPv4::setAddressNetmask(const std::string& s)
{
    delete inet_addr_mask;
    inet_addr_mask = new InetAddrMask(s);
}

void IPv4::dump(std::ostream &f,bool recursive,bool brief,int offset) const
{
    FWObject::dump(f, recursive, brief, offset);
    f << inet_addr_mask->getAddressPtr()->toString() << endl;
}

bool IPv4::isPrimaryObject() const
{
    return (!Interface::isA(getParent()));
}

