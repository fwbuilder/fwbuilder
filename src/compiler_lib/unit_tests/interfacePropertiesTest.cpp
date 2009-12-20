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
    vector<vector<string> > testData;
    vector<string> row;
    row.push_back("ethernet"); // type
    row.push_back("ethernet"); // parent_type
    row.push_back("false");    // is unnumbered
    row.push_back("true");     // right answer
    testData.push_back(row);

    row.clear();
    row.push_back("ethernet");
    row.push_back("ethernet");
    row.push_back("true");
    row.push_back("true");
    testData.push_back(row);

    row.clear();
    row.push_back("bonding");
    row.push_back("ethernet");
    row.push_back("false");
    row.push_back("false");
    testData.push_back(row);

    row.clear();
    row.push_back("bridge");
    row.push_back("ethernet");
    row.push_back("false");
    row.push_back("false");
    testData.push_back(row);

    string host_OS = "linux24";

    Resources* os_res = Resources::os_res[host_OS];
    string os_family = host_OS;
    if (os_res!=NULL)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    interfaceProperties * int_prop = interfacePropertiesObjectFactory::getInterfacePropertiesObject(os_family);

    QString err;

    for (unsigned int i = 0; i<testData.size(); i++)
    {
        string parent_type = testData[i][0];
        string type = testData[i][1];
        bool unnumbered = testData[i][2] == "true";
        bool answer = testData[i][3] == "true";

        Cluster cluster;
        Interface parent;
        Interface iface;

        db->add(&cluster);
        db->add(&parent);
        parent.add(&iface, false);
        parent.getOptionsObject()->setStr("type", parent_type);

        iface.setUnnumbered(unnumbered);
        iface.getOptionsObject()->setStr("type", type);
        cluster.setStr("host_OS", host_OS);


        CPPUNIT_ASSERT(int_prop != NULL);

        bool result = int_prop->validateInterface(dynamic_cast<FWObject*>(&cluster),
                                               dynamic_cast<FWObject*>(&iface), false, err);
        CPPUNIT_ASSERT(result == answer);
        CPPUNIT_ASSERT(err.isEmpty() == answer);
    }

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
    vector<vector<string> > testData;
    vector<string> row;
    row.push_back("ethernet"); // type
    row.push_back("ethernet"); // parent_type
    row.push_back("true");     // right answer
    testData.push_back(row);

    row.clear();
    row.push_back("ethernet");
    row.push_back("bridge");
    row.push_back("false");
    testData.push_back(row);

    row.clear();
    row.push_back("bonding");
    row.push_back("ethernet");
    row.push_back("true");
    testData.push_back(row);

    row.clear();
    row.push_back("bridge");
    row.push_back("ethernet");
    row.push_back("true");
    testData.push_back(row);

    row.clear();
    row.push_back("8021q");
    row.push_back("ethernet");
    row.push_back("true");
    testData.push_back(row);

    row.clear();
    row.push_back("ethernet");
    row.push_back("bridge");
    row.push_back("false");
    testData.push_back(row);

    Resources* os_res = Resources::os_res["linux24"];
    string os_family = "linux24";
    if (os_res!=NULL)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    interfaceProperties * int_prop = interfacePropertiesObjectFactory::getInterfacePropertiesObject(os_family);

    for (unsigned int i = 0; i<testData.size(); i++)
    {
        string type = testData[i][0];
        string parent_type = testData[i][1];
        bool answer = testData[i][2] == "true";

        Interface parent;
        Interface iface;

        db->add(&parent);
        parent.add(&iface, true);
        parent.getOptionsObject()->setStr("type", parent_type);

        iface.getOptionsObject()->setStr("type", type);
        iface.getParent()->setStr("type", parent_type);

        CPPUNIT_ASSERT(int_prop != NULL);

        bool result = int_prop->isEligibleForCluster(&iface);

        CPPUNIT_ASSERT(result == answer);
    }

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

