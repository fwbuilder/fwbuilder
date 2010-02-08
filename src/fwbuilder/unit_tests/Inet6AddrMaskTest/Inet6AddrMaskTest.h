#ifndef INET6ADDRMASKTEST_H
#define INET6ADDRMASKTEST_H

#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

class Inet6AddrMaskTest : public CppUnit::TestCase
{
public:
    void runTest();

    static CppUnit::Test *suite()
    {
      CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "Inet6AddrMaskTest" );
      suiteOfTests->addTest( new CppUnit::TestCaller<Inet6AddrMaskTest>( "runTest", &Inet6AddrMaskTest::runTest ) );
      return suiteOfTests;
    }
};

#endif // INET6ADDRMASKTEST_H
