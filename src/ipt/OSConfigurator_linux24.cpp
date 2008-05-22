/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: OSConfigurator_linux24.cpp 1369 2007-06-17 03:28:20Z vkurland $

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


#include "config.h"

#include "OSConfigurator_linux24.h"

#include "fwbuilder/InetAddr.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/MultiAddress.h"

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

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string OSConfigurator_linux24::myPlatformName() { return "Linux24"; }

OSConfigurator_linux24::OSConfigurator_linux24(FWObjectDatabase *_db,
                                               const string &fwname,
                                               bool ipv6_policy) : 
    OSConfigurator(_db, fwname, ipv6_policy) , os_data(fw->getStr("host_OS"))
{
}

string OSConfigurator_linux24::getInterfaceVarName(FWObject *iface)
{
    ostringstream  ostr;
    string iname=iface->getName();
    string::size_type p1;
    while ( (p1=iname.find("."))!=string::npos)
        iname=iname.replace(p1,1,"_");
    ostr << "i_" << iname;
    return ostr.str();
}


void OSConfigurator_linux24::processFirewallOptions() 
{
    FWOptions* options=fw->getOptionsObject();
    string s;
    int    i;

/*
 * check if all interfaces configured for the firewall are present
 */
    if (options->getBool("verify_interfaces")) 
    {
        list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
        if ( ! l2.empty() )
        {
            output << endl;
            output << "INTERFACES=\"";
            for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
            {
                Interface *iface=Interface::cast(*i);
                if (iface->getName().find("*")==string::npos)
                    output << iface->getName() << " ";
            }
            output << "\"" << endl;
            output << "for i in $INTERFACES ; do" << endl;
            output << "  $IP link show \"$i\" > /dev/null 2>&1 || {" << endl;
            output << "    log \"Interface $i does not exist\"" << endl;
            output << "    exit 1" << endl;
            output << "  }" << endl;
            output << "done" << endl;
            output << endl;
        }
    }

/*
 *    Turn off packet forwarding for now. We'll turn it on if needed in the end
 *
 * turned this off. This seems to be an overkill as we set default
 * policy to DROP in all chains before we purge the current firewall policy.

    output << "\n\n";
    output << "FWD=`cat /proc/sys/net/ipv4/ip_forward`\n";
    output << "echo \"0\" > /proc/sys/net/ipv4/ip_forward\n\n";
*/

    s=options->getStr("linux24_ip_dynaddr");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/ip_dynaddr\n\n";


    s=options->getStr("linux24_rp_filter");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/conf/all/rp_filter\n\n";

    s=options->getStr("linux24_accept_source_route");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/conf/all/accept_source_route\n\n";

    s=options->getStr("linux24_accept_redirects");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/conf/all/accept_redirects\n\n";

    s=options->getStr("linux24_log_martians");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/conf/all/log_martians\n\n";


    
    s=options->getStr("linux24_icmp_echo_ignore_broadcasts");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts\n\n";

    s=options->getStr("linux24_icmp_echo_ignore_all");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/icmp_echo_ignore_all\n\n";

    s=options->getStr("linux24_icmp_ignore_bogus_error_responses");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/icmp_ignore_bogus_error_responses\n\n";


    
    if ( (i=options->getInt("linux24_tcp_fin_timeout"))>0 )
	output << "echo " << i << " > /proc/sys/net/ipv4/tcp_fin_timeout\n\n";

    if ( (i=options->getInt("linux24_tcp_keepalive_interval"))>0 )
	output << "echo " << i << " > /proc/sys/net/ipv4/tcp_keepalive_intvl\n\n";

    s=options->getStr("linux24_tcp_window_scaling");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/tcp_window_scaling\n\n";

    s=options->getStr("linux24_tcp_sack");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/tcp_sack\n\n";

    s=options->getStr("linux24_tcp_fack");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/tcp_fack\n\n";

    s=options->getStr("linux24_tcp_syncookies");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/tcp_syncookies\n\n";

    s=options->getStr("linux24_tcp_ecn");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/tcp_ecn\n\n";

    s=options->getStr("linux24_tcp_timestamps");
    if (!s.empty())
	output << "echo " << s << " > /proc/sys/net/ipv4/tcp_timestamps\n\n";


    output << endl;
}

