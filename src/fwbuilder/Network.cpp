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

#include <assert.h>

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/Network.h>
#include <fwbuilder/XMLTools.h>

#include <string>


using namespace libfwbuilder;
using namespace std;


const char *Network::TYPENAME={"Network"};

Network::Network() : Address()
{
    setNetmask(InetAddr(0));
}

Network::Network(const FWObject *root,bool prepopulate) :
    Address(root, prepopulate)
{
    setNetmask(InetAddr(0));
}

Network::Network(Network &o) : Address(o)
{
    FWObject::operator=(o);
    setAddress(o.getAddress());
    setNetmask(o.getNetmask());
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
    xmlNodePtr me = FWObject::toXML(xml_parent_node);
    
    xmlNewProp(me, 
               TOXMLCAST("address"),
               STRTOXMLCAST(getAddress().toString()));
    
    xmlNewProp(me, 
               TOXMLCAST("netmask"),
               STRTOXMLCAST(getNetmask().toString()));
    
    return me;
}

/* check if host address bits are cleared */
bool Network::isValidRoutingNet() const
{
    return (getAddress() == (getAddress() & getNetmask()));
}

void Network::setAddress(const InetAddr &a, bool)
{
    inet_addr_mask->setAddress(a);
}

void Network::setNetmask(const InetAddr &nm, bool)
{
    inet_addr_mask->setNetmask(nm);
}

void Network::setAddressNetmask(const std::string& s)
{
    delete inet_addr_mask;
    inet_addr_mask = new InetAddrMask(s);
}

