/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

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

#include <assert.h>

#include "Configlet.h"
#include "OSConfigurator_bsd.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/StateSyncClusterGroup.h"

#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include <QTextStream>
#include <QString>
#include <QtDebug>

#include <algorithm>
#include <memory>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


/*
 * I need to sort interfaces by name but make sure carp interfaces are
 * always last. See #1807
 */
bool compare_names(FWObject *a, FWObject *b)
{
    QString a_name = QString(a->getName().c_str());
    QString b_name = QString(b->getName().c_str());
    if (a_name.startsWith("carp") && b_name.startsWith("carp"))
        return a_name < b_name;
    if (a_name.startsWith("carp")) return false;
    if (b_name.startsWith("carp")) return true;
    return a_name < b_name;
}


string OSConfigurator_bsd::configureInterfaces()
{
    FWOptions* options = fw->getOptionsObject();

    // Update vlans first because we may need to update ip addresses
    // on vlan interfaces later
    if ( options->getBool("configure_vlan_interfaces") ) 
    {
        // http://blog.scottlowe.org/2007/08/31/vlan-interfaces-with-openbsd-41/
        // ifconfig <VLAN interface name> vlan <VLAN ID> vlandev <physical network device>

        QStringList all_physical_interfaces;
        QMap<QString, Interface*> parent_interfaces;
        QMap<QString, QStringList> vlans;
        QStringList all_vlan_interfaces; // all vlan interfaces

        FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);
            QString iface_name = iface->getName().c_str();
            parent_interfaces[iface_name] = iface;
            all_physical_interfaces << iface_name;

            FWObjectTypedChildIterator si=iface->findByType(Interface::TYPENAME);
            for ( ; si!=si.end(); ++si ) 
            {
                Interface *subinterface = Interface::cast(*si);
                assert(subinterface);
                if (subinterface->getOptionsObject()->getStr("type") == "8021q")
                {
                    vlans[iface_name] << subinterface->getName().c_str();
                    all_vlan_interfaces << subinterface->getName().c_str();
                }
            }
        }

        // sort interfaces by name
        all_vlan_interfaces.sort();
        all_physical_interfaces.sort();

        // issue sync_vlan_interfaces command even if there are no vlans
        // since it deletes them on the firewall if they exist
        summaryConfigLineVlan(all_vlan_interfaces);

        foreach (QString iface_name, all_physical_interfaces)
        {
            Interface *iface = parent_interfaces[iface_name];
            QStringList vlan_subinterfaces = vlans[iface_name];
            if (vlan_subinterfaces.size() > 0)
                interfaceConfigLineVlan(iface, vlan_subinterfaces);
        }
    }

    if (options->getBool("configure_bridge_interfaces"))
    {
        list<Interface*> all_bridges = fw->getInterfacesByType("bridge");

        QStringList all_bridge_interfaces;
        QMap<QString, Interface*> bridge_interfaces_by_name;
        QMap<QString, QStringList> bridge_ports;

        for (list<Interface*>::iterator it=all_bridges.begin();
             it!=all_bridges.end(); ++it)
        {
            Interface *iface = Interface::cast(*it);
            assert(iface);
            QString iface_name = iface->getName().c_str();
            all_bridge_interfaces << iface_name;
            bridge_interfaces_by_name[iface_name] = iface;

            FWObjectTypedChildIterator si = iface->findByType(Interface::TYPENAME);
            for ( ; si!=si.end(); ++si ) 
            {
                Interface *subinterface = Interface::cast(*si);
                assert(subinterface);
                bridge_ports[iface_name] << subinterface->getName().c_str();
            }
        }

        // sort interfaces by name
        all_bridge_interfaces.sort();
        
        summaryConfigLineBridge(all_bridge_interfaces);

        foreach (QString iface_name, all_bridge_interfaces)
        {
            Interface *iface = bridge_interfaces_by_name[iface_name];
            if (bridge_ports.size() > 0)
                interfaceConfigLineBridge(iface, bridge_ports[iface_name]);
        }
    }



    if ( options->getBool("configure_carp_interfaces") ) 
    {
/*
 * Compiler::processFailoverGroup copies interfaces of the cluster to
 * the member firewall objects. This means when we scan interfaces of
 * the firewall here, we get both its normal interfaces and a copy of
 * cluster interfaces. 
 *
 */
        QStringList carp_interfaces;
        QMap<QString, Interface*> carp_interfaces_by_name;
        QMap<QString, FWObject*> failover_groups;

        FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);
            QString iface_name = iface->getName().c_str();
            if ( ! iface->isFailoverInterface()) continue;
            FWObject *failover_group =
                iface->getFirstByType(FailoverClusterGroup::TYPENAME);
            if (failover_group && failover_group->getStr("type") == "carp")
            {
                carp_interfaces << iface_name;
                carp_interfaces_by_name[iface_name] = iface;
                failover_groups[iface_name] = failover_group;
            }
        }

        // sort interfaces by name
        carp_interfaces.sort();
        
        // issue "sync_carp_interfaces" call even when we have none, it will
        // delete those that might exist on the firewall
        summaryConfigLineCARP(carp_interfaces);

        foreach (QString iface_name, carp_interfaces)
        {
            Interface *iface = carp_interfaces_by_name[iface_name];
            FWObject* failover_group = failover_groups[iface_name];
            interfaceConfigLineCARP(iface, failover_group);
        }
    }


    if ( options->getBool("configure_interfaces") ) 
    {
        std::auto_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                fw->getStr("host_OS")));

        list<FWObject*> all_interfaces = fw->getByTypeDeep(Interface::TYPENAME);
        all_interfaces.sort(compare_names);

        QStringList configure_intf_commands;
        QStringList intf_names;
        QStringList ipv6_names;
        QStringList all_names;
        QMap<QString, list<pair<InetAddr,InetAddr> > > all_addresses;
        QMap<QString, Interface*> interfaces_by_name;

        for (list<FWObject*>::iterator i=all_interfaces.begin();
             i != all_interfaces.end(); ++i )
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);
            QString iface_name = iface->getName().c_str();
            interfaces_by_name[iface_name] = iface;
            all_names << iface_name;

            QStringList update_addresses;
            QStringList ignore_addresses;
            if (int_prop->manageIpAddresses(iface, update_addresses, ignore_addresses))
            {
                // unfortunately addresses in update_addresses are in
                // the form of address/masklen but OpenBSD ifconfig
                // uses hex netmask representation and so should we.
                // Will ignore update_addresses and ignore_addresses and
                // build our own list here. Returned value of manageIpAddresses()
                // is useful though.
                list<FWObject*> all_addr = iface->getByType(IPv4::TYPENAME);
                list<FWObject*> all_ipv6 = iface->getByType(IPv6::TYPENAME);
                all_addr.insert(all_addr.begin(), all_ipv6.begin(), all_ipv6.end());

                bool have_ipv6 = false;

                const InetAddr *netmask = iface->getNetmaskPtr();

                list<pair<InetAddr,InetAddr> > iface_all_addresses;

                for (list<FWObject*>::iterator j = all_addr.begin();
                     j != all_addr.end(); ++j)
                {
                    Address *iaddr = Address::cast(*j);
                    const InetAddr *ipaddr = iaddr->getAddressPtr();
                    const InetAddr *ipnetm = iaddr->getNetmaskPtr();
                    iface_all_addresses.push_back(
                        pair<InetAddr,InetAddr>(*ipaddr, *ipnetm));
                    if (ipaddr->isV6()) have_ipv6 = true;
                }

                set<const Address*>::iterator it;
                for (it=virtual_addresses.begin(); it!=virtual_addresses.end(); ++it)
                {
                    const Address *addr = *it;
                    const InetAddr *ipaddr = addr->getAddressPtr();
                    FWObject *iaddr = findAddressFor(addr, fw );
                    if (iaddr!=NULL)
                    {
                        Interface *iface_2 = Interface::cast(iaddr->getParent());
                        if (iface_2 == iface)
                        {
                            iface_all_addresses.push_back(
                                pair<InetAddr,InetAddr>(*ipaddr, *netmask));
                            if (ipaddr->isV6()) have_ipv6 = true;
                        }
                    }
                }

                // see #2032.  About interfaces with no addresses:
                //
                // - when we generate rc.conf file, we should add line 
                //   "ifconfig_em0="DHCP"" for dynamic interfaces, so we should 
                //   include them in the management list as well.
                //
                // Note that int_prop returns false for dynamic interfaces on
                // OpenBSD because we do not support rc.conf format for it atm
                // and should not try to manage dynamic interfaces in the shell
                // script format.
                //
                intf_names << iface_name;
                if (have_ipv6) ipv6_names << iface_name;

                all_addresses[iface_name] = iface_all_addresses;
            }
        }

        // sort interfaces by name
        all_names.sort();
        ipv6_names.sort();
        intf_names.sort();

        summaryConfigLineIP(ipv6_names, true);
        summaryConfigLineIP(intf_names, false);

        foreach (QString iface_name, all_names)
        {
            interfaceConfigLineIP(interfaces_by_name[iface_name],
                                  all_addresses[iface_name]);
        }
    }


    if ( options->getBool("configure_pfsync_interfaces") ) 
    {
        bool have_pfsync_interfaces = false;
        QStringList pfsync_output;

        FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i)
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);

            if ( ! iface->getOptionsObject()->getBool("state_sync_group_member"))
                continue;

            int state_sync_group_id = FWObjectDatabase::getIntId(
                iface->getOptionsObject()->getStr("state_sync_group_id"));
            StateSyncClusterGroup *state_sync_group =
                StateSyncClusterGroup::cast(dbcopy->findInIndex(state_sync_group_id));
            assert(state_sync_group!=NULL);

            // Interface can be state sync group member, but of a different type
            if (state_sync_group->getStr("type") != "pfsync") continue;

            have_pfsync_interfaces = true;

            summaryConfigLinePfsync(have_pfsync_interfaces);
            interfaceConfigLinePfsync(iface, state_sync_group);

            break;
        }

        if (!have_pfsync_interfaces) summaryConfigLinePfsync(false);
    }

    
    return printAllInterfaceConfigurationLines().toStdString();
}

