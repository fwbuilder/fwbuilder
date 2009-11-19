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


#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "events.h"

#include "ProjectPanel.h"
#include "FWBTree.h"
#include "FWBSettings.h"
#include "FWObjectPropertiesFactory.h"
#include "GroupObjectDialog.h"
#include "FWObjectDrag.h"
#include "FWObjectClipboard.h"
#include "ObjectTreeView.h"
#include "FWCmdChange.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/ServiceGroup.h"

#include <qlineedit.h>
#include <qtextedit.h>
#include <qlistwidget.h>
#include <qtreewidget.h>
#include <qtoolbutton.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qstackedwidget.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmenu.h>
#include <qtooltip.h>
#include <qscrollarea.h>
#include <qpixmapcache.h>
#include <QCoreApplication>
#include <QUndoStack>

#include <iostream>
#include <algorithm>
#include "FWBSettings.h"

using namespace std;
using namespace libfwbuilder;

enum GroupObjectDialog::viewType GroupObjectDialog::vt = GroupObjectDialog::Icon;

#define LIST_VIEW_MODE "list"
#define ICON_VIEW_MODE "icon"


GroupObjectDialog::GroupObjectDialog(QWidget *parent) :
        BaseObjectDialog(parent)
{
    m_dialog = new Ui::GroupObjectDialog_q;
    m_dialog->setupUi(this);

    obj=NULL;
    selectedObject=NULL;

    listView = new ObjectListView( m_dialog->objectViewsStack, "listView" );
    QStringList sl;
    sl << "Name" << "Properties";
    listView->setHeaderLabels (sl);
    listView->setAcceptDrops( true );
    listView->setDragDropMode( QAbstractItemView::DragDrop );
    listView->setContextMenuPolicy ( Qt::CustomContextMenu );

    iconView = new ObjectIconView( m_dialog->objectViewsStack, "iconView" );
    iconView->setContextMenuPolicy ( Qt::CustomContextMenu );

    m_dialog->objectViewsStack->addWidget(iconView);
    m_dialog->objectViewsStack->addWidget(listView);
    m_dialog->objectViewsStack->setCurrentWidget(iconView);


    setTabOrder( m_dialog->obj_name, iconView );
    setTabOrder( iconView, listView );
    setTabOrder( listView, m_dialog->comment );

    listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    iconView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_dialog->iconViewBtn->setCheckable(true);
    m_dialog->listViewBtn->setCheckable(true);

    //listView->hide();
    //iconView->show();


    m_dialog->iconViewBtn->setAutoRaise(false);
    m_dialog->listViewBtn->setAutoRaise(false);

    connect( iconView, SIGNAL( currentItemChanged(QListWidgetItem*,QListWidgetItem*) ),
             this,     SLOT( iconViewCurrentChanged(QListWidgetItem*) ) );

    connect( iconView, SIGNAL (itemSelectionChanged()),
            this,      SLOT (iconViewSelectionChanged()));

    connect( iconView, SIGNAL( dropped(QDropEvent*) ),
             this,     SLOT( dropped(QDropEvent*) ) );

    connect( iconView, SIGNAL( customContextMenuRequested(const QPoint&) ),
             this,     SLOT( iconContextMenu(const QPoint&) ) );

    connect( iconView, SIGNAL( delObject_sign() ),
             this,     SLOT( deleteObj() ) );


    connect( listView, SIGNAL( currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*) ),
             this,     SLOT( listViewCurrentChanged(QTreeWidgetItem*) ) );
    connect( listView, SIGNAL (itemSelectionChanged()),
            this,      SLOT (listViewSelectionChanged()));

    connect( listView, SIGNAL( dropped(QDropEvent*) ),
             this,     SLOT( dropped(QDropEvent*) ) );
    connect( listView, SIGNAL( customContextMenuRequested(const QPoint&) ),
             this,     SLOT( listContextMenu(const QPoint&) ) );
    connect( listView, SIGNAL( delObject_sign() ),
             this,     SLOT( deleteObj() ) );

    QString s    = st->getGroupViewColumns();
    int     col0 = s.section(',',0,0).toInt();
    int     col1 = s.section(',',1,1).toInt();

    if (col0 == 0)
        col0 = listView->width()/2;

    listView->setColumnWidth(0,col0);
    listView->setColumnWidth(1,col1);

    QString mode=st->getGroupViewMode();
    if (mode==ICON_VIEW_MODE) switchToIconView();
    if (mode==LIST_VIEW_MODE) switchToListView();
}

