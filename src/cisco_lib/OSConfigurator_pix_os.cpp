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


#include "OSConfigurator_pix_os.h"
#include "Helper.h"
#include "Configlet.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"


#include <QtDebug>

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

    output << _printInterfaceConfiguration();
    output << endl;

    if (fw->getOptionsObject()->getBool("cluster_member"))
    {
        output << _printFailoverConfiguration();
        output << endl;
        output << endl;
        output << endl;
    }

    output << _printLogging();
    output << endl;

    output << _printTimeouts();
    output << endl;

    output << _printSSHConfiguration();
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

void OSConfigurator_pix_os::_getFailoverAddresses(ClusterGroup *cluster_group,
                                                  QString *primary_addr,
                                                  QString *primary_netm,
                                                  QString *standby_addr)
{
    int master_id = FWObjectDatabase::getIntId(cluster_group->getStr("master_iface"));
    if (master_id <= 0)
    {
        QString err;
        if (StateSyncClusterGroup::isA(cluster_group))
            err = QObject::tr("One of the state synchronization group members "
                              "must be marked as master. Can not configure "
                              "PIX failover without master.");
        else
            err = QObject::tr("One of the failover group members must be marked "
                              "as master. Can not configure PIX failover "
                              "without master.");
        abort(err.toStdString());
    }

    for (FWObjectTypedChildIterator it = cluster_group->findByType(FWObjectReference::TYPENAME);
         it != it.end(); ++it)
    {
        Interface *gorup_intf = Interface::cast(FWObjectReference::getObject(*it));
        assert(gorup_intf);
        QString addr;
        QString netm;
        const InetAddr *a = gorup_intf->getAddressPtr();
        const InetAddr *n = gorup_intf->getNetmaskPtr();
        if (a && n)
        {
            addr = a->toString().c_str();
            netm = n->toString().c_str();
            if (gorup_intf->getId() == master_id)
            {
                *primary_addr = addr;
                *primary_netm = netm;
            } else
            {
                *standby_addr = addr;
            }
        }
    }
}

void OSConfigurator_pix_os::_getAddressConfigurationForInterface(
    Interface *iface, QString *addr, QString *netm, QString *standby_addr)
{
    const InetAddr *a = iface->getAddressPtr();
    const InetAddr *n = iface->getNetmaskPtr();
    if (a && n)
    {
        *addr = a->toString().c_str();
        *netm = n->toString().c_str();
    }

    if (standby_addr)
    {
        // find standby address (address of the other unit in failover group)
        int failover_group_id = FWObjectDatabase::getIntId(
            iface->getOptionsObject()->getStr("failover_group_id"));
        FWObject *failover_group = fw->getRoot()->findInIndex(failover_group_id);
        if (failover_group)
        {
            for (FWObjectTypedChildIterator it =
                     failover_group->findByType(FWObjectReference::TYPENAME);
                 it != it.end(); ++it)
            {
                Interface *failover_intf = Interface::cast(FWObjectReference::getObject(*it));
                assert(failover_intf);
                if (failover_intf->getId() != iface->getId())
                {
                    const InetAddr *a = failover_intf->getAddressPtr();
                    if (a) *standby_addr = a->toString().c_str();
                    break;
                }
            }
        }
    }
}

