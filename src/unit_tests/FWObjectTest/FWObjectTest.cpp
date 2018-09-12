/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: FWObjectTest.cpp 581 2010-03-10 18:31:03Z a2k $

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

#include "FWObjectTest.h"

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
#include "fwbuilder/Group.h"

using namespace libfwbuilder;
using namespace std;

void FWObjectTest::cmpTest()
{
    FWObjectDatabase db;
    FWObject *obj1 = db.create(Firewall::TYPENAME);
    FWObject *obj2 = db.create(Firewall::TYPENAME);
    obj2->duplicate(obj1);
    obj1->setStr("key", "value1");
    CPPUNIT_ASSERT(obj1->cmp(obj2) == false);
    obj2->setStr("key", "value2");
    CPPUNIT_ASSERT(obj1->cmp(obj2) == false);
    obj2->remStr("key");
    obj1->setStr("key", "");
    CPPUNIT_ASSERT(obj1->cmp(obj2) == false);
    obj1->remStr("key");
    CPPUNIT_ASSERT(obj2->cmp(obj1) == true);


    FWObject *ch1 = db.create(Interface::TYPENAME);
    FWObject *ch2 = db.create(Interface::TYPENAME);
    FWObject *ch3 = db.create(Interface::TYPENAME);
    FWObject *ch4 = db.create(Interface::TYPENAME);

    ch1->setStr("key", "value1");
    ch2->duplicate(ch1);
    CPPUNIT_ASSERT(ch1->cmp(ch2) == true);
    ch3->duplicate(ch1);
    CPPUNIT_ASSERT(ch1->cmp(ch3) == true);
    ch4->duplicate(ch1);
    ch4->setStr("key", "value2");
    CPPUNIT_ASSERT(ch1->cmp(ch4) == false);

    obj1->add(ch1);
    obj1->add(ch2);

    obj2->add(ch3);
    obj2->add(ch4);

    CPPUNIT_ASSERT(obj1->cmp(obj2, true) == false);

    ch4->setStr("key", "value1");

    CPPUNIT_ASSERT(obj1->cmp(obj2, true) == true);
}
