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
#include "fwbuilder/FWException.h"

#include <QTest>

using namespace libfwbuilder;
using namespace std;


void InetAddrMaskTest::testIntToInetAddr()
{
    InetAddr *sa1 = new InetAddr(0);
    QVERIFY2(sa1->toString() == "0.0.0.0", sa1->toString().data());

    sa1 = new InetAddr(AF_INET, 0);
    QVERIFY2(sa1->toString() == "0.0.0.0", sa1->toString().data());

    sa1 = new InetAddr(1);
    QVERIFY2(sa1->toString() == "128.0.0.0", sa1->toString().data());

    sa1 = new InetAddr(24);
    QVERIFY2(sa1->toString() == "255.255.255.0", sa1->toString().data());
}

void InetAddrMaskTest::testStringToInetAddr()
{
    InetAddr *sa1 = new InetAddr("0.0.0.1");
    QVERIFY2(sa1->toString() == "0.0.0.1", std::string("0.0.0.1 -> " + sa1->toString()).data());

    sa1 = new InetAddr("0.0.0.0");
    QVERIFY2(sa1->toString() == "0.0.0.0", std::string("0.0.0.0 -> " + sa1->toString()).data());

    sa1 = new InetAddr("1.2.3.4");
    QVERIFY2(sa1->toString() == "1.2.3.4", std::string("1.2.3.4 -> " + sa1->toString()).data());

    sa1 = new InetAddr("0.0.1");
    QVERIFY2(sa1->toString() == "0.0.1.0", std::string("0.0.1 -> " + sa1->toString()).data());

    sa1 = new InetAddr("0.1");
    QVERIFY2(sa1->toString() == "0.1.0.0", std::string("0.1 -> " + sa1->toString()).data());

    // "1" ---> "128.0.0.0"   I am not sure this is correct
    sa1 = new InetAddr("1");
    QVERIFY2(sa1->toString() == "128.0.0.0", std::string("1 -> " + sa1->toString()).data());

    sa1 = new InetAddr("1.0");
    QVERIFY2(sa1->toString() == "1.0.0.0", std::string("1.0 -> " + sa1->toString()).data());

    sa1 = new InetAddr("1.0.0");
    QVERIFY2(sa1->toString() == "1.0.0.0", std::string("1.0.0 -> " + sa1->toString()).data());

    sa1 = new InetAddr("255.255.255.255");
    QVERIFY2(sa1->toString() == "255.255.255.255", std::string("255.255.255.255 -> " + sa1->toString()).data());
    QVERIFY(sa1->isValidV4Netmask() == true);

    sa1 = new InetAddr("255.255.255.128");
    QVERIFY2(sa1->toString() == "255.255.255.128", std::string("255.255.255.128 -> " + sa1->toString()).data());
    QVERIFY(sa1->isValidV4Netmask() == true);

    sa1 = new InetAddr("255.255.255.0");
    QVERIFY2(sa1->toString() == "255.255.255.0", std::string("255.255.255.0 -> " + sa1->toString()).data());
    QVERIFY(sa1->isValidV4Netmask() == true);

    sa1 = new InetAddr("255.255.0.0");
    QVERIFY2(sa1->toString() == "255.255.0.0", std::string("255.255.0.0 -> " + sa1->toString()).data());
    QVERIFY(sa1->isValidV4Netmask() == true);

    sa1 = new InetAddr("255.0.0.0");
    QVERIFY2(sa1->toString() == "255.0.0.0", std::string("255.0.0.0 -> " + sa1->toString()).data());
    QVERIFY(sa1->isValidV4Netmask() == true);

    sa1 = new InetAddr("0.0.0.0");
    QVERIFY2(sa1->toString() == "0.0.0.0", std::string("0.0.0.0 -> " + sa1->toString()).data());
    QVERIFY(sa1->isValidV4Netmask() == true);

    sa1 = new InetAddr("255.0.255.0");
    QVERIFY2(sa1->toString() == "255.0.255.0", std::string("255.0.255.0 -> " + sa1->toString()).data());
    QVERIFY(sa1->isValidV4Netmask() == false);

}

