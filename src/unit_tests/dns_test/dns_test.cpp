/* 

                          Firewall Builder

                 Copyright (C) 2005 NetCitadel, LLC

  Author:  Illiya Yalovoy     yalovoy@gmail.com

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
#include "fwbuilder/Group.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/ObjectGroup.h"

#include <assert.h>

using namespace libfwbuilder;
using namespace std;


FWObjectDatabase       *objdb = NULL;


bool testDNSNameObject(FWObjectDatabase *objdb, FWObject *root,
                       const string &dnsrec,
                       char* results[])
{
    list<std::string> expected_results;
    for (char** cptr=results; *cptr!=NULL; ++cptr)
        expected_results.push_back(*cptr);

    FWObject *nobj = objdb->create(DNSName::TYPENAME);
    if (root != NULL)
    {
        root->add(nobj);    
    }
    DNSName* dnsnameobj = DNSName::cast(nobj);
            
    dnsnameobj->setName(dnsrec);
    dnsnameobj->setStr("dnsrec", dnsrec);
    dnsnameobj->setRunTime(false);

    bool passed = true;
    try
    {
        cout << endl << dnsnameobj->getName()
             << " ( " << dnsnameobj->getId() <<  " ) " << endl;

        dnsnameobj->loadFromSource();

        for (FWObject::iterator j=dnsnameobj->begin();
             j!=dnsnameobj->end(); ++j)
        {
            Address* addr = Address::cast(FWReference::cast(*j)->getPointer());
            const InetAddr* inet_addr = addr->getAddressPtr();

            cout << inet_addr->toString() << " : "; 

            list<std::string>::const_iterator res;

            res = std::find(expected_results.begin(),
                            expected_results.end(),
                            inet_addr->toString());

            if ( res != expected_results.end())
            {
                cout << "Passed." << endl;
            }
            else
            {
                cout << "Failed." << endl;
                passed = false;
            }
        }
    }catch(FWException &ex)  
    {
        cerr << ex.toString() << endl;
    }
    return passed;
}

int main(int, char * const *)
{   

    try 
    {
        libfwbuilder::init();

        objdb = new FWObjectDatabase();

    

        FWObject *nlib = objdb->create(Library::TYPENAME);
        objdb->add(nlib);
        nlib->setName( "Library" );

        FWObject *o1 = objdb->create(ObjectGroup::TYPENAME);
        o1->setName("Objects");
        nlib->add(o1);
            
        FWObject *root = objdb->create(ObjectGroup::TYPENAME);
        root->setName("DNS Names");
        o1->add(root);
            
        InetAddr addr;
            
        cout << "Start test for DNS Names :" << endl;

        char* test1[] = {"localhost", "127.0.0.1", NULL};
        testDNSNameObject(objdb, root, test1[0], &(test1[1]));

        char* test2[] = {"www.fwbuilder.org","69.56.183.146", NULL};
        testDNSNameObject(objdb, root, test2[0], &(test2[1]));
        
        char* test3[] = {"www.microsoft.com",
                         "207.46.19.254",
                         "207.46.192.254",
                         "207.46.193.254",
                         NULL};
        testDNSNameObject(objdb, root, test3[0], &(test3[1]));
       


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

