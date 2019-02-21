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



#include "global.h"
#include "platforms.h"
#include "events.h"

#include "listOfLibrariesModel.h"
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
#include "fwbuilder/AttachedNetworks.h"
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
    QMenu* newObjectPopup = new QMenu( mw );

    newObjectPopup->setObjectName("newObjectPopup");

    addNewObjectMenuItem(newObjectPopup, Library::TYPENAME, tr( "New &Library"));

    newObjectPopup->addSeparator();

    foreach (const char *type, FWBTree::getObjectTypes()) {
        addNewObjectMenuItem(newObjectPopup, type);
    }

    newObjectPopup->addSeparator();

    foreach (const char *type, FWBTree::getServiceTypes()) {
        addNewObjectMenuItem(newObjectPopup, type);
    }

    newObjectPopup->addSeparator();

    addNewObjectMenuItem(newObjectPopup, Interval::TYPENAME);

    mw->addNewObjectMenu(newObjectPopup);
    mw->showNewObjectMenu();
}

QAction* ObjectManipulator::addNewObjectMenuItem(QMenu *menu,
                                                 const char* type_name,
                                                 const QString &text,
                                                 int add_to_group_id)
{
    QString icon_path=":/Icons/";
    QAction *act;
    QString menu_item_text = text;
    if (menu_item_text.isEmpty())
        menu_item_text = FWBTree().getTranslatableNewObjectMenuText(type_name);

    act = menu->addAction(QIcon(icon_path + QString(type_name) + "/icon-tree"),
                          menu_item_text, this, SLOT( createNewObject() ));
    act->setObjectName(QString("newObject_") + type_name);
    QMap<QString, QVariant> d;
    d["type_name"] = QVariant(QString(type_name));
    d["add_to_group"] = QVariant(add_to_group_id);
    act->setData(QVariant(d));
    return act;
}

void ObjectManipulator::createNewObject()
{
    const QAction *action = dynamic_cast<const QAction*>(sender());
    assert(action!=nullptr);
    QVariant v = action->data();
    if (!v.isValid()) return;

    QMap<QString, QVariant> d =  v.value<QMap<QString, QVariant> >();

    QVariant v1 = d["type_name"];
    QString type_name = v1.value<QString>();

    QVariant v2 = d["add_to_group"];
    int add_to_group_id = v2.value<int>();

    if (fwbdebug)
        qDebug() << "ObjectManipulator::createNewObject()"
                 << "type:" << type_name
                 << "add_to_group_id:" << add_to_group_id;
    FWObject *new_obj = nullptr;

    if (!isObjectAllowed(type_name)) return;

    QString descr = FWBTree().getTranslatableObjectTypeName(type_name);
    // FWCmdMacro should be used for commands grouping
    FWCmdMacro* macro = nullptr;
    if (add_to_group_id == -1)
        macro = new FWCmdMacro(
            FWBTree().getTranslatableNewObjectMenuText(type_name));
    else
        macro = new FWCmdMacro(tr("Create and add to group"));


    if (type_name ==  Firewall::TYPENAME ||
        type_name ==  Cluster::TYPENAME ||
        type_name ==  Host::TYPENAME)
    {
        // These three functions call separate modal dialogs that can
        // be cancelled by the user
        if (type_name ==  Firewall::TYPENAME) new_obj = newFirewall(macro);
        if (type_name ==  Cluster::TYPENAME) new_obj = newCluster(macro);
        if (type_name ==  Host::TYPENAME) new_obj = newHost(macro);
        if (new_obj == nullptr)
        {
            delete macro;
            return;
        } 
    }

    if (type_name ==  Library::TYPENAME) new_obj = newLibrary(macro);
    if (type_name ==  Interface::TYPENAME) new_obj = newInterface(macro);
    if (type_name ==  IPv4::TYPENAME) new_obj = newInterfaceAddress(macro);
    if (type_name ==  IPv6::TYPENAME) new_obj = newInterfaceAddressIPv6(macro);
    if (type_name ==  physAddress::TYPENAME) new_obj = newPhysicalAddress(macro);
    if (type_name ==  FailoverClusterGroup::TYPENAME) new_obj = newFailoverClusterGroup(macro);
    if (type_name ==  StateSyncClusterGroup::TYPENAME) new_obj = newStateSyncClusterGroup(macro);
    if (type_name ==  Policy::TYPENAME) new_obj = newPolicyRuleSet(macro);
    if (type_name ==  NAT::TYPENAME) new_obj = newNATRuleSet(macro);
    //if (type_name ==  Routing::TYPENAME) new_obj = newRoutingRuleSet();
    if (type_name ==  AttachedNetworks::TYPENAME) new_obj = newAttachedNetworks(macro);

    if (new_obj == nullptr) new_obj = createObject(type_name, descr, nullptr, macro);

    if (new_obj == nullptr)
    {
        delete macro;
        return;
    }

    if (add_to_group_id != -1)
    {
        FWObject *grp = m_project->db()->findInIndex(add_to_group_id);

        if (fwbdebug)
            qDebug() << "ObjectManipulator::createNewObject()"
                     << "Adding to group grp=" << grp;

        if (grp)
        {
            FWCmdChange *cmd = new FWCmdChange(
                m_project, grp, QObject::tr("Add object to group"), false, macro);
            FWObject *new_state = cmd->getNewState();
            new_state->addRef(new_obj);

            // if we add new object to a group, we should still open
            // the object in the editor rather than the group. Command
            // that adds it to the group opens the group though. Send
            // event to open the object.

            QCoreApplication::postEvent(
                mw, new openObjectInEditorEvent(
                    m_project->getFileName(), new_obj->getId()));
        }
    }

    QCoreApplication::postEvent(
        m_project, new expandObjectInTreeEvent(
            m_project->getFileName(), new_obj->getId()));

    if (Firewall::cast(new_obj)!=nullptr)  // Cluster too
    {
        FWObject *ruleset = new_obj->getFirstByType(Policy::TYPENAME);
        if (ruleset)
            QCoreApplication::postEvent(
                m_project, new openRulesetEvent(
                    m_project->getFileName(), ruleset->getId()));
    }

    //directly move object to it's subfolder
    list<FWObject*> newObjs;
    newObjs.push_back(new_obj);
    moveItems(lastClickedItem, newObjs);
    lastClickedItem = nullptr;

    m_project->undoStack->push(macro);
}

