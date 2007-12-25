/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: AddressRange.h 975 2006-09-10 22:40:37Z vkurland $


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

#include <fwbuilder/Address.h>
#include <fwbuilder/IPAddress.h>

namespace libfwbuilder
{

class AddressRange : public Address 
{
    private:
    
    IPAddress start_address;
    IPAddress end_address;
    
    public:
    
    AddressRange();
    AddressRange(const FWObject *root,bool prepopulate);
    AddressRange(AddressRange &);

    const IPAddress &getRangeStart() const { return start_address; }
    const IPAddress &getRangeEnd() const   { return end_address;   }

    void setRangeStart(const IPAddress &o) { start_address=o; }
    void setRangeEnd(const IPAddress &o)   { end_address=o;   }

    /**
     * virtual methods inherited from Address
     */
    virtual IPAddress getAddress() const;
    virtual Netmask   getNetmask() const;
    virtual guint32   dimension()  const;

    virtual void setAddress(const IPAddress &a);
    virtual void setNetmask(const Netmask   &nm);
    virtual void setAddress(const std::string &a);
    virtual void setNetmask(const std::string &nm);

    
    virtual FWObject& shallowDuplicate(const FWObject *obj, bool preserve_id) throw(FWException);
    virtual bool cmp(const FWObject *obj) throw(FWException);
   
    virtual void       fromXML (xmlNodePtr parent) throw(FWException);
    virtual xmlNodePtr toXML   (xmlNodePtr xml_parent_node) throw(FWException);
    
    DECLARE_FWOBJECT_SUBTYPE(AddressRange);
    
};

}

#endif // __ADDRESSRANGE_HH_FLAG__




