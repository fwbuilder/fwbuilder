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
#include "utils_no_qt.h"
#include "platforms.h"

#include "ObjectManipulator.h"
#include "ObjectEditor.h"
#include "ObjectTreeViewItem.h"
#include "ObjectTreeView.h"
#include "FWObjectClipboard.h"
#include "FWObjectPropertiesFactory.h"
#include "FWBSettings.h"
#include "newFirewallDialog.h"
#include "newClusterDialog.h"
#include "newHostDialog.h"
#include "findDialog.h"
#include "newGroupDialog.h"
#include "FindObjectWidget.h"
#include "AskLibForCopyDialog.h"
#include "FindWhereUsedWidget.h"
#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"
#include "events.h"
#include "FWCmdChange.h"
#include "FWCmdAddObject.h"

#include <QTextEdit>
#include <QTime>
#include <QtDebug>

#include <qobject.h>
#include <qobject.h>
#include <qimage.h>
#include <qpixmapcache.h>
#include <qheaderview.h>
#include <QStackedWidget>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qaction.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qsplitter.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qmenu.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qstatusbar.h>
#include <qeventloop.h>
#include <QPixmap>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QRegExp>
#include <QUndoStack>

#include "DialogFactory.h"
#include "FWBTree.h"
#include "FWWindow.h"
#include "ProjectPanel.h"
#include "ConfirmDeleteObjectDialog.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ObjectGroup.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/RuleElement.h"

#include "fwbuilder/CustomService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UserService.h"

#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/Management.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <memory>

using namespace std;
using namespace libfwbuilder;

#define OBJTREEVIEW_WIDGET_NAME  "ObjTreeView"



ObjectTreeViewItem* ObjectManipulator::insertObject(ObjectTreeViewItem *itm,
                                                    FWObject *obj)
{
    if (FWReference::cast(obj)!=NULL) return NULL;
    if (Resources::global_res->getObjResourceBool(obj,"hidden") ) return NULL;

    ObjectTreeViewItem *nitm = NULL;

    if (Resources::global_res->getResourceBool(
            string("/FWBuilderResources/Type/") +
            obj->getTypeName() + "/hidden")) return NULL;

    nitm = new ObjectTreeViewItem( itm );

    nitm->setLib("");
    nitm->setText( 0, QString::fromUtf8(obj->getName().c_str()) );
    nitm->setText( 1, getTreeLabel(obj) );

    QPixmap pm;
    doSetObjectIcon(obj, &pm, 0);

    nitm->setIcon( 0, QIcon(pm) );
//    nitm->setIcon( 1, QIcon(pm) );
    nitm->setFlags(nitm->flags() | Qt::ItemIsDragEnabled);

    nitm->setProperty("type", obj->getTypeName().c_str() );
    nitm->setFWObject( obj );

    allItems[obj] = nitm;

//    itm->sortChildren(0, Qt::AscendingOrder);

    return nitm;
}

void ObjectManipulator::insertSubtree(FWObject *parent, FWObject *obj)
{
    ObjectTreeViewItem* parent_item = allItems[parent];
    insertSubtree(parent_item, obj);
    QTreeWidgetItem *itm = allItems[parent];
    if (itm==NULL) return;
    refreshSubtree(itm, NULL);
}

