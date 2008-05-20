/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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

#ifndef __NETWORK_HH_FLAG__
#define __NETWORK_HH_FLAG__

#include <fwbuilder/Address.h>
#include <fwbuilder/InetAddrMask.h>
#include <fwbuilder/InetAddr.h>
#include <fwbuilder/InetAddrMask.h>

namespace libfwbuilder
{

    class Network : public Address
{
public:
    
    Network();
    Network(const FWObject *root,bool prepopulate);
    Network(Network &);
    Network(const std::string &);
    virtual ~Network();

    bool isValidRoutingNet() const;

    virtual void       fromXML (xmlNodePtr parent) throw(FWException);
    virtual xmlNodePtr toXML   (xmlNodePtr xml_parent_node) throw(FWException);
    
    DECLARE_FWOBJECT_SUBTYPE(Network);

    virtual const bool hasInetAddress() const { return true; }

    virtual const Address* getAddressObject() const { return this; }

    virtual void setAddress(const InetAddr &a);
    virtual void setNetmask(const InetAddr &nm);
    virtual void setAddressNetmask(const std::string& s);

};

}

#endif // __NETWORK_HH_FLAG__




