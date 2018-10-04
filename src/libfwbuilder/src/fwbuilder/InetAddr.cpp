/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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



#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Interface.h"

extern "C" {
#  include "fwbuilder/inet_net.h"
}

#ifndef _WIN32

#  include <sys/types.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <sys/types.h>
#  include <sys/socket.h>

#endif

#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <assert.h>

#ifndef _WIN32
#  include <sys/types.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif

#ifdef __linux__
# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# define A6PREF __in6_u
#else
# define A6PREF __u6_addr
#endif


using namespace std;
using namespace libfwbuilder;

/*
 * if data is a string that represents integer number without '.' or ':'
 * in it, call init_from_int
 */
void InetAddr::init_from_string(const char* data)
{
    if(!data) throw FWException("NULL IP address data..");
    if (strchr(data, '.')==nullptr && strchr(data, ':')==nullptr)
    {
        char *invalid_chars;
        long r = strtol(data, &invalid_chars, 10);
        if (invalid_chars && *invalid_chars == '\0')
            init_from_int(r);
        else
            throw FWException(string("Invalid IP address: '") + string(data) + "'");

    } else
    {
        if (address_family == AF_INET)
        {
            if (inet_net_pton(AF_INET, data, &ipv4, sizeof(ipv4)) < 0)
                throw FWException(string("Invalid IP address: '") +
                                  string(data)+"'");
        } else if (address_family == AF_INET6)
        {
            if (inet_net_pton(AF_INET6, data, &ipv6, sizeof(ipv6)) < 0)
                throw FWException(string("Invalid IPv6 address: '") +
                                  string(data)+"'");
        } else if (address_family == AF_UNSPEC)
        {
            if (inet_net_pton(AF_INET, data, &ipv4, sizeof(ipv4)) >= 0) {
                address_family = AF_INET;
                return;
            }

            if (inet_net_pton(AF_INET6, data, &ipv6, sizeof(ipv6)) >= 0) {
                address_family = AF_INET6;
                return;
            }

            throw FWException(string("Invalid IP address: '") + string(data)+"'");
        } else
        {
            std::ostringstream s;
            s << "Invalid IP address family: '" << addressFamily() << "'";
            throw FWException(s.str());
        }
    }
}

void InetAddr::init_from_int(unsigned int len)
{
   if (address_family == AF_INET)
   {
       if (len > addressLengthBits())
       {
           throw FWException(string("Invalid netmask length"));
       }

       unsigned long nm_bits = 0;
       int i = len;
       while (i>0)
       {
           nm_bits >>= 1;
           nm_bits |= 0x80000000;
           i--;
       }
       ipv4.s_addr = htonl(nm_bits);
   } else if (address_family == AF_INET6)
   {
       if (len > addressLengthBits())
       {
           throw FWException(string("Invalid netmask length"));
       }

       ((uint32_t *) (&ipv6))[0] = 0xffffffff;
       ((uint32_t *) (&ipv6))[1] = 0xffffffff;
       ((uint32_t *) (&ipv6))[2] = 0xffffffff;
       ((uint32_t *) (&ipv6))[3] = 0xffffffff;

       // bits is number of zeros counting from the right end
       unsigned int nbits = addressLengthBits() - len;
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
   } else
   {
       std::ostringstream s;
       s << "Invalid IP address family: '" << addressFamily() << "'";
       throw FWException(s.str());
   }
}

/*
 * Netmask with "holes" is accepted by InetAddr, but we do not support
 * it at this time. This function returns true if InetAddr object
 * corresponds to an integer with a string of consequitive "1" bits
 * and then string of consequtive "0" bits. The function only works
 * for ipv4 addresses.
 */
bool InetAddr::isValidV4Netmask()
{
    assert(isV4());

    unsigned int n = ntohl(ipv4.s_addr);

    while (n & 0x80000000)
    {
        n = n<<1;
    }

    return (n == 0);
}

