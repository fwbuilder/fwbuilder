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
#include "fwbuilder/Address.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Host.h"

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

    CPPUNIT_ASSERT(om.checkComplexMatch(Interface::cast(o("eth0")), o("fw1")));
//    CPPUNIT_ASSERT(om.checkComplexMatch(Interface::cast(o("eth1")), o("fw1"))); // fails
    CPPUNIT_ASSERT(om.checkComplexMatch(Interface::cast(o("eth0")), o("eth0")));

    CPPUNIT_ASSERT(om.checkComplexMatch(IPv6::cast(o("fw1:eth2:ipv6")), o("fw1")));
    CPPUNIT_ASSERT(om.checkComplexMatch(physAddress::cast(o("fw1:eth2:mac")), o("fw1")));

    CPPUNIT_ASSERT(om.checkComplexMatch(Host::cast(o("host1")), o("fw1")));
    CPPUNIT_ASSERT(om.checkComplexMatch(Interface::cast(dbsearch(o("host1"), "eth0")), o("fw1")));

//    CPPUNIT_ASSERT(om.checkComplexMatch(Host::cast(o("host2")), o("fw1")) == false);
//    CPPUNIT_ASSERT(om.checkComplexMatch(Interface::cast(dbsearch(o("host2"), "eth0")), o("fw1")) == false);
//    CPPUNIT_ASSERT(om.checkComplexMatch(Interface::cast(dbsearch(o("host3"), "eth0")), o("fw1")) == false);
//    CPPUNIT_ASSERT(om.checkComplexMatch(Interface::cast(dbsearch(o("host3"), "eth1")), o("fw1")) == false);

    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.1")), o("fw1")));
    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.1")), o("eth1")));
//    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.1")), o("eth0")) == false);

//    CPPUNIT_ASSERT(om.checkComplexMatch(IPv6::cast(o("addr-ipv6-1")), o("fw1")));
    CPPUNIT_ASSERT(om.checkComplexMatch(IPv6::cast(o("addr-ipv6-2")), o("fw1")) == false);

//    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("net-192.168.1.0")), o("fw1")) == false);
//    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("net-192.168.1.0")), o("eth1")) == false);
//    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("net-192.168.1.0")), o("eth0")) == false);
//    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("net-192.168.1.1")), o("fw1"))); //segfault

    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.255")), o("fw1")));
    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.0")), o("fw1")));

    CPPUNIT_ASSERT(om.checkComplexMatch(AddressRange::cast(o("range1")), o("fw1")));
//    CPPUNIT_ASSERT(om.checkComplexMatch(AddressRange::cast(o("range2")), o("fw1")) == false);
//    CPPUNIT_ASSERT(om.checkComplexMatch(AddressRange::cast(o("range3")), o("fw1")) == false);

    om.setRecognizeBroadcasts(false);
    om.setAddressRangeMatchMode(ObjectMatcher::PARTIAL);

//    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.255")), o("fw1")) == false);
//    CPPUNIT_ASSERT(om.checkComplexMatch(IPv4::cast(o("addr-192.168.1.0")), o("fw1")) == false);
    CPPUNIT_ASSERT(om.checkComplexMatch(AddressRange::cast(o("range3")), o("fw1")));

}
