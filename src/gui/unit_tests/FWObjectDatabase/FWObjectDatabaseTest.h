#ifndef FWOBJECTDATABASETEST_H
#define FWOBJECTDATABASETEST_H

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Address.h"

#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

class FWObjectDatabaseTest : public CppUnit::TestCase
{
public:
    FWObjectDatabaseTest();
    libfwbuilder::FWObjectDatabase *db;
    libfwbuilder::Address *addr1;
    libfwbuilder::Address *addr2;

    void setUp();

    void findWhereObjectIsUsed();
    void findFirewallsForObject();

    static CppUnit::Test *suite()
    {
      CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "interfaceProperties" );
      suiteOfTests->addTest( new CppUnit::TestCaller<FWObjectDatabaseTest>(
                                   "validateInterface",
                                   &FWObjectDatabaseTest::findWhereObjectIsUsed ) );
      suiteOfTests->addTest( new CppUnit::TestCaller<FWObjectDatabaseTest>(
                                   "validateInterface",
                                   &FWObjectDatabaseTest::findFirewallsForObject ) );
      return suiteOfTests;
    }
};

#endif // FWOBJECTDATABASETEST_H