void OSConfigurator_bsd::summaryConfigLineIP(QStringList , bool )
{
}

void OSConfigurator_bsd::interfaceConfigLineIP(
    Interface *iface, list<pair<InetAddr,InetAddr> > all_addresses)
{
    if (iface->isDyn()) return;

    QStringList arg1;
    arg1.push_back(iface->getName().c_str());

    for (list<pair<InetAddr,InetAddr> >::iterator j = all_addresses.begin();
         j != all_addresses.end(); ++j)
    {
        InetAddr ipaddr = j->first;
        InetAddr ipnetm = j->second;

        if (ipaddr.isV6())
            arg1.push_back(QString("%1/%2").arg(ipaddr.toString().c_str())
                           .arg(ipnetm.getLength()));
        else
        {
/*
  on OpenBSD ifconfig prints netmask of ipv4 addresses in hex

  # ifconfig em0                             
  em0: flags=8843<UP,BROADCAST,RUNNING,SIMPLEX,MULTICAST> mtu 1500
  lladdr 00:0c:29:83:4d:2f
  media: Ethernet autoselect (1000baseT full-duplex,master)
  status: active
  inet 10.1.1.50 netmask 0xffffff00 broadcast 10.1.1.255
  inet6 fe80::20c:29ff:fe83:4d2f%em0 prefixlen 64 scopeid 0x2
*/
            int nbits = ipnetm.getLength();
            uint32_t netm = 0;
            while (nbits)
            {
                netm = netm >> 1;
                netm |= 1<<31;
                nbits--;
            }

            arg1.push_back(QString("%1/0x%2")
                           .arg(ipaddr.toString().c_str())
                           .arg(netm, -8, 16));
        }
    }

    QString cmd = QString("update_addresses_of_interface \"%1\" \"\"")
        .arg(arg1.join(" "));

    //qDebug() << cmd;

    update_address_lines[iface->getName().c_str()] = cmd;
}



