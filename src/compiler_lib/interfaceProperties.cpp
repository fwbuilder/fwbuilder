/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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


#include "interfaceProperties.h"

#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/FailoverClusterGroup.h"

#include <string>
#include <list>
#include <algorithm>

#include <QDebug>
#include <QObject>
#include <QStringList>
#include <QRegExp>


using namespace std;
using namespace libfwbuilder;


bool interfaceProperties::parseVlan(const QString&, QString*, int*)
{
    return false;
}

bool interfaceProperties::looksLikeVlanInterface(InterfaceData *intf)
{
    return parseVlan(intf->name.c_str(), nullptr, nullptr);
}

bool interfaceProperties::looksLikeVlanInterface(const QString &int_name)
{
    return parseVlan(int_name, nullptr, nullptr);
}

/*
 * common denominator interface name guess. Something like "eth0",
 * "foo0", "longname0", "name0.1", "name0:1". This is mostly intended
 * for Linux and BSD, even though it probably matches some Cisco
 * interfaces too.
 */
bool interfaceProperties::looksLikeInterface(const QString &name)
{
    QRegExp basic_interface_name_pattern("^[a-zA-Z]+\\d{1,}(\\.\\d{1,})?(:\\d{1,})?$");
    return (basic_interface_name_pattern.indexIn(name) != -1);
}

// simple name validation: does not allow space and "-"
// However some platform permit space (procurve).
bool interfaceProperties::basicValidateInterfaceName(Interface *,
                                                     const QString &obj_name,
                                                     QString &err)
{
    if (obj_name.indexOf(' ') != -1 || obj_name.indexOf('-') != -1)
    {
        err = QObject::tr("Interface name '%1' can not contain white space and \"-\"").arg(obj_name);
        return false;
    }
    return true;
}
    

/*
 * While looksLikeVlanInterface only checks interface name format,
 * this method does more detailed check to determine if the interface
 * is valid vlan. In particular, it checks that given interface is
 * indeed a subinterface (parent is also interface) and its base name
 * matches the name of the parent
 */
bool interfaceProperties::isValidVlanInterfaceName(const QString &subint_name,
                                                   const QString &parent_name,
                                                   QString &err)
{
    if (!looksLikeVlanInterface(subint_name))
    {
        err = QObject::tr("'%1' is not a valid vlan interface name")
            .arg(subint_name);
        return false;
    }

    QString parent_name_from_regex;
    int vlan_id;
    if (parseVlan(subint_name, &parent_name_from_regex, &vlan_id))
    {
        if (!parent_name.isEmpty() &&
            parent_name_from_regex != "vlan" &&
            parent_name != parent_name_from_regex)
        {
            err = QObject::tr("'%1' looks like a name of a vlan interface "
                              "but it does not match the name of the parent "
                              "interface '%2'").arg(subint_name).arg(parent_name);
            return false;
        }

        if (vlan_id > 4095)
        {
            err = QObject::tr("'%1' looks like a name of a vlan interface "
                              "but vlan ID it defines is outside of the valid "
                              "range.").arg(subint_name);
            return false;
        }
    }

    return true;
}

void interfaceProperties::getListOfAddresses(Interface *intf, QStringList &addr_list)
{
    list<FWObject*> addresses = intf->getByType(IPv4::TYPENAME);
    list<FWObject*> ipv6_addresses = intf->getByType(IPv6::TYPENAME);
    addresses.splice(addresses.begin(), ipv6_addresses);

    for (FWObject::iterator j=addresses.begin(); j!=addresses.end(); ++j)
    {
        const InetAddr *iaddr_addr = Address::cast(*j)->getAddressPtr();
        const InetAddr *iaddr_netm = Address::cast(*j)->getNetmaskPtr();
        addr_list.push_back(
            QString("%1/%2").
            arg(iaddr_addr->toString().c_str()).
            arg(iaddr_netm->getLength()));
    }
}

/*
 * we manage only addresses of vrrp cluster interfaces.
 *
 * We ignore addresses of heartbeat and openais cluster interfaces.
 * To ignore them, we pass list of addresses managed by heartbeat to
 * shell function update_addresses (defined in configlet
 * "update_addresses") as its second argument to make it ignore these
 *
 * This code assumes the name of the cluster interface is the same as
 * names of the corresponding interfaces of member firewalls.
 */
