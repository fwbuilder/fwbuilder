/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#include "pixInterfaces.h"

#include <QRegExp>

/*
 * http://www.cisco.com/en/US/docs/security/asa/asa70/command/reference/gl.html#wp1644971
 *
 * hostname(config)# interface gigabitethernet0/1.1
 * hostname(config-subif)# vlan 101
 * hostname(config-subif)# nameif dmz1
 * hostname(config-subif)# security-level 50
 * hostname(config-subif)# ip address 10.1.2.1 255.255.255.0
 * hostname(config-subif)# no shutdown
 * 
 */

bool pixInterfaces::parseVlan(const QString &name, QString *base_name, int *vlan_id)
{
    QRegExp vlan_name_pattern("([a-zA-Z-]+\\d{1,}(/\\d{1,})*)\\.(\\d{1,})");
    if (vlan_name_pattern.indexIn(name) != -1)
    {
        if (base_name!=nullptr) *base_name = vlan_name_pattern.cap(1);
        if (vlan_id!=nullptr) *vlan_id = vlan_name_pattern.cap(3).toInt();
        return true;
    }
    return false;
}

