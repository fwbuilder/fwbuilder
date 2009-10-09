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

#include <QMessageBox>
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


HistoryItem::~HistoryItem() {}

ObjectManipulator::~ObjectManipulator()
{
    delete m_objectManipulator;
}

ObjectManipulator::ObjectManipulator(QWidget *parent):
    QWidget(parent), currentObj(0), current_tree_view(0)
{
    m_objectManipulator = new Ui::ObjectManipulator_q;
    m_objectManipulator->setupUi(this);
    setObjectName(tr("Object Manipulator"));

    m_project = NULL;

    treeWidth    = -1;
    treeHeight   = -1;
    currentObj   = NULL;
    active       = false;
    current_tree_view=NULL;

    // used in duplicateWithDependencies()
    dedup_marker_global_counter = time(NULL);

//    setFocusPolicy( QWidget::StrongFocus  );

/* Adding pop-down menu to the button "New" */

    QString icon_path=":/Icons/";

    QMenu* newObjectPopup = new QMenu( this );

    newObjectPopup->addAction(QIcon(icon_path+Library::TYPENAME+"/icon-tree"),
                              tr( "New &Library"), this, SLOT( newLibrary() ));

    newObjectPopup->addSeparator();
    newObjectPopup->addAction(QIcon(icon_path+Firewall::TYPENAME+"/icon-tree"), 
                              tr("New Firewall"), this, SLOT( newFirewall()));
    newObjectPopup->addAction(QIcon(icon_path+Cluster::TYPENAME+"/icon-tree"),
                              tr("New Cluster"), this, SLOT(newCluster()));
    newObjectPopup->addAction(QIcon(icon_path+Host::TYPENAME+"/icon-tree"),
                              tr("New Host"), this, SLOT( newHost() ));
    newObjectPopup->addAction(QIcon(icon_path+Interface::TYPENAME+"/icon-tree"),
                              tr("New Interface"), this, SLOT(newInterface()));
    newObjectPopup->addAction(QIcon(icon_path+Network::TYPENAME+"/icon-tree"), 
                              tr("New Network"), this, SLOT( newNetwork() ));
    newObjectPopup->addAction(QIcon(icon_path+NetworkIPv6::TYPENAME+"/icon-tree"),
                              tr("New Network IPv6"), this,
                              SLOT(newNetworkIPv6() ));    
    newObjectPopup->addAction(QIcon(icon_path+IPv4::TYPENAME+"/icon-tree"),
                              tr("New Address"), this, SLOT(newAddress() ));
    newObjectPopup->addAction(QIcon(icon_path+IPv6::TYPENAME+"/icon-tree"),
                              tr("New Address IPv6"), this,
                              SLOT(newAddressIPv6() ));
    newObjectPopup->addAction(QIcon(icon_path+DNSName::TYPENAME+"/icon-tree"),
                              tr("New DNS Name"), this, SLOT(newDNSName() ));
    newObjectPopup->addAction(QIcon(icon_path+AddressTable::TYPENAME+"/icon-tree"),
                              tr("New Address Table"), this,
                              SLOT(newAddressTable() ));
    newObjectPopup->addAction(QIcon(icon_path+AddressRange::TYPENAME+"/icon-tree"),
                              tr("New Address Range"), this,
                              SLOT(newAddressRange() ));
    newObjectPopup->addAction(QIcon(icon_path+ObjectGroup::TYPENAME+"/icon-tree"),
                              tr("New Object Group"), this,
                              SLOT(newObjectGroup() ));

    newObjectPopup->addSeparator();
    newObjectPopup->addAction(QIcon(icon_path+CustomService::TYPENAME+"/icon-tree"),
                              tr("New Custom Service"), this,
                              SLOT(newCustom() ));
    newObjectPopup->addAction(QIcon(icon_path+IPService::TYPENAME+"/icon-tree"),
                              tr("New IP Service"), this,
                              SLOT(newIP() ));
    newObjectPopup->addAction(QIcon(icon_path+ICMPService::TYPENAME+"/icon-tree"),
                              tr("New ICMP Service"), this,
                              SLOT(newICMP() ));
    newObjectPopup->addAction(QIcon(icon_path+ICMP6Service::TYPENAME+"/icon-tree"),
                              tr("New ICMP6 Service"), this,
                              SLOT(newICMP6() ));
    newObjectPopup->addAction(QIcon(icon_path+TCPService::TYPENAME+"/icon-tree"),
                              tr("New TCP Serivce"), this,
                              SLOT(newTCP() ));
    newObjectPopup->addAction(QIcon(icon_path+UDPService::TYPENAME+"/icon-tree"),
                              tr("New UDP Service"), this,
                              SLOT(newUDP() ));
    newObjectPopup->addAction(QIcon(icon_path+TagService::TYPENAME+"/icon-tree"),
                              tr("New TagService"), this,
                              SLOT(newTagService() ));
    newObjectPopup->addAction(QIcon(icon_path+UserService::TYPENAME+"/icon-tree"),
                              tr("New User Service"), this,
                              SLOT(newUserService() ));
    newObjectPopup->addAction(QIcon(icon_path+ServiceGroup::TYPENAME+"/icon-tree"),
                              tr("New Service Group"), this,
                              SLOT(newServiceGroup() ));

    newObjectPopup->addSeparator();
    newObjectPopup->addAction(QIcon(icon_path+Interval::TYPENAME+"/icon-tree"),
                              tr( "New Time Interval"), this,
                              SLOT( newInterval() ));
//    QToolButton *btn = (QToolButton*)toolBar->child("newObjectAction_action_button");

    m_objectManipulator->newButton->setMenu( newObjectPopup );
}

void ObjectManipulator::setupProject(ProjectPanel *project)
{
    m_project = project;
}

/*
 * This method decides what should be shown in the column #1 of the
 * tree for the given object
 */
QString ObjectManipulator::getTreeLabel( FWObject *obj )
{
    QString name;
    if (RuleSet::cast(obj)) return "";

    return FWObjectPropertiesFactory::getObjectPropertiesBrief(obj);
}

void ObjectManipulator::refreshSubtree(QTreeWidgetItem *itm)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::refreshSubtree %s", itm->text(0).toLatin1().constData());

    QTreeWidgetItem *parent = itm->parent();
    if (parent)
    {
        /*
         * re-sorting parent tree item causes havoc. If I do not
         * collapse/expand it, I get strange glitches in display. If I
         * collapse/expand it, it scrolls the tree up. If I use
         * scrollToItem() to force scrolling position, I get problems
         * when this method is called from ProjectPanel::updateLastModifiedTimestampForAllFirewalls
         * (via event) since the user modified one object, but this method
         * repositions the tree to show the firewall that uses it.
         *
         * To work around this I sort the parent and see if the item
         * next to the itm has changed. If it has, then sorting order
         * has changed and we need to collapse then expand and
         * scroll. If sorting order has not changed, this means the
         * update was only to the attributes of the object or the name
         * sorted in the same order and we do not need to
         * expand/collapse the subtree.
         */

        QTreeWidgetItemIterator old_neighbor_iter(itm);
        old_neighbor_iter--;
        QTreeWidgetItem *old_neighbor = *old_neighbor_iter;

        parent->sortChildren(0, Qt::AscendingOrder);//();

        QTreeWidgetItemIterator new_neighbor_iter(itm);
        new_neighbor_iter--;
        QTreeWidgetItem *new_neighbor = *new_neighbor_iter;

        if (old_neighbor != new_neighbor)
        {
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
            getCurrentObjectTree()->scrollToItem(itm, QAbstractItemView::EnsureVisible);
            getCurrentObjectTree()->update();
        }
    }
}

void ObjectManipulator::insertObjectInTree(FWObject *parent, FWObject *obj)
{
    ObjectTreeViewItem* parent_item = allItems[parent];
    insertObject(parent_item, obj);
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
    nitm->setText( 0, obj->getName().c_str() );
    nitm->setText( 1, getTreeLabel(obj) );

    QPixmap pm;
    FWBTree().setObjectIcon(obj, &pm);

    nitm->setIcon( 0, QIcon(pm) );
//    nitm->setIcon( 1, QIcon(pm) );
    nitm->setFlags(nitm->flags() | Qt::ItemIsDragEnabled);

    nitm->setProperty("type", obj->getTypeName().c_str() );
    nitm->setFWObject( obj );

    allItems[obj] = nitm;

//    itm->sortChildren(0, Qt::AscendingOrder);

    return nitm;
}

/**
 * This method enforces certain order of items in the tree depending
 * on the parent item type.
 */
void ObjectManipulator::insertSubtree(ObjectTreeViewItem *itm, FWObject *obj)
{
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

void ObjectManipulator::showDeletedObjects(bool f)
{
    try
    {
        FWObject *dobj = m_project->db()->findInIndex(
            FWObjectDatabase::DELETED_OBJECTS_ID);

        if (fwbdebug)
            qDebug("ObjectManipulator::showDeletedObjects f=%d  dobj=%p",
                   f, dobj);

        if (dobj==NULL)
        {
            dobj = m_project->db()->create(Library::TYPENAME);
            dobj->setId(FWObjectDatabase::DELETED_OBJECTS_ID);
            dobj->setName("Deleted Objects");
            dobj->setReadOnly(false);
            m_project->db()->add(dobj);
        }

        int idx = getIdxForLib(dobj);

        if (fwbdebug)
            qDebug("ObjectManipulator::showDeletedObjects idx=%d", idx);

        if (f)
        {
            if (idx>=0) return;
            addTreePage( dobj );
            openLib( dobj );
        } else
        {
            if (idx<0) return;

            QTreeWidget *otv = idxToTrees[idx];

            if (fwbdebug)
                qDebug("ObjectManipulator::showDeletedObjects otv=%p", otv);

            assert(otv!=NULL);
            m_objectManipulator->widgetStack->removeWidget( otv );
            removeLib(idx);
        }
    }
    catch(FWException &ex)
    {
/* we get exception if file is opened read-only and there is no "deleted
 * objects" library yet
 */
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
    QString clr=lib->getStr("color").c_str();

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

int ObjectManipulator::getIdxForLib(FWObject* lib)
{
    for (int i=0; i<m_objectManipulator->libs->count(); i++)
        if ( idxToLibs[i]->getId() == lib->getId() ) return i;

    return -1;
}

void ObjectManipulator::updateLibName(FWObject *lib)
{
    int oldidx = getIdxForLib(lib);
    QTreeWidget *objTreeView = idxToTrees[oldidx];
    QString newlibname = QString::fromUtf8(lib->getName().c_str());

    if (m_objectManipulator->libs->itemText(oldidx) != newlibname)
    {
        removeLib(oldidx);
        addLib(lib, objTreeView);
    }
}

/*
 * Update tree item for the given object, including its name and brief summary
 * of properties. If @subtree=true, do the same for all its children as well.
 */
void ObjectManipulator::updateObjectInTree(FWObject *obj, bool subtree)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::updateObjectInTree  obj=%s", obj->getName().c_str());

    QTreeWidgetItem *itm = allItems[obj];
    assert(itm!=NULL);
    QString old_itm_text = itm->text(0);

    itm->setText( 0, obj->getName().c_str() );
    itm->setText( 1, getTreeLabel(obj) );

    getCurrentObjectTree()->updateTreeIcons();

    if (subtree)
    {
        for (list<FWObject*>::iterator j=obj->begin(); j!=obj->end(); ++j)
        {
            FWObject *child_obj = *j;
            // not all child objects are visible in the tree, so check
            // if the tree item exists
            if (allItems.count(child_obj) > 0)
            {
                updateObjectInTree(child_obj, subtree);
            }
        }
    }

    refreshSubtree(itm);
}

/*
 * TODO: make this signal/slot. Dialogs just emit signal
 * 'updateObject_sign', which objectManipulator should have connected
 * to its slot which would do what updateObjName does now, and more.
 */
void ObjectManipulator::updateObjName(FWObject *obj,
                                      const QString &oldName,
                                      bool  askForAutorename)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::updateObjName  changing name '%s' -> '%s'",
               oldName.toLatin1().constData(),
               QString::fromUtf8(obj->getName().c_str()).toLatin1().constData());
    
    if (oldName == QString::fromUtf8(obj->getName().c_str())) return;
    
    if (obj!=currentObj) openObject(obj);
    
    QTreeWidgetItem *itm = allItems[obj];
    assert(itm!=NULL);
    
    if ((QString::fromUtf8(obj->getName().c_str())!=oldName) &&
        (Host::isA(obj) || Firewall::isA(obj) || Interface::isA(obj)))
    {
        if (fwbdebug) qDebug("ObjectManipulator::updateObjName  autorename");
        autorename(obj, askForAutorename);
        if (fwbdebug) qDebug("ObjectManipulator::updateObjName  autorename done");
    }

    QCoreApplication::postEvent(
        mw, new objectNameChangedEvent(m_project->getFileName(), obj->getId()));
}

/*
 * variant specifically used for interfaces that have name and a label
 */
void ObjectManipulator::updateObjName(FWObject *obj,
                                      const QString &oldName,
                                      const QString &oldLabel,
                                      bool  askForAutorename)
{
    if (obj!=currentObj) openObject(obj);

    QTreeWidgetItem *itm = allItems[obj];
    assert(itm!=NULL);

    if (fwbdebug)
    {
        qDebug("ObjectManipulator::updateObjName  changing name %s -> %s",
               oldName.toLatin1().constData(), QString::fromUtf8(obj->getName().c_str()).toLatin1().constData());
    }

    if ((QString::fromUtf8(obj->getName().c_str()) != oldName) &&
        Interface::isA(obj))
        autorename(obj, askForAutorename);

    // Do not call updateObjectInTree because it will be called
    // from ProjectPanel::event when we process updateObjectInTree event
    //updateObjectInTree(obj);

    Interface *i = Interface::cast(obj);
    if  ((i!=NULL && i->getLabel()!=oldLabel.toLatin1().constData()) ||
         (QString::fromUtf8(obj->getName().c_str()) != oldName))
    {
        QCoreApplication::postEvent(
            mw, new objectNameChangedEvent(m_project->getFileName(), obj->getId()));
//        m_project->scheduleRuleSetRedraw();
    }
}

