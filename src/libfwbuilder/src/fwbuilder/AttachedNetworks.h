/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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


#ifndef _ATTACHEDNETWORKS_HH_
#define _ATTACHEDNETWORKS_HH_

#include "fwbuilder/MultiAddress.h"

namespace libfwbuilder
{

class AttachedNetworks : public MultiAddress
{
    private:
    
    void addNetworkObject(const InetAddrMask &addr_mask);
    
    public:

    DECLARE_FWOBJECT_SUBTYPE(AttachedNetworks);

    DECLARE_DISPATCH_METHODS(AttachedNetworks);
    
    AttachedNetworks();

    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr xml_parent_node);
    virtual void loadFromSource(bool ipv6, FWOptions *options,
                                bool test_mode=false);

    virtual std::string getSourceName();
};

}


#endif

