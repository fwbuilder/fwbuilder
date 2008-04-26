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

#ifndef __INET6ADDR_HH_FLAG__
#define __INET6ADDR_HH_FLAG__

#include <string>
#include <vector>

#ifndef _WIN32
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#else
#  include <winsock2.h>
#endif

#include <fwbuilder/InetAddr.h>

namespace libfwbuilder
{

/**
 * Class Inet6Addr is a wrapper for struct in6_addr
 *
 */
class Inet6Addr : public InetAddr
{
    protected:

    friend class IPv6Network;

    // Address in network order
    struct in6_addr ipv6;

    // copy in6_addr from sa to da
    static inline void _copy_in6_addr(struct in6_addr* da,
                                      const struct in6_addr* sa)
    {
        for (int i=0; i<4; ++i)
            ((uint32_t*)(da))[i] = ((uint32_t*)(sa))[i];
    }

    public:
    
    explicit Inet6Addr()
    {
        ((uint32_t *) (&ipv6))[0] = 0;
        ((uint32_t *) (&ipv6))[1] = 0;
        ((uint32_t *) (&ipv6))[2] = 0;
        ((uint32_t *) (&ipv6))[3] = 0;
    }
        
    virtual ~Inet6Addr() {}

    Inet6Addr(const char *data) throw(FWException);
    Inet6Addr(const struct in6_addr*) throw(FWException);
    explicit Inet6Addr(const std::string&)
        throw(FWException, FWNotSupportedException);
    Inet6Addr(const Inet6Addr &);
    Inet6Addr(int n) throw(FWException);  // creates netmask 'n' bits long
    
    virtual bool isV4() { return false; }
    virtual bool isV6() { return true; }
    
    static inline Inet6Addr getAny()
    {
        return Inet6Addr();
    }

    static inline Inet6Addr getAllOnes()
    {
        struct in6_addr a;
        ((uint32_t *) (&a))[0] = 0xffffffff;
        ((uint32_t *) (&a))[1] = 0xffffffff;
        ((uint32_t *) (&a))[2] = 0xffffffff;
        ((uint32_t *) (&a))[3] = 0xffffffff;
        return Inet6Addr(&a);
    }

    static inline Inet6Addr getLoopbackAddr()
    {
        struct in6_addr a;
        ((uint32_t *) (&a))[0] = 0;
        ((uint32_t *) (&a))[1] = 0;
        ((uint32_t *) (&a))[2] = 0;
        ((uint32_t *) (&a))[3] = htonl (1);
        return Inet6Addr(&a);
    }

    inline virtual std::string toString() const
    {
        char ntop_buf[INET6_ADDRSTRLEN];
        const char *cp;
        cp = inet_ntop(AF_INET6, &ipv6, ntop_buf, sizeof(ntop_buf));
        return std::string(strdup(cp));
    }

    /**
     * Broadcast :  there are no broadcast addresses in ipv6
     * However some multicast addresses serve similar purpose. For example
     * "link-scope all-hosts multicast" address ff02::1 corresponds to
     * the ipv4 broadcast 255.255.255.255
     */
    inline virtual bool isBroadcast() const
    {
        return IN6_IS_ADDR_MC_LINKLOCAL(&ipv6);
    }

    /**
     * Multicast
     */
    inline virtual bool isMulticast() const
    {
        return IN6_IS_ADDR_MULTICAST(&ipv6);
    }

    /**
     * Unspecified ipv6 address
     */
    inline virtual bool isAny() const
    {
        return (IN6_IS_ADDR_UNSPECIFIED(&ipv6));
    }

    /**
     * calculate distance between _this_ address and address a2 and return
     * it as int.
     * This method is limited, it only calculates distance that fit in 32 bit
     * number
     */
    inline virtual int distance(const Inet6Addr &a2)
    {
        uint32_t *d1 = (uint32_t *)(&ipv6);
        uint32_t *d2 = (uint32_t *)(&(a2.ipv6));
        return *d2 - *d1 + 1;
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
        return (((uint32_t*)(&ipv6))[0] == 0xffffffff &&
                ((uint32_t*)(&ipv6))[1] == 0xffffffff &&
                ((uint32_t*)(&ipv6))[2] == 0xffffffff &&
                ((uint32_t*)(&ipv6))[3] == 0xffffffff);
    }

    /*****************************************************************/

    inline friend Inet6Addr operator&(const Inet6Addr &addr,
                                     const Inet6Addr &mask)
    {
        struct in6_addr res;
        for (int i=0; i<4; ++i)
            ((uint32_t*)(&res))[i] = 
                htonl(ntohl(((uint32_t*)(&(addr.ipv6)))[i]) &
                      ntohl(((uint32_t*)(&(mask.ipv6)))[i]));
        return Inet6Addr(&res);
    }

    inline friend Inet6Addr operator|(const Inet6Addr &addr,
                                     const Inet6Addr &mask)
    {
        struct in6_addr res;
        for (int i=0; i<4; ++i)
            ((uint32_t*)(&res))[i] = 
                htonl(ntohl(((uint32_t*)(&(addr.ipv6)))[i]) |
                      ntohl(((uint32_t*)(&(mask.ipv6)))[i]));
        return Inet6Addr(&res);
    }

    inline friend Inet6Addr operator+(const Inet6Addr &addr, int increment)
    {
        struct in6_addr res;
        Inet6Addr::_copy_in6_addr(&res, &(addr.ipv6) );
        ((uint32_t*)(&res))[3] =
            htonl(ntohl( ((uint32_t*)(&(addr.ipv6)))[3] + increment));
        return Inet6Addr(&res);
    }

    inline friend Inet6Addr operator-(const Inet6Addr &addr,int decrement)
    {
        struct in6_addr res;
        Inet6Addr::_copy_in6_addr(&res, &(addr.ipv6) );
        ((uint32_t*)(&res))[3] =
            htonl(ntohl( ((uint32_t*)(&(addr.ipv6)))[3] - decrement));
        return Inet6Addr(&res);
    }

    inline Inet6Addr& operator=(const Inet6Addr &addr)
    {
        Inet6Addr::_copy_in6_addr(&ipv6, &(addr.ipv6) );
        return *this;
    }

    inline friend bool operator<(const Inet6Addr &a, const Inet6Addr &b)
    {
        return (ntohl(((uint32_t*)(&(a.ipv6)))[3]) <
                ntohl(((uint32_t*)(&(b.ipv6)))[3]));
    }

    inline friend bool operator>(const Inet6Addr &a, const Inet6Addr &b)
    {
        return (ntohl(((uint32_t*)(&(a.ipv6)))[3]) >
                ntohl(((uint32_t*)(&(b.ipv6)))[3]));
    }

    inline friend bool operator==(const Inet6Addr &a, const Inet6Addr &b)
    {
        return (IN6_ARE_ADDR_EQUAL(&(a.ipv6), &(b.ipv6)));
    }

    inline friend bool operator!=(const Inet6Addr &a, const Inet6Addr &b)
    {
        return (!(IN6_ARE_ADDR_EQUAL(&(a.ipv6), &(b.ipv6))));
    }
    
};

}

#endif
