/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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



#include "OSConfigurator_linux24.h"

#include "fwbuilder/InetAddr.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/MultiAddress.h"
#include "fwbuilder/FailoverClusterGroup.h"

#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"
#include "Configlet.h"


#ifndef _WIN32
#  include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>

#include <assert.h>

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QtDebug>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


string OSConfigurator_linux24::printVerifyInterfacesCommands()
{
    // string os_family = Resources::os_res[fw->getStr("host_OS")]->
    //     getResourceStr("/FWBuilderResources/Target/family");
    QStringList interfaces_to_check;
    list<FWObject*> interfaces = fw->getByTypeDeep(Interface::TYPENAME);
    list<FWObject*>::iterator i;
    for (i=interfaces.begin(); i!=interfaces.end(); ++i )
    {
        QString iface_name = (*i)->getName().c_str();
/* if interface name ends with '*', this is a wildcard interface. Do
 * not check if it exists.
 */
        if (!iface_name.contains("*") && interfaces_to_check.indexOf(iface_name) == -1)
        {
            interfaces_to_check.push_back((*i)->getName().c_str());
        }
    }

    Configlet verify_interfaces(fw, "linux24", "verify_interfaces");
    verify_interfaces.setVariable("have_interfaces", interfaces_to_check.size());
    verify_interfaces.setVariable("interfaces", interfaces_to_check.join(" "));
    return verify_interfaces.expand().toStdString();
}

/*
 * Generate calls to the shell function update_addresses_of_interface
 * to add or remove ip addresses of interfaces. The following cases
 * are supported, depending on the value of
 * @add_virtual_addresses_for_nat and @configure_interfaces
 *
 * configure_interfaces == false && add_virtual_addresses_for_nat == false:  
 *   do not generate any commands
 *
 * configure_interfaces == false && add_virtual_addresses_for_nat == true:  
 *   use only  virtual_addresses_for_nat, add normal addresses of the interface
 *   to the list of addresses we should ignore
 * 
 * configure_interfaces == true && add_virtual_addresses_for_nat == false:
 *   ignore  virtual_addresses_for_nat
 * 
 * configure_interfaces == true && add_virtual_addresses_for_nat == true:
 *   use  virtual_addresses_for_nat
 * 
 * 
 */
string OSConfigurator_linux24::printInterfaceConfigurationCommands()
{
    FWOptions* options = fw->getOptionsObject();

    std::unique_ptr<interfaceProperties> int_prop(
        interfacePropertiesObjectFactory::getInterfacePropertiesObject(
            fw->getStr("host_OS")));

    Configlet script(fw, "linux24", "configure_interfaces");
    script.removeComments();
    script.collapseEmptyStrings(true);

    list<FWObject*> interfaces = fw->getByTypeDeep(Interface::TYPENAME);
    bool need_promote_command = false;
    QStringList gencmd;
    list<FWObject*>::iterator i;
    for (i=interfaces.begin(); i!=interfaces.end(); ++i )
    {
        Interface *iface = Interface::cast(*i);
        assert(iface);
        string iface_name = iface->getName();

        QStringList update_addresses;
        QStringList ignore_addresses;

        if (int_prop->manageIpAddresses(iface, update_addresses, ignore_addresses))
        {
            if (options->getBool("manage_virtual_addr") &&
                virtual_addresses_for_nat.count(iface_name) > 0)
                update_addresses.push_back(
                    virtual_addresses_for_nat[iface_name].c_str());

            // generate update_addresses calls even if interface has
            // no addresses in fwbuilder. This makes sure all
            // addresses it might have on the machine will be
            // removed. Say, interface was regular and had an address
            // and then user converted it to unnumbered. In this case
            // the address should be removed.

            gencmd.push_back(
                printUpdateAddressCommand(iface, update_addresses, ignore_addresses));

            // update_addresses list looks like this:
            // ("eth0", "22.22.22.22/24", "22.22.22.23/24") 
            // I need to add "promote" command only when there is more than 1 address.
            need_promote_command |= (update_addresses.size() > 2);
        }

        known_interfaces.push_back(iface_name);
    }

    script.setVariable("have_interfaces", interfaces.size() > 0);
    script.setVariable("need_promote_command", need_promote_command);
    script.setVariable("configure_interfaces_script", gencmd.join("\n"));
    return script.expand().toStdString() + "\n";
}


