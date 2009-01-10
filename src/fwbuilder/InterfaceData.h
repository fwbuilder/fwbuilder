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

        InterfaceData();
        InterfaceData(const InterfaceData& other);
        InterfaceData(const libfwbuilder::Interface &iface);
        virtual ~InterfaceData();

/**
 *  this method is a collection of heuristics that allow us to assign
 *  a reasonable label to the interface based on firewall platform,
 *  name of the interface, its label and other parameters.
 */
        void guessLabel(const std::string &platform);

/**
 *  this method is a collection of heuristics that allow us to make an
 *  educated guess about interface's security level based on the
 *  firewall platform, name of the interface, its label and other
 *  parameters.
 */
        void guessSecurityLevel(const std::string &platform);

/**
 *  This method is a collection of heuristics that allow us to assign
 *  a reasonable security level to many interfaces based on firewall
 *  platform, their names and labels and other parameters. This method
 *  compares parameters of many interfaces and in certain cases can
 *  guess their relative security levels.
 */
        static void  guessSecurityLevel(const std::string &platform, 
                                        std::list<InterfaceData> &interfaces);

    };
}

#endif
