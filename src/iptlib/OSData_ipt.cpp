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

#include "OSData_ipt.h"

using namespace std;

OSData_ipt::OSData_ipt(const std::string &ho)
{
    host_os = ho;

    attribute_names[LSMOD] = "path_lsmod";
    attribute_names[MODPROBE] = "path_modprobe";
    attribute_names[IPTABLES] = "path_iptables";
    attribute_names[IP6TABLES] = "path_ip6tables";
    attribute_names[IPTABLES_RESTORE] = "path_iptables_restore";
    attribute_names[IP6TABLES_RESTORE] = "path_ip6tables_restore";
    attribute_names[IP] = "path_ip";
    attribute_names[IFCONFIG] = "path_ifconfig";
    attribute_names[VCONFIG] = "path_vconfig";
    attribute_names[BRCTL] = "path_brctl";
    attribute_names[IFENSLAVE] = "path_ifenslave";
    attribute_names[IPSET] = "path_ipset";
    attribute_names[LOGGER] = "path_logger";

    variable_names[LSMOD] = "LSMOD";
    variable_names[MODPROBE] = "MODPROBE";
    variable_names[IPTABLES] = "IPTABLES";
    variable_names[IP6TABLES] = "IP6TABLES";
    variable_names[IPTABLES_RESTORE] = "IPTABLES_RESTORE";
    variable_names[IP6TABLES_RESTORE] = "IP6TABLES_RESTORE";
    variable_names[IP] = "IP";
    variable_names[IFCONFIG] = "IFCONFIG";
    variable_names[VCONFIG] = "VCONFIG";
    variable_names[BRCTL] = "BRCTL";
    variable_names[IFENSLAVE] = "IFENSLAVE";
    variable_names[IPSET] = "IPSET";
    variable_names[LOGGER] = "LOGGER";

    all_tools.push_back(LSMOD);
    all_tools.push_back(MODPROBE);
    all_tools.push_back(IPTABLES);
    all_tools.push_back(IP6TABLES);
    all_tools.push_back(IPTABLES_RESTORE);
    all_tools.push_back(IP6TABLES_RESTORE);
    all_tools.push_back(IP);
    all_tools.push_back(IFCONFIG);
    all_tools.push_back(VCONFIG);
    all_tools.push_back(BRCTL);
    all_tools.push_back(IFENSLAVE);
    all_tools.push_back(IPSET);
    all_tools.push_back(LOGGER);
}