void OSConfigurator_bsd::summaryConfigLineVlan(QStringList vlan_names)
{
    interface_configuration_lines <<
        QString("sync_vlan_interfaces %1").arg(vlan_names.join(" "));
}


void OSConfigurator_bsd::interfaceConfigLineVlan(Interface *iface,
                                                QStringList vlan_names)
{
    interface_configuration_lines << 
        QString("update_vlans_of_interface \"%1 %2\"")
        .arg(iface->getName().c_str())
        .arg(vlan_names.join(" "));
}

void OSConfigurator_bsd::summaryConfigLineBridge(QStringList bridge_names)
{
    interface_configuration_lines <<
        QString("sync_bridge_interfaces %1").arg(bridge_names.join(" "));
}


void OSConfigurator_bsd::interfaceConfigLineBridge(Interface *iface,
                                                    QStringList bridge_port_names)
{
    interface_configuration_lines <<
        QString("update_bridge_interface %1 \"%2\"")
        .arg(iface->getName().c_str())
        .arg(bridge_port_names.join(" "));
}

void OSConfigurator_bsd::summaryConfigLineCARP(QStringList carp_names)
{
    interface_configuration_lines <<
        QString("sync_carp_interfaces %1").arg(carp_names.join(" "));
}

void OSConfigurator_bsd::interfaceConfigLineCARP(Interface *iface,
                                             FWObject *failover_group)
{
    Configlet configlet(fw, "bsd", "carp_interface");
    interfaceConfigLineCARPInternal(iface, failover_group, &configlet);
}

