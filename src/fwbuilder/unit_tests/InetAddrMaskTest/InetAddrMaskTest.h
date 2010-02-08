#ifndef INETADDRMASKTEST_H
#define INETADDRMASKTEST_H

#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

class InetAddrMaskTest : public CppUnit::TestCase
{
public:
    void runTest();

    static CppUnit::Test *suite()
    {
      CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "InetAddrMaskTest" );
      suiteOfTests->addTest( new CppUnit::TestCaller<InetAddrMaskTest>( "runTest", &InetAddrMaskTest::runTest ) );
      return suiteOfTests;
    }
};

#endif // INETADDRMASKTEST_H
