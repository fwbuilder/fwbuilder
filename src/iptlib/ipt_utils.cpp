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

#include "ipt_utils.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/ObjectGroup.h"

#include "fwcompiler/Compiler.h"

#include "combinedAddress.h"

#include <QRegExp>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


void build_interface_groups(
    FWObjectDatabase *dbcopy, Library *persistent_objects, Firewall *fw, bool ipv6,
    QMap<QString, libfwbuilder::FWObject*> &regular_interfaces)
{
    // object group that will hold all regular inetrfaces
    FWObject *all_itf_group = dbcopy->create(ObjectGroup::TYPENAME);
    persistent_objects->add(all_itf_group);
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
                persistent_objects->add(itf_group);
                itf_group->setName(iname.toStdString());
                regular_interfaces[iname] = itf_group;
            }

            regular_interfaces[iname]->addRef(iface);
            regular_interfaces["*"]->addRef(iface);
        }

    }
}




void expand_interface_with_phys_address(Compiler *compiler,
                                        Rule *rule,
                                        Interface *iface, 
                                        std::list<FWObject*> &ol1,
                                        std::list<FWObject*> &list_result)
{
    (void) rule; // Unused

    std::list<FWObject*> lipaddr;
    std::list<FWObject*> lother;
    physAddress *pa = nullptr;

    for (std::list<FWObject*>::iterator j=ol1.begin(); j!=ol1.end(); j++)
    {
        if ((*j)->getTypeName() == IPv4::TYPENAME)
        {
            lipaddr.push_back(*j);
            continue;
        }
        if (physAddress::cast(*j)!=nullptr)
        {
            pa = physAddress::cast(*j);
            continue;
        }
        lother.push_back(*j);
    }

/* 
 * if pa==nullptr then this is trivial case: there is no physical address
 */
    if (pa==nullptr)
    {
        list_result.insert(list_result.end(), ol1.begin(), ol1.end());
        return;
    }

/* At this point we have physAddress object and have to deal with it
 *
 * Compiler::_expand_interface picks all IPv4 objects and physAddress
 * object under Interface; it can also add interface object(s) to
 * the list.
 *
 * We have two possibilities now: there could be IPv4 objects or
 * not. In either case list ol1 may contain also interface object(s).
 * If there are IPv4 objects, we replace them with combinedAddress
 * objects which store information about IPv4 address and physAddress pa.
 * If there were no IPv4 objects, then we pass physAddress along.
 * We always copy interface  objects to the output list.
 * 
 *
 *
 * we use physAddress only if Host option "use_mac_addr_filter" of the
 * parent Host object is true
 */
    FWObject *p = Host::getParentHost(iface);
    //FWObject *p = iface->getParentHost();
    assert(p!=nullptr);

    FWOptions *hopt = Host::cast(p)->getOptionsObject();
    bool use_mac = (hopt!=nullptr && hopt->getBool("use_mac_addr_filter") );

    if (lipaddr.empty())    list_result.push_back(pa);
    else
    {
        std::list<FWObject*>::iterator j=lipaddr.begin();
        for ( ; j!=lipaddr.end(); j++)
        {
            const InetAddr *ip_addr = Address::cast(*j)->getAddressPtr();
            const InetAddr *ip_netm = Address::cast(*j)->getNetmaskPtr();

            if (use_mac)
            {
                combinedAddress *ca = new combinedAddress();
                compiler->persistent_objects->add(ca);
                ca->setName( "CA("+iface->getName()+")" );
                ca->setAddress( *ip_addr );
                ca->setNetmask( *ip_netm );
                ca->setPhysAddress( pa->getPhysAddress() );

                list_result.push_back(ca);
            } else
                list_result.push_back(*j);
        }
    }

    list_result.insert(list_result.end(), lother.begin(), lother.end());
}



