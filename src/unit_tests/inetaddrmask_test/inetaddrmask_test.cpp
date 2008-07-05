/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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
#include "fwbuilder/InetAddrMask.h"

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
        cout << "Start test for InetAddrMask" << endl;

        InetAddrMask *a1 = new InetAddrMask();
        sa = a1->getAddressPtr()->toString();
        assert(sa=="0.0.0.0");
        assert(a1->dimension()==0);

        InetAddrMask *a2 = new InetAddrMask(InetAddr("1.1.1.1"), InetAddr("255.255.255.0"));
        sa = a2->getAddressPtr()->toString();
        assert(sa=="1.1.1.0");
        sa = a2->getNetmaskPtr()->toString();
        assert(sa=="255.255.255.0");
        assert(a2->dimension()==256);
        assert(a2->toString()=="1.1.1.0/255.255.255.0");

        InetAddrMask *a3 = new InetAddrMask(string("1.1.1.1"));
        sa = a3->getAddressPtr()->toString();
        assert(sa=="1.1.1.1");
        sa = a3->getNetmaskPtr()->toString();
        assert(sa=="255.255.255.255");
        assert(a3->dimension()==1);

        assert(a2->belongs( *(a3->getAddressPtr()) ));


        InetAddrMask *a4 = new InetAddrMask(*a3);
        sa = a4->getAddressPtr()->toString();
        assert(sa=="1.1.1.1");
        sa = a4->getNetmaskPtr()->toString();
        assert(sa=="255.255.255.255");
        assert(a4->dimension()==1);

        a4->setAddress(InetAddr("2.2.2.2"));
        sa = a4->getAddressPtr()->toString();
        assert(sa=="2.2.2.2");
        sa = a4->getNetmaskPtr()->toString();
        assert(sa=="255.255.255.255");
        assert(a4->dimension()==1);

        a4->setNetmask(InetAddr("255.255.0.0"));
        sa = a4->getAddressPtr()->toString();
        assert(sa=="2.2.2.2");
        sa = a4->getNetmaskPtr()->toString();
        assert(sa=="255.255.0.0");
        assert(a4->dimension()==256*256);

        a4->setNetmask(InetAddr("8"));
        sa = a4->getAddressPtr()->toString();
        assert(sa=="2.2.2.2");
        sa = a4->getNetmaskPtr()->toString();
        assert(sa=="255.0.0.0");
        assert(a4->dimension()==256*256*256);

        try 
        {
            a4->setNetmask(InetAddr("40"));
        } catch(FWException &ex)  {
            cerr << "Expected exception: " << ex.toString() << endl;
        }
        // a4 should not have changed
        sa = a4->getAddressPtr()->toString();
        assert(sa=="2.2.2.2");
        sa = a4->getNetmaskPtr()->toString();
        assert(sa=="255.0.0.0");
        assert(a4->dimension()==256*256*256);

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