void ObjectManipulator::newFirewallSlot()
{
    QString descr = FWBTree().getTranslatableObjectTypeName(Firewall::TYPENAME);
    // FWCmdMacro should be used for commands grouping
    FWCmdMacro* macro = nullptr;
    macro = new FWCmdMacro(
        FWBTree().getTranslatableNewObjectMenuText(Firewall::TYPENAME));

    FWObject *new_obj = newFirewall(macro);

    if (new_obj == nullptr)
    {
        delete macro;
        return;
    }

    QCoreApplication::postEvent(
        m_project, new expandObjectInTreeEvent(
            m_project->getFileName(), new_obj->getId()));

    FWObject *ruleset = new_obj->getFirstByType(Policy::TYPENAME);
    if (ruleset)
        QCoreApplication::postEvent(
            m_project, new openRulesetEvent(
                m_project->getFileName(), ruleset->getId()));

    m_project->undoStack->push(macro);
}

FWObject* ObjectManipulator::createObject(const QString &objType,
                                          const QString &objName,
                                          FWObject *copyFrom,
                                          QUndoCommand* macro)
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

    while ( lib == nullptr ||
            lib->getId()==FWObjectDatabase::STANDARD_LIB_ID ||
            lib->getId()==FWObjectDatabase::TEMPLATE_LIB_ID ||
            lib->getId()==FWObjectDatabase::DELETED_OBJECTS_ID  ||
            lib->isReadOnly() )
    {
        if (i>=m_objectManipulator->libs->count())
        {
//            if (fwbdebug)
//                qDebug("ObjectManipulator::createObject   return nullptr");
//            return nullptr;
            lib  = getCurrentLib();
            break;
        }

//        lib = idxToLibs[i];

        lib = libs_model->getLibrary(i);

        if (fwbdebug)
        {
            qDebug("i=%d",i);
            qDebug("lib: %s %s",
                   lib->getName().c_str(),
                   FWObjectDatabase::getStringId(lib->getId()).c_str());
        }
        i++;
    }

    FWObject *parent = FWBTree().getStandardSlotForObject(lib, objType);

    if (parent==nullptr)
    {
      QMessageBox::warning(this,"Firewall Builder",
                           QObject::tr(
"Type '%1': new object can not be created because\n"
"corresponding branch is missing in the object tree.\n"
"Please repair the tree using command 'fwbedit checktree -f file.fwb'.")
                           .arg(objType),
                           "&Continue", QString::null, QString::null,
                           0, 1 );
      return nullptr;
    }

    return actuallyCreateObject(parent, objType, objName, copyFrom, macro);
}

