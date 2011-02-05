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
    ostringstream ostr;
    FWOptions* options = fw->getOptionsObject();

    // Update vlans first because we may need to update ip addresses
    // on vlan interfaces later
    if ( options->getBool("configure_vlan_interfaces") ) 
    {
        QStringList vlan_interfaces; // all vlan interfaces
        QStringList vlan_output;

        // http://blog.scottlowe.org/2007/08/31/vlan-interfaces-with-openbsd-41/
        // ifconfig <VLAN interface name> vlan <VLAN ID> vlandev <physical network device>
        FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);

            QStringList vlan_subinterfaces;

            FWObjectTypedChildIterator si=iface->findByType(Interface::TYPENAME);
            for ( ; si!=si.end(); ++si ) 
            {
                Interface *subinterface = Interface::cast(*si);
                assert(subinterface);

                if (subinterface->getOptionsObject()->getStr("type") == "8021q")
                {
                    vlan_subinterfaces << subinterface->getName().c_str();
                    vlan_interfaces << subinterface->getName().c_str();
                }
            }

            if (vlan_subinterfaces.size() > 0)
                vlan_output << updateVlansOfInterface(iface, vlan_subinterfaces);
        }

        // issue sync_vlan_interfaces command even if there are no vlans
        // since it deletes them on the firewall if they exist
        ostr << listAllVlansConfgLine(vlan_interfaces).toStdString()
             << endl;

        if (vlan_output.size() > 0)
        {
            ostr << vlan_output.join("\n").toStdString()
                 << endl;
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
        QStringList carp_output;
        QStringList carp_interfaces;

        FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);

            if ( ! iface->isFailoverInterface()) continue;

            FWObject *failover_group =
                iface->getFirstByType(FailoverClusterGroup::TYPENAME);
            if (failover_group && failover_group->getStr("type") == "carp")
            {
                carp_interfaces << iface->getName().c_str();
                carp_output << updateCARPInterface(iface, failover_group);
            }
        }

        // issue "sync_carp_interfaces" call even when we have none, it will
        // delete those that might exist on the firewall
        ostr << listAllCARPConfgLine(carp_interfaces).toStdString()
             << endl;

        if (carp_interfaces.size() > 0)
        {
            ostr << carp_output.join("\n").toStdString() << endl;
        }
    }


    if ( options->getBool("configure_interfaces") ) 
    {
        ostr << endl;

        std::auto_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                fw->getStr("host_OS")));

        list<FWObject*> all_interfaces = fw->getByTypeDeep(Interface::TYPENAME);
        all_interfaces.sort(compare_names);

        QStringList configure_intf_commands;
        QStringList intf_names;
        QStringList ipv6_names;

        for (list<FWObject*>::iterator i=all_interfaces.begin();
             i != all_interfaces.end(); ++i )
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);

            //if (!iface->isRegular()) continue;

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
                intf_names << iface->getName().c_str();
                ipv6_names << iface->getName().c_str();

                const InetAddr *netmask = iface->getNetmaskPtr();

                list<pair<InetAddr,InetAddr> > all_addresses;

                for (list<FWObject*>::iterator j = all_addr.begin();
                     j != all_addr.end(); ++j)
                {
                    Address *iaddr = Address::cast(*j);
                    const InetAddr *ipaddr = iaddr->getAddressPtr();
                    const InetAddr *ipnetm = iaddr->getNetmaskPtr();
                    all_addresses.push_back(
                        pair<InetAddr,InetAddr>(*ipaddr, *ipnetm));
                }

                set<const Address*>::iterator it;
                for (it=virtual_addresses.begin(); it!=virtual_addresses.end(); ++it)
                {
                    const Address *addr = *it;
                    FWObject *iaddr = findAddressFor(addr, fw );
                    if (iaddr!=NULL)
                    {
                        Interface *iface_2 = Interface::cast(iaddr->getParent());
                        if (iface_2 == iface)
                        {
                            all_addresses.push_back(
                                pair<InetAddr,InetAddr>(
                                    *(addr->getAddressPtr()), *netmask));
                        }
                    }
                }

                configure_intf_commands << updateAddressesOfInterface(
                    iface, all_addresses);

            }
        }

        QString list_command;
        list_command = listAllInterfacesConfigLine(ipv6_names, true);
        if (!list_command.isEmpty())
            configure_intf_commands.push_front(list_command);

        list_command = listAllInterfacesConfigLine(intf_names, false);
        if (!list_command.isEmpty())
            configure_intf_commands.push_front(list_command);

        ostr << configure_intf_commands.join("\n").toStdString();
        ostr << endl;
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

            pfsync_output << updatePfsyncInterface(iface, state_sync_group);

            break;
        }

        ostr << listAllPfsyncConfgLine(have_pfsync_interfaces).toStdString()
             << endl;

        if (have_pfsync_interfaces)
        {
            ostr << pfsync_output.join("\n").toStdString()
                 << endl;
        }
    }

    return ostr.str();
}

QString OSConfigurator_bsd::listAllInterfacesConfigLine(QStringList , bool )
{
    return "";
}

QString OSConfigurator_bsd::updateAddressesOfInterface(
    Interface *iface, list<pair<InetAddr,InetAddr> > all_addresses)
{
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

    return QString("update_addresses_of_interface ") +
        "\"" + arg1.join(" ") + "\"" + " \"\"";
}



QString OSConfigurator_bsd::listAllVlansConfgLine(QStringList vlan_names)
{
    return QString("sync_vlan_interfaces %1").arg(vlan_names.join(" "));
}


QString OSConfigurator_bsd::updateVlansOfInterface(Interface *iface,
                                                   QStringList vlan_names)
{
    return QString("update_vlans_of_interface \"%1 %2\"")
        .arg(iface->getName().c_str())
        .arg(vlan_names.join(" "));
}

QString OSConfigurator_bsd::listAllCARPConfgLine(QStringList carp_names)
{
    return  QString("sync_carp_interfaces %1").arg(carp_names.join(" "));
}

QString OSConfigurator_bsd::updateCARPInterface(Interface *iface,
                                                FWObject *failover_group)
{
    Configlet configlet(fw, "bsd", "carp_interface");
    return updateCARPInterfaceInternal(iface, failover_group, &configlet);
}

QString OSConfigurator_bsd::updateCARPInterfaceInternal(
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

    return configlet->expand();
}

QString OSConfigurator_bsd::listAllPfsyncConfgLine(bool have_pfsync)
{
    return QString("sync_pfsync_interfaces %1").arg(have_pfsync?"pfsync0":"");
}

/*
 *  http://www.kernel-panic.it/openbsd/carp/index.html
 *  http://www.openbsd.org/faq/pf/carp.html
 * pfsync configuration:
 *
 * ifconfig pfsyncN syncdev syncdev [syncpeer syncpeer]
 */

QString OSConfigurator_bsd::updatePfsyncInterface(
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
    return configlet.expand();
}



