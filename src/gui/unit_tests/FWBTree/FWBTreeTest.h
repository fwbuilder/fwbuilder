#ifndef FWBTREETEST_H
#define FWBTREETEST_H


#include "FWBTree.h"

#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

class FWBTreeTest : public CppUnit::TestCase
{
    bool isStandardFolder(std::string path);
    QSet<libfwbuilder::FWObject*> getStandardFolders(libfwbuilder::FWObject *root);
public:
    void isSystem();
    void validateForInsertion();

    static CppUnit::Test *suite()
    {
      CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "FWBTree" );
      suiteOfTests->addTest( new CppUnit::TestCaller<FWBTreeTest>( "isSystem",
                                   &FWBTreeTest::isSystem ) );
      suiteOfTests->addTest( new CppUnit::TestCaller<FWBTreeTest>( "validateForInsertion",
                                   &FWBTreeTest::validateForInsertion ) );
      return suiteOfTests;
    }
};

#endif // FWBTREETEST_H