void OSConfigurator_linux24::addVirtualAddressForNAT(const Network *nw)
{
    ostringstream  ostr;

    FWOptions* options=fw->getOptionsObject();
    if ( options->getBool("manage_virtual_addr") ) 
    {
        if (virtual_addresses.empty() || 
            find(virtual_addresses.begin(),virtual_addresses.end(),
                 *(nw->getAddressPtr())) == virtual_addresses.end())
        {
            Interface *iface=findInterfaceFor( nw, fw );
            if (iface!=NULL)
            {
                const InetAddr *a = nw->getAddressPtr();
                string str, subnet, first, last;

                first  = (*a + 1).toString();
                last   = (*(nw->getBroadcastAddressPtr()) -1).toString();

                ostr                                       << endl;

                ostr << "a=\"" << first << "\""            << endl;
                ostr << "while test \"$a\" != \"" << last << "\"; do" << endl;

                ostr << "  add_addr ${a} " << nw->getNetmaskPtr()->getLength()
                     << " " << iface->getName() << endl;

                ostr                                       << endl;
                ostr << "  OIFS=$IFS"                      << endl;
                ostr << "  IFS=\".\""                      << endl;
                ostr << "  set $a"                         << endl;
                ostr << "  a4=$1"                          << endl;
                ostr << "  a3=$2"                          << endl;
                ostr << "  a2=$3"                          << endl;
                ostr << "  a1=$4"                          << endl;
                ostr << "  IFS=$OIFS"                      << endl;
                ostr << "  incaddr a4 a3 a2 a1"            << endl;
                ostr << "  a=$a4\".\"$a3\".\"$a2\".\"$a1"  << endl;

                ostr << "done" << endl << endl;
                virtual_addresses.push_back( *(nw->getAddressPtr()) );
                registerVirtualAddressForNat();
            } else
                warning(_("Can not add virtual address ") +
                        nw->getAddressPtr()->toString() +
                        _(" (object ") + nw->getName() + ")" );
        }
        commands_to_add_virtual_addresses.push_back(ostr.str());
    }
}

void OSConfigurator_linux24::addVirtualAddressForNAT(const Address *addr)
{
    ostringstream  ostr;

    FWOptions* options=fw->getOptionsObject();
    if ( options->getBool("manage_virtual_addr") ) 
    {
        if (virtual_addresses.empty() || 
            find(virtual_addresses.begin(),virtual_addresses.end(),
                 *(addr->getAddressPtr())) == virtual_addresses.end()) 
        {
            FWObject *vaddr = findAddressFor(addr, fw );
            if (vaddr!=NULL)
            {
                Interface *iface = Interface::cast(vaddr->getParent());
                assert(iface!=NULL);

                const InetAddr *vaddr_netm =
                    Address::cast(vaddr)->getNetmaskPtr();

//                const InetAddrMask *vaddr_addr = Address::cast(
//                    vaddr)->getAddressObjectInetAddrMask();
//                assert(vaddr_addr!=NULL);

                ostr << "add_addr " << addr->getAddressPtr()->toString()
                     << " "
                     << vaddr_netm->getLength()
                     <<  " "
                     << iface->getName() << endl;
        
                virtual_addresses.push_back(*(addr->getAddressPtr()));
                registerVirtualAddressForNat();
            } else
                warning(_("Can not add virtual address ") + 
                        addr->getAddressPtr()->toString() +
                        _(" (object ") + addr->getName() + ")" );
        }
        commands_to_add_virtual_addresses.push_back(ostr.str());
        return;
    }
}

void OSConfigurator_linux24::printCommandsToAddVirtualAddressesForNAT()
{
    output << "# Add virtual addresses for NAT rules" << endl;

    list<string>::iterator i;
    for (i=commands_to_add_virtual_addresses.begin();
         i!=commands_to_add_virtual_addresses.end(); ++i)
        output << *i;

    output << endl;
}

void OSConfigurator_linux24::registerMultiAddressObject(MultiAddressRunTime *at)
{
    address_table_objects[at->getName()] = at->getSourceName();
}

void OSConfigurator_linux24::printChecksForRunTimeMultiAddress()
{
    output << "# Using " << address_table_objects.size() << " address table files" << endl;

    map<string,string>::iterator i;
    for (i=address_table_objects.begin(); i!=address_table_objects.end(); ++i)
    {
        string at_name = i->first;
        string at_file = i->second;
        output << "check_file \"" + at_name + "\" \"" + at_file + "\"" << endl;
    }
    output << endl;
}

