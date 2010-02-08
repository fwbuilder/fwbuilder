/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivkiy     a2k0001@gmail.com

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

using namespace libfwbuilder;
using namespace std;

void Inet6AddrMaskTest::runTest()
{
    string sa;

    InetAddr x1(AF_INET6, 0);
    CPPUNIT_ASSERT(x1.toString()=="::" && x1.getLength()==128);

    InetAddr x2(AF_INET6, 8);
    CPPUNIT_ASSERT(x2.toString()=="ff00::" && x2.getLength()==8);

    InetAddr x3(AF_INET6, 16);
    CPPUNIT_ASSERT(x3.toString()=="ffff::" && x3.getLength()==16);

    InetAddr x4(AF_INET6, 64);
    CPPUNIT_ASSERT(x4.toString()=="ffff:ffff:ffff:ffff::" && x4.getLength()==64);

    InetAddr x5(AF_INET6, 128);
    CPPUNIT_ASSERT(x5.toString()=="ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff" && x5.getLength()==128);

    InetAddr x6(AF_INET6, "64");
    CPPUNIT_ASSERT(x6.toString()=="ffff:ffff:ffff:ffff::" && x6.getLength()==64);

    InetAddr x7(AF_INET6, "fe80::21d:9ff:fe8b:8e94");
    InetAddr y6(AF_INET6, 64);
    InetAddr z = x7 & y6;

    CPPUNIT_ASSERT(z.toString()=="fe80::");

    CPPUNIT_ASSERT((~y6).toString()=="::ffff:ffff:ffff:ffff");

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
