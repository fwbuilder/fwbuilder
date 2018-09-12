/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Theron Tock

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


#ifndef __DynamicGroupDialog_h__
#define __DynamicGroupDialog_h__

#include <ui_dynamicgroupdialog_q.h>

#include <qtooltip.h>
#include <vector>

#include "fwbuilder/FWObject.h"
#include "ObjectListView.h"
#include "ObjectIconView.h"
#include "BaseObjectDialog.h"
#include <QComboBox>
#include <QItemDelegate>
#include <QStandardItemModel>

class ObjectListViewItem;
class ProjectPanel;

class DynamicGroupDialog : public BaseObjectDialog
{
    Q_OBJECT;

    Ui::DynamicGroupDialog_q m_ui;
    QStandardItemModel *m_model;
    bool m_reloadObjFilter;
    std::list<std::string> m_loadedObjFilter;
    std::set<std::string> m_loadedAllKeywords;

    void loadObjFilter();

 public:
    DynamicGroupDialog(QWidget *parent);
    libfwbuilder::FWObject *getCurrentObj() { return obj; }

 public slots:
    virtual void applyChanges();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool *);

    void addMatchClicked();
    void deleteFilterClicked();

    void gotItemDoubleClicked(QTreeWidgetItem *item, int);
};

class DynamicItemDelegate : public QItemDelegate {
    Q_OBJECT;

    DynamicGroupDialog *m_dialog;

public:
    DynamicItemDelegate(DynamicGroupDialog *dialog, QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

 public slots:
    void comboActivated(int);
};


#endif  /* __DynamicGroupDialog_h__ */