void InetAddrMaskTest::testStringToInetAddrExceptions()
{
    try {
        new InetAddr("1.2.3.4");
    } catch (const FWException &e) {
        QFAIL(std::string("Exception thrown: ").append(e.toString()).data());
    }

    try {
        new InetAddr("1.2.3.4/24");
    } catch (const FWException &e) {
        QFAIL(std::string("Exception thrown: ").append(e.toString()).data());
    }

    QVERIFY_EXCEPTION_THROWN(new InetAddr("1.2.3.4/40"), FWException);

    QVERIFY_EXCEPTION_THROWN(new InetAddr("300.300.300.300"), FWException);
    QVERIFY_EXCEPTION_THROWN(new InetAddr("1.2.3.4.5"), FWException);
    QVERIFY_EXCEPTION_THROWN(new InetAddr("foo.bar"), FWException);
    QVERIFY_EXCEPTION_THROWN(new InetAddr("1.2.foo.bar"), FWException);

    QVERIFY_EXCEPTION_THROWN(new InetAddr(40), FWException);
    try {
        new InetAddr(24);
    } catch (const FWException &e) {
        QFAIL(std::string("Exception thrown: ").append(e.toString()).data());
    }

    QVERIFY_EXCEPTION_THROWN(new InetAddr((char*)(nullptr)), FWException);

    try {
        new InetAddr(0);
    } catch (const FWException &e) {
        QFAIL(std::string("Exception thrown: ").append(e.toString()).data());
    }

}

void InetAddrMaskTest::testInetAddressOps()
{
    InetAddr x1("1.2.3.4");
    InetAddr y1(24);
    InetAddr z1 = x1 & y1;

    QVERIFY(z1.toString() == "1.2.3.0");
    QVERIFY( (~y1).toString() == "0.0.0.255");

    InetAddr z2 = z1 | ~y1;
    QVERIFY(z2.toString() == "1.2.3.255");

    InetAddr z3 = z1 | ~y1;
    QVERIFY(z3.toString() == "1.2.3.255");

    InetAddr z4 = x1 + 1;
    QVERIFY(z4.toString() == "1.2.3.5");

    InetAddr z5 = z4 - 1;
    QVERIFY(z5.toString() == "1.2.3.4");

    QVERIFY(z5 == x1);

    InetAddr x2("255.255.255.255");
    InetAddr z6 = x2 + 1;
    QVERIFY(z6.toString() == "0.0.0.0");

    InetAddr x3("1.2.2.4");
    QVERIFY(x3 < x1);
    QVERIFY(x1 > x3);

}

