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



#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Inet6AddrMask.h"

extern "C" {
#include "fwbuilder/inet_net.h"
}

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#ifndef _WIN32
#  include <sys/types.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif

#include <errno.h>
#include <iostream>

using namespace std;
using namespace libfwbuilder;

Inet6AddrMask::Inet6AddrMask() : InetAddrMask()
{
    address->address_family = AF_INET6;
    netmask->address_family = AF_INET6;
    setNetworkAndBroadcastAddress();
}

Inet6AddrMask::Inet6AddrMask(const string &s) :
    InetAddrMask(true)
{
    struct in6_addr a_ipv6;
    int nbits;
    nbits = inet_net_pton(AF_INET6, s.c_str(), &a_ipv6, sizeof(a_ipv6));
    if (nbits < 0)
        throw FWException(string("Invalid IP address: '") + s + "'");

    address = new InetAddr(&a_ipv6);
    netmask = new InetAddr(AF_INET6, nbits);
    
    broadcast_address = new InetAddr();
    network_address = new InetAddr();
    last_host = new InetAddr();
    setNetworkAndBroadcastAddress();
}

Inet6AddrMask::Inet6AddrMask(const InetAddr &a, const InetAddr &n) :
    InetAddrMask(true)
{    
    address = new InetAddr(a & n);
    netmask = new InetAddr(AF_INET6, n.getLength());
    broadcast_address = new InetAddr();
    network_address = new InetAddr();
    last_host = new InetAddr();
    setNetworkAndBroadcastAddress();
}

Inet6AddrMask::~Inet6AddrMask()
{
    // destructor of InetAddrMask deletes address, netmask
    // and other member variables
}

std::string Inet6AddrMask::toString() const
{
    char ntop_buf[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255/128"];
    char *cp;
    cp = inet_net_ntop(AF_INET6, (const void*)(&(address->ipv6)),
                       netmask->getLength(),
                       ntop_buf, sizeof(ntop_buf));
    if (cp==nullptr)
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

