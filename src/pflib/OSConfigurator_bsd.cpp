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

#include "OSConfigurator_bsd.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/StateSyncClusterGroup.h"

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

void OSConfigurator_bsd::processFirewallOptions() 
{
}

void OSConfigurator_bsd::addVirtualAddressForNAT(const Network*)
{
}

void OSConfigurator_bsd::addVirtualAddressForNAT(const Address *addr)
{
    if (virtual_addresses.empty() || 
	find(virtual_addresses.begin(),
             virtual_addresses.end(),
             *(addr->getAddressPtr())) == virtual_addresses.end())
    {
        FWObject *iaddr = findAddressFor(addr, fw );
        if (iaddr!=NULL)
        {
            Address *iaddr_addr = Address::cast(iaddr);
            assert(iaddr_addr!=NULL);
            Interface *iface = Interface::cast(iaddr->getParent());
            assert(iface!=NULL);

            output << "add_addr " << addr->getAddressPtr()->toString() << " "
                   << iaddr_addr->getNetmaskPtr()->toString() <<  " "
                   << iface->getName() << endl;
        
            virtual_addresses.push_back(*(addr->getAddressPtr()));
        } else
            warning("Can not add virtual address " +
                    addr->getAddressPtr()->toString() );
    }
}

int OSConfigurator_bsd::prolog()
{
    return 0;
}

void  OSConfigurator_bsd::printPathForAllTools(const string &)
{
}

void  OSConfigurator_bsd::printFunctions()
{
    FWOptions* options=fw->getOptionsObject();

    output                                                     << endl;
    output << "log() {"                                        << endl;
    output << "  test -x \"$LOGGER\" && $LOGGER -p info \"$1\"" << endl;
    output << "}"                                              << endl;
    output                                                     << endl;

    output << "add_addr() {" << endl;
    output << "  addr=$1"    << endl;
    output << "  nm=$2"      << endl;
    output << "  dev=$3"     << endl;
    output << "  ( ifconfig $dev | egrep -q \"inet +${addr} \" ) || " << endl;
    output << "    { "       << endl;
    output << "      echo \"$dev: $addr/$nm\"" << endl;
    output << "      ifconfig $dev inet $addr netmask $nm alias" << endl; 
    output << "    } "       << endl;
    output << "}"            << endl;
    output << endl;
    output << endl;

    if (options->getBool("dynAddr"))
    {
        output << "getaddr() {"                       << endl;
        output << "  intf=$1"                         << endl;
        output << "  varname=$2"                      << endl;
        output << "  L=`ifconfig $1 | grep 'inet '`"  << endl;
        output << "  if [ -z \"$L\" ]; then"          << endl;
        output << "      L=\"inet 0.0.0.0/32\""       << endl;
        output << "  fi"                              << endl;
        output << "  set $L"                          << endl;
        output << "  a=$2"                            << endl;
        output << "  eval \"$varname=$a\""            << endl;
        output << "}"                                 << endl;
        output << endl;
        output << endl;

/*
 * get addresses of dynamic interfaces
 */
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
                    output << "getaddr "
                           << iface->getName()
                           << "  "
                           << getInterfaceVarName(iface)
                           << endl;
            }
        }
    }

    output << endl;
}

void  OSConfigurator_bsd::configureInterfaces()
{
    FWOptions* options=fw->getOptionsObject();

    if ( options->getBool("configure_interfaces") ) 
    {
        output << endl;

        FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);

            if (!iface->isRegular()) continue;
            if (iface->isFailoverInterface()) continue;

            FWObjectTypedChildIterator j=iface->findByType(IPv4::TYPENAME);
            for ( ; j!=j.end(); ++j ) 
            {
                Address *iaddr = Address::cast(*j);
                output << "add_addr "
                       << iaddr->getAddressPtr()->toString() << " "
                       << iaddr->getNetmaskPtr()->toString() << " "
                       << iface->getName() << endl;
                virtual_addresses.push_back(*(iaddr->getAddressPtr()));
            }
        }
        output << endl;
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

                // TODO: How do we add support for ipv6 CARP interfaces ?
                // Currently support only IPv4 and only one address 
                // per carp interface
                IPv4 *ipv4 = IPv4::cast(iface->getFirstByType(IPv4::TYPENAME));
                const InetAddr *addr = ipv4->getAddressPtr();
                const InetAddr *mask = ipv4->getNetmaskPtr();

                carp_output << " " << addr->toString()
                            << " netmask " << mask->toString();
                carp_output << endl;
            }
        }
        if (have_carp_interfaces)
        {
            output << "$SYSCTL -w net.inet.carp.allow=1" << endl;
            output << carp_output.str() << endl;
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
        for ( ; i!=i.end(); ++i ) 
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
            output << pfsync_output.str() << endl;
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
            output << vlan_output.str() << endl;
        }
    }
}


