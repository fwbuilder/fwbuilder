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


#ifndef _ADDRESS_SPEC_H_
#define _ADDRESS_SPEC_H_

#include <map>
#include <list>
#include <string>


class AddressSpec
{
public:
    
    typedef enum {
        UNKNOWN,
        ANY,
        HOST_NAME,
        HOST_ADDRESS,
        NETWORK_ADDRESS,
        SPECIAL_ADDRESS,
        INTERFACE_NAME,
        INTERFACE_NETWORK,
        INTERFACE_BROADCAST,
        TABLE } address_type;

    address_type at;
    bool neg;
    std::string address;
    std::string netmask;

    AddressSpec()
    { at = UNKNOWN; neg = false;  address = ""; netmask = ""; }

    AddressSpec(const AddressSpec &other)
    {
        at = other.at;
        neg = other.neg;
        address = other.address;
        netmask = other.netmask;
    }
    
    AddressSpec(address_type _at, bool _neg, const std::string _addr, const std::string _nm)
    { at = _at; neg= _neg; address = _addr; netmask = _nm; }
};


#endif
