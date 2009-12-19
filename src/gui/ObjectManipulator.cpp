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
#include "utils_no_qt.h"
#include "platforms.h"
#include "events.h"


#include "ProjectPanel.h"
#include "ObjectManipulator.h"
#include "ObjectEditor.h"
#include "ObjectTreeViewItem.h"
#include "ObjectTreeView.h"
#include "FWObjectClipboard.h"
#include "FWObjectPropertiesFactory.h"
#include "FWBSettings.h"
#include "findDialog.h"
#include "newGroupDialog.h"
#include "FindObjectWidget.h"
#include "AskLibForCopyDialog.h"
#include "FindWhereUsedWidget.h"
#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"
#include "FWBTree.h"
#include "FWWindow.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/TagService.h"

#include <QMessageBox>
#include <QTime>
#include <QtDebug>
#include <QHeaderView>
#include <QUndoStack>
#include <QMenu>
#include <QAction>

#include <memory>
#include <algorithm>


using namespace std;
using namespace libfwbuilder;


HistoryItem::~HistoryItem() {}

ObjectManipulator::~ObjectManipulator()
{
    delete m_objectManipulator;
}

ObjectManipulator::ObjectManipulator(QWidget *parent):
    QWidget(parent), current_tree_view(0)
{
    m_objectManipulator = new Ui::ObjectManipulator_q;
    m_objectManipulator->setupUi(this);
    setObjectName(tr("Object Manipulator"));

    m_project = NULL;

    treeWidth    = -1;
    treeHeight   = -1;
    active       = false;
    current_tree_view=NULL;
    previous_lib_index = -1;

    // used in duplicateWithDependencies()
    dedup_marker_global_counter = time(NULL);

    buildNewObjectMenu();

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
            addLib( dobj );
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

int ObjectManipulator::getIdxForLib(FWObject* lib)
{
    if (fwbdebug)
        qDebug() << "ObjectManipulator::getIdxForLib"
                 << "lib=" << lib->getName().c_str();

    for (int i=0; i<m_objectManipulator->libs->count(); i++)
    {
        if ( idxToLibs[i]->getId() == lib->getId() )
        {
            if (fwbdebug) qDebug() << "Index=" << i;
            return i;
        }
    }
    if (fwbdebug) qDebug() << "Library not found";

    return -1;
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
        QString obj_name = QString::fromUtf8(obj->getName().c_str());
        FWObject *fw = target;
        while (fw && !Firewall::cast(fw)) fw = fw->getParent();

        std::auto_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(fw));

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
        openObjectInTree( otvi, true );

    //if (currentObj==NULL)  currentObj=otvi->getFWObject();
    FWObject *currentObj = getSelectedObject();

    popup_menu->clear();

    QAction *edtID =
        popup_menu->addAction(tr("Edit"), this, SLOT( editSelectedObject()));

    if (RuleSet::cast(currentObj))
        popup_menu->addAction(tr("Open"), this, SLOT( openSelectedRuleSet()));

    QMenu *duptargets  = NULL;
    QAction *dupID = NULL;
    QMenu *movetargets = NULL;
    int moveTargetsCounter = 0;

    if (!Interface::isA(currentObj) && RuleSet::cast(currentObj)==NULL &&
        !Library::isA(currentObj) && !FWBTree().isStandardFolder(currentObj))
    {
        duptargets = popup_menu->addMenu( tr("Duplicate ...") );
        movetargets = popup_menu->addMenu( tr("Move ...") );

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

    popup_menu->addSeparator();

    QAction *copyID = popup_menu->addAction(tr("Copy"), this, SLOT(copyObj()));
    QAction *cutID = popup_menu->addAction(tr("Cut"), this, SLOT(cutObj()));
    QAction *pasteID = popup_menu->addAction(tr("Paste"), this, SLOT(pasteObj()));

    popup_menu->addSeparator();

    QAction * delID = popup_menu->addAction( tr("Delete"), this,
                                        SLOT( delObj() ) );

    popup_menu->addSeparator();

    QList<QAction*> AddObjectActions;

    if (getCurrentObjectTree()->getNumSelected()==1)
    {

        if ( (Firewall::isA(currentObj) || Host::isA(currentObj)) &&
             ! currentObj->isReadOnly() )
        {
            AddObjectActions.append(addNewObjectMenuItem(popup_menu, Interface::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("Add Interface"), this,
            //                                           SLOT( newInterface() ) ));

        }
        if ((Firewall::isA(currentObj) || Cluster::isA(currentObj)) &&
             ! currentObj->isReadOnly())
        {
            AddObjectActions.append(addNewObjectMenuItem(popup_menu, Policy::TYPENAME));
            AddObjectActions.append(addNewObjectMenuItem(popup_menu, NAT::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("Add Policy Rule Set"), this,
            //                                           SLOT( newPolicyRuleSet() ) ));
            // AddObjectActions.append(popup_menu->addAction( tr("Add NAT Rule Set"), this,
            //                                           SLOT( newNATRuleSet() ) ));
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
                    AddObjectActions.append(addNewObjectMenuItem(popup_menu, Interface::TYPENAME));
                    // popup_menu->addAction(
                    //     tr("Add Interface"), this, SLOT( newInterface() ) );
            }

            AddObjectActions.append(addNewObjectMenuItem(popup_menu, IPv4::TYPENAME));
            AddObjectActions.append(addNewObjectMenuItem(popup_menu, IPv6::TYPENAME));
            AddObjectActions.append(addNewObjectMenuItem(popup_menu, physAddress::TYPENAME));

            // AddObjectActions.append(popup_menu->addAction( tr("Add IP Address"), this,
            //                                           SLOT( newInterfaceAddress() ) ));
            // AddObjectActions.append(popup_menu->addAction( tr("Add IPv6 Address"), this,
            //                                           SLOT( newInterfaceAddressIPv6() ) ));
            // AddObjectActions.append(popup_menu->addAction( tr("Add MAC Address"), this,
            //                                           SLOT( newPhysicalAddress() ) ));


            // Check if we should add menu item that creates failover
            // group. if parent is a cluster, allow one vrrp type
            // FailoverClusterGroup per Interface only
            FWObject *parent = NULL;
            parent = currentObj->getParent();
            if (parent != NULL && Cluster::isA(parent))
            {
                QAction *failover_menu_id = addNewObjectMenuItem(
                    popup_menu, FailoverClusterGroup::TYPENAME);
                // QAction *failover_menu_id = popup_menu->addAction(
                //     tr("Add Failover Group"), this,
                //     SLOT( newFailoverClusterGroup() ) );
                failover_menu_id->setEnabled(
                    currentObj->getFirstByType(FailoverClusterGroup::TYPENAME) == NULL);
            }
        }

        if (Cluster::isA(currentObj) && ! currentObj->isReadOnly())
        {
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, Interface::TYPENAME, "Add cluster interface"));
            // AddObjectActions.append(popup_menu->addAction( tr("Add Cluster interface"), this,
            //                                           SLOT( newClusterIface() ) ));

            // allow multiple state syncing groups per cluster
            // Rationale: these groups may represent different state syncing
            // protocols that can synchronize different things.
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, StateSyncClusterGroup::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("Add State Synchronization Group"), this,
            //                                           SLOT( newStateSyncClusterGroup() ) ));
        }

        if (currentObj->getPath(true)=="Firewalls")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, Firewall::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New Firewall"), this,
            //                                           SLOT( newFirewall() ) ));

        if (currentObj->getPath(true)=="Clusters")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, Cluster::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New Cluster"), this,
            //                                           SLOT( newCluster() ) ));

        if (currentObj->getPath(true)=="Objects/Addresses")
        {
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, IPv4::TYPENAME));
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, IPv6::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New Address"), this,
            //                                           SLOT( newAddress() ) ));
            // AddObjectActions.append(popup_menu->addAction( tr("New Address IPv6"), this,
            //                                           SLOT( newAddressIPv6() ) ));
        }

        if (currentObj->getPath(true)=="Objects/DNS Names")
        {
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, DNSName::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New DNS Name"), this,
            //                                           SLOT( newDNSName() ) ));
        }

        if (currentObj->getPath(true)=="Objects/Address Tables")
        {
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, AddressTable::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New Address Table"), this,
            //                                           SLOT( newAddressTable() ) ));
        }

        if (currentObj->getPath(true)=="Objects/Address Ranges")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, AddressRange::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New Address Range"), this,
            //                                           SLOT( newAddressRange() ) ));

        if (currentObj->getPath(true)=="Objects/Hosts")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, Host::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New Host"), this,
            //                                           SLOT( newHost() ) ));

        if (currentObj->getPath(true)=="Objects/Networks")
        {
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, Network::TYPENAME));
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, NetworkIPv6::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New Network"), this,
            //                                           SLOT( newNetwork() ) ));
            // AddObjectActions.append(popup_menu->addAction( tr("New Network IPv6"), this,
            //                                           SLOT( newNetworkIPv6() ) ));
        }

        if (currentObj->getPath(true)=="Objects/Groups")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, ObjectGroup::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New Group"), this,
            //                                           SLOT( newObjectGroup() ) ));

        if (currentObj->getPath(true)=="Services/Custom")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, CustomService::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New Custom Service"),this,
            //                                           SLOT( newCustom() ) ));

        if (currentObj->getPath(true)=="Services/IP")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, IPService::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New IP Service"), this,
            //                                           SLOT( newIP() ) ));

        if (currentObj->getPath(true)=="Services/ICMP")
        {
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, ICMPService::TYPENAME));
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, ICMP6Service::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New ICMP Service"), this,
            //                                           SLOT( newICMP() ) ));
            // AddObjectActions.append(popup_menu->addAction( tr("New ICMP6 Service"), this,
            //                                           SLOT( newICMP6() ) ));
        }

        if (currentObj->getPath(true)=="Services/TCP")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, TCPService::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New TCP Service"), this,
            //                                           SLOT( newTCP() ) ));

        if (currentObj->getPath(true)=="Services/UDP")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, UDPService::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New UDP Service"), this,
            //                                           SLOT( newUDP() ) ));

        if (currentObj->getPath(true)=="Services/TagServices")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, TagService::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New TagService"), this,
            //                                           SLOT( newTagService() ) ));

        if (currentObj->getPath(true)=="Services/Groups")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, ServiceGroup::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New Group"), this,
            //                                           SLOT( newServiceGroup() ) ));

        if (currentObj->getPath(true)=="Services/Users")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, UserService::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction(tr("New User Service"), this,
            //                                          SLOT(newUserService() )));

        if (currentObj->getPath(true)=="Time")
            AddObjectActions.append(
                addNewObjectMenuItem(popup_menu, Interval::TYPENAME));
            // AddObjectActions.append(popup_menu->addAction( tr("New Time Interval"), this,
            //                                           SLOT( newInterval() ) ));

        popup_menu->addSeparator();
        popup_menu->addAction( tr("Find"), this, SLOT( findObject()));

        popup_menu->addAction( tr("Where used"), this, SLOT( findWhereUsedSlot()));
    }

    popup_menu->addSeparator();
    popup_menu->addAction( tr("Group"), this, SLOT( groupObjects() ) )
            ->setDisabled(getCurrentObjectTree()->getNumSelected()==1);

    if (Firewall::cast(currentObj)!=NULL ||
        (ObjectGroup::cast(currentObj)!=NULL &&
         currentObj->getName()=="Firewalls"))
    {
        bool canCreateCluster = true;
        if (getCurrentObjectTree()->getNumSelected() > 1)
        {
            foreach( FWObject *obj, getCurrentObjectTree()->getSelectedObjects())
            {
                if (!Firewall::isA(obj))
                {
                    canCreateCluster = false;
                    break;
                }
            }
        } else canCreateCluster = false;
        popup_menu->addAction( tr("New cluster from selected firewalls"),
                               this, SLOT( newClusterFromSelected() ) )->setEnabled(canCreateCluster);
        popup_menu->addSeparator();
        popup_menu->addAction( tr("Compile"), this, SLOT( compile()));
        popup_menu->addAction( tr("Install"), this, SLOT( install()));

        if (Firewall::cast(currentObj)!=NULL)
        {
            Resources* os_res = Resources::os_res[currentObj->getStr("host_OS")];
            if (os_res)
            {
                string transfer = os_res->getTransferAgent();
                if (!transfer.empty())
                    popup_menu->addAction( tr("Transfer"), this, SLOT(transferfw()));
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
                popup_menu->addAction( tr("Transfer"), this, SLOT(transferfw()));
        }
//        popup_menu->addSeparator();
//        popup_menu->addAction( tr("Simulate install"), this, SLOT( simulateInstall()));
    }

    popup_menu->addSeparator();
    QAction* lcID=popup_menu->addAction( tr("Lock"), this,
                       SLOT( lockObject() ) );
    QAction* unlcID=popup_menu->addAction( tr("Unlock"), this,
                       SLOT( unlockObject() ) );
    lcID->setEnabled(getCurrentObjectTree()->isLockable());
    unlcID->setEnabled(getCurrentObjectTree()->isUnlockable());

    if (fwbdebug)
    {
/* keep this for debugging  */
        popup_menu->addSeparator();
        popup_menu->addAction( tr("dump"), this, SLOT( dumpObj()));
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

    popup_menu->exec( objTreeView->mapToGlobal( pos ) );
}

bool ObjectManipulator::getDeleteMenuState(FWObject *obj)
{
    QString objPath = obj->getPath(true).c_str();
    bool del_menu_item_state = FWBTree().getDeleteMenuState(objPath);

    // can't delete last policy, nat and routing child objects
    // also can't delete "top" policy ruleset
    if (del_menu_item_state && RuleSet::cast(obj))
    {
        //if (dynamic_cast<RuleSet*>(obj)->isTop()) del_menu_item_state = false;
        //else
        //{
            FWObject *fw = obj->getParent();
            // fw can be NULL if this ruleset is in the Deleted objects
            // library
            if (fw==NULL) return del_menu_item_state;
            list<FWObject*> child_objects = fw->getByType(obj->getTypeName());
            if (child_objects.size()==1) del_menu_item_state = false;
        //}
    }
    return del_menu_item_state;
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

        copyMenuItem = copyMenuItem && FWBTree().getCopyMenuState(object_path);
        pasteMenuItem = pasteMenuItem &&
            FWBTree().getPasteMenuState(object_path) &&
            (FWObjectClipboard::obj_clipboard->size()!=0);
        delMenuItem = delMenuItem && getDeleteMenuState(obj);
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
        std::auto_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(parent_fw));

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

void ObjectManipulator::editSelectedObject()
{
    if (fwbdebug) qDebug("ObjectManipulator::editSelectedObject");

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj = getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;

    if (RuleSet::cast(obj)!=NULL && m_project->getCurrentRuleSet()!=obj)
        QCoreApplication::postEvent(
            m_project, new openRulesetEvent(m_project->getFileName(), obj->getId()));

    QCoreApplication::postEvent(
        mw, new openObjectInEditorEvent(m_project->getFileName(), obj->getId()));
}

void ObjectManipulator::openSelectedRuleSet()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *obj = getCurrentObjectTree()->getSelectedObjects().front();
    if (obj==NULL) return;

    if (RuleSet::cast(obj)!=NULL && m_project->getCurrentRuleSet()!=obj)
        QCoreApplication::postEvent(
            m_project, new openRulesetEvent(m_project->getFileName(), obj->getId()));
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

    if (!mw->requestEditorOwnership(this, obj, ObjectEditor::optNone, true))
    {
        if (fwbdebug) qDebug("Can not get editor panel ownership");
        return false;
    }

    if (fwbdebug) qDebug("Open object in editor");
    mw->openEditor(obj); // opens object in the editor
    //currentObj = obj;
    active = true;

    if (fwbdebug) qDebug("ObjectManipulator::switchObjectInEditor done");

    return true;      // successfully (re)opened obj in the editor
}