void ObjectManipulator::autorename(FWObject *obj, bool ask)
{
    if (Host::isA(obj) || Firewall::isA(obj) || Cluster::isA(obj))
    {
        QString dialog_txt = tr(
"The name of the object '%1' has changed. The program can also "
"rename IP address objects that belong to this object, "
"using standard naming scheme 'host_name:interface_name:ip'. "
"This makes it easier to distinguish what host or a firewall "
"given IP address object belongs to when it is used in "
"the policy or NAT rule. The program also renames MAC address "
"objects using scheme 'host_name:interface_name:mac'. "
"Do you want to rename child IP and MAC address objects now? "
"(If you click 'No', names of all address objects that belong to "
"%2 will stay the same.)")
            .arg(QString::fromUtf8(obj->getName().c_str()))
            .arg(QString::fromUtf8(obj->getName().c_str()));

        if (!ask || QMessageBox::warning(
                this,"Firewall Builder", dialog_txt,
                tr("&Yes"), tr("&No"), QString::null,
                0, 1 )==0 )
        {
            list<FWObject*> il = obj->getByType(Interface::TYPENAME);
            for (list<FWObject*>::iterator i=il.begin(); i!=il.end(); ++i)
                autorename(*i, false);
        }
    }
 
    if (Interface::isA(obj))
    {
        list<FWObject*> subinterfaces = obj->getByType(Interface::TYPENAME);
        if (obj->getByType(IPv4::TYPENAME).size() ||
            obj->getByType(IPv6::TYPENAME).size() ||
            obj->getByType(physAddress::TYPENAME).size() ||
            subinterfaces.size())
        {
            QString dialog_txt = tr(
                "The name of the interface '%1' has changed. The program can also "
                "rename IP address objects that belong to this interface, "
                "using standard naming scheme 'host_name:interface_name:ip'. "
                "This makes it easier to distinguish what host or a firewall "
                "given IP address object belongs to when it is used in "
                "the policy or NAT rule. The program also renames MAC address "
                "objects using scheme 'host_name:interface_name:mac'. "
                "Do you want to rename child IP and MAC address objects now? "
                "(If you click 'No', names of all address objects that belong to "
                "interface '%2' will stay the same.)")
                .arg(QString::fromUtf8(obj->getName().c_str()))
                .arg(QString::fromUtf8(obj->getName().c_str()));

            if (!ask || QMessageBox::warning(
                    this, "Firewall Builder", dialog_txt,
                    tr("&Yes"), tr("&No"), QString::null,
                    0, 1 )==0 )
            {
                list<FWObject*> vlans;
                for (list<FWObject*>::iterator j=subinterfaces.begin();
                     j!=subinterfaces.end(); ++j)
                {
                    Interface *intf = Interface::cast(*j);
                    if (intf->getOptionsObject()->getStr("type") == "8021q")
                        vlans.push_back(intf);
                }

                if (vlans.size()) autorenameVlans(vlans);

                for (list<FWObject*>::iterator j=subinterfaces.begin();
                     j!=subinterfaces.end(); ++j)
                    autorename(*j, false);

                list<FWObject*> obj_list = obj->getByType(IPv4::TYPENAME);
                autorename(obj_list, IPv4::TYPENAME, "ip");
                obj_list = obj->getByType(IPv6::TYPENAME);
                autorename(obj_list, IPv6::TYPENAME, "ip6");
                obj_list = obj->getByType(physAddress::TYPENAME);
                autorename(obj_list, physAddress::TYPENAME, "mac");

            }

        }
    }
}

void ObjectManipulator::autorename(list<FWObject*> &obj_list,
                                   const string &objtype,
                                   const string &namesuffix)
{
    for (list<FWObject*>::iterator j=obj_list.begin(); j!=obj_list.end(); ++j)
    {
        FWObject *obj = *j;
        FWObject *parent = obj->getParent();
        QString name = getStandardName(parent, objtype, namesuffix);
        name = makeNameUnique(parent, name, objtype.c_str());
        obj->setName(string(name.toUtf8()));
        //QTreeWidgetItem *itm1 = allItems[obj];
        //if (itm1!=NULL) updateObjectInTree(obj);
    }
}

void ObjectManipulator::autorenameVlans(list<FWObject*> &obj_list)
{
    for (list<FWObject*>::iterator j=obj_list.begin(); j!=obj_list.end(); ++j)
    {
        FWObject *obj = *j;
        FWObject *parent = obj->getParent();
        FWObject *fw = parent;
        while (fw && Firewall::cast(fw)==NULL) fw = fw->getParent();
        assert(fw);
        QString obj_name = obj->getName().c_str();

        Resources* os_res = Resources::os_res[fw->getStr("host_OS")];
        string os_family = fw->getStr("host_OS");
        if (os_res!=NULL)
            os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

        std::auto_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                os_family));
        if (int_prop->looksLikeVlanInterface(obj_name))
        {
            // even though we only call this function if the type of
            // this interface is 8021q, need to check its naming
            // schema as well. We can't automatically rename
            // interfaces that do not follow known naming convention.

            QString base_name;
            int vlan_id;
            int_prop->parseVlan(obj_name, &base_name, &vlan_id);
            if (base_name != "vlan")
            {
                QString new_name("%1.%2");
                obj->setName(new_name.arg(parent->getName().c_str()).arg(vlan_id).toStdString());
                //QTreeWidgetItem *itm1 = allItems[obj];
                //if (itm1!=NULL) updateObjectInTree(obj);
            }
        }
    }
}

QString ObjectManipulator::getStandardName(FWObject *parent,
                                           const string&,
                                           const string &namesuffix)
{
    QStringList names;
    FWObject *po = parent;
    while (po!=NULL)
    {
        names.push_front(QString::fromUtf8(po->getName().c_str()));
        if (Host::cast(po)) break;
        po = po->getParent();
    }
//    names.push_back(QString::fromUtf8(parent->getName().c_str()));
    names.push_back(namesuffix.c_str());
    return names.join(":");
}

void ObjectManipulator::clearObjects()
{
    if (fwbdebug) qDebug("ObjectManipulator::clearObjects %p start",this);

    invalidateDialog();
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

        addTreePage( lib );

        if (fwbdebug) qDebug("ObjectManipulator::loadObjects %p added lib %s",
                             this, lib->getName().c_str());
    }

    if (firstUserLib==NULL) firstUserLib=ll.front();
    openLib( firstUserLib );
    if (fwbdebug) qDebug("ObjectManipulator::loadObjects %p done", this);
}

void ObjectManipulator::addLib( FWObject *lib, QTreeWidget* otv)
{
    QString newlibname = QString::fromUtf8(lib->getName().c_str());
    int              N = m_objectManipulator->libs->count();
    int            idx = 0;
    vector<FWObject*>::iterator  i1=idxToLibs.begin();
    vector<QTreeWidget*>::iterator i2=idxToTrees.begin();
    for ( ; idx<N; ++idx,++i1,++i2)
        if ( m_objectManipulator->libs->itemText(idx) > newlibname ) break;

    string icn=":/Icons/"+lib->getTypeName()+"/icon-tree";
            //Resources::global_res->getObjResourceStr(lib,"icon-tree").c_str();
    QPixmap pm;
    if ( ! QPixmapCache::find( icn.c_str(), pm) )
    {
        pm.load( icn.c_str() );
        QPixmapCache::insert( icn.c_str(), pm);
    }
    m_objectManipulator->libs->insertItem( idx, pm, newlibname);
//    idx=libs->count()-1;

    m_objectManipulator->libs->setCurrentIndex(idx);

    idxToLibs.insert(i1,lib);
    if (otv!=NULL) idxToTrees.insert(i2,otv);

}

void ObjectManipulator::addTreePage( FWObject *lib)
{
    if (fwbdebug) qDebug("Object Manipulator::addTreePage %p lib: %s",
                         this, lib->getName().c_str());

    ObjectTreeView *objTreeView = new ObjectTreeView(
        m_project, m_objectManipulator->widgetStack, OBJTREEVIEW_WIDGET_NAME );

    addLib(lib, objTreeView);

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
             this, SLOT( deleteObj() ) );

    connect(objTreeView, SIGNAL( objectDropped_sign(libfwbuilder::FWObject*) ),
             this, SLOT( openObject(libfwbuilder::FWObject*) ) );

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

    itm1->setText( 0, lib->getName().c_str() );
    itm1->setText( 1, getTreeLabel(lib) );

    QPixmap pm;
    FWBTree().setObjectIcon(lib, &pm);
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

void ObjectManipulator::currentTreePageChanged(int i)
{
    QWidget *w = m_objectManipulator->widgetStack->widget(i);
    switchingTrees(w);
}

void ObjectManipulator::switchingTrees(QWidget* w)
{
    ObjectTreeView *new_otv = dynamic_cast<ObjectTreeView*>(w);

    if (!new_otv)
        return;//assert(new_otv)

    if (current_tree_view!=NULL) current_tree_view->becomingHidden();
    new_otv->becomingVisible();
    current_tree_view = new_otv;

}

/*
 * Make the name of the object @obj unique across all children of the
 * given @target object. If this object is an interface, use pattern
 * <ifname><ifnumber> and increment the number until the name becomes
 * unique. For all other types use pattern <basename>-<number>
 *
 * This method has ugly side-effect: if @obj is an Interface, this
 * method needs to check its type. To do that, it calls
 * Interface::getOptionsObject() which creates options object if it
 * does not exits. To do initial options configuration, it needs
 * access to the parent. We call Interface::getOptionsObject() in copt
 * and startDrag methods to make sure interfaces have options objects
 * before copy or drag operation starts to avoid this problem here.
 *
 * In case of copy/paste or d&d of an interface, the naming
 * conventions are dictated by the platform of the new parent firewall
 * rather than the old one, which in this case is either <target> or
 * its parent. So we'll use @target to get proper interfaceProperties
 * object which will do checks for us.
 */
void ObjectManipulator::makeNameUnique(FWObject *target, FWObject *obj)
{
    Interface *intf = Interface::cast(obj);
    if (intf)
    {
        // check if this is vlan subinterface. We should not change
        // names of those
        if (intf->getOptionsObject()->getStr("type") == "8021q")
            return;
        // one of the typical usage patterns is to create vlan
        // interface "eth0.101" and then immediately try to copy/paste
        // it to under br0 to make it bridge port. In this case
        // interface eth0.101 won't have type "8021q" just yet because
        // the user did not open interface "advanced" settings dialog
        // to set its type and VLAN ID. Users assume that if its name
        // is "eth0.101", then it must be vlan interface. We should
        // follow this assumption too. Also, check for names "vlanNNN"
        // as well.
        //
        QString obj_name = obj->getName().c_str();
        FWObject *fw = target;
        while (fw && !Firewall::cast(fw)) fw = fw->getParent();

        Resources* os_res = Resources::os_res[fw->getStr("host_OS")];
        string os_family = fw->getStr("host_OS");
        if (os_res!=NULL)
            os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

        std::auto_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                os_family));
        if (int_prop->looksLikeVlanInterface(obj_name)) return;
    }
    QString newname = makeNameUnique(target,
                                     QString::fromUtf8(obj->getName().c_str()),
                                     obj->getTypeName().c_str());
    obj->setName(string(newname.toUtf8()));
}

QString ObjectManipulator::makeNameUnique(FWObject* parent,
                                          const QString &obj_name,
                                          const QString &obj_type)
{
    int suffix = 1;
    QString basename = obj_name;
    QString newname = basename;

    if (fwbdebug)
        qDebug("ObjectManipulator::makeNameUnique parent=%s obj_name=%s",
               parent->getName().c_str(),
               obj_name.toStdString().c_str());

    if (obj_type == Interface::TYPENAME)
    {
        QRegExp rx("([a-zA-Z-]+)(\\d{1,})");
        if (rx.indexIn(obj_name) != -1)
        {
            basename = rx.cap(1);
            suffix = rx.cap(2).toInt();
        }
    }

/*
 * Check if there is another object with the same name. Note that
 * FWObject::findObjectByName() searches in depth, but we only need to
 * scan child objects of the first level.
 */
    while (true)
    {
        if (fwbdebug)
            qDebug("ObjectManipulator::makeNameUnique newname=%s basename=%s suffix=%d",
                   newname.toStdString().c_str(),
                   basename.toStdString().c_str(),
                   suffix);

        FWObject::const_iterator i = find_if(
            parent->begin(), parent->end(),
            FWObjectNameEQPredicate(newname.toStdString()));
        if (i==parent->end()) break;
        if (obj_type == Interface::TYPENAME)
            newname = QString("%1%2").arg(basename).arg(suffix);
        else
            newname = QString("%1-%2").arg(basename).arg(suffix);
        suffix++;
    }
    return newname;
}