void OSConfigurator_linux24::configureInterfaces()
{
    FWOptions* options=fw->getOptionsObject();

    output << "# Configure interfaces" << endl;

/*
 *  Remove all host static routes and "pub" ARP entries if we are going to 
 *  create new ones
 */
    if ( options->getBool("manage_virtual_addr") ) 
    {
	list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
	for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
        {
	    Interface *interface_=Interface::cast(*i);
	    if ( interface_->isDyn() ) continue;
	    if ( interface_->isUnnumbered() ) continue;
	    if ( interface_->isBridgePort() ) continue;
	    if ( interface_->isLoopback() ) continue;

            output << "$IP -4 neigh flush dev " 
                   << interface_->getName() << " >/dev/null 2>&1" << endl;

            output << "$IP -4 addr flush dev " 
                   << interface_->getName() 
                   << " secondary label \"" << interface_->getName() << ":FWB*\""
                   <<  " >/dev/null 2>&1" << endl;
	}
	output << endl;
    }

    if ( options->getBool("configure_interfaces") ) 
    {

        output << endl;

        FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
        for ( ; i!=i.end(); ++i ) 
        {
            Interface *iface=dynamic_cast<Interface*>(*i);
            assert(iface);

            if (iface->isDyn()) continue;
            if (iface->isUnnumbered()) continue;
	    if (iface->isBridgePort() ) continue;

            FWObjectTypedChildIterator j=iface->findByType(IPv4::TYPENAME);
            for ( ; j!=j.end(); ++j ) 
            {
                //const InetAddrMask *iaddr = Address::cast(*j)->getAddressObjectInetAddrMask();
                const InetAddr *iaddr_addr =
                    Address::cast(*j)->getAddressPtr();
                const InetAddr *iaddr_netm =
                    Address::cast(*j)->getNetmaskPtr();

                output << "add_addr " << iaddr_addr->toString()
                       << " "
                       << iaddr_netm->getLength()
                       << " "
                       << iface->getName() << endl;
// add to the table of virtual addresses so we won't generate code to
// configure the same address if it is needed for NAT
                virtual_addresses.push_back(*iaddr_addr);
            }
            output << "$IP link set " << iface->getName() << " up" << endl;
        }
        output << endl;
    }

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
 * not get its address at this time. */
            if (iface->getName().find("*")==string::npos)
                output << "getaddr "
                       << iface->getName()
                       << "  "
                       << getInterfaceVarName(iface)
                       << endl;
        }
    }
    output << endl;
}


int  OSConfigurator_linux24::prolog()
{
    printShellFunctions();

/*
 * Process firewall options, build OS network configuration script
 */
//    processFirewallOptions();

    output << endl;

//    configureInterfaces();

    return 0;
}