void ObjectManipulator::insertSubtree(ObjectTreeViewItem *itm, FWObject *obj)
{
    this->m_objectManipulator->filter->clearEditText();
    ObjectTreeViewItem *nitm = insertObject(itm, obj);
    if (nitm==NULL) return;

    if (FWBTree().isStandardFolder(obj)) nitm->setExpanded( st->getExpandTree());

    if (Cluster::isA(obj))
    {
         for (FWObjectTypedChildIterator it = obj->findByType(StateSyncClusterGroup::TYPENAME);
              it != it.end(); ++it) insertSubtree( nitm, *it );
    }

    if (Cluster::isA(obj) || Firewall::isA(obj))
    {
         for (FWObjectTypedChildIterator it = obj->findByType(Interface::TYPENAME);
              it != it.end(); ++it) insertSubtree( nitm, *it );

         for (FWObjectTypedChildIterator it = obj->findByType(Policy::TYPENAME);
              it != it.end(); ++it) insertSubtree( nitm, *it );

         for (FWObjectTypedChildIterator it = obj->findByType(NAT::TYPENAME);
              it != it.end(); ++it) insertSubtree( nitm, *it );

         for (FWObjectTypedChildIterator it = obj->findByType(Routing::TYPENAME);
              it != it.end(); ++it) insertSubtree( nitm, *it );

         return ;
    }

    if (Interface::isA(obj))
    {
         for (FWObjectTypedChildIterator it = obj->findByType(Interface::TYPENAME);
              it != it.end(); ++it) insertSubtree( nitm, *it );
         for (FWObjectTypedChildIterator it = obj->findByType(IPv4::TYPENAME);
              it != it.end(); ++it) insertSubtree( nitm, *it );
         for (FWObjectTypedChildIterator it = obj->findByType(IPv6::TYPENAME);
              it != it.end(); ++it) insertSubtree( nitm, *it );
         for (FWObjectTypedChildIterator it = obj->findByType(physAddress::TYPENAME);
              it != it.end(); ++it) insertSubtree( nitm, *it );
         for (FWObjectTypedChildIterator it = obj->findByType(FailoverClusterGroup::TYPENAME);
              it != it.end(); ++it) insertSubtree( nitm, *it );

         return;
    }


    for (list<FWObject*>::iterator m=obj->begin(); m!=obj->end(); m++)
    {
        FWObject *o1=*m;
        if (FWReference::cast(o1)!=NULL) continue;
        insertSubtree( nitm, o1 );
    }
}

void ObjectManipulator::removeObjectFromTreeView(FWObject *obj)
{
//    QTreeWidget *objTreeView = idxToTrees[ getIdxForLib(getCurrentLib()) ];
    int current_lib_idx = m_objectManipulator->libs->currentIndex();
    QTreeWidget *objTreeView = idxToTrees[current_lib_idx];
    assert(objTreeView);
    dynamic_cast<ObjectTreeView*>(objTreeView)->clearLastSelected();

    ObjectTreeViewItem *itm = allItems[obj];
    allItems[obj] = NULL;
    itm->parent()->takeChild(itm->parent()->indexOfChild(itm));
    delete itm;
}

void ObjectManipulator::updateLibColor(FWObject *lib)
{
    QString clr = lib->getStr("color").c_str();
    int index = getIdxForLib(lib);
    if (index >= 0)
    {
        QTreeWidget *objTreeView = idxToTrees[index];
        if (clr=="" || clr=="#000000" || clr=="black") clr="#FFFFFF";
        QPalette palette = objTreeView->palette();
        palette.setColor(QPalette::Active, QPalette::Base, QColor( clr ));
        objTreeView->setPalette(palette);
    }
}

/*
 * Update tree item for the given object, including its name and brief summary
 * of properties. If @subtree=true, do the same for all its children as well.
 */
void ObjectManipulator::updateObjectInTree(FWObject *obj, bool subtree)
{
    if (fwbdebug)
        qDebug() << "ObjectManipulator::updateObjectInTree"
                 << "obj=" << obj->getName().c_str()
                 << "subtree=" << subtree;

    QTreeWidgetItem *itm = allItems[obj];
    if (itm==NULL) return;

    // first, update tree item that represents @obj. Its name or label
    // (second column) might have changed
    QString old_itm_text = itm->text(0);
    itm->setText( 0, QString::fromUtf8(obj->getName().c_str()) );
    itm->setText( 1, getTreeLabel(obj) );
    getCurrentObjectTree()->updateTreeIcons();

    if (itm->parent())
        refreshSubtree(itm->parent(), itm);

    // now if we need to update subtree, call refreshSubtree()
    if (subtree)
        refreshSubtree(itm, NULL);
}