void ObjectManipulator::contextMenuRequested(const QPoint &pos)
{
/* in extended selection mode there may be several selected items */

    QTreeWidget *objTreeView = getCurrentObjectTree();
    QTreeWidgetItem *item = objTreeView->itemAt(pos);//clicked item

    if (fwbdebug)
        qDebug("ObjectManipulator::contextMenu  selectedObjects.size=%d",
               getCurrentObjectTree()->getNumSelected());

    ObjectTreeViewItem *otvi=dynamic_cast<ObjectTreeViewItem*>(item);
    if (otvi==NULL)  return;  // happens when user clicks outside an item

    if (!getCurrentObjectTree()->isSelected(otvi->getFWObject()))
        openObject( otvi, true );

    if (currentObj==NULL)  currentObj=otvi->getFWObject();

    QMenu *popup = new QMenu(this);

    QAction *edtID = popup->addAction(
        tr("Edit"), this, SLOT( editSelectedObject()));

    QMenu *duptargets  = NULL;
    QAction *dupID = NULL;
    QMenu *movetargets = NULL;
    int moveTargetsCounter = 0;

    if (!Interface::isA(currentObj) && RuleSet::cast(currentObj)==NULL &&
        !Library::isA(currentObj) && !FWBTree().isStandardFolder(currentObj))
    {
        duptargets = popup->addMenu( tr("Duplicate ...") );
        movetargets = popup->addMenu( tr("Move ...") );

        connect ( duptargets, SIGNAL ( triggered(QAction*) ),
                  this, SLOT( duplicateObj(QAction*) ) );
        connect ( movetargets, SIGNAL ( triggered(QAction*) ),
                  this, SLOT( moveObj(QAction*) ) );

/* we add " ... to library ..." submenu to the "Move " menu item only
 * if user did not select a library, or if they selected several
 * objects. Method moveObj knows that library should not be moved
 * into another library.
 */
        bool libSelected =
            (getCurrentObjectTree()->getNumSelected()==1 &&
             Library::isA(getCurrentObjectTree()->getSelectedObjects().front()));

        int libid = 0;

        FWObject *cl = getCurrentLib();
        vector<FWObject*>::iterator i;

        for (i=idxToLibs.begin(); i!=idxToLibs.end(); ++i,++libid)
        {
            FWObject *lib   = *i;

            if ( lib->getId()==FWObjectDatabase::STANDARD_LIB_ID ||
                 lib->getId()==FWObjectDatabase::TEMPLATE_LIB_ID ||
                 lib->getId()==FWObjectDatabase::DELETED_OBJECTS_ID  ||
                 lib->isReadOnly())
                continue;

            dupID = duptargets->addAction(
                tr("place in library %1").arg(
                    QString::fromUtf8(lib->getName().c_str())));
            dupID->setData(libid);

            /* can't move to the same library or if selected object is
             * a library
             */
            if (!libSelected && lib!=cl)
            {
                moveTargetsCounter++;
                QAction* mact = movetargets->addAction(
                    tr("to library %1").arg(
                        QString::fromUtf8(lib->getName().c_str())));
                mact->setData(libid);
            }
        }
    }

    popup->addSeparator();

    QAction *copyID = popup->addAction(tr("Copy"), this, SLOT(copyObj()));
    QAction *cutID = popup->addAction(tr("Cut"), this, SLOT(cutObj()));
    QAction *pasteID = popup->addAction(tr("Paste"), this, SLOT(pasteObj()));

    popup->addSeparator();

    QAction * delID = popup->addAction( tr("Delete"), this,
                                        SLOT( deleteObj() ) );

    QList<QAction*> AddObjectActions;

    if (getCurrentObjectTree()->getNumSelected()==1)
    {
        popup->addSeparator();

        if ( (Firewall::isA(currentObj) || Host::isA(currentObj)) &&
             ! currentObj->isReadOnly() )
        {
            AddObjectActions.append(popup->addAction( tr("Add Interface"), this,
                                                      SLOT( newInterface() ) ));

        }
        if ((Firewall::isA(currentObj) || Cluster::isA(currentObj)) &&
             ! currentObj->isReadOnly())
        {
            AddObjectActions.append(popup->addAction( tr("Add Policy Rule Set"), this,
                                                      SLOT( newPolicyRuleSet() ) ));
            AddObjectActions.append(popup->addAction( tr("Add NAT Rule Set"), this,
                                                      SLOT( newNATRuleSet() ) ));
        }

        if (Interface::isA(currentObj) && ! currentObj->isReadOnly())
        {
            Interface *iface = Interface::cast(currentObj);
            FWObject *h = iface->getParentHost();

            bool supports_advanced_ifaces = false;
            try {
                /*
                 * ignore raised exception; this just means that the host_OS
                 * option is undefined for this target (e.g. for a host).
                 */
                supports_advanced_ifaces = Resources::getTargetCapabilityBool
                        (h->getStr("host_OS"), "supports_subinterfaces");
            } catch (FWException &ex) { }

            /* 
             * check if this interface can have subinterfaces. Show "Add Interface"
             * menu item only if host_os has attribute "supports_subinterfaces"
             * and if parent interface (currentObj) has the type that can have
             * subinterfaces. Also, cluster interfaces can't have subinterfaces
             * and only one level of subinterfaces is allowed.
             */
            if (supports_advanced_ifaces && Firewall::isA(currentObj->getParent()))
            {
                list<QStringPair> subint_types;
                getSubInterfaceTypes(iface, subint_types);
                if (subint_types.size())
                    popup->addAction(
                        tr("Add Interface"), this, SLOT( newInterface() ) );
            }

            AddObjectActions.append(popup->addAction( tr("Add IP Address"), this,
                                                      SLOT( newInterfaceAddress() ) ));
            AddObjectActions.append(popup->addAction( tr("Add IPv6 Address"), this,
                                                      SLOT( newInterfaceAddressIPv6() ) ));
            AddObjectActions.append(popup->addAction( tr("Add MAC Address"), this,
                                                      SLOT( newPhysicalAddress() ) ));
            // Check if we should add menu item that creates failover
            // group. if parent is a cluster, allow one vrrp type
            // FailoverClusterGroup per Interface only
            FWObject *parent = NULL;
            parent = currentObj->getParent();
            if (parent != NULL && Cluster::isA(parent))
            {
                QAction *failover_menu_id = popup->addAction(
                    tr("Add Failover Group"), this,
                    SLOT( newFailoverClusterGroup() ) );
                failover_menu_id->setEnabled(
                    currentObj->getFirstByType(FailoverClusterGroup::TYPENAME) == NULL);
            }
        }

        if (Cluster::isA(currentObj) && ! currentObj->isReadOnly())
        {
            AddObjectActions.append(popup->addAction( tr("Add Cluster interface"), this,
                                                      SLOT( newClusterIface() ) ));
            // allow multiple state syncing groups per cluster
            // Rationale: these groups may represent different state syncing
            // protocols that can synchronize different things.
            AddObjectActions.append(popup->addAction( tr("Add State Synchronization Group"), this,
                                                      SLOT( newStateSyncClusterGroup() ) ));
        }

        if (currentObj->getPath(true)=="Firewalls")
            AddObjectActions.append(popup->addAction( tr("New Firewall"), this,
                                                      SLOT( newFirewall() ) ));

        if (currentObj->getPath(true)=="Clusters")
            AddObjectActions.append(popup->addAction( tr("New Cluster"), this,
                                                      SLOT( newCluster() ) ));

        if (currentObj->getPath(true)=="Objects/Addresses")
        {
            AddObjectActions.append(popup->addAction( tr("New Address"), this,
                                                      SLOT( newAddress() ) ));
            AddObjectActions.append(popup->addAction( tr("New Address IPv6"), this,
                                                      SLOT( newAddressIPv6() ) ));
        }

        if (currentObj->getPath(true)=="Objects/DNS Names")
        {
            AddObjectActions.append(popup->addAction( tr("New DNS Name"), this,
                                                      SLOT( newDNSName() ) ));
        }

        if (currentObj->getPath(true)=="Objects/Address Tables")
        {
            AddObjectActions.append(popup->addAction( tr("New Address Table"), this,
                                                      SLOT( newAddressTable() ) ));
        }

        if (currentObj->getPath(true)=="Objects/Address Ranges")
            AddObjectActions.append(popup->addAction( tr("New Address Range"), this,
                                                      SLOT( newAddressRange() ) ));

        if (currentObj->getPath(true)=="Objects/Hosts")
            AddObjectActions.append(popup->addAction( tr("New Host"), this,
                                                      SLOT( newHost() ) ));

        if (currentObj->getPath(true)=="Objects/Networks")
        {
            AddObjectActions.append(popup->addAction( tr("New Network"), this,
                                                      SLOT( newNetwork() ) ));
            AddObjectActions.append(popup->addAction( tr("New Network IPv6"), this,
                                                      SLOT( newNetworkIPv6() ) ));
        }

        if (currentObj->getPath(true)=="Objects/Groups")
            AddObjectActions.append(popup->addAction( tr("New Group"), this,
                                                      SLOT( newObjectGroup() ) ));

        if (currentObj->getPath(true)=="Services/Custom")
            AddObjectActions.append(popup->addAction( tr("New Custom Service"),this,
                                                      SLOT( newCustom() ) ));

        if (currentObj->getPath(true)=="Services/IP")
            AddObjectActions.append(popup->addAction( tr("New IP Service"), this,
                                                      SLOT( newIP() ) ));

        if (currentObj->getPath(true)=="Services/ICMP")
        {
            AddObjectActions.append(popup->addAction( tr("New ICMP Service"), this,
                                                      SLOT( newICMP() ) ));
            AddObjectActions.append(popup->addAction( tr("New ICMP6 Service"), this,
                                                      SLOT( newICMP6() ) ));
        }

        if (currentObj->getPath(true)=="Services/TCP")
            AddObjectActions.append(popup->addAction( tr("New TCP Service"), this,
                                                      SLOT( newTCP() ) ));

        if (currentObj->getPath(true)=="Services/UDP")
            AddObjectActions.append(popup->addAction( tr("New UDP Service"), this,
                                                      SLOT( newUDP() ) ));

        if (currentObj->getPath(true)=="Services/TagServices")
            AddObjectActions.append(popup->addAction( tr("New TagService"), this,
                                                      SLOT( newTagService() ) ));

        if (currentObj->getPath(true)=="Services/Groups")
            AddObjectActions.append(popup->addAction( tr("New Group"), this,
                                                      SLOT( newServiceGroup() ) ));

        if (currentObj->getPath(true)=="Services/Users")
            AddObjectActions.append(popup->addAction(tr("New User Service"), this,
                                                     SLOT(newUserService() )));

        if (currentObj->getPath(true)=="Time")
            AddObjectActions.append(popup->addAction( tr("New Time Interval"), this,
                                                      SLOT( newInterval() ) ));

        popup->addSeparator();
        popup->addAction( tr("Find"), this, SLOT( findObject()));

        popup->addAction( tr("Where used"), this, SLOT( findWhereUsedSlot()));
    } else
    {
        popup->addAction( tr("Group"), this, SLOT( groupObjects() ) );
    }

    if (Firewall::cast(currentObj)!=NULL ||
        (ObjectGroup::cast(currentObj)!=NULL &&
         currentObj->getName()=="Firewalls"))
    {
        popup->addSeparator();
        popup->addAction( tr("Compile"), this, SLOT( compile()));
        popup->addAction( tr("Install"), this, SLOT( install()));

        if (Firewall::cast(currentObj)!=NULL)
        {
            Resources* os_res = Resources::os_res[currentObj->getStr("host_OS")];
            if (os_res)
            {
                string transfer = os_res->getTransferAgent();
                if (!transfer.empty())
                    popup->addAction( tr("Transfer"), this, SLOT(transferfw()));
            }
        }

        if (ObjectGroup::cast(currentObj)!=NULL && currentObj->getName()=="Firewalls")
        {
            // Config transfer is currently only supported for Secuwall.
            // Check if we have any
            bool have_transfer_support = false;
            for (FWObject::iterator it=currentObj->begin();
                 it!=currentObj->end(); ++it)
            {
                FWObject *fw = *it;
                Resources* os_res = Resources::os_res[fw->getStr("host_OS")];
                if (os_res)
                {
                    string transfer = os_res->getTransferAgent();
                    have_transfer_support = have_transfer_support || (!transfer.empty());
                }
            }
            if (have_transfer_support)
                popup->addAction( tr("Transfer"), this, SLOT(transferfw()));
        }
//        popup->addSeparator();
//        popup->addAction( tr("Simulate install"), this, SLOT( simulateInstall()));
    }

    popup->addSeparator();
    QAction* lcID=popup->addAction( tr("Lock"), this,
                       SLOT( lockObject() ) );
    QAction* unlcID=popup->addAction( tr("Unlock"), this,
                       SLOT( unlockObject() ) );
    lcID->setEnabled(getCurrentObjectTree()->isLockable());
    unlcID->setEnabled(getCurrentObjectTree()->isUnlockable());

    if (fwbdebug)
    {
/* keep this for debugging  */
        popup->addSeparator();
        popup->addAction( tr("dump"), this, SLOT( dumpObj()));
    }

    if (getCurrentObjectTree()->getNumSelected()==1)
    {
        edtID->setEnabled( !FWBTree().isStandardFolder(currentObj));
    } else
        edtID->setEnabled(false);

    bool dupMenuItem=true;
    bool moveMenuItem=true;
    bool copyMenuItem=true;
    bool pasteMenuItem=true;
    bool delMenuItem=true;
    bool newMenuItem=true;
    bool inDeletedObjects = false;

    getMenuState( (moveTargetsCounter>0),
                  dupMenuItem, moveMenuItem, copyMenuItem, pasteMenuItem,
                  delMenuItem, newMenuItem, inDeletedObjects);

    if (dupID) dupID->setEnabled(dupMenuItem);
//    movetargets->setEnabled(moveMenuItem);
    copyID->setEnabled(copyMenuItem);
    pasteID->setEnabled(pasteMenuItem);

    cutID->setEnabled(delMenuItem);
    delID->setEnabled(delMenuItem);

    QList<QAction*>::iterator iter;
    for (iter=AddObjectActions.begin(); iter!=AddObjectActions.end(); iter++)
        (*iter)->setEnabled(newMenuItem);

//    if (inDeletedObjects) movID->setText( tr("Undelete...") );

    popup->exec( objTreeView->mapToGlobal( pos ) );
}

