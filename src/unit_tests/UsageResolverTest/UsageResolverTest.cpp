/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#include "UsageResolverTest.h"
#include "UsageResolver.h"

#include <QTest>
#include <QDebug>
#include <set>

using namespace std;
using namespace libfwbuilder;


void UsageResolverTest::addToLib(FWObject* obj)
{
    FWBTree().getStandardSlotForObject(lib, obj->getTypeName().c_str())->add(obj);
}

void UsageResolverTest::init()
{
    db = new FWObjectDatabase();
    db->setName("Database");
    lib = Library::cast(FWBTree().createNewLibrary(db));
    lib->setName("Library");

    Firewall *fw1 = Firewall::cast(db->create(Firewall::TYPENAME));
    Firewall *fw2 = Firewall::cast(db->create(Firewall::TYPENAME));
    Firewall *fw3 = Firewall::cast(db->create(Firewall::TYPENAME));
    Firewall *fw4 = Firewall::cast(db->create(Firewall::TYPENAME));
    fw1->setName("Firewall 1");
    fw2->setName("Firewall 2");
    fw3->setName("Firewall 3");
    fw4->setName("Firewall 4");

    addToLib(fw1);
    addToLib(fw2);
    addToLib(fw3);
    addToLib(fw4);

    addr1 = IPv4::cast(db->create(IPv4::TYPENAME));
    addr2 = IPv4::cast(db->create(IPv4::TYPENAME));
    addr3 = IPv4::cast(db->create(IPv4::TYPENAME));
    addr1->setName("Address 1");
    addr2->setName("Address 2");
    addr3->setName("Address 3");
    addToLib(addr1);
    addToLib(addr2);
    addToLib(addr3);

    FWObject *grp1 = db->create(ObjectGroup::TYPENAME);
    FWObject *grp2 = db->create(ObjectGroup::TYPENAME);
    FWObject *grp3 = db->create(ObjectGroup::TYPENAME);
    grp1->setName("Group 1");
    grp2->setName("Group 2");
    grp3->setName("Group 3");

    addToLib(grp1);
    addToLib(grp2);
    addToLib(grp3);

    // addr1 belongs to grp1
    // addr2 belongs to grp2
    // grp1 belongs to grp2
    grp1->addRef(addr1);
    grp2->addRef(addr2);
    grp2->addRef(grp1);
    grp3->addRef(grp2);

    Policy *policy = fw1->getPolicy();
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

/*
 * Branches:
 *
 * fw4 rule1 --> fw3 Policy
 * fw3 rule2 --> fw2 Policy
 * fw3 rule3 --> fw4 Policy  (circular)
 *
 * object addr3 is used in rule 1 of fw2.
 */

    rule = PolicyRule::cast(fw2->getPolicy()->createRule());
    rule->setName("PolicyRule 1 of Firewall 2");
    rule->getSrc()->addRef(addr2);
    rule->getDst()->addRef(addr3);
    fw2->getPolicy()->add(rule);

    rule = PolicyRule::cast(fw3->getPolicy()->createRule());
    rule->setName("PolicyRule 1 of Firewall 3");
    rule->getSrc()->addRef(grp3);
    fw3->getPolicy()->add(rule);

    rule = PolicyRule::cast(fw3->getPolicy()->createRule());
    rule->setName("PolicyRule 2 of Firewall 3");
    rule->setAction(PolicyRule::Branch);
    rule->setBranch(fw2->getPolicy());
    fw3->getPolicy()->add(rule);

    rule = PolicyRule::cast(fw4->getPolicy()->createRule());
    rule->setName("PolicyRule 1 of Firewall 4");
    rule->setAction(PolicyRule::Branch);
    rule->setBranch(fw3->getPolicy());
    fw4->getPolicy()->add(rule);

    rule = PolicyRule::cast(fw3->getPolicy()->createRule());
    rule->setName("PolicyRule 3 of Firewall 3");
    rule->setAction(PolicyRule::Branch);
    rule->setBranch(fw4->getPolicy());
    fw3->getPolicy()->add(rule);

}

