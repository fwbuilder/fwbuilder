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

#ifndef __INETADDR_HH_FLAG__
#define __INETADDR_HH_FLAG__

#include <string>
#include <vector>
#include <typeinfo>
#include <assert.h>

#ifndef _WIN32
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#else
#  include <winsock2.h>
#  include <wtypes.h>
#  include <ws2tcpip.h>

typedef unsigned int uint32_t;

#endif

#include "fwbuilder/uint128.h"
#include "fwbuilder/FWException.h"

namespace libfwbuilder
{

/**
 * Class InetAddr is a wrapper for struct inet_addr and in6_addr
 *
 * Why both address families are implemented as the same class ? Mostly
 * because I need to have a family of two-argument operators such as
 * operator&, operator| etc which return new object of the class that
 * represent the same address family as the arguments. These operators
 * should be "friends" of class InetAddr since they return new object
 * by value rather than modify "this". But how to do it if returned
 * type should be different depending on the types of arguments ?
 *
 */
class InetAddr
{
    protected:

    friend class InetAddrMask;
    friend class Inet6AddrMask;

    int address_family;
    // Address in network order
    struct in_addr  ipv4;
    struct in6_addr ipv6;

    // copy in6_addr from sa to da
    static inline void _copy_in6_addr(struct in6_addr* da,
                                      const struct in6_addr* sa)
    {
        ((uint32_t*)(da))[0] = ((uint32_t*)(sa))[0];
        ((uint32_t*)(da))[1] = ((uint32_t*)(sa))[1];
        ((uint32_t*)(da))[2] = ((uint32_t*)(sa))[2];
        ((uint32_t*)(da))[3] = ((uint32_t*)(sa))[3];
    }

    void init_from_string(const char* data);
    void init_from_int(unsigned int n);

    public:
    
    explicit InetAddr()
    {
        address_family = AF_INET;
        ipv4.s_addr = 0;
        ((uint32_t *) (&ipv6))[0] = 0;
        ((uint32_t *) (&ipv6))[1] = 0;
        ((uint32_t *) (&ipv6))[2] = 0;
        ((uint32_t *) (&ipv6))[3] = 0;
    }
        
    virtual ~InetAddr() {}

    inline unsigned int addressLengthBits() const
    {
        if (address_family == AF_INET) return sizeof(ipv4) * 8;
        return sizeof(ipv6) * 8;
    }

    bool isValidV4Netmask();
    
    void init_from_uint128(uint128 int128a);
    uint128 to_uint128() const;
    
    InetAddr(const char *data);
    InetAddr(int af, const char *data);
    InetAddr(const struct in_addr*);
    InetAddr(const struct in6_addr*);
    explicit InetAddr(const std::string&);
    explicit InetAddr(int af, const std::string&);
    InetAddr(const InetAddr &);
    // creates netmask 'n' bits long
    explicit InetAddr(int n);
    explicit InetAddr(int af, int n);

    InetAddr& operator=(const InetAddr &addr);

    int addressFamily() const { return address_family; }
    bool isV4() const { return (address_family==AF_INET); }
    bool isV6() const { return (address_family==AF_INET6); }

    const struct in_addr* getV4() const { return &ipv4; }
    const struct in6_addr* getV6() const { return &ipv6; }
    
    static inline InetAddr getAny()
    {
        return InetAddr();
    }

    static inline InetAddr getAllOnes(int af=AF_INET)
    {
        if (af==AF_INET)
        {
            struct in_addr allones;
            allones.s_addr = 0xffffffff;
            return InetAddr(&allones);
        } else
        {
            struct in6_addr a;
            ((uint32_t *) (&a))[0] = 0xffffffff;
            ((uint32_t *) (&a))[1] = 0xffffffff;
            ((uint32_t *) (&a))[2] = 0xffffffff;
            ((uint32_t *) (&a))[3] = 0xffffffff;
            return InetAddr(&a);
        }
    }

    static inline InetAddr getLoopbackAddr(int af=AF_INET)
    {
        if (af==AF_INET)
        {
            struct in_addr loopback;
            loopback.s_addr = htonl(INADDR_LOOPBACK);
            return InetAddr(&loopback);
        } else
        {
            struct in6_addr a;
            ((uint32_t *) (&a))[0] = 0;
            ((uint32_t *) (&a))[1] = 0;
            ((uint32_t *) (&a))[2] = 0;
            ((uint32_t *) (&a))[3] = htonl (1);
            return InetAddr(&a);
        }
    }