/*
 * printVirtualAddressesForNatCommands() deals with the case when we
 * add virtual addresses for NAT but do not configure normal addresses
 * of interfaces
 */
string OSConfigurator_linux24::printVirtualAddressesForNatCommands()
{
    QStringList gencmd;
    std::unique_ptr<interfaceProperties> int_prop(
        interfacePropertiesObjectFactory::getInterfacePropertiesObject(
            fw->getStr("host_OS")));

    list<FWObject*> interfaces = fw->getByTypeDeep(Interface::TYPENAME);
    list<FWObject*>::iterator i;
    for (i=interfaces.begin(); i!=interfaces.end(); ++i )
    {
        Interface *iface = Interface::cast(*i);
        assert(iface);
        string iface_name = iface->getName();

        QStringList update_addresses;
        QStringList ignore_addresses;

        // Return value of InterfaceProperties::manageIpAddresses()
        // signals if we should manage addresses of the interface at
        // all, so it is useful even if we are not going to use the
        // lists.

        if (int_prop->manageIpAddresses(iface, update_addresses, ignore_addresses))
        {
            // we should not configure normal addresses of interfaces, but
            // should configure virtual addresses for nat. This means we should
            // add normal addresses to the ignore_addresses list.

            ignore_addresses += update_addresses;
            update_addresses.clear();

            if (virtual_addresses_for_nat.count(iface_name) > 0)
            {
                update_addresses.push_back(
                    virtual_addresses_for_nat[iface_name].c_str());

                gencmd.push_back(
                    printUpdateAddressCommand(iface, update_addresses, ignore_addresses));
            }
        }
        
        known_interfaces.push_back(iface_name);
    }

    return gencmd.join("\n").toStdString() + "\n";
}

string OSConfigurator_linux24::printCommandsToClearKnownInterfaces()
{
    if (fw->getOptionsObject()->getBool("clear_unknown_interfaces") &&
        known_interfaces.size() > 0)
    {
        // last resort protection: if there are no interfaces with
        // addresses in fwbuilder configuration, we should not kill
        // all addresses of all interfaces on the firewall
        string res = "clear_addresses_except_known_interfaces ";
        for (list<string>::iterator it=known_interfaces.begin();
             it!=known_interfaces.end(); ++it)
        {
            res += *it + " ";
        }
        return res;
    }
    return "";
}


QString OSConfigurator_linux24::printUpdateAddressCommand(
    Interface *intf, QStringList &update_addresses, QStringList &ignore_addresses)
{
    QStringList out;
    update_addresses.push_front(intf->getName().c_str());
    out.push_back("update_addresses_of_interface");
    out.push_back("\"" + update_addresses.join(" ") + "\"");
    out.push_back("\"" + ignore_addresses.join(" ") + "\"");
    return out.join(" ");
}

