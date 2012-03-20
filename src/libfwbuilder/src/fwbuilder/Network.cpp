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

#include "config.h"
#include "fwbuilder/libfwbuilder-config.h"


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

void Network::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);
    
    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("address")));
    assert(n!=NULL);
    setAddress(InetAddr(n));
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("netmask")));
    assert(n!=NULL);
    setNetmask(InetAddr(n));
    FREEXMLBUFF(n);
}

xmlNodePtr Network::toXML(xmlNodePtr xml_parent_node) throw(FWException)
{
    if (getName().empty()) setName(getTypeName());

    xmlNodePtr me = FWObject::toXML(xml_parent_node);
    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));
    
    xmlNewProp(me, 
               TOXMLCAST("address"),
               STRTOXMLCAST(getAddressPtr()->toString()));
    
    xmlNewProp(me, 
               TOXMLCAST("netmask"),
               STRTOXMLCAST(getNetmaskPtr()->toString()));
    
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
    return NULL;
}

const InetAddr* Network::getLastHostPtr() const
{
    const InetAddrMask *inet_addr_mask = getInetAddrMaskObjectPtr();
    if (inet_addr_mask) return inet_addr_mask->getLastHostPtr();
    return NULL;
}
