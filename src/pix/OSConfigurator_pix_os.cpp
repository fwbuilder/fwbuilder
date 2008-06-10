/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: OSConfigurator_pix_os.cpp,v 1.1 2008/03/06 06:49:00 vkurland Exp $

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


#include "OSConfigurator_pix_os.h"
#include "Helper.h"
#include "fwbuilder/Resources.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"

#include <list>
#include <algorithm>
#include <functional>
#include <assert.h>
#include <iostream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string OSConfigurator_pix_os::myPlatformName() { return "pix_os"; }

int OSConfigurator_pix_os::prolog()
{
    string host_os = fw->getStr("host_OS");

    if (host_os!="pix_os" && host_os!="fwsm_os")   
	abort("Unsupported OS " + host_os );

    return Compiler::prolog();
}


void OSConfigurator_pix_os::processFirewallOptions() 
{
//    FWOptions* options=fw->getOptionsObject();
    string s;
//    int    i;
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");


    if ( fw->getOptionsObject()->getBool("pix_set_host_name") )
    {
        output << "hostname " << fw->getName() << endl;
        output << endl;
    }

    output << _printNameif();
    output << endl;
    output << _printIPAddress();
    output << endl;
    output << _printLogging();
    output << endl;
    output << _printTimeouts();
    output << endl;
    output << _printSNMP();
    output << endl;
    output << _printNTP();
    output << endl;
    output << _printSysopt();
    output << endl;
    output << getProtocolInspectionCommands();
    output << endl;
}

string OSConfigurator_pix_os::_printNameif()
{
    ostringstream res;
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");
    string::size_type n;

    list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
    {
	Interface *iface=dynamic_cast<Interface*>(*i);
	assert(iface);

        string nameifCmd = Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/version_")+
            version+"/pix_commands/nameif");
        
        if ((n = nameifCmd.find("%il"))!=string::npos)
            nameifCmd.replace(n,3,iface->getLabel());
        if ((n = nameifCmd.find("%in"))!=string::npos)
            nameifCmd.replace(n,3,iface->getName());
        if ((n = nameifCmd.find("%sl"))!=string::npos)
        {
            ostringstream sls;
            sls << iface->getSecurityLevel();
            nameifCmd.replace(n,3,sls.str());
        }
        res << nameifCmd;
    }

    res << endl;

    return res.str();
}

string OSConfigurator_pix_os::_printIPAddress()
{
    ostringstream res;
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");
    string setAddrCmd;
    string::size_type n;

    if ( fw->getOptionsObject()->getBool("pix_ip_address") )
    {
        list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
        for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
        {
            Interface *iface=dynamic_cast<Interface*>(*i);
            assert(iface);
            if (iface->isDyn())
            {
                setAddrCmd = Resources::platform_res[platform]->getResourceStr(
                    string("/FWBuilderResources/Target/options/version_")+
                    version+"/pix_commands/ip_addr_dyn");
            } else
            {
                setAddrCmd = Resources::platform_res[platform]->getResourceStr(
                    string("/FWBuilderResources/Target/options/version_")+
                    version+"/pix_commands/ip_addr_static");
            }

            if ((n = setAddrCmd.find("%il"))!=string::npos)
                setAddrCmd.replace(n,3,iface->getLabel());
            if ((n = setAddrCmd.find("%in"))!=string::npos)
                setAddrCmd.replace(n,3,iface->getName());
            if ((n = setAddrCmd.find("%a"))!=string::npos)
                setAddrCmd.replace(n,2,iface->getAddressPtr()->toString());
            if ((n = setAddrCmd.find("%n"))!=string::npos)
                setAddrCmd.replace(n,2,iface->getNetmaskPtr()->toString());

            res << setAddrCmd;
        }        
    }

    res << endl;

    return res.str();
}