GroupObjectDialog::~GroupObjectDialog()
{
    delete m_dialog;
}

void GroupObjectDialog::iconViewSelectionChanged()
{
    if (fwbdebug) qDebug("GroupObjectDialog::iconViewSelectionChanged()");

    selectedObjects.clear();

    for (int it=0; it<iconView->count(); ++it)
    {
        QListWidgetItem *itm = iconView->item(it);
        if (itm->isSelected())
        {
            int obj_id = itm->data(Qt::UserRole).toInt();
            if (fwbdebug) qDebug("obj_id=%d", obj_id);
            selectedObjects.push_back(obj_id);
        }
    }
}

void GroupObjectDialog::listViewSelectionChanged()
{
    if (fwbdebug) qDebug("GroupObjectDialog::listViewSelectionChanged()");

    selectedObjects.clear();

    for (int it=0; it<listView->topLevelItemCount(); ++it)
    {
        QTreeWidgetItem *itm = listView->topLevelItem(it);
        if (itm->isSelected())
        {
            int obj_id = itm->data(0, Qt::UserRole).toInt();
            if (fwbdebug) qDebug("obj_id=%d", obj_id);
            selectedObjects.push_back(obj_id);
        }
    }
}

void GroupObjectDialog::iconViewCurrentChanged(QListWidgetItem *itm)
{
    if (itm==NULL)
    {
        selectedObject=NULL;
        return;
    }
    int obj_id = itm->data(Qt::UserRole).toInt();
    FWObject *o = m_project->db()->findInIndex(obj_id);
    selectedObject = o;
}


void GroupObjectDialog::listViewCurrentChanged(QTreeWidgetItem *itm)
{
    if (itm==NULL)
    {
        selectedObject=NULL;
        return;
    }
    int obj_id = itm->data(0, Qt::UserRole).toInt();
    FWObject *o = m_project->db()->findInIndex(obj_id);
    selectedObject = o;
}

/*
 * used to add an object for paste and drop operations
 */
void GroupObjectDialog::insertObject(FWObject *o)
{
    assert(o!=NULL);
    Group *g = dynamic_cast<Group*>(obj);
    assert(g!=NULL);

    if ( ! g->validateChild(o) || g->isReadOnly() ) return;

    if (fwbdebug)
        qDebug("Adding object %s to the group %s",
               o->getName().c_str(), g->getName().c_str());

/* avoid duplicates */
    int cp_id = o->getId();

    for (int it=0; it<listView->topLevelItemCount(); ++it)
    {
        QTreeWidgetItem *itm = listView->topLevelItem(it);
        int obj_id = itm->data(0, Qt::UserRole).toInt();
	if(obj_id==cp_id) return;
    }

    addIcon(o, ! FWBTree().isSystem(obj) );

    changed();
}

void GroupObjectDialog::addIcon(FWObject *fwo)
{
    FWObject *o=fwo;
    bool      ref=false;
    if (FWReference::cast(o)!=NULL)
    {
        o=FWReference::cast(o)->getPointer();
        ref=true;
    }

    addIcon(o,ref);
}

void GroupObjectDialog::addIcon(FWObject *o, bool ref)
{
    if (Resources::global_res->getResourceBool(
            string("/FWBuilderResources/Type/") +
            o->getTypeName() + "/hidden") ) return;

    QString obj_name=QString::fromUtf8(o->getName().c_str());

    QString icn_filename =
            (":/Icons/"+o->getTypeName()+((ref)?"/icon-ref":"/icon")).c_str();

    QPixmap pm;
    if ( ! QPixmapCache::find( icn_filename, pm) )
    {
        pm.load( icn_filename );
        QPixmapCache::insert( icn_filename, pm);
    }

    QListWidgetItem *list_item = new QListWidgetItem(QIcon(pm),
                                                     obj_name, iconView);
    list_item->setData(Qt::UserRole, QVariant(o->getId()));
    iconView->addItem(list_item);

    QTreeWidgetItem *tree_item = new QTreeWidgetItem(listView);
    tree_item->setText(0, obj_name);
    tree_item->setText(1, FWObjectPropertiesFactory::getObjectProperties(o) );
    tree_item->setIcon(0, QIcon(pm) );
    tree_item->setData(0, Qt::UserRole, QVariant(o->getId()));
    listView->addTopLevelItem(tree_item);
}

