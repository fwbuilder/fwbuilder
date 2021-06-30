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

#ifndef __IPV4_HH_FLAG__
#define __IPV4_HH_FLAG__

#include <string>

#include "fwbuilder/InetAddrMask.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/ObjectMatcher.h"

namespace libfwbuilder
{

class IPv4 : public Address
{
public:
    
    IPv4();
    virtual ~IPv4();

    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr xml_parent_node);

    virtual unsigned int dimension()  const { return 1; }
    
    DECLARE_FWOBJECT_SUBTYPE(IPv4);

    DECLARE_DISPATCH_METHODS(IPv4);
    
    virtual bool hasInetAddress() const { return true; }

    /**
     * similar to hasInetAddress() but counts addresses
     */
    virtual int countInetAddresses(bool) const { return 1; }
    
    virtual const Address* getAddressObject() const { return this; }

    virtual void setAddress(const InetAddr &a);
    virtual void setNetmask(const InetAddr &nm);
    virtual void setAddressNetmask(const std::string& s);

    virtual void dump(std::ostream &f,bool recursive,bool brief,int offset=0) const;

    virtual bool isPrimaryObject() const;

};

}

#endif