void ObjectManipulator::clearObjects()
{
    if (fwbdebug) qDebug("ObjectManipulator::clearObjects %p start",this);

    while (history.size()!=0) history.pop();

    int N = m_objectManipulator->libs->count();

    if (fwbdebug)
    {
        qDebug("ObjectManipulator::clearObjects %d libs, "
               "idxToLibs size: %d, idxToTrees size: %d", N,
               int(idxToLibs.size()),
               int(idxToTrees.size()));
    }

    for (int i=N-1; i>=0; i--)
    {
        QTreeWidget *otv = idxToTrees[i];
        assert(otv!=NULL);
        m_objectManipulator->widgetStack->removeWidget( otv );
       // delete otv;

        removeLib(i);
    }
    idxToLibs.clear();
    idxToTrees.clear();
    m_objectManipulator->libs->clear();

    if (fwbdebug) qDebug("ObjectManipulator::clearObjects done");
}

void ObjectManipulator::reload()
{
    FWObject *current_lib = getCurrentLib();
    FWObject *currentObj = getSelectedObject();

    saveExpandedTreeItems();
    saveSectionSizes();
    loadObjects();
    openLib(current_lib);
    loadExpandedTreeItems();
    loadSectionSizes();

    if (currentObj) openObjectInTree(currentObj);
}

void ObjectManipulator::loadObjects()
{
    if (fwbdebug) qDebug("ObjectManipulator::loadObjects %p start", this);

    clearObjects();

    FWObject *firstUserLib = NULL;
    list<FWObject*> ll = m_project->db()->getByType( Library::TYPENAME );

    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        FWObject *lib = (*i);

        if (fwbdebug)
            qDebug("ObjectManipulator::loadObjects %p lib %p %s %s",
                   this,
                   lib,
                   FWObjectDatabase::getStringId(lib->getId()).c_str(),
                   lib->getName().c_str() );

        if ( lib->getId()==FWObjectDatabase::DELETED_OBJECTS_ID &&
             ! st->getBool("UI/ShowDeletedObjects")) continue;

        if ( lib->getId()!=FWObjectDatabase::STANDARD_LIB_ID &&
             lib->getId()!=FWObjectDatabase::TEMPLATE_LIB_ID &&
             firstUserLib==NULL) firstUserLib = *i;

        addLib( lib );

        if (fwbdebug) qDebug("ObjectManipulator::loadObjects %p added lib %s",
                             this, lib->getName().c_str());
    }

    if (firstUserLib==NULL) firstUserLib=ll.front();
    openLib( firstUserLib );
    if (fwbdebug) qDebug("ObjectManipulator::loadObjects %p done", this);
}

void ObjectManipulator::addLib(FWObject *lib)
{
    if (fwbdebug) qDebug("Object Manipulator::addLib lib: %s",
                         lib->getName().c_str());

    ObjectTreeView *objTreeView = new ObjectTreeView(
        m_project, m_objectManipulator->widgetStack, OBJTREEVIEW_WIDGET_NAME );

    QString newlibname = QString::fromUtf8(lib->getName().c_str());
    int N = m_objectManipulator->libs->count();
    int idx = 0;
    vector<FWObject*>::iterator  i1=idxToLibs.begin();
    vector<QTreeWidget*>::iterator i2=idxToTrees.begin();
    for ( ; idx<N; ++idx,++i1,++i2)
        if ( m_objectManipulator->libs->itemText(idx) > newlibname ) break;

    QPixmap pm;
    doSetObjectIcon(lib, &pm, 0);

    m_objectManipulator->libs->insertItem( idx, pm, newlibname);

    m_objectManipulator->libs->setCurrentIndex(idx);

    idxToLibs.insert(i1, lib);
    idxToTrees.insert(i2, objTreeView);

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    policy.setHorizontalStretch(0);
    policy.setVerticalStretch(0);
    policy.setHeightForWidth(objTreeView->sizePolicy().hasHeightForWidth());

    objTreeView->setSizePolicy(policy);

    m_objectManipulator->widgetStack->addWidget( objTreeView );
    m_objectManipulator->widgetStack->show();
    objTreeView->show();

    updateLibColor( lib );

    connect(m_objectManipulator->widgetStack, SIGNAL( currentChanged(int) ),
             this, SLOT( currentTreePageChanged(int) ) );

    connect(objTreeView, SIGNAL( editCurrentObject_sign() ),
            this, SLOT( editSelectedObject()) );

    connect(objTreeView,
            SIGNAL( switchObjectInEditor_sign(libfwbuilder::FWObject*) ),
            this, SLOT( switchObjectInEditor(libfwbuilder::FWObject*)) );

    connect(objTreeView, SIGNAL( deleteObject_sign(libfwbuilder::FWObject*) ),
             this, SLOT( delObj() ) );

    connect(objTreeView, SIGNAL( objectDropped_sign(libfwbuilder::FWObject*) ),
             this, SLOT( openObjectInTree(libfwbuilder::FWObject*) ) );

    connect(objTreeView, SIGNAL( contextMenuRequested_sign(const QPoint&) ),
             this, SLOT( contextMenuRequested(const QPoint&) ) );

    connect(objTreeView,
            SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*) ),
            this, SLOT(selectionChanged(QTreeWidgetItem*)));


    ObjectTreeViewItem *itm1=new ObjectTreeViewItem( objTreeView );

    itm1->setLib("");
    itm1->setExpanded(TRUE);

