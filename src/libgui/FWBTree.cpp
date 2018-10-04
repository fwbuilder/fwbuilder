/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include <QtDebug>
#include <QPixmap>
#include <QString>

#include "FWBTree.h"
#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/DynamicGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/FWObjectDatabase.h"
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
#include "fwbuilder/NAT.h"
#include "fwbuilder/AttachedNetworks.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/UserService.h"

#include <iostream>
#include <algorithm>
#include <memory>

#include "utils.h"

using namespace std;
using namespace libfwbuilder;


QStringList            FWBTree::standardFolders;
QSet<QString>          FWBTree::standardIDs;
QMap<QString,QString>  FWBTree::systemGroupTypes;
QMap<QString,QString>  FWBTree::systemGroupNames;
QMap<QString,QString>  FWBTree::systemGroupPaths;

QMap<QString,bool>     FWBTree::copyMenuState;
QMap<QString,bool>     FWBTree::cutMenuState;
QMap<QString,bool>     FWBTree::pasteMenuState;
QMap<QString,bool>     FWBTree::deleteMenuState;


FWBTree::FWBTree()
{
    init_statics();
}

void FWBTree::init_statics()
{
    if (systemGroupPaths.size() == 0)
    {
        // Names of the standard folders should be translatable.
        // Function isStandardFolder() takes this into account.

        standardFolders << "Objects";
        standardFolders << "Objects/Addresses";
        standardFolders << "Objects/DNS Names";
        standardFolders << "Objects/Address Tables";
        standardFolders << "Objects/Address Ranges";
        standardFolders << "Objects/Groups";
        standardFolders << "Objects/Hosts";
        standardFolders << "Objects/Networks";

        standardFolders << "Services";
        standardFolders << "Services/Custom";
        standardFolders << "Services/Groups";
        standardFolders << "Services/IP";
        standardFolders << "Services/ICMP";
        standardFolders << "Services/TCP";
        standardFolders << "Services/UDP";
        standardFolders << "Services/TagServices";
        standardFolders << "Services/Users";

        standardFolders << "Firewalls";
        standardFolders << "Clusters";
        standardFolders << "Time";


        systemGroupPaths[Library::TYPENAME]       = "";

        systemGroupPaths[IPv4::TYPENAME]          = "Objects/Addresses";
        systemGroupPaths[IPv6::TYPENAME]          = "Objects/Addresses";
        systemGroupPaths[DNSName::TYPENAME]       = "Objects/DNS Names";
        systemGroupPaths[AddressTable::TYPENAME]  = "Objects/Address Tables";
        systemGroupPaths[AddressRange::TYPENAME]  = "Objects/Address Ranges";
        systemGroupPaths[ObjectGroup::TYPENAME]   = "Objects/Groups";
        systemGroupPaths[DynamicGroup::TYPENAME]  = "Objects/Groups";
        systemGroupPaths[Host::TYPENAME]          = "Objects/Hosts";
        systemGroupPaths[Network::TYPENAME]       = "Objects/Networks";
        systemGroupPaths[NetworkIPv6::TYPENAME]   = "Objects/Networks";

        systemGroupPaths[ServiceGroup::TYPENAME]  = "Services/Groups";
        systemGroupPaths[CustomService::TYPENAME] = "Services/Custom";
        systemGroupPaths[IPService::TYPENAME]     = "Services/IP";
        systemGroupPaths[ICMPService::TYPENAME]   = "Services/ICMP";
        systemGroupPaths[ICMP6Service::TYPENAME]  = "Services/ICMP";
        systemGroupPaths[TCPService::TYPENAME]    = "Services/TCP";
        systemGroupPaths[UDPService::TYPENAME]    = "Services/UDP";
        systemGroupPaths[UserService::TYPENAME]   = "Services/Users";

        systemGroupPaths[TagService::TYPENAME]    = "Services/TagServices";

        systemGroupPaths[Firewall::TYPENAME]      = "Firewalls";
        systemGroupPaths[Cluster::TYPENAME]       = "Clusters";

        systemGroupPaths[Interval::TYPENAME]      = "Time";



        systemGroupTypes[Firewall::TYPENAME]=      ObjectGroup::TYPENAME;
        systemGroupNames[Firewall::TYPENAME]=      "Firewalls"    ;

        systemGroupTypes[Cluster::TYPENAME]=       ObjectGroup::TYPENAME;
        systemGroupNames[Cluster::TYPENAME]=       "Clusters";

        systemGroupTypes[Host::TYPENAME]=          ObjectGroup::TYPENAME;
        systemGroupNames[Host::TYPENAME]=          "Hosts"          ;

        systemGroupTypes[Network::TYPENAME]=       ObjectGroup::TYPENAME;
        systemGroupNames[Network::TYPENAME]=       "Networks"       ;

        systemGroupTypes[NetworkIPv6::TYPENAME]=   ObjectGroup::TYPENAME;
        systemGroupNames[NetworkIPv6::TYPENAME]=   "Networks"       ;

        systemGroupTypes[IPv4::TYPENAME]=          ObjectGroup::TYPENAME;
        systemGroupNames[IPv4::TYPENAME]=          "Addresses"      ;

        systemGroupTypes[IPv6::TYPENAME]=          ObjectGroup::TYPENAME;
        systemGroupNames[IPv6::TYPENAME]=          "Addresses"      ;

        systemGroupTypes[DNSName::TYPENAME]=       ObjectGroup::TYPENAME;
        systemGroupNames[DNSName::TYPENAME]=       "DNS Names"      ;

        systemGroupTypes[AddressTable::TYPENAME]=  ObjectGroup::TYPENAME;
        systemGroupNames[AddressTable::TYPENAME]=  "Address Tables"      ;

        systemGroupTypes[AddressRange::TYPENAME]=  ObjectGroup::TYPENAME;
        systemGroupNames[AddressRange::TYPENAME]=  "Address Ranges" ;

        systemGroupTypes[ObjectGroup::TYPENAME]=   ObjectGroup::TYPENAME;
        systemGroupNames[ObjectGroup::TYPENAME]=   "Groups"         ;

        systemGroupTypes[DynamicGroup::TYPENAME]=  ObjectGroup::TYPENAME;
        systemGroupNames[DynamicGroup::TYPENAME]=  "Groups"         ;

        systemGroupTypes[CustomService::TYPENAME]= ServiceGroup::TYPENAME;
        systemGroupNames[CustomService::TYPENAME]= "Custom";

        systemGroupTypes[IPService::TYPENAME]=     ServiceGroup::TYPENAME;
        systemGroupNames[IPService::TYPENAME]=     "IP"            ;

        systemGroupTypes[ICMPService::TYPENAME]=   ServiceGroup::TYPENAME;
        systemGroupNames[ICMPService::TYPENAME]=   "ICMP"          ;

        systemGroupTypes[ICMP6Service::TYPENAME]=  ServiceGroup::TYPENAME;
        systemGroupNames[ICMP6Service::TYPENAME]=  "ICMP"          ;

        systemGroupTypes[TCPService::TYPENAME]=    ServiceGroup::TYPENAME;
        systemGroupNames[TCPService::TYPENAME]=    "TCP"           ;

        systemGroupTypes[UDPService::TYPENAME]=    ServiceGroup::TYPENAME;
        systemGroupNames[UDPService::TYPENAME]=    "UDP"           ;

        systemGroupTypes[TagService::TYPENAME]=    ServiceGroup::TYPENAME;
        systemGroupNames[TagService::TYPENAME]=    "TagServices"    ;

        systemGroupTypes[UserService::TYPENAME]=   ServiceGroup::TYPENAME;
        systemGroupNames[UserService::TYPENAME]=   "Users"    ;

        systemGroupTypes[ServiceGroup::TYPENAME]=  ServiceGroup::TYPENAME;
        systemGroupNames[ServiceGroup::TYPENAME]=  "Groups"        ;

        systemGroupTypes[Interval::TYPENAME]=      IntervalGroup::TYPENAME;
        systemGroupNames[Interval::TYPENAME]=      "Time"         ;

        systemGroupTypes[Interface::TYPENAME]=     "";
        systemGroupNames[Interface::TYPENAME]=     "";

        systemGroupTypes[Library::TYPENAME]=      FWObjectDatabase::TYPENAME;
        systemGroupNames[Library::TYPENAME]=      "FWObjectDatabase";

        standardIDs.insert("syslib000");
        standardIDs.insert("syslib001");

        standardIDs.insert("sysid0");
        standardIDs.insert("sysid1");
        standardIDs.insert("sysid2");
        standardIDs.insert("sysid99");

        standardIDs.insert("stdid01");
        standardIDs.insert("stdid01_1");
        standardIDs.insert("stdid02");
        standardIDs.insert("stdid02_1");
        standardIDs.insert("stdid03");
        standardIDs.insert("stdid03_1");
        standardIDs.insert("stdid04");
        standardIDs.insert("stdid04_1");
        standardIDs.insert("stdid05");
        standardIDs.insert("stdid05_1");
        standardIDs.insert("stdid06");
        standardIDs.insert("stdid06_1");
        standardIDs.insert("stdid07");
        standardIDs.insert("stdid07_1");
        standardIDs.insert("stdid08");
        standardIDs.insert("stdid08_1");
        standardIDs.insert("stdid09");
        standardIDs.insert("stdid09_1");
        standardIDs.insert("stdid10");
        standardIDs.insert("stdid10_1");
        standardIDs.insert("stdid11");
        standardIDs.insert("stdid11_1");
        standardIDs.insert("stdid12");
        standardIDs.insert("stdid12_1");
        standardIDs.insert("stdid13");
        standardIDs.insert("stdid13_1");
        standardIDs.insert("stdid14");
        standardIDs.insert("stdid14_1");
        standardIDs.insert("stdid15");
        standardIDs.insert("stdid15_1");
        standardIDs.insert("stdid16");
        standardIDs.insert("stdid16_1");
        standardIDs.insert("stdid17");
        standardIDs.insert("stdid17_1");
        standardIDs.insert("stdid18");
        standardIDs.insert("stdid18_1");
        standardIDs.insert("stdid19");
        standardIDs.insert("stdid19_1");

        // these can not be replaced with set<QString> because they
        // are effectively three-state: if the key is missing in the
        // map, the result is assumed to be "true", otherwise it is
        // either "true" or "false" depending on the map entry.
        copyMenuState[""] = false;
        copyMenuState["Firewalls"] = false;
        copyMenuState["Clusters"] = false;
        copyMenuState["Objects"] = false;
        copyMenuState["Objects/Addresses"] = false;
        copyMenuState["Objects/DNS Names"] = false;
        copyMenuState["Objects/Address Tables"] = false;
        copyMenuState["Objects/Address Ranges"] = false;
        copyMenuState["Objects/Groups"] = false;
        copyMenuState["Objects/Hosts"] = false;
        copyMenuState["Objects/Networks"] = false;
        copyMenuState["Services"] = false;
        copyMenuState["Services/Custom"] = false;
        copyMenuState["Services/Groups"] = false;
        copyMenuState["Services/ICMP"] = false;
        copyMenuState["Services/IP"] = false;
        copyMenuState["Services/TCP"] = false;
        copyMenuState["Services/UDP"] = false;
        copyMenuState["Services/TagServices"] = false;
        copyMenuState["Time"] = false;

        cutMenuState[""] = true;
        cutMenuState["Firewalls"] = false;
        cutMenuState["Clusters"] = false;
        cutMenuState["Objects"] = false;
        cutMenuState["Objects/Addresses"] = false;
        cutMenuState["Objects/DNS Names"] = false;
        cutMenuState["Objects/Address Tables"] = false;
        cutMenuState["Objects/Address Ranges"] = false;
        cutMenuState["Objects/Groups"] = false;
        cutMenuState["Objects/Hosts"] = false;
        cutMenuState["Objects/Networks"] = false;
        cutMenuState["Services"] = false;
        cutMenuState["Services/Custom"] = false;
        cutMenuState["Services/Groups"] = false;
        cutMenuState["Services/ICMP"] = false;
        cutMenuState["Services/IP"] = false;
        cutMenuState["Services/TCP"] = false;
        cutMenuState["Services/UDP"] = false;
        cutMenuState["Services/Users"] = false;
        cutMenuState["Services/TagServices"] = false;
        cutMenuState["Time"] = false;

        pasteMenuState[""] = false;
        pasteMenuState["Firewalls"] = true;
        pasteMenuState["Clusters"] = true;
        pasteMenuState["Objects"] = false;
        pasteMenuState["Objects/Addresses"] = true;
        pasteMenuState["Objects/DNS Names"] = true;
        pasteMenuState["Objects/Address Tables"] = true;
        pasteMenuState["Objects/Address Ranges"] = true;
        pasteMenuState["Objects/Groups"] = true;
        pasteMenuState["Objects/Hosts"] = true;
        pasteMenuState["Objects/Networks"] = true;
        pasteMenuState["Services"] = false;
        pasteMenuState["Services/Custom"] = true;
        pasteMenuState["Services/Groups"] = true;
        pasteMenuState["Services/ICMP"] = true;
        pasteMenuState["Services/IP"] = true;
        pasteMenuState["Services/TCP"] = true;
        pasteMenuState["Services/UDP"] = true;
        pasteMenuState["Services/Users"] = true;
        pasteMenuState["Services/TagServices"] = true;
        pasteMenuState["Time"] = true;

        deleteMenuState[""] = true;
        deleteMenuState["Firewalls"] = false;
        deleteMenuState["Clusters"] = false;
        deleteMenuState["Objects"] = false;
        deleteMenuState["Objects/Addresses"] = false;
        deleteMenuState["Objects/DNS Names"] = false;
        deleteMenuState["Objects/Address Tables"] = false;
        deleteMenuState["Objects/Address Ranges"] = false;
        deleteMenuState["Objects/Groups"] = false;
        deleteMenuState["Objects/Hosts"] = false;
        deleteMenuState["Objects/Networks"] = false;
        deleteMenuState["Services"] = false;
        deleteMenuState["Services/Custom"] = false;
        deleteMenuState["Services/Groups"] = false;
        deleteMenuState["Services/ICMP"] = false;
        deleteMenuState["Services/IP"] = false;
        deleteMenuState["Services/TCP"] = false;
        deleteMenuState["Services/UDP"] = false;
        deleteMenuState["Services/Users"] = false;
        deleteMenuState["Services/TagServices"] = false;
        deleteMenuState["Time"] = false;

    }
}