void ObjectManipulator::getMenuState(bool haveMoveTargets,
                                     bool &dupMenuItem,
                                     bool &moveMenuItem,
                                     bool &copyMenuItem,
                                     bool &pasteMenuItem,
                                     bool &delMenuItem,
                                     bool &newMenuItem,
                                     bool &inDeletedObjects)
{
    if (fwbdebug) qDebug("ObjectManipulator::getMenuState");

    dupMenuItem = true;
    moveMenuItem = true;
    copyMenuItem = true;
    pasteMenuItem = true;
    delMenuItem = true;
    newMenuItem = true;

    inDeletedObjects = false;

    FWObject *del_obj_library =
        m_project->db()->findInIndex( FWObjectDatabase::DELETED_OBJECTS_ID);

    FWObject *current_library = getCurrentLib();

    vector<FWObject*> so = getCurrentObjectTree()->getSelectedObjects();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        FWObject *obj= *i;

        QString object_path = obj->getPath(true).c_str();

        copyMenuItem = copyMenuItem && m_project->getCopyMenuState(object_path);
        pasteMenuItem = pasteMenuItem &&
            m_project->getPasteMenuState(object_path) &&
            (FWObjectClipboard::obj_clipboard->size()!=0);
        delMenuItem = delMenuItem && m_project->getDeleteMenuState(obj);
        delMenuItem = delMenuItem && 
            current_library->getId() != FWObjectDatabase::STANDARD_LIB_ID &&
            current_library->getId() != FWObjectDatabase::TEMPLATE_LIB_ID;

#if DISABLE_PASTE_MENU_ITEM_IF_PASTE_IS_ILLEGAL
        if (pasteMenuItem)
        {
            /*
             * We used to enable Paste menu item only if object can be
             * pasted. The problem with this is that there was no
             * indication why Paste operation was not allowed. Since
             * we call validateForPaste during actual Paste operation
             * anyway, so is more user friendly to let them try and
             * actually see the error if it fails.
             */
            vector<std::pair<int,ProjectPanel*> >::iterator i;
            for (i= FWObjectClipboard::obj_clipboard->begin();
                 i!=FWObjectClipboard::obj_clipboard->end(); ++i)
            {
                FWObject *co= m_project->db()->findInIndex(i->first);
                if (co==NULL)
                {
                    continue ;
                    //QString s2 = obj->getTypeName().c_str();
                }
                QString s3 = obj->getTypeName().c_str();
                QString err;
                bool validated = validateForPaste(obj, co, err);
                pasteMenuItem = pasteMenuItem && validated;
            }
        }
#endif

        dupMenuItem=
            (dupMenuItem && ! FWBTree().isStandardFolder(obj) && ! Library::isA(obj) );

        inDeletedObjects = (del_obj_library!=NULL && obj->isChildOf(del_obj_library));
        dupMenuItem = dupMenuItem && !inDeletedObjects;

// can't move system objects or libraries
        moveMenuItem = moveMenuItem && ! FWBTree().isStandardFolder(obj) && ! Library::isA(obj);

// can't move interfaces unless parent host object is also selected
        if ( Interface::isA(obj) &&
             std::find(so.begin(),so.end(),obj->getParent())==so.end())
            moveMenuItem = false;

// can't move ip addresses if parent is interface
        if (IPv4::isA(obj) && IPv6::isA(obj) && Interface::isA(obj->getParent()))
            moveMenuItem = false;

// can't move physAddress objects
        moveMenuItem = moveMenuItem && ! physAddress::isA(obj);

// can't move read-only objects
        moveMenuItem = moveMenuItem && ! obj->isReadOnly();

// can't move libraries unless in deleted objects
        if (Library::isA(obj) && ! inDeletedObjects) moveMenuItem = false;

// can't move if there is only one user-defined library in the tree
// but we dont care about number of libraries if this will become
// 'undelete' operation
        if (!haveMoveTargets && ! inDeletedObjects) moveMenuItem = false;

//        copyMenuItem= (copyMenuItem &&
//                       ! FWBTree().isSystem(currentObj) &&
//                       ! Library::isA(currentObj));
//        delMenuItem= (delMenuItem && ! FWBTree().isSystem(currentObj));

        newMenuItem= (newMenuItem && ! obj->isReadOnly() );
        Interface *intf = Interface::cast(obj);
        if (intf &&
            (intf->isDyn() ||
             intf->isUnnumbered() ||
             intf->isBridgePort())
        )
            newMenuItem = false;

    }
    if (fwbdebug) qDebug("ObjectManipulator::getMenuState done");
}

void ObjectManipulator::find()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;
    m_project->setFDObject(obj);
}
void ObjectManipulator::findObject()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;
    mw->findObject( obj );
}

void ObjectManipulator::dumpObj()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;
    obj->dump(true,false);
}

void ObjectManipulator::compile()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();

    set<Firewall*> fo;
    filterFirewallsFromSelection(so, fo);

    if (fwbdebug)
        qDebug("ObjectManipulator::compile filtered %d firewalls",
               int(fo.size()));

    m_project->compile(fo);
}

void ObjectManipulator::filterFirewallsFromSelection(vector<FWObject*> &so,
                                                     set<Firewall*> &fo)
{
    Firewall *fw;
    ObjectGroup *gr;
    Cluster *cl;
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        cl = Cluster::cast(*i);
        if (cl != NULL)
        {
            list<Firewall*> members;
            cl->getMembersList(members);
            // display warning if no firewalls could be extracted for a cluster
            if (members.size() == 0)
            {
                QMessageBox::warning(this, "Firewall Builder",
                        QObject::tr("No firewalls assigned to cluster '%1'").
                                     arg(cl->getName().c_str()),
                        "&Continue", QString::null, QString::null, 0, 1 );
                continue;
            }
            fo.insert(cl);
            continue;
        }
        fw = Firewall::cast(*i);
        if (fw!=NULL)
        {
            fo.insert(fw);
            continue;
        }
        gr = ObjectGroup::cast(*i);
        if (gr!=NULL)
        {
            extractFirewallsFromGroup(gr,fo);
        }
    }
}

void ObjectManipulator::extractFirewallsFromGroup(ObjectGroup *gr,
                                                  set<Firewall*> &fo)
{
   set<FWObject*> oset;
   m_project->db()->findObjectsInGroup(gr, oset);

   set<FWObject*>::iterator i;
   for(i=oset.begin();i!=oset.end();++i)
       if (Firewall::cast(*i)) fo.insert(Firewall::cast(*i));
}

void ObjectManipulator::install()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    set<Firewall*> fo;
    filterFirewallsFromSelection(so,fo);

    m_project->install(fo);
}

void ObjectManipulator::transferfw()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    set<Firewall*> fo;
    filterFirewallsFromSelection(so, fo);

    m_project->transferfw(fo);
}

FWObject* ObjectManipulator::duplicateObject(FWObject *targetLib,
                                             FWObject *obj,
                                             const QString &name,
                                             bool  askForAutorename)
{
    if (!isTreeReadWrite(this, targetLib)) return NULL;

    openLib(targetLib);

    FWObject *o=NULL;

    QString newName;
    if (!name.isEmpty()) newName = name;
    else                 newName = QString::fromUtf8(obj->getName().c_str());

    o = createObject(obj->getTypeName().c_str(), newName, obj);
    if (o)
    {
      openObject(o);
      if (!o->isReadOnly() &&
          (Host::isA(o) || Firewall::isA(o) || Cluster::isA(o) ||
           Interface::isA(o)) )
        autorename(o, askForAutorename);
      if (Firewall::isA(o))
      {
          // reset lastModified, lastCompiled, lastInstalled
          o->setInt("lastCompiled", 0);
          o->setInt("lastModified", 0);
          o->setInt("lastInstalled", 0);
          // switch policy 
          m_project->openRuleSet(o->getFirstByType(Policy::TYPENAME));
      }
    }
    return o;
}

void ObjectManipulator::duplicateObj(QAction *action)
{
    int libid = action->data().toInt();
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    ObjectTreeView* ot=getCurrentObjectTree();
    ot->freezeSelection(true);
    FWObject *obj;
    FWObject *nobj = NULL;
    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        obj= *i;
        if ( FWBTree().isSystem(obj) || Interface::isA(obj) ) continue;
        FWObject *cl = idxToLibs[libid];
        nobj = duplicateObject(cl, obj, "", false);
    }
    editObject(nobj);
    ot->freezeSelection(false);
}

void ObjectManipulator::moveObject(FWObject *targetLib, FWObject *obj)
{
    FWObject *cl=getCurrentLib();
    if (cl==targetLib) return;

    FWObject *grp = NULL;

    if (FWObjectDatabase::isA(targetLib)) grp = targetLib;
    else
    {
        grp = m_project->getFWTree()->getStandardSlotForObject(
            targetLib, obj->getTypeName().c_str());
    }

    if (fwbdebug)
        qDebug("ObjectManipulator::moveObject  grp= %p", grp);

    if (grp==NULL) grp=targetLib;

    if (fwbdebug)
        qDebug("ObjectManipulator::moveObject  grp= %s",
               grp->getName().c_str());

    if (!grp->isReadOnly())
    {
        obj->ref();
        obj->ref();

        if (fwbdebug)
            qDebug("ObjectManipulator::moveObject  remove from the widget");

        ObjectTreeViewItem *itm = allItems[obj];
        if (itm->parent()==NULL) return;

        itm->parent()->takeChild(itm->parent()->indexOfChild(itm));

        if (fwbdebug)
            qDebug("ObjectManipulator::moveObject  removing from the tree");

        obj->getParent()->remove(obj);

        if (fwbdebug)
            qDebug("ObjectManipulator::moveObject  adding to the tree");

        grp->add(obj);
        obj->unref();

        if (fwbdebug)
            qDebug("ObjectManipulator::moveObject  adding to the widget");

        if (allItems[grp]==NULL)
        {
            /* adding to the root, there is not such tree item */
            if (Library::isA(obj))
            {
                addTreePage(obj);
                openLib(obj);
            } else
            {
                /* it screwed up, just print debugging message */
                if (fwbdebug)
                    qDebug("ObjectManipulator::moveObject  no place in "
                           "the tree corresponding to the object %p %s",
                           grp, grp->getName().c_str());
            }
        } else
            allItems[grp]->addChild(itm);
    }
    if (fwbdebug)
        qDebug("ObjectManipulator::moveObject  all done");
}

/*
 *  targetLibName is the name of the target library in Unicode
 */
void ObjectManipulator::moveObject(const QString &targetLibName,
                                   FWObject *obj)
{
    list<FWObject*> ll = m_project->db()->getByType( Library::TYPENAME );
    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        FWObject *lib=*i;
        if (targetLibName==QString::fromUtf8(lib->getName().c_str()))
        {
            if (fwbdebug)
                qDebug("ObjectManipulator::moveObject  found lib %s",
                       lib->getName().c_str() );

            moveObject(lib,obj);
        }
    }
}

/* 
 * moveObj is a slot called from the context menu
 */
void ObjectManipulator::moveObj(QAction* action)
{
    int libid = action->data().toInt();

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    ObjectTreeView* ot=getCurrentObjectTree();
    ot->freezeSelection(true);
    FWObject *obj;

    FWObject *targetLib   = idxToLibs[libid];

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        obj= *i;

        if (fwbdebug)
        {
            qDebug("ObjectManipulator::moveObj  obj=%p  obj: %s",
                   obj, obj->getName().c_str() );
        }
        if (Library::isA(obj))
        {
/* We can only move library to the root of the tree. This case only
 * happens when user tries to undelete a library.
 */
            moveObject(m_project->db(),obj);
        } else
        {
            if (obj->isChildOf(targetLib)) continue;

            if ( FWBTree().isSystem(obj) ||
                 Interface::isA(obj)    ||
                 Interface::isA(obj->getParent())) continue;

            moveObject(targetLib, obj);
        }

        QCoreApplication::postEvent(
            mw, new dataModifiedEvent(m_project->getFileName(), obj->getId()));
    }
    ot->freezeSelection(false);

}

void ObjectManipulator::copyObj()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;
    FWObject *obj;
    FWObjectClipboard::obj_clipboard->clear();

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();

    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        obj = *i;
        if ( ! FWBTree().isSystem(obj) )
        {
            // while obj is still part of the tree, do some clean up
            // to avoid problems in the future.  Create
            // InterfaceOptions objects for interfaces because we'll
            // need them for various validations during paste
            // operation.
            Interface *intf = Interface::cast(obj);
            if (intf) intf->getOptionsObject();
            FWObjectClipboard::obj_clipboard->add(obj, m_project);
        }
    }
}

void ObjectManipulator::cutObj()
{
    copyObj();
    deleteObj();   // works with the list getCurrentObjectTree()->getSelectedObjects()
}

void ObjectManipulator::pasteObj()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;
    FWObject *target_object = getCurrentObjectTree()->getSelectedObjects().front();
    if (target_object==NULL) return;

    vector<std::pair<int,ProjectPanel*> >::iterator i;
    int idx = 0;
    FWObject *last_object = NULL;
    bool need_to_reload = false;
    map<int,int> map_ids;
    if (fwbdebug)
    {
        qDebug() << "**************** pasteObj loop starts";
        qDebug() << "Target object: " << target_object->getPath().c_str();
    }

// If we copy many objects in the following loop, and some of them are
// groups that refer other objects in the same batch, then it is
// possible that an object would be copied by
// FWObjectDatabase::recursivelyCopySubtree() by the way of a
// reference from a group, and then the same object is found in the
// list of objects to be copied AGAIN. Since this object is already
// present in the target object tree by the time it needs to be copied
// again, actuallyPasteTo() chooses the path for copying of objects
// inside the same tree and creates a copy.  To avoid this, prepare a
// list of objects to be copied before copy operation starts.

    list<FWObject*> copy_objects;

    for (i= FWObjectClipboard::obj_clipboard->begin();
            i!=FWObjectClipboard::obj_clipboard->end(); ++i)
    {
        FWObject *co = FWObjectClipboard::obj_clipboard->getObjectByIdx(idx);
        copy_objects.push_back(co);
        idx++;
    }

    for (list<FWObject*>::iterator i=copy_objects.begin(); i!=copy_objects.end(); ++i)
    {
        FWObject *co = *i;

        if (fwbdebug)
            qDebug("Copy object %s (id=%d, root=%p)",
                   co->getName().c_str(), co->getId(), co->getRoot());
        if (map_ids.count(co->getId()) > 0)
            continue;

        // Check if we have already copied the same object before
        char s[64];
        sprintf(s, ".copy_of_%p", co->getRoot());
        string dedup_attribute = s;

        sprintf(s, "%d", co->getId());
        FWObject *n_obj =
            target_object->getRoot()->findObjectByAttribute(dedup_attribute, s);
        if (n_obj) continue;

        if (target_object->getRoot() != co->getRoot()) need_to_reload = true;

        last_object = actuallyPasteTo(target_object, co, map_ids);
    }
    if (fwbdebug) qDebug("**************** pasteObj loop done");


    if (need_to_reload) loadObjects();
    openObject(last_object);

}

