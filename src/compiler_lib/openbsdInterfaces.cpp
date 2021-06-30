/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#include "openbsdInterfaces.h"

#include "fwbuilder/Interface.h"

#include <QRegExp>

using namespace libfwbuilder;


bool openbsdInterfaces::parseVlan(const QString &name, QString *base_name, int *vlan_id)
{
    QRegExp vlan_name_pattern(QRegExp("(vlan)(\\d{1,})"));
    if (vlan_name_pattern.indexIn(name) != -1)
    {
        if (base_name!=nullptr) *base_name = vlan_name_pattern.cap(1);
        if (vlan_id!=nullptr) *vlan_id = vlan_name_pattern.cap(2).toInt();
        return true;
    }
    return false;
}

bool openbsdInterfaces::manageIpAddresses(Interface *intf,
                                          QStringList &update_addresses,
                                          QStringList &ignore_addresses)
{
    if (intf->isDyn())
    {
        // We never manage dynamic interfaces on OpenBSD
        // But we should add them to the list when rc.conf output format is
        // implemented, just like it is now done for FreeBSD
        return false;
    } else
        return interfaceProperties::manageIpAddresses(
            intf, update_addresses, ignore_addresses);
}

