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

#include "listOfLibrariesModel.h"
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
#include "IconSetter.h"

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
#include <QScrollBar>

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


/*
 * This method decides what should be shown in the columns 0 and 1 of
 * the tree for the given object
 */
QString ObjectManipulator::getTreeLabel(FWObject *obj, int col)
{
    switch (col)
    {
    case 0:
    {
        QString name = QString::fromUtf8(obj->getName().c_str());
        if (Interface::isA(obj))
        {
            Interface *intf = Interface::cast(obj);
            QString label = QString::fromUtf8(intf->getLabel().c_str());
            if (label.isEmpty())
                return name;
            else
                return QString("%1 (%2)").arg(name).arg(label);
        } else
            return name;
        break;
    }
    case 1:
        return FWObjectPropertiesFactory::getObjectPropertiesBrief(obj);
    }
    return "";
}

void ObjectManipulator::expandObjectInTree(FWObject *obj)
{
    FWObject *o = FWReference::getObject(obj);

    if (fwbdebug)
        qDebug() << "ObjectManipulator::expandObjectInTree"
                 << "o=" << QString::fromUtf8(o->getName().c_str());

    //if (FWReference::cast(o)!=NULL) o = FWReference::cast(o)->getPointer();

    QTreeWidgetItem *it = allItems[o];
    if (it==NULL)
    {
        if (fwbdebug) qDebug() << "####  Tree node not found";
        return;
    }

    expandOrCollapseCurrentTreeNode(it, true);
}

void ObjectManipulator::expandOrCollapseCurrentTreeNode(QTreeWidgetItem *item,
                                                        bool expand)
{
    QTreeWidgetItem *parent = item->parent();
    if (expand && ! parent->isExpanded()) parent->setExpanded(true);
    item->setExpanded(expand);
    for (int i=0; i<item->childCount(); ++i)
        expandOrCollapseCurrentTreeNode(item->child(i), expand);
}

void ObjectManipulator::expandCurrentTreeNode()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;
    QTreeWidgetItem *item = getCurrentObjectTree()->currentItem();
    expandOrCollapseCurrentTreeNode(item, true);
}

void ObjectManipulator::collapseCurrentTreeNode()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;
    QTreeWidgetItem *item = getCurrentObjectTree()->currentItem();
    expandOrCollapseCurrentTreeNode(item, false);
}


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
    nitm->setText( 0, getTreeLabel(obj, 0) );
    nitm->setText( 1, getTreeLabel(obj, 1) );

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
    removeObjectFromHistory(obj);

    int current_lib_idx = m_objectManipulator->libs->currentIndex();

    ObjectTreeView *objTreeView = libs_model->getTreeWidget(current_lib_idx);
    assert(objTreeView);
    objTreeView->clearLastSelected();

    ObjectTreeViewItem *itm = allItems[obj];
    allItems[obj] = NULL;
    if (itm && itm->parent())
    {
        itm->parent()->takeChild(itm->parent()->indexOfChild(itm));
        delete itm;
    }

    if (Library::isA(obj))
    {
        removeLib(obj);
    }
}

bool FindHistoryItemByObjectId::operator()(const HistoryItem &itm)
{
    return (itm.id() == id);
}

void ObjectManipulator::removeObjectFromHistory(FWObject *obj)
{
    if (fwbdebug)
        qDebug() << "ObjectManipulator::removeObjectFromHistory"
                 << "obj:" << obj->getName().c_str()
                 << "id=" << obj->getId()
                 << "history.size()=" << history.size();

    history.remove_if(FindHistoryItemByObjectId(obj->getId()));

    if (fwbdebug)
        qDebug() << "ObjectManipulator::removeObjectFromHistory"
                 << "history.size()=" << history.size();

    if (history.empty()) mw->enableBackAction();
}

void ObjectManipulator::updateLibColor(FWObject *lib)
{
    QString clr = lib->getStr("color").c_str();
    QModelIndex index = libs_model->getIdxForLib(lib);
    if (index.isValid())
    {
        QTreeWidget *objTreeView = libs_model->getTreeWidget(index);

        if (clr=="" || clr=="#000000" || clr=="black") clr="#FFFFFF";
        QPalette palette = objTreeView->palette();
        palette.setColor(QPalette::Active, QPalette::Base, QColor( clr ));
        palette.setColor(QPalette::Inactive, QPalette::Base, QColor( clr ));
        objTreeView->setPalette(palette);
    }
}