bool ObjectManipulator::validateForPaste(FWObject *target, FWObject *obj,
                                         QString &err)
{
    FWObject *ta = target;
    if (IPv4::isA(ta) || IPv6::isA(ta)) ta=ta->getParent();

    err = QObject::tr("Impossible to insert object %1 (type %2) into %3\n"
                      "because of incompatible type.")
        .arg(obj->getName().c_str())
        .arg(obj->getTypeName().c_str())
        .arg(ta->getName().c_str());
    
    if (FWBTree().isSystem(ta))
        return m_project->validateForInsertion(ta, obj);

    Host *hst = Host::cast(ta);
    Firewall *fw = Firewall::cast(ta);
    Interface *intf = Interface::cast(ta);
    FWObject *parent_fw = ta;
    while (parent_fw && Firewall::cast(parent_fw)==NULL) parent_fw = parent_fw->getParent();

    if (parent_fw && Interface::isA(obj))
    {
        Resources* os_res = Resources::os_res[parent_fw->getStr("host_OS")];
        string os_family = parent_fw->getStr("host_OS");
        if (os_res!=NULL)
            os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

        std::auto_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                os_family));

        return int_prop->validateInterface(ta, obj, false, err);
    }

    if (fw!=NULL)
    {
        // inserting some object into firewall or cluster
        if (!fw->validateChild(obj)) return false;
        return true;
    }

    if (hst!=NULL)  return (hst->validateChild(obj));

    if (intf!=NULL)
    {
        if (!intf->validateChild(obj)) return false;
        return true;
    }

    Group *grp=Group::cast(ta);
    if (grp!=NULL) return grp->validateChild(obj);
    
    return false;
}

FWObject* ObjectManipulator::prepareForInsertion(FWObject *target, FWObject *obj)
{
    if (fwbdebug)
        qDebug("prepareForInsertion   %s --> %s", obj->getName().c_str(),
               target->getName().c_str());

    FWObject *ta = target;
    if (IPv4::isA(ta) || IPv6::isA(ta)) ta = ta->getParent();
    QString err;
    if (!validateForPaste(ta, obj, err))
    {
        QMessageBox::critical(
            this,"Firewall Builder",
            err,
            "&Continue", QString::null, QString::null,
            0, 1 );

        return NULL;
    }
    return ta;
}

FWObject*  ObjectManipulator::pasteTo(FWObject *target, FWObject *obj)
{
    map<int,int> map_ids;
    return actuallyPasteTo(target, obj, map_ids);
}

FWObject* ObjectManipulator::actuallyPasteTo(FWObject *target,
                                              FWObject *obj,
                                              std::map<int,int> &map_ids)
{
    FWObject *res = NULL;

    FWObject *ta = prepareForInsertion(target, obj);
    if (ta == NULL) return NULL;

    qDebug() << "ObjectManipulator::actuallyPasteTo"
             << "Target object: " << target->getPath().c_str();

    try
    {
/* clipboard holds a copy of the object */
        if (obj->getRoot() != ta->getRoot())
        {
            if (fwbdebug) qDebug("Copy object %s (%d) to a different object tree",
                                 obj->getName().c_str(), obj->getId());
            return m_project->db()->recursivelyCopySubtree(target, obj, map_ids);
        }

        Group *grp = Group::cast(ta);
        if (grp!=NULL && !FWBTree().isSystem(ta))
        {
            if (fwbdebug) qDebug("Copy object %s (%d) to a regular group",
                                 obj->getName().c_str(), obj->getId());
/* check for duplicates. We just won't add an object if it is already there */
            int cp_id = obj->getId();
            list<FWObject*>::iterator j;
            for (j=grp->begin(); j!=grp->end(); ++j)
            {
                FWObject *o1=*j;
                if(cp_id==o1->getId()) return o1;

                FWReference *ref;
                if( (ref=FWReference::cast(o1))!=NULL &&
                    cp_id==ref->getPointerId()) return o1;
            }
            grp->addRef(obj);
            res = obj;
        } else
        {
/* add a copy of the object to system group , or
 * add ruleset object to a firewall.
 */
            if (fwbdebug)
                qDebug("Copy object %s (%d) to a system group, "
                       "a ruleset to a firewall or an address to an interface",
                       obj->getName().c_str(), obj->getId());
            FWObject *nobj = m_project->db()->create(obj->getTypeName());
            assert (nobj!=NULL);
            nobj->ref();
            nobj->duplicate(obj, true);

            // make name unique before adding this object to the
            // parent, otherwise makeNameUnique finds it and thinks
            // the name is duplicate
            // makeNameUnique(ta, nobj);

            ta->add(nobj);

            // If we paste interface, reset the type of the copy
            // See #299
            if (Interface::isA(obj) && Interface::isA(ta))
            {
                Interface *new_intf = Interface::cast(nobj);
                new_intf->getOptionsObject()->setStr("type", "ethernet");
                // see #391 : need to reset "mamagement" flag in the copy
                // to make sure we do not end up with two management interfaces
                new_intf->setManagement(false);
            }

            insertSubtree( allItems[ta], nobj);
            res = nobj;
        }
    }
    catch(FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            ex.toString().c_str(),
            "&Continue", QString::null,QString::null,
            0, 1 );
    }

    return res;
}

void ObjectManipulator::relocateTo(FWObject *target, FWObject *obj)
{
    FWObject *ta = prepareForInsertion(target, obj);
    if (ta == NULL) return;

    if (obj->getRoot() != ta->getRoot())
    {
        if (fwbdebug)
            qDebug("Attempt to relocate object %s (%d) to a different object tree",
                   obj->getName().c_str(), obj->getId());
        return;
    }
    if (obj == ta) return;  // can't insert into intself

    removeObjectFromTreeView(obj);

    obj->ref();
    obj->getParent()->remove(obj);
    ta->add(obj);

    // If we paste interface, reset the type of the copy
    // See #299
    if (Interface::isA(obj) && Interface::isA(ta))
        Interface::cast(obj)->getOptionsObject()->setStr("type", "ethernet");

    insertSubtree(allItems[ta], obj);

    refreshSubtree(allItems[obj]);

    m_project->db()->setDirty(true);

    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(m_project->getFileName(), ta->getId()));

}

void ObjectManipulator::lockObject()
{
    if (fwbdebug)
        qDebug("ObjectManipulator::lockObject selected %d objects ",
               getCurrentObjectTree()->getNumSelected());

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj;

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        obj= *i;
        FWObject *lib = obj->getLibrary();
        // these lbraries are locked anyway, do not let the user
        // lock objects inside because they won't be able to unlock them.
        if (lib->getId()!=FWObjectDatabase::STANDARD_LIB_ID &&
            lib->getId()!=FWObjectDatabase::TEMPLATE_LIB_ID)
        {
            obj->setReadOnly(true);
            updateObjectInTree(obj, false);
        }
    }
    getCurrentObjectTree()->setLockFlags();

    // Arguably, locking an object should not change lastModified timestamp
    // because none of the attributes that affect generated policy change.
    //QCoreApplication::postEvent(
    //    mw, new dataModifiedEvent(m_project->getFileName(), 0));
}

void ObjectManipulator::unlockObject()
{
    if (fwbdebug)
        qDebug("ObjectManipulator::unlockObject selected %d objects ",
               getCurrentObjectTree()->getNumSelected());

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj;

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        obj= *i;
        FWObject *lib = obj->getLibrary();
        if (lib->getId()!=FWObjectDatabase::STANDARD_LIB_ID &&
            lib->getId()!=FWObjectDatabase::TEMPLATE_LIB_ID)
        {
            obj->setReadOnly(false);
            updateObjectInTree(obj, false);
        }
    }
    getCurrentObjectTree()->setLockFlags();

    //QCoreApplication::postEvent(
    //    mw, new dataModifiedEvent(m_project->getFileName(), 0));
}

/*
 * Note: this slot gets controlwhen user presses "Delete" key in
 * addition to menu items activation
 */
void ObjectManipulator::deleteObj()
{

    if (fwbdebug)
        qDebug("ObjectManipulator::deleteObj selected %d objects ",
               getCurrentObjectTree()->getNumSelected());
    
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *current_library = getCurrentLib();
    if (current_library->getId() == FWObjectDatabase::STANDARD_LIB_ID ||
        current_library->getId() == FWObjectDatabase::TEMPLATE_LIB_ID)
        return;

    FWObject *obj;
    bool emptyingTrash      = false;
    bool emptyingTrashInLib = false;
    
    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    vector<FWObject*> so2;
    
    for (vector<FWObject*>::iterator i=so.begin(); i!=so.end(); ++i)
    {
        bool del_obj_status = m_project->getDeleteMenuState(*i);
        if (fwbdebug)
            qDebug("ObjectManipulator::deleteObj object: %s del_obj_status=%d",
                   (*i)->getName().c_str(), del_obj_status);
        if (del_obj_status) so2.push_back(*i);
    }
    
    if (so2.size()==0) return;
    
    FWObject *delObjLib = m_project->db()->findInIndex(
        FWObjectDatabase::DELETED_OBJECTS_ID);

    if (fwbdebug)
        qDebug("ObjectManipulator::deleteObj  delObjLib=%p", delObjLib);
    
    if (delObjLib!=NULL)
    {
        for (vector<FWObject*>::iterator i=so2.begin();  i!=so2.end(); ++i)
        {
            obj = *i;
            emptyingTrash |= obj->isChildOf(delObjLib);
        }
    }
    
    emptyingTrashInLib = emptyingTrash && m_project->editingLibrary();
    
    /* Ask user iff:
     *
     * we are emptying trash while editing library file (.fwl)
     *    else
     *
     * if we are not emptying Trash (i.e. not deleting "Deleted objects" library)
     *    and
     * (we delete more than one object
     *    or
     * we delete one object and it is not a library (because in this case
     * we ask them later anyway))
     */
    
    QString msg;
    
    if (emptyingTrashInLib)
    {
        msg = tr(
            "Emptying the 'Deleted Objects' in a library file is not recommended.\n"
            "When you remove deleted objects from a library file, Firewall Builder\n"
            "loses ability to track them. If a group or a policy rule in some\n"
            "data file still uses removed object from this library, you may encounter\n"
            "unusual and unexpected behavior of the program.\n"
            "Do you want to delete selected objects anyway ?"
        );
        if (QMessageBox::warning(
                this,"Firewall Builder", msg,
                tr("&Yes"), tr("&No"), QString::null,
                0, 1 )!=0) return;
    } else
    {
    
        if (fwbdebug)
            qDebug("ObjectManipulator::deleteObj emptyingTrash=%d "
                   "so.size=%d  so.front()->type=%s",
                   emptyingTrash,
                   int(so.size()),
                   so2.front()->getTypeName().c_str() );
    
    
        if (!emptyingTrash && (so2.size()>1 || !Library::isA(so2.front())))
        {
            /*
              msg = tr(
              "When you delete an object, it is removed from the tree and\n"
              "all groups and firewall policy rules that reference it.\n"
              "Do you want to delete selected objects ?"
              );
              if (QMessageBox::warning(
              this,"Firewall Builder", msg,
              tr("&Yes"), tr("&No"), QString::null,
              0, 1 )!=0) return;
            */
            QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
            ConfirmDeleteObjectDialog * dlg =
                new ConfirmDeleteObjectDialog(this);
    
            dlg->load(so2);
    
            QApplication::restoreOverrideCursor();
            if(dlg->exec()==QDialog::Rejected ) return;
        }
    }
        
    /* need to work with a copy of the list of selected objects because
     * some of the methods we call below clear list
     * getCurrentObjectTree()->getSelectedObjects()
     */
    
    if (fwbdebug)
    {
        for (vector<FWObject*>::iterator i=so2.begin();  i!=so2.end(); ++i)
        {
            obj= *i;
            qDebug("ObjectManipulator::deleteObj will delete obj=%p ( %s %s ) ",
                   obj, obj->getTypeName().c_str(), obj->getName().c_str());
        }
    }
    
    try
    {
        for (vector<FWObject*>::iterator i=so2.begin();  i!=so2.end(); ++i)
        {
            obj= *i;
    
            //        openObject(obj,false);
    
            if ( ! FWBTree().isSystem(obj) )
            {
                if (Library::isA(obj))
                {
                    list<FWObject*> ll=m_project->db()->getByType(Library::TYPENAME);
                    if (ll.size()==1)  return;
    
                    if (QMessageBox::warning(
                            this,"Firewall Builder",
                            tr(
                                "When you delete a library, all objects that belong to it\n"
                                "disappear from the tree and all groups and rules that reference them.\n"
                                "You won't be able to reverse this operation later.\n"
                                "Do you still want to delete library %1?")
                            .arg(QString::fromUtf8(obj->getName().c_str())),
                            tr("&Yes"), tr("&No"), QString::null,
                            0, 1 )!=0 ) continue;
                }
    
                if (mw->isEditorVisible() &&
                    mw->getOpenedEditor()==obj) mw->hideEditor();
                    
                delObj(obj);
            }
        }
    }
    catch(FWException &ex)
    {
    }

}

