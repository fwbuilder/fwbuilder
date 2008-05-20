/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: GroupObjectDialog.cpp,v 1.58 2007/04/14 00:18:43 vkurland Exp $

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


#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"
#include "utils.h"
#include "ProjectPanel.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "FWWindow.h"
#include "FWBTree.h"
#include "FWBSettings.h"
#include "FWObjectPropertiesFactory.h"
#include "GroupObjectDialog.h"
#include "ObjectListViewItem.h"
#include "ObjectIconViewItem.h"
#include "FWObjectDrag.h"
#include "FWObjectClipboard.h"
#include "ObjectTreeView.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/Resources.h"

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

#include <iostream>
#include <algorithm>
#include "FWBSettings.h"

using namespace std;
using namespace libfwbuilder;

enum GroupObjectDialog::viewType GroupObjectDialog::vt = GroupObjectDialog::Icon;

#define LIST_VIEW_MODE "list"
#define ICON_VIEW_MODE "icon"


GroupObjectDialog::GroupObjectDialog(ProjectPanel *project, QWidget *parent) :
        QWidget(parent), m_project(project)
{
    m_dialog = new Ui::GroupObjectDialog_q;
    m_dialog->setupUi(this);
    setFont(st->getUiFont());

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

    //apply->setEnabled( false );

    connect( iconView, SIGNAL( itemActivated(QListWidgetItem*) ),
             this,     SLOT( openObject(QListWidgetItem*) ) );

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


    connect( listView, SIGNAL( itemActivated(QTreeWidgetItem*,int) ),
             this,     SLOT( openObject(QTreeWidgetItem*) ) );
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
    selectedObjects.clear();

    for ( int itemn = 0; itemn < iconView->count(); itemn++ )
    {
        QListWidgetItem *item = iconView->item(itemn);
        if(item->isSelected())
        {
            ObjectIconViewItem *oivi=dynamic_cast<ObjectIconViewItem*>(item);
            assert(oivi!=NULL);
            FWObject *o = oivi->getFWObject();
            if (o!=NULL)
                selectedObjects.push_back(o);
        }

    }

}
void GroupObjectDialog::listViewSelectionChanged()
{
    selectedObjects.clear();
    //QTreeWidgetItemIterator it(listView);

    for ( int i = 0; i < listView->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *itm= listView->topLevelItem(i);
        if(itm->isSelected())
        {
            ObjectListViewItem *otvi=dynamic_cast<ObjectListViewItem*>(itm);
            assert(otvi!=NULL);

            FWObject *o=otvi->getFWObject();
            assert(o!=NULL);
            selectedObjects.push_back(o);
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

    ObjectIconViewItem *oivi=dynamic_cast<ObjectIconViewItem*>(itm);
    assert(oivi!=NULL);

    FWObject *o = oivi->getFWObject();
    if (o!=NULL)
        selectedObject=o;

}


void GroupObjectDialog::listViewCurrentChanged(QTreeWidgetItem *itm)
{
    if (itm==NULL)
    {
        selectedObject=NULL;
        return;
    }

    ObjectListViewItem *otvi=dynamic_cast<ObjectListViewItem*>(itm);
    assert(otvi!=NULL);

    FWObject *o = otvi->getFWObject();
    if (o!=NULL)
        selectedObject=o;

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
    string cp_id=o->getId();

    map<string, ObjectListViewItem*>::iterator i;
    for (i=allListViewItems.begin(); i!=allListViewItems.end(); ++i)
    {
        string go=(*i).first;
        //if (FWReference::cast(go)!=NULL) go=FWReference::cast(go)->getPointer();
        //if (o==go || o->getId()==go->getId()) return;
	if(go==cp_id) return;
    }

    addIcon(o, ! m_project->isSystem(obj) );


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

void GroupObjectDialog::addIcon(FWObject *o,bool ref)
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

    ObjectIconViewItem *ivitm = new ObjectIconViewItem(iconView,obj_name,pm );
    ivitm->setProperty("id",   o->getId().c_str()   );
    ivitm->setProperty("type", o->getTypeName().c_str() );
    ivitm->setFWObject( o );

    allIconViewItems[o->getId()]=ivitm;

    icn_filename = Resources::global_res->getObjResourceStr(o, "icon-tree").c_str();

    ObjectListViewItem *tvitm=new ObjectListViewItem( listView );
    tvitm->setText( 0, obj_name );
    tvitm->setText( 1, FWObjectPropertiesFactory::getObjectProperties(o) );
    tvitm->setIcon( 0, QIcon(pm) );

    tvitm->setProperty("id",   o->getId().c_str()   );
    tvitm->setProperty("type", o->getTypeName().c_str() );
    tvitm->setFWObject( o );

    allListViewItems[o->getId()]=tvitm;
}

void GroupObjectDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Group *g = Group::cast(obj);
    assert(g!=NULL);

    init=true;

    allIconViewItems.clear();
    allListViewItems.clear();

    fillLibraries(m_dialog->libs,obj);

    m_dialog->obj_name->setText( QString::fromUtf8(g->getName().c_str()) );
    m_dialog->comment->setText( QString::fromUtf8(g->getComment().c_str()) );

    m_dialog->obj_name->setEnabled( !m_project->isSystem(obj) );
    m_dialog->libs->setEnabled(     !m_project->isSystem(obj) );
    m_dialog->comment->setEnabled(  !m_project->isSystem(obj) );

    listView->clear();

    iconView->clear();

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

    m_dialog->obj_name->setEnabled(!o->isReadOnly() && !m_project->isSystem(o));
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->comment->setEnabled(!o->isReadOnly() && !m_project->isSystem(o));
    setDisabledPalette(m_dialog->comment);

    m_dialog->libs->setEnabled(!o->isReadOnly() && !m_project->isSystem(o));
    setDisabledPalette(m_dialog->libs);

//    listView->setEnabled(!o->isReadOnly());
    setDisabledPalette(listView);

//    iconView->setEnabled(!o->isReadOnly());
    setDisabledPalette(iconView);


    init=false;
}

void GroupObjectDialog::changed()
{
    //if (!init) apply->setEnabled( true );

    emit changed_sign();
}

void GroupObjectDialog::validate(bool *res)
{
    *res=true;
    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}

void GroupObjectDialog::isChanged(bool*)
{
    //*res=(!init && apply->isEnabled());
}

void GroupObjectDialog::libChanged()
{
    changed();
}

void GroupObjectDialog::applyChanges()
{
    if (fwbdebug)
        qDebug("GroupObjectDialog::applyChanges");

    string oldname=obj->getName();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );

    init=true;

    set<FWObject*> oldobj;
    set<FWObject*> newobj;
    map<string, ObjectListViewItem*>::iterator i;
    for (i=allListViewItems.begin(); i!=allListViewItems.end(); ++i)
    {
        newobj.insert( mw->db()->findInIndex((*i).first) );
    }

    for (FWObject::iterator j=obj->begin(); j!=obj->end(); ++j)
    {
        FWObject *o= *j;
        if (FWReference::cast(o)!=NULL)
            o=FWReference::cast(o)->getPointer();
        oldobj.insert( o );
    }

    set<FWObject*> diff;
    set_difference( oldobj.begin(), oldobj.end(),
                    newobj.begin(), newobj.end(),
                    inserter(diff,diff.begin()));
/* diff contains objects present in oldobj but not in newobj - these objects
   were deleted from the group */

    for (set<FWObject*>::iterator k=diff.begin(); k!=diff.end(); ++k)
    {
        if (m_project->isSystem(obj)) mw->delObj(*k, false);
        else               obj->removeRef( *k );
    }

    diff.clear();

    set_difference( newobj.begin(), newobj.end(),
                    oldobj.begin(), oldobj.end(),
                    inserter(diff,diff.begin()));
/* diff contains objects present in newobj but not in oldobj - these objects
   were added to the group */

    for (set<FWObject*>::iterator k1=diff.begin(); k1!=diff.end(); ++k1)
    {
        if (m_project->isSystem(obj))  mw->pasteTo(obj,*k1, false);
        else                obj->addRef( *k1);
    }

    mw->updateObjName(obj,QString::fromUtf8(oldname.c_str()));

    init=true;

/* move to another lib if we have to */
    if (! m_project->isSystem(obj) && m_dialog->libs->currentText() != QString(obj->getLibrary()->getName().c_str()))
        mw->moveObject(m_dialog->libs->currentText(), obj);

    init=false;

    //apply->setEnabled( false );
    mw->updateLastModifiedTimestampForAllFirewalls(obj);

    if (fwbdebug)
        qDebug("GroupObjectDialog::applyChanges done");
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

void GroupObjectDialog::openObject()
{
    if (selectedObject!=NULL)
    {
        mw->openObject( selectedObject );
        mw->editObject( selectedObject );
    }
}

void GroupObjectDialog::openObject(QTreeWidgetItem *itm)
{
    ObjectListViewItem *otvi=dynamic_cast<ObjectListViewItem*>(itm);
    assert(otvi!=NULL);

    FWObject *o = otvi->getFWObject();
    if (o!=NULL)
    {
        mw->openObject( o );
        mw->editObject( o );
    }
}

void GroupObjectDialog::openObject(QListWidgetItem *itm)
{
    ObjectIconViewItem *oivi=dynamic_cast<ObjectIconViewItem*>(itm);
    assert(oivi!=NULL);

    FWObject *o = oivi->getFWObject();
    if (o!=NULL)
    {
        mw->openObject( o );
        mw->editObject( o );
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
        ObjectTreeView *otv = mw->getCurrentObjectTree();
        otv->ignoreNextMouseReleaseEvent();

    }
    if (fwbdebug) qDebug("GroupObjectDialog::dropped  done");
}

void GroupObjectDialog::iconContextMenu(const QPoint & pos)
{
    FWObject *o=NULL;
    ObjectIconViewItem *oivi=dynamic_cast<ObjectIconViewItem*>(
            iconView->itemAt(pos));
    if (oivi!=NULL) o = oivi->getFWObject();
    selectedObject=o;

    setupPopupMenu(iconView->mapToGlobal(pos));
}


void GroupObjectDialog::listContextMenu(const QPoint & pos)
{
    FWObject *o=NULL;
    ObjectListViewItem *otvi=dynamic_cast<ObjectListViewItem*>(
            listView->itemAt(pos));
    if (otvi!=NULL) o = otvi->getFWObject();
    selectedObject=o;

    setupPopupMenu(listView->viewport()->mapToGlobal(pos));
}

void GroupObjectDialog::setupPopupMenu(const QPoint &pos)
{
    QMenu *popup=new QMenu(this);

    if (selectedObject!=NULL)
    {
        if (selectedObject->isReadOnly() )
            popup->addAction( tr("Open") ,  this , SLOT( openObject())    );
        else
            popup->addAction( tr("Edit") ,  this , SLOT( openObject())    );
    }

    QAction *copyID =popup->addAction( tr("Copy") ,  this , SLOT( copyObj())   );
    QAction *cutID  =popup->addAction( tr("Cut") ,   this , SLOT( cutObj())    );
    QAction *pasteID=popup->addAction( tr("Paste") , this , SLOT( pasteObj())  );
    QAction *delID  =popup->addAction( tr("Delete") ,this , SLOT( deleteObj()) );

    copyID->setEnabled(selectedObject!=NULL &&
                          ! m_project->isSystem(selectedObject) );
    cutID->setEnabled(selectedObject!=NULL &&
                          ! m_project->isSystem(obj) &&
                          ! obj->isReadOnly() );
    pasteID->setEnabled(! m_project->isSystem(obj) &&
                          ! obj->isReadOnly() );
    delID->setEnabled(selectedObject!=NULL &&
                          ! m_project->isSystem(obj) &&
                          ! obj->isReadOnly() );

    popup->exec( pos );
}

void GroupObjectDialog::copyObj()
{
    FWObjectClipboard::obj_clipboard->clear();
    for(vector<FWObject*>::iterator it=selectedObjects.begin();
            it!=selectedObjects.end(); ++it)
    {
        FWObject* selectedObject=*it;

        if (selectedObject!=NULL && ! m_project->isSystem(selectedObject) )
        {
            FWObject *o=selectedObject;
            if (FWReference::cast(o)!=NULL)
                o=FWReference::cast(o)->getPointer();


            FWObjectClipboard::obj_clipboard->add( o,this->m_project );
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
    vector<string>::iterator i;

    for (i= FWObjectClipboard::obj_clipboard->begin();
         i!=FWObjectClipboard::obj_clipboard->end(); ++i)
    {
        insertObject( mw->db()->findInIndex(*i) );
    }

//    if (FWObjectClipboard::obj_clipboard->getObject()==NULL) return;
//    insertObject( FWObjectClipboard::obj_clipboard->getObject() );
}

void GroupObjectDialog::deleteObj()
{
    vector<FWObject*> tv;
    FWObject* selectedObject;
    copy (selectedObjects.begin(),selectedObjects.end(),inserter(tv,tv.begin()));

    for(vector<FWObject*>::iterator it=tv.begin();
            it!=tv.end(); ++it)
    {
        selectedObject=(*it);

        if (selectedObject!=NULL &&  ! m_project->isSystem(obj) )
        {
            Group *g = dynamic_cast<Group*>(obj);
            assert(g!=NULL);

            FWObject *o=selectedObject;
            assert(o!=NULL);
            if (FWReference::cast(o)!=NULL)
                o=FWReference::cast(o)->getPointer();

            selectedObject=NULL;

    //        g->removeRef(o);

            assert(allListViewItems[o->getId()]!=NULL);
            delete allListViewItems[o->getId()];
            allListViewItems.erase(o->getId());

            assert(allIconViewItems[o->getId()]!=NULL);
            delete allIconViewItems[o->getId()];
            allIconViewItems.erase(o->getId());

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

/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void GroupObjectDialog::closeEvent(QCloseEvent *e)
{
    saveColumnWidths();
    emit close_sign(e);
}

void GroupObjectDialog::hideEvent(QHideEvent *)
{
    saveColumnWidths();
}

void GroupObjectDialog::selectObject(FWObject *o)
{
//    ObjectListViewItem* list_item=allListViewItems[o->getId()];
    ObjectIconViewItem* icon_item=allIconViewItems[o->getId()];

    iconView->setCurrentItem (icon_item);
}