FWObject* ObjectManipulator::createObject(FWObject *parent,
                                          const QString &objType,
                                          const QString &objName,
                                          FWObject *copyFrom,
                                          QUndoCommand* macro)
{
    FWObject *lib  = getCurrentLib();
    int       i = 0;

    assert(parent!=nullptr);

    if (fwbdebug)
    {
        qDebug("ObjectManipulator::createObject 2: parent=%s",
               parent->getName().c_str());
        qDebug("ObjectManipulator::createObject 2: objType=%s  objName=%s",
               objType.toLatin1().constData(), objName.toLatin1().constData());
    }

    while ( lib == nullptr ||
            lib->getId()==FWObjectDatabase::STANDARD_LIB_ID ||
            lib->getId()==FWObjectDatabase::TEMPLATE_LIB_ID ||
            lib->getId()==FWObjectDatabase::DELETED_OBJECTS_ID  ||
            lib->isReadOnly() )
    {
        if (i >= m_objectManipulator->libs->count())
        {
            lib=getCurrentLib();
            break;
        }
//        lib = idxToLibs[i];

        lib = libs_model->getLibrary(i);

        i++;
    }

    if (parent==nullptr) parent=lib;

    return actuallyCreateObject(parent, objType, objName, copyFrom, macro);
}

FWObject* ObjectManipulator::actuallyCreateObject(FWObject *parent,
                                                  const QString &objType,
                                                  const QString &objName,
                                                  FWObject *copyFrom,
                                                  QUndoCommand* macro)
{
    FWObject *nobj=nullptr;
    if (!isTreeReadWrite(this, parent)) return nullptr;
    nobj = m_project->db()->create(objType.toLatin1().constData());
    assert(nobj!=nullptr);
    if (copyFrom!=nullptr) nobj->duplicate(copyFrom, true);
    if (nobj->isReadOnly()) nobj->setReadOnly(false);

    QString new_name = makeNameUnique(parent, objName, objType);

    nobj->setName( string(new_name.toUtf8().constData()) );

    if (objType == DNSName::TYPENAME)
    {
        if (st->getBool("Objects/DNSName/useCompileTimeForNewObjects"))
            DNSName::cast(nobj)->setRunTime(false);
        else
            DNSName::cast(nobj)->setRunTime(true);
        if (st->getBool("Objects/DNSName/useNameForDNSRecord"))
            DNSName::cast(nobj)->setSourceName(nobj->getName());
    }

    if (objType == AddressTable::TYPENAME)
    {
        if (st->getBool("Objects/AddressTable/useCompileTimeForNewObjects"))
            AddressTable::cast(nobj)->setRunTime(false);
        else
            AddressTable::cast(nobj)->setRunTime(true);
    }

    FWCmdAddObject *cmd = new FWCmdAddObject(
        m_project, parent, nobj,
        QObject::tr("Create new %1").arg(objType), macro);
    FWObject *new_state = cmd->getNewState();
    new_state->add(nobj);
    if (!macro)
        m_project->undoStack->push(cmd);
    m_project->db()->setDirty(true);

    if (objType == IPService::TYPENAME ||
        objType == ICMPService::TYPENAME ||
        objType == UDPService::TYPENAME ||
        objType == TCPService::TYPENAME) reminderAboutStandardLib();

    return nobj;
}

FWObject* ObjectManipulator::newLibrary(QUndoCommand* macro)
{
    FWObject *nlib = FWBTree().createNewLibrary(m_project->db()); //   m_project->createNewLibrary(m_project->db());
    // At this point new library is already inserted into the object tree
    // but it has not been added to the QTreeWidget yet.
    FWCmdAddLibrary *cmd = new FWCmdAddLibrary(
        m_project, m_project->db(), nullptr, QObject::tr("Create library"), macro);
    FWObject *new_state = cmd->getNewState();
    m_project->db()->remove(nlib, false);
    new_state->add(nlib);

//    m_project->undoStack->push(cmd);

    m_project->db()->setDirty(true);

    return nlib;
}

