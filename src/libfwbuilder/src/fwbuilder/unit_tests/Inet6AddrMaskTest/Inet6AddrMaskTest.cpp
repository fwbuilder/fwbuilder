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

#include "Inet6AddrMaskTest.h"
#include <fwbuilder/Inet6AddrMask.h>
#include <fwbuilder/uint128.h>

using namespace libfwbuilder;
using namespace std;

void Inet6AddrMaskTest::testUInt128ToInetAddr6()
{
    InetAddr x1(AF_INET6, "::1");
    uint128 x = x1.to_uint128();
    InetAddr x2(AF_INET6, 0);
    x2.init_from_uint128(x);
    CPPUNIT_ASSERT(x2.toString() == "::1");

    InetAddr x3(AF_INET6, "::8000");
    x = x3.to_uint128();
    InetAddr x4(AF_INET6, 0);
    x4.init_from_uint128(x);
    CPPUNIT_ASSERT(x4.toString() == "::8000");

    InetAddr x5(AF_INET6, "::ff00");
    x = x5.to_uint128();
    InetAddr x6(AF_INET6, 0);
    x6.init_from_uint128(x);
    CPPUNIT_ASSERT(x6.toString() == "::ff00");

    InetAddr x90(AF_INET6, "fe80::20c:29ff:fed2:cca1");
    x = x90.to_uint128();
    InetAddr x91(AF_INET6, 0);
    x91.init_from_uint128(x);
    CPPUNIT_ASSERT(x91.toString() == "fe80::20c:29ff:fed2:cca1");
}

void Inet6AddrMaskTest::testInetAddr6ToUInt128()
{
    InetAddr x1(AF_INET6, 0);
    uint128 x = x1.to_uint128();
    CPPUNIT_ASSERT(x.to_string() == "0");

    InetAddr x2(AF_INET6, 1);
    x = x2.to_uint128();
    CPPUNIT_ASSERT(x.to_string() == "800000000000000000000000");

    InetAddr x3(AF_INET6, 8);
    x = x3.to_uint128();
    CPPUNIT_ASSERT(x.to_string() == "FF0000000000000000000000");

    InetAddr x4(AF_INET6, 16);
    x = x4.to_uint128();
    CPPUNIT_ASSERT(x.to_string() == "FFFF00000000000000000000");

    InetAddr x5(AF_INET6, 64);
    x = x5.to_uint128();
    CPPUNIT_ASSERT(x.to_string() == "FFFFFFFFFFFFFFFF00000000");

    InetAddr x6(AF_INET6, 128);
    x = x6.to_uint128();
    CPPUNIT_ASSERT(x.to_string() == "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");

    InetAddr x7(AF_INET6, "fe80::20c:29ff:fed2:cca1");
    x = x7.to_uint128();
    CPPUNIT_ASSERT(x.to_string() == "FE8000000000000020C29FFFED2CCA1");
}

void Inet6AddrMaskTest::testIntToInetAddr6()
{
    InetAddr x1(AF_INET6, 0);
    CPPUNIT_ASSERT(x1.toString()=="::" && x1.getLength()==128);

    InetAddr x2(AF_INET6, 1);
    CPPUNIT_ASSERT(x2.toString()=="8000::" && x2.getLength()==1);

    InetAddr x3(AF_INET6, 8);
    CPPUNIT_ASSERT(x3.toString()=="ff00::" && x3.getLength()==8);

    InetAddr x4(AF_INET6, 16);
    CPPUNIT_ASSERT(x4.toString()=="ffff::" && x4.getLength()==16);

    InetAddr x5(AF_INET6, 64);
    CPPUNIT_ASSERT(x5.toString()=="ffff:ffff:ffff:ffff::" && x5.getLength()==64);

    InetAddr x6(AF_INET6, 128);
    CPPUNIT_ASSERT(x6.toString()=="ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff" && x6.getLength()==128);

}        


void Inet6AddrMaskTest::testStringToInetAddr6()
{
    InetAddr *sa1 = new InetAddr(AF_INET6, "::");
    CPPUNIT_ASSERT_MESSAGE(sa1->toString(),  sa1->toString() == "::");

    sa1 = new InetAddr(AF_INET6, "::1");
    CPPUNIT_ASSERT_MESSAGE(sa1->toString(),  sa1->toString() == "::1");
        
    sa1 = new InetAddr(AF_INET6, "fe80::20c:29ff:fed2:cca1");
    CPPUNIT_ASSERT_MESSAGE(sa1->toString(),
                           sa1->toString() == "fe80::20c:29ff:fed2:cca1");

    InetAddr x6(AF_INET6, "64");
    CPPUNIT_ASSERT_MESSAGE(x6.toString(),
                           x6.toString()=="ffff:ffff:ffff:ffff::");
    CPPUNIT_ASSERT(x6.getLength()==64);
}