void ObjectManipulator::updateLibName(FWObject *lib)
{
    QModelIndex index = libs_model->getIdxForLib(lib);
    if (index.isValid())
    {
        QString newlibname = QString::fromUtf8(lib->getName().c_str());
        libs_model->setName(index, newlibname);
        libs_model->sort(0, Qt::AscendingOrder);
        QModelIndex lib_idx = libs_model->getIdxForLib(lib);
        m_objectManipulator->libs->setCurrentIndex(lib_idx.row());
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
    itm->setText( 0, getTreeLabel(obj, 0) );
    itm->setText( 1, getTreeLabel(obj, 1) );

    QPixmap pm_obj;
    IconSetter::setObjectIcon(obj, &pm_obj, 0);
    itm->setIcon(0, pm_obj );

    //getCurrentObjectTree()->updateTreeIcons();

    if (itm->parent())
        refreshSubtree(itm->parent(), itm);

    // now if we need to update subtree, call refreshSubtree()
    if (subtree)
        refreshSubtree(itm, NULL);
}

void ObjectManipulator::clearObjects()
{
    if (fwbdebug) qDebug("ObjectManipulator::clearObjects %p start",this);

    while (history.size()!=0) history.pop_back();

    int N = libs_model->rowCount();

    for (int i=N-1; i>=0; i--)
    {

        QTreeWidget *objTreeView = libs_model->getTreeWidget(i);
        if (objTreeView == NULL) continue;
        m_objectManipulator->widgetStack->removeWidget(objTreeView);
       // delete otv;

        removeLib(i);
    }

    libs_model->removeRows(0, libs_model->rowCount());

    libs_model->addStaticItems();
    
    current_tree_view = NULL;

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
    if (fwbdebug) qDebug() << "Object Manipulator::addLib lib: " << lib->getName().c_str();

    ObjectTreeView *objTreeView = new ObjectTreeView(
        m_project, m_objectManipulator->widgetStack, OBJTREEVIEW_WIDGET_NAME );

    QString newlibname = QString::fromUtf8(lib->getName().c_str());

    int idx = libs_model->rowCount();
    if (fwbdebug) qDebug() << "Adding at idx=" << idx;

    QPixmap pm;
    doSetObjectIcon(lib, &pm, 0);

    libs_model->insertRows(idx, 1);
    QModelIndex model_idx = libs_model->index(idx, 0);
    libs_model->setData(model_idx, newlibname, lib, objTreeView);
    libs_model->sort(0, Qt::AscendingOrder);

    // after sorting the row of the new library may be different from where we added it
    QModelIndex lib_idx = libs_model->getIdxForLib(lib);
    m_objectManipulator->libs->setCurrentIndex(lib_idx.row());




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

    itm1->setText( 0, getTreeLabel(lib, 0) );
    itm1->setText( 1, getTreeLabel(lib, 1) );
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
    if (fwbdebug) qDebug() << "ObjectManipulator::removeLib lib=" << lib;

    QModelIndex idx = libs_model->getIdxForLib(lib);
    if (idx.isValid())
        removeLib( idx.row() );

}

void ObjectManipulator::removeLib(int row)
{
    if (fwbdebug) qDebug() << "ObjectManipulator::removeLib row=" << row;
    libs_model->removeRows(row, 1);
    m_objectManipulator->libs->setCurrentIndex(libs_model->rowCount() - 1);
    //libs_model->reset();
}

void ObjectManipulator::refreshSubtree(QTreeWidgetItem *parent, QTreeWidgetItem *itm)
{
    if (fwbdebug)
        qDebug() << "ObjectManipulator::refreshSubtree parent:"
                 << parent->text(0)
                 << "itm:" << QString((itm)?itm->text(0):"");

    QScrollBar* scrollbar = getCurrentObjectTree()->verticalScrollBar();

    // remember current scrolling position
    int y_pos = scrollbar->value();

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

    if (itm)
        getCurrentObjectTree()->scrollToItem(itm, QAbstractItemView::EnsureVisible);

    scrollbar->setValue(y_pos);

    getCurrentObjectTree()->update();
}