FWObject* ObjectManipulator::newPolicyRuleSet(QUndoCommand* macro)
{
    FWObject *currentObj = getSelectedObject();
    if ( !currentObj || currentObj->isReadOnly() ) return nullptr;
    QString name = "Policy";
    Firewall * fw = Firewall::cast(currentObj);
    if (fw!=nullptr)
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
    FWObject *o = createObject(currentObj, Policy::TYPENAME, name, nullptr, macro);
    this->getCurrentObjectTree()->sortItems(0, Qt::AscendingOrder);
    return o;
}

FWObject* ObjectManipulator::newNATRuleSet(QUndoCommand* macro)
{
    FWObject *currentObj = getSelectedObject();
    if ( !currentObj || currentObj->isReadOnly() ) return nullptr;
    QString name = "NAT";
    Firewall * fw = Firewall::cast(currentObj);
    if (fw!=nullptr)
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
    FWObject *o = createObject(currentObj, NAT::TYPENAME, name, nullptr, macro);
    this->getCurrentObjectTree()->sortItems(0, Qt::AscendingOrder);
    return o;
}

FWObject* ObjectManipulator::newFirewall(QUndoCommand* macro)
{
    FWObject *parent =
        FWBTree().getStandardSlotForObject(getCurrentLib(), Firewall::TYPENAME);
    assert(parent);

#ifndef NDEBUG
    ObjectTreeViewItem* parent_item = allItems[parent];
    assert(parent_item);
#endif

    newFirewallDialog *nfd = new newFirewallDialog(this, parent);
    if (mw->isEditorVisible()) mw->hideEditor();
    nfd->setWindowModality(Qt::WindowModal);
    nfd->setWindowFlags(Qt::Window);
    nfd->exec();
    FWObject *nfw = nfd->getNewFirewall();
    delete nfd;

    if (nfw!=nullptr)
    {
        FWCmdAddObject *cmd = new FWCmdAddObject(
            m_project, parent, nullptr, QObject::tr("Create new Firewall"), macro);
        FWObject *new_state = cmd->getNewState();

        parent->remove(nfw, false);
        new_state->add(nfw);
    }

    return nfw;
}

FWObject* ObjectManipulator::newCluster(QUndoCommand* macro, bool fromSelected)
{
    FWObject *parent =
        FWBTree().getStandardSlotForObject(getCurrentLib(), Cluster::TYPENAME);
    assert(parent);

#ifndef NDEBUG
    ObjectTreeViewItem* parent_item = allItems[parent];
    assert(parent_item);
#endif

    newClusterDialog *ncd = new newClusterDialog(this, parent);
    if (mw->isEditorVisible())  mw->hideEditor();
    if (fromSelected)
    {
        if (fwbdebug)
            qDebug() << "ObjectManipulator::newCluster: creating cluster from selected firewalls";
        ncd->setFirewallList(getCurrentObjectTree()->getSelectedObjects(), true);
    }
    else
    {
        list<Firewall*> fwlist;
        mw->findAllFirewalls(fwlist);
        vector<FWObject*> fwvector;
        foreach(Firewall* fw, fwlist)
            fwvector.push_back(FWObject::cast(fw));
        ncd->setFirewallList(fwvector);
    }
    if ( ncd->exec() != QDialog::Accepted) return nullptr;

    FWObject *ncl = ncd->getNewCluster();
    delete ncd;

    if (ncl)
    {
        if (fwbdebug)
            qDebug() << "ObjectManipulator::newCluster checkpoint 1";

        FWCmdAddObject *cmd = new FWCmdAddObject(
            m_project, parent, nullptr, QObject::tr("Create new Cluster"), macro);
        // newCluster dialog may create backup copies of member firewalls,
        // to see them in the tree need to reload it.
        cmd->setNeedTreeReload(true);
        FWObject *new_state = cmd->getNewState();
        parent->remove(ncl, false);
        new_state->add(ncl);
        // if (macro)
        //     m_project->undoStack->push(cmd);

    }

    return ncl;
}


