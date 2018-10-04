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

#include "InetAddrMaskTest.h"
#include <fwbuilder/InetAddrMask.h>

using namespace libfwbuilder;
using namespace std;


void InetAddrMaskTest::testIntToInetAddr()
{
    InetAddr *sa1 = new InetAddr(0);
    CPPUNIT_ASSERT_MESSAGE(sa1->toString(),  sa1->toString() == "0.0.0.0");
        
    sa1 = new InetAddr(AF_INET, 0);
    CPPUNIT_ASSERT_MESSAGE(sa1->toString(),  sa1->toString() == "0.0.0.0");
        
    sa1 = new InetAddr(1);
    CPPUNIT_ASSERT_MESSAGE(sa1->toString(),  sa1->toString() == "128.0.0.0");

    sa1 = new InetAddr(24);
    CPPUNIT_ASSERT_MESSAGE(sa1->toString(),  sa1->toString() == "255.255.255.0");
}

void InetAddrMaskTest::testStringToInetAddr()
{
    InetAddr *sa1 = new InetAddr("0.0.0.1");
    CPPUNIT_ASSERT_MESSAGE("0.0.0.1 -> " + sa1->toString(),  sa1->toString() == "0.0.0.1");
        
    sa1 = new InetAddr("0.0.0.0");
    CPPUNIT_ASSERT_MESSAGE("0.0.0.0 -> " + sa1->toString(),  sa1->toString() == "0.0.0.0");
        
    sa1 = new InetAddr("1.2.3.4");
    CPPUNIT_ASSERT_MESSAGE("1.2.3.4 -> " + sa1->toString(),  sa1->toString() == "1.2.3.4");

    sa1 = new InetAddr("0.0.1");
    CPPUNIT_ASSERT_MESSAGE("0.0.1 -> " + sa1->toString(), sa1->toString() == "0.0.1.0");

    sa1 = new InetAddr("0.1");
    CPPUNIT_ASSERT_MESSAGE("0.1 -> " + sa1->toString(), sa1->toString() == "0.1.0.0");
        
    // "1" ---> "128.0.0.0"   I am not sure this is correct
    sa1 = new InetAddr("1");
    CPPUNIT_ASSERT_MESSAGE("1 -> " + sa1->toString(), sa1->toString() == "128.0.0.0");

    sa1 = new InetAddr("1.0");
    CPPUNIT_ASSERT_MESSAGE("1.0 -> " + sa1->toString(), sa1->toString() == "1.0.0.0");
        
    sa1 = new InetAddr("1.0.0");
    CPPUNIT_ASSERT_MESSAGE("1.0.0 -> " + sa1->toString(), sa1->toString() == "1.0.0.0");

    sa1 = new InetAddr("255.255.255.255");
    CPPUNIT_ASSERT_MESSAGE("255.255.255.255 -> " + sa1->toString(), sa1->toString() == "255.255.255.255");
    CPPUNIT_ASSERT(sa1->isValidV4Netmask() == true);

    sa1 = new InetAddr("255.255.255.128");
    CPPUNIT_ASSERT_MESSAGE("255.255.255.128 -> " + sa1->toString(), sa1->toString() == "255.255.255.128");
    CPPUNIT_ASSERT(sa1->isValidV4Netmask() == true);

    sa1 = new InetAddr("255.255.255.0");
    CPPUNIT_ASSERT_MESSAGE("255.255.255.0 -> " + sa1->toString(), sa1->toString() == "255.255.255.0");
    CPPUNIT_ASSERT(sa1->isValidV4Netmask() == true);

    sa1 = new InetAddr("255.255.0.0");
    CPPUNIT_ASSERT_MESSAGE("255.255.0.0 -> " + sa1->toString(), sa1->toString() == "255.255.0.0");
    CPPUNIT_ASSERT(sa1->isValidV4Netmask() == true);

    sa1 = new InetAddr("255.0.0.0");
    CPPUNIT_ASSERT_MESSAGE("255.0.0.0 -> " + sa1->toString(), sa1->toString() == "255.0.0.0");
    CPPUNIT_ASSERT(sa1->isValidV4Netmask() == true);

    sa1 = new InetAddr("0.0.0.0");
    CPPUNIT_ASSERT_MESSAGE("0.0.0.0 -> " + sa1->toString(), sa1->toString() == "0.0.0.0");
    CPPUNIT_ASSERT(sa1->isValidV4Netmask() == true);

    sa1 = new InetAddr("255.0.255.0");
    CPPUNIT_ASSERT_MESSAGE("255.0.255.0 -> " + sa1->toString(), sa1->toString() == "255.0.255.0");
    CPPUNIT_ASSERT(sa1->isValidV4Netmask() == false);

}

