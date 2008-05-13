/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/Inet6Addr.h>
#include <fwbuilder/Interface.h>
#include <fwbuilder/inet_net.h>

#include <stdio.h>
#include <iostream>
#include <sstream>

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

    if (inet_net_pton(PGSQL_AF_INET6, s.c_str(), &ipv6, sizeof(ipv6)) < 0)
        throw FWException(string("Invalid IPv6 address: '")+s+"'");
}

Inet6Addr::Inet6Addr(const Inet6Addr &o) : InetAddr()
{
    *this = o;
}

Inet6Addr::Inet6Addr(const char *data) throw(FWException) : InetAddr()
{
    if(!data)
        throw FWException("NULL IP address data..");
    if (inet_net_pton(PGSQL_AF_INET6, data, &ipv6, sizeof(ipv6)) < 0)
        throw FWException(string("Invalid IP address: '")+string(data)+"'");
}

Inet6Addr::Inet6Addr(const struct in6_addr *na) throw(FWException) : InetAddr()
{
    _copy_in6_addr(&ipv6, na);
}

// Set netmask to 'n' bits
Inet6Addr::Inet6Addr(int len)  throw(FWException)
{
    if (len<0 || len>128) throw FWException(string("Invalid netmask length"));

    ((uint32_t *) (&ipv6))[0] = 0xffffffff;
    ((uint32_t *) (&ipv6))[1] = 0xffffffff;
    ((uint32_t *) (&ipv6))[2] = 0xffffffff;
    ((uint32_t *) (&ipv6))[3] = 0xffffffff;

    // bits is number of zeros counting from the right end
    int nbits = 128 - len;
    for (int i=3; i>=0; --i)
    {
        if (nbits >= 32)
        {
            ((uint32_t*)(&ipv6))[i] = 0;
            nbits -= 32;
            continue;
        }
        uint32_t t = 0xffffffff;
        for (int k = nbits % 32; k; --k)
        {
            t <<= 1;
            t &= 0xfffffffe;
        }
        ((uint32_t*)(&ipv6))[i] = htonl(t);
        break;
    }
}

Inet6Addr& Inet6Addr::operator=(const Inet6Addr &addr)
{
    cerr << "Inet6Addr::operator=" << endl;

    Inet6Addr::_copy_in6_addr(&ipv6, &(addr.ipv6) );
    return *this;
}


std::string Inet6Addr::toString() const
{
    char ntop_buf[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255/128"];
    char *cp;
    cp = inet_net_ntop(PGSQL_AF_INET6, (const void*)(&ipv6), -1, ntop_buf, sizeof(ntop_buf));
    if (cp==NULL)
    {
        ostringstream err;
        switch (errno)
        {
        case EINVAL:
            err << "Inet6Addr::toString() Invalid bit length 0";
            throw FWException(err.str());
            ;;
        case EMSGSIZE:
            err << "Inet6Addr::toString() EMSGSIZE error";
            throw FWException(err.str());
            ;;
        case EAFNOSUPPORT:
            err << "Inet6Addr::toString() EAFNOSUPPORT error";
            throw FWException(err.str());
            ;;
        default:
            err << "Inet6Addr::toString() other error: " << errno;
            throw FWException(err.str());
            ;;
        }
    }
    return std::string(strdup(cp));
}


int Inet6Addr::getLength() const
{
    int bits = 0;
    for (int i=3; i>=0; --i)
    {
        uint32_t n = ntohl(((uint32_t*)(&ipv6))[i]);
        if (n==0)
        {
            bits += 32;
            continue;
        }
        while ((n & 1) == 0)
        {
            bits++;
            n = n >> 1;
        }
        bits = 128 - bits;
        break;
    }
    return bits;
}

}