// uint128 is always in the host order
void InetAddr::init_from_uint128(uint128 la)
{
    ((uint32_t *) (&ipv6))[0] = htonl((la >> 96).to_integer() & 0xffffffff);
    ((uint32_t *) (&ipv6))[1] = htonl((la >> 64).to_integer() & 0xffffffff);
    ((uint32_t *) (&ipv6))[2] = htonl((la >> 32).to_integer() & 0xffffffff);
    ((uint32_t *) (&ipv6))[3] = htonl( la.to_integer() & 0xffffffff);
}
   
uint128 InetAddr::to_uint128() const
{
    assert(isV6());
    uint128 res;
    uint128 x = uint64_t(ntohl(((uint32_t *) (&ipv6))[0]));
    x <<= 96;
    res |= x;
    x = uint64_t(ntohl(((uint32_t *) (&ipv6))[1]));
    x <<= 64;
    res |= x;
    x = uint64_t(ntohl(((uint32_t *) (&ipv6))[2]));
    x <<= 32;
    res |= x;
    x = uint64_t(ntohl(((uint32_t *) (&ipv6))[3]));
    res |= x;
    return res;
}

InetAddr::InetAddr(const InetAddr &o)
{
    *this = o;
}

InetAddr::InetAddr(const string &s)
{
    address_family = AF_INET;
    init_from_string(s.c_str());
}

InetAddr::InetAddr(int af, const string &s)
{
    address_family = af;
    init_from_string(s.c_str());
}

InetAddr::InetAddr(const char *data)
{
    address_family = AF_INET;
    init_from_string(data);
}

InetAddr::InetAddr(int af, const char *data)
{
    address_family = af;
    init_from_string(data);
}

InetAddr::InetAddr(const struct in_addr *na)
{
    address_family = AF_INET;
    ipv4.s_addr = na->s_addr;
}

InetAddr::InetAddr(const struct in6_addr *na)
{
    address_family = AF_INET6;
    _copy_in6_addr(&ipv6, na);
}

// Set netmask to 'n' bits
InetAddr::InetAddr(int n)
{
    address_family = AF_INET;
    init_from_int(n);
}

InetAddr::InetAddr(int af, int n)
{
    address_family = af;
    init_from_int(n);
}

InetAddr& InetAddr::operator=(const InetAddr &addr)
{
    if ((address_family = addr.address_family)==AF_INET)
    {
        ipv4.s_addr = addr.ipv4.s_addr;
    } else if ((address_family = addr.address_family)==AF_INET6)
    {
        InetAddr::_copy_in6_addr(&ipv6, &(addr.ipv6) );
    } else
    {
        std::ostringstream s;
        s << "Invalid IP address family: '" << addressFamily() << "'";
        throw FWException(s.str());
    }
    return *this;
}

int InetAddr::getLength() const
{
    if (address_family==AF_INET)
    {
        if (ipv4.s_addr == INADDR_BROADCAST) return addressLengthBits();
        if (ipv4.s_addr == 0) return 0;

        unsigned int n = ntohl(ipv4.s_addr);

        int   i=0;
        while (n)
        {
            n=n<<1;
            i++;
        }

        return i;
    } else if (address_family==AF_INET6)
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
    } else
    {
        std::ostringstream s;
        s << "Invalid IP address family: '" << addressFamily() << "'";
        throw FWException(s.str());
    }
}