void ObjectManipulator::newClusterFromSelected()
{
    FWCmdMacro* macro = new FWCmdMacro(
        FWBTree().getTranslatableNewObjectMenuText(Cluster::TYPENAME));
    FWObject *ncl = newCluster(macro, true);
    if (ncl == nullptr)
    {
        delete macro;
        return;
    }
    m_project->undoStack->push(macro);
}

FWObject* ObjectManipulator::newClusterIface(QUndoCommand* macro)
{
    FWObject *currentObj = getSelectedObject();
    if ( !currentObj || currentObj->isReadOnly() ) return nullptr;
    QString new_name = makeNameUnique(currentObj,
                                      findNewestInterfaceName(currentObj),
                                      Interface::TYPENAME);
    return createObject(currentObj, Interface::TYPENAME, new_name, nullptr, macro);
}

/*
 * Creates new state sync group; this method is called by context menu item
 * associated with Cluster object.
 * By default assume conntrack protocol and set group type accordingly.
 */
FWObject* ObjectManipulator::newStateSyncClusterGroup(QUndoCommand* macro)
{
    FWObject *currentObj = getSelectedObject();
    if ( !currentObj || currentObj->isReadOnly() ) return nullptr;

    FWObject *o = nullptr;

    FWObject *cluster = currentObj;
    while (cluster && !Cluster::isA(cluster)) cluster = cluster->getParent();
    assert(cluster != nullptr);
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
        return nullptr;
    }

    QString group_type = lst.front().first;

    o = createObject(currentObj, StateSyncClusterGroup::TYPENAME,
                     tr("State Sync Group"), nullptr, macro);
    o->setStr("type", group_type.toStdString());
    return o;
}

/*
 * Creates new failover group; this method is called by context menu item
 * associated with Interface object if its parent is a Cluster object
 * By default assume VRRP protocol and set group type accordingly.
 */
FWObject* ObjectManipulator::newFailoverClusterGroup(QUndoCommand* macro)
{
    FWObject *currentObj = getSelectedObject();
    if ( !currentObj || currentObj->isReadOnly() ) return nullptr;

    FWObject *o = nullptr;

    QString group_type;
    if (Interface::isA(currentObj))
    {
        group_type = "vrrp";
    } else
    {
        qWarning("newClusterGroup: invalid currentObj");
        return nullptr;
    }

    o = createObject(currentObj, FailoverClusterGroup::TYPENAME,
                     tr("Failover group"), nullptr, macro);
    o->setStr("type", group_type.toStdString());
    return o;
}

/*
 * Creates new AttachedNetworks object; this method is called by
 * context menu item associated with Interface object
 */
FWObject* ObjectManipulator::newAttachedNetworks(QUndoCommand* macro)
{
    FWObject *currentObj = getSelectedObject();
    if ( !currentObj || currentObj->isReadOnly() ) return nullptr;

    if (Interface::isA(currentObj))
    {
        FWObject *no = createObject(currentObj, AttachedNetworks::TYPENAME,
                                    tr("Attached Networks"), nullptr, macro);
        FWObject *parent_host = Host::getParentHost(currentObj);
        string name = parent_host->getName() +
            ":" + currentObj->getName() + ":attached";
        no->setName(name);
        return no;
    } else
    {
        qWarning("newAttachedNetworks: invalid currentObj");
        return nullptr;
    }
}

