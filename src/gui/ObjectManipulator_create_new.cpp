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
#include "platforms.h"
#include "events.h"

#include "ObjectManipulator.h"
#include "ObjectTreeView.h"
#include "newFirewallDialog.h"
#include "newClusterDialog.h"
#include "newHostDialog.h"
#include "newGroupDialog.h"
#include "FWCmdChange.h"
#include "FWCmdAddObject.h"
#include "FWBTree.h"
#include "FWWindow.h"
#include "ProjectPanel.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWReference.h"
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
#include "fwbuilder/Management.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/UserService.h"

#include <QMessageBox>
#include <QtDebug>
#include <QMenu>
#include <QAction>



using namespace std;
using namespace libfwbuilder;

void ObjectManipulator::buildNewObjectMenu()
{
    QString icon_path=":/Icons/";

    popup_menu = new QMenu(this);

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


FWObject* ObjectManipulator::createObject(const QString &objType,
                                          const QString &objName,
                                          FWObject *copyFrom)
{
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

    FWCmdAddObject *cmd = new FWCmdAddObject(
        m_project, parent, nobj,
        QObject::tr("Create new %1").arg(objType));
    FWObject *new_state = cmd->getNewState();
    new_state->add(nobj);
    m_project->undoStack->push(cmd);
    m_project->db()->setDirty(true);
    return nobj;
}

void ObjectManipulator::newLibrary()
{
    FWObject *nlib = FWBTree().createNewLibrary(m_project->db()); //   m_project->createNewLibrary(m_project->db());
    addLib( nlib );
}

void ObjectManipulator::newPolicyRuleSet ()
{
    FWObject *currentObj = getSelectedObject();
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
    createObject(currentObj,Policy::TYPENAME,name);

    this->getCurrentObjectTree()->sortItems(0, Qt::AscendingOrder);
}

void ObjectManipulator::newNATRuleSet ()
{
    FWObject *currentObj = getSelectedObject();
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
    createObject(currentObj,NAT::TYPENAME,name);

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
        FWCmdAddObject *cmd = new FWCmdAddObject(
            m_project, parent, NULL, QObject::tr("Create new Firewall"));
        FWObject *new_state = cmd->getNewState();
        parent->remove(nfw, false);
        new_state->add(nfw);
        m_project->undoStack->push(cmd);
    }
}

void ObjectManipulator::newCluster(bool fromSelected)
{
    FWObject *parent = 
        FWBTree().getStandardSlotForObject(getCurrentLib(), Cluster::TYPENAME);
    assert(parent);
    ObjectTreeViewItem* parent_item = allItems[parent];
    assert(parent_item);

    newClusterDialog *ncd = new newClusterDialog(parent);
    if (mw->isEditorVisible())  mw->hideEditor();
    if (fromSelected)
    {
        qDebug() << "creating cluster from selected firewalls";
        ncd->setFirewallList(getCurrentObjectTree()->getSelectedObjects());
    }
    ncd->exec();
    FWObject *ncl = ncd->getNewCluster();
    delete ncd;

    if (ncl)
    {
        FWCmdAddObject *cmd = new FWCmdAddObject(
            m_project, parent, NULL, QObject::tr("Create new Cluster"));
        // newCluster dialog may create backup copies of member firewalls,
        // to see them in the tree need to reload it.
        cmd->setNeedTreeReload(true);
        FWObject *new_state = cmd->getNewState();
        parent->remove(ncl, false);
        new_state->add(ncl);
        m_project->undoStack->push(cmd);
    }
}


void ObjectManipulator::newClusterFromSelected()
{
    newCluster(true);
}

void ObjectManipulator::newClusterIface()
{
    FWObject *currentObj = getSelectedObject();
    if (currentObj->isReadOnly()) return;

    QString new_name = makeNameUnique(currentObj,
                                      findNewestInterfaceName(currentObj),
                                      Interface::TYPENAME);
    createObject(currentObj, Interface::TYPENAME, new_name);
}

/*
 * Creates new state sync group; this method is called by context menu item
 * associated with Cluster object.
 * By default assume conntrack protocol and set group type accordingly.
 */
void ObjectManipulator::newStateSyncClusterGroup()
{
    FWObject *currentObj = getSelectedObject();
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
}

/*
 * Creates new failover group; this method is called by context menu item
 * associated with Interface object if its parent is a Cluster object
 * By default assume VRRP protocol and set group type accordingly.
 */
void ObjectManipulator::newFailoverClusterGroup()
{
    FWObject *currentObj = getSelectedObject();
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
}