string OSConfigurator_pix_os::_printLogging()
{
    Helper helper(this);

    ostringstream  str;
    bool  logging_on=false;

    string syslog_host = fw->getOptionsObject()->getStr("pix_syslog_host");
    int syslog_queue_size = fw->getOptionsObject()->getInt(
        "pix_syslog_queue_size");
    string syslog_facility = fw->getOptionsObject()->getStr(
        "pix_syslog_facility");
    string trap_level = fw->getOptionsObject()->getStr(
        "pix_logging_trap_level");
    bool buffered = fw->getOptionsObject()->getBool("pix_logging_buffered");
    string buffered_level = fw->getOptionsObject()->getStr(
        "pix_logging_buffered_level");
    bool console = fw->getOptionsObject()->getBool("pix_logging_console");
    string console_level = fw->getOptionsObject()->getStr(
        "pix_logging_console_level");
    bool timestamp = fw->getOptionsObject()->getBool("pix_logging_timestamp");

    if ( ! syslog_host.empty() )
    {
        InetAddr syslog_addr(syslog_host);
        int iface_id = helper.findInterfaceByNetzone(&syslog_addr);
        if (iface_id == -1)
            abort("Log server " + syslog_host +
                  " does not belong to any known network zone");
        Interface  *syslog_iface = getCachedFwInterface(iface_id);

        str << endl;

        str << "logging host " 
            << syslog_iface->getLabel() 
            << " " << syslog_host;

        if ( fw->getOptionsObject()->getBool("pix_emblem_log_format") )
            str << " format emblem ";

        str << endl;

        str << "logging queue " 
            << syslog_queue_size <<  endl;

        if ( ! syslog_facility.empty() )
            str << "logging facility " << syslog_facility <<  endl;

        if ( ! trap_level.empty() )
            str << "logging trap " << trap_level << endl;

        logging_on=true;
    }

    if ( ! buffered ) str << "no logging buffered" << endl;
    else
    {
        str << "logging buffered " << buffered_level << endl;
        logging_on=true;
    }

    if ( ! console )  str << "no logging console" << endl;
    else
    {
        str << "logging console " << console_level << endl;
        logging_on=true;
    }

    if ( ! timestamp ) str << "no ";
    str << "logging timestamp" << endl;

    if ( ! logging_on) str << "no ";
    str << "logging on" << endl;

    string s=fw->getOptionsObject()->getStr("pix_syslog_device_id_opt");
    string v=fw->getOptionsObject()->getStr("pix_syslog_device_id_val");
    if (s=="hostname")  str << "logging device-id hostname" << endl;
    if (s=="interface") str << "logging device-id ipaddress " << v << endl;
    if (s=="string")    str << "logging device-id string " << v << endl;

    str << endl;

    return str.str();
}

string  OSConfigurator_pix_os::_printSNMPServer(const std::string &srv,
                                                int poll_trap)
{
    Helper helper(this);

    ostringstream  str;
    InetAddr srv_addr(srv);
    int iface_id=helper.findInterfaceByNetzone(&srv_addr);
    if (iface_id == -1)
        abort(string("SNMP server ") + srv + 
              " does not belong to any known network zone");
    Interface  *snmp_iface = getCachedFwInterface(iface_id);
    str << "snmp-server host " << snmp_iface->getLabel() << " " << srv;
    switch (poll_trap)
    {
    case 1:  str << " poll" << endl; break;
    case 2:  str << " trap" << endl; break;
    default: str << endl; break;
    }
    return str.str();
}

