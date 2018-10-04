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
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/XMLTools.h"

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

#include <assert.h>

#include <QString>
#include <QtDebug>
#include <QRegExp>
#include <QStringList>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string OSConfigurator_linux24::myPlatformName() { return "Linux24"; }

OSConfigurator_linux24::OSConfigurator_linux24(FWObjectDatabase *_db,
                                               Firewall *fw,
                                               bool ipv6_policy) : 
    OSConfigurator(_db, fw, ipv6_policy) , os_data(fw->getStr("host_OS"))
{
    command_wrappers = new Configlet(fw, "linux24", "run_time_wrappers");

    FWOptions* fwopt = fw->getOptionsObject();
    string version = fw->getStr("version");
    using_ipset = (XMLTools::version_compare(version, "1.4.1.1") >= 0 &&
                          fwopt->getBool("use_m_set"));
}

OSConfigurator_linux24::~OSConfigurator_linux24()
{
    delete command_wrappers;
}

/*
 * this function generates acceptable shell variable name from
 * interface name. Note that PolicyCompiler_ipt::getInterfaceVarName()
 * and NATCompiler_ipt::getInterfaceVarName do the same thing and
 * these functions should be identical.
 *
 * TODO: really need to have one function for this instead of three in
 * three different classes.
 */
string OSConfigurator_linux24::getInterfaceVarName(FWObject *iface, bool v6)
{
    ostringstream  ostr;
    string iname=iface->getName();
    string::size_type p1;
    while ( (p1=iname.find("."))!=string::npos)
        iname=iname.replace(p1,1,"_");
    while ( (p1=iname.find("-"))!=string::npos)
        iname=iname.replace(p1,1,"_");
    ostr << "i_" << iname;
    if (v6) ostr << "_v6";
    return ostr.str();
}

void OSConfigurator_linux24::setConfigletMacroForOptionStr(
    const string &s, Configlet *c, const char *option_name)
{
    // "" means no change, do not include the command in the script
    if (!s.empty())
    {
        c->setVariable(QString("if_") + option_name, 1);
        c->setVariable(option_name, s.c_str());
    } else
    {
        c->setVariable(QString("if_") + option_name, 0);
        c->setVariable(option_name, s.c_str());
    }
}

void OSConfigurator_linux24::setConfigletMacroForOptionInt(
    int val, Configlet *c, const char *option_name)
{
    // -1 means no change, do not include the command in the script
    if (val >= 0)
    {
        c->setVariable(QString("if_") + option_name, 1);
        c->setVariable(option_name, val);
    } else
    {
        c->setVariable(QString("if_") + option_name, 0);
        c->setVariable(option_name, 0);
    }
}