/**
 * returns true if object 'obj' is a system group.  System groups are
 * those that hold other objects. Unlike user-defined groups, system
 * groups always contain only objects themselves and never contain
 * references to objects. User-defined groups, on the other hand,
 * always contain only references to objects.
 *
 */
bool FWBTree::isSystem(FWObject *obj)
{
    if (Library::isA(obj))
        return (obj->getId()==FWObjectDatabase::STANDARD_LIB_ID ||
                obj->getId()==FWObjectDatabase::DELETED_OBJECTS_ID);

    if (FWObjectDatabase::isA(obj)) return true;

    return isStandardFolder(obj);
}

/**
 * returns true if @obj is a standard folder,e.g. "Firewalls", "Objects",
 * "Objects/Hosts" etc.
 */
bool FWBTree::isStandardFolder(FWObject *obj)
{
    string path = obj->getPath(true);  // relative path
    return (standardFolders.contains(QString::fromUtf8(path.c_str())));
}

bool FWBTree::isStandardId(FWObject *obj)
{
    return standardIDs.contains(FWObjectDatabase::getStringId(obj->getId()).c_str());
}

bool FWBTree::validateForInsertion(FWObject *target, FWObject *obj, QString &err)
{
    if (fwbdebug) qDebug("FWBTree::validateForInsertion  target %s  obj %s",
                         target->getTypeName().c_str(),
                         obj->getTypeName().c_str());

    FWObject *ta = target;
    if (IPv4::isA(ta) || IPv6::isA(ta)) ta=ta->getParent();

    err = QObject::tr("Impossible to insert object %1 (type %2) into %3\n"
                      "because of incompatible type.")
        .arg(obj->getName().c_str())
        .arg(obj->getTypeName().c_str())
        .arg(ta->getName().c_str());

    if (Host::isA(ta)      && Interface::isA(obj))   return true;
    if (Firewall::isA(ta)  && Interface::isA(obj))   return true;
    if (Interface::isA(ta) && IPv4::isA(obj))        return true;
    if (Interface::isA(ta) && IPv6::isA(obj))        return true;
    if (Interface::isA(ta) && physAddress::isA(obj)) return true;

    FWBTree objtree;
    if (objtree.isSystem(ta))
    {
        QString parentType = systemGroupTypes[obj->getTypeName().c_str()];
        QString parentName = systemGroupNames[obj->getTypeName().c_str()];

/* parentType or/and parentName are going to be empty if information
 * about object obj is missing in systemGroupTypes/Names tables
 */
        if (parentType.isEmpty() || parentName.isEmpty()) return false;

        if (ta->getTypeName() == string(parentType.toLatin1()) &&
            ta->getName()     == string(parentName.toLatin1()) )
            return true;

        return false;
    }

    Host *hst = Host::cast(ta);
    Firewall *fw = Firewall::cast(ta);
    Interface *intf = Interface::cast(ta);
    FWObject *parent_fw = ta;
    while (parent_fw && Firewall::cast(parent_fw)==nullptr)
        parent_fw = parent_fw->getParent();

    if (parent_fw && Interface::isA(obj))
    {
        std::unique_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(parent_fw));

        return int_prop->validateInterface(ta, obj, false, err);
    }

    if (fw!=nullptr)
    {
        // inserting some object into firewall or cluster
        if (!fw->validateChild(obj)) return false;
        return true;
    }

    if (hst!=nullptr)  return (hst->validateChild(obj));

    if (intf!=nullptr)
    {
        if (!intf->validateChild(obj)) return false;
        return true;
    }

    Group *grp=Group::cast(ta);
    if (grp!=nullptr) return grp->validateChild(obj);

    return false;
}

