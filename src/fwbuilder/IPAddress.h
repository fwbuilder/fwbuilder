/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: IPAddress.h 966 2006-08-18 03:59:32Z vkurland $


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

#ifndef __IPADDRESS_HH_FLAG__
#define __IPADDRESS_HH_FLAG__

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

typedef unsigned int guint32;

namespace libfwbuilder
{

    class Netmask;

class IPAddress
{
    protected:

    friend class IPNetwork;

    unsigned int octets[4];
    virtual void validate() throw(FWException);
    
    public:
    
    explicit IPAddress();
    virtual ~IPAddress();

    IPAddress(const unsigned char *data, size_t len) throw(FWException);
    IPAddress(const struct in_addr *) throw(FWException);
    explicit IPAddress(const std::string &) throw(FWException, FWNotSupportedException);
    IPAddress(const IPAddress &);

    IPAddress& operator=(const IPAddress &);

    friend IPAddress operator+(const IPAddress &a,const IPAddress &b);
    friend IPAddress operator-(const IPAddress &a,const IPAddress &b);

    friend IPAddress operator+(const IPAddress &a,int increment);
    friend IPAddress operator-(const IPAddress &a,int decrement);

    IPAddress& operator=(const std::string &s) throw(FWException, FWNotSupportedException);

    IPAddress& addMask(const Netmask &b);

    friend bool operator<(const IPAddress &a, const IPAddress &b);
    friend bool operator==(const IPAddress &a, const IPAddress &b);
    unsigned const int operator[](size_t pos) const { return octets[pos]; }
    
    operator std::string() const { return toString(); }
    operator guint32() const { return to32BitInt(); }
    
    std::string toString() const;
    guint32 to32BitInt() const;

    /**
     * Broadcast :  255.255.255.255  and  0.0.0.0,
     */
    bool isBroadcast() const;

    /**
     * Multicast :  224.0.0.0 - 239.0.0.0
     */
    bool isMulticast() const;
};

class Netmask: public IPAddress
{
    protected:
    
    virtual void validate() throw(FWException);

    public:

    explicit Netmask();
    explicit Netmask(const std::string &) throw(FWException);
    Netmask(const unsigned char *data, size_t len) throw(FWException);
    Netmask(int n) throw(FWException);  // creates netmask with 'n' bits set to '1'

    /**
     * This constructor creates natural classful netmask for given IP address
     *
     * Cheatsheet:
     *                                   Net     Host    Total
     * Net      Addr                      Addr    Addr    Number
     * Class   Range      NetMask         Bits    Bits   of hosts
     * ----------------------------------------------------------
     * A        0-127    255.0.0.0         8      24     16777216
     * B      128-191    255.255.0.0      16      16        65536
     * C      192-254    255.255.255.0    24       8          256
     * 
     * (ref: RFC1375 & www.isi.edu/in-notes/iana/assignments/ipv4-address-space)
     *
     */
    explicit Netmask(const IPAddress &);

    virtual ~Netmask();

    /**
     * returns the "length" of the netmask, that is number of bits set to '1'
     * counting from left to right
     */ 
    int getLength() const;

    Netmask& operator=(const std::string &s) throw(FWException);
    friend Netmask operator~(const Netmask &nm);

    friend bool operator==(const Netmask &a, const Netmask &b);

};

class IPNetwork;
std::vector<IPNetwork> getOverlap(const IPNetwork &n1,const IPNetwork &n2);
std::vector<IPNetwork> substract(const IPNetwork &n1,const IPNetwork &n2);
std::vector<IPNetwork> convertAddressRange(const IPAddress &start,
                                           const IPAddress &end);

class IPNetwork
{
    private:
    
    IPAddress address    ;
    Netmask   netmask    ;
    int       bcast_bits ;

    static bool _convert_range_to_networks(const IPAddress &start,
					   const IPAddress &end,
					   std::vector<IPNetwork> &res);

    public:

    IPNetwork(const IPAddress &, const Netmask&, int bcast_bits=1);

    const IPAddress &getAddress         () const;
    const Netmask   &getNetmask         () const;
    IPAddress getBroadcastAddress () const;

    /**
     * Broadcast :  255.255.255.255  and  0.0.0.0,
     */
    bool isBroadcast() const;

    /**
     * Multicast :  224.0.0.0 - 239.0.0.0
     */
    bool isMulticast() const;

    IPNetwork& operator=(const IPNetwork &o);
    friend bool operator==(const IPNetwork &a, const IPNetwork &b);
    friend bool operator<(const IPNetwork &a, const IPNetwork &b);
    operator std::string() const { return address.toString()+"/"+netmask.toString(); }

    bool belongs(const IPAddress &) const;

    /**
     * calculates overlapping part of two networks n1 and
     * n2. Overlapping part is defined as in sets: if we think of
     * networks as sets of addresses, then intersection contains all
     * addresses that belong to both networks
     */
    friend std::vector<IPNetwork> getOverlap(const IPNetwork &n1,const IPNetwork &n2);

    /**
     * substract network n2 from the network n1. The meaning of this
     * operation is opposite to getOverlap: it returns all addresses
     * that belong to n1 but do not belong to n2
     */
    friend std::vector<IPNetwork> substract(const IPNetwork &n1,const IPNetwork &n2);

    /**
     * converts address range (defined by its start and end) to a
     * bunch of networks
     */
    friend std::vector<IPNetwork> convertAddressRange(const IPAddress &start,
                                                      const IPAddress &end);
};

class Interface;
class IPRoute
{
    public:

    IPRoute(const IPRoute &);
    IPRoute(const IPAddress &_dst, const Netmask &_nm, const IPAddress &_gw, const Interface *_intf,  bool _direct);
    virtual ~IPRoute();

    bool isDirect() const { return direct;}
    const Netmask   &getNetmask    () const { return nm;   }
    const IPAddress &getDestination() const { return dst;  }
    const IPAddress &getGateway    () const { return gw;   }

    /**
     * @return interface associated with this route, or
     * NULL if none.
     */
    const Interface *getInterface  () const { return intf; }
    
    private:
    
    Netmask   nm     ;
    IPAddress dst    ;
    IPAddress gw     ;
    const Interface  *intf;
    bool      direct ;
};

}

#endif

