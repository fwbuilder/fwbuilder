/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#include "UsageResolverTest.h"
#include "UsageResolver.h"
#include "LibraryManipulator.h"

#include <QDebug>
#include <set>

using namespace std;
using namespace libfwbuilder;


void UsageResolverTest::addToLib(FWObject* obj)
{
    LibraryManipulator().getStandardSlotForObject(lib, obj->TYPENAME)->add(obj);
}

void UsageResolverTest::setUp()
{
    db = new FWObjectDatabase();
    db->setName("Database");
    lib = Library::cast(LibraryManipulator().createNewLibrary(db));
    lib->setName("Library");
    Firewall *fw1 = Firewall::cast(db->create(Firewall::TYPENAME));
    Firewall *fw2 = Firewall::cast(db->create(Firewall::TYPENAME));
    Firewall *fw3 = Firewall::cast(db->create(Firewall::TYPENAME));
    fw1->setName("Firewall 1");
    fw2->setName("Firewall 2");
    fw3->setName("Firewall 3");
    addToLib(fw1); addToLib(fw2); addToLib(fw3);

    addr1 = new Address();
    addr2 = new Address();
    addr1->setName("Address 1");
    addr1->setName("Address 2");
    addToLib(addr1); addToLib(addr2);

    Group *grp1 = new Group(),
          *grp2 = new Group();
    grp1->setName("Group 1");
    grp2->setName("Group 2");
    addToLib(grp1); addToLib(grp2);
    grp1->add(addr1);
    grp2->add(grp1);

    Policy *p1 = fw1->getPolicy(), *p2 = fw2->getPolicy(), *p3 = fw3->getPolicy();
    p1->setName("Policy of Firewall 1");
    p2->setName("Policy of Firewall 2");
    p3->setName("Policy of Firewall 3");
    addToLib(p1); addToLib(p2); addToLib(p3);

    r1 = PolicyRule::cast(p1->createRule());
    r1->setName("PolicyRule 1 of Firewall 1");
    addToLib(r1);
    r1->getSrc()->addRef(grp1);

    r2 = PolicyRule::cast(p1->createRule());
    r2->setName("PolicyRule 2 of Firewall 1");
    addToLib(r2);
    r2->getSrc()->addRef(addr1);

    r3 = PolicyRule::cast(p2->createRule());
    r3->setName("PolicyRule 1 of Firewall 2");
    addToLib(r3);
    r3->getSrc()->addRef(grp2);

    r4 = PolicyRule::cast(p3->createRule());
    r4->setName("PolicyRule 1 of Firewall 3");
    addToLib(r4);
    r4->getSrc()->addRef(addr2);

}


void UsageResolverTest::findWhereObjectIsUsed()
{
    qDebug() << "running findWhereObjectIsUsed";
    set<FWObject*> res;
    db->findWhereObjectIsUsed(addr1, db, res);
    set<FWObject*>::iterator iter = res.begin();
    CPPUNIT_ASSERT(res.size() == 2);
    while (iter!=res.end())
    {
        string name = (*iter)->getName();
        qDebug() << "got name '" << name.c_str() << "'";
        CPPUNIT_ASSERT ( name == "Group 1" || name == "Rule 2 of Firewall 1" );
        iter++;
    }
}

void UsageResolverTest::findFirewallsForObject()
{
    qDebug() << "running findFirewallsForObject";
    list<Firewall*> res = UsageResolver::findFirewallsForObject(addr1, db);
    list<Firewall*>::iterator iter = res.begin();
    CPPUNIT_ASSERT(res.size() == 5);
    while (iter!=res.end())
    {
        string name = (*iter++)->getName();
        qDebug() << "got name '" << name.c_str() << "'";
        CPPUNIT_ASSERT ( name == "Group 1" || name == "Group 2" || name == "Rule 1 of Firewall 1"
                         || name == "Rule 2 of Firewall 1" || name == "Rule 1 of Firewall 2" );
    }
}
