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

#include "OSData_pf.h"
#include "fwbuilder/Resources.h"

using namespace std;
using namespace libfwbuilder;

OSData_pf::OSData_pf() {}

string  OSData_pf::getPathForTool(const string &os,tools t)
{
    string r = "/FWBuilderResources/Target/tools/";
    switch (t)
    {
    case IFCONFIG: r += "path_ifconfig"; break;
    case SYSCTL:   r += "path_sysctl";   break;
    case PFCTL:    r += "path_pfctl";    break;
    case IPFW:     r += "path_ipfw";     break;
    case IPF:      r += "path_ipf";      break;
    case IPNAT:    r += "path_ipnat";    break;
    case LOGGER:   r += "path_logger";   break;
    }
    return Resources::os_res[os]->getResourceStr(r);
}