void OSConfigurator_linux24::processFirewallOptions() 
{
    Configlet kernel_vars(fw, "linux24", "kernel_vars");
    kernel_vars.removeComments();
    kernel_vars.collapseEmptyStrings(true);

    FWOptions* options = fw->getOptionsObject();

    setConfigletMacroForOptionStr(options->getStr("linux24_ip_dynaddr"), &kernel_vars, "linux24_ip_dynaddr");
    setConfigletMacroForOptionStr(options->getStr("linux24_rp_filter"), &kernel_vars, "linux24_rp_filter");
    setConfigletMacroForOptionStr(options->getStr("linux24_accept_source_route"), &kernel_vars, "linux24_accept_source_route");
    setConfigletMacroForOptionStr(options->getStr("linux24_accept_redirects"), &kernel_vars, "linux24_accept_redirects");
    setConfigletMacroForOptionStr(options->getStr("linux24_log_martians"), &kernel_vars, "linux24_log_martians");
    setConfigletMacroForOptionStr(options->getStr("linux24_icmp_echo_ignore_broadcasts"), &kernel_vars, "linux24_icmp_echo_ignore_broadcasts");
    setConfigletMacroForOptionStr(options->getStr("linux24_icmp_echo_ignore_all"), &kernel_vars, "linux24_icmp_echo_ignore_all");
    setConfigletMacroForOptionStr(options->getStr("linux24_icmp_ignore_bogus_error_responses"), &kernel_vars, "linux24_icmp_ignore_bogus_error_responses");
    setConfigletMacroForOptionStr(options->getStr("linux24_tcp_window_scaling"), &kernel_vars, "linux24_tcp_window_scaling");
    setConfigletMacroForOptionStr(options->getStr("linux24_tcp_sack"), &kernel_vars, "linux24_tcp_sack");
    setConfigletMacroForOptionStr(options->getStr("linux24_tcp_fack"), &kernel_vars, "linux24_tcp_fack");
    setConfigletMacroForOptionStr(options->getStr("linux24_tcp_syncookies"), &kernel_vars, "linux24_tcp_syncookies");
    setConfigletMacroForOptionStr(options->getStr("linux24_tcp_ecn"), &kernel_vars, "linux24_tcp_ecn");
    setConfigletMacroForOptionStr(options->getStr("linux24_tcp_timestamps"), &kernel_vars, "linux24_tcp_timestamps");

    int opt = options->getInt("linux24_tcp_fin_timeout");
    setConfigletMacroForOptionInt((opt==0)?-1:opt, &kernel_vars, "linux24_tcp_fin_timeout");
    opt = options->getInt("linux24_tcp_keepalive_interval");
    setConfigletMacroForOptionInt((opt==0)?-1:opt, &kernel_vars, "linux24_tcp_keepalive_interval");

    Configlet conntrack_vars(fw, "linux24", "conntrack");
    conntrack_vars.removeComments();
    conntrack_vars.collapseEmptyStrings(true);

    string version = fw->getStr("version");
    bool version_ge_1_4 = XMLTools::version_compare(version, "1.4.0") >= 0;
    conntrack_vars.setVariable("iptables_version_ge_1_4", version_ge_1_4);
    conntrack_vars.setVariable("iptables_version_lt_1_4", !version_ge_1_4);

    // if conntrack_max and conntrack_hashsize are equal to 0, we do
    // not add commands from the configlet (so the kernel defaults are
    // used). Options above assume -1 is the default. Need to pass -1
    // instead of 0 for the conntrack vars
    opt = options->getInt("linux24_conntrack_max");
    setConfigletMacroForOptionInt(
        (opt==0)?-1:opt,
        &conntrack_vars, "conntrack_max");
    opt = options->getInt("linux24_conntrack_hashsize");
    setConfigletMacroForOptionInt(
        (opt==0)?-1:opt,
        &conntrack_vars, "conntrack_hashsize");

    // This option uses three-state control and assumes empty string is the default
    setConfigletMacroForOptionStr(
        options->getStr("linux24_conntrack_tcp_be_liberal"),
        &conntrack_vars, "conntrack_tcp_be_liberal");

    output << kernel_vars.expand().toStdString();
    output << endl;
    output << conntrack_vars.expand().toStdString();
}

void OSConfigurator_linux24::addVirtualAddressForNAT(const Network *nw)
{
    FWOptions* options=fw->getOptionsObject();
    if (options->getBool("manage_virtual_addr"))
    {
        if (virtual_addresses.empty() || 
            find(virtual_addresses.begin(),virtual_addresses.end(),
                 *(nw->getAddressPtr())) == virtual_addresses.end())
        {
            Interface *iface = findInterfaceFor( nw, fw );
            if (iface!=nullptr)
            {
                const InetAddr *addr = nw->getAddressPtr();
                InetAddr first, last;
                InetAddr a;

                first = *addr;
                last = *(nw->getBroadcastAddressPtr());

                QStringList addresses;
                for (a=first; a<last; a=a+1)
                {
                    addresses.push_back(QString("%1/32").arg(a.toString().c_str()));
                }

                if (virtual_addresses_for_nat.count(iface->getName()) > 0)
                    addresses.push_front(virtual_addresses_for_nat[iface->getName()].c_str());

                virtual_addresses_for_nat[iface->getName()] =
                    addresses.join(" ").toStdString();

                virtual_addresses.push_back( *(nw->getAddressPtr()) );
                registerVirtualAddressForNat();
            } else
                warning("Can not add virtual address " +
                        nw->getAddressPtr()->toString() +
                        " (object " + nw->getName() + ")" );
        }
    }
}

