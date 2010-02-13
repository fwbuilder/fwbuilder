/* 

                          Firewall Builder

                 Copyright (C) 2002,2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#include <QTextStream>
#include <QString>

#include <algorithm>
#include <functional>
#include <iostream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string OSConfigurator_bsd::myPlatformName() { return "BSD"; }

string OSConfigurator_bsd::getInterfaceVarName(FWObject *iface)
{
    return string("i_") + iface->getName();
}

string OSConfigurator_bsd::printKernelVarsCommands() 
{
    return "";
}

string OSConfigurator_bsd::updateAddressesOfInterfaceCall(
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

    return string("update_addresses_of_interface ") +
        "\"" +
        arg1.join(" ").toStdString() +
        "\"" +
        " \"\"";
}


void OSConfigurator_bsd::addVirtualAddressForNAT(const Network*)
{
}

/**
 * This method is called from  NATCompiler_pf::addVirtualAddress::processNext()
 */
void OSConfigurator_bsd::addVirtualAddressForNAT(const Address *addr)
{
    FWObject *iaddr = findAddressFor(addr, fw );
    if (iaddr!=NULL)
    {
        virtual_addresses.insert(addr);
    } else
        warning("Can not add virtual address " +
                addr->getAddressPtr()->toString() );
}

int OSConfigurator_bsd::prolog()
{
    return 0;
}

string OSConfigurator_bsd::printFunctions()
{
    ostringstream ostr;

    FWOptions* options=fw->getOptionsObject();

    Configlet functions(fw, "bsd", "shell_functions");
    functions.removeComments();
    functions.setVariable("dyn_addr", options->getBool("dynAddr"));

    if (options->getBool("dynAddr"))
    {
/*
 * get addresses of dynamic interfaces
 */
        QString script_buffer;
        QTextStream ostr(&script_buffer, QIODevice::WriteOnly);
        FWObjectTypedChildIterator j=fw->findByType(Interface::TYPENAME);
        for ( ; j!=j.end(); ++j ) 
        {
            Interface *iface=Interface::cast(*j);
        
            if ( iface->isDyn() )
            {
/* if interface name ends with '*', this is a wildcard interface. Do
 * not get its address at this time. 
 *
 * Do we support wildcard interfaces on *BSD at all ?
 */
                if (iface->getName().find("*")==string::npos)
                    ostr << "getaddr "
                         << iface->getName().c_str()
                         << "  "
                         << getInterfaceVarName(iface).c_str()
                         << "\n";
            }
        }
        functions.setVariable("get_dyn_addr_commands", script_buffer);
    } else
        functions.setVariable("get_dyn_addr_commands", "");

    ostr << functions.expand().toStdString();

    if ( options->getBool("configure_interfaces") ) 
    {
        Configlet update_addresses(fw, "bsd", "update_addresses");
        update_addresses.removeComments();
        ostr << update_addresses.expand().toStdString();
    }

    return ostr.str();
}

