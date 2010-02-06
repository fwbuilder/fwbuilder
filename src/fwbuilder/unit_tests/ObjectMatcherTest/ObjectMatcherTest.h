#ifndef OBJECTMATCHERTEST_H
#define OBJECTMATCHERTEST_H


#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

class ObjectMatcherTest : public CppUnit::TestCase
{
public:
    void matchTest();

    static CppUnit::Test *suite()
    {
      CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "ObjectMatcherTest" );
      suiteOfTests->addTest( new CppUnit::TestCaller<ObjectMatcherTest>(
                                   "matchTest",
                                   &ObjectMatcherTest::matchTest ) );
      return suiteOfTests;
    }
};

#endif // OBJECTMATCHERTEST_H
