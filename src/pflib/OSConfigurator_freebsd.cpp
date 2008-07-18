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

#include "OSConfigurator_freebsd.h"

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

string OSConfigurator_freebsd::myPlatformName() { return "FreeBSD"; }

string OSConfigurator_freebsd::getInterfaceVarName(FWObject *iface)
{
    return string("i_") + iface->getName();
}


void OSConfigurator_freebsd::processFirewallOptions() 
{
    FWOptions* options=fw->getOptionsObject();
    string s;

    s=options->getStr("freebsd_ip_forward");
    if (!s.empty()) {
        if (s=="1" || s=="On" || s=="on") s="1";
        else                              s="0";

	output << "$SYSCTL -w net.inet.ip.forwarding=" << s << endl;
    }

    s=options->getStr("freebsd_ipv6_forward");
    if (!s.empty()) {
        if (s=="1" || s=="On" || s=="on") s="1";
        else                              s="0";
        // by the way, this is different from OpenBSD
	output << "$SYSCTL -w net.inet6.ip6.forwarding=" << s << endl;
    }

    s=options->getStr("freebsd_ip_sourceroute");
    if (!s.empty()) {
	if (s!="0" && s!="1")
	    throw FWException(_("Illegal value for OS parameter freebsd_ip_sourceroute: '")+s+"'");

	output << "$SYSCTL -w net.inet.ip.sourceroute=" << s << endl;
    }

    s=options->getStr("freebsd_ip_redirect");
    if (!s.empty()) {
	if (s!="0" && s!="1")
	    throw FWException(_("Illegal value for OS parameter freebsd_ip_redirect: '")+s+"'");

	output << "$SYSCTL -w net.inet.ip.redirect=" << s << endl;
    }
}

void OSConfigurator_freebsd::addVirtualAddressForNAT(const Network*)
{
}

void OSConfigurator_freebsd::addVirtualAddressForNAT(const Address *addr)
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
            Interface *iface = Interface::cast(iaddr->getParent());
            assert(iface!=NULL);

            output << "add_addr " << addr->getAddressPtr()->toString() << " "
                   << iaddr_addr->getNetmaskPtr()->toString() <<  " "
                   << iface->getName() << endl;
        
            virtual_addresses.push_back(*(addr->getAddressPtr()));
        } else
            warning(_("Can not add virtual address ") +
                    addr->getAddressPtr()->toString() );
    }
}

int OSConfigurator_freebsd::prolog()
{
    printPathForAllTools("freebsd");

    processFirewallOptions();

    configureInterfaces();

    return 0;
}

void  OSConfigurator_freebsd::printPathForAllTools(const string &os)
{
    FWOptions* options=fw->getOptionsObject();
    
    string s, path_ipf, path_ipnat, path_ipfw, path_pfctl, path_sysctl, path_logger;

    s=options->getStr("freebsd_path_ipf");
    if (!s.empty()) path_ipf=s;
    else            path_ipf=os_data.getPathForTool(os,OSData::IPF);

    s=options->getStr("freebsd_path_ipnat");
    if (!s.empty()) path_ipnat=s;
    else            path_ipnat=os_data.getPathForTool(os,OSData::IPNAT);

    s=options->getStr("freebsd_path_ipfw");
    if (!s.empty()) path_ipfw=s;
    else            path_ipfw=os_data.getPathForTool(os,OSData::IPFW);

    s=options->getStr("openbsd_path_pfctl");
    if (!s.empty()) path_pfctl=s;
    else            path_pfctl=os_data.getPathForTool(os,OSData::PFCTL);

    s=options->getStr("freebsd_path_sysctl");
    if (!s.empty()) path_sysctl=s;
    else            path_sysctl=os_data.getPathForTool(os,OSData::SYSCTL);

    s=options->getStr("freebsd_path_logger");
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
    output << "  ( ifconfig $dev | egrep -q \"inet +${addr} \" ) || " << endl;
    output << "    { "       << endl;
    output << "      echo \"$dev: $addr\"" << endl;
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
    }

    output << "IPF=\""    + path_ipf    + "\"\n";
    output << "IPNAT=\""  + path_ipnat  + "\"\n";
    output << "IPFW=\""   + path_ipfw   + "\"\n";
    output << "PFCTL=\""  + path_pfctl  + "\"\n";
    output << "SYSCTL=\"" + path_sysctl + "\"\n";
    output << "LOGGER=\"" + path_logger + "\"\n";
    output << endl;

    if (options->getBool("dynAddr"))
    {
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

void  OSConfigurator_freebsd::configureInterfaces()
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

            FWObjectTypedChildIterator j = iface->findByType(IPv4::TYPENAME);
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