void GroupObjectDialog::getHelpName(QString *str)
{
    *str = "GroupObjectDialog";
}

void GroupObjectDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Group *g = Group::cast(obj);
    assert(g!=NULL);

    // if (ServiceGroup::cast(obj)!=NULL)
    // {
    //     m_dialog->icon->setPixmap(QIcon(":/Icons/ServiceGroup/icon").pixmap(
    //                                   25,25));
    // }
    // else
    // {
    //     m_dialog->icon->setPixmap(QIcon(":/Icons/ObjectGroup/icon").pixmap(
    //                                   25,25));
    // }
    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(g->getName().c_str()) );
    m_dialog->comment->setText( QString::fromUtf8(g->getComment().c_str()) );

    m_dialog->obj_name->setEnabled( !FWBTree().isSystem(obj) );
    m_dialog->comment->setEnabled(  !FWBTree().isSystem(obj) );

    listView->clear();
    iconView->clear();

    listView->setDB(o->getRoot());
    iconView->setDB(o->getRoot());

    iconView->setResizeMode( QListWidget::Adjust );
    iconView->setGridSize ( QSize(50, 40) );

    switch (vt)
    {
    case Icon:
        if ( ! m_dialog->iconViewBtn->isChecked() ) m_dialog->iconViewBtn->toggle();
        iconView->raise();
        break;

    case List:
        if ( ! m_dialog->listViewBtn->isChecked() ) m_dialog->listViewBtn->toggle();
        listView->raise();
        break;
    }

    for (FWObject::iterator i=g->begin(); i!=g->end(); i++)
        addIcon( *i );


    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly() && !FWBTree().isSystem(o));
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->comment->setEnabled(!o->isReadOnly() && !FWBTree().isSystem(o));
    setDisabledPalette(m_dialog->comment);

//    listView->setEnabled(!o->isReadOnly());
    setDisabledPalette(listView);

//    iconView->setEnabled(!o->isReadOnly());
    setDisabledPalette(iconView);


    init=false;
}

void GroupObjectDialog::validate(bool *res)
{
    *res=true;
    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this, obj, m_dialog->obj_name->text()))
    {
        *res=false;
        return;
    }
}



void GroupObjectDialog::applyChanges()
{
    if (fwbdebug) qDebug("GroupObjectDialog::applyChanges");

    FWCmdChange* cmd = new FWCmdChange(m_project, obj);
    FWObject* new_state = cmd->getNewState();

    string oldname = obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    new_state->setComment(
        string(m_dialog->comment->toPlainText().toUtf8().constData()) );

    //init=true;

    set<int> oldobj;
    set<int> newobj;

    for (int it=0; it<listView->topLevelItemCount(); ++it)
    {
        QTreeWidgetItem *itm = listView->topLevelItem(it);
        int obj_id = itm->data(0, Qt::UserRole).toInt();
        newobj.insert(obj_id);
    }

    for (FWObject::iterator j=new_state->begin(); j!=new_state->end(); ++j)
    {
        FWObject *o = *j;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        oldobj.insert(o->getId());
    }

    set<int> diff;

    set_difference( oldobj.begin(), oldobj.end(),
                    newobj.begin(), newobj.end(),
                    inserter(diff,diff.begin()));
/* diff contains objects present in oldobj but not in newobj - these objects
   were deleted from the group */

    for (set<int>::iterator k=diff.begin(); k!=diff.end(); ++k)
    {
        FWObject *o = m_project->db()->findInIndex(*k);
        if (FWBTree().isSystem(new_state))
            m_project->delObj(o, false);
        else
            new_state->removeRef(o);
    }

    diff.clear();

    set_difference( newobj.begin(), newobj.end(),
                    oldobj.begin(), oldobj.end(),
                    inserter(diff,diff.begin()));
/* diff contains objects present in newobj but not in oldobj - these objects
   were added to the group */

    for (set<int>::iterator k1=diff.begin(); k1!=diff.end(); ++k1)
    {
        FWObject *o = m_project->db()->findInIndex(*k1);
        if (FWBTree().isSystem(o))
            m_project->pasteTo(new_state, o);
        else
            new_state->addRef(o);
    }

    saveColumnWidths();

    m_project->undoStack->push(cmd);
    
    BaseObjectDialog::applyChanges();
}