string InetAddr::toString() const
{
    if (address_family==AF_INET)
    {
        return std::string(inet_ntoa(ipv4));
    } else if (address_family==AF_INET6)
    {
        char ntop_buf[sizeof
                      "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255/128"];
        /*
         * Our included copy of inet_net_ntop does not add /netmask if
         * bits==-1 (argument #3). However, the same function included
         * in libc on FreeBSD returns EINVAL for bits=-1. Here is a
         * hack: use bits=128 and then strip /128 that inet_net_ntop
         * adds to the generated string. Both our included
         * inet_net_ntop and the one shipped with FreeBSD add "/128"
         * consistently, so this works on all platforms.
         */
        char *cp;
        cp = inet_net_ntop(AF_INET6, (const void*)(&ipv6),
                           128, ntop_buf, sizeof(ntop_buf));
        if (cp==nullptr)
        {
            ostringstream err;
            switch (errno)
            {
            case EINVAL:
                err << "InetAddr::toString() Invalid bit length 0";
                throw FWException(err.str());
                ;;
            case EMSGSIZE:
                err << "InetAddr::toString() EMSGSIZE error";
                throw FWException(err.str());
                ;;
            case EAFNOSUPPORT:
                err << "InetAddr::toString() EAFNOSUPPORT error";
                throw FWException(err.str());
                ;;
            default:
                err << "InetAddr::toString() other error: " << errno;
                throw FWException(err.str());
                ;;
            }
        }
        char *slash_p = strchr(ntop_buf, '/');
        if (slash_p!=nullptr) *slash_p = '\0';
        return std::string(ntop_buf);
    } else
    {
        std::ostringstream s;
        s << "Invalid IP address family: '" << addressFamily() << "'";
        throw FWException(s.str());
    }
}

// note that address family of the result is dictated by the address family of
// "this". Address family of mask must be the same.
InetAddr InetAddr::opAnd(const InetAddr &mask) const
{
    assert(address_family==mask.address_family);
    if (address_family==AF_INET)
    {
        struct in_addr res;
        res.s_addr = htonl(ntohl(ipv4.s_addr) & ntohl(mask.ipv4.s_addr));
        return InetAddr(&res);
    } else if (address_family==AF_INET6) {
        struct in6_addr res;
        for (int i=0; i<4; ++i)
            ((uint32_t*)(&res))[i] = 
                htonl(ntohl(((uint32_t*)(&(ipv6)))[i]) &
                      ntohl(((uint32_t*)(&(mask.ipv6)))[i]));
        return InetAddr(&res);
    } else
    {
        std::ostringstream s;
        s << "Invalid IP address family: '" << addressFamily() << "'";
        throw FWException(s.str());
    }
}

InetAddr InetAddr::opOr(const InetAddr &mask) const
{
    assert(address_family==mask.address_family);
    if (address_family==AF_INET)
    {
        struct in_addr res;
        res.s_addr = htonl(ntohl(ipv4.s_addr) | ntohl(mask.ipv4.s_addr));
        return InetAddr(&res);
    } else if (address_family==AF_INET6)
    {
        struct in6_addr res;
        for (int i=0; i<4; ++i)
            ((uint32_t*)(&res))[i] = 
                htonl(ntohl(((uint32_t*)(&(ipv6)))[i]) |
                      ntohl(((uint32_t*)(&(mask.ipv6)))[i]));
        return InetAddr(&res);
    } else
    {
        std::ostringstream s;
        s << "Invalid IP address family: '" << addressFamily() << "'";
        throw FWException(s.str());
    }
}

InetAddr InetAddr::opPlus(int increment) const
{
    if (address_family==AF_INET)
    {
        struct in_addr res;
        res.s_addr = htonl(ntohl(ipv4.s_addr) + increment);
        return InetAddr(&res);
    } else if (address_family==AF_INET6)
    {
        uint128 x = to_uint128();
        x += increment;
        InetAddr res(AF_INET6, 0);
        res.init_from_uint128(x);
        return res;

//         struct in6_addr res;
//         InetAddr::_copy_in6_addr(&res, &(ipv6) );
//         ((uint32_t*)(&res))[3] =
//             htonl(ntohl( ((uint32_t*)(&(ipv6)))[3]) + increment);
//         return InetAddr(&res);
    } else
    {
        std::ostringstream s;
        s << "Invalid IP address family: '" << addressFamily() << "'";
        throw FWException(s.str());
    }
}

