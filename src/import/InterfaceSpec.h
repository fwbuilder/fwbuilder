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


#ifndef _INTERFACE_SPEC_H_
#define _INTERFACE_SPEC_H_

#include <map>
#include <list>
#include <string>


class InterfaceSpec
{
public:

    bool neg;
    std::string name;
    std::string inet_address;
    std::string inet_netmask;
    std::string inet_broadcast;
    std::string inet6_address;
    std::string inet6_prefixlen;
    bool status;   // up / down
    std::list<std::string> groups;
    int mtu;
    
    InterfaceSpec()
    {
        neg = false;
        name = "";
        inet_address = "";
        inet_netmask = "";
        inet_broadcast = "";
        inet6_address = "";
        inet6_prefixlen = "";
        status = false;
        groups.clear();
        mtu = 0;
    }

    InterfaceSpec(const InterfaceSpec &other)
    {
        neg = other.neg;
        name = other.name;
        inet_address = other.inet_address;
        inet_netmask = other.inet_netmask;
        inet_broadcast = other.inet_broadcast;
        inet6_address = other.inet6_address;
        inet6_prefixlen = other.inet6_prefixlen;
        status = other.status;
        groups = other.groups;
        mtu = other.mtu;
    }
    
    InterfaceSpec(bool _neg, const std::string _name)
    {
        neg = _neg;
        name = _name;
        inet_address = "";
        inet_netmask = "";
        inet_broadcast = "";
        inet6_address = "";
        inet6_prefixlen = "";
        status = false;
        groups.clear();
        mtu = 0;
    }
};

#endif
