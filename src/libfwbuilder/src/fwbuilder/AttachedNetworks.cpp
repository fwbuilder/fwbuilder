/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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



#include "fwbuilder/AttachedNetworks.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace libfwbuilder;
using namespace std;


const char *AttachedNetworks::TYPENAME={"AttachedNetworks"};


AttachedNetworks::AttachedNetworks() : MultiAddress() 
{
}

void AttachedNetworks::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);
}

xmlNodePtr AttachedNetworks::toXML(xmlNodePtr parent)
{
    remStr("run_time");

    xmlNodePtr me = FWObject::toXML(parent, false);

    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    return me;
}


void AttachedNetworks::addNetworkObject(const InetAddrMask &addr_mask)
{
    const InetAddr *ip_addr = addr_mask.getAddressPtr();
    const InetAddr *ip_netm = addr_mask.getNetmaskPtr();

    FWObject *new_obj = nullptr;

    if (ip_addr->isV4())
    {
        Network *net = getRoot()->createNetwork();
        net->setAddress(*ip_addr);
        net->setNetmask(*ip_netm);
        ostringstream str;
        str << "net-" << ip_addr->toString() << "/" << ip_netm->toString();
        net->setName(str.str());
        new_obj = net;
    }

    if (ip_addr->isV6())
    {
        NetworkIPv6 *net = getRoot()->createNetworkIPv6();
        net->setAddress(*ip_addr);
        net->setNetmask(*ip_netm);
        ostringstream str;
        str << "net-" << ip_addr->toString() << "/" << ip_netm->getLength();
        net->setName(str.str());
        new_obj = net;
    }

    if (validateChild(new_obj))
    {
        getRoot()->add(new_obj);
        addRef(new_obj);
    }
}

/*
 * Read addresses of the parent interface and build a group of
 * corresponding networks.
 */
void AttachedNetworks::loadFromSource(bool ipv6, FWOptions*, bool)
{
    Interface *parent_intf = Interface::cast(getParent());
    assert(parent_intf);

    string c_type = (ipv6) ? IPv6::TYPENAME : IPv4::TYPENAME;

    // assemble list of address/netmask pairs to eliminate duplicates
    map<string, InetAddrMask> networks;

    FWObjectTypedChildIterator k = parent_intf->findByType(c_type);
    for ( ; k!=k.end(); ++k)
    {
        Address *addr = Address::cast(*k);
        const InetAddr *ip_netm = addr->getNetmaskPtr();
        const InetAddr *ip_net_addr = addr->getNetworkAddressPtr();
        ostringstream net;
        if (ip_net_addr->isV6())
        {
            net << ip_net_addr->toString() << "/" << ip_netm->getLength();
        } else
        {
            net << ip_net_addr->toString() << "/" << ip_netm->toString();
        }
        networks[net.str()] = InetAddrMask(*ip_net_addr, *ip_netm);
    }

    for (map<string, InetAddrMask>::iterator it=networks.begin(); it!=networks.end(); ++it)
    {
        addNetworkObject(it->second);
    }
}

string AttachedNetworks::getSourceName()
{
    Interface *parent = Interface::cast(getParent());
    assert(parent!=nullptr);
    return parent->getName();
}

