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

#include "InetAddrMaskTest.h"
#include <fwbuilder/InetAddrMask.h>

using namespace libfwbuilder;
using namespace std;

void InetAddrMaskTest::runTest()
{
    string sa;

    InetAddrMask *a1 = new InetAddrMask();
    CPPUNIT_ASSERT(a1->toString() == "0.0.0.0/0.0.0.0");
    sa = a1->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="0.0.0.0");
    // 0.0.0.0/0.0.0.0 has maximum dimension (represents all possible addresses)
    CPPUNIT_ASSERT(a1->dimension() == (1<<31)-1);

    InetAddrMask *a2 = new InetAddrMask(InetAddr("1.1.1.1"), InetAddr("255.255.255.0"));
    sa = a2->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="1.1.1.0");
    sa = a2->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="255.255.255.0");
    CPPUNIT_ASSERT(a2->dimension()==256);
    CPPUNIT_ASSERT(a2->toString()=="1.1.1.0/255.255.255.0");

    InetAddrMask *a3 = new InetAddrMask(string("1.1.1.1"));
    sa = a3->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="1.1.1.1");
    sa = a3->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="255.255.255.255");
    CPPUNIT_ASSERT(a3->dimension()==1);

    CPPUNIT_ASSERT(a2->belongs( *(a3->getAddressPtr()) ));

    InetAddrMask *a4 = new InetAddrMask(*a3);
    sa = a4->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="1.1.1.1");
    sa = a4->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="255.255.255.255");
    CPPUNIT_ASSERT(a4->dimension()==1);

    a4->setAddress(InetAddr("2.2.2.2"));
    sa = a4->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="2.2.2.2");
    sa = a4->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="255.255.255.255");
    CPPUNIT_ASSERT(a4->dimension()==1);

    a4->setNetmask(InetAddr("255.255.0.0"));
    sa = a4->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="2.2.2.2");
    sa = a4->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="255.255.0.0");
    CPPUNIT_ASSERT(a4->dimension()==256*256);

    a4->setNetmask(InetAddr("8"));
    sa = a4->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="2.2.2.2");
    sa = a4->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="255.0.0.0");
    CPPUNIT_ASSERT(a4->dimension()==256*256*256);

    CPPUNIT_ASSERT_THROW(a4->setNetmask(InetAddr("40")), FWException);

    // a4 should not have changed
    sa = a4->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="2.2.2.2");
    sa = a4->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="255.0.0.0");
    CPPUNIT_ASSERT(a4->dimension()==256*256*256);

}
