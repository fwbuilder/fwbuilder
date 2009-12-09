#ifndef FWOBJECTDATABASETEST_H
#define FWOBJECTDATABASETEST_H

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/Library.h"
#include "FWBTree.h"

#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

class UsageResolverTest : public CppUnit::TestCase
{
    void addToLib(libfwbuilder::FWObject* obj);
public:
    libfwbuilder::FWObjectDatabase *db;
    libfwbuilder::Library *lib;
    libfwbuilder::Address *addr1;
    libfwbuilder::Address *addr2;
    libfwbuilder::PolicyRule *r1, *r2, *r3, *r4;

    void setUp();

    void findWhereObjectIsUsed();
    void findFirewallsForObject();

    static CppUnit::Test *suite()
    {
      CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "FWObjectDatabase" );
      suiteOfTests->addTest( new CppUnit::TestCaller<UsageResolverTest>(
                                   "findWhereObjectIsUsed",
                                   &UsageResolverTest::findWhereObjectIsUsed ) );
      suiteOfTests->addTest( new CppUnit::TestCaller<UsageResolverTest>(
                                   "findFirewallsForObject",
                                   &UsageResolverTest::findFirewallsForObject ) );
      return suiteOfTests;
    }
};

#endif // FWOBJECTDATABASETEST_H