void ObjectManipulator::delObj(FWObject *obj, bool openobj)
{
    bool firstAction = true ;

    if (fwbdebug)
        qDebug("ObjectManipulator::delObj  delete obj %p %s openobj=%d",
               obj,obj->getName().c_str(),openobj);

    FWObject *obj_library = obj->getLibrary();
    FWObject *parent = obj->getParent();
    FWObject *delObjLib = m_project->db()->findInIndex(
        FWObjectDatabase::DELETED_OBJECTS_ID );

    if (obj_library->getId() == FWObjectDatabase::STANDARD_LIB_ID ||
        obj_library->getId() == FWObjectDatabase::TEMPLATE_LIB_ID)
        return;

    if (obj->getId() == FWObjectDatabase::STANDARD_LIB_ID ||
        obj->getId() == FWObjectDatabase::DELETED_OBJECTS_ID) return;
    
    bool islib  = Library::isA(obj);
//        bool isintf = (Interface::isA(obj) && Firewall::isA(parent));
    bool isfw   = Firewall::isA(obj);
    bool isDelObj = (delObjLib!=NULL && obj->isChildOf(delObjLib));
   
    mw->findObjectWidget->reset();

    QCoreApplication::postEvent(
        mw, new closeObjectEvent(m_project->getFileName(), obj->getId()));
 
    try
    {    
        if (fwbdebug)
            qDebug("ObjectManipulator::delObj  delete islib=%d isfw=%d "
                   "isDelObj=%d", islib, isfw, isDelObj);
    
        /*
         * TODO: we have to remove not only the object, but also all
         * its child objects from the database, as well as all
         * references to them. This logic should really be in
         * FWObject::removeAllInstances(FWObject*);
         */
    
        /* remove from our internal tables before it is removed from the
         * object tree so we could use obj->getId()
         */
        if (islib && !isDelObj)
        {
            int idx = getIdxForLib(obj);
            QTreeWidget *otv = idxToTrees[idx];
            assert(otv!=NULL);
            m_objectManipulator->widgetStack->removeWidget( otv );
            removeLib(idx);
        } else
            removeObjectFromTreeView(obj);

        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
    
        if (islib && obj->isReadOnly()) obj->setReadOnly(false);
        if (obj->getId()==FWObjectDatabase::TEMPLATE_LIB_ID) // special case
        {
            if (fwbdebug)
                qDebug("ObjectManipulator::delObj:   "
                       "special case: deleting template library");
            m_project->db()->removeAllInstances(obj);
        } else
        {
            if (fwbdebug)
                qDebug("ObjectManipulator::delObj:   "
                       "recursively deleting library and all its objects");
            m_project->db()->recursivelyRemoveObjFromTree(obj,
                                                          false);
            if (islib)
                parent = m_project->db()->getFirstByType(
                    Library::TYPENAME);
        }
        
        QApplication::restoreOverrideCursor();

        QCoreApplication::postEvent(
            mw, new dataModifiedEvent(m_project->getFileName(), parent->getId()));

//        removeObjectFromTreeView(obj);
        m_project->scheduleRuleSetRedraw();
    
        if (!isDelObj)
        {
            if (allItems[delObjLib]!=NULL)
                insertSubtree( allItems[delObjLib], obj );
        } else
            FWObjectClipboard::obj_clipboard->clear();
    
        if (openobj)
        {
            if (isfw && !isDelObj)
            {
                std::list<Firewall*> fwlist;
                findAllFirewalls(fwlist);
                if (fwlist.size()>0)
                {
                    FWObject *first_fw = fwlist.front();
                    if (first_fw!=NULL)
                    {
                        openObject( first_fw );
                    }
                }
                //QTimer::singleShot( 0, m_project, SLOT(reopenFirewall()) );
            } else {
                openObject(parent);
            }
        }
    }
    catch(FWException &ex)
    {
        if (fwbdebug)
            qDebug("ObjectManipulator::delObj: catch:  restoreOverrideCursor");
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(
            this,"Firewall Builder",
            ex.toString().c_str(),
            "&Continue", QString::null,QString::null,
            0, 1 );
        throw(ex);
    }

    if (fwbdebug) qDebug("ObjectManipulator::delObj  done");

    firstAction = false ;
}

void ObjectManipulator::groupObjects()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *co = getCurrentObjectTree()->getSelectedObjects().front();

    newGroupDialog ngd(this, m_project->db());

    if (ngd.exec()==QDialog::Accepted)
    {
        QString objName = ngd.m_dialog->obj_name->text();
        QString libName = ngd.m_dialog->libs->currentText();

        QString type = ObjectGroup::TYPENAME;
        if (Service::cast(co)!=NULL)  type=ServiceGroup::TYPENAME;
        if (Interval::cast(co)!=NULL) type=IntervalGroup::TYPENAME;

        FWObject *newgrp=NULL;

        list<FWObject*> ll = m_project->db()->getByType( Library::TYPENAME );
        for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
        {
            FWObject *lib=*i;
            if (libName==QString::fromUtf8(lib->getName().c_str()))
            {
/* TODO: need to show a dialog and say that chosen library is
 * read-only.  this is not critical though since newGroupDialog fills
 * the pull-down only with names of read-write libraries
 */
                if (lib->isReadOnly()) return;
                FWObject *parent = m_project->getFWTree()->getStandardSlotForObject(lib,type);
                if (parent==NULL)
                {
                    if (fwbdebug)
                        qDebug("ObjectManipulator::groupObjects(): could not find standard slot for object of type %s in library %s",
                               type.toAscii().constData(),lib->getName().c_str());
                    return;
                }
                newgrp = createObject(parent,type,objName);

                break;
            }
        }
        if (newgrp==NULL) return;

        FWObject *obj;

        ObjectTreeView* ot=getCurrentObjectTree();
        ot->freezeSelection(true);

        vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();

        for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
        {
            obj= *i;
            newgrp->addRef(obj);
        }
        ot->freezeSelection(false);

        openObject(newgrp);
        editObject(newgrp);

        QCoreApplication::postEvent(
            mw, new dataModifiedEvent(m_project->getFileName(), newgrp->getId()));
    }
}

void ObjectManipulator::restoreSelection(bool same_widget)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::restoreSelection  same_widget=%d",
               same_widget);

//    select();
    openObject( mw->getOpenedEditor(), false);
}

void ObjectManipulator::editSelectedObject()
{
    if (fwbdebug) qDebug("ObjectManipulator::editSelectedObject");

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;
//    obj->dump(false,false);
    if (RuleSet::cast(obj)!=NULL)
    {
        if (m_project->getCurrentRuleSet()!=obj)
        {
            m_project->openRuleSet(obj);
            return ;
        }
    }
    editObject(obj);
}

bool ObjectManipulator::editObject(FWObject *obj)
{
    if (fwbdebug) qDebug("ObjectManipulator::editObject");
    if (!mw->isEditorVisible()) mw->showEditor();
    return switchObjectInEditor(obj);
}

bool ObjectManipulator::switchObjectInEditor(FWObject *obj)
{
    if (fwbdebug) qDebug("ObjectManipulator::switchObjectInEditor");

    if (obj && fwbdebug)
    {
        qDebug("obj: %s", obj->getName().c_str());
        FWObject *edt_obj = mw->getOpenedEditor();
        if (edt_obj)
            qDebug("in editor: %s", edt_obj->getName().c_str());
    }

    if (RuleSet::cast(obj)!=NULL)
    {
        if (obj!=m_project->getCurrentRuleSet()) 
        {           
            m_project->openRuleSet(obj);
        }
    
    }
    if (!mw->isEditorVisible()) return false;

    if (!mw->requestEditorOwnership(
            this, obj, ObjectEditor::optNone, true))
    {
        if (fwbdebug) qDebug("Can not get editor panel ownership");
        return false;
    }

    if (fwbdebug) qDebug("Calling select");
    
    if (obj != mw->getOpenedEditor())
    {
        if (fwbdebug) qDebug("Open object in editor");
        mw->openEditor(obj);
        currentObj = obj;
        active = true;
        openObject(obj);  // position the tree so that obj is visible
        if (fwbdebug) qDebug("Done");
    }

    if (fwbdebug) qDebug("ObjectManipulator::switchObjectInEditor done");
    
    return true;      // successfully (re)opened obj in the editor
}

void ObjectManipulator::closeObject()
{
    currentObj = NULL;
    if (mw->isEditorVisible()) mw->hideEditor();
}

void ObjectManipulator::openObject(ObjectTreeViewItem *otvi,
                                   bool register_in_history)
{
    openObject(otvi->getFWObject(),register_in_history);
}

void ObjectManipulator::selectionChanged(QTreeWidgetItem *cur)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::selectionChanged");

    QTreeWidget *qlv = getCurrentObjectTree();
    if (qlv==NULL) return;

    ObjectTreeViewItem* otvi = dynamic_cast<ObjectTreeViewItem*>(cur);

    if (otvi==NULL) return;

    FWObject *obj = otvi->getFWObject();
    if (obj==NULL) return;

    if (fwbdebug) qDebug("ObjectManipulator::selectionChanged obj=%s",
                         obj->getName().c_str());

    FWObject *o=obj;
//    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    if (history.empty() || otvi!=history.top().item() )
    {
        mw->enableBackAction();
        history.push( HistoryItem(otvi, o->getId()) );
    }

    currentObj = obj;

    active=true;

    update();

    // Send event to project panel object to cause update of currentObj
    // display in rules. If this object is selected in the tree, it gets
    // highlighted with a thin red border in the rules.
    QCoreApplication::postEvent(
        m_project, new updateObjectInRulesetEvent(m_project->getFileName(),
                                                  currentObj->getId()));

    if (fwbdebug) qDebug("ObjectManipulator::selectionChanged done");
}

/*
 * I could use default value for the parameter register_in_history,
 * but that caused problems when this method was used as a slot
 */
void ObjectManipulator::openObject(QTreeWidgetItem *item)
{
    ObjectTreeViewItem *otvi=dynamic_cast<ObjectTreeViewItem*>(item);
    openObject(otvi,true);
}

void ObjectManipulator::openObject(FWObject *obj)
{
    openObject(obj,true);
}

/* This method is called from the GroupObjectDialog when user double
 * clicks on the object in a group, so first we should check if this
 * object is shown in the tree and if not, find and open it.
 */
void ObjectManipulator::openObject(FWObject *obj, bool /*register_in_history*/)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::openObject   obj=%s (%d)",
               (obj)?obj->getName().c_str():"NULL",
               (obj)?obj->getId():0);

    if (obj==NULL) return;

    raise();
    FWObject *o=obj;
    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    ObjectTreeViewItem *otvi = allItems[o];

// this changes selection and thus calls slot slectionChanged
    showObjectInTree(otvi);

    m_objectManipulator->libs->setCurrentIndex(
        getIdxForLib( obj->getLibrary()));

    if (fwbdebug)
        qDebug("ObjectManipulator::openObject:  libs->currentIndex=%d",
               m_objectManipulator->libs->currentIndex());

    updateCreateObjectMenu(obj->getLibrary());

    if (fwbdebug) qDebug("ObjectManipulator::openObject: done");
}

void ObjectManipulator::showObjectInTree(ObjectTreeViewItem *otvi)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::showObjectInTree");
    if (otvi==NULL) return;

    ObjectTreeView* otv = otvi->getTree();

    if (fwbdebug) qDebug("ObjectManipulator::showObjectInTree  current_tree_view=%p  new_otv=%p",current_tree_view,otv);

//    otv->raise();
    m_objectManipulator->widgetStack->setCurrentWidget(otv);

    otvi->getTree()->clearSelection();
    otvi->getTree()->scrollToItem( otvi );
    otvi->getTree()->setCurrentItem( otvi );
    otvi->setSelected( true );
}

void ObjectManipulator::invalidateDialog()
{
    currentObj=NULL;
}

void ObjectManipulator::libChangedById(int id)
{
    for (vector<FWObject*>::size_type i = 0 ; i < idxToLibs.size(); i++)
    {
        if (idxToLibs[i]->getId()==id)
        {
            libChanged(i);
            m_objectManipulator->libs->setCurrentIndex(i);
            return;
        }
    }
}

void ObjectManipulator::changeFirstNotSystemLib()
{
    QString sid2 = "syslib000";
    QString sid3 = "syslib001";
    for (vector<FWObject*>::size_type i = 0 ; i < idxToLibs.size(); i++)
    {
        QString sid1 = FWObjectDatabase::getStringId(idxToLibs[i]->getId()).c_str();
        if ( sid1 != sid2)
        {
            if (sid1!=sid3)
            {
                libChanged(i);
                m_objectManipulator->libs->setCurrentIndex(i);
                return;
            }
        }
    }
}

void ObjectManipulator::libChanged(int ln)
{
    QTreeWidget *lv = idxToTrees[ln];
    assert(lv!=NULL);

    ObjectTreeViewItem *otvi=dynamic_cast<ObjectTreeViewItem*>(lv->currentItem());

    if (otvi == NULL)
    {
        if (lv->invisibleRootItem()->childCount() > 0)
            otvi = dynamic_cast<ObjectTreeViewItem*>(
                lv->invisibleRootItem()->child(0));
        else
            assert(FALSE);
    }
    currentObj=otvi->getFWObject();
    showObjectInTree( otvi );

    updateCreateObjectMenu( idxToLibs[ln] );
    return;
}

void ObjectManipulator::updateCreateObjectMenu(FWObject* lib)
{
    bool      f   =
        lib->getId()==FWObjectDatabase::STANDARD_LIB_ID ||
        lib->getId()==FWObjectDatabase::TEMPLATE_LIB_ID ||
        lib->getId()==FWObjectDatabase::DELETED_OBJECTS_ID  ||
        lib->isReadOnly();

    m_objectManipulator->newButton->setEnabled( !f );
    QAction *noa = (QAction*)(mw->findChild<QAction*>("newObjectAction"));
    noa->setEnabled( !f );
}

void ObjectManipulator::back()
{
//    if (!validateDialog()) return;

    if (!history.empty())
    {
        history.pop();

/* skip objects that have been deleted */
        while ( ! history.empty())
        {
            if (m_project->db()->findInIndex( history.top().id() )!=NULL) break;
            history.pop();
        }

        if (history.empty())
        {
            mw->enableBackAction();
            return;
        }

        openObject( history.top().item(), false );

        if (mw->isEditorVisible())
        {
            ObjectTreeViewItem *otvi=history.top().item();
            switchObjectInEditor(otvi->getFWObject());
        }
    }
}

FWObject*  ObjectManipulator::getCurrentLib()
{
    int idx = m_objectManipulator->libs->currentIndex();
    FWObject *lib = idxToLibs[idx];
    if (fwbdebug)
    {
        qDebug("ObjectManipulator::getCurrentLib(): idx=%d  lib=%p", idx, lib);
    }

    return lib;
}

ObjectTreeView* ObjectManipulator::getCurrentObjectTree()
{
    return current_tree_view;
}

void ObjectManipulator::openLib(FWObject *obj)
{
    openObject(obj->getLibrary(),false);
}

