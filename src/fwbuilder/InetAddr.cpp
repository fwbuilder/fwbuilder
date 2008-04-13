/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: InetAddr.cpp 1034 2007-08-02 05:19:28Z vkurland $


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

#include <fwbuilder/InetAddr.h>
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

InetAddr::InetAddr(const string &s)
    throw(FWException, FWNotSupportedException)
{
    if (inet_aton(s.c_str(), &ipv4)==0)
        throw FWException(string("Invalid IP address: '")+s+"'");
}

InetAddr::InetAddr(const InetAddr &o)
{
    *this = o;
}

InetAddr::InetAddr(const char *data) throw(FWException)
{
    if(!data)
        throw FWException("NULL IP address data..");
    if (inet_aton(data, &ipv4)==0)
        throw FWException(string("Invalid IP address: '")+string(data)+"'");
}

InetAddr::InetAddr(const struct in_addr *na) throw(FWException)
{
    ipv4.s_addr = na->s_addr;
}

/*****************************************************************
 *
 *   InetNetmask
 */

InetNetmask::InetNetmask() : InetAddr() {}

InetNetmask::InetNetmask(const InetAddr& a) : InetAddr(a) {}

InetNetmask::InetNetmask(const char *data) throw(FWException) : InetAddr(data) 
{
}

InetNetmask::InetNetmask(const string &s) throw(FWException) : InetAddr(s)
{
}

// Set netmask to 'n' bits
InetNetmask::InetNetmask(int n)  throw(FWException)
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

InetNetmask::~InetNetmask()
{
}

int InetNetmask::getLength() const
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

