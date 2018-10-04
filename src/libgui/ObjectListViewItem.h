/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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



#ifndef  OBJECTLISTVIEWITEM_H
#define  OBJECTLISTVIEWITEM_H

#include "global.h"

#include <qtreewidget.h>
#include "FWWindow.h"
#include "fwbuilder/FWObjectDatabase.h"

#include <string>
#include <map>

namespace libfwbuilder {
    class FWObject;
}

class ObjectTreeView;

class ObjectListViewItem : public QTreeWidgetItem {

    libfwbuilder::FWObjectDatabase *db;
    QMap<QString, QString>  props;
    QString lib;
    int ID;  
    
 public:

    ObjectListViewItem(QTreeWidget *parent) : QTreeWidgetItem(parent)
    {
        db = nullptr;
        ID = -1;
    }

    ObjectListViewItem(QTreeWidgetItem *parent) : QTreeWidgetItem(parent)
    {
        db = nullptr;
        ID = -1;
    }

    libfwbuilder::FWObject *getFWObject() const
    {
        if (ID > -1) return db->getById(ID, true);
        else return nullptr;
    }
    
    int getFWObjectID() {return ID; }

    void setFWObject(libfwbuilder::FWObject *obj)
    {
        db = obj->getRoot();
        ID = obj->getId();
    }

    ObjectTreeView* getTree();

    QString getLib() { return lib; }
    void setLib(const QString &l) { lib=l; }

    QString getProperty(const QString &name) { return props[name]; }
    void setProperty(const QString &name,const QString &val) {
        props[name]=val;
    }
    virtual bool operator< ( const QTreeWidgetItem & other ) const;
};

#endif