void Inet6AddrMaskTest::testStringToInetAddrExceptions()
{
    CPPUNIT_ASSERT_THROW(new InetAddr(AF_INET, "fe80::20c:29ff:fed2:cca1"), FWException);
    CPPUNIT_ASSERT_THROW(new InetAddr("fe80::20c:29ff:fed2:cca1"), FWException);
    CPPUNIT_ASSERT_NO_THROW(new InetAddr(AF_INET6, "fe80::20c:29ff:fed2:cca1/64"));
    CPPUNIT_ASSERT_THROW(new InetAddr(AF_INET6, "fe80::20c:29ff:fed2:cca1/200"), FWException);
    CPPUNIT_ASSERT_THROW(new InetAddr(AF_INET6, "fe80::foo:bar:fed2:cca1"), FWException);
    CPPUNIT_ASSERT_THROW(new InetAddr(AF_INET6, "1.2.3.4"), FWException);

    CPPUNIT_ASSERT_NO_THROW(new InetAddr(AF_INET6, 64));
    CPPUNIT_ASSERT_THROW(new InetAddr(AF_INET6, 256), FWException);
}

/*
 * Note that our current implementation of address operations for ipv6 is very
 * limited
 */
void Inet6AddrMaskTest::testInet6AddressOps()
{
    InetAddr x7(AF_INET6, "fe80::21d:9ff:fe8b:8e94");
    InetAddr y6(AF_INET6, 64);
    InetAddr z1 = x7 & y6;

    CPPUNIT_ASSERT(z1.toString()=="fe80::");

    CPPUNIT_ASSERT((~y6).toString()=="::ffff:ffff:ffff:ffff");

    InetAddr z2 = z1 | ~y6;
    CPPUNIT_ASSERT_MESSAGE(z2.toString(), z2.toString()=="fe80::ffff:ffff:ffff:ffff");

    InetAddr z3 = x7 + 1;
    CPPUNIT_ASSERT_MESSAGE(z3.toString(), z3.toString() == "fe80::21d:9ff:fe8b:8e95");

    InetAddr z4 = z3 - 1;
    CPPUNIT_ASSERT_MESSAGE(z4.toString(), z4.toString() == "fe80::21d:9ff:fe8b:8e94");


    InetAddr z5 = x7 + 65536;
    CPPUNIT_ASSERT_MESSAGE(z5.toString(), z5.toString() == "fe80::21d:9ff:fe8c:8e94");

    InetAddr z6 = z5 - 65536;
    CPPUNIT_ASSERT_MESSAGE(z6.toString(), z6.toString() == "fe80::21d:9ff:fe8b:8e94");

    InetAddr z7 = x7 + 2147483647;  // 2^31-1
    CPPUNIT_ASSERT_MESSAGE(z7.toString(), z7.toString() == "fe80::21d:a00:7e8b:8e93");

    InetAddr z8 = z7 - 2147483647;
    CPPUNIT_ASSERT_MESSAGE(z8.toString(), z8.toString() == "fe80::21d:9ff:fe8b:8e94");

    InetAddr x8(AF_INET6, "fe80::21d:9ff:fe8b:1111");
    CPPUNIT_ASSERT(x7 > x8);
    CPPUNIT_ASSERT(x8 < x7);

    unsigned int dist = x8.distance(x7);
    CPPUNIT_ASSERT(dist == 32132);

    InetAddr x9(AF_INET6, "fe80::21d:9ff:fe8b:8e94");
    CPPUNIT_ASSERT(x7 == x9);

    InetAddr x10(AF_INET6, "fe80::21d:a00:7e8b:8e93");
    CPPUNIT_ASSERT(x10 > x9);
    CPPUNIT_ASSERT(x9 < x10);
}

void Inet6AddrMaskTest::testStringToInetAddrMask()
{
    string sa;

    Inet6AddrMask *a1 = new Inet6AddrMask();
    sa = a1->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="::");

    Inet6AddrMask *a2 = new Inet6AddrMask(
        InetAddr(AF_INET6, "fe80::21d:9ff:fe8b:8e94"),
        InetAddr(AF_INET6, 128));

    sa = a2->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="fe80::21d:9ff:fe8b:8e94");
    sa = a2->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
    CPPUNIT_ASSERT(a2->getNetmaskPtr()->getLength()==128);

    CPPUNIT_ASSERT(a2->toString()=="fe80::21d:9ff:fe8b:8e94");

    Inet6AddrMask *a3 = new Inet6AddrMask(
        InetAddr(AF_INET6, "fe80::21d:9ff:fe8b:8e94"),
        InetAddr(AF_INET6, 128));

    a3->setNetmask(InetAddr(AF_INET6, 64));
    sa = a3->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="fe80::21d:9ff:fe8b:8e94");
    sa = a3->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="ffff:ffff:ffff:ffff::");
    CPPUNIT_ASSERT(a3->getNetmaskPtr()->getLength()==64);
    CPPUNIT_ASSERT(a3->toString()=="fe80::21d:9ff:fe8b:8e94/64");


    Inet6AddrMask *a4 = new Inet6AddrMask(
        string("fe80::21d:9ff:fe8b:8e94/64"));
    sa = a4->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="fe80::21d:9ff:fe8b:8e94");
    sa = a4->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="ffff:ffff:ffff:ffff::");

    CPPUNIT_ASSERT(a4->belongs( *(a2->getAddressPtr()) ));


    Inet6AddrMask *a5 = new Inet6AddrMask(*a3);
    sa = a5->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="fe80::21d:9ff:fe8b:8e94");
    sa = a5->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="ffff:ffff:ffff:ffff::");

    a5->setAddress(InetAddr(AF_INET6, "3ffe:1200:2001:1:8000::1"));
    sa = a5->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="3ffe:1200:2001:1:8000::1");
    sa = a5->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="ffff:ffff:ffff:ffff::");

}
