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
        QFont usual = QTreeWidgetItem::data(column, role).value<QFont>();

        FWObject *obj = getFWObject();
        Firewall *o = NULL;

        if (obj!=NULL && getProperty("type")==Firewall::TYPENAME)
        {
            o = Firewall::cast( obj );
        }

        if (o!=NULL)
        {
            bool mf = !o->getInactive() && (o->needsInstall()) ;
            usual.setBold (mf);
            usual.setStrikeOut(o->getInactive());
            return QVariant(usual);
        }
        else
            return QVariant(usual);
    }
    return QTreeWidgetItem::data(column, role);
}

bool ObjectTreeViewItem::operator<( const QTreeWidgetItem & other ) const
{
    int rank1 = -1;
    int rank2 = -1;
    const ObjectTreeViewItem * otvi =
        dynamic_cast<const ObjectTreeViewItem*>(& other);

    if (otvi->objptr==NULL) return true ;
    if (objptr==NULL) return true ;

    if (Interface::cast(otvi->objptr)!=NULL)
    {
        rank1=3;
    }
    if (Policy::cast(otvi->objptr)!=NULL)
    {
        rank1=0;
    }
    if (NAT::cast(otvi->objptr)!=NULL)
    {
        rank1=1;
    }
    if (Routing::cast(otvi->objptr)!=NULL)
    {
        rank1=2;
    }
    if (Interface::cast(objptr)!=NULL)
    {
        rank2=3;
    }
    if (Policy::cast(objptr)!=NULL)
    {
        rank2=0;
    }
    if (NAT::cast(objptr)!=NULL)
    {
        rank2=1;
    }
    if (Routing::cast(objptr)!=NULL)
    {
        rank2=2;
    }

    if (rank1==rank2)
    {
        QString s1 = objptr->getName().c_str();
        QString s2 = otvi->objptr->getName ().c_str();
        //return ( s1 < s2);
        return ( s1.toLower() < s2.toLower());
    }
    if (rank1>rank2)
    {
        return true ;
    }
    return false ;
}
