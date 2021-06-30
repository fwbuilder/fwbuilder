/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "freebsdInterfaces.h"

#include "fwbuilder/Interface.h"
#include "fwbuilder/Firewall.h"

#include <QStringList>

using namespace libfwbuilder;

/*
 see #2032.  About dynamic interfaces:

 - when we generate rc.conf file, we should add line 
   "ifconfig_em0="DHCP"" for dynamic interfaces, so we should 
   include them in the management list as well.
*/
bool freebsdInterfaces::manageIpAddresses(Interface *intf,
                                          QStringList &update_addresses,
                                          QStringList &ignore_addresses)
{
    if (intf->isDyn())
    {
        FWObject *p = intf;
        while (Firewall::cast(p) == nullptr) p = p->getParent();
        Firewall *fw = Firewall::cast(p);
        FWOptions* options = fw->getOptionsObject();
        return options->getBool("generate_rc_conf_file");
    } else
        return openbsdInterfaces::manageIpAddresses(
            intf, update_addresses, ignore_addresses);
}