void ObjectManipulator::newHost()
{
    FWObject *parent = 
        FWBTree().getStandardSlotForObject(getCurrentLib(), Host::TYPENAME);
    assert(parent);
    newHostDialog *nhd = new newHostDialog(parent);
    if (mw->isEditorVisible()) mw->hideEditor();
    nhd->exec();
    FWObject *o = nhd->getNewHost();
    delete nhd;

    if (o!=NULL)
    {
        FWCmdAddObject *cmd = new FWCmdAddObject(
            m_project, parent, NULL, QObject::tr("Create new Host"));
        FWObject *new_state = cmd->getNewState();
        parent->remove(o, false);
        new_state->add(o);
        m_project->undoStack->push(cmd);
    }
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
    FWObject *currentObj = getSelectedObject();
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
    {
        interfaceProperties *int_prop =
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                new_interface->getParentHost());
        int_prop->guessSubInterfaceTypeAndAttributes(new_interface);
        delete int_prop;
        //guessSubInterfaceTypeAndAttributes(new_interface);
    } else
        new_interface->getOptionsObject()->setStr("type", "ethernet");
}

void ObjectManipulator::newNetwork()
{
    createObject(Network::TYPENAME,tr("Network"));
}
void ObjectManipulator::newNetworkIPv6()
{
    createObject(NetworkIPv6::TYPENAME,tr("Network IPv6"));
}

void ObjectManipulator::newAddress()
{
    FWObject *currentObj = getSelectedObject();
    if ( currentObj->isReadOnly() ) return;
    createObject(IPv4::TYPENAME, tr("Address"));
}

void ObjectManipulator::newAddressIPv6()
{
    FWObject *currentObj = getSelectedObject();
    if ( currentObj->isReadOnly() ) return;
    createObject(IPv6::TYPENAME,tr("Address IPv6"));
}

void ObjectManipulator::newDNSName()
{
    createObject(DNSName::TYPENAME,tr("DNS Name"));
}

void ObjectManipulator::newAddressTable()
{
    createObject(AddressTable::TYPENAME,tr("Address Table"));
}

void ObjectManipulator::newInterfaceAddress()
{
    FWObject *currentObj = getSelectedObject();
    if ( currentObj->isReadOnly() ) return;

    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf &&
            (intf->isDyn() || intf->isUnnumbered() || intf->isBridgePort())
        ) return;
        QString iname = getStandardName(currentObj, IPv4::TYPENAME, "ip");
        iname = makeNameUnique(currentObj, iname, IPv4::TYPENAME);
        createObject(currentObj, IPv4::TYPENAME, iname);
    }
}

void ObjectManipulator::newInterfaceAddressIPv6()
{
    FWObject *currentObj = getSelectedObject();
    if ( currentObj->isReadOnly() ) return;

    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf &&
            (intf->isDyn() || intf->isUnnumbered() || intf->isBridgePort())
        ) return;
        QString iname = getStandardName(currentObj, IPv4::TYPENAME, "ipv6");
        iname = makeNameUnique(currentObj, iname, IPv4::TYPENAME);
        createObject(currentObj, IPv6::TYPENAME, iname);
    }
}

void ObjectManipulator::newTagService()
{
    createObject(TagService::TYPENAME,tr("TagService"));
}

void ObjectManipulator::newUserService()
{
    createObject(UserService::TYPENAME,tr("User Service"));
}

void ObjectManipulator::newPhysicalAddress()
{
    FWObject *currentObj = getSelectedObject();
    if ( currentObj->isReadOnly() ) return;

    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf->getByType(physAddress::TYPENAME).empty())
        {
            QString iname=QString("%1:%2:mac")
                .arg(QString::fromUtf8(currentObj->getParent()->getName().c_str()))
                .arg(QString::fromUtf8(currentObj->getName().c_str()));
            createObject(currentObj,physAddress::TYPENAME,iname);
        }
    }
}

void ObjectManipulator::newAddressRange()
{
    createObject(AddressRange::TYPENAME,tr("Address Range"));
}

void ObjectManipulator::newObjectGroup()
{
    createObject(ObjectGroup::TYPENAME,tr("Object Group"));
}


void ObjectManipulator::newCustom()
{
    createObject(CustomService::TYPENAME,tr("Custom Service"));
}

void ObjectManipulator::newIP()
{
    createObject(IPService::TYPENAME,tr("IP Service"));
}

void ObjectManipulator::newICMP()
{
    createObject(ICMPService::TYPENAME,tr("ICMP Service"));
}

void ObjectManipulator::newICMP6()
{
    createObject(ICMP6Service::TYPENAME,tr("ICMP6 Service"));
}

void ObjectManipulator::newTCP()
{
    createObject(TCPService::TYPENAME,tr("TCP Service"));
}

void ObjectManipulator::newUDP()
{
    createObject(UDPService::TYPENAME,tr("UDP Service"));
}

void ObjectManipulator::newServiceGroup()
{
    createObject(ServiceGroup::TYPENAME,tr("Service Group"));
}

void ObjectManipulator::newInterval()
{
    createObject(Interval::TYPENAME,tr("Time Interval"));
}