void GroupObjectDialog::discardChanges()
{
    loadFWObject(obj);
}

void GroupObjectDialog::switchToIconView()
{
    if (vt == Icon) return;
    vt = Icon;

    if ( ! m_dialog->iconViewBtn->isChecked() ) m_dialog->iconViewBtn->toggle();

    m_dialog->objectViewsStack->setCurrentWidget(iconView);

    st->setGroupViewMode(ICON_VIEW_MODE);
}

void GroupObjectDialog::switchToListView()
{
    if (vt == List) return;
    vt = List;

    if ( ! m_dialog->listViewBtn->isChecked() ) m_dialog->listViewBtn->toggle();

    m_dialog->objectViewsStack->setCurrentWidget(listView);

    st->setGroupViewMode(LIST_VIEW_MODE);
}

// This method is attached to the context menu item "Edit"
void GroupObjectDialog::openObject()
{
    if (selectedObject!=NULL)
    {
        QCoreApplication::postEvent(
            m_project, new showObjectInTreeEvent(selectedObject->getRoot()->getFileName().c_str(),
                                                 selectedObject->getId()));
        QCoreApplication::postEvent(
            m_project, new openObjectInEditorEvent(selectedObject->getRoot()->getFileName().c_str(),
                                                   selectedObject->getId()));
    }
}

void GroupObjectDialog::dropped(QDropEvent *ev)
{
    if (fwbdebug) qDebug("GroupObjectDialog::dropped");

    list<FWObject*> ol;
    if (FWObjectDrag::decode(ev, ol))
    {
        if (ol.size()==0) return;
        for (list<FWObject*>::iterator i=ol.begin(); i!=ol.end(); ++i)
            insertObject( *i );
        if (fwbdebug) qDebug("GroupObjectDialog::dropped  ev->acceptAction()");
        ev->setAccepted(true);

        // see comment in ObjectTreeView.cpp explaining the purpose of
        // flag process_mouse_release_event
        ObjectTreeView *otv = m_project->getCurrentObjectTree();
        otv->ignoreNextMouseReleaseEvent();

    }
    if (fwbdebug) qDebug("GroupObjectDialog::dropped  done");
}

void GroupObjectDialog::iconContextMenu(const QPoint & pos)
{
    FWObject *o = NULL;
    QListWidgetItem *itm = iconView->itemAt(pos);
    if (itm)
    {
        int obj_id = itm->data(Qt::UserRole).toInt();
        o = m_project->db()->findInIndex(obj_id);
        selectedObject = o;
    }
    setupPopupMenu(iconView->mapToGlobal(pos));
}


void GroupObjectDialog::listContextMenu(const QPoint & pos)
{
    FWObject *o=NULL;
    QTreeWidgetItem *itm = listView->itemAt(pos);
    if (itm)
    {
        int obj_id = itm->data(0, Qt::UserRole).toInt();
        o = m_project->db()->findInIndex(obj_id);
        selectedObject = o;
    }
    setupPopupMenu(listView->viewport()->mapToGlobal(pos));
}

