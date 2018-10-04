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

#include "junosInterfaces.h"

#include "fwbuilder/Interface.h"

#include <QDebug>
#include <QObject>
#include <QRegExp>

using namespace std;
using namespace libfwbuilder;


bool junosInterfaces::parseVlan(const QString &name, QString *base_name, int *vlan_id)
{
    QRegExp vlan_name_pattern("unit (\\d{1,})");
    if (vlan_name_pattern.indexIn(name) != -1)
    {
        if (base_name!=nullptr) *base_name = QString("unit");
        if (vlan_id!=nullptr) *vlan_id = vlan_name_pattern.cap(1).toInt();
        return true;
    }
    return false;
}

// simple name validation: DOES allow space and "-"
bool junosInterfaces::basicValidateInterfaceName(Interface *,
                                               const QString &obj_name,
                                               QString &err)
{
    (void) obj_name; (void) err; // Unused

    return true;
}
    
bool junosInterfaces::isValidVlanInterfaceName(const QString &subint_name,
                                                   const QString &parent_name,
                                                   QString &err)
{
    (void) parent_name; // Unused

    if (!looksLikeVlanInterface(subint_name))
    {
        err = QObject::tr("'%1' is not a valid unit name")
            .arg(subint_name);
        return false;
    }

    QString parent_name_from_regex;
    int vlan_id;
    if (parseVlan(subint_name, &parent_name_from_regex, &vlan_id))
    {
        if (vlan_id > 16384)
        {
            err = QObject::tr("'%1' looks like a name of a unit "
                              "but the unit number it defines is outside of the valid "
                              "range.").arg(subint_name);
            return false;
        }
    }

    return true;
}
