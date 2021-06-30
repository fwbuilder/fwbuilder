/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

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


#include "global.h"

#include "ObjectTreeViewItem.h"
#include "ObjectTreeView.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"

#include <qfont.h>
#include <qpainter.h>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;


ObjectTreeView* ObjectTreeViewItem::getTree()
{
    return dynamic_cast<ObjectTreeView*>(treeWidget());
}


QVariant ObjectTreeViewItem::data(int column, int role) const
{
    if (column == 0 && role == Qt::FontRole)
    {
        QFont item_font = QTreeWidgetItem::data(column, role).value<QFont>();

        FWObject *obj = getFWObject();
        Firewall *o = nullptr;

        if (obj!=nullptr && (
                getProperty("type")==Firewall::TYPENAME ||
                getProperty("type")==Cluster::TYPENAME))
        {
            o = Firewall::cast( obj );
        }

        if (o!=nullptr)
        {
            bool mf = !o->getInactive() && (o->needsCompile()) ;
            item_font.setBold (mf);
            item_font.setStrikeOut(o->getInactive());
            return QVariant(item_font);
        }
        else
            return QVariant(item_font);
    }
    return QTreeWidgetItem::data(column, role);
}


static int getRank(FWObject *obj)
{
    /* User-defined folders are first */
    if (obj == nullptr) return 0;
    
    if (Interface::cast(obj) != nullptr) return 5;
    if (Policy::cast(obj) != nullptr) return 2;
    if (NAT::cast(obj) != nullptr) return 3;
    if (Routing::cast(obj) != nullptr) return 4;

    return 1;
}


bool ObjectTreeViewItem::operator<(const QTreeWidgetItem &other) const
{
    const ObjectTreeViewItem *otvi =
        dynamic_cast<const ObjectTreeViewItem*>(&other);

    int rank1 = getRank(otvi->objptr);
    int rank2 = getRank(objptr);

    if (rank1 == rank2)
        return text(0).toLower() < otvi->text(0).toLower();

    return rank1 > rank2;
}