void  OSConfigurator_linux24::printShellFunctions()
{
    FWOptions* options=fw->getOptionsObject();
    
    output                                                      << endl;
    output << "log() {"                                         << endl;
    output << "  echo \"$1\""                                   << endl;
    output << "  test -x \"$LOGGER\" && $LOGGER -p info \"$1\"" << endl;
    output << "}"                                               << endl;
    output                                                      << endl;
    
    output << "check_file() {"                             << endl;
    output << "  test -r \"$2\" || {"                      << endl;
    output << "    echo \"Can not find file $2 referenced by AddressTable object $1\"" << endl;
    output << "    exit 1"                                 << endl;
    output << "  }"                                        << endl;
    output << "}"                                          << endl;
    output                                                 << endl;

    output << "va_num=1"                                   << endl;

    output << "add_addr() {"                               << endl;
    output << "  addr=$1"                                  << endl;
    output << "  nm=$2"                                    << endl;
    output << "  dev=$3"                                   << endl;
    output << ""                                           << endl;
    output << "  type=\"\""                                << endl;
    output << "  aadd=\"\""                                << endl;
    output << ""                                           << endl;
    output << "  L=`$IP -4 link ls $dev | head -n1`"       << endl;
    output << "  if test -n \"$L\"; then"                  << endl;
    output << "    OIFS=$IFS"                              << endl;
    output << "    IFS=\" /:,<\""                          << endl;
    output << "    set $L"                                 << endl;
    output << "    type=$4"                                << endl;
    output << "    IFS=$OIFS"                              << endl;
    output << "    if test \"$type\" = \"NO-CARRIER\"; then" << endl;
    output << "      type=$5"                              << endl;
    output << "    fi"                                     << endl;
    output << ""                                           << endl;

/*
 *  see comment about using grep with or without regex below
 */
    output << "    L=`$IP -4 addr ls $dev to $addr | grep inet | grep -v :`" << endl;
    output << "    if test -n \"$L\"; then"                << endl;
    output << "      OIFS=$IFS"                            << endl;
    output << "      IFS=\" /\""                           << endl;
    output << "      set $L"                               << endl;
    output << "      aadd=$2"                              << endl;
    output << "      IFS=$OIFS"                            << endl;
    output << "    fi"                                     << endl;
    output << "  fi"                                       << endl;
    output << "  if test -z \"$aadd\"; then"               << endl;
    output << "    if test \"$type\" = \"POINTOPOINT\"; then"<< endl;
    output << "      $IP -4 addr add $addr dev $dev scope global label $dev:FWB${va_num}"            << endl;
    output << "      va_num=`expr $va_num + 1`"             << endl;
    output << "    fi"                                      << endl;
    output << "    if test \"$type\" = \"BROADCAST\"; then" << endl;
    output << "      $IP -4 addr add $addr/$nm dev $dev brd + scope global label $dev:FWB${va_num}"  << endl;
    output << "      va_num=`expr $va_num + 1`"             << endl;
    output << "    fi"                                      << endl;
    output << "  fi"                                        << endl;
    output << "}"                                           << endl;
    output << endl;
    output << "getInterfaceVarName() {"                     << endl;
    output << "  echo $1 | sed 's/\\./_/'"                  << endl;
    output << "}"                                           << endl;
    output << endl;
    output << "getaddr() {"                                 << endl;
    output << "  dev=$1"                                    << endl;
    output << "  name=$2"                                   << endl;
/*
 * originally this command looked like this:
 * $IP -4 addr ls $dev to $addr | grep inet | grep -E \"$dev$\"`
 *
 *  i.e. it looked for a line that ends with "$dev":
 *  inet 10.3.14.40/24 brd 10.3.14.255 scope global eth0  
 *    as opposed to
 *  inet 192.168.1.100/24 brd 192.168.1.255 scope global eth0:1
 *
 *  It turns out, some busybox-based systems have grep compiled w/o
 *  support for regular expressions. Using "grep -v :" seems to be an
 *  easy way to filter out secondary addresses without using regex
 */
    output << "  L=`$IP -4 addr show dev $dev | grep inet | grep -v :`"   << endl;
    output << "  test -z \"$L\" && { "                      << endl;
    output << "    eval \"$name=''\""                       << endl;
    output << "    return"                                  << endl;
    output << "  }"                                         << endl;
    output << "  OIFS=$IFS"                                 << endl;
    output << "  IFS=\" /\""                                << endl;
    output << "  set $L"                                    << endl;
    output << "  eval \"$name=$2\""                         << endl;
    output << "  IFS=$OIFS"                                 << endl;
    output << "}"                                           << endl;
    output << endl;
    output << endl;

/* we use function getinterfaces to process wildcard interfaces */

    output << "getinterfaces() {"                           << endl;
    output << "  NAME=$1"                                   << endl;
    output << "  $IP link show | grep \": $NAME\" | while read L; do" << endl;
    output << "    OIFS=$IFS"                               << endl;
    output << "    IFS=\" :\""                              << endl;
    output << "    set $L"                                  << endl;
    output << "    IFS=$OIFS"                               << endl;
    output << "    echo $2"                                 << endl;
    output << "  done"                                      << endl;
    output << "}"                                           << endl;
    output << endl;
    output << endl;

    output << "# increment ip address"                      << endl;
    output << "incaddr()"                                   << endl;
    output << "{"                                           << endl;
    output << "  n1=$4"                                     << endl;
    output << "  n2=$3"                                     << endl;
    output << "  n3=$2"                                     << endl;
    output << "  n4=$1"                                     << endl;
    output                                                  << endl;
    output << "  vn1=`eval  \"echo \\\\$$n1\"`"             << endl;
    output                                                  << endl;
    output << "  R=`expr $vn1 \\< 255`"                     << endl;
    output << "  if test $R = \"1\"; then"                  << endl;
    output << "    eval \"$n1=`expr $vn1 + 1`\""            << endl;
    output << "  else"                                      << endl;
    output << "    eval \"$n1=0\""                          << endl;
    output << "    incaddr XX $n4 $n3 $n2"                  << endl;
    output << "  fi"                                        << endl;
    output << "}"                                           << endl;
    output                                                  << endl;

/* check if package iproute2 is installed, but do this only if
 * we really need /usr/sbin/ip 
 */
    if (options->getBool("verify_interfaces") || 
        options->getBool("manage_virtual_addr") ||
        options->getBool("configure_interfaces") )
    {
        output << "if $IP link ls >/dev/null 2>&1; then" << endl;
        output << "  echo;"                              << endl;
        output << "else"                                 << endl;
        output << "  echo \"iproute not found\""         << endl;
        output << "  exit 1"                             << endl;
        output << "fi"                                   << endl;
    }

    output << endl;
}