void ObjectManipulator::closeObject()
{
    //currentObj = NULL;
    if (mw->isEditorVisible()) mw->hideEditor();
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

    FWObject *o = obj;
    //if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    if (history.empty() || otvi!=history.top().item() )
    {
        mw->enableBackAction();
        history.push( HistoryItem(otvi, o->getId()) );
    }

    //currentObj = obj;

    active = true;

    update();

    if (fwbdebug) qDebug("ObjectManipulator::selectionChanged done");
}

void ObjectManipulator::openObjectInTree(ObjectTreeViewItem *otvi,
                                   bool register_in_history)
{
    openObjectInTree(otvi->getFWObject(),register_in_history);
}

/*
 * I could use default value for the parameter register_in_history,
 * but that caused problems when this method was used as a slot
 */
void ObjectManipulator::openObjectInTree(QTreeWidgetItem *item)
{
    ObjectTreeViewItem *otvi=dynamic_cast<ObjectTreeViewItem*>(item);
    openObjectInTree(otvi,true);
}

void ObjectManipulator::openObjectInTree(FWObject *obj)
{
    openObjectInTree(obj,true);
}

/* This method is called from the GroupObjectDialog when user double
 * clicks on the object in a group, so first we should check if this
 * object is shown in the tree and if not, find and open it.
 */