void InetAddrMaskTest::testStringToInetAddrMask()
{
    string sa;

    InetAddrMask *a1 = new InetAddrMask();
    QVERIFY(a1->toString() == "0.0.0.0/0.0.0.0");
    sa = a1->getAddressPtr()->toString();
    QVERIFY(sa=="0.0.0.0");
    // 0.0.0.0/0.0.0.0 has maximum dimension (represents all possible addresses)
    QVERIFY(a1->dimension() == (((unsigned int)1)<<31)-1);

    InetAddrMask *a2 = new InetAddrMask(InetAddr("1.1.1.1"), InetAddr("255.255.255.0"));
    sa = a2->getAddressPtr()->toString();
    QVERIFY(sa=="1.1.1.0");
    sa = a2->getNetmaskPtr()->toString();
    QVERIFY(sa=="255.255.255.0");
    QVERIFY(a2->dimension()==256);
    QVERIFY(a2->toString()=="1.1.1.0/255.255.255.0");


    InetAddrMask *a3 = new InetAddrMask(string("1.1.1.1"));
    sa = a3->getAddressPtr()->toString();
    QVERIFY(sa=="1.1.1.1");
    sa = a3->getNetmaskPtr()->toString();
    QVERIFY(sa=="255.255.255.255");
    QVERIFY(a3->dimension()==1);

    QVERIFY(a2->belongs( *(a3->getAddressPtr()) ));

    InetAddrMask *a4 = new InetAddrMask(*a3);
    sa = a4->getAddressPtr()->toString();
    QVERIFY(sa=="1.1.1.1");
    sa = a4->getNetmaskPtr()->toString();
    QVERIFY(sa=="255.255.255.255");
    QVERIFY(a4->dimension()==1);

    a4->setAddress(InetAddr("2.2.2.2"));
    sa = a4->getAddressPtr()->toString();
    QVERIFY(sa=="2.2.2.2");
    sa = a4->getNetmaskPtr()->toString();
    QVERIFY(sa=="255.255.255.255");
    QVERIFY(a4->dimension()==1);

    a4->setNetmask(InetAddr("255.255.0.0"));
    sa = a4->getAddressPtr()->toString();
    QVERIFY(sa=="2.2.2.2");
    sa = a4->getNetmaskPtr()->toString();
    QVERIFY(sa=="255.255.0.0");
    QVERIFY(a4->dimension()==256*256);

    a4->setNetmask(InetAddr("8"));
    sa = a4->getAddressPtr()->toString();
    QVERIFY(sa=="2.2.2.2");
    sa = a4->getNetmaskPtr()->toString();
    QVERIFY(sa=="255.0.0.0");
    QVERIFY(a4->dimension()==256*256*256);

    QVERIFY_EXCEPTION_THROWN(a4->setNetmask(InetAddr("40")), FWException);

    // a4 should not have changed
    sa = a4->getAddressPtr()->toString();
    QVERIFY(sa=="2.2.2.2");
    sa = a4->getNetmaskPtr()->toString();
    QVERIFY(sa=="255.0.0.0");
    QVERIFY(a4->dimension()==256*256*256);

    InetAddrMask *a5 = new InetAddrMask(string("1.1.1.1/24"));
    sa = a5->getAddressPtr()->toString();
    QVERIFY(sa=="1.1.1.1");
    sa = a5->getNetmaskPtr()->toString();
    QVERIFY(sa=="255.255.255.0");
    QVERIFY(a5->dimension()==256);

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
    QVERIFY(res=="10.0.0.0/255.255.255.0 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0")),
            InetAddrMask(InetAddr("10.0.0.255"), InetAddr("255.255.255.255"))
        )
    );
    QVERIFY(res=="10.0.0.255/255.255.255.255 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.255"), InetAddr("255.255.255.255")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0"))
        )
    );
    QVERIFY(res=="10.0.0.255/255.255.255.255 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.254")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0"))
        )
    );
    QVERIFY(res=="10.0.0.0/255.255.255.254 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.254"))
        )
    );
    QVERIFY(res=="10.0.0.0/255.255.255.254 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.252"), InetAddr("255.255.255.252")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0"))
        )
    );
    QVERIFY(res=="10.0.0.252/255.255.255.252 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0")),
            InetAddrMask(InetAddr("10.0.0.252"), InetAddr("255.255.255.252"))
        )
    );
    QVERIFY(res=="10.0.0.252/255.255.255.252 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.128"), InetAddr("255.255.255.252")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0"))
        )
    );
    QVERIFY(res=="10.0.0.128/255.255.255.252 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0")),
            InetAddrMask(InetAddr("10.0.0.128"), InetAddr("255.255.255.252"))
        )
    );
    QVERIFY(res=="10.0.0.128/255.255.255.252 ");

    // test specifically for #1934
    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.2"), InetAddr("255.255.255.254")),
            InetAddrMask(InetAddr("10.0.0.0"), InetAddr("255.255.255.0")))
    );
    QVERIFY(res=="10.0.0.2/255.255.255.254 ");

    res = vectorInetAddrMaskToString(
        libfwbuilder::getOverlap(
            InetAddrMask(InetAddr("10.0.0.2"), InetAddr("255.255.255.254")),
            InetAddrMask(InetAddr("0.0.0.0"), InetAddr("0.0.0.0")))
    );
    QVERIFY(res=="10.0.0.2/255.255.255.254 ");

}
