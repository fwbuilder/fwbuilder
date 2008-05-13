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

#include <fwbuilder/Inet6AddrMask.h>
#include <fwbuilder/inet_net.h>

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>

#ifndef _WIN32
#  include <sys/types.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif

using namespace std;
using namespace libfwbuilder;

void Inet6AddrMask::setNetworkAndBroadcastAddress()
{
    delete network_address;
    network_address = new Inet6Addr(
        dynamic_cast<const Inet6Addr&>(*address) & dynamic_cast<const Inet6Addr&>(*netmask));
    delete broadcast_address;
    broadcast_address = new Inet6Addr(
        dynamic_cast<const Inet6Addr&>(*address) | (~(dynamic_cast<const Inet6Addr&>(*netmask))));
}

Inet6AddrMask::Inet6AddrMask() : InetAddrMask(true)
{
    address = new Inet6Addr();
    netmask = new Inet6Addr();
    broadcast_address = new Inet6Addr();
    network_address = new Inet6Addr();
}

Inet6AddrMask::Inet6AddrMask(const Inet6Addr &a, const Inet6Addr &n) :
    InetAddrMask(true)
{    
    address = new Inet6Addr(a & n);
    netmask = new Inet6Addr(n);
    broadcast_address = new Inet6Addr();
    network_address = new Inet6Addr();
    setNetworkAndBroadcastAddress();
}

Inet6AddrMask::Inet6AddrMask(const Inet6AddrMask& other) : InetAddrMask(true)
{
    Inet6Addr *i6_addr = dynamic_cast<Inet6Addr*>(other.address);
    assert(i6_addr);
    Inet6Addr *i6_nm = dynamic_cast<Inet6Addr*>(other.netmask);
    assert(i6_nm);

    address = new Inet6Addr(*i6_addr);
    netmask = new Inet6Addr(*i6_nm);
    broadcast_address = new Inet6Addr();
    network_address = new Inet6Addr();
    setNetworkAndBroadcastAddress();
}

Inet6AddrMask::Inet6AddrMask(const string &s) throw(FWException) : InetAddrMask(true)
{
    struct in6_addr a_ipv6;
    int nbits;
    nbits = inet_net_pton(PGSQL_AF_INET6, s.c_str(), &a_ipv6, sizeof(a_ipv6));
    if (nbits < 0)
        throw FWException(string("Invalid IP address: '") + s + "'");

    address = new Inet6Addr(&a_ipv6);
    netmask = new Inet6Addr(nbits);
    broadcast_address = new Inet6Addr();
    network_address = new Inet6Addr();
    setNetworkAndBroadcastAddress();
}

Inet6AddrMask::~Inet6AddrMask()
{
    // destructor of InetAddrMask deletes address, netmask
    // and other member variables
}

void Inet6AddrMask::setAddress(const InetAddr &a)
{
    assert(a.isV6());
    delete address;
    address = new Inet6Addr(dynamic_cast<const Inet6Addr&>(a));
    setNetworkAndBroadcastAddress();
}

void Inet6AddrMask::setNetmask(const InetAddr &nm)
{
    assert(nm.isV6());
    delete netmask;
    netmask = new Inet6Addr(dynamic_cast<const Inet6Addr&>(nm));
    setNetworkAndBroadcastAddress();
}


std::string Inet6AddrMask::toString() const
{
    char ntop_buf[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255/128"];
    char *cp;
    cp = inet_net_ntop(PGSQL_AF_INET6,
                       (const void*)(&(dynamic_cast<Inet6Addr*>(address)->ipv6)),
                       netmask->getLength(),
                       ntop_buf, sizeof(ntop_buf));
    if (cp==NULL)
    {
        ostringstream err;
        switch (errno)
        {
        case EINVAL:
            err << "Inet6AddrMask::toString() Invalid bit length 0";
            throw FWException(err.str());
            ;;
        case EMSGSIZE:
            err << "Inet6AddrMask::toString() EMSGSIZE error";
            throw FWException(err.str());
            ;;
        case EAFNOSUPPORT:
            err << "Inet6AddrMask::toString() EAFNOSUPPORT error";
            throw FWException(err.str());
            ;;
        default:
            err << "Inet6AddrMask::toString() other error: " << errno;
            throw FWException(err.str());
            ;;
        }
    }
    return std::string(strdup(cp));
}