void ObjectManipulator::openObjectInTree(FWObject *obj, bool /*register_in_history*/)
{
    if (fwbdebug)
        qDebug() << "ObjectManipulator::openObjectInTree"
                 << "obj:" << ((obj)?obj->getName().c_str():"NULL")
                 << "id:" << ((obj)?obj->getId():0);

    if (obj==NULL) return;

    openLibForObject(obj);

    //raise();
    FWObject *o=obj;
    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    ObjectTreeViewItem *otvi = allItems[o];
    // this changes selection and thus calls slot slectionChanged
    showObjectInTree(otvi);

    if (fwbdebug)
        qDebug() << "ObjectManipulator::openObjectInTree"
                 << "libs->currentIndex="
                 << m_objectManipulator->libs->currentIndex();

    updateCreateObjectMenu(obj->getLibrary());

    if (fwbdebug) qDebug() << "ObjectManipulator::openObjectInTree: done";
}

void ObjectManipulator::openLibForObject(FWObject *obj)
{
    // if obj is Library, its getLibrary() method returns itself. If
    // this library has been deleted and is now in the Deleted Objects
    // library, getIdxForLib() is not going to find it.
    if (FWObjectDatabase::isA(obj->getParent()))
        m_objectManipulator->libs->setCurrentIndex(
            getIdxForLib(obj->getLibrary()));
    else
        m_objectManipulator->libs->setCurrentIndex(
            getIdxForLib(obj->getParent()->getLibrary()));
}

