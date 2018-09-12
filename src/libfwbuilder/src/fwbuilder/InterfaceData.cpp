/*

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

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




#include "InterfaceData.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

#include <iostream>

using namespace libfwbuilder;
using namespace std;

InterfaceData::InterfaceData() : addr_mask()
{
    ext = false;
    isDyn = false;
    isUnnumbered = false;
    isBridgePort = false;
    securityLevel = 0;
    snmp_type = 0;
    ostatus = 0;
}

InterfaceData::InterfaceData(const InterfaceData& other) : addr_mask()
{
    id = other.id;
    name = other.name;
    label = other.label;
    if (other.addr_mask.size())
    {
        for (list<InetAddrMask*>::const_iterator i=other.addr_mask.begin();
             i!=other.addr_mask.end(); ++i)
        {
            InetAddrMask *am;
            const InetAddr *ad = (*i)->getAddressPtr();
            const InetAddr *nm = (*i)->getNetmaskPtr();
            if (ad==nullptr) continue;
            if (ad->isV6())
            {
                am = new Inet6AddrMask();
                am->setAddress(*(ad));
                am->setNetmask(*(nm));
            } else
                am = new InetAddrMask(*(*i));
            addr_mask.push_back(am);
        }
    }

    ext = other.ext;
    isDyn = other.isDyn;
    isUnnumbered = other.isUnnumbered;
    isBridgePort = other.isBridgePort;
    securityLevel = other.securityLevel;
    mac_addr = other.mac_addr;
    snmp_type = other.snmp_type;
    ostatus = other.ostatus;
}

InterfaceData::InterfaceData(const Interface &iface)  : addr_mask()
{
    id = iface.getId();
    name = iface.getName();

    IPv4 *addr = IPv4::cast(iface.getFirstByType(IPv4::TYPENAME));
    if (addr)
    {
        addr_mask.push_back(new InetAddrMask(*(addr->getAddressPtr()),
                                             *(addr->getNetmaskPtr())));
    }

    IPv6 *addr6 = IPv6::cast(iface.getFirstByType(IPv6::TYPENAME));
    if (addr6)
    {
        addr_mask.push_back(new Inet6AddrMask(*(addr6->getAddressPtr()),
                                              *(addr6->getNetmaskPtr())));
    }

    securityLevel = iface.getSecurityLevel();
    isDyn = iface.isDyn();
    isUnnumbered = iface.isUnnumbered();
    isBridgePort = iface.isBridgePort();
    libfwbuilder::physAddress *pa = iface.getPhysicalAddress();
    if (pa!=nullptr)
        mac_addr = pa->getPhysAddress();
    label = iface.getLabel();
    networkZone = iface.getStr("network_zone");
}

InterfaceData::~InterfaceData()
{
    // TODO: memory leak! need to delete items in the list addr_mask. 
    addr_mask.clear();
}

