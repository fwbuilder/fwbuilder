/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: Inet6Addr.cpp 1034 2007-08-02 05:19:28Z vkurland $


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

#include <fwbuilder/Inet6Addr.h>
#include <fwbuilder/Interface.h>

#include <stdio.h>
#include <iostream>

#ifndef _WIN32
#  include <sys/types.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif

using namespace std;


namespace libfwbuilder
{

Inet6Addr::Inet6Addr(const string &s)
    throw(FWException, FWNotSupportedException)
{
    if (inet_pton(AF_INET6, s.c_str(), &ipv4) == 0)
        throw FWException(string("Invalid IPv6 address: '")+s+"'");
}

Inet6Addr::Inet6Addr(const Inet6Addr &o)
{
    *this = o;
}

Inet6Addr::Inet6Addr(const char *data) throw(FWException)
{
    if(!data)
        throw FWException("NULL IP address data..");
    if (inet_pton(AF_INET6, data, &ipv4) == 0)
        throw FWException(string("Invalid IP address: '")+string(data)+"'");
}

Inet6Addr::Inet6Addr(const struct in6_addr *na) throw(FWException)
{
    if (int i=0; i<4; ++i)
        ipv6.s_addr32[i] = na->s_addr32[i];
}

/*****************************************************************
 *
 *   Inet6Netmask
 */

Inet6Netmask::Inet6Netmask() : Inet6Addr() {}

Inet6Netmask::Inet6Netmask(const Inet6Addr& nm)
{
    if (int i=0; i<4; ++i)
        ipv6.s_addr32[i] = nm.s_addr32[i];
}

Inet6Netmask::Inet6Netmask(const char *data) throw(FWException) : Inet6Addr(data) 
{
}

Inet6Netmask::Inet6Netmask(const string &s) throw(FWException) : Inet6Addr(s)
{
}

// Set netmask to 'n' bits
Inet6Netmask::Inet6Netmask(int n)  throw(FWException)
{
    if (n<0 || n>32) throw FWException(string("Invalid netmask length"));
    unsigned long nm_bits = 0;
    int i = n;
    while (i>0)
    {
        nm_bits >>= 1;
        nm_bits |= 0x80000000;
        i--;
    }
    ipv4.s_addr = htonl(nm_bits);
}

Inet6Netmask::~Inet6Netmask()
{
}

int Inet6Netmask::getLength() const
{
    if (ipv4.s_addr == INADDR_BROADCAST) return 32;
    if (ipv4.s_addr == 0) return 0;

    unsigned int n = ntohl(ipv4.s_addr);

    int   i=0;
    while (n)
    {
	n=n<<1;
	i++;
    }

    return i;
}

}

