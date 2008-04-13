/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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

#ifndef __INETADDR_HH_FLAG__
#define __INETADDR_HH_FLAG__

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

#include <fwbuilder/FWException.h>

namespace libfwbuilder
{

/**
 * Class InetAddr is a wrapper for struct inet_addr
 *
 */
class InetAddr
{
    protected:

    friend class InetAddrMask;

    // Address in network order
    struct in_addr ipv4;

    public:
    
    explicit InetAddr() { ipv4.s_addr = 0; }
        
    virtual ~InetAddr() {}

    InetAddr(const char *data) throw(FWException);
    InetAddr(const struct in_addr*) throw(FWException);
    explicit InetAddr(const std::string&)
        throw(FWException, FWNotSupportedException);
    InetAddr(const InetAddr &);

    static inline InetAddr getAny()
    {
        return InetAddr();
    }

    static inline InetAddr getAllOnes()
    {
        struct in_addr allones;
        allones.s_addr = 0xffffffff;
        return InetAddr(&allones);
    }

    static inline InetAddr getLoopbackAddr()
    {
        struct in_addr loopback;
        loopback.s_addr = htonl(INADDR_LOOPBACK);
        return InetAddr(&loopback);
    }

    inline virtual std::string toString() const
    {
        return std::string(strdup(inet_ntoa(ipv4)));
    }

    /**
     * Broadcast :  255.255.255.255
     */
    inline virtual bool isBroadcast() const
    {
        return ipv4.s_addr == INADDR_BROADCAST;
    }

    /**
     * Multicast :  224.0.0.0 - 239.0.0.0
     */
    inline virtual bool isMulticast() const
    {
        return IN_MULTICAST(ntohl(ipv4.s_addr));
    }

    /**
     * INADDR_ANY: 0
     */
    inline virtual bool isAny() const
    {
        return ipv4.s_addr == INADDR_ANY;
    }

    /**
     * calculate distance between _this_ address and address a2 and return
     * it as int
     */
    inline virtual int distance(const InetAddr &a2) const
    {
        return ntohl(a2.ipv4.s_addr) - ntohl(ipv4.s_addr) + 1;
    }

    /*****************************************************************/

    inline friend InetAddr operator&(const InetAddr &addr,
                                     const InetAddr &mask)
    {
        struct in_addr res;
        res.s_addr = htonl(ntohl(addr.ipv4.s_addr) & ntohl(mask.ipv4.s_addr));
        return InetAddr(&res);
    }

    inline friend InetAddr operator|(const InetAddr &addr,
                                     const InetAddr &mask)
    {
        struct in_addr res;
        res.s_addr = htonl(ntohl(addr.ipv4.s_addr) | ntohl(mask.ipv4.s_addr));
        return InetAddr(&res);
    }

    inline friend InetAddr operator+(const InetAddr &addr, int increment)
    {
        struct in_addr res;
        res.s_addr = htonl(ntohl(addr.ipv4.s_addr) + increment);
        return InetAddr(&res);
    }

    inline friend InetAddr operator-(const InetAddr &addr,int decrement)
    {
        struct in_addr res;
        res.s_addr = htonl(ntohl(addr.ipv4.s_addr) - decrement);
        return InetAddr(&res);
    }

    inline InetAddr& operator=(const InetAddr &addr)
    {
        ipv4.s_addr = addr.ipv4.s_addr;
        return *this;
    }

    inline friend bool operator<(const InetAddr &a, const InetAddr &b)
    {
        return (ntohl( a.ipv4.s_addr ) < ntohl( b.ipv4.s_addr ));
    }

    inline friend bool operator>(const InetAddr &a, const InetAddr &b)
    {
        return (ntohl( a.ipv4.s_addr ) > ntohl( b.ipv4.s_addr ));
    }

    inline friend bool operator==(const InetAddr &a, const InetAddr &b)
    {
        return a.ipv4.s_addr == b.ipv4.s_addr;
    }

    inline friend bool operator!=(const InetAddr &a, const InetAddr &b)
    {
        return a.ipv4.s_addr != b.ipv4.s_addr;
    }
    
    inline friend InetAddr operator~(const InetAddr &a)
    {
        struct in_addr res;
        res.s_addr = htonl(~(ntohl(a.ipv4.s_addr)));
        return InetAddr(&res);
    }

};

/*
 * class InetNetmask represents netmask. The only difference between it
 * and its base class InetAddr is that its constructor can accept
 * an integer that defines netmask length. Correspondingly, this class
 * has a method that returns the length of the netmask.
 */
class InetNetmask: public InetAddr
{
    public:

    explicit InetNetmask();
    explicit InetNetmask(const std::string &) throw(FWException);
    InetNetmask(const char *data) throw(FWException);
    InetNetmask(int n) throw(FWException);  // creates netmask 'n' bits long

    explicit InetNetmask(const InetAddr &);

    virtual ~InetNetmask();

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
        return ipv4.s_addr == INADDR_BROADCAST;
    }

};

}

#endif
