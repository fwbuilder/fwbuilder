/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

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

#include "config.h"

#include "BaseObjectGroup.h"

#include <iostream>
#include <sstream>

using namespace libfwbuilder;
using namespace std;

map<string,int>  BaseObjectGroup::nc;

const char *BaseObjectGroup::TYPENAME={"BaseObjectGroup"};

string BaseObjectGroup::registerGroupName(const std::string &prefix)
{
    ostringstream  str;
    str << prefix;

    switch (getObjectGroupType())
    {
    case UNKNOWN:      str << ".unknown"; break;
    case NETWORK:      str << ".net";     break;
    case PROTO:        str << ".proto";   break;
    case ICMP_TYPE:    str << ".icmp";    break;
    case TCP_SERVICE:  str << ".tcp";     break;
    case UDP_SERVICE:  str << ".udp";     break;
    }

    int n=nc[str.str()];
    nc[str.str()]=n+1;
    str << "." << n;
    return str.str();
}

void BaseObjectGroup::setName(const std::string &prefix)
{
    FWObject::setName( registerGroupName(prefix) );
}

bool BaseObjectGroup::isServiceGroup()
{
    switch (getObjectGroupType())
    {
    case PROTO:        return true;
    case ICMP_TYPE:    return true;
    case TCP_SERVICE:  return true;
    case UDP_SERVICE:  return true;
    default:           return false;
    }
    return false;
}

bool BaseObjectGroup::isObjectGroup()
{
    switch (getObjectGroupType())
    {
    case UNKNOWN:      return true;
    case NETWORK:      return true;
    default:           return false;
    }
    return false;
}

string BaseObjectGroup::getSrvTypeName()
{
    switch (getObjectGroupType())
    {
    case ICMP_TYPE:    return "icmp";
    case TCP_SERVICE:  return "tcp";
    case UDP_SERVICE:  return "udp";
    default:           break;
    }
    return "";
}

string BaseObjectGroup::toString()  throw(FWException)
{
    return "";
}

