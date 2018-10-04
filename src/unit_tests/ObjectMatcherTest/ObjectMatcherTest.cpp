/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

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


#define o(x) dbsearch(db, x)

#include "ObjectMatcherTest.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/ObjectMatcher.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Constants.h"

using namespace libfwbuilder;
using namespace std;

FWObject* dbsearch(FWObject *parent, string name)
{
    for (list<FWObject*>::iterator it = parent->begin(); it != parent->end(); it++)
    {
        FWObject *obj = *it;
        if (obj->getName() == name)
            return obj;
        FWObject *chs = dbsearch(obj, name);
        if (chs != nullptr) return chs;
    }
    return nullptr;
}

void ObjectMatcherTest::matchTest()
{
    FWObjectDatabase *db = new FWObjectDatabase();

    XMLTools::UpgradePredicate up;
    db->load(string("test_data.fwb"), &up, Constants::getTemplateDirectory());

    ObjectMatcher om;
    om.setRecognizeBroadcasts(true);
    om.setRecognizeMulticasts(true);
    om.setIPV6(false);
    om.setMatchSubnets(false);
    om.setAddressRangeMatchMode(ObjectMatcher::EXACT);

    Firewall *fw1 = Firewall::cast(dbsearch(db, "fw1"));
    CPPUNIT_ASSERT(fw1 != nullptr);

    Interface *fw1_eth0 = Interface::cast(dbsearch(fw1, "eth0"));
    CPPUNIT_ASSERT(fw1_eth0 != nullptr);

    Interface *fw1_eth1 = Interface::cast(dbsearch(fw1, "eth1"));
    CPPUNIT_ASSERT(fw1_eth1 != nullptr);

    Interface *fw1_eth2 = Interface::cast(dbsearch(fw1, "eth2"));
    CPPUNIT_ASSERT(fw1_eth2 != nullptr);

    IPv6 *fw1_eth2_ipv6 = IPv6::cast(dbsearch(fw1_eth2, "fw1:eth2:ipv6"));
    CPPUNIT_ASSERT(fw1_eth2_ipv6 != nullptr);

    physAddress *fw1_eth2_mac = physAddress::cast(dbsearch(fw1_eth2, "fw1:eth2:mac"));
    CPPUNIT_ASSERT(fw1_eth2_mac != nullptr);

    Host *host1 = Host::cast(dbsearch(db, "host1"));
    CPPUNIT_ASSERT(host1 != nullptr);

    Host *host2 = Host::cast(dbsearch(db, "host2"));
    CPPUNIT_ASSERT(host2 != nullptr);

    Host *host3 = Host::cast(dbsearch(db, "host3"));
    CPPUNIT_ASSERT(host3 != nullptr);

    Interface *host1_eth0 = Interface::cast(dbsearch(host1, "eth0"));
    CPPUNIT_ASSERT(host1_eth0 != nullptr);

    Interface *host2_eth0 = Interface::cast(dbsearch(host2, "eth0"));
    CPPUNIT_ASSERT(host2_eth0 != nullptr);

    IPv4 *host2_eth0_ip = IPv4::cast(dbsearch(host2_eth0, "ip"));
    CPPUNIT_ASSERT(host2_eth0_ip != nullptr);

    Interface *host3_eth0 = Interface::cast(dbsearch(host3, "eth0"));
    CPPUNIT_ASSERT(host3_eth0 != nullptr);

    Interface *host3_eth1 = Interface::cast(dbsearch(host3, "eth1"));
    CPPUNIT_ASSERT(host3_eth1 != nullptr);


    CPPUNIT_ASSERT(om.dispatch(fw1, fw1));
    CPPUNIT_ASSERT(om.dispatch(fw1_eth0, fw1));
    CPPUNIT_ASSERT(om.dispatch(fw1_eth1, fw1));
    CPPUNIT_ASSERT(om.dispatch(fw1_eth0, fw1_eth0));

    CPPUNIT_ASSERT( ! om.dispatch(fw1_eth2_ipv6, fw1));
    om.setIPV6(true);
    CPPUNIT_ASSERT( om.dispatch(fw1_eth2_ipv6, fw1));
    om.setIPV6(false);

    CPPUNIT_ASSERT(om.dispatch(fw1_eth2_mac, fw1));

    CPPUNIT_ASSERT(om.dispatch(host1_eth0, fw1));
    CPPUNIT_ASSERT(om.dispatch(host1, fw1));

    CPPUNIT_ASSERT(om.dispatch(host2_eth0_ip, fw1) == false);
    CPPUNIT_ASSERT(om.dispatch(host2_eth0, fw1) == false);
    CPPUNIT_ASSERT(om.dispatch(host2, fw1) == false);

    om.setMatchSubnets(true);
    CPPUNIT_ASSERT(om.dispatch(host2_eth0_ip, fw1));

    om.setMatchSubnets(false);

    CPPUNIT_ASSERT(om.dispatch(host3_eth0, fw1) == false);
    CPPUNIT_ASSERT(om.dispatch(host3_eth1, fw1) == false);

    CPPUNIT_ASSERT(om.dispatch(IPv4::cast(o("addr-192.168.1.1")), fw1));
    CPPUNIT_ASSERT(om.dispatch(IPv4::cast(o("addr-192.168.1.1")), fw1_eth1));
    CPPUNIT_ASSERT(om.dispatch(IPv4::cast(o("addr-192.168.1.1")), fw1_eth0) == false);

    om.setIPV6(true);
    CPPUNIT_ASSERT(om.dispatch(IPv6::cast(o("addr-ipv6-1")), fw1));
    CPPUNIT_ASSERT(om.dispatch(IPv6::cast(o("addr-ipv6-2")), fw1) == false);
    CPPUNIT_ASSERT(om.dispatch(IPv4::cast(o("addr-192.168.1.1")), fw1) == false);

    om.setIPV6(false);
    CPPUNIT_ASSERT(om.dispatch(IPv6::cast(o("addr-ipv6-1")), fw1) == false);
    CPPUNIT_ASSERT(om.dispatch(IPv6::cast(o("addr-ipv6-2")), fw1) == false);


    CPPUNIT_ASSERT(om.dispatch(Network::cast(o("net-192.168.1.0")), fw1) == false);
    CPPUNIT_ASSERT(om.dispatch(Network::cast(o("net-192.168.1.0")), fw1_eth1) == false);
    CPPUNIT_ASSERT(om.dispatch(Network::cast(o("net-192.168.1.0")), fw1_eth0) == false);
    CPPUNIT_ASSERT(om.dispatch(Network::cast(o("net-192.168.1.1")), fw1));

    CPPUNIT_ASSERT(om.dispatch(IPv4::cast(o("addr-192.168.1.255")), fw1));
    CPPUNIT_ASSERT(om.dispatch(IPv4::cast(o("addr-192.168.1.0")), fw1));
    CPPUNIT_ASSERT(om.dispatch(Network::cast(o("all multicasts")), fw1));

    om.setRecognizeBroadcasts(false);
    CPPUNIT_ASSERT(om.dispatch(IPv4::cast(o("addr-192.168.1.255")), fw1) == false);
    CPPUNIT_ASSERT(om.dispatch(IPv4::cast(o("addr-192.168.1.0")), fw1) == false);

    // ================================================================
    // AddressRange tests
    // here match_subnets == false  address_range_match_mode = EXACT

    // range1 192.168.1.10 - 192.168.1.20
    // does not match fw1 exactly, but matches when match_subnets == true
    CPPUNIT_ASSERT(om.dispatch(AddressRange::cast(o("range1")), fw1) == false);

    // range2 192.168.2.1-192.168.2.3 does not match fw1:eth2 at all
    CPPUNIT_ASSERT(om.dispatch(AddressRange::cast(o("range2")), fw1) == false);

    // range3 192.168.2.27-192.168.2.50 partially overlaps with fw1:eth2 subnet
    CPPUNIT_ASSERT(om.dispatch(AddressRange::cast(o("range3")), fw1) == false);

    // range4 192.168.2.27-192.168.2.30 is completely inside fw1:eth2 subnet
    CPPUNIT_ASSERT(om.dispatch(AddressRange::cast(o("range4")), fw1) == false);

    // ================================================================
    om.setAddressRangeMatchMode(ObjectMatcher::PARTIAL);
    // here match_subnets == false  address_range_match_mode = PARTIAL
    // when match_subnets == false, ObjectMatcher compares address of interface with
    // the range and ignores netmask of the interface. Address has to be inside the range
    // to match

    // range3 192.168.2.27-192.168.2.50 partially overlaps with fw1:eth2 subnet
    // but address of interface 192.168.2.24 is outside the range
    CPPUNIT_ASSERT(om.dispatch(AddressRange::cast(o("range3")), fw1) == false);

    // range4 192.168.2.27-192.168.2.30 is completely inside fw1:eth2 subnet
    CPPUNIT_ASSERT(om.dispatch(AddressRange::cast(o("range4")), fw1) == false);

    // fw1:eth2:ip 192.168.2.24 falls inside range5
    CPPUNIT_ASSERT(om.dispatch(AddressRange::cast(o("range5")), fw1));

    // ================================================================
    om.setMatchSubnets(true); // ranges will be compared to subnets defined by interface addr/mask
    // here match_subnets == true  address_range_match_mode = PARTIAL

    CPPUNIT_ASSERT(om.dispatch(host2_eth0_ip, fw1));
    CPPUNIT_ASSERT(om.dispatch(AddressRange::cast(o("range1")), fw1));

    // range2 192.168.2.1-192.168.2.3 does not match fw1:eth2 at all
    CPPUNIT_ASSERT(om.dispatch(AddressRange::cast(o("range2")), fw1) == false);

    // range3 192.168.2.27-192.168.2.50 partially overlaps with fw1:eth2 subnet
    CPPUNIT_ASSERT(om.dispatch(AddressRange::cast(o("range3")), fw1));

    // range4 192.168.2.27-192.168.2.30 is completely inside fw1:eth2 subnet
    CPPUNIT_ASSERT(om.dispatch(AddressRange::cast(o("range4")), fw1));

    // range5 192.168.2.21-192.168.2.27 partially overlaps with fw1:eth2 subnet
    CPPUNIT_ASSERT(om.dispatch(AddressRange::cast(o("range5")), fw1));

}