string OSConfigurator_linux24::printVlanInterfaceConfigurationCommands()
{
    ostringstream out;

    // Generate code to call shell function "update_vlans"

    // http://www.cyberciti.biz/tips/howto-configure-linux-virtual-local-area-network-vlan.html
    // vconfig add eth0 5

    QStringList all_vlans;

    FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
    for ( ; i!=i.end(); ++i ) 
    {
        Interface *iface = Interface::cast(*i);
        assert(iface);

        QStringList vlan_interfaces;
        QString parent_interface = iface->getName().c_str();
        FWObjectTypedChildIterator si=iface->findByType(Interface::TYPENAME);
        for ( ; si!=si.end(); ++si ) 
        {
            Interface *subinterface = Interface::cast(*si);
            assert(subinterface);

            if (subinterface->getOptionsObject()->getStr("type") == "8021q")
            {
                vlan_interfaces.push_back(subinterface->getName().c_str());
                string name_type = "";
                int vlan_id = subinterface->getOptionsObject()->getInt("vlan_id");
                QString sintf_name = subinterface->getName().c_str();
                QString vlan_name;
                QStringList supported_vlan_names;
                vlan_name.sprintf("vlan%04d", vlan_id);
                supported_vlan_names.append(vlan_name);
                if (vlan_name == sintf_name) name_type = "VLAN_PLUS_VID";
                else
                {
                    vlan_name.sprintf("vlan%d", vlan_id);
                    supported_vlan_names.append(vlan_name);
                    if (vlan_name == sintf_name) name_type = "VLAN_PLUS_VID_NO_PAD";
                    else
                    {
                        vlan_name.sprintf("%s.%04d", iface->getName().c_str(), vlan_id);
                        supported_vlan_names.append(vlan_name);
                        if (vlan_name == sintf_name) name_type = "DEV_PLUS_VID_PAD";
                        else
                        {
                            vlan_name.sprintf("%s.%d", iface->getName().c_str(), vlan_id);
                            supported_vlan_names.append(vlan_name);
                            if (vlan_name == sintf_name) name_type = "DEV_PLUS_VID_NO_PAD";
                        }
                    }
                }

                if (name_type.empty())
                {
                    QString err_str(
                        "Name of the VLAN interface '%1' "
                        "does not match any supported naming "
                        "type for VLAN interfaces. Possible names: %2");
                    abort(err_str.arg(sintf_name).
                          arg(supported_vlan_names.join(", ")).toStdString());
                }
            }
        }

        if (vlan_interfaces.size() > 0)
        {
            out << "update_vlans_of_interface \""
                << parent_interface.toStdString()
                << " "
                << vlan_interfaces.join(" ").toStdString()
                << "\""
                << endl;
            foreach(QString intf, vlan_interfaces)
            {
                all_vlans.push_back(QString("%1@%2").arg(intf).arg(parent_interface));
            }
        }
    }
    out << "clear_vlans_except_known"
        << " " << all_vlans.join(" ").toStdString() << endl;
    return out.str();
}

string OSConfigurator_linux24::printBridgeInterfaceConfigurationCommands()
{
    /*
     * http://www.linuxfoundation.org/en/Net:Bridge#Manual_Configuration
     * # brctl addbr br12
     * # brctl addif br12 eth0
     * # brctl addif br12 eth1 
     * # ifconfig br12 up
     */

    list<Interface*> all_bridges = fw->getInterfacesByType("bridge");
    QStringList names;
    for (list<Interface*>::iterator it=all_bridges.begin(); it!=all_bridges.end(); ++it)
        names.push_back((*it)->getName().c_str());

    QStringList gencmd;
    gencmd.push_back("sync_bridge_interfaces " + names.join(" "));

    for (list<Interface*>::iterator it=all_bridges.begin(); it!=all_bridges.end(); ++it)
    {
        Interface *iface = Interface::cast(*it);
        assert(iface);

        QStringList out;
        QStringList bridge_interfaces;

        if (iface->getOptionsObject()->getStr("type") == "bridge")
        {
            out.push_back("update_bridge");
            out.push_back(iface->getName().c_str());

            FWObjectTypedChildIterator si=iface->findByType(Interface::TYPENAME);
            for ( ; si!=si.end(); ++si ) 
            {
                Interface *subinterface = Interface::cast(*si);
                assert(subinterface);

                bridge_interfaces.push_back(subinterface->getName().c_str());
            }

            out.push_back("\"" + bridge_interfaces.join(" ") + "\"");
            gencmd.push_back(out.join(" "));
            
            out.clear();
            bool enable_stp = iface->getOptionsObject()->getBool("enable_stp");
            out.push_back("$BRCTL stp");
            out.push_back(iface->getName().c_str());
            out.push_back((enable_stp)?"on":"off");
            gencmd.push_back(out.join(" "));
        }
    }
    return gencmd.join("\n").toStdString() + "\n";
}

