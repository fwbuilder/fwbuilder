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


#ifndef __GROUPOBJECTDIALOG_H_
#define __GROUPOBJECTDIALOG_H_

#include "../../config.h"
#include <ui_groupobjectdialog_q.h>

#include <qtooltip.h>
#include <vector>

#include "fwbuilder/FWObject.h"
#include "ObjectListView.h"
#include "ObjectIconView.h"
#include <QWidget>

class ObjectIconViewItem;
class ObjectListViewItem;
class ProjectPanel;

class GroupObjectDialog : public QWidget
{
    Q_OBJECT

    libfwbuilder::FWObject *obj;

    Ui::GroupObjectDialog_q*m_dialog;
    ObjectIconView         *iconView;
    ObjectListView         *listView;

    std::vector<int> selectedObjects;

    libfwbuilder::FWObject *selectedObject;

    bool init;

    ProjectPanel *m_project;
    void addIcon(libfwbuilder::FWObject *o);
    void addIcon(libfwbuilder::FWObject *o, bool ref);

    void insertObject(libfwbuilder::FWObject *o);

    void setupPopupMenu(const QPoint&);
    void saveColumnWidths();

 public:
    GroupObjectDialog(ProjectPanel *project, QWidget *parent);
    ~GroupObjectDialog();

    enum viewType { Icon, List };

 public slots:
    virtual void changed();
    virtual void libChanged();
    virtual void applyChanges();
    virtual void discardChanges();
    virtual void loadFWObject(libfwbuilder::FWObject *obj);
    virtual void validate(bool*);
    virtual void isChanged(bool*);
    virtual void switchToIconView();
    virtual void switchToListView();
    virtual void openObject();
    virtual void openObject(QListWidgetItem *itm);
    virtual void openObject(QTreeWidgetItem *itm);
    virtual void dropped(QDropEvent *ev);
    virtual void iconContextMenu(const QPoint & pos);
    virtual void listContextMenu(const QPoint & pos);
    virtual void closeEvent(QCloseEvent *e);
    virtual void hideEvent(QHideEvent *e);
    virtual void getHelpName(QString*);
    
    void copyObj();
    void cutObj();
    void pasteObj();
    void deleteObj();

    void iconViewCurrentChanged(QListWidgetItem *itm);
    void listViewCurrentChanged(QTreeWidgetItem *itm);
    
    void iconViewSelectionChanged();
    void listViewSelectionChanged();
    
    void selectObject(libfwbuilder::FWObject *o);

 signals:
/**
 * This signal is emitted from closeEvent, ObjectEditor connects
 * to this signal to make checks before the object editor can be closed
 * and to store its position on the screen
 */
    void close_sign(QCloseEvent *e);
    void changed_sign();
    
 private:

    static enum viewType vt;

 public:

    
};

#endif // GROUPOBJECTDIALOG_H