/* need to enable dragging in order to avoid object highlighting in
 * the tree when user drags mouse cursor */

    itm1->setFlags(itm1->flags() | Qt::ItemIsDragEnabled);

    itm1->setText( 0, QString::fromUtf8(lib->getName().c_str()) );
    itm1->setText( 1, getTreeLabel(lib) );
    itm1->setIcon( 0, pm);

    itm1->setProperty("type", lib->getTypeName().c_str() );
    itm1->setFWObject( lib );
    allItems[lib] = itm1;

    for (list<FWObject*>::iterator m=lib->begin(); m!=lib->end(); m++)
        insertSubtree( itm1, (*m) );

    objTreeView->updateTreeIcons();
    // apparently sortByColumn does not work in QT 4.5, use sortItems
    objTreeView->sortItems(0, Qt::AscendingOrder);
    objTreeView->header()->resizeSections(QHeaderView::ResizeToContents);

    m_objectManipulator->filter->connect(m_objectManipulator->filter,
                                         SIGNAL(editTextChanged(QString)),
                                         objTreeView, SLOT(setFilter(QString)));
}

void ObjectManipulator::removeLib(FWObject* lib)
{
    removeLib( getIdxForLib(lib) );
}

void ObjectManipulator::removeLib(int id)
{
    int N = m_objectManipulator->libs->count();
    int idx = 0;
    vector<FWObject*>::iterator  i1 = idxToLibs.begin();
    vector<QTreeWidget*>::iterator i2 = idxToTrees.begin();
    for ( ; idx<N; ++idx,++i1,++i2)
    {
        if ( idx==id )
        {
            m_objectManipulator->libs->removeItem( idx );
            idxToLibs.erase(i1);
            idxToTrees.erase(i2);
            break;
        }
    }
}

void ObjectManipulator::refreshSubtree(QTreeWidgetItem *parent, QTreeWidgetItem *itm)
{
    if (fwbdebug)
        qDebug() << "ObjectManipulator::refreshSubtree parent:"
                 << parent->text(0)
                 << "itm:" << QString((itm)?itm->text(0):"");
    /*
     * re-sorting parent tree item causes havoc. If I do not
     * collapse/expand it, I get strange glitches in display.
     */
    parent->sortChildren(0, Qt::AscendingOrder);//();

    if (fwbdebug)
        qDebug("ObjectManipulator::refreshSubtree expand/collapse parent");
    /*
     * workaround for QT4 bug
     * http://www.qtsoftware.com/developer/task-tracker/index_html?method=entry&id=233975
     * Affects QT 4.4.1
     *
     * This has a side effect in that the tree loses its scrollong
     * position and scrolls all the way to the top. If the object
     * being edited was in the middle or close to the bottom, it disappears
     * from view. Call to scrollToItem() fixes this.
     */
    parent->setExpanded(false);
    parent->setExpanded(true);
    //getCurrentObjectTree()->header()->resizeSections(QHeaderView::ResizeToContents);
    if (itm)
        getCurrentObjectTree()->scrollToItem(itm, QAbstractItemView::EnsureVisible);
    getCurrentObjectTree()->update();
}

