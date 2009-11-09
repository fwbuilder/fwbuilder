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

#include "../../config.h"

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

#include <QObject>
#include <QStringList>


using namespace std;
using namespace libfwbuilder;


void interfaceProperties::parseVlan(InterfaceData *intf)
{
    intf->interface_type = "8021q";
    QString base_name;
    parseVlan(intf->name.c_str(), &base_name, &(intf->vlan_id));
}

void interfaceProperties::parseVlan(const QString &name,
                                    QString *base_name,
                                    int *vlan_id)
{
    for (int idx=0; idx < vlan_name_patterns.size(); ++idx)
    {
        if (vlan_name_patterns[idx].indexIn(name) != -1)
        {
            *base_name = vlan_name_patterns[idx].cap(1);
            *vlan_id = vlan_name_patterns[idx].cap(2).toInt();
        }
    }
}

bool interfaceProperties::looksLikeVlanInterface(InterfaceData *intf)
{
    return looksLikeVlanInterface(intf->name.c_str());
}

bool interfaceProperties::looksLikeVlanInterface(const QString &int_name)
{
    for (int idx=0; idx < vlan_name_patterns.size(); ++idx)
    {
        if (vlan_name_patterns[idx].indexIn(int_name) != -1) return true;
    }
    return false;
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
        err = QObject::tr("'%1' is not a valid vlan interface name").arg(subint_name);
        return false;
    }

    for (int idx=0; idx < vlan_name_patterns.size(); ++idx)
    {
        if (vlan_name_patterns[idx].indexIn(subint_name) != -1)
        {
            QString parent_name_from_regex = vlan_name_patterns[idx].cap(1);
            if (!parent_name.isEmpty() &&
                parent_name_from_regex != "vlan" &&
                parent_name != parent_name_from_regex)
            {
                err = QObject::tr("'%1' looks like a name of a vlan interface "
                                  "but it does not match the name of the parent "
                                  "interface '%2'").arg(subint_name).arg(parent_name);
                return false;
            }
            int vlan_id = vlan_name_patterns[idx].cap(2).toInt();
            if (vlan_id > 4095)
            {
                err = QObject::tr("'%1' looks like a name of a vlan interface "
                                  "but vlan ID it defines is outside of the valid range."
                                  "").arg(subint_name);
                return false;
            }
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

    FWObject *fw = intf->getParentHost();
    Resources *os_res = Resources::os_res[fw->getStr("host_OS")];
    assert(os_res != NULL);

    if (intf->isDyn()) return false;
    if (intf->isBridgePort()) return false;
    if (intf->isSlave()) return false;

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

            FWObject *fw = Interface::cast(target)->getParentHost();
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
        // per ticket #487
        if (Interface::cast(intf)->isUnnumbered())
        {
            err = QObject::tr("Interface %1 is unnumbered, "
                              "it can not belong to a cluster")
                .arg(intf->getName().c_str());
            return false;
        }

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
        if (looksLikeVlanInterface(interface_name))
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
        if (looksLikeVlanInterface(interface_name))
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

