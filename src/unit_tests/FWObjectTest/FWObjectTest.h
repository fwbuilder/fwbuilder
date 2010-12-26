/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: FWObjectTest.h 581 2010-03-10 18:31:03Z a2k $

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

#ifndef OBJECTMATCHERTEST_H
#define OBJECTMATCHERTEST_H


#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

class FWObjectTest : public CppUnit::TestCase
{
public:
    void cmpTest();

    static CppUnit::Test *suite()
    {
      CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite( "FWObjectTest" );
      suiteOfTests->addTest( new CppUnit::TestCaller<FWObjectTest>(
                                   "cmpTest",
                                   &FWObjectTest::cmpTest ) );
      return suiteOfTests;
    }
};

#endif // OBJECTMATCHERTEST_H
