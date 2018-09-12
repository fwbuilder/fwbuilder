/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

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



#include "global.h"

#include "ObjectTreeView.h"
#include "listOfLibrariesModel.h"

#include "fwbuilder/FWObject.h"

#include <QModelIndex>
#include <QtDebug>

using namespace libfwbuilder;


ListOfLibrariesModel::ListOfLibrariesModel(QObject *parent) : QStringListModel(parent)
{
    top_static_items.push_back(tr("Object Libraries:"));
//    top_static_items.push_back(tr("----------------"));
}

void ListOfLibrariesModel::addStaticItems()
{
    int row = rowCount();
    foreach(QString itm, top_static_items)
    {
        insertRows(row, 1);
        QModelIndex idx = index(row, 0);
        QStringListModel::setData(idx, itm, Qt::DisplayRole);
        row++;
    }

//    setData(idx, QString::fromLatin1("separator"), Qt::AccessibleDescriptionRole);
}

Qt::ItemFlags ListOfLibrariesModel::flags(const QModelIndex &index) const
{
    int row = index.row();
    if (row < 0 || row >= items.size()) return nullptr;
    
    FWObject *lib = items.at(index.row()).lib;
    if (lib == nullptr) return Qt::ItemIsEnabled;
    else return QStringListModel::flags(index);
}

bool ListOfLibrariesModel::insertRows(int row, int count, const QModelIndex & parent)
{
    for (int c=0; c < count; ++c)
        items.insert(row, _item_data("", nullptr, nullptr));
    return QStringListModel::insertRows(row, count, parent);
}

bool ListOfLibrariesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    int c = count;
    while (c > 0)
    {
        items.removeAt(row);
        c--;
    }

    return QStringListModel::removeRows(row, count, parent);
}

static bool ascendingLessThan(const _item_data &s1, const _item_data &s2)
{
    return s1.name < s2.name;
}

static bool decendingLessThan(const _item_data &s1, const _item_data &s2)
{
    return s1.name > s2.name;
}

void ListOfLibrariesModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)

    QList<_item_data> list;
    for (int i=0; i<rowCount(); ++i)
    {
        FWObject *l = getLibrary(i);
        if (l==nullptr) continue;
        list.append(items.at(i));
    }

    if (order == Qt::AscendingOrder)
        qSort(list.begin(), list.end(), ascendingLessThan);
    else
        qSort(list.begin(), list.end(), decendingLessThan);

    int pos = 0;
    foreach(QString itm, top_static_items)
    {
        list.insert(pos, _item_data(itm, nullptr, nullptr));
        pos++;
    }

    items.clear();
    QStringListModel::removeRows(0, rowCount());

    for (int i = 0; i < list.count(); ++i)
    {
        QString text = list.at(i).name;
        insertRows(i, 1);
        QModelIndex idx = index(i, 0);
        QStringListModel::setData(idx, indentLibName(text, list.at(i).lib!=nullptr), Qt::DisplayRole);
        items[i] = list[i];
    }
}

QModelIndex ListOfLibrariesModel::getIdxForLib(FWObject* lib)
{
    for (int i=0; i<rowCount(); ++i)
    {
        QModelIndex idx = index(i, 0);
        if (idx.isValid())
        {
            FWObject *l = getLibrary(idx);
            if (l && l->getId() == lib->getId())
            {
                return idx;
            }
        }
    }

    return QModelIndex();
}

FWObject* ListOfLibrariesModel::getLibrary(QModelIndex idx)
{
    return getLibrary(idx.row());
}

FWObject* ListOfLibrariesModel::getLibrary(int row)
{
    if (row < 0 || row >= items.size()) return nullptr;
    return items[row].lib;
}

ObjectTreeView* ListOfLibrariesModel::getTreeWidget(QModelIndex idx)
{
    return getTreeWidget(idx.row());
}

ObjectTreeView* ListOfLibrariesModel::getTreeWidget(int row)
{
    if (row < 0 || row >= items.size()) return nullptr;
    return items[row].tree;
}

void ListOfLibrariesModel::setData(QModelIndex idx, const QString &name, FWObject *lib, ObjectTreeView *otv)
{
    int row = idx.row();
    if (row < 0 || row >= items.size()) return ;
    items[row] = _item_data(name, lib, otv);
    QStringListModel::setData(idx, indentLibName(name, lib!=nullptr), Qt::DisplayRole);
}

void ListOfLibrariesModel::setName(QModelIndex idx, const QString &name)
{
    int row = idx.row();
    if (row < 0 || row >= items.size()) return ;
    items[row].name = name;
    QStringListModel::setData(idx, indentLibName(name, items.at(row).lib!=nullptr), Qt::DisplayRole);
}

QString ListOfLibrariesModel::indentLibName(const QString &name, bool indent)
{
    if (indent) return "  " + name;
    else return name;
}


