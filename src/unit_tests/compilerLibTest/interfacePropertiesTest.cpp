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

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Constants.h"

#include <QStringList>
#include <QDebug>


using namespace std;
using namespace libfwbuilder;
using namespace CppUnit;


class UpgradePredicate: public XMLTools::UpgradePredicate
{
    public:
    virtual bool operator()(const string&) const 
    { 
	cout << "Data file has been created in the old version of Firewall Builder. Use fwbuilder GUI to convert it." << std::endl;
	return false;
    }
};


interfaceProperties* interfacePropertiesTest::getIntProps(const QString &os)
{
    Resources* os_res = Resources::os_res[os.toStdString()];
    string os_family = os.toStdString();
    if (os_res!=nullptr)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");
    return interfacePropertiesObjectFactory::getInterfacePropertiesObject(os_family);
}

void interfacePropertiesTest::setUp()
{
    db = new FWObjectDatabase();
}

void interfacePropertiesTest::validateInterfaceNameCommon()
{
    QString err;

    Firewall fw;
    fw.setStr("host_OS", "unknown");
    db->add(&fw);

    Interface* iface = Interface::cast(db->create(Interface::TYPENAME));

    fw.add(iface);

    iface->getOptionsObject()->setStr("type", "ethernet");

    interfaceProperties * int_prop = getIntProps("unknown");

    CPPUNIT_ASSERT(int_prop != nullptr);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "eth0", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "foo0", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "bar0", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "vlan100", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "Vlan100", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "VlAn100", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "foo 0", err) == false);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "foo-1", err) == false);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "foo 12345", err) == false);

    iface->getOptionsObject()->setStr("type", "bridge");

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "br0", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "Br0", err) == true);

    // all OS except Linux and possibly some other do not permit
    // interface name with "-"
    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "br-lan", err) == false);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "br 200", err) == false);

}

void interfacePropertiesTest::validateInterfaceNameLinux()
{
    QString err;

    Firewall fw;
    fw.setStr("host_OS", "linux24");
    db->add(&fw);

    Interface* iface = Interface::cast(db->create(Interface::TYPENAME));

    fw.add(iface);

    iface->getOptionsObject()->setStr("type", "ethernet");

    interfaceProperties * int_prop = getIntProps("linux24");

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "eth0", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "foo0", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "bar0", err) == true);
    
    // we do not have special type for p2p interfaces yet
    // Linux permits "-" in interface names (see #1856)
    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "ppp-dsl", err) == true);

    iface->getOptionsObject()->setStr("type", "8021q");

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "vlan100", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "Vlan100", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "VlAn100", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "foo 0", err) == false);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "foo 12345", err) == false);

    iface->getOptionsObject()->setStr("type", "bridge");

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "br0", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "Br0", err) == true);

    // Linux permits "-" in interface names (see #1856)
    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "br-lan", err) == true);

    // spaces are not permitted
    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "br 200", err) == false);

}

void interfacePropertiesTest::validateInterfaceNameProCurve()
{
    QString err;

    Firewall fw;
    fw.setStr("host_OS", "procurve");
    db->add(&fw);

    Interface* iface = Interface::cast(db->create(Interface::TYPENAME));

    fw.add(iface);

    iface->getOptionsObject()->setStr("type", "ethernet");

    interfaceProperties * int_prop = getIntProps("procurve");

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "a1", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "b1", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "A1", err) == true);

    // basicValidateInterfaceName() only checks name format,
    // it does not check if port number makes sense
    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "A1234567890", err) == true);

    // "-" is not permitted
    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "vlan-100", err) == false);

    // but space is permitted
    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "vlan 100", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "Vlan 100", err) == true);

    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "VlAn 100", err) == true);

    // basicValidateInterfaceName() only checks name format,
    // it does not check if vlan ID is valid.
    CPPUNIT_ASSERT(int_prop->basicValidateInterfaceName(
                       iface, "VlAn 1000000000", err) == true);
}

void interfacePropertiesTest::validateInterface()
{
    string host_OS = "linux24";

    Resources* os_res = Resources::os_res[host_OS];
    string os_family = host_OS;
    if (os_res!=nullptr)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    interfaceProperties * int_prop = interfacePropertiesObjectFactory::getInterfacePropertiesObject(os_family);

    CPPUNIT_ASSERT(int_prop != nullptr);

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
    if (os_res!=nullptr)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    interfaceProperties * int_prop = interfacePropertiesObjectFactory::getInterfacePropertiesObject(os_family);

    CPPUNIT_ASSERT(int_prop != nullptr);

    Firewall *fw1 = Firewall::cast(db->create(Firewall::TYPENAME));
    fw1->setName("iface");
    fw1->setStr("host_OS", "unknown");
    db->add(fw1);

    Interface *parent1 = Interface::cast(db->create(Interface::TYPENAME));
    Interface *iface1 = Interface::cast(db->create(Interface::TYPENAME));

    fw1->add(parent1);
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
    iface->getOptionsObject()->setStr("type", "bonding");
    iface->add(subface);
    CPPUNIT_ASSERT ( interfaceProperties().isEligibleForCluster(subface) == false );

    iface->getOptionsObject()->setStr("type", "ethernet");
    CPPUNIT_ASSERT ( interfaceProperties().isEligibleForCluster(iface) == false );

}