void ObjectManipulator::newObject()
{
//    QToolButton *btn = (QToolButton*)(m_project->toolBar)->child("newObjectAction_action_button");
    m_objectManipulator->newButton->showMenu();
}

FWObject* ObjectManipulator::createObject(const QString &objType,
                                          const QString &objName,
                                          FWObject *copyFrom)
{

    if (!validateDialog()) return NULL;

    if (fwbdebug) qDebug("ObjectManipulator::createObject   check 1");

    FWObject *lib  = getCurrentLib();
    int       i = 0;

    if (fwbdebug)
    {
        qDebug("lib: %s %s",
               lib->getName().c_str(),
               FWObjectDatabase::getStringId(lib->getId()).c_str());
        qDebug("libs->count()=%d", m_objectManipulator->libs->count() );
    }

    while ( lib->getId()==FWObjectDatabase::STANDARD_LIB_ID ||
            lib->getId()==FWObjectDatabase::TEMPLATE_LIB_ID ||
            lib->getId()==FWObjectDatabase::DELETED_OBJECTS_ID  ||
            lib->isReadOnly() )
    {
        if (i>=m_objectManipulator->libs->count())
        {
//            if (fwbdebug)
//                qDebug("ObjectManipulator::createObject   return NULL");
//            return NULL;
            lib  = getCurrentLib();
            break;
        }

        lib= idxToLibs[i];

        if (fwbdebug)
        {
            qDebug("i=%d",i);
            qDebug("lib: %s %s",
                   lib->getName().c_str(),
                   FWObjectDatabase::getStringId(lib->getId()).c_str());
        }
        i++;
    }

    FWObject *parent = 
        m_project->getFWTree()->getStandardSlotForObject(lib, objType);

    if (parent==NULL)
    {
      QMessageBox::warning(this,"Firewall Builder",
                           QObject::tr(
"Type '%1': new object can not be created because\n"
"corresponding branch is missing in the object tree.\n"
"Please repair the tree using command 'fwbedit checktree -f file.fwb'.")
                           .arg(objType),
                           "&Continue", QString::null, QString::null,
                           0, 1 );
      return NULL;
    }

    return actuallyCreateObject(parent, objType, objName, copyFrom);
}

FWObject* ObjectManipulator::createObject(FWObject *parent,
                                          const QString &objType,
                                          const QString &objName,
                                          FWObject *copyFrom)
{
    if (!validateDialog()) return NULL;

    FWObject *lib  = getCurrentLib();
    int       i = 0;

    assert(parent!=NULL);

    if (fwbdebug)
    {
        qDebug("ObjectManipulator::createObject 2: parent=%s",
               parent->getName().c_str());
        qDebug("ObjectManipulator::createObject 2: objType=%s  objName=%s",
               objType.toLatin1().constData(), objName.toLatin1().constData());
    }

    while ( lib->getId()==FWObjectDatabase::STANDARD_LIB_ID ||
            lib->getId()==FWObjectDatabase::TEMPLATE_LIB_ID ||
            lib->getId()==FWObjectDatabase::DELETED_OBJECTS_ID  ||
            lib->isReadOnly() )
    {
        if (i >= m_objectManipulator->libs->count())
        {
            lib=getCurrentLib();
            break;
        }
        lib= idxToLibs[i];
        i++;
    }

    if (parent==NULL) parent=lib;

    return actuallyCreateObject(parent, objType, objName, copyFrom);   
}

FWObject* ObjectManipulator::actuallyCreateObject(FWObject *parent,
                                                  const QString &objType,
                                                  const QString &objName,
                                                  FWObject *copyFrom)
{
    FWObject *nobj=NULL;
    if (!isTreeReadWrite(this, parent)) return NULL;
    nobj = m_project->db()->create(objType.toLatin1().constData());
    assert(nobj!=NULL);
    if (copyFrom!=NULL) nobj->duplicate(copyFrom, true);
    if (nobj->isReadOnly()) nobj->setReadOnly(false);
    nobj->setName( string(objName.toUtf8().constData()) );
    parent->add(nobj);

    ObjectTreeViewItem* parent_item = allItems[parent];
    if (fwbdebug)
        qDebug("insertSubtree: parent=%s  allItems[parent]=%p",
               parent->getName().c_str(), parent_item);

    insertSubtree(parent_item, nobj);
    refreshSubtree(allItems[nobj]);

    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(m_project->getFileName(), parent->getId()));

    m_project->db()->setDirty(true);
    return nobj;
}

void ObjectManipulator::newLibrary()
{
    if (!validateDialog()) return;
    FWObject *nlib = m_project->createNewLibrary(m_project->db());
    addTreePage( nlib );

    //QCoreApplication::postEvent(
    //    mw, new dataModifiedEvent(m_project->getFileName(), nlib->getId()));

    openObject(nlib);
    editObject(nlib);
}

void ObjectManipulator::newPolicyRuleSet ()
{
    if ( currentObj->isReadOnly() ) return;
    QString name = "Policy";
    Firewall * fw = Firewall::cast(currentObj);
    if (fw!=NULL)
    {
        int count = 0;
        for (FWObjectTypedChildIterator it = fw->findByType(Policy::TYPENAME);it != it.end(); ++it)
            count++;
        if (count>0)
        {
            name+="_";
            name+=QString().setNum(count);
        }
    }
    FWObject *o = createObject(currentObj,Policy::TYPENAME,name);
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }

    this->getCurrentObjectTree()->sortItems(0, Qt::AscendingOrder);
}

void ObjectManipulator::newNATRuleSet ()
{
    if ( currentObj->isReadOnly() ) return;
    QString name = "NAT";
    Firewall * fw = Firewall::cast(currentObj);
    if (fw!=NULL)
    {
        int count = 0;
        for (FWObjectTypedChildIterator it = fw->findByType(NAT::TYPENAME);
             it != it.end(); ++it) count++;

        if (count>0)
        {
            name += "_";
            name += QString().setNum(count);
        }
    }
    FWObject *o = createObject(currentObj,NAT::TYPENAME,name);
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }

    this->getCurrentObjectTree()->sortItems(0, Qt::AscendingOrder);
}

void ObjectManipulator::newFirewall()
{
    FWObject *parent = 
        FWBTree().getStandardSlotForObject(getCurrentLib(), Firewall::TYPENAME);
    assert(parent);
    ObjectTreeViewItem* parent_item = allItems[parent];
    assert(parent_item);

    newFirewallDialog *nfd = new newFirewallDialog(parent);
    if (mw->isEditorVisible()) mw->hideEditor();
    nfd->exec();
    FWObject *nfw = nfd->getNewFirewall();
    delete nfd;

    if (nfw!=NULL)
    {
        insertSubtree(parent_item, nfw);
        updateObjName(nfw, "", false);
        openObject(nfw);
        editObject(nfw);
        QCoreApplication::postEvent(
            mw, new dataModifiedEvent(m_project->getFileName(), nfw->getId()));
    }
}

void ObjectManipulator::newCluster()
{
    FWObject *parent = 
        FWBTree().getStandardSlotForObject(getCurrentLib(), Cluster::TYPENAME);
    assert(parent);
    ObjectTreeViewItem* parent_item = allItems[parent];
    assert(parent_item);

    newClusterDialog *ncd = new newClusterDialog(parent);
    if (mw->isEditorVisible())  mw->hideEditor();
    ncd->exec();
    FWObject *ncl = ncd->getNewCluster();
    delete ncd;

    if (ncl != NULL)
    {
        insertSubtree(parent_item, ncl);
        updateObjName(ncl, "", false);
        openObject(ncl);
        editObject(ncl);
        QCoreApplication::postEvent(
            mw, new dataModifiedEvent(m_project->getFileName(), ncl->getId()));
    }
}

void ObjectManipulator::newClusterIface()
{
    if (currentObj->isReadOnly()) return;

    QString new_name = makeNameUnique(currentObj,
                                      findNewestInterfaceName(currentObj),
                                      Interface::TYPENAME);
    FWObject *o = createObject(currentObj, Interface::TYPENAME, new_name);

    // Do not call updateObjectInTree because it will be called
    // from ProjectPanel::event when we process updateObjectInTree event
    //updateObjectInTree(o);
    openObject(o);

//    updateLastModifiedTimestampForAllFirewalls(o);
    editObject(o);
}

/*
 * Creates new state sync group; this method is called by context menu item
 * associated with Cluster object.
 * By default assume conntrack protocol and set group type accordingly.
 */
void ObjectManipulator::newStateSyncClusterGroup()
{
    if ( currentObj->isReadOnly() ) return;

    FWObject *o = NULL;

    FWObject *cluster = currentObj;
    while (cluster && !Cluster::isA(cluster)) cluster = cluster->getParent();
    assert(cluster != NULL);
    QString host_os = cluster->getStr("host_OS").c_str();

    list<QStringPair> lst;
    getStateSyncTypesForOS(host_os, lst);
    if (lst.size() == 0)
    {
        // No state sync. protocols for this host OS
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("Cluster host OS %1 does not support state synchronization").arg(host_os),
            "&Continue", QString::null, QString::null, 0, 1 );
        return;
    }

    QString group_type = lst.front().first;

    o = createObject(currentObj, StateSyncClusterGroup::TYPENAME,
                     tr("State Sync Group"));
    o->setStr("type", group_type.toStdString());

    openObject(o);

//    updateLastModifiedTimestampForAllFirewalls(o);

    editObject(o);
}

/*
 * Creates new failover group; this method is called by context menu item
 * associated with Interface object if its parent is a Cluster object
 * By default assume VRRP protocol and set group type accordingly.
 */
void ObjectManipulator::newFailoverClusterGroup()
{
    if ( currentObj->isReadOnly() ) return;

    FWObject *o = NULL;

    QString group_type;
    if (Interface::isA(currentObj))
    {
        group_type = "vrrp";
    } else
    {
        qWarning("newClusterGroup: invalid currentObj");
        return;
    }

    o = createObject(currentObj, FailoverClusterGroup::TYPENAME,
                     tr("Failover group"));
    o->setStr("type", group_type.toStdString());

    openObject(o);

//    updateLastModifiedTimestampForAllFirewalls(o);

    editObject(o);
}

void ObjectManipulator::newHost()
{
    newHostDialog *nhd = new newHostDialog();
    if (mw->isEditorVisible()) mw->hideEditor();
    nhd->exec();
    FWObject *o = nhd->getNewHost();
    delete nhd;

    this->getCurrentObjectTree()->sortItems(0, Qt::AscendingOrder);

    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }

    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(m_project->getFileName(), o->getId()));

}

QString ObjectManipulator::findNewestInterfaceName(FWObject *parent)
{
    time_t newest_interface = 0;
    QString newest_interface_name = "Interface";
    // look for interfaces on the same level (do not use getByTypeDeep() because
    // it also finds subinterfaces)
    // find interface that was created last and use its name as a prototype
    for (FWObjectTypedChildIterator it = parent->findByType(Interface::TYPENAME);
         it != it.end(); ++it)
    {
        if (newest_interface < (*it)->getCreationTime())
        {
            newest_interface = (*it)->getCreationTime();
            newest_interface_name = (*it)->getName().c_str();
        }
    }
    return newest_interface_name;
}

void ObjectManipulator::newInterface()
{
    if ( currentObj->isReadOnly() ) return;

    Interface *new_interface = NULL;
    FWObject *parent = NULL;

    if (Host::isA(currentObj) || Firewall::isA(currentObj))
        parent = currentObj;

    if (Interface::isA(currentObj))
    {
        FWObject *h = Interface::cast(currentObj)->getParentHost();

        bool supports_advanced_ifaces = false;
        supports_advanced_ifaces =
                Resources::getTargetCapabilityBool(h->getStr("host_OS"),
                                                   "supports_subinterfaces");
        if (supports_advanced_ifaces)
        {
            parent = currentObj;
        } else {
            parent = h;
        }
    }

    if (parent == NULL)
    {
        // since we can;t find quitable parent for the new interface,
        // we can't create it.
        return;
    }

    QString new_name = makeNameUnique(parent, findNewestInterfaceName(parent),
                                      Interface::TYPENAME);
    new_interface = Interface::cast(
        createObject(parent, Interface::TYPENAME, new_name));

    if (new_interface == NULL) return;

    if (Interface::isA(parent))
        guessSubInterfaceTypeAndAttributes(new_interface);
    else
        new_interface->getOptionsObject()->setStr("type", "ethernet");

    openObject(new_interface);

//    updateLastModifiedTimestampForAllFirewalls(new_interface);

    editObject(new_interface);
}

