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

void OSConfigurator_solaris::processFirewallOptions() 
{
    FWOptions* options=fw->getOptionsObject();
    string s;

    s=options->getStr("solaris_ip_forward");
    if (!s.empty()) {
        if (s=="1" || s=="On" || s=="on") s="1";
        else                              s="0";

	output << "ndd -set /dev/ip ip_forwarding " << s << endl;
    }

    s=options->getStr("solaris_ip_ignore_redirect");
    if (!s.empty()) {
	if (s!="0" && s!="1")
	    throw FWException("Illegal value for OS parameter solaris_ip_ignore_redirect: '"+s+"'");

	output << "ndd -set /dev/ip ip_ignore_redirect " << s << endl;
    }

    s=options->getStr("solaris_ip_respond_to_echo_broadcast");
    if (!s.empty()) {
	if (s!="0" && s!="1")
	    throw FWException("Illegal value for OS parameter solaris_ip_respond_to_echo_broadcast: '"+s+"'");

	output << "ndd -set /dev/ip ip_respond_to_echo_broadcast " << s << endl;
    }

    s=options->getStr("solaris_ip_forward_directed_broadcasts");
    if (!s.empty()) {
	if (s!="0" && s!="1")
	    throw FWException("Illegal value for OS parameter solaris_ip_forward_directed_broadcasts: '"+s+"'");

	output << "ndd -set /dev/ip ip_forward_directed_broadcasts " << s << endl;
    }

    s=options->getStr("solaris_ip_forward_src_routed");
    if (!s.empty()) {
	if (s!="0" && s!="1")
	    throw FWException("Illegal value for OS parameter solaris_ip_forward_src_routed: '"+s+"'");

	output << "ndd -set /dev/ip ip_forward_src_routed " << s << endl;
    }

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
        if (iaddr!=NULL)
        {
            Address *iaddr_addr = Address::cast(iaddr);
            assert(iaddr_addr!=NULL);
            Interface *iface=Interface::cast(iaddr->getParent());
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

int OSConfigurator_solaris::prolog()
{
    printPathForAllTools("solaris");

    processFirewallOptions();

    configureInterfaces();

    return 0;
}

void  OSConfigurator_solaris::printPathForAllTools(const string &os)
{
    FWOptions* options=fw->getOptionsObject();
    
    string s, path_ipf, path_ipnat, path_logger;

    s=options->getStr("solaris_path_ipf");
    if (!s.empty()) path_ipf=s;
    else            path_ipf=os_data.getPathForTool(os,OSData::IPF);

    s=options->getStr("solaris_path_ipnat");
    if (!s.empty()) path_ipnat=s;
    else            path_ipnat=os_data.getPathForTool(os,OSData::IPNAT);

    s=options->getStr("solaris_path_logger");
    if (!s.empty()) path_logger=s;
    else            path_logger=os_data.getPathForTool(os,OSData::LOGGER);

    output                                                     << endl;
    output << "log() {"                                        << endl;
    output << "  test -x \"$LOGGER\" && $LOGGER -p info \"$1\"" << endl;
    output << "}"                                              << endl;
    output                                                     << endl;


    output << "add_addr() {" << endl;
    output << "  addr=$1"    << endl;
    output << "  nm=$2"      << endl;
    output << "  dev=$3"     << endl;
    output << "  ( ifconfig $dev | egrep -s \"inet +${addr} \" ) || " << endl;
    output << "    { "       << endl;
    output << "      echo \"$dev: $addr\"" << endl;
    output << "      ifconfig $dev $addr alias" << endl; 
    output << "    } "       << endl;
    output << "}"            << endl;
    output << endl;
    output << endl;

    output << "IPF=\""    + path_ipf    + "\"\n";
    output << "IPNAT=\""  + path_ipnat  + "\"\n";
    output << "LOGGER=\"" + path_logger + "\"\n";
    output << endl;

    output << endl;
}

void  OSConfigurator_solaris::configureInterfaces()
{
    FWOptions* options=fw->getOptionsObject();
    if ( options->getBool("configure_interfaces") ) 
    {

        output << endl;

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
                output << "add_addr "
                       << iaddr->getAddressPtr()->toString() << " "
                       << iaddr->getNetmaskPtr()->toString() << " "
                       << iface->getName() << endl;
        
                virtual_addresses.push_back(*(iaddr->getAddressPtr()));
            }
        }
        output << endl;
    }
}