string OSConfigurator_bsd::configureInterfaces()
{
    ostringstream ostr;
    FWOptions* options = fw->getOptionsObject();

    if ( options->getBool("configure_interfaces") ) 
    {
        ostr << endl;

        FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);

            if (!iface->isRegular()) continue;
            if (iface->isFailoverInterface()) continue;

            list<FWObject*> all_addr = iface->getByType(IPv4::TYPENAME);
            list<FWObject*> all_ipv6 = iface->getByType(IPv6::TYPENAME);
            all_addr.insert(all_addr.begin(), all_ipv6.begin(), all_ipv6.end());

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

            ostr << updateAddressesOfInterfaceCall(iface, all_addresses) << endl;

        }
        ostr << endl;
    }

    if ( options->getBool("configure_carp_interfaces") ) 
    {
/*
 * Compiler::processFailoverGroup copies interfaces of the cluster to
 * the member firewall objects. This means when we scan interfaces of
 * the firewall here, we get both its normal interfaces and a copy of
 * cluster interfaces. 
 *
 * Need to generate commands
 *
 *    ifconfig carpN create
 *
 *    ifconfig carpN vhid vhid [pass password] [carpdev carpdev]   \
 *       [advbase advbase] [advskew advskew] [state state] ipaddress   \
 *       netmask mask 
 *
 * for pfsync and CARP see http://www.kernel-panic.it/openbsd/carp/
 * "Redundant firewalls with OpenBSD, CARP and pfsync"
 */
        ostringstream carp_output;
        bool have_carp_interfaces = false;

        FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);

            if ( ! iface->isFailoverInterface()) continue;

            // failover_master and base_device are set in Compiler::processFailoverGroup
            FWOptions *ifopt = (Interface::cast(iface))->getOptionsObject();
            assert(ifopt != NULL);

            bool master = ifopt->getBool("failover_master");
            string base_interface = ifopt->getStr("base_device");

            FWObject *failover_group =
                iface->getFirstByType(FailoverClusterGroup::TYPENAME);
            if (failover_group && failover_group->getStr("type") == "carp")
            {
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

                have_carp_interfaces = true;


                carp_output << "ifconfig " << iface->getName() << " create" << endl;

                carp_output << "ifconfig " << iface->getName()
                            << " vhid " << vhid
                            << " pass " << carp_password
                            << " ";
                if (!base_interface.empty())
                    carp_output << " carpdev " << base_interface;

                // the default for advbase is 1, skip it if use_advskew < 1
                // (can be -1 if not defined at all)
                if (advbase > 1)
                    carp_output << " advbase " << advbase;

                int use_advskew;
                if (master)
                    use_advskew = master_advskew;
                else
                    use_advskew = default_advskew;

                // the default for advskew is 0, skip it if use_advskew == 0
                if (use_advskew > 0)
                    carp_output << " advskew " << use_advskew;

                carp_output << endl;

                list<FWObject*> all_addr = iface->getByType(IPv4::TYPENAME);
                list<FWObject*> all_ipv6 = iface->getByType(IPv6::TYPENAME);
                all_addr.insert(all_addr.begin(), all_ipv6.begin(), all_ipv6.end());

                for (list<FWObject*>::iterator j = all_addr.begin();
                     j != all_addr.end(); ++j) 
                {
                    Address *address = Address::cast(*j);
                    const InetAddr *addr = address->getAddressPtr();
                    const InetAddr *mask = address->getNetmaskPtr();

                    carp_output << "ifconfig " << iface->getName();
                    if (addr->isV6())
                        carp_output << " inet6";
                    else
                        carp_output << " inet";

                    carp_output << " " << addr->toString();

                    carp_output << " prefixlen " << mask->getLength();

                    carp_output << endl;
                }

            }
        }

        if (have_carp_interfaces)
        {
            ostr << "$SYSCTL -w net.inet.carp.allow=1" << endl;
            ostr << carp_output.str() << endl;
        }
    }


    if ( options->getBool("configure_pfsync_interfaces") ) 
    {
        bool have_pfsync_interfaces = false;
        ostringstream pfsync_output;
        /*
         *  http://www.kernel-panic.it/openbsd/carp/index.html
         *  http://www.openbsd.org/faq/pf/carp.html
         * pfsync configuration:
         *
         * ifconfig pfsyncN syncdev syncdev [syncpeer syncpeer]
         */
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

            pfsync_output << "ifconfig pfsync0 ";
            pfsync_output << "syncdev " << iface->getName() << " ";

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

                    pfsync_output << "syncpeer " << addr->toString();
                }
            }
            pfsync_output << endl;
            pfsync_output << "ifconfig pfsync0 up" << endl;
        }
        if (have_pfsync_interfaces)
        {
            ostr << pfsync_output.str() << endl;
        }
    }

    if ( options->getBool("configure_vlan_interfaces") ) 
    {
        bool have_vlan_interfaces = false;
        ostringstream vlan_output;

        // http://blog.scottlowe.org/2007/08/31/vlan-interfaces-with-openbsd-41/
        // ifconfig <VLAN interface name> vlan <VLAN ID> vlandev <physical network device>
        FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);

            FWObjectTypedChildIterator si=iface->findByType(Interface::TYPENAME);
            for ( ; si!=si.end(); ++si ) 
            {
                Interface *subinterface = Interface::cast(*si);
                assert(subinterface);

                if (subinterface->getOptionsObject()->getStr("type") == "8021q")
                {
                    have_vlan_interfaces = true;

                    vlan_output << "ifconfig " << subinterface->getName()
                                << " create"
                                << endl;

                    vlan_output << "ifconfig " << subinterface->getName()
                                << " vlan " 
                                << subinterface->getOptionsObject()->getInt("vlan_id")
                                << " vlandev " << iface->getName()
                                << endl;
                }
            }
        }
        if (have_vlan_interfaces)
        {
            ostr << vlan_output.str() << endl;
        }
    }
    return ostr.str();
}

void OSConfigurator_bsd::setKernelVariable(Firewall *fw,
                                           const string &var_name,
                                           Configlet *configlet)
{
    FWOptions* options = fw->getOptionsObject();
    string s;

    s = options->getStr(var_name);
    if (!s.empty())
    {
        configlet->setVariable(QString("have_") + var_name.c_str(), 1);
        configlet->setVariable(var_name.c_str(), s=="1" || s=="on" || s=="On");
    }
}