void ObjectManipulator::newNetwork()
{
    FWObject *o=createObject(Network::TYPENAME,tr("Network"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}
void ObjectManipulator::newNetworkIPv6()
{
    FWObject *o = createObject(NetworkIPv6::TYPENAME,tr("Network IPv6"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newAddress()
{
    if ( currentObj->isReadOnly() ) return;

    FWObject *o = createObject(IPv4::TYPENAME, tr("Address"));

#if 0
    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf &&
            (intf->isDyn() || intf->isUnnumbered() || intf->isBridgePort())
        ) return;
	QString iname=QString("%1:%2:ip")
	    .arg(QString::fromUtf8(currentObj->getParent()->getName().c_str()))
	    .arg(QString::fromUtf8(currentObj->getName().c_str()));
        o=createObject(currentObj, IPv4::TYPENAME, iname);
    }
    else
    {
        o=createObject(IPv4::TYPENAME,tr("Address"));
    }
#endif


    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newAddressIPv6()
{
    if ( currentObj->isReadOnly() ) return;

    FWObject *o;
    o=createObject(IPv6::TYPENAME,tr("Address IPv6"));

    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}


void ObjectManipulator::newDNSName()
{
    FWObject *o;
    o=createObject(DNSName::TYPENAME,tr("DNS Name"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newAddressTable()
{
    FWObject *o;
    o=createObject(AddressTable::TYPENAME,tr("Address Table"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newInterfaceAddress()
{
    if ( currentObj->isReadOnly() ) return;

    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf &&
            (intf->isDyn() || intf->isUnnumbered() || intf->isBridgePort())
        ) return;
        QString iname = getStandardName(currentObj, IPv4::TYPENAME, "ip");
        iname = makeNameUnique(currentObj, iname, IPv4::TYPENAME);
        FWObject *o = createObject(currentObj, IPv4::TYPENAME, iname);
        if (o!=NULL)
        {
            openObject(o);
            editObject(o);
//            updateLastModifiedTimestampForAllFirewalls(o);
        }
    }
}

void ObjectManipulator::newInterfaceAddressIPv6()
{
    if ( currentObj->isReadOnly() ) return;

    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf &&
            (intf->isDyn() || intf->isUnnumbered() || intf->isBridgePort())
        ) return;
        QString iname = getStandardName(currentObj, IPv4::TYPENAME, "ipv6");
        iname = makeNameUnique(currentObj, iname, IPv4::TYPENAME);
        FWObject *o = createObject(currentObj, IPv6::TYPENAME, iname);
        if (o!=NULL)
        {
            openObject(o);
            editObject(o);
//            updateLastModifiedTimestampForAllFirewalls(o);
        }
    }
}

void ObjectManipulator::newTagService()
{
    FWObject *o;
    o=createObject(TagService::TYPENAME,tr("TagService"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}
void ObjectManipulator::newUserService()
{
    FWObject *o;
    o=createObject(UserService::TYPENAME,tr("User Service"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newPhysicalAddress()
{
    if ( currentObj->isReadOnly() ) return;

    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf->getByType(physAddress::TYPENAME).empty())
        {
            QString iname=QString("%1:%2:mac")
                .arg(QString::fromUtf8(currentObj->getParent()->getName().c_str()))
                .arg(QString::fromUtf8(currentObj->getName().c_str()));
            FWObject *o=createObject(currentObj,physAddress::TYPENAME,iname);
            if (o!=NULL)
            {
                openObject(o);
                editObject(o);
//                updateLastModifiedTimestampForAllFirewalls(o);
            }
        }
    }
}

void ObjectManipulator::newAddressRange()
{
    FWObject *o;
    o = createObject(AddressRange::TYPENAME,tr("Address Range"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newObjectGroup()
{
    FWObject *o;
    o=createObject(ObjectGroup::TYPENAME,tr("Object Group"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}


void ObjectManipulator::newCustom()
{
    FWObject *o;
    o=createObject(CustomService::TYPENAME,tr("Custom Service"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newIP()
{
    FWObject *o;
    o=createObject(IPService::TYPENAME,tr("IP Service"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newICMP()
{
    FWObject *o;
    o=createObject(ICMPService::TYPENAME,tr("ICMP Service"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newICMP6()
{
    FWObject *o;
    o=createObject(ICMP6Service::TYPENAME,tr("ICMP6 Service"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newTCP()
{
    FWObject *o;
    o=createObject(TCPService::TYPENAME,tr("TCP Service"));

    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newUDP()
{
    FWObject *o;
    o=createObject(UDPService::TYPENAME,tr("UDP Service"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

void ObjectManipulator::newServiceGroup()
{
    FWObject *o;
    o=createObject(ServiceGroup::TYPENAME,tr("Service Group"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}


void ObjectManipulator::newInterval()
{
    FWObject *o;
    o=createObject(Interval::TYPENAME,tr("Time Interval"));
    if (o!=NULL)
    {
        openObject(o);
        editObject(o);
    }
}

bool ObjectManipulator::validateDialog()
{
    if (currentObj==NULL) return true;
    if (!mw->isEditorVisible()) return true;
    return mw->validateAndSaveEditor();
}

void ObjectManipulator::select()
{
//    if (fwbdebug)
        qDebug("ObjectManipulator::select()");

    if (currentObj==NULL) return;

    if (fwbdebug) qDebug("currentObj=%s", currentObj->getName().c_str());

    m_objectManipulator->libs->setCurrentIndex(
        getIdxForLib(currentObj->getLibrary()));

    // TODO: I forget why do we need flag "active", check this.
    ObjectTreeViewItem *otvi = allItems[currentObj];
    if (otvi)
    {
        active = true;
    }

    //QCoreApplication::postEvent(
    //    m_project, new updateObjectInRulesetEvent(m_project->getFileName(),
    //                                              currentObj->getId()));

    if (fwbdebug) qDebug("ObjectManipulator::select() done");
}

void ObjectManipulator::unselect()
{
    if (currentObj==NULL) return;

    for (int i=0; i<m_objectManipulator->libs->count(); i++)
        idxToTrees[i]->clearSelection();

    active=false;
}

bool ObjectManipulator::isSelected()
{
    return active;
}

/*
 * per bug #2412334, FWObjectDatabase::findWhereObjectIsUsed finds
 * only "direct" uses of object (i.e. it finds group the object is
 * member of, but not other groups or rules the group is member of).
 * This method is recursive wrapper around FWObjectDatabase::findWhereObjectIsUsed
 */
void ObjectManipulator::findWhereUsedRecursively(FWObject *obj,
                                                 FWObject *top,
                                                 set<FWObject*> &resset)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::findWhereUsedRecursively obj=%s (%s)",
               obj->getName().c_str(), obj->getTypeName().c_str());

    set<FWObject*> resset_tmp;

/*
 * findWhereObjectIsUsed() finds references to object 'obj' in a subtree
 *  rooted at object 'top'.
 */
    m_project->db()->findWhereObjectIsUsed(obj, top, resset_tmp);
    set<FWObject *>::iterator i = resset.begin();
    for ( ; i!=resset.end(); ++i)
        if (resset_tmp.count(*i)) resset_tmp.erase(*i);

    resset.insert(resset_tmp.begin(), resset_tmp.end());

    i = resset_tmp.begin();
    for ( ; i!=resset_tmp.end(); ++i)
    {
        FWObject *parent_obj = *i;
        FWReference  *ref = FWReference::cast(parent_obj);
        if (ref)
            parent_obj = ref->getParent();  // NB! We need parent of this ref.

        // add new results to a separate set to avoid modifying the resset_tmp
        // in the middle of iteration
        if (Group::cast(parent_obj) && !RuleElement::cast(parent_obj))
            findWhereUsedRecursively(parent_obj, top, resset);
    }
}

list<Firewall*> ObjectManipulator::findFirewallsForObject(FWObject *o)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::findFirewallsForObject");

    list<Firewall *> fws;

    set<FWObject *> resset;
    QTime tt;
    tt.start();
    FWObject *f=o;
    while (f!=NULL && !Firewall::cast(f)) f=f->getParent();
    if (f) fws.push_back(Firewall::cast(f));

    findWhereUsedRecursively(o, m_project->db(), resset);

    //m_project->db()->findWhereObjectIsUsed(o, m_project->db(), resset);

    FindWhereUsedWidget::humanizeSearchResults(resset);

    set<FWObject *>::iterator i = resset.begin();
    for ( ;i!=resset.end();++i)
    {
        RuleElement *re = RuleElement::cast(*i);
        if (re==NULL) continue;

        Rule *r=Rule::cast(re->getParent());
        if (r && !r->isDisabled())
        {
            f=r;
            while (f!=NULL && !Firewall::isA(f)) f=f->getParent();
            if (f && std::find(fws.begin(),fws.end(),f)==fws.end())
            {
                fws.push_back(Firewall::cast(f));
            }
        }
    }

    if (fwbdebug)
        qDebug(QString("Program spent %1 ms searching for firewalls.")
               .arg(tt.elapsed()).toAscii().constData());

    return fws;
}

list<Cluster*> ObjectManipulator::findClustersUsingFirewall(FWObject *fw)
{
    list<Cluster*> res;
    list<Cluster*> all_clusters;
    findAllClusters(all_clusters);
    list<Cluster*>::iterator it;
    for (it=all_clusters.begin(); it!=all_clusters.end(); ++it)
    {
        Cluster *cl = *it;
        list<Firewall*> members;
        cl->getMembersList(members);
        if (std::find(members.begin(), members.end(), Firewall::cast(fw)) != members.end())
            res.push_back(cl);
    }
    return res;
}

void ObjectManipulator::findAllFirewalls(list<Firewall*> &fws)
{
    if (fwbdebug) qDebug("ObjectManipulator::findAllFirewalls");

    list<FWObject*> fwlist;
    findByObjectType(m_project->db(), Firewall::TYPENAME, fwlist);
    for (list<FWObject*>::iterator m=fwlist.begin(); m!=fwlist.end(); m++)
        fws.push_back(Firewall::cast(*m));
}

void ObjectManipulator::findAllClusters(list<Cluster*> &clusters)
{
    list<FWObject*> cllist;
    findByObjectType(m_project->db(), Cluster::TYPENAME, cllist);
    for (list<FWObject*>::iterator m=cllist.begin(); m!=cllist.end(); m++)
        clusters.push_back(Cluster::cast(*m));
}

void ObjectManipulator::simulateInstall()
{
    if (fwbdebug) qDebug("ObjectManipulator::simulateInstall");

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    Firewall *fw;

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
    {
        fw= Firewall::cast( *i );
        if (fw!=NULL)
        {
            fw->updateLastCompiledTimestamp();
            fw->updateLastInstalledTimestamp();
        }
    }
}

FWObject* ObjectManipulator::getSelectedObject()
{
    return currentObj;
}

void ObjectManipulator::findWhereUsedSlot()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj=getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;
    mw->findWhereUsed(obj, m_project);

}

void ObjectManipulator::reopenCurrentItemParent()
{
    QTreeWidgetItem *itm = getCurrentObjectTree()->currentItem();
    if (itm)  itm = itm->parent();
    if (!itm) return;
    itm->parent()->setExpanded(false);
    itm->parent()->setExpanded(true);
    getCurrentObjectTree()->scrollToItem(itm, QAbstractItemView::EnsureVisible);
    getCurrentObjectTree()->update();
}

void ObjectManipulator::loadSectionSizes()
{
    for (int i=0; i<m_objectManipulator->libs->count(); i++)
    {
        ObjectTreeView *objTreeView =
            dynamic_cast<ObjectTreeView*>(idxToTrees[i]);
        FWObject *lib = idxToLibs[i];
        objTreeView->header()->resizeSection(
            0, 
            st->getTreeSectionSize(
                m_project->getFileName(), lib->getName().c_str(), 0));
        objTreeView->header()->resizeSection(
            1, 
            st->getTreeSectionSize(
                m_project->getFileName(), lib->getName().c_str(), 1));
    }
}

void ObjectManipulator::saveSectionSizes()
{
    for (int i=0; i<m_objectManipulator->libs->count(); i++)
    {
        ObjectTreeView *objTreeView =
            dynamic_cast<ObjectTreeView*>(idxToTrees[i]);
        FWObject *lib = idxToLibs[i];
        st->setTreeSectionSize(
            m_project->getFileName(), lib->getName().c_str(), 0,
            objTreeView->header()->sectionSize(0));
        st->setTreeSectionSize(
            m_project->getFileName(), lib->getName().c_str(), 1,
            objTreeView->header()->sectionSize(1));
    }
}

void ObjectManipulator::loadExpandedTreeItems()
{
    for (int i=0; i<m_objectManipulator->libs->count(); i++)
    {
        ObjectTreeView *objTreeView =
            dynamic_cast<ObjectTreeView*>(idxToTrees[i]);
        FWObject *lib = idxToLibs[i];
        set<int> expanded_objects;
        st->getExpandedObjectIds(m_project->getFileName(),
                                 lib->getName().c_str(),
                                 expanded_objects);
        objTreeView->ExpandTreeItems(expanded_objects);
        // there is no need to resize columns because call to
        //loadExpandedTreeItems is usually followed by the call to loadSectionSizes
        //objTreeView->header()->resizeSections(QHeaderView::ResizeToContents);
    }
}

void ObjectManipulator::saveExpandedTreeItems()
{
    for (int i=0; i<m_objectManipulator->libs->count(); i++)
    {
        ObjectTreeView *objTreeView =
            dynamic_cast<ObjectTreeView*>(idxToTrees[i]);
        FWObject *lib = idxToLibs[i];
        st->setExpandedObjectIds(m_project->getFileName(),
                                 lib->getName().c_str(),
                                 objTreeView->getListOfExpandedObjectIds());
    }
}

void ObjectManipulator::setAttributesColumnEnabled(bool f)
{
    for (int i=0; i<m_objectManipulator->libs->count(); i++)
    {
        ObjectTreeView *objTreeView =
            dynamic_cast<ObjectTreeView*>(idxToTrees[i]);
        objTreeView->showOrHideAttributesColumn();
    }
}

/*
 * This method tries to guess appropriate interface type and some other
 * attributes for subinterfaces.
 */
void ObjectManipulator::guessSubInterfaceTypeAndAttributes(Interface *intf)
{
    Interface *parent_intf = Interface::cast(intf->getParent());

    if (parent_intf == NULL) return;

    FWObject *f = intf->getParentHost();

    Resources* os_res = Resources::os_res[f->getStr("host_OS")];
    string os_family = f->getStr("host_OS");
    if (os_res!=NULL)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    interfaceProperties *int_prop =
        interfacePropertiesObjectFactory::getInterfacePropertiesObject(
            os_family);
    QString err;
    if (int_prop->looksLikeVlanInterface(intf->getName().c_str()) &&
        int_prop->isValidVlanInterfaceName(intf->getName().c_str(),
                                           intf->getParent()->getName().c_str(),
                                           err)
    )
    {
        InterfaceData *idata = new InterfaceData(*intf);
        int_prop->parseVlan(idata);
        if (!idata->interface_type.empty())
        {
            intf->getOptionsObject()->setStr("type", idata->interface_type);
            if (idata->interface_type == "8021q")
                intf->getOptionsObject()->setInt("vlan_id", idata->vlan_id);
        }
        delete idata;
    } else
    {
        if (parent_intf->getOptionsObject()->getStr("type") == "bridge")
        {
            intf->getOptionsObject()->setStr("type", "ethernet");
        }

        if (parent_intf->getOptionsObject()->getStr("type") == "bonding")
        {
            intf->getOptionsObject()->setStr("type", "ethernet");
            intf->setUnnumbered(true);
        }
    }

    delete int_prop;
}