string OSConfigurator_pix_os::_printInterfaceConfiguration()
{
    ostringstream res;
    string host_os = fw->getStr("host_OS");
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");

    bool configure_address = fw->getOptionsObject()->getBool("pix_ip_address");
    bool configure_standby_address =
        configure_address && fw->getOptionsObject()->getBool("cluster_member");

    list<FWObject*> l2=fw->getByTypeDeep(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
    {
	Interface *iface=dynamic_cast<Interface*>(*i);
	assert(iface);

        if (iface->getOptionsObject()->getBool("cluster_interface")) continue;

        Configlet *cnf = nullptr;
        QString configlet_name;
        if (iface->isDedicatedFailover()) 
        {
            configlet_name = "failover_interface_";
            if (iface->getLabel().empty()) iface->setLabel("failover");
        }

        if (iface->getOptionsObject()->getStr("type") == "8021q")
            configlet_name = "vlan_subinterface_";

        if ((iface->getOptionsObject()->getStr("type") == "" ||
             iface->getOptionsObject()->getStr("type") == "ethernet") &&
            iface->getByType(Interface::TYPENAME).size() > 0)
        {
            // vlan parent
            configlet_name = "vlan_parent_interface_";
        }

        if (configlet_name.isEmpty())  configlet_name = "regular_interface_";

        if (host_os == "pix_os")
        {
            if (XMLTools::version_compare(version, "7.0") < 0)
                configlet_name += "6";
            if (XMLTools::version_compare(version, "7.0") >= 0)
                configlet_name += "7";
        }

        if (host_os == "fwsm_os")
        {
            if (XMLTools::version_compare(version, "3.2") < 0)
                configlet_name += "2";
            if (XMLTools::version_compare(version, "3.2") >= 0)
                configlet_name += "3_2";
        }

        cnf = new Configlet(fw, "pix_os", configlet_name);

        cnf->removeComments();
        cnf->collapseEmptyStrings(true);

        cnf->setVariable("configure_interface_address", configure_address);
        cnf->setVariable("configure_standby_address", configure_standby_address);

        cnf->setVariable("interface_name",  iface->getName().c_str());
        cnf->setVariable("interface_label", iface->getLabel().c_str());
        cnf->setVariable("security_level",  iface->getSecurityLevel());

        if (iface->getOptionsObject()->getStr("type") == "8021q")
        {
            cnf->setVariable("vlan_id",
                             iface->getOptionsObject()->getInt("vlan_id"));
            cnf->setVariable("parent_interface",
                             iface->getParent()->getName().c_str());
        }

        cnf->setVariable("static_address", ! iface->isDyn());
        cnf->setVariable("dhcp_address",     iface->isDyn());

        if (!iface->isDyn())
        {
            QString addr;
            QString netm;
            QString standby_addr;

            _getAddressConfigurationForInterface(
                iface, &addr, &netm,
                (configure_standby_address) ? &standby_addr : nullptr);

            if (!addr.isEmpty() && !netm.isEmpty())
            {
                cnf->setVariable("address", addr);
                cnf->setVariable("netmask", netm);
            } else
            {
                cnf->setVariable("configure_interface_address", false);
                cnf->setVariable("configure_standby_address", false);
            }

            if (configure_standby_address && !standby_addr.isEmpty())
                cnf->setVariable("standby_address", standby_addr);

        }

        res << cnf->expand().toStdString();
        res << endl;
        res << endl;
        
        delete cnf;
    }
    return res.str();
}

string OSConfigurator_pix_os::_printFailoverConfiguration()
{
    ostringstream res;
    string host_os = fw->getStr("host_OS");
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");

    QString configlet_name = "failover_commands_";

    if (host_os == "pix_os")
    {
        if (XMLTools::version_compare(version, "7.0") < 0)  configlet_name += "6";
        if (XMLTools::version_compare(version, "7.0") >= 0) configlet_name += "7";
    }

    if (host_os == "fwsm_os")
    {
        if (XMLTools::version_compare(version, "3.2") < 0)  configlet_name += "2";
        if (XMLTools::version_compare(version, "3.2") >= 0) configlet_name += "3_2";
    }

    Configlet cnf(fw, "pix_os", configlet_name);
    cnf.removeComments();
    cnf.collapseEmptyStrings(true);

    list<FWObject*> l2 = fw->getByTypeDeep(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
    {
	Interface *iface=dynamic_cast<Interface*>(*i);
	assert(iface);

        if (iface->getOptionsObject()->getBool("cluster_interface")) continue;

        // Intrfaces used for failover and state sync must be marked
        // as "dedicated failover" in PIX firewall objects. This can
        // be the same or two different interfaces.
        if (iface->isDedicatedFailover()) 
        {
            // configure state sync.  StateSyncClusterGroup object
            // belongs to the cluster but method
            // CompilerDriver::processStateSyncGroups sets variables
            // "state_sync_group_id" and "state_sync_interface" in the
            // member firewall object

            int state_sync_group_id = FWObjectDatabase::getIntId(
                fw->getOptionsObject()->getStr("state_sync_group_id"));
            StateSyncClusterGroup *state_sync_group = StateSyncClusterGroup::cast(
                fw->getRoot()->findInIndex(state_sync_group_id));

            if (state_sync_group && state_sync_group->hasMember(iface))
            {
                cnf.setVariable("state_sync_interface_name",  iface->getName().c_str());
                cnf.setVariable("state_sync_interface_label", iface->getLabel().c_str());

                QString key = state_sync_group->getOptionsObject()->getStr("pix_failover_key").c_str();
                cnf.setVariable("failover_key", key);

                QString primary_addr;
                QString primary_netm;
                QString standby_addr;

                /*
                 * Note that in the "failover interface ip" command
                 * the first address is always that of the primary
                 * unit and the second address is that of the
                 * standby. They come in this order in the
                 * configuration of BOTH units which is rather
                 * counter-intuitive.
                 */
                _getFailoverAddresses(
                    state_sync_group,
                    &primary_addr, &primary_netm, &standby_addr);

                cnf.setVariable("state_sync_interface_primary_address", primary_addr);
                cnf.setVariable("state_sync_interface_primary_netmask", primary_netm);
                cnf.setVariable("state_sync_interface_standby_address", standby_addr);
            }

            int failover_group_id = FWObjectDatabase::getIntId(
                iface->getOptionsObject()->getStr("failover_group_id"));
            FailoverClusterGroup *failover_group = FailoverClusterGroup::cast(
                fw->getRoot()->findInIndex(failover_group_id));
            if (failover_group)
            {
                cnf.setVariable("failover_interface_name",  iface->getName().c_str());
                cnf.setVariable("failover_interface_label", iface->getLabel().c_str());

                QString primary_or_secondary = "secondary";
                int master_id = FWObjectDatabase::getIntId(
                    failover_group->getStr("master_iface"));
                if (iface->getId() == master_id)
                    primary_or_secondary = "primary";

                cnf.setVariable("primary_or_secondary", primary_or_secondary);

                QString primary_addr;
                QString primary_netm;
                QString standby_addr;

                _getFailoverAddresses(
                    failover_group,
                    &primary_addr, &primary_netm, &standby_addr);

                cnf.setVariable("failover_interface_primary_address", primary_addr);
                cnf.setVariable("failover_interface_primary_netmask", primary_netm);
                cnf.setVariable("failover_interface_standby_address", standby_addr);
            }
        }
    }



    return cnf.expand().toStdString();
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
        Interface *syslog_iface = Interface::cast(dbcopy->findInIndex(iface_id));

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

void OSConfigurator_pix_os::_configureSNMPServer(Configlet *cnf,
                                                 int server_num,
                                                 const string &srv,
                                                 int poll_trap)
{
    QString interface_var = QString("interface_%1_label").arg(server_num);
    QString address_var = QString("address_%1").arg(server_num);
    QString poll_or_trap_var = QString("poll_or_trap_%1").arg(server_num);

    Helper helper(this);
    InetAddr srv_addr(srv);
    int iface_id = helper.findInterfaceByNetzone(&srv_addr);
    if (iface_id == -1)
        abort(string("SNMP server ") + srv + 
              " does not belong to any known network zone");
    Interface *snmp_iface = Interface::cast(dbcopy->findInIndex(iface_id));
    cnf->setVariable(interface_var, snmp_iface->getLabel().c_str());
    cnf->setVariable(address_var, srv.c_str());

    switch (poll_trap)
    {
    case 1:
        cnf->setVariable(poll_or_trap_var, "poll");
        break;
    case 2:
        cnf->setVariable(poll_or_trap_var, "trap");
        break;
    default:
        cnf->setVariable(poll_or_trap_var, "");
        break;
    }
}

string OSConfigurator_pix_os::_printSNMP()
{
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");

    // for pix
    bool version_ge_70 = XMLTools::version_compare(version, "7.0") >= 0;
    // for fwsm
    bool version_ge_32 = XMLTools::version_compare(version, "3.2") >= 0;

    Configlet cnf(fw, "pix_os", "snmp");
    cnf.removeComments();
    cnf.collapseEmptyStrings(true);

    cnf.setVariable("pix_version_lt_70", ! version_ge_70);
    cnf.setVariable("pix_version_ge_70",   version_ge_70);
    cnf.setVariable("fwsm_version_lt_32", ! version_ge_32);
    cnf.setVariable("fwsm_version_ge_32",   version_ge_32);

    bool set_communities = fw->getOptionsObject()->getBool(
        "pix_set_communities_from_object_data");
    bool set_sysinfo = fw->getOptionsObject()->getBool(
        "pix_set_sysinfo_from_object_data" );   
    bool enable_traps = fw->getOptionsObject()->getBool(
        "pix_enable_snmp_traps");

    cnf.setVariable("clear",
                    !fw->getOptionsObject()->getBool("pix_acl_no_clear"));
    cnf.setVariable("disable",
                    fw->getOptionsObject()->getBool("pix_disable_snmp_agent"));
    cnf.setVariable("not_disable",
                    ! fw->getOptionsObject()->getBool("pix_disable_snmp_agent"));
    cnf.setVariable("set_community", set_communities);
    cnf.setVariable(
        "read_community",
        fw->getManagementObject()->getSNMPManagement()->getReadCommunity().c_str());

    cnf.setVariable("set_sysinfo", set_sysinfo);
    QString location = fw->getOptionsObject()->getStr("snmp_location").c_str();
    QString contact = fw->getOptionsObject()->getStr("snmp_contact").c_str();

    cnf.setVariable("not_enable_traps", ! enable_traps);

    string snmp_server_1 = fw->getOptionsObject()->getStr("pix_snmp_server1");
    string snmp_server_2 = fw->getOptionsObject()->getStr("pix_snmp_server2");
    int snmp_poll_traps_1 = fw->getOptionsObject()->getInt("pix_snmp_poll_traps_1");
    int snmp_poll_traps_2 = fw->getOptionsObject()->getInt("pix_snmp_poll_traps_2");

    cnf.setVariable("not_server_1_empty", ! snmp_server_1.empty());
    cnf.setVariable("not_server_2_empty", ! snmp_server_2.empty());
    _configureSNMPServer(&cnf, 1, snmp_server_1, snmp_poll_traps_1);
    _configureSNMPServer(&cnf, 2, snmp_server_2, snmp_poll_traps_2);

    return cnf.expand().toStdString() + "\n";
}

void OSConfigurator_pix_os::_configureNTPServer(Configlet *cnf, int server_num,
                                                const std::string &server, bool pref)
{
    Helper helper(this);
    InetAddr srv_addr(server);
    int iface_id = helper.findInterfaceByNetzone(&srv_addr);
    if (iface_id == -1)
    {
        QString err("NTP server %1 does not belong to any known network zone");
        abort(fw, err.arg(server.c_str()).toStdString());
    }
    Interface *ntp_iface = Interface::cast(dbcopy->findInIndex(iface_id));

    QString var_name("not_server_%1_empty");
    cnf->setVariable(var_name.arg(server_num), ! server.empty());
    var_name = QString("address_%1");
    cnf->setVariable(var_name.arg(server_num), server.c_str());

    var_name = QString("interface_%1_label");
    cnf->setVariable(var_name.arg(server_num), ntp_iface->getLabel().c_str());

    var_name = QString("prefer_%1");
    cnf->setVariable(var_name.arg(server_num), pref);
}

string OSConfigurator_pix_os::_printNTP()
{
    string version = fw->getStr("version");
    string platform = fw->getStr("platform");

    // for pix
    bool version_ge_70 = XMLTools::version_compare(version, "7.0") >= 0;
    // for fwsm
    bool version_ge_32 = XMLTools::version_compare(version, "3.2") >= 0;

    Configlet cnf(fw, "pix_os", "ntp");
    cnf.removeComments();
    cnf.collapseEmptyStrings(true);
    cnf.setVariable("pix_version_lt_70", ! version_ge_70);
    cnf.setVariable("pix_version_ge_70",   version_ge_70);
    cnf.setVariable("fwsm_version_lt_32", ! version_ge_32);
    cnf.setVariable("fwsm_version_ge_32",   version_ge_32);

    string ntp_server_1=fw->getOptionsObject()->getStr("pix_ntp1");
    bool   ntp1_pref=fw->getOptionsObject()->getBool("pix_ntp1_pref");
    string ntp_server_2=fw->getOptionsObject()->getStr("pix_ntp2");
    bool   ntp2_pref=fw->getOptionsObject()->getBool("pix_ntp2_pref");
    string ntp_server_3=fw->getOptionsObject()->getStr("pix_ntp3");
    bool   ntp3_pref=fw->getOptionsObject()->getBool("pix_ntp3_pref");

    cnf.setVariable("clear",
                    !fw->getOptionsObject()->getBool("pix_acl_no_clear"));

    _configureNTPServer(&cnf, 1, ntp_server_1, ntp1_pref);
    _configureNTPServer(&cnf, 2, ntp_server_2, ntp2_pref);
    _configureNTPServer(&cnf, 3, ntp_server_3, ntp3_pref);

    return cnf.expand().toStdString() + "\n";
}

string OSConfigurator_pix_os::_printSysopt()
{
    ostringstream  res;
    string platform = fw->getStr("platform");
    string version = fw->getStr("version");

#ifndef NDEBUG
    FWOptions *options=fw->getOptionsObject();
    assert(options!=nullptr);
#endif

    bool tcpmss = fw->getOptionsObject()->getBool("pix_tcpmss");
    int  tcpmss_val = fw->getOptionsObject()->getInt("pix_tcpmss_value");

    res << endl;

    if (fw->getOptionsObject()->getBool("pix_resetinbound"))
        res << "service resetinbound" << endl;
    else
        res << "no service resetinbound" << endl;

    if (Resources::platform_res[platform]->getResourceBool(
            "/FWBuilderResources/Target/options/version_" + version +
            "/pix_resetoutside_supported"))
    {
        if (fw->getOptionsObject()->getBool("pix_resetoutside"))
            res << "service resetoutside" << endl;
        else
            res << "no service resetoutside" << endl;
    }

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
            "/FWBuilderResources/Target/options/version_" + version +
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

string OSConfigurator_pix_os::_printServiceTimeout(const string &pix_service)
{
    QStringList  res;

    QString hh, mm, ss;

    string version = fw->getStr("version");
    string platform = fw->getStr("platform");
    bool use_sunrpc = Resources::platform_res[platform]->getResourceBool(
        "/FWBuilderResources/Target/options/version_" + version +
        "/pix_timeout_rpc_is_sunrpc");

    hh = fw->getOptionsObject()->getStr(pix_service+"_hh").c_str();
    mm = fw->getOptionsObject()->getStr(pix_service+"_mm").c_str();
    ss = fw->getOptionsObject()->getStr(pix_service+"_ss").c_str();

    if ( ! hh.isEmpty() && ! mm.isEmpty() && ! ss.isEmpty())
    {
        QString service_name = pix_service.c_str();

        if (pix_service == "rpc" && use_sunrpc) service_name = "sunrpc";

        bool ok1, ok2, ok3;
        int hh_int = hh.toInt(&ok1); if (!ok1) hh_int = 0;
        int mm_int = mm.toInt(&ok2); if (!ok2) mm_int = 0;
        int ss_int = ss.toInt(&ok3); if (!ok3) ss_int = 0;

        if (!ok1 || !ok2 || !ok3)
            qDebug() << QString("Invalid timeout spec '%1:%2:%3'")
                .arg(hh).arg(mm).arg(ss);

        if (pix_service == "xlate" && hh_int == 0 && mm_int == 0 && ss_int == 0)
        {
            ss_int = 30;
        }

        res << QString("timeout %1 %2:%3:%4")
            .arg(service_name).arg(hh_int).arg(mm_int).arg(ss_int);

        if (pix_service == "uauth")
        {
            bool abs = fw->getOptionsObject()->getBool("uauth_abs");
            bool inact = fw->getOptionsObject()->getBool("uauth_inact");
            if (abs)   res << "absolute";
            if (inact) res << "inactivity";
        }

        res << "\n";
    }

    return res.join(" ").toStdString();
}

string OSConfigurator_pix_os::_printTimeouts()
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
    to = fw->getOptionsObject()->getInt("pix_telnet_timeout");
    if (to>60)  abort("Telnet timeout should not exceed 60 minutes");
    if (to!=0)  res << "telnet timeout " << to << endl;

    return res.str();
}


void OSConfigurator_pix_os::addVirtualAddressForNAT(const Address*)
{
}

void OSConfigurator_pix_os::addVirtualAddressForNAT(const Network*)
{
}

string OSConfigurator_pix_os::_printSSHConfiguration()
{
    string platform = fw->getStr("platform");
    string version = fw->getStr("version");

    // for pix
    bool version_ge_70 = XMLTools::version_compare(version, "7.0") >= 0;
    // for fwsm
    bool version_ge_32 = XMLTools::version_compare(version, "3.2") >= 0;

    Configlet cnf(fw, "pix_os", "ssh");
    cnf.removeComments();
    cnf.collapseEmptyStrings(true);
    cnf.setVariable("pix_version_lt_70", ! version_ge_70);
    cnf.setVariable("pix_version_ge_70",   version_ge_70);
    cnf.setVariable("fwsm_version_lt_32", ! version_ge_32);
    cnf.setVariable("fwsm_version_ge_32",   version_ge_32);

    cnf.setVariable("clear", 
                    ! fw->getOptionsObject()->getBool("pix_acl_no_clear") );

    cnf.setVariable("use_scp", fw->getOptionsObject()->getBool("use_scp"));

    int to = fw->getOptionsObject()->getInt("pix_ssh_timeout");
    if (to>60)  abort("SSH timeout should not exceed 60 minutes");
    cnf.setVariable("ssh_timeout", to);

    // ssh accress control is added later when we generate rules

    return cnf.expand().toStdString() + "\n";
}

