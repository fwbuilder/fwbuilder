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


#ifndef _ROUTE_SPEC_H_
#define _ROUTE_SPEC_H_

#include <map>
#include <list>
#include <string>

class RouteSpec
{
public:
    
    std::string iface;
    std::string address;
    std::string netmask;

    RouteSpec()
    { iface = ""; address = ""; netmask = ""; }

    RouteSpec(const RouteSpec &other)
    {
        iface = other.iface;
        address = other.address;
        netmask = other.netmask;
    }
    
    RouteSpec(const std::string _iface,
              const std::string _addr, const std::string _nm)
    { iface = _iface; address = _addr; netmask = _nm; }
};



#endif
