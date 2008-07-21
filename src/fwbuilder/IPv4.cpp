/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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
#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/IPv4.h>
#include <fwbuilder/InterfacePolicy.h>
#include <fwbuilder/XMLTools.h>

using namespace std;
using namespace libfwbuilder;

const char *IPv4::TYPENAME={"IPv4"};

IPv4::IPv4() : Address()
{
}

IPv4::IPv4(const FWObject *root, bool prepopulate) : Address(root, prepopulate)
{
}

IPv4::~IPv4()
{
}

void IPv4::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);

    const char* n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("address")));
    assert(n!=NULL);
    setAddress(InetAddr(n));
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("netmask")));
    assert(n!=NULL);
    if (strlen(n)) setNetmask(InetAddr(n));
    else           setNetmask(InetAddr(0));
    FREEXMLBUFF(n);
}

xmlNodePtr IPv4::toXML(xmlNodePtr xml_parent_node) throw(FWException)
{
    if (getName().empty()) setName(getTypeName());

    xmlNodePtr me = FWObject::toXML(xml_parent_node);
    
    xmlNewProp(me, 
               TOXMLCAST("address"),
               STRTOXMLCAST(inet_addr_mask->getAddressPtr()->toString()));
    
    xmlNewProp(me, 
               TOXMLCAST("netmask"),
               STRTOXMLCAST(inet_addr_mask->getNetmaskPtr()->toString()));
    
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

void IPv4::dump(std::ostream &f,bool recursive,bool brief,int offset)
{
    FWObject::dump(f, recursive, brief, offset);
    f << inet_addr_mask->getAddressPtr()->toString() << endl;
}