void interfacePropertiesTest::isValidVlanInterfaceNameLinuxFamilies(
    interfaceProperties * int_prop)
{
    QString err, parent = "eth0";
/*
sub              parent  result
vlan101 	 eth0	 true
eth0.101	 eth0	 true
foo 	         eth0	 false
foo101	         eth0	 false
eth0.bar	 eth0	 false
eth0.99999	 eth0	 false
eth1.101	 eth0	 false
*/
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("vlan101", parent, err) == true);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("eth0.101", parent, err) == true);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("foo", parent, err) == false);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("foo101", parent, err) == false);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("eth0.bar", parent, err) == false);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("eth0.99999", parent, err) == false);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("eth1.101", parent, err) == false);

    parent = "bond1";
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("bond1.15", parent, err) == true);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("bond1.515", parent, err) == true);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("bond1.1205", parent, err) == true);
}



void interfacePropertiesTest::isValidVlanInterfaceNameLinux24()
{
    isValidVlanInterfaceNameLinuxFamilies(getIntProps("linux24"));
}

void interfacePropertiesTest::isValidVlanInterfaceNameOpenWRT()
{
    isValidVlanInterfaceNameLinuxFamilies(getIntProps("openwrt"));
}

void interfacePropertiesTest::isValidVlanInterfaceNameDDWRT1()
{
    isValidVlanInterfaceNameLinuxFamilies(getIntProps("dd-wrt-nvram"));
}

void interfacePropertiesTest::isValidVlanInterfaceNameDDWRT2()
{
    isValidVlanInterfaceNameLinuxFamilies(getIntProps("dd-wrt-jffs"));
}

void interfacePropertiesTest::isValidVlanInterfaceNameSecuwall()
{
    isValidVlanInterfaceNameLinuxFamilies(getIntProps("secuwall"));
}

void interfacePropertiesTest::isValidVlanInterfaceNameBSD()
{
    QString err, parent = "en0";
    interfaceProperties *int_prop = getIntProps("openbsd");
/*
vlan101	 en0	 true
en0.101	 en0	 false
foo	 en0	 false
foo101	 en0	 false
*/
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("vlan101", parent, err) == true);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("eth0.101", parent, err) == false);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("foo", parent, err) == false);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("foo101", parent, err) == false);


    int_prop = getIntProps("freebsd");
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("vlan101", parent, err) == true);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("eth0.101", parent, err) == false);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("foo", parent, err) == false);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("foo101", parent, err) == false);
}

void interfacePropertiesTest::isValidVlanInterfaceNameIOS()
{
    QString err, parent;

/*
FastEthernet0/1.101	 FastEthernet0/1	 true
vlan101                  FastEthernet0/1	 false
Ethernet0/0.101          FastEthernet0/1	 false
Ethernet0/0.99999	 Ethernet0/0             false
*/
    interfaceProperties *int_prop = getIntProps("ios");
    parent = "FastEthernet0/1";
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("FastEthernet0/1.101", parent, err) == true);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("vlan101", parent, err) == false);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("Ethernet0/0.101", parent, err) == false);
    parent = "Ethernet0/0";
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("Ethernet0/0.99999", parent, err) == false);
}

void interfacePropertiesTest::isValidVlanInterfaceNamePIX()
{
    QString err, parent;
    interfaceProperties *int_prop = getIntProps("pix_os");
    parent = "FastEthernet0/1";
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("FastEthernet0/1.101", parent, err) == true);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("vlan101", parent, err) == false);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("Ethernet0/0.101", parent, err) == false);
    parent = "Ethernet0/0";
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("Ethernet0/0.99999", parent, err) == false);
}

void interfacePropertiesTest::isValidVlanInterfaceNameProCurve()
{
    QString err, parent;

/*
 * As of 05/10/2010 we do not restrict interfaces for ProCurve
 * 
  Vlan interface name    parent                  ok/not ok
  vlan 2                 anything               true
  vlan2                  anything               false
  Ethernet0/0.101        FastEthernet0/1	 false
  Ethernet0/0.99999	 Ethernet0/0             false
*/
    interfaceProperties *int_prop = getIntProps("procurve");
    parent = "FastEthernet0/1";
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("vlan 2", parent, err) == true);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("VLAN 2", parent, err) == true);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("Vlan 2", parent, err) == true);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("vlan2", parent, err) == false);

    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("vlan 101", parent, err) == true);
    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("vlan101", parent, err) == false);

    CPPUNIT_ASSERT (int_prop->isValidVlanInterfaceName("Ethernet0/0.101", parent, err) == false);
}