void OSConfigurator_linux24::addVirtualAddressForNAT(const Address *addr)
{
    FWOptions* options=fw->getOptionsObject();
    if ( options->getBool("manage_virtual_addr") )
    {
        const InetAddr *addr_addr = addr->getAddressPtr();

        if (virtual_addresses.empty() || 
            find(virtual_addresses.begin(),
                 virtual_addresses.end(), *addr_addr) == virtual_addresses.end())
        {
            FWObject *vaddr = findAddressFor(addr, fw );
            if (vaddr!=nullptr)
            {
                Interface *iface = Interface::cast(vaddr->getParent());
                assert(iface!=nullptr);

                QStringList addresses;
                const InetAddr *vaddr_netm =
                    Address::cast(vaddr)->getNetmaskPtr();

                addresses.push_back(QString("%1/%2").
                                    arg(addr_addr->toString().c_str()).
                                    arg(vaddr_netm->getLength()));

                if (virtual_addresses_for_nat.count(iface->getName()) > 0)
                    addresses.push_front(virtual_addresses_for_nat[iface->getName()].c_str());

                virtual_addresses_for_nat[iface->getName()] =
                    addresses.join(" ").toStdString();
        
                virtual_addresses.push_back(*(addr_addr));
                registerVirtualAddressForNat();
            } else
                warning("Can not add virtual address for object " + addr->getName());
        }
        return;
    }
}

string OSConfigurator_linux24::normalizeSetName(const string &txt)
{
    QString table_name = txt.c_str();
    table_name.replace(QRegExp("[ +*!#|]"), "_");
    return table_name.toStdString();
}

void OSConfigurator_linux24::registerMultiAddressObject(MultiAddressRunTime *at)
{
    // std::map<std::string,std::string>
    address_table_objects[normalizeSetName(at->getName())] = at->getSourceName();
}

int  OSConfigurator_linux24::prolog()
{
    return 0;
}

/**
 * Print shell functions used by the script. If argument (boolean) is true,
 * do not add comments.
 */
string OSConfigurator_linux24::printShellFunctions(bool have_ipv6)
{
    QStringList output;
    FWOptions* options = fw->getOptionsObject();

    string version = fw->getStr("version");
    // string host_os = fw->getStr("host_OS");
    // string os_family = Resources::os_res[host_os]->
    //     getResourceStr("/FWBuilderResources/Target/family");

    Configlet shell_functions(fw, "linux24", "shell_functions");
    output.push_back(shell_functions.expand());

/* check if package iproute2 is installed, but do this only if
 * we really need /usr/sbin/ip 
 */
    Configlet configlet(fw, "linux24", "check_utilities");
    configlet.removeComments();
    configlet.collapseEmptyStrings(true);

    configlet.setVariable("load_modules", options->getBool("load_modules"));

    if (options->getBool("load_modules") ||
        options->getBool("configure_vlan_interfaces") ||
        options->getBool("configure_bonding_interfaces"))
    {
        configlet.setVariable("need_modprobe", true);
    }

    if (options->getBool("verify_interfaces") || 
        options->getBool("manage_virtual_addr") ||
        options->getBool("configure_interfaces") )
    {
        configlet.setVariable("need_vconfig", 
                              options->getBool("configure_vlan_interfaces"));
        configlet.setVariable("need_brctl", 
                              options->getBool("configure_bridge_interfaces"));
        configlet.setVariable("need_ifenslave", 
                              options->getBool("configure_bonding_interfaces"));
    }

    configlet.setVariable("need_ipset", using_ipset);

    configlet.setVariable("need_iptables_restore",
                          options->getBool("use_iptables_restore"));

    configlet.setVariable("need_ip6tables_restore",
                          have_ipv6 && options->getBool("use_iptables_restore"));

    output.push_back(configlet.expand());

    /*
     * Generate commands to reset all tables and chains and set
     * default policy
     */
    Configlet reset_iptables(fw, "linux24", "reset_iptables");
    if (XMLTools::version_compare(version, "1.4.20") >= 0)
        reset_iptables.setVariable("opt_wait", "-w");
    else
        reset_iptables.setVariable("opt_wait", "");

    output.push_back(reset_iptables.expand());

    Configlet addr_conf(fw, "linux24", "update_addresses");
    output.push_back(addr_conf.expand());

    if (options->getBool("configure_vlan_interfaces"))
    {
        Configlet conf(fw, "linux24", "update_vlans");
        output.push_back(conf.expand());
    }

    if (options->getBool("configure_bridge_interfaces"))
    {
        Configlet conf(fw, "linux24", "update_bridge");
        output.push_back(conf.expand());
    }

    if (options->getBool("configure_bonding_interfaces"))
    {
        Configlet conf(fw, "linux24", "update_bonding");
        output.push_back(conf.expand());
    }

    return output.join("\n").toStdString();
}