void FWBTree::getStandardSlotForObject(const QString &objType,
                                       QString &parentType,
                                       QString &parentName)
{
    parentType = systemGroupTypes[objType];
    parentName = systemGroupNames[objType];
}

/**
 * this method finds standard system folder for an object of a given
 * type in a given library. This method implemented our standard tree
 * structure (the one that is created in the method createNewLibrary)
 */
FWObject* FWBTree::getStandardSlotForObject(FWObject* lib,const QString &objType)
{
    QString path = systemGroupPaths[objType];

    if (path.isEmpty()) return lib;

    QString level1 = path.section('/',0,0);
    QString level2 = path.section('/',1,1);

    FWObject::iterator i=std::find_if(lib->begin(),lib->end(),
        FWObjectNameEQPredicate(static_cast<const char*>(level1.toLatin1())));
    if (i==lib->end()) return nullptr;
    FWObject *l1obj = *i;
    if (level2.isEmpty()) return l1obj;

    i=std::find_if(l1obj->begin(),l1obj->end(),
        FWObjectNameEQPredicate(static_cast<const char*>(level2.toLatin1())));

    if (i==l1obj->end()) return nullptr;
    return (*i);
}

FWObject* FWBTree::createNewLibrary(FWObjectDatabase *db)
{

    FWObject *nlib = db->create(Library::TYPENAME);
    db->add(nlib);
    nlib->setName( string(QObject::tr("New Library").toUtf8()) );

    FWObject *o1 = db->create(ObjectGroup::TYPENAME);
    o1->setName("Objects");
    nlib->add(o1);

    FWObject *o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("Addresses");
    o1->add(o2);

    o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("DNS Names");
    o1->add(o2);

    o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("Address Tables");
    o1->add(o2);

    o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("Groups");
    o1->add(o2);

    o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("Hosts");
    o1->add(o2);

    o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("Networks");
    o1->add(o2);

    o2 = db->create(ObjectGroup::TYPENAME);
    o2->setName("Address Ranges");
    o1->add(o2);

    o1 = db->create(ServiceGroup::TYPENAME);
    o1->setName("Services");
    nlib->add(o1);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("Groups");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("ICMP");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("IP");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("TCP");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("UDP");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("Users");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("Custom");
    o1->add(o2);

    o2 = db->create(ServiceGroup::TYPENAME);
    o2->setName("TagServices");
    o1->add(o2);

    o1 = db->create(ObjectGroup::TYPENAME);
    o1->setName("Firewalls");
    nlib->add(o1);

    o1 = db->create(ObjectGroup::TYPENAME);
    o1->setName("Clusters");
    nlib->add(o1);

    o1 = db->create(IntervalGroup::TYPENAME);
    o1->setName("Time");
    nlib->add(o1);

    return nlib;
}

