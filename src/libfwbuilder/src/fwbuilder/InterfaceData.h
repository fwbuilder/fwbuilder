/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

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



#ifndef __INTERFACE_DATA_HH_
#define __INTERFACE_DATA_HH_


#include "fwbuilder/Interface.h"
#include "fwbuilder/physAddress.h"
#include "fwbuilder/InetAddrMask.h"
#include "fwbuilder/Inet6AddrMask.h"

#include <list>

namespace libfwbuilder
{
    class InterfaceData
    {
public:
        
        std::string id;
        std::string name;
        std::list<libfwbuilder::InetAddrMask*> addr_mask;
        bool ext;
        int snmp_type;
        int ostatus;
        int securityLevel;
        bool isDyn;
        bool isUnnumbered;
        bool isBridgePort;
        std::string mac_addr;
        std::string label;
        std::string networkZone;

        std::string interface_type;
        int vlan_id;
        std::list<InterfaceData*> subinterfaces;

        InterfaceData();
        InterfaceData(const InterfaceData& other);
        InterfaceData(const libfwbuilder::Interface &iface);
        InterfaceData& operator=(const InterfaceData&) = default;
        virtual ~InterfaceData();

    };
}

#endif
