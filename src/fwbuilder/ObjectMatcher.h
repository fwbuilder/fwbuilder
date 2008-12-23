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

#ifndef  __OBJECTMATCHER_HH_FLAG__
#define  __OBJECTMATCHER_HH_FLAG__

#include <string>
#include <list>

#include <fwbuilder/libfwbuilder-config.h>

namespace libfwbuilder {
    class FWObject;
    class Address;
    class IPv4;
    class IPv6;
    class Network;
    class NetworkIPv6;
    class AddressRange;
    class Host;
    class physAddress;
    class Firewall;
    class Interface;
};


namespace libfwbuilder
{

    class ObjectMatcher {

        bool recognize_broadcasts;
        bool recognize_multicasts;
        bool ipv6;
        
        bool checkComplexMatchForSingleAddress(Address *obj1, FWObject *obj2);
        
public:
        ObjectMatcher(bool _broadcasts, bool _multicasts, bool _ipv6)
        {
            recognize_broadcasts = _broadcasts;
            recognize_multicasts = _multicasts;
            ipv6 = _ipv6;
        }

        /**
         * This method returns true if one of the following conditions is met:
         *
         * 1. obj1 is the same as obj2 (compares ID of both objects), or 
         * 2. obj1 is a child of obj2 on any depth, or
         * 3. address of obj1 matches that of any obj2's interfaces, or 
         * 4. address of obj1 is a broadcast address of one of 
         *    the interfaces of obj2
         * 5. address of obj1 is a broadcast (255.255.255.255)
         */
        bool complexMatch(Address *obj1, Address *obj2);
        
        bool checkComplexMatch(Interface *obj1, FWObject *obj2);

        bool checkComplexMatch(Network *obj1, FWObject *obj2);

        bool checkComplexMatch(NetworkIPv6 *obj1, FWObject *obj2);

        bool checkComplexMatch(IPv4 *obj1, FWObject *obj2);

        bool checkComplexMatch(IPv6 *obj1, FWObject *obj2);
        
        bool checkComplexMatch(Host *obj1, FWObject *obj2);
        
        bool checkComplexMatch(Firewall *obj1, FWObject *obj2);
        
        bool checkComplexMatch(AddressRange *obj1, FWObject *obj2);
        
        bool checkComplexMatch(physAddress *obj1, FWObject *obj2);
    
    };
};

#endif
