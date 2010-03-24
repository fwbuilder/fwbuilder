/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com
           Vadim Kurland          vadim@fwbuilder.org

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

#ifndef ADDRESSTABLETEST_H
#define ADDRESSTABLETEST_H


#include "fwbuilder/FWObjectDatabase.h"

#include <set>
#include <string>

#include <cppunit/extensions/HelperMacros.h>

typedef std::set <std::string, std::less<std::string> > setStrings;

class AddressTableTest : public CppUnit::TestFixture
{
    libfwbuilder::FWObjectDatabase *objdb;
    libfwbuilder::FWObject *address_tables_group;

    CPPUNIT_TEST_SUITE(AddressTableTest);
    CPPUNIT_TEST(positiveTest);
    CPPUNIT_TEST(negativeTest1);
    CPPUNIT_TEST(negativeTest2);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void positiveTest();
    void negativeTest1();
    void negativeTest2();
    
};

#endif // ADDRESSTABLETEST_H
