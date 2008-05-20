/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: NetworkIPv6.cpp 975 2006-09-10 22:40:37Z vkurland $


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
#include <fwbuilder/Inet6AddrMask.h>
#include <fwbuilder/NetworkIPv6.h>
#include <fwbuilder/XMLTools.h>

#include <string>


using namespace libfwbuilder;
using namespace std;


const char *NetworkIPv6::TYPENAME={"NetworkIPv6"};

NetworkIPv6::NetworkIPv6() : Address()
{
    delete inet_addr_mask;
    inet_addr_mask = new Inet6AddrMask();
}

NetworkIPv6::NetworkIPv6(const FWObject *root,bool prepopulate) :
    Address(root, prepopulate)
{
    delete inet_addr_mask;
    inet_addr_mask = new Inet6AddrMask();
}

NetworkIPv6::NetworkIPv6(NetworkIPv6 &other) : Address(other)
{
    delete inet_addr_mask;
    inet_addr_mask = new Inet6AddrMask(
        *(dynamic_cast<Inet6AddrMask*>(other.inet_addr_mask)));
    FWObject::operator=(other);
}

NetworkIPv6::NetworkIPv6 (const string &s) : Address()
{
    setAddressNetmask(s);
}
                                     
NetworkIPv6::~NetworkIPv6() {}

FWObject& NetworkIPv6::shallowDuplicate(const FWObject *other,
                                        bool preserve_id) throw(FWException)
{
    const NetworkIPv6* a_other = NetworkIPv6::constcast(other);
    delete inet_addr_mask;
    inet_addr_mask = new Inet6AddrMask(
        *(dynamic_cast<Inet6AddrMask*>(a_other->inet_addr_mask)));
    return FWObject::shallowDuplicate(other, preserve_id);
}

void NetworkIPv6::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);
    
    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("address")));
    assert(n!=NULL);
    setAddress(Inet6Addr(n));
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("netmask")));
    assert(n!=NULL);
    setNetmask(Inet6Addr(n));
    FREEXMLBUFF(n);
}

xmlNodePtr NetworkIPv6::toXML(xmlNodePtr xml_parent_node) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(xml_parent_node);
    
    xmlNewProp(me, 
               TOXMLCAST("address"),
               STRTOXMLCAST(inet_addr_mask->getAddressPtr()->toString()));
    
    xmlNewProp(me, 
               TOXMLCAST("netmask"),
               STRTOXMLCAST(inet_addr_mask->getNetmaskPtr()->toString()));
    
    return me;
}

/* check if host address bits are cleared */
bool NetworkIPv6::isValidRoutingNet() const
{
    return (*(getAddressPtr()) == *(getNetworkAddressPtr()));
}

void NetworkIPv6::setAddress(const InetAddr &a)
{
    inet_addr_mask->setAddress(a);
}

void NetworkIPv6::setNetmask(const InetAddr &nm)
{
    inet_addr_mask->setNetmask(nm);
}

void NetworkIPv6::setAddressNetmask(const std::string& s)
{
    delete inet_addr_mask;
    inet_addr_mask = new Inet6AddrMask(s);
}

