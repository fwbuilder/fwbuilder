#include "FWObjectDatabaseTest.h"
#include "UsageResolver.h"

#include <QDebug>
#include <set>

using namespace std;
using namespace libfwbuilder;

FWObjectDatabaseTest::FWObjectDatabaseTest()
{
}

void FWObjectDatabaseTest::setUp()
{
    db = new FWObjectDatabase();
    Firewall *fw1 = new Firewall(),
             *fw2 = new Firewall(),
             *fw3 = new Firewall();
    fw1->setName("Firewall 1");
    fw2->setName("Firewall 2");
    fw3->setName("Firewall 3");
    db->add(fw1);
    db->add(fw2);
    db->add(fw3);
    Group *grp1 = new Group(),
          *grp2 = new Group();
    grp1->setName("Group 1");
    grp2->setName("Group 2");
    addr1 = new Address();
    addr2 = new Address();
    addr1->setName("Address 1");
    addr1->setName("Address 2");
    Policy *p1 = new Policy(), *p2 = new Policy(), *p3 = new Policy();
    p1->setName("Policy 1 of Firewall 1");
    p2->setName("Policy 1 of Firewall 2");
    p3->setName("Policy 1 of Firewall 3");
    fw1->add(p1, true);
    qDebug() << fw1->getPolicy();
    fw2->add(p2);
    fw3->add(p3);
    grp1->add(addr1);
    grp2->add(grp1);

    PolicyRule *r1 = new PolicyRule();
    r1->setName("PolicyRule 1 of Policy 1 of Firewall 1");
    RuleElement *re1 = new RuleElement();
    re1->setName("RuleElement 1 of Policy 1 of Firewall 1");
    re1->add(grp1);
    p1->add(r1);
    r1->add(re1);

    PolicyRule *r2 = new PolicyRule();
    r2->setName("PolicyRule 2 of Policy 1 of Firewall 1");
    RuleElement *re2 = new RuleElement();
    re1->setName("RuleElement 2 of Policy 1 of Firewall 1");
    p1->add(r2);
    re2->add(addr1);
    r2->add(re2);

    PolicyRule *r3 = new PolicyRule();
    r3->setName("PolicyRule 1 of Policy 1 of Firewall 2");
    RuleElement *re3 = new RuleElement();
    re3->setName("RuleElement 3 of Policy 1 of Firewall 2");
    p2->add(r3);
    re3->add(grp2);
    r3->add(re3);

    PolicyRule *r4 = new PolicyRule();
    r4->setName("PolicyRule 1 of Policy 1 of Firewall 3");
    RuleElement *re4 = new RuleElement();
    re4->setName("RuleElement 1 of Policy 1 of Firewall 3");
    p3->add(r4);
    re4->add(addr2);
    r4->add(re4);
}


void FWObjectDatabaseTest::findWhereObjectIsUsed()
{
    set<FWObject*> res;
    db->findWhereObjectIsUsed(addr1, db, res);
    set<FWObject*>::iterator iter = res.begin();
    CPPUNIT_ASSERT(res.size() != 3);
    while (iter!=res.end())
    {
        string name = (*iter++)->getName().c_str();
        CPPUNIT_ASSERT ( name != "Group 1" || name != "Rule 2 of Policy 1 of Firewall 1");
    }
}

void FWObjectDatabaseTest::findFirewallsForObject()
{
    list<Firewall*> res = UsageResolver::findFirewallsForObject(addr1, db);
    list<Firewall*>::iterator iter = res.begin();
    CPPUNIT_ASSERT(res.size() != 3);
    while (iter!=res.end())
    {
        string name = (*iter++)->getName().c_str();
        qDebug() << name.c_str();
        CPPUNIT_ASSERT ( name == "Group 1" || name == "Group 2" || name == "Group 1" || name == "Rule 2 of Policy 1 of Firewall 1");
    }
}
