/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

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

#include "OSConfigurator_solaris.h"
#include "Configlet.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"

#include <algorithm>
#include <functional>
#include <iostream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string OSConfigurator_solaris::myPlatformName() { return "Solaris"; }

string OSConfigurator_solaris::printKernelVarsCommands() 
{
    Configlet kernel_vars(fw, "bsd", "kernel_vars");
    kernel_vars.removeComments();
    setKernelVariable(fw, "solaris_ip_forward", &kernel_vars);
    setKernelVariable(fw, "solaris_ip_ignore_redirect", &kernel_vars);
    setKernelVariable(fw, "solaris_ip_respond_to_echo_broadcast", &kernel_vars);
    setKernelVariable(fw, "solaris_ip_forward_directed_broadcasts", &kernel_vars);
    setKernelVariable(fw, "solaris_ip_forward_src_routed", &kernel_vars);
    return kernel_vars.expand().toStdString();
}

void OSConfigurator_solaris::addVirtualAddressForNAT(const Network*)
{
}

void OSConfigurator_solaris::addVirtualAddressForNAT(const Address *addr)
{
    if (virtual_addresses.empty() || 
	find(virtual_addresses.begin(),virtual_addresses.end(),
             *(addr->getAddressPtr())) == virtual_addresses.end()) 
    {
        FWObject *iaddr = findAddressFor(addr, fw );
        if (iaddr!=nullptr)
        {
            Address *iaddr_addr = Address::cast(iaddr);
            assert(iaddr_addr!=nullptr);
            Interface *iface=Interface::cast(iaddr->getParent());
            assert(iface!=nullptr);

            output << "add_addr " << addr->getAddressPtr()->toString() << " "
                   << iaddr_addr->getNetmaskPtr()->toString() <<  " "
                   << iface->getName() << endl;
        
            virtual_addresses.push_back(*(addr->getAddressPtr()));
        } else
            warning("Can not add virtual address " +
                    addr->getAddressPtr()->toString() );
    }
}

int OSConfigurator_solaris::prolog()
{
    //printPathForAllTools("solaris");

    //processFirewallOptions();

    //configureInterfaces();

    return 0;
}

string  OSConfigurator_solaris::configureInterfaces()
{
    ostringstream ostr;
    FWOptions* options=fw->getOptionsObject();
    if ( options->getBool("configure_interfaces") ) 
    {

        ostr << endl;

        FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface=dynamic_cast<Interface*>(*i);
            assert(iface);

            if (!iface->isRegular()) continue;

            FWObjectTypedChildIterator j=iface->findByType(IPv4::TYPENAME);
            for ( ; j!=j.end(); ++j ) 
            {
                Address *iaddr = Address::cast(*j);
                ostr << "add_addr "
                       << iaddr->getAddressPtr()->toString() << " "
                       << iaddr->getNetmaskPtr()->toString() << " "
                       << iface->getName() << endl;
        
                virtual_addresses.push_back(*(iaddr->getAddressPtr()));
            }
        }
        ostr << endl;
    }
    return ostr.str();
}

