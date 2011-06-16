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



#ifndef  OBJECTTREEVIEWITEM_H
#define  OBJECTTREEVIEWITEM_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <string>
#include <map>

namespace libfwbuilder {
    class FWObject;
}

class ObjectTreeView;

class ObjectTreeViewItem : public QTreeWidgetItem {

    libfwbuilder::FWObject *objptr;
    libfwbuilder::FWObject *userFolderParent;
    QString                 userFolderName;
    QMap<QString, QString>  props;
    QString                 lib;
    
 public:

    ObjectTreeViewItem(QTreeWidget *parent) :
        QTreeWidgetItem(parent), objptr(0), userFolderParent(0) {}

    ObjectTreeViewItem(QTreeWidgetItem *parent) :
        QTreeWidgetItem(parent), objptr(0), userFolderParent(0) {}

    libfwbuilder::FWObject *getFWObject() const { return objptr; }
    libfwbuilder::FWObject *getUserFolderParent() { return userFolderParent; }
    void setFWObject(libfwbuilder::FWObject *obj) { objptr=obj; }
    void setUserFolderParent(libfwbuilder::FWObject *obj) {
        userFolderParent = obj;
    }

    void setUserFolderName(const QString &name) { userFolderName = name; }
    const QString &getUserFolderName() { return userFolderName; }

    ObjectTreeView* getTree();

    QString getLib() { return lib; }
    void    setLib(const QString &l) { lib=l; }

    QString getProperty(const QString &name) const { return props[name]; }
    void    setProperty(const QString &name, const QString &val) {
        props[name]=val;
    }
    
    QVariant data (int column, int role) const;
    virtual bool operator< ( const QTreeWidgetItem & other ) const;
};

#endif