string OSConfigurator_linux24::printRunTimeAddressTablesCode()
{
    Configlet conf(fw, "linux24", "run_time_address_tables");
    conf.setVariable("using_ipset", using_ipset);

    ostringstream check_ostr;
    ostringstream load_ostr;
    map<string,string>::iterator i;
    for (i=address_table_objects.begin(); i!=address_table_objects.end(); ++i)
    {
        string at_name = i->first;
        string at_file = i->second;
        // If the file name is empty, this run-time table is
        // completely controlled by the user outside fwbuilder so we
        // do not need to add commands to check if the file exits and
        // load it
        if (!at_file.empty())
        {
            check_ostr << "check_file \"" + at_name +
                "\" \"" + at_file + "\"" << endl;
            load_ostr << "reload_address_table \"" + at_name +
                "\" \"" + at_file + "\"" << endl;
        }
    }

    conf.setVariable("check_files_commands", check_ostr.str().c_str());
    conf.setVariable("load_files_commands", load_ostr.str().c_str());

    return conf.expand().toStdString();
}

string OSConfigurator_linux24::getPathForATool(const std::string &os_variant, OSData_ipt::tools tool_name)
{
    FWOptions* options = fw->getOptionsObject();
    string attr = os_data.getAttributeNameForTool(tool_name);

    string s = options->getStr("linux24_" + attr);
    if (!s.empty()) return s;

    string host_os = fw->getStr("host_OS");
    string r = "/FWBuilderResources/Target/tools/" + os_variant + "/" + attr;
    if (Resources::os_res[host_os]->getResourceStr(r).empty())
        r = "/FWBuilderResources/Target/tools/Unknown/" + attr;
    
    return Resources::os_res[host_os]->getResourceStr(r);
}

string  OSConfigurator_linux24::printPathForAllTools(const string &os)
{
    ostringstream res;

    list<int>::const_iterator i;
    const list<int> &all_tools = os_data.getAllTools(); 
    for (i=all_tools.begin(); i!=all_tools.end(); ++i)
        res << os_data.getVariableName(OSData_ipt::tools(*i))
            << "=\""
            << getPathForATool(os, OSData_ipt::tools(*i))
            << "\""
            << endl;
    return res.str();
}

string OSConfigurator_linux24::generateCodeForProtocolHandlers()
{
    FWOptions* options = fw->getOptionsObject();
    bool nomod = Resources::os_res[fw->getStr("host_OS")]->
        Resources::getResourceBool("/FWBuilderResources/Target/options/suppress_modules");

    // string host_os = fw->getStr("host_OS");
    // string os_family = Resources::os_res[host_os]->
    //     getResourceStr("/FWBuilderResources/Target/family");
    Configlet load_modules(fw, "linux24", "load_modules");
    load_modules.removeComments();

    // See ticket #2
    string modules_dir = Resources::os_res[fw->getStr("host_OS")]->
        Resources::getResourceStr("/FWBuilderResources/Target/options/default/modules_dir");

/* there is no need to load modules on some platforms */
    load_modules.setVariable("load_modules", options->getBool("load_modules") && !nomod);
    load_modules.setVariable("modules_dir", modules_dir.c_str());
    return load_modules.expand().toStdString();
}

QString OSConfigurator_linux24::addressTableWrapper(FWObject *rule,
                                                    const QString &command,
                                                    bool ipv6)
{
    QString combined_command = command;
    QRegExp address_table_re("\\$at_(\\S+)");
    int pos = address_table_re.indexIn(command);
    if (pos > -1)
    {
        QStringList command_lines = QString(command).split("\n", QString::SkipEmptyParts);
        if (command_lines.size() > 1)
        {
            command_lines.push_front("{");
            command_lines.push_back("}");
        }
        combined_command = command_lines.join("\n");

        command_wrappers->clear();
        command_wrappers->removeComments();
        command_wrappers->collapseEmptyStrings(true);
        command_wrappers->setVariable("ipv6", ipv6);

        QString at_var = address_table_re.cap(1);
        QString at_file = rule->getStr("address_table_file").c_str();

        command_wrappers->setVariable("address_table_file", at_file);
        command_wrappers->setVariable("address_table_var", at_var);
        command_wrappers->setVariable("command", combined_command);
        command_wrappers->setVariable("address_table", true);
        command_wrappers->setVariable("wildcard_interface", false);
        command_wrappers->setVariable("no_dyn_addr", false);
        command_wrappers->setVariable("one_dyn_addr", false);
        command_wrappers->setVariable("two_dyn_addr", false);

        combined_command = command_wrappers->expand();
    }
    return combined_command;
}