bool interfaceProperties::manageIpAddresses(Interface *intf,
                                            QStringList &update_addresses,
                                            QStringList &ignore_addresses)
{
    update_addresses.clear();
    ignore_addresses.clear();

    FWObject *fw = Host::getParentHost(intf);
    //FWObject *fw = intf->getParentHost();
    Resources *os_res = Resources::os_res[fw->getStr("host_OS")];
    assert(os_res != nullptr);

    if (intf->isDyn()) return false;
    if (intf->isBridgePort()) return false;
    if (intf->isSlave()) return false;
    if (intf->isUnnumbered()) return false;   // see #1506

    string intf_name = intf->getName();

    if (intf->getOptionsObject()->getBool("cluster_interface"))
    {
        if (intf->isLoopback()) return false;

        if (intf->isFailoverInterface())
        {
            FWObject *failover_group =
                intf->getFirstByType(FailoverClusterGroup::TYPENAME);
            string failover_type = failover_group->getStr("type");

            if (os_res->getResourceBool(
                    "/FWBuilderResources/Target/protocols/" + failover_type + "/manage_addresses"))
            {
                getListOfAddresses(intf, update_addresses);
                return true;
            } else
                return false;
        }

        /*
         * if this is cluster interface with the same name as fw interface
         * (as happens in case of hearbeat or openais), skip it
         */
        if (intf->getOptionsObject()->getStr("base_device") == intf_name)
            return false;
    } else
    {
        // regular interface. Lets see if there is cluster interface
        // with the same name. If there is and its failover protocol
        // is heartbeat or openais, we should ignore all addresses it
        // might have.

        getListOfAddresses(intf, update_addresses);

        list<FWObject*> interfaces = fw->getByTypeDeep(Interface::TYPENAME);
        list<FWObject*>::iterator i;
        for (i=interfaces.begin(); i!=interfaces.end(); ++i )
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);

            if (iface->getName() == intf_name &&
                iface->getOptionsObject()->getBool("cluster_interface") &&
                iface->isFailoverInterface())
            {
                FWObject *failover_group =
                    iface->getFirstByType(FailoverClusterGroup::TYPENAME);
                string failover_type = failover_group->getStr("type");

                // some protocols do not like it when failover ip address
                // is managed outside their software
                if (! os_res->getResourceBool(
                        "/FWBuilderResources/Target/protocols/" + failover_type + "/manage_addresses"))
                    getListOfAddresses(iface, ignore_addresses);
                break;
            }
        }
    }

    return true;
}

bool interfaceProperties::validateInterface(FWObject *target,
                                            FWObject *intf,
                                            bool check_types,
                                            QString &err)
{
    if (Interface::cast(target) && Interface::cast(intf))
    {
        if (!Interface::cast(target)->validateChild(intf))
        {
            // See Interface::validateChild(). Currently the only
            // condition when interface can not become a child of
            // another interface is when interface has subinterfaces
            // of its own.
            err = QObject::tr("Interface %1 can not become subinterface of %2 "
                              "because only one level of subinterfaces is allowed.")
                .arg(intf->getName().c_str())
                .arg(target->getName().c_str());
            return false;
        }

        if (check_types)
        {
            // We check types when this method is called from a compiler
            string target_interface_type =
                Interface::cast(target)->getOptionsObject()->getStr("type");
            if (target_interface_type.empty()) target_interface_type = "ethernet";

            FWObject *fw = Host::getParentHost(target);
            //FWObject *fw = Interface::cast(target)->getParentHost();
            QString host_os = fw->getStr("host_OS").c_str();
            Resources* os_res = Resources::os_res[host_os.toStdString()];
            list<string> interface_type_pairs;
            os_res->getResourceStrList(
                "/FWBuilderResources/Target/subinterfaces/" + target_interface_type,
                interface_type_pairs);
            list<string> interface_types;
            for (list<string>::iterator it=interface_type_pairs.begin();
                 it!=interface_type_pairs.end(); ++it)
            {
                QString p = it->c_str();
                interface_types.push_back(p.split(",")[0].toStdString());
            }

            // Implement interface type checks here
            string interface_type = Interface::cast(intf)->getOptionsObject()->getStr("type");
            if (interface_type.empty()) interface_type = "ethernet";

            if (std::find(interface_types.begin(), interface_types.end(),
                          interface_type) == interface_types.end())
            {
                err = QObject::tr("Interface %1 (type '%2') can not be a child "
                                  "of interface %3 (type '%4')")
                    .arg(intf->getName().c_str())
                    .arg(interface_type.c_str())
                    .arg(target->getName().c_str())
                    .arg(target_interface_type.c_str());
                return false;
            }
        }
    }

    if (Cluster::cast(target) && Interface::cast(intf))
    {
        // Note that @target may not be actually a parent of @intf at
        // the time of call to this function. We use this function to
        // validate operation of making @intf a child of @target. @intf
        // can have some other parent at the moment.
        FWObject *parent_interface = intf->getParent();
        if (Interface::isA(parent_interface))
        {
            string interface_type = Interface::cast(intf)->getOptionsObject()->getStr("type");
            if (interface_type.empty()) interface_type = "ethernet";
            string parent_interface_type =
                Interface::cast(parent_interface)->getOptionsObject()->getStr("type");
            if (parent_interface_type.empty()) parent_interface_type = "ethernet";
            if (parent_interface_type == "bridge" && interface_type == "ethernet")
            {
                err = QObject::tr("Interface %1 is a bridge port, "
                                  "it can not belong to a cluster")
                    .arg(intf->getName().c_str());
                return false;
            }
            if (parent_interface_type == "bonding" && interface_type == "ethernet")
            {
                err = QObject::tr("Interface %1 is a bonding interface slave, "
                                  "it can not belong to a cluster")
                    .arg(intf->getName().c_str());
                return false;
            }
        }
    }

    return validateInterface(target, intf->getName().c_str(), err);
}

