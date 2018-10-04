/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@fwbuilder.org>

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


#ifndef __GEN_ADDRESS_HH_FLAG__
#define __GEN_ADDRESS_HH_FLAG__

#include "fwbuilder/FWObject.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/InetAddrMask.h"

namespace libfwbuilder
{

/**
 * This is base class for all objects that can have an address and can
 * be a child of RuleElementSrc, RuleElementDst and other rule
 * elements holding addresses.  It is never used on it's own, only
 * it's subclasses are used.
 *
 * TODO: we might need to derive ObjectGroup and AddressRange from Address,
 * but this requires lot more testing
 */
class Address : public FWObject
{
protected:
    InetAddrMask*  inet_addr_mask;

public:

    DECLARE_FWOBJECT_SUBTYPE(Address);

    DECLARE_DISPATCH_METHODS(Address);
    
    Address();
    Address(const Address&);
    virtual ~Address();

//    Address(const std::string& addr,const std::string& mask);
//    Address(const std::string &s);

    virtual FWObject& shallowDuplicate(const FWObject *obj,
                                       bool preserve_id = true);

    const InetAddrMask* getInetAddrMaskObjectPtr() const;

    /**
     * This method returns true if object "owns" ip address. Owning
     * in this context means that the object itself has an address, rather
     * than one of its child objects. In this sense Firewall, Host,
     * Interface do not "own" ip address, whereas IPv4, Network, IPv6
     * NetworkIPv6 do.
     */
    virtual bool hasInetAddress() const;

    /**
     * similar to hasInetAddress() but counts addresses
     */
    virtual int countInetAddresses(bool skip_loopback) const;
    
    /**
     * returns const pointer to internal InetAddr object. Some objects
     * that inherit this class may return nullptr if they do not have
     * their own IP address (examples: physAddress or Interface with
     * no child IPv4 object). Using exclusively method that returns
     * pointer rather than reference to the object allows us to
     * distinguish between when an object has address 0.0.0.0 or has no
     * address at all.
     */
    virtual const InetAddr* getAddressPtr() const;
    virtual const InetAddr* getNetmaskPtr() const;

    virtual const InetAddr* getNetworkAddressPtr() const;
    virtual const InetAddr* getBroadcastAddressPtr() const;

    virtual void setAddress(const InetAddr &a);
    virtual void setNetmask(const InetAddr &nm);
    virtual void setAddressNetmask(const std::string& s);

    virtual const Address* getAddressObject() const;
    virtual unsigned int dimension()  const;
    bool belongs(const InetAddr &) const;
    
    virtual FWReference* createRef();
    virtual bool cmp(const FWObject *obj, bool recursive=false);

    bool isAny() const;

    virtual bool isPrimaryObject() const { return true; }
};

}


#endif