string OSConfigurator_linux24::printRunTimeWrappers(FWObject *rule,
                                                    const string &command,
                                                    bool ipv6)
{
/* if anywhere in command_line we used variable holding an address of
 * dynamic interface (named $i_something) then we need to add this
 * command with a check for the value of this variable. We execute
 * iptables command only if the value is a non-empty string.
 *
 * bug #1851166: there could be two dynamic interfaces in the same
 * rule.
 */

    bool wildcard_interface = false;
    QString combined_command;

    if (using_ipset)
        combined_command = command.c_str();
    else
        combined_command = addressTableWrapper(rule, command.c_str(), ipv6);

    command_wrappers->clear();
    command_wrappers->removeComments();
    command_wrappers->collapseEmptyStrings(true);
    command_wrappers->setVariable("ipv6", ipv6);

    command_wrappers->setVariable("address_table", false);

    QRegExp intf_re("\\$i_([^ :]+)");

    QStringList iface_names;
    QStringList iface_vars;
    int pos = -1;
    while ((pos = intf_re.indexIn(combined_command, pos + 1)) > -1)
    {
        QString name = intf_re.cap(1);
        int match_len = intf_re.matchedLength();
        iface_names.push_back(name);
        iface_vars.push_back("$i_" + name);
        if (name.contains("*")) 
        {
            wildcard_interface = true;
            QString intf_family = name.section('*', 0, 0);
            command_wrappers->setVariable("interface_family_name", intf_family);

            // replace $i_ppp* with $addr. This must match shell code
            // in the configlet run_time_wrappers
            combined_command.replace(pos, match_len, "$addr");

            break;
        }
    }

    bool no_wrapper = !wildcard_interface && iface_names.size() == 0;

    if (!no_wrapper)
    {
        QStringList command_lines = 
            QString(combined_command).split("\n", QString::SkipEmptyParts);
        if (command_lines.size() > 1)
        {
            command_lines.push_front("{");
            command_lines.push_back("}");
        }
        combined_command = command_lines.join("\n");
    }

    command_wrappers->setVariable("no_wrapper", no_wrapper);
    command_wrappers->setVariable("wildcard_interface", wildcard_interface);
    command_wrappers->setVariable("one_dyn_addr",
                                  !wildcard_interface && iface_names.size() == 1);
    command_wrappers->setVariable("two_dyn_addr",
                                  !wildcard_interface && iface_names.size() > 1);

    for (int idx=0; idx<iface_names.size(); ++idx)
    {
        QString intf_name = iface_names[idx];
        //if (ipv6) intf_name += "_v6";
        command_wrappers->setVariable(QString("intf_%1_var_name").arg(idx+1),
                                      intf_name);
    }

    command_wrappers->setVariable("command", combined_command);

    return command_wrappers->expand().toStdString() + "\n";
}

string OSConfigurator_linux24::printIPForwardingCommands()
{
/* Turn on packet forwarding if we have to */
    // string os_family = Resources::os_res[fw->getStr("host_OS")]->
    //     getResourceStr("/FWBuilderResources/Target/family");
    FWOptions* options = fw->getOptionsObject();
    Configlet ip_forwarding(fw, "linux24", "ip_forwarding");
    ip_forwarding.removeComments();
    ip_forwarding.collapseEmptyStrings(true);

    string s = options->getStr("linux24_ip_forward");
    ip_forwarding.setVariable("ipv4", !s.empty());
    ip_forwarding.setVariable("ipv4_forw", (s=="1" || s=="On" || s=="on")?1:0);

    s = options->getStr("linux24_ipv6_forward");
    ip_forwarding.setVariable("ipv6", !s.empty());
    ip_forwarding.setVariable("ipv6_forw", (s=="1" || s=="On" || s=="on")?1:0);

    return ip_forwarding.expand().toStdString();
}

void OSConfigurator_linux24::epilog()
{
}


map<string, string> OSConfigurator_linux24::getGeneratedFiles() const
{
    map<string, string> files;
    return files;
}
