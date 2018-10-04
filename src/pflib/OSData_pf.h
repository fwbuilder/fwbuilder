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

#ifndef __OSDATA_PF_HH
#define __OSDATA_PF_HH


#include <string>
#include <map>


class OSData_pf {

    public:

    OSData_pf();
    
    typedef enum { IFCONFIG, SYSCTL, PFCTL, IPFW, IPF, IPNAT, LOGGER } tools;

    std::string  getPathForTool(const std::string &os,tools t);
};

#endif
