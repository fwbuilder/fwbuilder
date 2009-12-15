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

#include <QDebug>
#include <set>

using namespace std;
using namespace libfwbuilder;


void UsageResolverTest::addToLib(FWObject* obj)
{
    FWBTree().getStandardSlotForObject(lib, obj->getTypeName().c_str())->add(obj);
}

void UsageResolverTest::setUp()
{
    db = new FWObjectDatabase();
    db->setName("Database");
    lib = Library::cast(FWBTree().createNewLibrary(db));
    lib->setName("Library");

    FWObject *fw1 = db->create(Firewall::TYPENAME);
    FWObject *fw2 = db->create(Firewall::TYPENAME);
    FWObject *fw3 = db->create(Firewall::TYPENAME);
    fw1->setName("Firewall 1");
    fw2->setName("Firewall 2");
    fw3->setName("Firewall 3");

    addToLib(fw1);
    addToLib(fw2);
    addToLib(fw3);

    addr1 = IPv4::cast(db->create(IPv4::TYPENAME));
    addr2 = IPv4::cast(db->create(IPv4::TYPENAME));
    addr1->setName("Address 1");
    addr2->setName("Address 2");
    addToLib(addr1);
    addToLib(addr2);

    FWObject *grp1 = db->create(ObjectGroup::TYPENAME);
    FWObject *grp2 = db->create(ObjectGroup::TYPENAME);
    FWObject *grp3 = db->create(ObjectGroup::TYPENAME);
    grp1->setName("Group 1");
    grp2->setName("Group 2");
    grp3->setName("Group 3");
    grp1->addRef(addr1);

    addToLib(grp1);
    addToLib(grp2);
    addToLib(grp3);

    // addr1 belongs to grp1
    // addr2 belongs to grp2
    // grp2 belongs to grp1
    grp1->add(addr1);
    grp2->add(addr2);
    grp2->add(grp1);
    grp3->add(grp2);

    Policy *policy = Firewall::cast(fw1)->getPolicy();
    PolicyRule *rule;
    rule = PolicyRule::cast(policy->createRule());
    rule->setName("PolicyRule 1 of Firewall 1");
    rule->getSrc()->addRef(addr1);
    policy->add(rule);

    rule = PolicyRule::cast(policy->createRule());
    rule->setName("PolicyRule 2 of Firewall 1");
    rule->getSrc()->addRef(grp1);
    policy->add(rule);

    rule = PolicyRule::cast(policy->createRule());
    rule->setName("PolicyRule 3 of Firewall 1");
    rule->getSrc()->addRef(grp2);
    policy->add(rule);

    rule = PolicyRule::cast(Firewall::cast(fw2)->getPolicy()->createRule());
    rule->setName("PolicyRule 1 of Firewall 2");
    rule->getSrc()->addRef(addr2);
    Firewall::cast(fw2)->getPolicy()->add(rule);

    rule = PolicyRule::cast(Firewall::cast(fw3)->getPolicy()->createRule());
    rule->setName("PolicyRule 1 of Firewall 3");
    //rule->getSrc()->addRef(addr2);
    rule->getSrc()->addRef(grp3);
    //rule->getSrc()->addRef(grp2);
    Firewall::cast(fw3)->getPolicy()->add(rule);

}


void UsageResolverTest::findWhereObjectIsUsed()
{
//    db->dump(true, true);

    set<FWObject*> res;

    db->findWhereObjectIsUsed(addr1, db, res);
    CPPUNIT_ASSERT(res.size() == 4);

    set<FWObject*>::iterator iter = res.begin();
    while (iter!=res.end())
    {
        FWObject *obj = *iter;

        string name = obj->getName();

        if (FWReference::cast(obj))
        {
            // if we get reference, the parent must be rule element
            obj = obj->getParent();
            CPPUNIT_ASSERT(obj->getTypeName() == RuleElementSrc::TYPENAME || obj->getTypeName() == ObjectGroup::TYPENAME);
            CPPUNIT_ASSERT(obj->getParent()->getName() == "PolicyRule 1 of Firewall 1" || obj->getParent()->getName() == "Group 2");
        } else
        {
            // otherwise we should get the group grp1 or
            // system folder "Addresses"
            CPPUNIT_ASSERT(name == "Group 1" ||
                           name == "Addresses" );
        }
        iter++;
    }

    qDebug() << "test FWObjectDatabase::findWhereObjectIsUsed done";
    qDebug() << "";
}

void UsageResolverTest::findFirewallsForObject()
{
    list<Firewall*> res = UsageResolver::findFirewallsForObject(addr1, db);
    list<Firewall*>::iterator iter = res.begin();
    CPPUNIT_ASSERT(res.size() == 2);
    while (iter!=res.end())
    {
        string name = (*iter++)->getName();
        //qDebug() << "got object: '" << name.c_str()
        //         << "' (" << (*iter)->getTypeName().c_str() << ")";
        CPPUNIT_ASSERT ( name == "Firewall 1" || name == "Firewall 3" );
    }

    res = UsageResolver::findFirewallsForObject(addr2, db);
    iter = res.begin();
    CPPUNIT_ASSERT(res.size() == 3);
    while (iter!=res.end())
    {
        string name = (*iter++)->getName();
        //qDebug() << "got object: '" << name.c_str();
        //         << "' (" << (*iter)->getTypeName().c_str() << ")";
        CPPUNIT_ASSERT ( name == "Firewall 1" || name == "Firewall 2" || name == "Firewall 3" );
    }
}

void UsageResolverTest::humanizeSearchResults()
{
    set<FWObject*> res;
    db->findWhereObjectIsUsed(addr1, db, res);
    UsageResolver::humanizeSearchResults(res);
    set<FWObject*>::iterator iter = res.begin();
    while (iter!=res.end())
    {
        string name = (*iter++)->getName();
        CPPUNIT_ASSERT ( name == "Group 1" || name == "Addresses" || (*iter)->getTypeName() == "ObjectGroup" );
    }
}
