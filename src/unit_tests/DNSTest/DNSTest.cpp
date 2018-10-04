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

#include "DNSTest.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/ObjectMatcher.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/DNSName.h"
#include <algorithm>

using namespace libfwbuilder;
using namespace std;

bool DNSTest::testDNSNameObject(FWObjectDatabase *objdb, FWObject *root,
                       const string &dnsrec,
                       char* results[])
{
    list<std::string> expected_results;
    for (char** cptr=results; *cptr!=nullptr; ++cptr)
        expected_results.push_back(*cptr);

    FWObject *nobj = objdb->create(DNSName::TYPENAME);
    if (root != nullptr)
    {
        root->add(nobj);
    }
    DNSName* dnsnameobj = DNSName::cast(nobj);

    dnsnameobj->setName(dnsrec);
    dnsnameobj->setStr("dnsrec", dnsrec);
    dnsnameobj->setRunTime(false);

    for (FWObject::iterator j=dnsnameobj->begin();
         j!=dnsnameobj->end(); ++j)
    {
        Address* addr = Address::cast(FWReference::cast(*j)->getPointer());
        const InetAddr* inet_addr = addr->getAddressPtr();

        list<std::string>::const_iterator res;

        res = std::find(expected_results.begin(),
                        expected_results.end(),
                        inet_addr->toString());

        if ( res != expected_results.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

void DNSTest::runTest()
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
    char* test1[] = {"localhost", "127.0.0.1", nullptr};
    CPPUNIT_ASSERT(testDNSNameObject(objdb, root, test1[0], &(test1[1])));

    char* test2[] = {"www.fwbuilder.org","70.85.175.170", nullptr};
    CPPUNIT_ASSERT(testDNSNameObject(objdb, root, test2[0], &(test2[1])));

    char* test3[] = {"www.microsoft.com",
                     "65.55.21.250",
                     "207.46.232.182",
                     "207.46.197.32",
                     "207.46.19.254",
                     "207.46.192.254",
                     "207.46.193.254",
                     nullptr};

    CPPUNIT_ASSERT(testDNSNameObject(objdb, root, test3[0], &(test3[1])));
}