string    OSConfigurator_pix_os::_printSNMP()
{
    ostringstream  str;
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");

    bool set_communities = fw->getOptionsObject()->getBool(
        "pix_set_communities_from_object_data");
    bool set_sysinfo = fw->getOptionsObject()->getBool(
        "pix_set_sysinfo_from_object_data" );   
    bool enable_traps = fw->getOptionsObject()->getBool(
        "pix_enable_snmp_traps");

    string clearSNMPcmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/version_")+
        version+"/pix_commands/clear_snmp");

    if ( !fw->getOptionsObject()->getBool("pix_acl_no_clear") ) 
        str << clearSNMPcmd << endl;

    if ( fw->getOptionsObject()->getBool("pix_disable_snmp_agent") ) 
    {
        str << "no snmp-server " << endl;
    } else
    {

        if (set_communities) {
            string read_c = fw->getManagementObject()->
                getSNMPManagement()->getReadCommunity();
            str << endl;
            str << "snmp-server community " << read_c << endl;
        }

        if (set_sysinfo) {
            string location=fw->getOptionsObject()->getStr("snmp_location");
            string contact =fw->getOptionsObject()->getStr("snmp_contact");
            str << endl;
            if (!location.empty())
                str << "snmp-server location " << location << endl;
            if (!contact.empty())
                str << "snmp-server contact  " << contact  << endl;
        }

        if (enable_traps) {
            str << endl;
            str << "snmp-server enable traps" << endl;
        } else {
            str << endl;
            str << "no snmp-server enable traps" << endl;
        }

        string snmp_server_1 = fw->getOptionsObject()->getStr(
            "pix_snmp_server1");
        string snmp_server_2 = fw->getOptionsObject()->getStr(
            "pix_snmp_server2");
        int  snmp_poll_traps_1 = fw->getOptionsObject()->getInt(
            "pix_snmp_poll_traps_1");
        int  snmp_poll_traps_2 = fw->getOptionsObject()->getInt(
            "pix_snmp_poll_traps_2");

        if (!snmp_server_1.empty()) 
            str << _printSNMPServer(snmp_server_1,snmp_poll_traps_1);

        if (!snmp_server_2.empty()) 
            str << _printSNMPServer(snmp_server_2,snmp_poll_traps_2);
    }
    return str.str();
}

string  OSConfigurator_pix_os::_printNTPServer(const std::string &srv,bool pref)
{
    Helper helper(this);

    ostringstream  str;
    InetAddr srv_addr(srv);
    int iface_id=helper.findInterfaceByNetzone(&srv_addr);
    if (iface_id == -1)
        abort("NTP server "+srv+" does not belong to any known network zone");
    Interface  *ntp_iface = getCachedFwInterface(iface_id);
    str << "ntp server " << srv << " source " << ntp_iface->getLabel();
    if (pref) str << " prefer";
    str << endl;

    return str.str();
}

string    OSConfigurator_pix_os::_printNTP()
{
    ostringstream  res;
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");
    string clearNTPcmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/version_")+
        version+"/pix_commands/clear_ntp");


    if ( !fw->getOptionsObject()->getBool("pix_acl_no_clear") ) 
        res << clearNTPcmd << endl;

    string ntp_server_1=fw->getOptionsObject()->getStr("pix_ntp1");
    bool   ntp1_pref=fw->getOptionsObject()->getBool("pix_ntp1_pref");
    string ntp_server_2=fw->getOptionsObject()->getStr("pix_ntp2");
    bool   ntp2_pref=fw->getOptionsObject()->getBool("pix_ntp2_pref");
    string ntp_server_3=fw->getOptionsObject()->getStr("pix_ntp3");
    bool   ntp3_pref=fw->getOptionsObject()->getBool("pix_ntp3_pref");

    if (!ntp_server_1.empty()) 
        res << _printNTPServer(ntp_server_1,ntp1_pref);

    if (!ntp_server_2.empty()) 
        res << _printNTPServer(ntp_server_2,ntp2_pref);

    if (!ntp_server_3.empty()) 
        res << _printNTPServer(ntp_server_3,ntp3_pref);

    return res.str();
}