void ObjectManipulator::showObjectInTree(ObjectTreeViewItem *otvi)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::showObjectInTree");
    if (otvi==NULL) return;

    ObjectTreeView* otv = otvi->getTree();

    if (fwbdebug)
        qDebug() << "ObjectManipulator::showObjectInTree"
                 << "current_tree_view=" << current_tree_view
                 << "new_otv=" << otv;

//    otv->raise();
    m_objectManipulator->widgetStack->setCurrentWidget(otv);

    otvi->getTree()->clearSelection();
    otvi->getTree()->scrollToItem( otvi, QAbstractItemView::EnsureVisible );
    otvi->getTree()->setCurrentItem( otvi );
    otvi->setSelected( true );
    otvi->getTree()->setFocus(Qt::OtherFocusReason);
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
    previous_lib_index = ln;

    QTreeWidget *lv = idxToTrees[ln];
    assert(lv!=NULL);

    ObjectTreeViewItem *otvi = dynamic_cast<ObjectTreeViewItem*>(lv->currentItem());
    if (otvi == NULL)
    {
        if (lv->invisibleRootItem()->childCount() > 0)
            otvi = dynamic_cast<ObjectTreeViewItem*>(
                lv->invisibleRootItem()->child(0));
        else
            assert(FALSE);
    }
    //currentObj = otvi->getFWObject();
    showObjectInTree( otvi );

