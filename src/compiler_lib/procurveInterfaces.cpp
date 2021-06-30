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

#include "procurveInterfaces.h"

#include "fwbuilder/Interface.h"

#include <QRegExp>
#include <QObject>

using namespace std;
using namespace libfwbuilder;


// simple name validation: does not allow space and "-"
// However PoCurve permits space. What about "-" ?
bool procurveInterfaces::basicValidateInterfaceName(Interface*,
                                                    const QString &obj_name,
                                                    QString &err)
{
    if (obj_name.indexOf('-') != -1)
    {
        err = QObject::tr("Interface name '%1' can not contain \"-\"").arg(obj_name);
        return false;
    }
    err = "";
    return true;
}

/*
 * The difference is that in ProCurve, vlan interfaces have names like
 * "VLAN 2".  We should permit white space between "vlan" and the
 * number. It is unclear whether "vlan" and "Vlan" are allowed besides
 * "VLAN".
 */
bool procurveInterfaces::parseVlan(
    const QString &name, QString *base_name, int *vlan_id)
{
    if (name == "DEFAULT_VLAN")
    {
        if (base_name!=nullptr) *base_name = "vlan";
        if (vlan_id!=nullptr) *vlan_id = 1;
        return true;
    }

    // Procurve SNMP reports vlan interface names without space
    QRegExp vlan_name_pattern("(vlan|Vlan|VLAN) *(\\d{1,})");
    if (vlan_name_pattern.indexIn(name) != -1)
    {
        if (base_name!=nullptr) *base_name = vlan_name_pattern.cap(1);
        if (vlan_id!=nullptr) *vlan_id = vlan_name_pattern.cap(2).toInt();
        return true;
    }
    return false;
}

/*
 * In ProCurve, parent interface and vlan interface names have nothing
 * in common and can not be verified.
 */
bool procurveInterfaces::isValidVlanInterfaceName(const QString &subint_name,
                                                  const QString & /*UNUSED parent_name */,
                                                  QString &err)
{
    if (!looksLikeVlanInterface(subint_name))
    {
        err = QObject::tr("'%1' is not a valid vlan interface name").arg(subint_name);
        return false;
    }

    QString parent_name_from_regex;
    int vlan_id;
    if (parseVlan(subint_name, &parent_name_from_regex, &vlan_id))
    {
        if (vlan_id > 4095)
        {
            err = QObject::tr("'%1' looks like a name of a vlan interface "
                              "but vlan ID it defines is outside of the valid range."
                              "").arg(subint_name);
            return false;
        }
    }

    return true;
}

/*
 * many switch ports can be part of the same vlan. It would be ideal
 * if I could make interface objects that represent switch ports as
 * subinterfaces of a vlan interface. Unfortunately this is reverse of
 * our normal model, where vlans are subinterfaces of ethernet
 * interface objects. Until I figure this out, there will be no
 * restrictions on ProCurve interface objects.
 */
bool procurveInterfaces::validateInterface(FWObject * /*UNUSED target */,
                                           FWObject * /*UNUSED intf */,
                                           bool  /*UNUSED check_types */,
                                           QString & /*UNUSED err */)
{
    return true;
}

bool procurveInterfaces::validateInterface(FWObject * /*UNUSED target */,
                                           const QString & /*UNUSED interface_name */,
                                           QString & /*UNUSED err */)
{
    return true;
}

