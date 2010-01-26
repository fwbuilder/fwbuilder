#include <QDebug>
#include "FWBTreeTest.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/IntervalGroup.h"

using namespace libfwbuilder;
using namespace std;

void FWBTreeTest::isSystem()
{
    FWBTree fwbtree;

    FWObjectDatabase db;
    CPPUNIT_ASSERT(fwbtree.isSystem(&db) == true);

    Library *lib = db.createLibrary(4);
    CPPUNIT_ASSERT(fwbtree.isSystem(lib) == true);
    lib = db.createLibrary(7);
    CPPUNIT_ASSERT(fwbtree.isSystem(lib) == true);
    lib = db.createLibrary(1);
    CPPUNIT_ASSERT(fwbtree.isSystem(lib) == false);

    lib = Library::cast(fwbtree.createNewLibrary(&db));
    FWObject *slot = fwbtree.getStandardSlotForObject(lib, Firewall::TYPENAME);
    CPPUNIT_ASSERT(fwbtree.isSystem(slot) == true);

    Firewall fw;
    CPPUNIT_ASSERT(fwbtree.isSystem(&fw) == false);
}


void FWBTreeTest::validateForInsertion()
{
    QString err;

    FWBTree tree;

    Host host;
    Firewall firewall;
    Interface iface;
    IPv4 ipv4;
    IPv6 ipv6;

    CPPUNIT_ASSERT(tree.validateForInsertion(&host, &iface, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&firewall, &iface, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&iface, &ipv4, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&iface, &ipv6, err) == true);
    firewall.addInterface(&iface);
    CPPUNIT_ASSERT(tree.validateForInsertion(&iface, &ipv4, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&iface, &ipv6, err) == true);

    ObjectGroup grp, grp2;
    Cluster cluster;
    DNSName dnsname;
    AddressRange addrrange;
    AddressTable addrtable;
    CPPUNIT_ASSERT(tree.validateForInsertion(&grp, &host, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&grp, &firewall, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&grp, &cluster, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&grp, &dnsname, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&grp, &addrrange, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&grp, &addrtable, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&grp, &grp2, err) == true);

    ServiceGroup sgrp, sgrp2;
    IPService ip;
    ICMPService icmp;
    TCPService tcp;
    UDPService udp;
    CustomService custom;
    CPPUNIT_ASSERT(tree.validateForInsertion(&sgrp, &ip, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&sgrp, &icmp, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&sgrp, &tcp, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&sgrp, &udp, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&sgrp, &custom, err) == true);
    CPPUNIT_ASSERT(tree.validateForInsertion(&sgrp, &sgrp2, err) == true);

    Interval interval;
    IntervalGroup igrp;
    CPPUNIT_ASSERT(tree.validateForInsertion(&igrp, &interval, err) == true);

    CPPUNIT_ASSERT(tree.validateForInsertion(&host, &ipv4, err) == false);
    CPPUNIT_ASSERT(tree.validateForInsertion(&firewall, &ipv4, err) == false);
    CPPUNIT_ASSERT(tree.validateForInsertion(&cluster, &ipv4, err) == false);
    CPPUNIT_ASSERT(tree.validateForInsertion(&grp, &ip, err) == false);
    CPPUNIT_ASSERT(tree.validateForInsertion(&sgrp, &ipv4, err) == false);
    CPPUNIT_ASSERT(tree.validateForInsertion(&grp, &interval, err) == false);
    CPPUNIT_ASSERT(tree.validateForInsertion(&sgrp, &interval, err) == false);

}