/*
 * return string that can be translated for the object type name
 */
QString FWBTree::getTranslatableObjectTypeName(const QString &type_name)
{
    if (type_name == Library::TYPENAME) return QObject::tr("Library");
    if (type_name == Firewall::TYPENAME) return QObject::tr("Firewall");
    if (type_name == Cluster::TYPENAME) return QObject::tr("Cluster");
    if (type_name == Host::TYPENAME) return QObject::tr("Host");
    if (type_name == Interface::TYPENAME) return QObject::tr("Interface");
    if (type_name == AttachedNetworks::TYPENAME) return QObject::tr("Attached Networks");
    if (type_name == Network::TYPENAME) return QObject::tr("Network");
    if (type_name == NetworkIPv6::TYPENAME) return QObject::tr("Network IPv6");
    if (type_name == IPv4::TYPENAME) return QObject::tr("Address");
    if (type_name == IPv6::TYPENAME) return QObject::tr("Address IPv6");
    if (type_name == DNSName::TYPENAME) return QObject::tr("DNS Name");
    if (type_name == AddressTable::TYPENAME) return QObject::tr("Address Table");
    if (type_name == AddressRange::TYPENAME) return QObject::tr("Address Range");
    if (type_name == ObjectGroup::TYPENAME) return QObject::tr("Object Group");
    if (type_name == DynamicGroup::TYPENAME) return QObject::tr("Dynamic Group");
    if (type_name == CustomService::TYPENAME) return QObject::tr("Custom Service");
    if (type_name == IPService::TYPENAME) return QObject::tr("IP Service");
    if (type_name == ICMPService::TYPENAME) return QObject::tr("ICMP Service");
    if (type_name == ICMP6Service::TYPENAME) return QObject::tr("ICMP6 Service");
    if (type_name == TCPService::TYPENAME) return QObject::tr("TCP Service");
    if (type_name == UDPService::TYPENAME) return QObject::tr("UDP Service");
    if (type_name == TagService::TYPENAME) return QObject::tr("TagService");
    if (type_name == UserService::TYPENAME) return QObject::tr("User Service");
    if (type_name == ServiceGroup::TYPENAME) return QObject::tr("Service Group");
    if (type_name == Interval::TYPENAME) return QObject::tr( "Time Interval");
    if (type_name == physAddress::TYPENAME) return QObject::tr( "MAC Address");

    if (type_name == Policy::TYPENAME) return QObject::tr( "Policy Rule Set");
    if (type_name == NAT::TYPENAME) return QObject::tr( "NAT Rule Set");
    if (type_name == Routing::TYPENAME) return QObject::tr( "Routing Rule Set");

    if (type_name == FailoverClusterGroup::TYPENAME) return QObject::tr("Failover group");
    if (type_name == StateSyncClusterGroup::TYPENAME) return QObject::tr("State synchronization group");

    return "";
}