void GroupObjectDialog::setupPopupMenu(const QPoint &pos)
{
    QMenu *popup=new QMenu(this);

    if (selectedObject!=NULL)
    {
        if (selectedObject->isReadOnly() )
            popup->addAction(tr("Open"), this, SLOT(openObject()));
        else
            popup->addAction(tr("Edit"), this, SLOT(openObject()));
    }

    QAction *copyID =popup->addAction(tr("Copy"), this, SLOT(copyObj()));
    QAction *cutID =popup->addAction(tr("Cut"), this, SLOT(cutObj()));
    QAction *pasteID=popup->addAction(tr("Paste"), this, SLOT(pasteObj()));
    QAction *delID =popup->addAction(tr("Delete"),this, SLOT(deleteObj()));

    copyID->setEnabled(selectedObject!=NULL &&
                          ! FWBTree().isSystem(selectedObject) );
    cutID->setEnabled(selectedObject!=NULL &&
                          ! FWBTree().isSystem(obj) &&
                          ! obj->isReadOnly() );
    pasteID->setEnabled(! FWBTree().isSystem(obj) &&
                          ! obj->isReadOnly() );
    delID->setEnabled(selectedObject!=NULL &&
                          ! FWBTree().isSystem(obj) &&
                          ! obj->isReadOnly() );

    popup->exec( pos );
}

void GroupObjectDialog::copyObj()
{
    FWObjectClipboard::obj_clipboard->clear();
    for(vector<int>::iterator it=selectedObjects.begin(); 
        it!=selectedObjects.end(); ++it)
    {
        FWObject* selectedObject = m_project->db()->findInIndex(*it);

        if (selectedObject!=NULL && ! FWBTree().isSystem(selectedObject) )
        {
            FWObjectClipboard::obj_clipboard->add(selectedObject,
                                                  this->m_project );
        }

    }
}

void GroupObjectDialog::cutObj()
{
    copyObj();
    deleteObj();
}

void GroupObjectDialog::pasteObj()
{
    vector<std::pair<int,ProjectPanel*> >::iterator i;

    for (i= FWObjectClipboard::obj_clipboard->begin();
         i!=FWObjectClipboard::obj_clipboard->end(); ++i)
    {
        insertObject( m_project->db()->findInIndex(i->first) );
    }
}

void GroupObjectDialog::deleteObj()
{
    // make a copy of the list of selected objects because selection
    // changes when we delete items 
    vector<int> tv;
    copy(selectedObjects.begin(),selectedObjects.end(),inserter(tv,tv.begin()));

    for(vector<int>::iterator it=tv.begin(); it!=tv.end(); ++it)
    {
        if (fwbdebug)
            qDebug("GroupObjectDialog::deleteObj()  (*it)=%d", (*it));

        FWObject* selectedObject = m_project->db()->findInIndex(*it);
        int o_id = selectedObject->getId();

        for (int it=0; it<listView->topLevelItemCount(); ++it)
        {
            QTreeWidgetItem *itm = listView->topLevelItem(it);
            if (o_id == itm->data(0, Qt::UserRole).toInt())
            {
                listView->takeTopLevelItem(it);
                break;
            }
        }

        for (int it=0; it<iconView->count(); ++it)
        {
            QListWidgetItem *itm = iconView->item(it);
            if (o_id == itm->data(Qt::UserRole).toInt())
            {
                iconView->takeItem(it);
                break;
            }
        }
    }
    changed();
}

void GroupObjectDialog::saveColumnWidths()
{
    if (fwbdebug)
        qDebug("GroupObjectDialog::saveColumnWidths()");

    QString s = QString("%1,%2")
        .arg(listView->columnWidth(0))
        .arg(listView->columnWidth(1));

    st->setGroupViewColumns(s);
}

void GroupObjectDialog::selectObject(FWObject *o)
{
    int o_id = o->getId();

    for (int it=0; it<listView->topLevelItemCount(); ++it)
    {
        QTreeWidgetItem *itm = listView->topLevelItem(it);
        if (o_id == itm->data(0, Qt::UserRole).toInt())
        {
            listView->setCurrentItem(itm);
            break;
        }
    }

    for (int it=0; it<iconView->count(); ++it)
    {
        QListWidgetItem *itm = iconView->item(it);
        if (o_id == itm->data(Qt::UserRole).toInt())
        {
            iconView->setCurrentItem(itm);
            break;
        }
    }
}
