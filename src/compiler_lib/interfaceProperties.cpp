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

#include <QObject>
#include <QStringList>


using namespace std;
using namespace libfwbuilder;

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

bool interfaceProperties::validateInterface(FWObject *parent,
                                            FWObject *intf,
                                            bool check_types,
                                            QString &err)
{
    if (Interface::cast(parent) && Interface::cast(intf))
    {
        if (!Interface::cast(parent)->validateChild(intf))
        {
            // See Interface::validateChild(). Currently the only
            // condition when interface can not become a child of
            // another interface is when interface has subinterfaces
            // of its own.
            err = QObject::tr("Interface %1 can not become subinterface of %2 "
                              "because only one level of subinterfaces is allowed.")
                .arg(intf->getName().c_str())
                .arg(parent->getName().c_str());
            return false;
        }

        if (check_types)
        {
            // We check types when this method is called from a compiler
            string parent_interface_type =
                Interface::cast(parent)->getOptionsObject()->getStr("type");
            if (parent_interface_type.empty()) parent_interface_type = "ethernet";

            FWObject *fw = Interface::cast(parent)->getParentHost();
            QString host_os = fw->getStr("host_OS").c_str();
            Resources* os_res = Resources::os_res[host_os.toStdString()];
            list<string> interface_type_pairs;
            os_res->getResourceStrList(
                "/FWBuilderResources/Target/subinterfaces/" + parent_interface_type,
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
                    .arg(parent->getName().c_str())
                    .arg(parent_interface_type.c_str());

                return false;
            }
        }
    }
    return validateInterface(intf->getParent(), intf->getName().c_str(), err);
}

/**
 * this method implements policy for the interface hierarchy, that is,
 * can given interface be a child of a cluster or a firewall or
 * another interface.
 */
bool interfaceProperties::validateInterface(FWObject *parent,
                                            const QString &interface_name,
                                            QString &err)
{
    if (Firewall::cast(parent))
    {
        if (looksLikeVlanInterface(interface_name))
        {
            QString parent_name = parent->getName().c_str();
            if (Cluster::isA(parent))
            {
                // cluster is allowed to have top-level vlan interfaces,
                // therefore we do not need to check the name of the
                // interface against the name of the parent. This is
                // signalled to isValidVlanInterfaceName() by passing
                // empty string as parent_interface
                parent_name = "";
            }
            return isValidVlanInterfaceName(interface_name, parent_name, err);
        }
        return true;
    }

    if (Interface::cast(parent))
    {
        string parent_interface_type =
            Interface::cast(parent)->getOptionsObject()->getStr("type");
        // check vlan conditions as well
        if (looksLikeVlanInterface(interface_name))
        {
            // vlan interface can be a child of a bridge, in which
            // case its base name does not match the
            // parent. Perform other checks except this, pass ""
            // as parent name argument to isValidVlanInterfaceName()
            if (parent_interface_type == "bridge")
                return isValidVlanInterfaceName(interface_name, "", err);

            QString parent_name = parent->getName().c_str();
            return isValidVlanInterfaceName(interface_name, parent_name, err);
        }

        // interface_name is not a vlan
        // regular interface can only be a child of bond or bridge
        return (parent_interface_type == "bridge" || parent_interface_type == "bonding");
    }

    // parent is not firewall (cluster) and not interface
    err = QObject::tr("Interface can not be a child object of %1").arg(
        parent->getTypeName().c_str());
    return false;
}

