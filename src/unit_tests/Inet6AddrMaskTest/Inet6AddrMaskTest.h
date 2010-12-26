/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

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

#ifndef INET6ADDRMASKTEST_H
#define INET6ADDRMASKTEST_H

#include <cppunit/extensions/HelperMacros.h>

class Inet6AddrMaskTest : public CppUnit::TestFixture
{
public:
    void testStringToInetAddrExceptions();
    void testIntToInetAddr6();
    void testStringToInetAddr6();
    void testStringToInetAddrMask();
    void testInet6AddressOps();
    void testUInt128ToInetAddr6();
    void testInetAddr6ToUInt128();

    CPPUNIT_TEST_SUITE(Inet6AddrMaskTest);

    CPPUNIT_TEST(testStringToInetAddrExceptions);
    CPPUNIT_TEST(testIntToInetAddr6);
    CPPUNIT_TEST(testUInt128ToInetAddr6);
    CPPUNIT_TEST(testInetAddr6ToUInt128);
    CPPUNIT_TEST(testStringToInetAddr6);
    CPPUNIT_TEST(testStringToInetAddrMask);
    CPPUNIT_TEST(testInet6AddressOps);
    
    CPPUNIT_TEST_SUITE_END();

};

#endif // INET6ADDRMASKTEST_H