void OSConfigurator_bsd::interfaceConfigLineCARPInternal(
    Interface *iface, FWObject *failover_group, Configlet *configlet)
{
    // failover_master and base_device are set in Compiler::processFailoverGroup
    FWOptions *ifopt = (Interface::cast(iface))->getOptionsObject();
    assert(ifopt != NULL);

    bool master = ifopt->getBool("failover_master");
    string base_interface = ifopt->getStr("base_device");
    QStringList carp_interfaces;

    carp_interfaces.push_back(iface->getName().c_str());

    FWOptions *failover_opts =
        FailoverClusterGroup::cast(failover_group)->getOptionsObject();
    string carp_password = failover_opts->getStr("carp_password");
    if (carp_password.empty()) carp_password = "\"\"";
    string vhid = failover_opts->getStr("carp_vhid");
    int advbase = failover_opts->getInt("carp_advbase");
    int master_advskew = failover_opts->getInt("carp_master_advskew");
    int default_advskew = failover_opts->getInt("carp_default_advskew");

    if (master_advskew < 0) master_advskew = 0;
    if (default_advskew < 0) default_advskew = 0;
    if (master_advskew == default_advskew) default_advskew++;

    int use_advskew;
    if (master)
        use_advskew = master_advskew;
    else
        use_advskew = default_advskew;

    configlet->removeComments();
    configlet->collapseEmptyStrings(true);
    configlet->setVariable("carp_interface", iface->getName().c_str());
    configlet->setVariable("have_advbase", advbase > 1);
    configlet->setVariable("advbase", advbase);
    configlet->setVariable("have_advskew", use_advskew > 0);
    configlet->setVariable("advskew", use_advskew);
    configlet->setVariable("have_base_inetrface", !base_interface.empty());
    configlet->setVariable("base_inetrface", base_interface.c_str());
    configlet->setVariable("carp_password", carp_password.c_str());
    configlet->setVariable("vhid", vhid.c_str());

    interface_configuration_lines <<  configlet->expand();
}

void OSConfigurator_bsd::summaryConfigLinePfsync(bool have_pfsync)
{
    interface_configuration_lines <<
        QString("sync_pfsync_interfaces %1").arg(have_pfsync?"pfsync0":"");
}

/*
 *  http://www.kernel-panic.it/openbsd/carp/index.html
 *  http://www.openbsd.org/faq/pf/carp.html
 * pfsync configuration:
 *
 * ifconfig pfsyncN syncdev syncdev [syncpeer syncpeer]
 */

void OSConfigurator_bsd::interfaceConfigLinePfsync(
    Interface *iface, StateSyncClusterGroup *state_sync_group)
{
    Configlet configlet(fw, "bsd", "pfsync_interface");
    configlet.removeComments();
    configlet.collapseEmptyStrings(true);
    configlet.setVariable("syncdev", iface->getName().c_str());

    if (state_sync_group->getOptionsObject()->getBool("syncpeer"))
    {
        for (FWObjectTypedChildIterator it =
                 state_sync_group->findByType(FWObjectReference::TYPENAME);
             it != it.end(); ++it)
        {
            Interface *cluster_iface = Interface::cast(
                FWObjectReference::getObject(*it));
            assert(cluster_iface);

            if (cluster_iface->getId() == iface->getId()) continue;
                    
            IPv4 *ipv4 = IPv4::cast(cluster_iface->getFirstByType(IPv4::TYPENAME));
            const InetAddr *addr = ipv4->getAddressPtr();

            configlet.setVariable("have_syncpeer", 1);
            configlet.setVariable("syncpeer", addr->toString().c_str());
        }
    }
    interface_configuration_lines <<  configlet.expand();
}


QString OSConfigurator_bsd::printAllInterfaceConfigurationLines()
{
    QStringList keys = update_address_lines.keys();
    keys.sort();
    foreach (QString iface_name, keys)
    {
        interface_configuration_lines << update_address_lines[iface_name];
    }

    return interface_configuration_lines.join("\n");
}


