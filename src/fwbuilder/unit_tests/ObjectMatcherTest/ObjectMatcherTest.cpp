/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivkiy     a2k0001@gmail.com

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
#include "fwbuilder/libfwbuilder-config.h"
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
        if (chs != NULL) return chs;
    }
    return NULL;
}

void ObjectMatcherTest::matchTest()
{
    FWObjectDatabase *db = new FWObjectDatabase();

    XMLTools::UpgradePredicate up;
    db->load(string("test_data.fwb"), &up, string(LIBFWBUILDER_TEMPLATE_DIR));

    ObjectMatcher om;
    om.setRecognizeBroadcasts(true);
    om.setRecognizeMulticasts(true);
    om.setIPV6(false);
    om.setMatchSubnets(true);
    om.setAddressRangeMatchMode(ObjectMatcher::EXACT);

    Firewall *fw1 = Firewall::cast(dbsearch(db, "fw1"));
    CPPUNIT_ASSERT(fw1 != NULL);

    Interface *fw1_eth0 = Interface::cast(dbsearch(fw1, "eth0"));
    CPPUNIT_ASSERT(fw1_eth0 != NULL);

    Interface *fw1_eth1 = Interface::cast(dbsearch(fw1, "eth1"));
    CPPUNIT_ASSERT(fw1_eth1 != NULL);

    Interface *fw1_eth2 = Interface::cast(dbsearch(fw1, "eth2"));
    CPPUNIT_ASSERT(fw1_eth2 != NULL);

    IPv6 *fw1_eth2_ipv6 = IPv6::cast(dbsearch(fw1_eth2, "fw1:eth2:ipv6"));
    CPPUNIT_ASSERT(fw1_eth2_ipv6 != NULL);

    physAddress *fw1_eth2_mac = physAddress::cast(dbsearch(fw1_eth2, "fw1:eth2:mac"));
    CPPUNIT_ASSERT(fw1_eth2_mac != NULL);

    Host *host1 = Host::cast(dbsearch(db, "host1"));
    CPPUNIT_ASSERT(host1 != NULL);

    Host *host2 = Host::cast(dbsearch(db, "host2"));
    CPPUNIT_ASSERT(host2 != NULL);

    Host *host3 = Host::cast(dbsearch(db, "host3"));
    CPPUNIT_ASSERT(host3 != NULL);

    Interface *host1_eth0 = Interface::cast(dbsearch(host1, "eth0"));
    CPPUNIT_ASSERT(host1_eth0 != NULL);

    Interface *host2_eth0 = Interface::cast(dbsearch(host2, "eth0"));
    CPPUNIT_ASSERT(host2_eth0 != NULL);

    Interface *host3_eth0 = Interface::cast(dbsearch(host3, "eth0"));
    CPPUNIT_ASSERT(host3_eth0 != NULL);

    Interface *host3_eth1 = Interface::cast(dbsearch(host3, "eth1"));
    CPPUNIT_ASSERT(host3_eth1 != NULL);


    CPPUNIT_ASSERT(om.checkComplexMatch(fw1, fw1));
    CPPUNIT_ASSERT(om.checkComplexMatch(fw1_eth0, fw1));
    CPPUNIT_ASSERT(om.checkComplexMatch(fw1_eth1, fw1));
    CPPUNIT_ASSERT(om.checkComplexMatch(fw1_eth0, fw1_eth0));

    CPPUNIT_ASSERT(om.checkComplexMatch(fw1_eth2_ipv6, fw1));
    CPPUNIT_ASSERT(om.checkComplexMatch(fw1_eth2_mac, fw1));

    CPPUNIT_ASSERT(om.checkComplexMatch(host1, fw1));
    CPPUNIT_ASSERT(om.checkComplexMatch(Interface::cast(dbsearch(host1, "eth0")), fw1));

//    CPPUNIT_ASSERT(om.checkComplexMatch(host2, fw1) == false);

//    CPPUNIT_ASSERT(om.checkComplexMatch(host2_eth0, fw1) == false);

    CPPUNIT_ASSERT(om.checkComplexMatch(host3_eth0, fw1) == false);
    CPPUNIT_ASSERT(om.checkComplexMatch(host3_eth1, fw1) == false);

    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.1")), fw1));
    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.1")), fw1_eth1));
    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.1")), fw1_eth0) == false);

//    CPPUNIT_ASSERT(om.checkComplexMatch(IPv6::cast(o("addr-ipv6-1")), fw1));

    CPPUNIT_ASSERT(om.checkComplexMatch(IPv6::cast(o("addr-ipv6-2")), fw1) == false);

    CPPUNIT_ASSERT(om.checkComplexMatch(Network::cast(o("net-192.168.1.0")), fw1) == false);
    CPPUNIT_ASSERT(om.checkComplexMatch(Network::cast(o("net-192.168.1.0")), fw1_eth1) == false);
    CPPUNIT_ASSERT(om.checkComplexMatch(Network::cast(o("net-192.168.1.0")), fw1_eth0) == false);
    CPPUNIT_ASSERT(om.checkComplexMatch(Network::cast(o("net-192.168.1.1")), fw1));

    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.255")), fw1));
    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.0")), fw1));
    CPPUNIT_ASSERT(om.checkComplexMatch(Network::cast(o("all multicasts")), fw1));

    CPPUNIT_ASSERT(om.checkComplexMatch(AddressRange::cast(o("range1")), fw1));
    CPPUNIT_ASSERT(om.checkComplexMatch(AddressRange::cast(o("range2")), fw1) == false);
    CPPUNIT_ASSERT(om.checkComplexMatch(AddressRange::cast(o("range3")), fw1) == false);

    om.setRecognizeBroadcasts(false);
    om.setAddressRangeMatchMode(ObjectMatcher::PARTIAL);

    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.255")), fw1));

//    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.0")), fw1) == false);

    CPPUNIT_ASSERT(om.checkComplexMatch(AddressRange::cast(o("range3")), fw1));

}