FWObject* ObjectManipulator::newHost(QUndoCommand* macro)
{
    FWObject *parent =
        FWBTree().getStandardSlotForObject(getCurrentLib(), Host::TYPENAME);
    assert(parent);
    newHostDialog *nhd = new newHostDialog(this, parent);
    if (mw->isEditorVisible()) mw->hideEditor();
    nhd->exec();
    FWObject *o = nhd->getNewHost();
    delete nhd;

    if (o!=nullptr)
    {
        FWCmdAddObject *cmd = new FWCmdAddObject(
            m_project, parent, nullptr, QObject::tr("Create new Host"), macro);
        FWObject *new_state = cmd->getNewState();
        parent->remove(o, false);
        new_state->add(o);
    }
    return o;
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

FWObject* ObjectManipulator::newInterface(QUndoCommand* macro)
{
    FWObject *currentObj = getSelectedObject();
    if ( !currentObj || currentObj->isReadOnly() ) return nullptr;

    Interface *new_interface = nullptr;
    FWObject *parent = nullptr;

    // Note that Firewall::cast matches Firewall and Cluster
    if (Host::isA(currentObj) || Firewall::cast(currentObj))
        parent = currentObj;

    if (Interface::isA(currentObj))
    {
        FWObject *h = Host::getParentHost(currentObj);
        //FWObject *h = Interface::cast(currentObj)->getParentHost();

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

    if (parent == nullptr)
    {
        // since we can;t find quitable parent for the new interface,
        // we can't create it.
        return nullptr;
    }

    QString new_name = makeNameUnique(parent, findNewestInterfaceName(parent),
                                      Interface::TYPENAME);
    new_interface = Interface::cast(
        createObject(parent, Interface::TYPENAME, new_name, nullptr, macro));

    if (new_interface == nullptr) return nullptr;

    if (Interface::isA(parent))
    {
        FWObject *parent_host = Host::getParentHost(parent);
        interfaceProperties *int_prop =
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                parent_host);
        int_prop->guessSubInterfaceTypeAndAttributes(new_interface);
        delete int_prop;
        //guessSubInterfaceTypeAndAttributes(new_interface);
    } else
        new_interface->getOptionsObject()->setStr("type", "ethernet");

    return new_interface;
}

FWObject* ObjectManipulator::newInterfaceAddress(QUndoCommand* macro)
{
    FWObject *currentObj = getSelectedObject();
    if ( !currentObj || currentObj->isReadOnly() ) return nullptr;

    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf &&
            (intf->isDyn() || intf->isUnnumbered() || intf->isBridgePort())
        ) return nullptr;
        QString iname = getStandardName(currentObj, IPv4::TYPENAME, "ip");
        iname = makeNameUnique(currentObj, iname, IPv4::TYPENAME);
        return createObject(currentObj, IPv4::TYPENAME, iname, nullptr, macro);
    }
    // if current object is not interface, create address in the standard folder
    return createObject(IPv4::TYPENAME,
                        FWBTree().getTranslatableObjectTypeName(IPv4::TYPENAME),
                        nullptr, macro);
}

FWObject* ObjectManipulator::newInterfaceAddressIPv6(QUndoCommand* macro)
{
    FWObject *currentObj = getSelectedObject();
    if ( !currentObj || currentObj->isReadOnly() ) return nullptr;

    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf &&
            (intf->isDyn() || intf->isUnnumbered() || intf->isBridgePort())
        ) return nullptr;
        QString iname = getStandardName(currentObj, IPv4::TYPENAME, "ipv6");
        iname = makeNameUnique(currentObj, iname, IPv4::TYPENAME);
        return createObject(currentObj, IPv6::TYPENAME, iname, nullptr, macro);
    }
    // if current object is not interface, create address in the standard folder
    return createObject(IPv6::TYPENAME,
                        FWBTree().getTranslatableObjectTypeName(IPv6::TYPENAME),
                        nullptr, macro);
}

FWObject* ObjectManipulator::newPhysicalAddress(QUndoCommand* macro)
{
    FWObject *currentObj = getSelectedObject();
    if ( !currentObj || currentObj->isReadOnly() ) return nullptr;

    if (Interface::isA(currentObj))
    {
        Interface *intf = Interface::cast(currentObj);
        if (intf->getByType(physAddress::TYPENAME).empty())
        {
            QString iname=QString("%1:%2:mac")
                .arg(QString::fromUtf8(currentObj->getParent()->getName().c_str()))
                .arg(QString::fromUtf8(currentObj->getName().c_str()));
            return createObject(currentObj, physAddress::TYPENAME, iname,
                                nullptr, macro);
        }
    }
    return nullptr;
}

void ObjectManipulator::reminderAboutStandardLib()
{
    if (st->isReminderAboutStandardLibSuppressed()) return;

    st->suppressReminderAboutStandardLib(true);
    QMessageBox::information(
        this,"Firewall Builder",
        QObject::tr(
            "<html>"
            "Did you know that Firewall Builder comes with over a hunderd "
            "standard address and service objects that represent "
            "often used protocols and services? You can "
            "find them in the object library \"Standard\"."
            " <br/>"
            "<hr/>"
            " <br/>"
            "<img src=\":/Images/library_switch_screenshot.png\"/>"
            "</html>"
        ));
}