    std::string toString() const;

    /**
     * Broadcast :  255.255.255.255
     *
     * there are no broadcast addresses in ipv6. However some multicast
     * addresses serve similar purpose. For example "link-scope
     * all-hosts multicast" address ff02::1 corresponds to the ipv4
     * broadcast 255.255.255.255
     */
    inline bool isBroadcast() const
    {
        if (address_family==AF_INET)
            return ipv4.s_addr == INADDR_BROADCAST;
        else
            return IN6_IS_ADDR_MC_LINKLOCAL(&ipv6);
    }

    /**
     * Multicast :  224.0.0.0 - 239.0.0.0
     */
    inline bool isMulticast() const
    {
        if (address_family==AF_INET)
            return IN_MULTICAST(ntohl(ipv4.s_addr));
        else
            return IN6_IS_ADDR_MULTICAST(&ipv6);
    }

    /**
     * INADDR_ANY: 0
     */
    inline bool isAny() const
    {
        if (address_family==AF_INET)
            return ipv4.s_addr == INADDR_ANY;
        else
            return (IN6_IS_ADDR_UNSPECIFIED(&ipv6));
    }

    /**
     * calculate distance between _this_ address and address a2 and return
     * it as int
     * This method is limited, it only calculates distance that fit in 32 bit
     * number
     */
    inline unsigned int distance(const InetAddr &a2) const
    {
        if (address_family==AF_INET)
            return ntohl(a2.ipv4.s_addr) - ntohl(ipv4.s_addr) + 1;
        else
        {
            uint128 d1 = to_uint128();
            uint128 d2 = a2.to_uint128();
            uint128 res;
            if (d1 < d2)
            {
                res = d2;
                res -= d1;
            } else
            {
                res = d1;
                res -= d2;
            }
            return res.to_integer()  + 1;
        }
    }

    /**
     * returns the "length" of the netmask, that is number of bits set to '1'
     * counting from left to right
     */ 
    int getLength() const;

    /**
     * for netmasks: return true if this is host mask, i.e. all '1'
     */
    inline bool isHostMask() const
    {
        if (address_family==AF_INET)
            return ipv4.s_addr == INADDR_BROADCAST;
        else
            return (((uint32_t*)(&ipv6))[0] == 0xffffffff &&
                    ((uint32_t*)(&ipv6))[1] == 0xffffffff &&
                    ((uint32_t*)(&ipv6))[2] == 0xffffffff &&
                    ((uint32_t*)(&ipv6))[3] == 0xffffffff);
    }

    /*****************************************************************/

    InetAddr opAnd(const InetAddr &mask) const;

    InetAddr opOr(const InetAddr &mask) const;

    InetAddr opPlus(int increment) const;

    InetAddr opMinus(int decrement) const;

    bool opLT(const InetAddr &other) const;

    bool opGT(const InetAddr &other) const;

    bool opEQ(const InetAddr &other) const;

    bool opNEQ(const InetAddr &other) const;
    
    InetAddr opCompl() const;

    /*****************************************************************/

    inline friend InetAddr operator&(const InetAddr &addr,
                                     const InetAddr &mask)
    {
        assert (typeid(addr) == typeid(mask));
        return addr.opAnd(mask);
    }

    inline friend InetAddr operator|(const InetAddr &addr,
                                     const InetAddr &mask)
    {
        assert (typeid(addr) == typeid(mask));
        return addr.opOr(mask);
    }

    inline friend InetAddr operator+(const InetAddr &addr, int increment)
    {
        return addr.opPlus(increment);
    }

    inline friend InetAddr operator-(const InetAddr &addr, int decrement)
    {
        return addr.opMinus(decrement);
    }

    inline friend bool operator<(const InetAddr &a, const InetAddr &b)
    {
        assert (typeid(a) == typeid(b));
        return a.opLT(b);
    }

    inline friend bool operator>(const InetAddr &a, const InetAddr &b)
    {
        assert (typeid(a) == typeid(b));
        return a.opGT(b);
    }

    inline friend bool operator==(const InetAddr &a, const InetAddr &b)
    {
        assert (typeid(a) == typeid(b));
        return a.opEQ(b);
    }

    inline friend bool operator!=(const InetAddr &a, const InetAddr &b)
    {
        assert (typeid(a) == typeid(b));
        return a.opNEQ(b);
    }
    
    inline friend InetAddr operator~(const InetAddr &a)
    {
        return a.opCompl();
    }

};

}

#endif
