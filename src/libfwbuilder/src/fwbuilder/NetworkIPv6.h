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

#ifndef __NETWORKIPV6_HH_FLAG__
#define __NETWORKIPV6_HH_FLAG__

#include "fwbuilder/Address.h"
#include "fwbuilder/InetAddrMask.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/InetAddrMask.h"
#include "fwbuilder/ObjectMatcher.h"

namespace libfwbuilder
{

    class NetworkIPv6 : public Address
{
public:
    
    NetworkIPv6();
    NetworkIPv6(NetworkIPv6 &);
    NetworkIPv6(const std::string &);
    virtual ~NetworkIPv6();

    bool isValidRoutingNet() const;

    virtual void       fromXML (xmlNodePtr parent);
    virtual xmlNodePtr toXML   (xmlNodePtr xml_parent_node);
    
    DECLARE_FWOBJECT_SUBTYPE(NetworkIPv6);

    DECLARE_DISPATCH_METHODS(NetworkIPv6);
    
    virtual FWObject& shallowDuplicate(const FWObject *obj,
                                       bool preserve_id = true);

    virtual bool hasInetAddress() const { return true; }

    /**
     * similar to hasInetAddress() but counts addresses
     */
    virtual int countInetAddresses(bool) const { return 1; }
    
    virtual const Address* getAddressObject() const { return this; }

    virtual void setAddress(const InetAddr &a);
    virtual void setNetmask(const InetAddr &nm);
    virtual void setAddressNetmask(const std::string& s);

    virtual bool isPrimaryObject() const { return true; }

};

}

#endif // __NETWORK_HH_FLAG__




