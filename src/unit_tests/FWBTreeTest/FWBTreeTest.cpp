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
#include "fwbuilder/Network.h"
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

QSet<FWObject*> FWBTreeTest::getStandardFolders(FWObject *root)
{
    QSet<FWObject*> res;
    for (FWObject::iterator i = root->begin(); i != root->end(); i++)
    {
        if (FWBTree().isStandardFolder(*i)) res.insert(*i);
        res += getStandardFolders(*i);
    }
    return res;
}

void FWBTreeTest::validateForInsertion()
{

    FWObjectDatabase db;
    FWBTree().createNewLibrary(&db);
    QSet<FWObject*> stdFolders = getStandardFolders(&db);

    QString err;

    FWBTree tree;

    Host host;
    Firewall firewall;
    Interface iface;
    IPv4 ipv4;
    IPv6 ipv6;
    Firewall fw;
    Network net;

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

    foreach (FWObject* folder, stdFolders)
    {
        if (folder->getName() == "Hosts")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &host, err) == true);
        if (folder->getName() == "Firewalls")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &fw, err) == true);
        if (folder->getName() == "Addresses")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &ipv4, err) == true);
        if (folder->getName() == "Addresses")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &ipv6, err) == true);
        if (folder->getName() == "TCP")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &tcp, err) == true);
        if (folder->getName() == "UDP")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &udp, err) == true);
        if (folder->getName() == "ICMP")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &icmp, err) == true);
        if (folder->getName() == "Address ranges")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &addrrange, err) == true);
        if (folder->getName() == "Networks")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &net, err) == true);
    }

    CPPUNIT_ASSERT(tree.validateForInsertion(&host, &ipv4, err) == false);
    CPPUNIT_ASSERT(tree.validateForInsertion(&firewall, &ipv4, err) == false);
    CPPUNIT_ASSERT(tree.validateForInsertion(&cluster, &ipv4, err) == false);
    CPPUNIT_ASSERT(tree.validateForInsertion(&grp, &ip, err) == false);
    CPPUNIT_ASSERT(tree.validateForInsertion(&sgrp, &ipv4, err) == false);
    CPPUNIT_ASSERT(tree.validateForInsertion(&grp, &interval, err) == false);
    CPPUNIT_ASSERT(tree.validateForInsertion(&sgrp, &interval, err) == false);

    foreach (FWObject* folder, stdFolders)
    {
        if (folder->getName() == "Addresses")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &host, err) == false);
        if (folder->getName() == "Addresses")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &fw, err) == false);
        if (folder->getName() == "Hosts")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &ipv4, err) == false);
        if (folder->getName() == "Firewalls")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &ipv6, err) == false);
        if (folder->getName() == "Networks")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &tcp, err) == false);
        if (folder->getName() == "TCP")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &udp, err) == false);
        if (folder->getName() == "Address ranges")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &icmp, err) == false);
        if (folder->getName() == "ICMP")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &addrrange, err) == false);
        if (folder->getName() == "UDP")
            CPPUNIT_ASSERT(tree.validateForInsertion(folder, &net, err) == false);
    }


    // destructor ~FWObject calls FWObject::destroyChildren() which
    // tries to delete all child objects. Since interface iface was
    // not created using new, it can not be deleted. Remove it to
    // avoid crash. Also need to ref() it so that FWObject::remove()
    // does not try to delete it as well. All these hacks just because
    // a2k@codeminders.com was too lazy to create objects as recommended.
    iface.ref();
    firewall.removeInterface(&iface);
}