string  OSConfigurator_linux24::printPathForAllTools(const string &os)
{
    string res;

    FWOptions* options=fw->getOptionsObject();
    
    string s, path_lsmod, path_modprobe, path_iptables, path_ip6tables;
    string path_iptables_restore, path_ip6tables_restore, path_ip, path_logger;

    s=options->getStr("linux24_path_lsmod");
    if (!s.empty()) path_lsmod=s;
    else            path_lsmod=os_data.getPathForTool(os,OSData::LSMOD);

    s=options->getStr("linux24_path_modprobe");
    if (!s.empty()) path_modprobe=s;
    else            path_modprobe=os_data.getPathForTool(os,OSData::MODPROBE);

    s=options->getStr("linux24_path_iptables");
    if (!s.empty()) path_iptables=s;
    else            path_iptables=os_data.getPathForTool(os,OSData::IPTABLES);

    s=options->getStr("linux24_path_ip6tables");
    if (!s.empty()) path_ip6tables=s;
    else            path_ip6tables=os_data.getPathForTool(os,OSData::IP6TABLES);

    s=options->getStr("linux24_path_iptables_restore");
    if (!s.empty()) path_iptables_restore=s;
    else            path_iptables_restore=os_data.getPathForTool(os,OSData::IPTABLES_RESTORE);

    s=options->getStr("linux24_path_ip6tables_restore");
    if (!s.empty()) path_ip6tables_restore=s;
    else            path_ip6tables_restore=os_data.getPathForTool(os,OSData::IP6TABLES_RESTORE);

    s=options->getStr("linux24_path_ip");
    if (!s.empty()) path_ip=s;
    else            path_ip=os_data.getPathForTool(os,OSData::IP);

    s=options->getStr("linux24_path_logger");
    if (!s.empty()) path_logger=s;
    else            path_logger=os_data.getPathForTool(os,OSData::LOGGER);


    res += "LSMOD=\""   +path_lsmod+"\"\n";
    res += "MODPROBE=\""+path_modprobe+"\"\n";
    res += "IPTABLES=\""+path_iptables+"\"\n";
    res += "IP6TABLES=\""+path_ip6tables+"\"\n";
    res += "IPTABLES_RESTORE=\""+path_iptables_restore+"\"\n";
    res += "IP=\""      +path_ip+"\"\n";
    res += "LOGGER=\""  +path_logger+"\"\n";
    res += "\n";

    return res;
}

void OSConfigurator_linux24::generateCodeForProtocolHandlers(bool have_nat)
{
    FWOptions* options=fw->getOptionsObject();

    bool nomod=Resources::os_res[fw->getStr("host_OS")]->Resources::getResourceBool("/FWBuilderResources/Target/options/suppress_modules");

/* there is no need to load modules on linksys */
    if (options->getBool("load_modules") && !nomod)
    {
        std::string sed_command = "sed  -e 's/^.*\\///' -e 's/\\([^\\.]\\)\\..*/\\1/'";
	output << endl;
        output << "MODULES_DIR=\"/lib/modules/`uname -r`/kernel/net/\"" << endl;
        output << "MODULES=`find $MODULES_DIR -name '*conntrack*'|" << sed_command <<  "`" << endl;
        if (have_nat)
        {
            output << "MODULES=\"$MODULES `find $MODULES_DIR -name '*nat*'|" << sed_command <<  "`\"" << endl;

        }

	output << "for module in $MODULES; do " << endl;
        output << "  if $LSMOD | grep ${module} >/dev/null; then continue; fi" << endl;
	output << "  $MODPROBE ${module} ||  exit 1 " << endl;
	output << "done" << endl;

	output << endl;
	output << endl;
    }
}