string OSConfigurator_linux24::printBondingInterfaceConfigurationCommands()
{
    /*
     * http://www.linuxfoundation.org/en/Net:Bonding#Configuring_Bonding_Manually
     *
     *  modprobe bonding mode=balance-alb miimon=100
     *  modprobe e100
     *  ifconfig bond0 192.168.1.1 netmask 255.255.255.0 up
     *  ifenslave bond0 eth0
     *  ifenslave bond0 eth1
     */

    QStringList gencmd;
    QStringList bonding_interfaces;
    QString module_parameters;

    FWObjectTypedChildIterator i = fw->findByType(Interface::TYPENAME);
    for ( ; i!=i.end(); ++i ) 
    {
        Interface *iface = Interface::cast(*i);
        assert(iface);
        QStringList out;
        if (iface->getOptionsObject()->getStr("type") == "bonding")
        {
            /*
             * current implementation of function load_bonding_module()
             * in the configlet loads the module once, which means we can
             * only support the same parameters for all bonding interfaces.
             * Take parameters from the first bonding interface and use that
             * in the call to load_bonding_module()
             *
             * However, check if parameters for the subsequent bonding
             * interfaces are different and issue warning.
             */

            QString mode = iface->getOptionsObject()->getStr("bonding_policy").c_str();
            QString xmit_hash_policy = iface->getOptionsObject()->getStr("xmit_hash_policy").c_str();
            QString driver_opts = iface->getOptionsObject()->getStr("bondng_driver_options").c_str();
            QStringList params;

            if (!mode.isEmpty())
                params.push_back("mode=" + mode);
            if (!xmit_hash_policy.isEmpty())
                params.push_back("xmit_hash_policy=" + xmit_hash_policy);
            if (!driver_opts.isEmpty())
                params.push_back(driver_opts);

            if (module_parameters.isEmpty())
            {
                module_parameters = params.join(" ");
            } else
            {
                if (module_parameters != params.join(" "))
                {
                    warning(
                        QString("Different protocol parameters for multiple "
                                "bonding interfaces are not supported at "
                                "this time. Module 'bonding' "
                                "will be loaded with the following parameters: '%1'")
                        .arg(module_parameters).toStdString());
                }
            }

            out.push_back("update_bonding");
            out.push_back(iface->getName().c_str());
            bonding_interfaces.push_back(iface->getName().c_str());

            QStringList bonding_interfaces;
            FWObjectTypedChildIterator j = iface->findByType(Interface::TYPENAME);
            for ( ; j!=j.end(); ++j ) 
            {
                Interface *subint = Interface::cast(*j);
                if (subint->getOptionsObject()->getStr("type") == "" ||
                    subint->getOptionsObject()->getStr("type") == "ethernet")
                    bonding_interfaces.push_back(subint->getName().c_str());
            }
            out.push_back(bonding_interfaces.join(" "));
            gencmd.push_back(out.join(" "));
        }
    }

    if (bonding_interfaces.size())
    {
        gencmd.push_front(
            QString("load_bonding_module \"%1\" max_bonds=%2 %3")
            .arg(bonding_interfaces.join(" "))
            .arg(bonding_interfaces.size())
            .arg(module_parameters));
        gencmd.push_back(
            QString("clear_bonding_except_known %1")
            .arg(bonding_interfaces.join(" ")));
    } else
        gencmd.push_front("unload_bonding_module");

    return gencmd.join("\n").toStdString() + "\n";
}

string OSConfigurator_linux24::printDynamicAddressesConfigurationCommands()
{
    ostringstream out;
/*
 * get addresses of dynamic interfaces
 */
    list<FWObject*> interfaces = fw->getByTypeDeep(Interface::TYPENAME);
    list<FWObject*>::iterator j;
    for (j=interfaces.begin(); j!=interfaces.end(); ++j )
    {
        Interface *iface = Interface::cast(*j);
        
        if ( iface->isDyn() )
        {
/* if interface name ends with '*', this is a wildcard interface. Do
 * not get its address at this time. */
            if (iface->getName().find("*")==string::npos)
            {
                out << "getaddr "
                    << iface->getName()
                    << "  "
                    << getInterfaceVarName(iface, false)
                    << endl;
                out << "getaddr6 "
                    << iface->getName()
                    << "  "
                    << getInterfaceVarName(iface, true)
                    << endl;

                out << "getnet "
                    << iface->getName()
                    << "  "
                    << getInterfaceVarName(iface, false) << "_network"
                    << endl;
                out << "getnet6 "
                    << iface->getName()
                    << "  "
                    << getInterfaceVarName(iface, true) << "_network"
                    << endl;
            }
        }
    }
    return out.str();
}

