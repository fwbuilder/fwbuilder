/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: InetAddr.h 966 2006-08-18 03:59:32Z vkurland $


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

    public:
    
    explicit Inet6Addr() { ipv6 = IN6ADDR_ANY_INIT; }
        
    virtual ~Inet6Addr() {}

    Inet6Addr(const char *data) throw(FWException);
    Inet6Addr(const struct in6_addr*) throw(FWException);
    explicit Inet6Addr(const std::string&)
        throw(FWException, FWNotSupportedException);
    Inet6Addr(const Inet6Addr &);
    
    static inline Inet6Addr getAny()
    {
        return Inet6Addr();
    }

    static inline Inet6Addr getAllOnes()
    {
        struct in6_addr allones;
        allones.s_addr32[0] = 0xffffffff;
        allones.s_addr32[1] = 0xffffffff;
        allones.s_addr32[2] = 0xffffffff;
        allones.s_addr32[3] = 0xffffffff;
        return Inet6Addr(&allones);
    }

    static inline Inet6Addr getLoopbackAddr()
    {
        struct in6_addr loopback;
        loopback = htonl(IN6ADDR_LOOPBACK_INIT);
        return Inet6Addr(&loopback);
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
        return IN6_IS_ADDR_MC_LINKLOCAL(ipv6);
    }

    /**
     * Multicast
     */
    inline virtual bool isMulticast() const
    {
        return IN6_IS_ADDR_MULTICAST(ntohl(ipv6));
    }

    /**
     * Unspecified ipv6 address
     */
    inline virtual bool isAny() const
    {
        return (IN6_IS_ADDR_UNSPECIFIED(ipv6));
    }

    /**
     * calculate distance between _this_ address and address a2 and return
     * it as int.
     * This method is limited, it only calculates distance between two
     * ipv6 addresses 
     */
    inline virtual int distance(const Inet6Addr &a2)
    {
        return a2.to32BitInt() - to32BitInt() + 1;
    }

    /*****************************************************************/

    inline friend Inet6Addr operator&(const Inet6Addr &addr,
                                     const Inet6Addr &mask)
    {
        struct in6_addr res;
        for (int i=0; i<4; ++i)
            res.s_addr32[i] =
                htonl(ntohl(addr.ipv6.s_addr32[i]) & ntohl(mask.ipv6.s_addr32[i]));
        return Inet6Addr(&res);
    }

    inline friend Inet6Addr operator|(const Inet6Addr &addr,
                                     const Inet6Addr &mask)
    {
        struct in6_addr res;
        for (int i=0; i<4; ++i)
            res.s_addr32[i] =
                htonl(ntohl(addr.ipv6.s_addr32[i]) | ntohl(mask.ipv6.s_addr32[i]));
        return Inet6Addr(&res);
    }

    inline friend Inet6Addr operator+(const Inet6Addr &addr, int increment)
    {
        struct in6_addr res;
        res.s_addr = htonl(ntohl(addr.ipv6.s_addr32[3]) + increment);
        return Inet6Addr(&res);
    }

    inline friend Inet6Addr operator-(const Inet6Addr &addr,int decrement)
    {
        struct in6_addr res;
        res.s_addr = htonl(ntohl(addr.ipv6.s_addr32[3]) - decrement);
        return Inet6Addr(&res);
    }

    inline Inet6Addr& operator=(const Inet6Addr &addr)
    {
        for (int i=0; i<4; ++i)
            ipv6.s_addr32[i] = addr.ipv6.s_addr32[i];
        return *this;
    }

    inline friend bool operator<(const Inet6Addr &a, const Inet6Addr &b)
    {
        return (ntohl(a.ipv6.s_addr32[3] ) < ntohl( b.ipv6.s_addr32[3]));
    }

    inline friend bool operator>(const Inet6Addr &a, const Inet6Addr &b)
    {
        return (ntohl(a.ipv6.s_addr32[3] ) > ntohl( b.ipv6.s_addr32[3]));
    }

    inline friend bool operator==(const Inet6Addr &a, const Inet6Addr &b)
    {
        return (IN6_ARE_ADDR_EQUAL(a.ipv6, b.ipv6));
    }

    inline friend bool operator!=(const Inet6Addr &a, const Inet6Addr &b)
    {
        return (!(IN6_ARE_ADDR_EQUAL(a.ipv6, b.ipv6)));
    }
    
};

/*
 * class Inet6Netmask represents netmask. The only difference between it
 * and its base class InetAddr is that its constructor can accept
 * an integer that defines netmask length. Correspondingly, this class
 * has a method that returns the length of the netmask.
 */
class Inet6Netmask: public Inet6Addr
{
    public:

    explicit Inet6Netmask();
    explicit Inet6Netmask(const std::string &) throw(FWException);
    Inet6Netmask(const char *data) throw(FWException);
    Inet6Netmask(int n) throw(FWException);  // creates netmask 'n' bits long

    explicit Inet6Netmask(const Inet6Addr &);

    virtual ~Inet6Netmask();

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
        return (ipv6.s_addr32[0] == 0xffffffff &&
                ipv6.s_addr32[1] == 0xffffffff &&
                ipv6.s_addr32[2] == 0xffffffff &&
                ipv6.s_addr32[3] == 0xffffffff);
    }
};

}

#endif
