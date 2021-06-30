/* 

                          Firewall Builder

                 Copyright (C) 2002-2011 NetCitadel, LLC

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
#include "fwbuilder/XMLTools.h"

#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include <QTextStream>
#include <QString>

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>


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

void OSConfigurator_bsd::addVirtualAddressForNAT(const Network*)
{
}

/**
 * This method is called from  NATCompiler_pf::addVirtualAddress::processNext()
 */
void OSConfigurator_bsd::addVirtualAddressForNAT(const Address *addr)
{
    FWObject *iaddr = findAddressFor(addr, fw );
    if (iaddr!=nullptr)
    {
        virtual_addresses.insert(addr->getId());
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

    FWOptions* options = fw->getOptionsObject();
    string host_os = fw->getStr("host_OS");
    string version = fw->getStr("version");

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
        update_addresses.setVariable("freebsd", host_os == "freebsd");
        update_addresses.setVariable("openbsd", host_os == "openbsd");
        ostr << update_addresses.expand().toStdString();
    }

    if ( options->getBool("configure_vlan_interfaces") ) 
    {
        Configlet update_vlans(fw, "bsd", "update_vlans");
        update_vlans.removeComments();
        update_vlans.setVariable("freebsd", host_os == "freebsd");
        update_vlans.setVariable("openbsd", host_os == "openbsd");
        ostr << update_vlans.expand().toStdString();
    }

    if (options->getBool("configure_bridge_interfaces"))
    {
        Configlet update_bridge(fw, "bsd", "update_bridge");
        update_bridge.removeComments();
        update_bridge.setVariable("freebsd", host_os == "freebsd");
        if (host_os == "openbsd")
        {
            update_bridge.setVariable("openbsd", true);
            update_bridge.setVariable("openbsd_lt_47",
                                      XMLTools::version_compare(version, "4.7")<0);
            update_bridge.setVariable("openbsd_ge_47",
                                      XMLTools::version_compare(version, "4.7")>=0);
        }

        ostr << update_bridge.expand().toStdString();
    }

    if ( options->getBool("configure_carp_interfaces") ) 
    {
        Configlet update_carp(fw, "bsd", "update_carp");
        update_carp.removeComments();
        update_carp.setVariable("freebsd", host_os == "freebsd");
        update_carp.setVariable("openbsd", host_os == "openbsd");
        ostr << update_carp.expand().toStdString();
    }

    if ( options->getBool("configure_pfsync_interfaces") ) 
    {
        Configlet update_pfsync(fw, "bsd", "update_pfsync");
        update_pfsync.removeComments();
        update_pfsync.setVariable("freebsd", host_os == "freebsd");
        update_pfsync.setVariable("openbsd", host_os == "openbsd");
        ostr << update_pfsync.expand().toStdString();
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