// Experiment: switching libraries does not open new lib in the editor
//    if (mw->isEditorVisible()) mw->openEditor(getSelectedObject());

    updateCreateObjectMenu( idxToLibs[ln] );
    return;
}

void ObjectManipulator::updateCreateObjectMenu(FWObject* lib)
{
    bool f =
        lib->getId()==FWObjectDatabase::STANDARD_LIB_ID ||
        lib->getId()==FWObjectDatabase::TEMPLATE_LIB_ID ||
        lib->getId()==FWObjectDatabase::DELETED_OBJECTS_ID  ||
        lib->isReadOnly();

    m_objectManipulator->newButton->setEnabled( !f );
    QAction *noa = (QAction*)(mw->findChild<QAction*>("newObjectAction"));
    noa->setEnabled( !f );
}

FWObject*  ObjectManipulator::getCurrentLib()
{
    int idx = m_objectManipulator->libs->currentIndex();
    if (idx == -1 ) return NULL;
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
    openObjectInTree(obj->getLibrary(),false);
}

void ObjectManipulator::newObject()
{
    m_objectManipulator->newButton->showMenu();
}

void ObjectManipulator::select()
{
    FWObject *currentObj = getSelectedObject();

    if (fwbdebug)
        qDebug() << "ObjectManipulator::select()"
                 << "currentObj=" << currentObj
                 << ((currentObj)?currentObj->getName().c_str():" [unknown] ");

    if (currentObj==NULL) return;

    m_objectManipulator->libs->setCurrentIndex(getIdxForLib(currentObj->getLibrary()));

    // TODO: I forget why do we need flag "active", check this.
    ObjectTreeViewItem *otvi = allItems[currentObj];
    if (otvi)
    {
        active = true;
    }

    if (fwbdebug) qDebug("ObjectManipulator::select() done");
}

