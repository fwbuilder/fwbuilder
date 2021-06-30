/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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

#ifndef __ADDRESSRANGE_HH_FLAG__
#define __ADDRESSRANGE_HH_FLAG__

#include "fwbuilder/Address.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/ObjectMatcher.h"

namespace libfwbuilder
{

class AddressRange : public Address 
{
    private:
    
    InetAddr start_address;
    InetAddr end_address;
    
    public:
    
    AddressRange();
    AddressRange(AddressRange &);

    const InetAddr &getRangeStart() const { return start_address; }
    const InetAddr &getRangeEnd() const   { return end_address;   }

    void setRangeStart(const InetAddr &o) { start_address = o; }
    void setRangeEnd(const InetAddr &o)   { end_address = o;   }

    bool isV4() const {
        return start_address.addressFamily() == AF_INET
                && end_address.addressFamily() == AF_INET;
    }

    bool isV6() const {
        return start_address.addressFamily() == AF_INET6
                && end_address.addressFamily() == AF_INET6;
    }

    /**
     * virtual methods inherited from Address
     */
    virtual bool hasInetAddress() const { return true; }
    virtual const InetAddr* getAddressPtr() const;
    virtual unsigned int dimension()  const;

    virtual void setAddress(const InetAddr &a);
    virtual void setNetmask(const InetAddr &nm);

    
    virtual FWObject& shallowDuplicate(const FWObject *obj, bool preserve_id);
    virtual bool cmp(const FWObject *obj, bool recursive=false);
   
    virtual void       fromXML (xmlNodePtr parent);
    virtual xmlNodePtr toXML   (xmlNodePtr xml_parent_node);

    virtual bool isPrimaryObject() const { return true; }

    DECLARE_FWOBJECT_SUBTYPE(AddressRange);

    DECLARE_DISPATCH_METHODS(AddressRange);
    
};

}

#endif // __ADDRESSRANGE_HH_FLAG__