void interfacePropertiesTest::validateInterfaceProCurve()
{
    string host_OS = "procurve";

    Resources* os_res = Resources::os_res[host_OS];
    string os_family = host_OS;
    if (os_res!=nullptr)
        os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

    interfaceProperties * int_prop = interfacePropertiesObjectFactory::getInterfacePropertiesObject(os_family);

    CPPUNIT_ASSERT(int_prop != nullptr);

    QString err;

    Firewall fw;
    fw.setStr("host_OS", host_OS);
    db->add(&fw);

    Interface* parent = Interface::cast(db->create(Interface::TYPENAME));
    Interface* iface = Interface::cast(db->create(Interface::TYPENAME));
    Interface* subiface = Interface::cast(db->create(Interface::TYPENAME));

    fw.add(parent);

    iface->setName("vlan 2");
    CPPUNIT_ASSERT(int_prop->validateInterface(&fw, iface, false, err) == true);

    iface->setName("vlan 34324");
    CPPUNIT_ASSERT(int_prop->validateInterface(&fw, iface, false, err) == false);
}

void interfacePropertiesTest::testManageIpAddresses()
{
    UpgradePredicate upgrade_predicate; 

    string file_name = "test.fwb";
    db->setReadOnly( false );
    db->load(file_name, &upgrade_predicate, Constants::getDTDDirectory());
    db->setFileName(file_name);
    db->reIndex();

    FWObject *fw = db->findObjectByName(Firewall::TYPENAME, "fw1");
    CPPUNIT_ASSERT(fw != nullptr);

    Interface *intf = Interface::cast(
        fw->findObjectByName(Interface::TYPENAME, "eth0"));

    interfaceProperties *int_prop = getIntProps("linux24");

    QStringList update_addresses;
    QStringList ignore_addresses;

    CPPUNIT_ASSERT(
        int_prop->manageIpAddresses(
            intf, update_addresses, ignore_addresses) == true);

    intf = Interface::cast(
        fw->findObjectByName(Interface::TYPENAME, "lo"));

    CPPUNIT_ASSERT(
        int_prop->manageIpAddresses(
            intf, update_addresses, ignore_addresses) == true);

    intf = Interface::cast(
        fw->findObjectByName(Interface::TYPENAME, "eth1")); // dyn

    CPPUNIT_ASSERT(
        int_prop->manageIpAddresses(
            intf, update_addresses, ignore_addresses) == false);

    intf = Interface::cast(
        fw->findObjectByName(Interface::TYPENAME, "eth2")); // bridge port

    CPPUNIT_ASSERT(
        int_prop->manageIpAddresses(
            intf, update_addresses, ignore_addresses) == false);

    intf = Interface::cast(
        fw->findObjectByName(Interface::TYPENAME, "eth3")); // bonding intf slave

    CPPUNIT_ASSERT(
        int_prop->manageIpAddresses(
            intf, update_addresses, ignore_addresses) == false);


    intf = Interface::cast(
        fw->findObjectByName(Interface::TYPENAME, "tun0")); // unnumbered

    CPPUNIT_ASSERT(
        int_prop->manageIpAddresses(
            intf, update_addresses, ignore_addresses) == false);

    intf = Interface::cast(
        fw->findObjectByName(Interface::TYPENAME, "tun*")); // unnumbered

    CPPUNIT_ASSERT(
        int_prop->manageIpAddresses(
            intf, update_addresses, ignore_addresses) == false);

}

void interfacePropertiesTest::testManageIpAddressesCluster()
{
    UpgradePredicate upgrade_predicate; 

    string file_name = "test.fwb";
    db->setReadOnly( false );
    db->load(file_name, &upgrade_predicate, Constants::getDTDDirectory());
    db->setFileName(file_name);
    db->reIndex();

    FWObject *fw = db->findObjectByName(Cluster::TYPENAME, "cluster1");
    CPPUNIT_ASSERT(fw != nullptr);

    Interface *intf = Interface::cast(
        fw->findObjectByName(Interface::TYPENAME, "lo"));

    intf->getOptionsObject()->setBool("cluster_interface", true);

    interfaceProperties *int_prop = getIntProps("linux24");

    QStringList update_addresses;
    QStringList ignore_addresses;

    CPPUNIT_ASSERT(
        int_prop->manageIpAddresses(
            intf, update_addresses, ignore_addresses) == false);

    intf = Interface::cast(
        fw->findObjectByName(Interface::TYPENAME, "eth0"));

    CPPUNIT_ASSERT(
        int_prop->manageIpAddresses(
            intf, update_addresses, ignore_addresses) == true);

}
