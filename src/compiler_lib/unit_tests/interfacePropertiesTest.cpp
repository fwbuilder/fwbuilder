/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivkiy     a2k0001@gmail.com

  $Id: interfaceProperties.cpp 2043 2009-12-06 01:10:10Z a2k $

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

#include "interfacePropertiesTest.h"

#include <common/init.cpp>

#include <QDebug>

using namespace std;
using namespace libfwbuilder;
using namespace CppUnit;

void interfacePropertiesTest::validateInterface()
{
    string host_OS = "linux24";

    Resources* os_res = Resources::os_res[host_OS];
    string os_family = host_OS;
    if (os_res!=NULL)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    interfaceProperties * int_prop = interfacePropertiesObjectFactory::getInterfacePropertiesObject(os_family);

    CPPUNIT_ASSERT(int_prop != NULL);

    QString err;

    Cluster *cluster1 = Cluster::cast(db->create(Cluster::TYPENAME));
    Interface *parent1 = Interface::cast(db->create(Interface::TYPENAME));
    Interface *iface1 = Interface::cast(db->create(Interface::TYPENAME));

    db->add(cluster1);
    db->add(parent1);
    parent1->add(iface1, false);
    parent1->getOptionsObject()->setStr("type", "eternet");

    iface1->setUnnumbered(false);
    iface1->getOptionsObject()->setStr("type", "ethernet");
    cluster1->setStr("host_OS", host_OS);

    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(cluster1),
                                           dynamic_cast<FWObject*>(iface1), false, err) == true );

    iface1->setUnnumbered(true);
    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(cluster1),
                                           dynamic_cast<FWObject*>(iface1), false, err) == true );

    parent1->getOptionsObject()->setStr("type", "bonding");
    iface1->setUnnumbered(false);
    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(cluster1),
                                           dynamic_cast<FWObject*>(iface1), false, err) == false );

    parent1->getOptionsObject()->setStr("type", "bridge");
    iface1->setUnnumbered(false);
    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(cluster1),
                                           dynamic_cast<FWObject*>(iface1), false, err) == false );


    Firewall fw;
    fw.setStr("host_OS", host_OS);
    db->add(&fw);

    Interface* parent = Interface::cast(db->create(Interface::TYPENAME));
    Interface* iface = Interface::cast(db->create(Interface::TYPENAME));
    Interface* subiface = Interface::cast(db->create(Interface::TYPENAME));

    fw.add(parent);

    init();

    Resources("../../res/resources.xml");

    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(parent),
                                               dynamic_cast<FWObject*>(iface), true, err)
                   == false);

    parent->getOptionsObject()->setStr("type", "bridge");
    iface->getOptionsObject()->setStr("type", "ethernet");

    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(parent),
                                               FWObject::cast(iface), true, err)
                   == true);

    iface->getOptionsObject()->setStr("type", "ethernet");
    iface->add(subiface);
    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(parent),
                                               dynamic_cast<FWObject*>(iface), false, err)
                   == false);
    iface->remove(subiface);

    Cluster *cluster = Cluster::cast(db->create(Cluster::TYPENAME));

    iface->setName("vlan0");
    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(cluster),
                                               dynamic_cast<FWObject*>(iface), false, err)
                   == true);

    iface->setName("vlan34324");
    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(cluster),
                                               dynamic_cast<FWObject*>(iface), false, err)
                   == false);

    parent->setName("vlan");
    parent->getOptionsObject()->setStr("type", "bridge");
    iface->setName("vlan1");
    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(parent),
                                               dynamic_cast<FWObject*>(iface), false, err)
                       == true);

    IPv4 *adr = IPv4::cast(db->create(IPv4::TYPENAME));
    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(adr),
                                               dynamic_cast<FWObject*>(iface), false, err)
                       == false);

    parent->setName("notAVlan");
    parent->getOptionsObject()->setStr("type", "ethernet");
    iface->setName("vlan1");
    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(parent),
                                               dynamic_cast<FWObject*>(iface), false, err)
                       == true);


    iface->setName("eth0");
    CPPUNIT_ASSERT(int_prop->validateInterface(dynamic_cast<FWObject*>(parent),
                                               dynamic_cast<FWObject*>(iface), false, err)
                       == false);

}

void interfacePropertiesTest::isEligibleForCluster()
{
    Resources* os_res = Resources::os_res["linux24"];
    string os_family = "linux24";
    if (os_res!=NULL)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    interfaceProperties * int_prop = interfacePropertiesObjectFactory::getInterfacePropertiesObject(os_family);

    CPPUNIT_ASSERT(int_prop != NULL);

    Interface *parent1 = Interface::cast(db->create(Interface::TYPENAME));
    Interface *iface1 = Interface::cast(db->create(Interface::TYPENAME));
    db->add(parent1);
    db->add(iface1);
    parent1->add(iface1);

    iface1->getOptionsObject()->setStr("type", "ethernet");
    parent1->getOptionsObject()->setStr("type", "ethernet");
    CPPUNIT_ASSERT(int_prop->isEligibleForCluster(iface1) == true);

    iface1->getOptionsObject()->setStr("type", "ethernet");
    parent1->getOptionsObject()->setStr("type", "bridge");
    CPPUNIT_ASSERT(int_prop->isEligibleForCluster(iface1) == false);

    iface1->getOptionsObject()->setStr("type", "bonding");
    parent1->getOptionsObject()->setStr("type", "ethernet");
    CPPUNIT_ASSERT(int_prop->isEligibleForCluster(iface1) == true);

    iface1->getOptionsObject()->setStr("type", "bridge");
    parent1->getOptionsObject()->setStr("type", "ethernet");
    CPPUNIT_ASSERT(int_prop->isEligibleForCluster(iface1) == true);

    iface1->getOptionsObject()->setStr("type", "8021q");
    parent1->getOptionsObject()->setStr("type", "ethernet");
    CPPUNIT_ASSERT(int_prop->isEligibleForCluster(iface1) == true);

    iface1->getOptionsObject()->setStr("type", "ethernet");
    parent1->getOptionsObject()->setStr("type", "bridge");
    CPPUNIT_ASSERT(int_prop->isEligibleForCluster(iface1) == false);

    Firewall *fw = Firewall::cast(db->create(Firewall::TYPENAME));
    fw->setName("iface");
    Interface *iface = Interface::cast(db->create(Interface::TYPENAME));
    iface->setName("iface1");
    Interface *subface = Interface::cast(db->create(Interface::TYPENAME));
    subface->setName("iface");
    fw->add(iface);
    fw->add(subface);
    iface->getOptionsObject()->setStr("type", "bonding");
    iface->add(subface);
    CPPUNIT_ASSERT ( interfaceProperties().isEligibleForCluster(subface) == false );

    iface->getOptionsObject()->setStr("type", "ethernet");
    CPPUNIT_ASSERT ( interfaceProperties().isEligibleForCluster(iface) == false );

}

void interfacePropertiesTest::setUp()
{
    db = new FWObjectDatabase();
}

