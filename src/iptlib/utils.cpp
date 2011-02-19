/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

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

#include "utils.h"

#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

#include <QRegExp>


using namespace libfwbuilder;
using namespace std;


void build_interface_groups(
    FWObjectDatabase *dbcopy, Firewall *fw, bool ipv6,
    QMap<QString, libfwbuilder::FWObject*> &regular_interfaces)
{
    // object group that will hold all regular inetrfaces
    FWObject *all_itf_group = dbcopy->create(ObjectGroup::TYPENAME);
    dbcopy->add(all_itf_group);
    all_itf_group->setName("*");
    regular_interfaces["*"] = all_itf_group;

    list<FWObject*> l2 = fw->getByTypeDeep(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) 
    {
        Interface *iface = Interface::cast(*i);
        assert(iface);

        if (iface->isLoopback() || iface->isUnnumbered() || iface->isBridgePort())
            continue;

        FWObjectTypedChildIterator ipv4_addresses =
            iface->findByType(IPv4::TYPENAME);
        FWObjectTypedChildIterator ipv6_addresses =
            iface->findByType(IPv6::TYPENAME);

        if ((ipv6 && ipv6_addresses != ipv6_addresses.end()) ||
            (!ipv6 && ipv4_addresses != ipv4_addresses.end()) ||
            (ipv4_addresses == ipv4_addresses.end() && ipv6_addresses == ipv6_addresses.end()))
        {
            /* 
             * regular_interfaces is a set of groups of
             * interfaces, where each group holds references to
             * all interfaces with "similar names". The group name
             * is then the base name of these interfaces with
             * numeric index replaced with "*". For example:
             * group "eth*" { eth0, eth1, eth2, ... }
             *
             * if interface name ends with '*', this is wildcard
             * interface. Just replace '*' with '+'. If interace
             * name does not end with '*', replace numeric
             * interface index with '+'.
             */

            QString iname = QString(iface->getName().c_str());
            iname.replace(QRegExp("[0-9]{1,}$"), "+");
            iname.replace("*", "+");
                
            if (regular_interfaces.count(iname) == 0)
            {
                FWObject *itf_group = dbcopy->create(ObjectGroup::TYPENAME);
                dbcopy->add(itf_group);
                itf_group->setName(iname.toStdString());
                regular_interfaces[iname] = itf_group;
            }

            regular_interfaces[iname]->addRef(iface);
            regular_interfaces["*"]->addRef(iface);
        }

    }
}