/**
 * this method implements policy for the interface hierarchy, that is,
 * can given interface be a child of a cluster or a firewall or
 * another interface.
 */
bool interfaceProperties::validateInterface(FWObject *target,
                                            const QString &interface_name,
                                            QString &err)
{
    if (Firewall::cast(target) || Host::cast(target))
    {
        if (vlan_checks && looksLikeVlanInterface(interface_name))
        {
            QString target_name = target->getName().c_str();
            if (Cluster::isA(target))
            {
                // cluster is allowed to have top-level vlan interfaces,
                // therefore we do not need to check the name of the
                // interface against the name of the parent. This is
                // signalled to isValidVlanInterfaceName() by passing
                // empty string as target_interface
                target_name = "";
            }

            return isValidVlanInterfaceName(interface_name, target_name, err);
        }

        return true;
    }

    if (Interface::cast(target))
    {
        string target_interface_type =
            Interface::cast(target)->getOptionsObject()->getStr("type");
        // check vlan conditions as well
        if (vlan_checks && looksLikeVlanInterface(interface_name))
        {
            // vlan interface can be a child of a bridge, in which
            // case its base name does not match the
            // parent. Perform other checks except this, pass ""
            // as parent name argument to isValidVlanInterfaceName()
            if (target_interface_type == "bridge")
                return isValidVlanInterfaceName(interface_name, "", err);

            QString target_name = target->getName().c_str();
            return isValidVlanInterfaceName(interface_name, target_name, err);
        }

        // interface_name is not a vlan
        // regular interface can only be a child of bond or bridge
        if (target_interface_type != "bridge" && target_interface_type != "bonding")
        {
            err = QObject::tr("Interface %1 which is not a vlan can only "
                              "be a subinterface of a bridge or bonding interface")
                .arg(interface_name);
            return false;
        }
        return true;
    }

    // target is not firewall (cluster) and not interface
    err = QObject::tr("Interface can not be a child object of %1").arg(
        target->getTypeName().c_str());
    return false;
}

/*
 * Ticket #727
 *
 * if type is ethernet and has vlan subinterfaces, not eligible
 * if type is vlan, eligible
 * if type is bridge, eligible
 * if type is bonding, eligible
 * if type is ethernet and interface with the same name is used for bonding, then not eligible
 * if type is ethernet and parent is bridge, then not eligible
 */
bool interfaceProperties::isEligibleForCluster(Interface *intf)
{
    list<FWObject*> subinterfaces = intf->getByType(Interface::TYPENAME);
    string interface_type = intf->getOptionsObject()->getStr("type");

    if (intf->isBridgePort())
        return false;

    if (interface_type.empty())
        interface_type = "ethernet";
    if (interface_type == "8021q")
        return true;
    if (interface_type == "bridge")
        return true;
    if (interface_type == "bonding")
        return true;

    if (interface_type == "ethernet")
    {
        Interface *parent_iface = Interface::cast(intf->getParent());
        if (parent_iface &&
            parent_iface->getOptionsObject()->getStr("type") == "bridge")
            return false;

        FWObject *fw = Host::getParentHost(intf);
        //FWObject *fw = intf->getParentHost();
        list<FWObject*> interfaces = fw->getByTypeDeep(Interface::TYPENAME);
        list<FWObject*>::iterator i;
        for (i=interfaces.begin(); i!=interfaces.end(); ++i )
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);
            Interface *parent_iface = Interface::cast(iface->getParent());
            if (parent_iface == nullptr)
         continue;
            if (parent_iface->getOptionsObject()->getStr("type") == "bonding" &&
                iface->getName() == intf->getName())
            {
                // @intf is used as a bond slave and can't be used for cluster
                return false;
            }

        }

        if (subinterfaces.size() > 0)
            return false;
    }
    return true;
}

void interfaceProperties::guessSubInterfaceTypeAndAttributes(Interface *intf)
{
    Interface *parent_intf = Interface::cast(intf->getParent());

    if (parent_intf == nullptr)
        return;

    // Do not modify read-only object
    if (intf->isReadOnly()) return;

    //FWObject *f = Host::getParentHost(intf);
    //FWObject *f = intf->getParentHost();

    // Resources* os_res = Resources::os_res[f->getStr("host_OS")];
    // string os_family = f->getStr("host_OS");
    // if (os_res!=nullptr)
    //     os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    QString err;
    if (looksLikeVlanInterface(intf->getName().c_str()) &&
        isValidVlanInterfaceName(intf->getName().c_str(),
                                 intf->getParent()->getName().c_str(), err)
    )
    {
        InterfaceData *idata = new InterfaceData(*intf);
        //parseVlan(idata);
        idata->interface_type = "8021q";
        parseVlan(idata->name.c_str(), nullptr, &(idata->vlan_id));
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
}