string  OSConfigurator_linux24::printRunTimeWrappers(FWObject *rule,
                                                     const string &command)
{
    string command_line = command;
    ostringstream  ext_command_line;

    int nlines = 0;
    string::size_type p1 = 0;
    string::size_type p2, p3;

    p1=command_line.find("$at_");
    if ( p1!=string::npos )
    {
        p2=command_line.find(" ",p1);
        string at_var= command_line.substr(p1+1,p2-p1-1);  // skip '$'
        string atfile = rule->getStr("address_table_file");
        ext_command_line << "grep -Ev '^#|^;|^\\s*$' " << atfile << " | ";
        ext_command_line << "while read L ; do" << endl;
        ext_command_line << "  set $L; " << at_var << "=$1; ";
        ext_command_line << command_line;
        ext_command_line << "done" << endl;

        command_line = ext_command_line.str();
    }

    p1 = 0;
    while (1)
    {
        p1=command_line.find_first_of("\n\r",p1);
        if (p1==string::npos) break;
        nlines++;
        p1=command_line.find_first_not_of("\n\r",p1);
        if (p1==string::npos) break;
    }

/* if anywhere in command_line we used variable holding an address of
 * dynamic interface (named $i_something) then we need to add this
 * command with a check for the value of this variable. We execute
 * iptables command only if the value is a non-empty string.
 *
 * bug #1851166: there could be two dynamic interfaces in the same
 * rule.
 */
    if (command_line.find("$i_")==string::npos) return command_line;

    ostringstream  res;
    bool wildcard_interfaces = false;
    p1=0;
    while ((p1=command_line.find("$i_", p1))!=string::npos)
    {
        string  iface_name;
        string  iface_var;

        p2=command_line.find(" ",p1);
        p3=command_line.find("_",p1) +1;
        iface_name=command_line.substr(p3,p2-p3);
        iface_var= command_line.substr(p1,p2-p1);

/* if interface name ends with '*', this is a wildcard interface. */
        string::size_type p4;
        if ((p4=iface_name.find("*"))!=string::npos)
        {
            wildcard_interfaces = true;
            string cmdline=command_line;
            string iface_family_name=iface_name.substr(0,p4);
            res << "getinterfaces " << iface_family_name << " | while read I; do" << endl;
            res << "  ivar=`getInterfaceVarName $I`" << endl;
            res << "  getaddr $I $ivar" << endl;
            res << "  cmd=\"$\"$ivar"   << endl;
            res << "  eval \"addr=$cmd\""          << endl;
            cmdline.replace(p1,p2-p1,"$addr");
            res << "  test -n \"$addr\" && ";
            if (nlines>1) res << "{" << endl;
            res << cmdline;
            if (nlines>1) res << "}" << endl;
            res << "done" << endl;
        } else
        {
            // bug #1851166: there could be two dynamic interfaces in
            // the same rule. Just print "test" command here and continue
            // in the "while" loop. We'll print actual commands when the loop
            // ends.
            res << "test -n \"" << iface_var << "\" && ";
        }
        p1++;  // p1 points at the previous "$i_" fragment
    }


    // for wildcard interfaces we only support one such interface
    // per rule and we have already printed the actual command above.
    if (!wildcard_interfaces)
    {
        if (nlines>1) res << "{" << endl;
        res << command_line;
        if (nlines>1) res << "}" << endl;
    }

    return res.str();
}

void OSConfigurator_linux24::epilog()
{
    FWOptions* options=fw->getOptionsObject();

    try {
	output << "#" << endl;
	output << "#" << endl;

/* Turn on packet forwarding if we have to */

        string s=options->getStr("linux24_ip_forward");
        if (!s.empty())
        {
            if (s=="1" || s=="On" || s=="on") s="1";
            else                              s="0";
            output << "echo " << s << " > /proc/sys/net/ipv4/ip_forward\n\n";
        }
//        else
//            output << "echo \"$FWD\" > /proc/sys/net/ipv4/ip_forward\n\n";

    } catch (FWException ex) {
	error(ex.toString());
	exit(1);
    }
}
