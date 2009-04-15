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

#include "OSData.h"
#include "fwbuilder/Resources.h"

#include <iostream>

using namespace std;


string  OSData::getPathForTool(const string &distro, tools t)
{
    string r="/FWBuilderResources/Target/tools/"+distro+"/";

    if (Resources::os_res[host_os]->getResourceStr(r+"path_ip").empty())
        r="/FWBuilderResources/Target/tools/Unknown/";

    switch (t)
    {
    case LSMOD:             r+="path_lsmod";            break;
    case MODPROBE:          r+="path_modprobe";         break;
    case IPTABLES:          r+="path_iptables";         break;
    case IP6TABLES:         r+="path_ip6tables";        break;
    case IPTABLES_RESTORE:  r+="path_iptables_restore"; break;
    case IP6TABLES_RESTORE: r+="path_ip6tables_restore"; break;
    case IP:                r+="path_ip";               break;
    case LOGGER:            r+="path_logger";           break;
    }

    return Resources::os_res[host_os]->getResourceStr(r);
}

