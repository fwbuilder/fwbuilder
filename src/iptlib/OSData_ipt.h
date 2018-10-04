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

#ifndef __OSDATA_IPT_HH
#define __OSDATA_IPT_HH


#include <string>
#include <map>
#include <list>

class OSData_ipt {

    std::string host_os;
    std::map<int, std::string> variable_names;
    std::map<int, std::string> attribute_names;
    std::list<int> all_tools;
    
 public:

    OSData_ipt(const std::string &ho);
    
    typedef enum { LSMOD,
                   MODPROBE,
                   IPTABLES,
                   IP6TABLES,
                   IPTABLES_RESTORE,
                   IP6TABLES_RESTORE,
                   IP,
                   IFCONFIG,
                   VCONFIG,
                   BRCTL,
                   IFENSLAVE,
                   IPSET,
                   LOGGER } tools;

    std::string getVariableName(tools t) { return variable_names[t]; }
    const std::list<int>& getAllTools() { return all_tools; }
    std::string getAttributeNameForTool(tools t) { return attribute_names[t]; }
};

#endif
