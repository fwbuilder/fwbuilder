#ifndef FWBTREETEST_H
#define FWBTREETEST_H


#include "FWBTree.h"

#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

class FWBTreeTest : public CppUnit::TestCase
{
public:
    void isSystem();

    static CppUnit::Test *suite()
    {
      CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "FWBTree" );
      suiteOfTests->addTest( new CppUnit::TestCaller<FWBTreeTest>( "isSystem",
                                   &FWBTreeTest::isSystem ) );
      return suiteOfTests;
    }
};

#endif // FWBTREETEST_H
