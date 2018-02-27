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

/*
 * This class is a holder of a pair address / netmask.
 * It can act both as a container for the address/netmask configuration
 * data (such as for an interface) or as a network.
 *
 * TODO(vadim): need better name. InetNetwork ? InetAddrMaskPair ?
 */

#ifndef __INETADDRMASK_HH_FLAG__
#define __INETADDRMASK_HH_FLAG__

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

#include "fwbuilder/FWException.h"
#include "fwbuilder/InetAddr.h"

namespace libfwbuilder
{

    class InetAddrMask;

    std::vector<InetAddrMask> getOverlap(const InetAddrMask &n1,
                                         const InetAddrMask &n2);
    std::vector<InetAddrMask> substract(const InetAddrMask &n1,
                                        const InetAddrMask &n2);
    std::vector<InetAddrMask> convertAddressRange(const InetAddr &start,
                                                  const InetAddr &end);
    bool _convert_range_to_networks(const InetAddr &start,
                                    const InetAddr &end,
                                    std::vector<InetAddrMask> &res);

    bool operator==(const InetAddrMask &a, const InetAddrMask &b);
    bool operator<(const InetAddrMask &a, const InetAddrMask &b);

class InetAddrMask
{
    
protected:
    
    InetAddr* address;
    InetAddr* netmask;
    InetAddr* broadcast_address;
    InetAddr* network_address;
    InetAddr* last_host;

    explicit InetAddrMask(bool no_address);

public:

    InetAddrMask();
    InetAddrMask(const InetAddr&, const InetAddr&);
    InetAddrMask(const std::string &s);
    InetAddrMask(const InetAddrMask&);
    virtual ~InetAddrMask();
    void setNetworkAndBroadcastAddress();

    virtual const InetAddr* getAddressPtr() const { return address; }
    virtual const InetAddr* getNetmaskPtr() const { return netmask; }

    virtual const InetAddr* getNetworkAddressPtr() const {
        return network_address; }
    virtual const InetAddr* getBroadcastAddressPtr() const {
        return broadcast_address; }

    const InetAddr* getFirstHostPtr() const {
        return network_address;}
    const InetAddr* getLastHostPtr() const{
        return last_host;}
    
    virtual void setAddress(const InetAddr &a);
    virtual void setNetmask(const InetAddr &nm);

    virtual unsigned int dimension()  const;

    bool isAny();
    
    InetAddrMask& operator=(const InetAddrMask &o);
    bool operator<(const InetAddrMask &b);

    friend bool operator==(const InetAddrMask &a, const InetAddrMask &b);
    friend bool operator<(const InetAddrMask &a, const InetAddrMask &b);

    virtual std::string toString() const
    {
        return address->toString()+"/"+netmask->toString();
    }

    bool belongs(const InetAddr &) const;

    /**
     * calculates overlapping part of two networks n1 and
     * n2. Overlapping part is defined as in sets: if we think of
     * networks as sets of addresses, then intersection contains all
     * addresses that belong to both networks
     */
    friend std::vector<InetAddrMask> getOverlap(const InetAddrMask &n1,
                                                const InetAddrMask &n2);

    /**
     * substract network n2 from the network n1. The meaning of this
     * operation is opposite to getOverlap: it returns all addresses
     * that belong to n1 but do not belong to n2
     */
    friend std::vector<InetAddrMask> substract(const InetAddrMask &n1,
                                               const InetAddrMask &n2);

    /**
     * converts address range (defined by its start and end) to a
     * bunch of networks
     */
    friend std::vector<InetAddrMask> convertAddressRange(const InetAddr &start,
                                                         const InetAddr &end);
};

}

#endif

