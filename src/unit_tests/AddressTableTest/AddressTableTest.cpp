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

#include "AddressTableTest.h"


#include "fwbuilder/Resources.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Group.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Network.h"


using namespace std;
using namespace libfwbuilder;





void AddressTableTest::setUp()
{
    objdb = new FWObjectDatabase();

    FWObject *nlib = objdb->create(Library::TYPENAME,true);
    objdb->add(nlib);
    nlib->setName( "Library" );

    FWObject *o1 = objdb->create(ObjectGroup::TYPENAME,true);
    o1->setName("Objects");
    nlib->add(o1);

    address_tables_group = objdb->create(ObjectGroup::TYPENAME,true);
    address_tables_group->setName("Address Tables");
    o1->add(address_tables_group);
}

void AddressTableTest::positiveTest()
{
    setStrings addrres;
    setStrings addrset;

    // This matches contents of the test file addresstable-1.txt
    addrset.insert("216.193.197.238/255.255.255.255");
    addrset.insert("207.46.20.60/255.255.255.0");
    addrset.insert("207.46.198.3/255.255.255.255");
    addrset.insert("207.46.198.60/255.255.255.255");
    addrset.insert("207.46.199.30/255.255.255.255");
    addrset.insert("207.46.225.60/255.255.255.252");
    addrset.insert("207.46.19.60/255.255.255.255");
    addrset.insert("192.168.105.57/255.255.255.255");
    addrset.insert("192.168.105.69/255.255.255.255");
    addrset.insert("192.168.105.68/255.255.255.255");
    addrset.insert("192.168.100.0/255.255.255.0");
    addrset.insert("192.168.11.0/255.255.255.0");

    CPPUNIT_ASSERT(address_tables_group!=nullptr);


    AddressTable *nobj = AddressTable::cast(objdb->create(AddressTable::TYPENAME, true));
    address_tables_group->add(nobj);
    nobj->setName("TestADT");
    nobj->setSourceName("addresstable-1.txt");
    nobj->loadFromSource(false, nullptr, true);

    list<FWObject*>::const_iterator t = nobj->begin();
    Network *net;
    FWReference *ref;

    for ( ; t != nobj->end(); ++t )
    {
        ref = FWReference::cast(*t);
        CPPUNIT_ASSERT(ref!=nullptr);
        net = Network::cast(ref->getPointer());
        CPPUNIT_ASSERT(net!=nullptr);
        addrres.insert(net->getAddressPtr()->toString() + "/" + net->getNetmaskPtr()->toString());
    }

    CPPUNIT_ASSERT(addrset==addrres);
}


void AddressTableTest::negativeTest1()
{
    setStrings addrres;

    CPPUNIT_ASSERT(address_tables_group!=nullptr);


    AddressTable *nobj = AddressTable::cast(objdb->create(AddressTable::TYPENAME, true));
    address_tables_group->add(nobj);
    nobj->setName("TestADT2");
    nobj->setSourceName("addresstable-2.txt");
    CPPUNIT_ASSERT_THROW(nobj->loadFromSource(false, nullptr, true), FWException);
}

void AddressTableTest::negativeTest2()
{
    setStrings addrres;

    CPPUNIT_ASSERT(address_tables_group!=nullptr);


    AddressTable *nobj = AddressTable::cast(objdb->create(AddressTable::TYPENAME, true));
    address_tables_group->add(nobj);
    nobj->setName("TestADT3");
    nobj->setSourceName("addresstable-not-found.txt");
    CPPUNIT_ASSERT_THROW(nobj->loadFromSource(false, nullptr, true), FWException);
}

