/* 

                          Firewall Builder

                 Copyright (C) 2002-2011 NetCitadel, LLC

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

#include "inspectionClassMap.h"
#include "inspectionProtocol.h"

#include <sstream>

using namespace std;

bool InspectionClassMap::isDefault()
{
    InspectionProtocol *ip = InspectionProtocol::protocols[fixup_name];
    if (ip!=nullptr) return (ip->par1==port1 && ip->par2==port2);
    return false;
}

string InspectionClassMap::getIPProtocol()
{
    InspectionProtocol *ip = InspectionProtocol::protocols[fixup_name];
    if (ip!=nullptr) return ip->ip_proto;
    return "";
}

string InspectionClassMap::getPrintableName()
{
    InspectionProtocol *ip = InspectionProtocol::protocols[fixup_name];
    if (ip!=nullptr) return ip->printable_name;
    return "";
}

string InspectionClassMap::getMatchCommand()
{
    ostringstream  res;
    res << "match port " << getIPProtocol() << " ";
    if (port1!=0 && port2==0)
        res << "eq " << port1;
    if (port1!=0 && port1==port2)
        res << "eq " << port1;
    if (port1!=0 && port2!=0 && port1!=port2)
        res << "range " << port1 << " " << port2;
    res << endl;
    return res.str();
}