InetAddr InetAddr::opMinus(int decrement) const
{
    if (address_family==AF_INET)
    {
        struct in_addr res;
        res.s_addr = htonl(ntohl(ipv4.s_addr) - decrement);
        return InetAddr(&res);
    } else if (address_family==AF_INET6)
    {
        uint128 x = to_uint128();
        x -= decrement;
        InetAddr res(AF_INET6, 0);
        res.init_from_uint128(x);
        return res;

//         struct in6_addr res;
//         InetAddr::_copy_in6_addr(&res, &(ipv6) );
//         ((uint32_t*)(&res))[3] =
//             htonl(ntohl( ((uint32_t*)(&(ipv6)))[3]) - decrement);
//         return InetAddr(&res);
    } else
    {
        std::ostringstream s;
        s << "Invalid IP address family: '" << addressFamily() << "'";
        throw FWException(s.str());
    }
}

bool InetAddr::opLT(const InetAddr &other) const
{
    if (address_family!=other.address_family) return false;
    if (address_family==AF_INET)
    {
        return (ntohl( ipv4.s_addr ) < ntohl( other.ipv4.s_addr ));
    } else if (address_family==AF_INET6)
    {
        uint128 a = to_uint128();
        uint128 b = other.to_uint128();
        return a < b;

//         return (ntohl(((uint32_t*)(&(ipv6)))[3]) <
//                 ntohl(((uint32_t*)(&(other.ipv6)))[3]));
    } else
    {
        std::ostringstream s;
        s << "Invalid IP address family: '" << addressFamily() << "'";
        throw FWException(s.str());
    }
}

bool InetAddr::opGT(const InetAddr &other) const
{
    if (address_family!=other.address_family) return false;
    if (address_family==AF_INET)
    {
        return (ntohl( ipv4.s_addr ) > ntohl( other.ipv4.s_addr ));
    } else if (address_family==AF_INET6)
    {
        uint128 a = to_uint128();
        uint128 b = other.to_uint128();
        return a > b;

//         return (ntohl(((uint32_t*)(&(ipv6)))[3]) >
//                 ntohl(((uint32_t*)(&(other.ipv6)))[3]));
    } else
    {
        std::ostringstream s;
        s << "Invalid IP address family: '" << addressFamily() << "'";
        throw FWException(s.str());
    }
}

bool InetAddr::opEQ(const InetAddr &other) const
{
    if (address_family!=other.address_family) return false;
    if (address_family==AF_INET)
    {
        return ipv4.s_addr == other.ipv4.s_addr;
    } else if (address_family==AF_INET6)
    {
        return (IN6_ARE_ADDR_EQUAL(&(ipv6), &(other.ipv6)));
    } else
    {
        std::ostringstream s;
        s << "Invalid IP address family: '" << addressFamily() << "'";
        throw FWException(s.str());
    }
}

bool InetAddr::opNEQ(const InetAddr &other) const
{
    if (address_family!=other.address_family) return false;
    if (address_family==AF_INET)
    {
        return ipv4.s_addr != other.ipv4.s_addr;
    } else if (address_family==AF_INET6)
    {
        return (!(IN6_ARE_ADDR_EQUAL(&(ipv6), &(other.ipv6))));
    } else
    {
        std::ostringstream s;
        s << "Invalid IP address family: '" << addressFamily() << "'";
        throw FWException(s.str());
    }
}
    
InetAddr InetAddr::opCompl() const
{
    if (address_family==AF_INET)
    {
        struct in_addr res;
        res.s_addr = htonl(~(ntohl(ipv4.s_addr)));
        return InetAddr(&res);
    } else if (address_family==AF_INET6)
    {
        struct in6_addr res;
        ((uint32_t *) (&res))[0] = htonl(~(ntohl(((uint32_t *) (&ipv6))[0])));
        ((uint32_t *) (&res))[1] = htonl(~(ntohl(((uint32_t *) (&ipv6))[1])));
        ((uint32_t *) (&res))[2] = htonl(~(ntohl(((uint32_t *) (&ipv6))[2])));
        ((uint32_t *) (&res))[3] = htonl(~(ntohl(((uint32_t *) (&ipv6))[3])));
        return InetAddr(&res);
    } else
    {
        std::ostringstream s;
        s << "Invalid IP address family: '" << addressFamily() << "'";
        throw FWException(s.str());
    }
}

