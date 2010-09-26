/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

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


#ifndef  __LISTOFLIBRARIESMODEL_H_
#define  __LISTOFLIBRARIESMODEL_H_

#include <QStringListModel>
#include <QMap>

class ObjectTreeView;
namespace libfwbuilder
{
    class FWObject;
};


class _item_data
{
public:
    QString name;
    libfwbuilder::FWObject* lib;
    ObjectTreeView* tree;

    _item_data(const QString &_n, libfwbuilder::FWObject* _lib, ObjectTreeView* _tree)
    {
        name = _n; lib = _lib; tree = _tree;
    }
};

class ListOfLibrariesModel : public QStringListModel
{
    Q_OBJECT;
    QStringList top_static_items;
    QList<_item_data> items;
    
public:

    ListOfLibrariesModel(QObject *parent = 0);

    void addStaticItems();

    QString indentLibName(const QString &name, bool indent);
    
//    virtual QModelIndex index(int, int, const QModelIndex &p = QModelIndex()) const;

    QModelIndex getIdxForLib(libfwbuilder::FWObject*);

    libfwbuilder::FWObject* getLibrary(QModelIndex idx);
    libfwbuilder::FWObject* getLibrary(int row);

    void setData(QModelIndex idx, const QString &name, libfwbuilder::FWObject *lib, ObjectTreeView *otv);
    void setName(QModelIndex idx, const QString &name);
    
    ObjectTreeView* getTreeWidget(QModelIndex idx);
    ObjectTreeView* getTreeWidget(int row);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    
    virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    
};

#endif
