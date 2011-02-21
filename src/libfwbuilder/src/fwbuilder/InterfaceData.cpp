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


#include <fwbuilder/libfwbuilder-config.h>

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
            if (ad==NULL) continue;
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
    if (pa!=NULL)
        mac_addr = pa->getPhysAddress();
    label = iface.getLabel();
    networkZone = iface.getStr("network_zone");
}

InterfaceData::~InterfaceData()
{
    // TODO: memory leak! need to delete items in the list addr_mask. 
    addr_mask.clear();
}

void InterfaceData::guessLabel(const string&)
{
/*
 *  some firewalls report fairly regular names for interfaces through
 *  their built-in SNMP agent. We can use this to assign labels
 *  automatically.
 *
 *  in PIX interfaces have names like "PIX Firewall 'inside' interface"
 *
 */
    string pat1 = "PIX Firewall '";
    string pat2 = "Adaptive Security Appliance '";
    string pat3 = "' interface";
    string::size_type p2;

    if ( name.find(pat1)==0 && (p2=name.find(pat3))!=string::npos )
        label = name.substr( pat1.size() , p2-pat1.size() );

    if ( name.find(pat2)==0 && (p2=name.find(pat3))!=string::npos )
        label = name.substr( pat2.size() , p2-pat2.size() );

    if (!isDyn &&
        !isUnnumbered &&
        !isBridgePort &&
        addr_mask.size()!=0 &&
        addr_mask.front()->getAddressPtr()->toString() == InetAddr::getLoopbackAddr().toString())
        label = "loopback";
}

void InterfaceData::guessSecurityLevel(const string&)
{
    InetAddrMask n10(InetAddr("10.0.0.0"), InetAddr("255.0.0.0"));
    InetAddrMask n172(InetAddr("172.16.0.0"), InetAddr("255.240.0.0"));
    InetAddrMask n192(InetAddr("192.168.0.0"), InetAddr("255.255.0.0"));

    securityLevel = -1;

    string llbl = label;

    for (string::size_type i=0; i<llbl.length(); i++)
        llbl[i] = tolower( llbl[i] );

    if ( llbl=="out" ||
         llbl=="ext" ||
         llbl=="internet" ||
         llbl=="wan" ||
         llbl=="dsl" ||
         llbl=="cable" ||
         llbl.find("outside")!=string::npos ||
         llbl.find("external")!=string::npos) securityLevel = 0;

    if ( llbl=="lan" ||
         llbl=="in" ||
         llbl.find("inside")!=string::npos ||
         llbl.find("internal")!=string::npos ) securityLevel = 100;

    if ( llbl.find("dmz")!=string::npos ) securityLevel = 50;

    if ( (*(addr_mask.front()->getAddressPtr()))==InetAddr::getLoopbackAddr())
        securityLevel = 100; 

    if (name=="Null0") securityLevel = 100; 

    if (securityLevel==-1 && !isDyn && !isUnnumbered && !isBridgePort)
    {
        if (n10.belongs(  InetAddr( *(addr_mask.front()->getAddressPtr()) ) ))
            securityLevel = 100;

        if (n172.belongs( InetAddr( *(addr_mask.front()->getAddressPtr()) ) ))
            securityLevel = 100;

        if (n192.belongs( InetAddr( *(addr_mask.front()->getAddressPtr()) ) ))
            securityLevel = 100;
    }

    if (isDyn || isUnnumbered || isBridgePort) securityLevel = 0;

    if (securityLevel==-1) securityLevel = 0;
}


class sort_order_func_adaptor
{
    public:

    explicit sort_order_func_adaptor() {}

    bool operator()(const InterfaceData &a, const InterfaceData &b)
    {
        if (a.label=="outside") return true;
        if (b.label=="inside")  return true;
        return (a.securityLevel<b.securityLevel || a.label<b.label || a.name<b.name);
    }
};




void  InterfaceData::guessSecurityLevel(const string &platform,
                                        list<InterfaceData> &ifaces)
{
// first pass - try to find internal and external interfaces and
// assign sec. levels and labels

//    bool supports_security_levels=Resources::getTargetCapabilityBool(platform,
//                                                                     "security_levels");

    list<InterfaceData> res;

    if (ifaces.size()==1)
    {
        ifaces.front().guessSecurityLevel(platform);
        return;
    }

    if (ifaces.size()==2)
    {
        const InetAddr *address = ifaces.front().addr_mask.front()->getAddressPtr();
        if (*address==InetAddr::getLoopbackAddr())
        {
            ifaces.front().securityLevel=100;
            ifaces.back().securityLevel=0;
        } else
        {
            const InetAddr *address = ifaces.back().addr_mask.front()->getAddressPtr();
            if (*address==InetAddr::getLoopbackAddr()) 
            {
                ifaces.front().securityLevel=0;
                ifaces.back().securityLevel=100;
            } else
            {
                ifaces.front().guessSecurityLevel(platform);
                ifaces.back().guessSecurityLevel(platform);
            }
        }
        ifaces.sort(sort_order_func_adaptor());
        return;
    }
    else
    {
        for (list<InterfaceData>::iterator i=ifaces.begin(); i!=ifaces.end(); i++)
        {
            i->guessSecurityLevel(platform);
        }
    }

    ifaces.sort(sort_order_func_adaptor());

// second pass - Assign sec. levels evenly if it is pix, or all zeros in all other cases.

    int sec_level_step= 100 / ( ifaces.size() - 1 );
    int sec_level = 0;

    for (list<InterfaceData>::iterator i=ifaces.begin(); i!=ifaces.end(); i++)
    {
        i->securityLevel=sec_level;
        sec_level += sec_level_step;
    }
}