bool OSConfigurator_linux24::validateInterfaces()
{
    FWOptions* options = fw->getOptionsObject();
    if ( options->getBool("configure_bridge_interfaces") ) 
    {
        /*
         * Per #315:
         *
         * The test should scan all subinterfaces of each interface and
         * try to find inetrfaces wth the same name, then check the
         * type. So, for the combination br0/br1, the type of br1
         * is (probably) "bridge" which is not allowed.
         */
        FWObjectTypedChildIterator i = fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);
            FWObjectTypedChildIterator j = iface->findByType(Interface::TYPENAME);
            for ( ; j!=j.end(); ++j ) 
            {
                Interface *subinterface = Interface::cast(*j);
                FWObject::const_iterator it = find_if(
                    fw->begin(), fw->end(),
                    FWObjectNameEQPredicate(subinterface->getName()));
                if (it != fw->end() && Interface::isA(*it))
                {
                    Interface *other_iface = Interface::cast(*it);
                    // Have top-level interface with the same name
                    if (other_iface->getOptionsObject()->getStr("type") == "bridge")
                    {
                        QString err(
                            "Subinterface '%1' of interface '%2' has the same name as "
                            "another bridge interface of the firewall '%3'. "
                            "The configuration where bridge interface is "
                            "a subinterface of another interface is not supported.");
                        abort(
                            err.arg(subinterface->getName().c_str()).
                            arg(iface->getName().c_str()).
                            arg(fw->getName().c_str()).toStdString());
                        return false;
                    }                    
                }
            }
        }
    }

    if (options->getBool("configure_vlan_interfaces"))
    {
        /*
         * Per #324:
         *
         * Unsupported configurations: 
         *
         * vlan interfaces under bridge interface (e.g. br0 = [eth1,
         * eth2], vlan inetrface br0.100 is not supported)
         *
         * bridge interface as part of bonding interface (e.g. bond0 =
         * [br0, br1]) (This is covered by the case above, including
         * unusual interface names)
         *
         * vlan interface as a slave of bonding interface (e.g. eth0.100,
         * eth1.100, bond0 = [eth0.100, eth1.100]). Only regular
         * interfaces can be slaves of bonding interface. If subinterface
         * type is "slave" but its name matches one of the vlan interface
         * regexes, assume this is vlan. Slave subintrfaces do not have to
         * be copies, one can have "eth4" only once, as a slave, so here
         * we rely on the subinterface type.
         *
         *  Allowed configurations:
         *
         * vlans can be created under bonding interface (e.g. bond0.100),
         * both regular interfaces and vlans can be bridge ports.  }
         */

        FWObjectTypedChildIterator i = fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);
            FWObjectTypedChildIterator j = iface->findByType(Interface::TYPENAME);
            for ( ; j!=j.end(); ++j ) 
            {
                Interface *subinterface = Interface::cast(*j);

                if ( options->getBool("configure_bridge_interfaces") ) 
                {
                    if (subinterface->getOptionsObject()->getStr("type") == "8021q" &&
                        iface->getOptionsObject()->getStr("type") == "bridge")
                    {
                        QString err(
                            "Vlan subinterfaces of bridge interfaces "
                            "are not supported. "
                            "Interface '%1', subinterface '%2'");
                        abort(
                            err.arg(iface->getName().c_str()).
                            arg(subinterface->getName().c_str()).toStdString());
                    }
                }

                if ( options->getBool("configure_bonding_interfaces"))
                {
                    if (subinterface->getOptionsObject()->getStr("type") != "8021q" &&
                        iface->getOptionsObject()->getStr("type") == "bonding")
                    {
                        QString subint_name = subinterface->getName().c_str();
                        QRegExp vlan1("[a-zA-Z-]+\\d{1,}\\.\\d{1,}");
                        QRegExp vlan2("vlan\\d{1,}");
                        if (vlan1.indexIn(subint_name) != -1 ||
                            vlan1.indexIn(subint_name) != -1)
                        {
                            QString err(
                                "Vlan subinterfaces as slaves of bonding interfaces "
                                "are not supported. "
                                "Interface '%1', subinterface '%2'");
                            abort(
                                err.arg(iface->getName().c_str()).
                                arg(subinterface->getName().c_str()).toStdString());
                        }
                    }
                }

            }
        }
    }

    return true;
}