QString FWBTree::getTranslatableNewObjectMenuText(const QString &type_name)
{
    if (type_name == Library::TYPENAME) return QObject::tr("New Library");
    if (type_name == Firewall::TYPENAME) return QObject::tr("New Firewall");
    if (type_name == Cluster::TYPENAME) return QObject::tr("New Cluster");
    if (type_name == Host::TYPENAME) return QObject::tr("New Host");
    if (type_name == Interface::TYPENAME) return QObject::tr("New Interface");
    if (type_name == AttachedNetworks::TYPENAME) return QObject::tr("New Attached Networks");
    if (type_name == Network::TYPENAME) return QObject::tr("New Network");
    if (type_name == NetworkIPv6::TYPENAME) return QObject::tr("New Network IPv6");
    if (type_name == IPv4::TYPENAME) return QObject::tr("New Address");
    if (type_name == IPv6::TYPENAME) return QObject::tr("New Address IPv6");
    if (type_name == DNSName::TYPENAME) return QObject::tr("New DNS Name");
    if (type_name == AddressTable::TYPENAME) return QObject::tr("New Address Table");
    if (type_name == AddressRange::TYPENAME) return QObject::tr("New Address Range");
    if (type_name == ObjectGroup::TYPENAME) return QObject::tr("New Object Group");
    if (type_name == DynamicGroup::TYPENAME) return QObject::tr("New Dynamic Group");
    if (type_name == CustomService::TYPENAME) return QObject::tr("New Custom Service");
    if (type_name == IPService::TYPENAME) return QObject::tr("New IP Service");
    if (type_name == ICMPService::TYPENAME) return QObject::tr("New ICMP Service");
    if (type_name == ICMP6Service::TYPENAME) return QObject::tr("New ICMP6 Service");
    if (type_name == TCPService::TYPENAME) return QObject::tr("New TCP Service");
    if (type_name == UDPService::TYPENAME) return QObject::tr("New UDP Service");
    if (type_name == TagService::TYPENAME) return QObject::tr("New TagService");
    if (type_name == UserService::TYPENAME) return QObject::tr("New User Service");
    if (type_name == ServiceGroup::TYPENAME) return QObject::tr("New Service Group");
    if (type_name == Interval::TYPENAME) return QObject::tr( "New Time Interval");
    if (type_name == physAddress::TYPENAME) return QObject::tr( "New MAC Address");

    if (type_name == Policy::TYPENAME) return QObject::tr( "New Policy Rule Set");
    if (type_name == NAT::TYPENAME) return QObject::tr( "New NAT Rule Set");
    if (type_name == Routing::TYPENAME) return QObject::tr( "New Routing Rule Set");

    if (type_name == FailoverClusterGroup::TYPENAME) return QObject::tr("New failover group");
    if (type_name == StateSyncClusterGroup::TYPENAME) return QObject::tr("New state synchronization group");

    return "";
}


QList<const char *> FWBTree::getObjectTypes()
{
    QList<const char *> ret;
    ret.append(Firewall::TYPENAME);
    ret.append(Cluster::TYPENAME);
    ret.append(Host::TYPENAME);
    ret.append(Network::TYPENAME);
    ret.append(NetworkIPv6::TYPENAME);
    ret.append(IPv4::TYPENAME);
    ret.append(IPv6::TYPENAME);
    ret.append(DNSName::TYPENAME);
    ret.append(AddressTable::TYPENAME);
    ret.append(AddressRange::TYPENAME);
    ret.append(ObjectGroup::TYPENAME);
    ret.append(DynamicGroup::TYPENAME);

    return ret;
}


QList<const char *> FWBTree::getServiceTypes()
{
    QList<const char *> ret;
    ret.append(CustomService::TYPENAME);
    ret.append(IPService::TYPENAME);
    ret.append(ICMPService::TYPENAME);
    ret.append(ICMP6Service::TYPENAME);
    ret.append(TCPService::TYPENAME);
    ret.append(UDPService::TYPENAME);
    ret.append(TagService::TYPENAME);
    ret.append(UserService::TYPENAME);
    ret.append(ServiceGroup::TYPENAME);

    return ret;
}