string    OSConfigurator_pix_os::_printSysopt()
{
    ostringstream  res;
    string platform = fw->getStr("platform");
    string version = fw->getStr("version");

    FWOptions *options=fw->getOptionsObject();
    assert(options!=NULL);



    bool tcpmss=    fw->getOptionsObject()->getBool("pix_tcpmss");
    int  tcpmss_val=fw->getOptionsObject()->getInt("pix_tcpmss_value");

    res << endl;


    if (fw->getOptionsObject()->getBool("pix_resetinbound"))
        res << "service resetinbound" << endl;
    else
        res << "no service resetinbound" << endl;

    if (fw->getOptionsObject()->getBool("pix_resetoutside"))
        res << "service resetoutside" << endl;
    else
        res << "no service resetoutside" << endl;

    if (tcpmss)
        res << "sysopt connection tcpmss " << tcpmss_val << endl;

    if (fw->getStr("platform")=="pix")
    {
        if (fw->getOptionsObject()->getBool("pix_connection_timewait"))
            res << "sysopt connection timewait" << endl;
        else
            res << "no sysopt connection timewait" << endl;
    }

    if (Resources::platform_res[platform]->getResourceBool(
            "/FWBuilderResources/Target/options/version_"+version+
            "/pix_security_fragguard_supported") )
    {
        if ( fw->getOptionsObject()->getBool("pix_fragguard") ) 
            res  << "sysopt security fragguard" << endl;
        else
            res  << "no sysopt security fragguard" << endl;
    }

    if ( fw->getOptionsObject()->getBool("pix_nodnsalias_inbound") )
        res << "sysopt nodnsalias inbound" << endl;
    else
        res << "no sysopt nodnsalias inbound" << endl;

    if ( fw->getOptionsObject()->getBool("pix_nodnsalias_outbound") )
        res << "sysopt nodnsalias outbound" << endl;
    else
        res << "no sysopt nodnsalias outbound" << endl;

    if (Resources::platform_res[platform]->getResourceBool(
            "/FWBuilderResources/Target/options/version_"+version+
            "/pix_route_dnat_supported") )
    {
        if ( fw->getOptionsObject()->getBool("pix_route_dnat") ) 
            res  << "sysopt route dnat" << endl;
        else
            res  << "no sysopt route dnat" << endl;
    }

    if (Resources::platform_res[platform]->getResourceBool(
            "/FWBuilderResources/Target/options/version_"+version+
            "/pix_floodguard_supported") )
    {
        if ( fw->getOptionsObject()->getBool("pix_floodguard") ) 
            res  << "floodguard enable" << endl;
        else
            res  << "floodguard disable" << endl;
    }

    res << endl;

    return res.str();
}

string OSConfigurator_pix_os::_printServiceTimeout(
    const std::string &pix_service)
{
    ostringstream  res;
    string hh,mm,ss;
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");
    bool use_sunrpc = Resources::platform_res[platform]->getResourceBool(
        "/FWBuilderResources/Target/options/version_"+version+
        "/pix_timeout_rpc_is_sunrpc");

    hh=fw->getOptionsObject()->getStr(pix_service+"_hh");
    mm=fw->getOptionsObject()->getStr(pix_service+"_mm");
    ss=fw->getOptionsObject()->getStr(pix_service+"_ss");

    if (hh!="" && mm!="" && ss!="")
    {
        string service_name = pix_service;
        if (pix_service=="rpc" && use_sunrpc)
            service_name = "sunrpc";

        res << "timeout " << service_name
            << " " << hh << ":" << mm << ":" << ss << " ";

        if (pix_service=="uauth")
        {
            bool abs=fw->getOptionsObject()->getBool("uauth_abs");
            bool inact=fw->getOptionsObject()->getBool("uauth_inact");
            if (abs)   res << "absolute";
            if (inact) res << "inactivity";
        }
        res  << endl;
    }
    return res.str();
}

string    OSConfigurator_pix_os::_printTimeouts()
{
    ostringstream  res;

    res << _printServiceTimeout("xlate");
    res << _printServiceTimeout("conn");
    res << _printServiceTimeout("udp");
    res << _printServiceTimeout("rpc");
    res << _printServiceTimeout("h323");
    res << _printServiceTimeout("sip");
    res << _printServiceTimeout("sip_media");
    res << _printServiceTimeout("half-closed");
    res << _printServiceTimeout("uauth");
    res << endl;

    int to;
    to=fw->getOptionsObject()->getInt("pix_telnet_timeout");
    if (to>60)  abort("Telnet timeout should not exceed 60 minutes");
    if (to!=0)  res << "telnet timeout " << to << endl;

    to=fw->getOptionsObject()->getInt("pix_ssh_timeout");
    if (to>60)  abort("SSH timeout should not exceed 60 minutes");
    if (to!=0)  res << "ssh timeout " << to << endl;

    return res.str();
}


void OSConfigurator_pix_os::addVirtualAddressForNAT(const Address *addr)
{
    if (addr==NULL) ;
}

void OSConfigurator_pix_os::addVirtualAddressForNAT(const Network *nw)
{
    if (nw==NULL) ;
}

