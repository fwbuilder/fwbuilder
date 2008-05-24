/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: dns_test.cpp 918 2006-03-05 06:07:10Z vkurland $

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

#include "fwbuilder/libfwbuilder-config.h"

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <deque>
#include <vector>
#include <map>

#ifndef _WIN32
#  include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#ifdef HAVE_GETOPT_H
#  include <getopt.h>
#else
#  ifdef _WIN32
#    include <getopt.h>
#  else
#    include <stdlib.h>
#  endif
#endif

#include "fwbuilder/Resources.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Inet6AddrMask.h"

#include <assert.h>

using namespace libfwbuilder;
using namespace std;


int main(int, char * const *)
{   
    string sa;

    try 
    {
        libfwbuilder::init();

        cout << endl;
        cout << "Start test for Inet6AddrMask" << endl;

        InetAddr x1(AF_INET6, 0);
        cout << "Inet6Addr::Inet6Addr(0) "
             << x1.toString() << "  length=" << x1.getLength() << endl;

        InetAddr x2(AF_INET6, 8);
        cout << "Inet6Addr::Inet6Addr(8) "
             << x2.toString() << "  length=" << x2.getLength() << endl;

        InetAddr x3(AF_INET6, 16);
        cout << "Inet6Addr::Inet6Addr(16) "
             << x3.toString() << "  length=" << x3.getLength() << endl;

        InetAddr x4(AF_INET6, 64);
        cout << "Inet6Addr::Inet6Addr(64) "
             << x4.toString() << "  length=" << x4.getLength() << endl;

        InetAddr x5(AF_INET6, 128);
        cout << "Inet6Addr::Inet6Addr(128) "
             << x5.toString() << "  length=" << x5.getLength() << endl;


        InetAddr x6(AF_INET6, "fe80::21d:9ff:fe8b:8e94");
        InetAddr y6(AF_INET6, 64);
        InetAddr z = x6 & y6;

        cout << "z=" << z.toString() << endl;
        assert(z.toString()=="fe80::");

        cout << "y6=" << y6.toString() << endl;
        cout << "~y6=" << (~y6).toString() << endl;
        assert((~y6).toString()=="::ffff:ffff:ffff:ffff");

        cout << endl;

        Inet6AddrMask *a1 = new Inet6AddrMask();
        sa = a1->getAddressPtr()->toString();
        cout << "a1=" << sa << endl;
        assert(sa=="::");
//        assert(a1->dimension()==0);


        cout << endl;

        Inet6AddrMask *a2 = new Inet6AddrMask(
            InetAddr(AF_INET6, "fe80::21d:9ff:fe8b:8e94"),
            InetAddr(AF_INET6, 128));

        sa = a2->getAddressPtr()->toString();
        cout << "a2.address=" << sa << endl;
        assert(sa=="fe80::21d:9ff:fe8b:8e94");
        sa = a2->getNetmaskPtr()->toString();
        cout << "a2.netmask=" << sa
             << " length=" << a2->getNetmaskPtr()->getLength() << endl;
        assert(sa=="ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
        assert(a2->getNetmaskPtr()->getLength()==128);

//        assert(a2->dimension()==18446744073709551616);

        cout << "a2.toString=" << a2->toString() << endl;
        assert(a2->toString()=="fe80::21d:9ff:fe8b:8e94");


        cout << endl;

        Inet6AddrMask *a3 = new Inet6AddrMask(
            InetAddr(AF_INET6, "fe80::21d:9ff:fe8b:8e94"),
            InetAddr(AF_INET6, 128));

        a3->setNetmask(InetAddr(AF_INET6, 64));
        sa = a3->getAddressPtr()->toString();
        cout << "a3.address=" << sa << endl;
        assert(sa=="fe80::21d:9ff:fe8b:8e94");
        sa = a3->getNetmaskPtr()->toString();
        cout << "a3.netmask=" << sa
             << " length=" << a3->getNetmaskPtr()->getLength() << endl;
        assert(sa=="ffff:ffff:ffff:ffff::");
        assert(a3->getNetmaskPtr()->getLength()==64);
//        assert(a3->dimension()==18446744073709551616);
        assert(a3->toString()=="fe80::21d:9ff:fe8b:8e94/64");


        cout << endl;

        Inet6AddrMask *a4 = new Inet6AddrMask(
            string("fe80::21d:9ff:fe8b:8e94/64"));
        sa = a4->getAddressPtr()->toString();
        cout << "a4.address=" << sa << endl;
        assert(sa=="fe80::21d:9ff:fe8b:8e94");
        sa = a4->getNetmaskPtr()->toString();
        cout << "a4.netmask=" << sa << endl;
        assert(sa=="ffff:ffff:ffff:ffff::");
//        assert(a4->dimension()==1);

        cout << "Checking Inet6AddrMask::belongs()" << endl;
        assert(a4->belongs( *(a2->getAddressPtr()) ));

        cout << endl;

        Inet6AddrMask *a5 = new Inet6AddrMask(*a3);
        sa = a5->getAddressPtr()->toString();
        assert(sa=="fe80::21d:9ff:fe8b:8e94");
        sa = a5->getNetmaskPtr()->toString();
        assert(sa=="ffff:ffff:ffff:ffff::");
//        assert(a5->dimension()==1);

        cout << "Checking Inet6AddrMask::setAddress()" << endl;
        a5->setAddress(InetAddr(AF_INET6, "3ffe:1200:2001:1:8000::1"));
        sa = a5->getAddressPtr()->toString();
        cout << "a5.address=" << sa << endl;
        assert(sa=="3ffe:1200:2001:1:8000::1");
        sa = a5->getNetmaskPtr()->toString();
        assert(sa=="ffff:ffff:ffff:ffff::");
//        assert(a5->dimension()==1);


        cout << "All tests passed" << endl;

    } catch(FWException &ex)  {
    cerr << ex.toString() << endl;
        exit(1);
    } catch (std::string s) {
    cerr << s;
        exit(1);
    } catch (std::exception ex) {
    cerr << ex.what();
        exit(1);
    } catch (...) {
    cerr << "Unsupported exception";
        exit(1);
    }

}

