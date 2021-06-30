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

#include <assert.h>



#include "fwbuilder/Network.h"
#include "fwbuilder/XMLTools.h"

#include <string>


using namespace libfwbuilder;
using namespace std;


const char *Network::TYPENAME={"Network"};

Network::Network() : Address()
{
    setNetmask(InetAddr(AF_INET, 32));
}

Network::Network(Network &o) : Address(o)
{
    FWObject::operator=(o);
    setAddress(*(o.getAddressPtr()));
    setNetmask(*(o.getNetmaskPtr()));
}

Network::Network (const string &s) : Address()
{
    setAddressNetmask(s);
}
                                     
Network::~Network() {}

void Network::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);
    
    const char *n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("address")));
    assert(n!=nullptr);
    setAddress(InetAddr(n));
    XMLTools::FreeXmlBuff(n);

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("netmask")));
    assert(n!=nullptr);
    setNetmask(InetAddr(n));
    XMLTools::FreeXmlBuff(n);
}

xmlNodePtr Network::toXML(xmlNodePtr xml_parent_node)
{
    if (getName().empty()) setName(getTypeName());

    xmlNodePtr me = FWObject::toXML(xml_parent_node);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));
    
    xmlNewProp(me, 
               XMLTools::ToXmlCast("address"),
               XMLTools::StrToXmlCast(getAddressPtr()->toString()));
    
    xmlNewProp(me, 
               XMLTools::ToXmlCast("netmask"),
               XMLTools::StrToXmlCast(getNetmaskPtr()->toString()));
    
    return me;
}

/* check if host address bits are cleared */
bool Network::isValidRoutingNet() const
{
    return (*(getAddressPtr()) == *(getNetworkAddressPtr()));
}

void Network::setAddress(const InetAddr &a)
{
    inet_addr_mask->setAddress(a);
}

void Network::setNetmask(const InetAddr &nm)
{
    inet_addr_mask->setNetmask(nm);
}

void Network::setAddressNetmask(const std::string& s)
{
    delete inet_addr_mask;
    inet_addr_mask = new InetAddrMask(s);
}

const InetAddr* Network::getFirstHostPtr() const
{
    const InetAddrMask *inet_addr_mask = getInetAddrMaskObjectPtr();
    if (inet_addr_mask) return inet_addr_mask->getFirstHostPtr();
    return nullptr;
}

const InetAddr* Network::getLastHostPtr() const
{
    const InetAddrMask *inet_addr_mask = getInetAddrMaskObjectPtr();
    if (inet_addr_mask) return inet_addr_mask->getLastHostPtr();
    return nullptr;
}