/*
 * addr1 is found in:
 *  - system group "Addresses"
 *  - group grp1
 *  - rule 1 of firewall 1
 */
void UsageResolverTest::findWhereObjectIsUsed()
{
//    db->dump(true, true);

    set<FWObject*> res;

    db->findWhereObjectIsUsed(addr1, db, res);
    QVERIFY(res.size() == 3);

    set<FWObject*>::iterator iter = res.begin();
    while (iter!=res.end())
    {
        FWObject *obj = *iter;

        string name = obj->getName();

        if (FWReference::cast(obj))
        {
            // if we get reference, the parent must be rule element or user group
            obj = obj->getParent();

            QVERIFY(
                obj->getTypeName() == RuleElementSrc::TYPENAME ||
                obj->getTypeName() == ObjectGroup::TYPENAME);

            if (RuleElementSrc::isA(obj))
            {
                QVERIFY(obj->getParent()->getName() == "PolicyRule 1 of Firewall 1");
            }

            if (ObjectGroup::isA(obj))
            {
                QVERIFY(obj->getName() == "Group 1");
            }

        } else
        {
            // otherwise we should get system folder "Addresses"
            QVERIFY(name == "Addresses" );
        }
        iter++;
    }

    qDebug() << "test FWObjectDatabase::findWhereObjectIsUsed done";
    qDebug() << "";
}

void UsageResolverTest::findFirewallsForObject()
{
    qDebug() << "Dependencies for addr1:";
    list<Firewall*> res = UsageResolver().findFirewallsForObject(addr1, db);
    list<Firewall*>::iterator iter = res.begin();
    QVERIFY(res.size() == 3);
    while (iter!=res.end())
    {
        string name = (*iter)->getName();
        qDebug() << name.c_str()
                 << "' (" << (*iter)->getTypeName().c_str() << ")";
        QVERIFY ( name == "Firewall 1" || name == "Firewall 3" || name == "Firewall 4" );
        iter++;
    }

    qDebug() << "Dependencies for addr2:";
    res = UsageResolver().findFirewallsForObject(addr2, db);
    iter = res.begin();
    QVERIFY(res.size() == 4);
    while (iter!=res.end())
    {
        string name = (*iter)->getName();
        qDebug() << name.c_str()
                 << "' (" << (*iter)->getTypeName().c_str() << ")";
        QVERIFY ( name == "Firewall 1" || name == "Firewall 2" || name == "Firewall 3"  || name == "Firewall 4");
        iter++;
    }

    //fwbdebug = 1;
    qDebug() << "Dependencies for addr3:";
    res = UsageResolver().findFirewallsForObject(addr3, db);
    iter = res.begin();
    //QVERIFY(res.size() == 3);
    while (iter!=res.end())
    {
        string name = (*iter)->getName();
        qDebug() << name.c_str()
                 << "' (" << (*iter)->getTypeName().c_str() << ")";
        QVERIFY ( name == "Firewall 2" || name == "Firewall 3" || name == "Firewall 4" );
        iter++;
    }
}

void UsageResolverTest::humanizeSearchResults()
{
    set<FWObject*> res;
    db->findWhereObjectIsUsed(addr1, db, res);
    UsageResolver().humanizeSearchResults(res);
    set<FWObject*>::iterator iter = res.begin();
    while (iter!=res.end())
    {
        string name = (*iter)->getName();
        string type = (*iter)->getTypeName();
        if (type == "ObjectRef")
        {
            FWObjectReference *o = FWObjectReference::cast(*iter);
            QVERIFY(o->getPointer()->getName() == "Address 1");
        }
        else
        {
           QVERIFY ( name == "Group 1" || name == "Addresses");
        }
        iter++;
    }
}
