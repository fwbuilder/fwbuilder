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

        FWObject *parent_fw = intf->getParentHost();

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