void ObjectManipulator::unselect()
{
    FWObject *currentObj = getSelectedObject();
    if (currentObj==NULL) return;

    for (int i=0; i<m_objectManipulator->libs->count(); i++)
        idxToTrees[i]->clearSelection();

    active=false;
}

bool ObjectManipulator::isSelected()
{
    return active;
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
    QTreeWidgetItem *cur = getCurrentObjectTree()->currentItem();
    if (cur)
    {
        ObjectTreeViewItem* otvi = dynamic_cast<ObjectTreeViewItem*>(cur);
        return otvi->getFWObject();
    }
    return NULL;

    vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
    if (so.size() > 0) return so.front();
    return NULL;

//    return currentObj;
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

void ObjectManipulator::setAttributesColumnEnabled(bool)
{
    for (int i=0; i<m_objectManipulator->libs->count(); i++)
    {
        ObjectTreeView *objTreeView =
            dynamic_cast<ObjectTreeView*>(idxToTrees[i]);
        objTreeView->showOrHideAttributesColumn();
    }
}

void ObjectManipulator::findWhereUsedRecursively(libfwbuilder::FWObject *obj,
                              libfwbuilder::FWObject *top,
                              std::set<libfwbuilder::FWObject*> &resset)
{
    UsageResolver::findWhereUsedRecursively(obj, top, resset, this->m_project->db());
}

std::list<libfwbuilder::Firewall*> ObjectManipulator::findFirewallsForObject(
    libfwbuilder::FWObject *o)
{
    return UsageResolver::findFirewallsForObject(o, this->m_project->db());
}

