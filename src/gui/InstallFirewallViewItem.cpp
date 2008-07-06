/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  alek@codeminders.com

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

#include "global.h"

#include "InstallFirewallViewItem.h"

#include "fwbuilder/Firewall.h"

#include <qfont.h>

using namespace std;
using namespace libfwbuilder;

InstallFirewallViewItem::InstallFirewallViewItem(QTreeWidget* parent,
    const QString & st, bool slt ):
    QTreeWidgetItem(parent,QStringList(st))
{
    showLastTimes=slt;
}

QVariant InstallFirewallViewItem::data (int column, int role) const
{
    /*int statCol=(showLastTimes)?4:1;

    if (role == Qt::FontRole)
    {
        QFont usual = QTreeWidgetItem::data(column, role).value<QFont>();

        if (text(statCol).contains("..."))
        {
            usual.setBold (true);
            return QVariant(usual);
        } else if (text(statCol)==QObject::tr("Failure"))
            return QVariant(usual);

        if (column==statCol)
        {
            usual.setBold (true);
            return QVariant(usual);
        }
    }

    if (role == Qt::ForegroundRole)
    {
        QBrush usual = QTreeWidgetItem::data(column, role).value<QBrush>();

        if (text(statCol)==QObject::tr("Failure"))
        {
            usual.setColor(Qt::red);
            return QVariant(usual);
        }

        if ((column==statCol) && (text(statCol)==QObject::tr("Success")))
        {
            usual.setColor(Qt::green);
            return QVariant(usual);
        }
    }*/

    return QTreeWidgetItem::data(column, role);
}