void InetAddrMaskTest::testStringToInetAddrExceptions()
{
    CPPUNIT_ASSERT_NO_THROW(new InetAddr("1.2.3.4"));
    CPPUNIT_ASSERT_NO_THROW(new InetAddr("1.2.3.4/24"));
    CPPUNIT_ASSERT_THROW(new InetAddr("1.2.3.4/40"), FWException);

    CPPUNIT_ASSERT_THROW(new InetAddr("300.300.300.300"), FWException);
    CPPUNIT_ASSERT_THROW(new InetAddr("1.2.3.4.5"), FWException);
    CPPUNIT_ASSERT_THROW(new InetAddr("foo.bar"), FWException);
    CPPUNIT_ASSERT_THROW(new InetAddr("1.2.foo.bar"), FWException);

    CPPUNIT_ASSERT_THROW(new InetAddr(40), FWException);
    CPPUNIT_ASSERT_NO_THROW(new InetAddr(24));
    CPPUNIT_ASSERT_THROW(new InetAddr((char*)(nullptr)), FWException);
    CPPUNIT_ASSERT_NO_THROW(new InetAddr(0));
}

void InetAddrMaskTest::testInetAddressOps()
{
    InetAddr x1("1.2.3.4");
    InetAddr y1(24);
    InetAddr z1 = x1 & y1;

    CPPUNIT_ASSERT(z1.toString() == "1.2.3.0");
    CPPUNIT_ASSERT( (~y1).toString() == "0.0.0.255");

    InetAddr z2 = z1 | ~y1;
    CPPUNIT_ASSERT(z2.toString() == "1.2.3.255");

    InetAddr z3 = z1 | ~y1;
    CPPUNIT_ASSERT(z3.toString() == "1.2.3.255");

    InetAddr z4 = x1 + 1;
    CPPUNIT_ASSERT(z4.toString() == "1.2.3.5");

    InetAddr z5 = z4 - 1;
    CPPUNIT_ASSERT(z5.toString() == "1.2.3.4");

    CPPUNIT_ASSERT(z5 == x1);

    InetAddr x2("255.255.255.255");
    InetAddr z6 = x2 + 1;
    CPPUNIT_ASSERT(z6.toString() == "0.0.0.0");

    InetAddr x3("1.2.2.4");
    CPPUNIT_ASSERT(x3 < x1);
    CPPUNIT_ASSERT(x1 > x3);

}

void InetAddrMaskTest::testStringToInetAddrMask()
{
    string sa;

    InetAddrMask *a1 = new InetAddrMask();
    CPPUNIT_ASSERT(a1->toString() == "0.0.0.0/0.0.0.0");
    sa = a1->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="0.0.0.0");
    // 0.0.0.0/0.0.0.0 has maximum dimension (represents all possible addresses)
    CPPUNIT_ASSERT(a1->dimension() == (((unsigned int)1)<<31)-1);

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

    InetAddrMask *a5 = new InetAddrMask(string("1.1.1.1/24"));
    sa = a5->getAddressPtr()->toString();
    CPPUNIT_ASSERT(sa=="1.1.1.1");
    sa = a5->getNetmaskPtr()->toString();
    CPPUNIT_ASSERT(sa=="255.255.255.0");
    CPPUNIT_ASSERT(a5->dimension()==256);

}

string InetAddrMaskTest::vectorInetAddrMaskToString(vector<InetAddrMask> vect)
{
    string res;
    vector<InetAddrMask>::iterator it;
    for (it=vect.begin(); it!=vect.end(); ++it)
    {
        res += it->toString() + " ";
    }
    return res;
}

void InetAddrMaskTest::testIPv4Overlap()
{
    string res;

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0"))
        )
    );
    CPPUNIT_ASSERT(res=="10.0.0.0/255.255.255.0 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0")),
            InetAddrMask(InetAddr("10.0.0.255"), InetAddr("255.255.255.255"))
        )
    );
    CPPUNIT_ASSERT(res=="10.0.0.255/255.255.255.255 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.255"), InetAddr("255.255.255.255")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0"))
        )
    );
    CPPUNIT_ASSERT(res=="10.0.0.255/255.255.255.255 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.254")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0"))
        )
    );
    CPPUNIT_ASSERT(res=="10.0.0.0/255.255.255.254 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.254"))
        )
    );
    CPPUNIT_ASSERT(res=="10.0.0.0/255.255.255.254 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.252"), InetAddr("255.255.255.252")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0"))
        )
    );
    CPPUNIT_ASSERT(res=="10.0.0.252/255.255.255.252 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0")),
            InetAddrMask(InetAddr("10.0.0.252"), InetAddr("255.255.255.252"))
        )
    );
    CPPUNIT_ASSERT(res=="10.0.0.252/255.255.255.252 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.128"), InetAddr("255.255.255.252")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0"))
        )
    );
    CPPUNIT_ASSERT(res=="10.0.0.128/255.255.255.252 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0")),
            InetAddrMask(InetAddr("10.0.0.128"), InetAddr("255.255.255.252"))
        )
    );
    CPPUNIT_ASSERT(res=="10.0.0.128/255.255.255.252 ");

    // test specifically for #1934
    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.2"), InetAddr("255.255.255.254")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0")))
    );
    CPPUNIT_ASSERT(res=="10.0.0.2/255.255.255.254 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.2"), InetAddr("255.255.255.254")),
            InetAddrMask(InetAddr("0.0.0.0"), InetAddr("0.0.0.0")))
    );
    CPPUNIT_ASSERT(res=="10.0.0.2/255.255.255.254 ");

}
