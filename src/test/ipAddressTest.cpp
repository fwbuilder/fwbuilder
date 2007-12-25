/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org
   
  $Id: ipAddressTest.cpp 918 2006-03-05 06:07:10Z vkurland $


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

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/IPAddress.h"
#include "fwbuilder/AddressRange.h"

#include <fstream>
#include <iostream>

using namespace libfwbuilder;
using namespace std;
    




void IPAddressTest(FWObjectDatabase *objdb)
{
    cout << endl;
    cout << "*** Testing IP address arithmetics" << endl;

    IPAddress *a=new IPAddress("192.168.1.127");

    cout << "created IPAddress object: " << a->toString() << endl;
    cout << "  trying address arithmetics:\n";
    cout << "  testing address increment by 1: a1+1=" << (*a+1).toString() << endl;
    cout << "  testing address decrement by 1: a1-1=" << (*a-1).toString() << endl;

    IPAddress *b=new IPAddress("192.168.1.254");
    cout << "created IPAddress object: " << b->toString() << endl;
    cout << "  testing address increments by 1:" << endl;

    *b=*b+1;
    cout << "  " <<  b->toString() << endl;
    *b=*b+1;
    cout << "  " <<  b->toString() << endl;
    *b=*b+1;
    cout << "  " <<  b->toString() << endl;

    b=new IPAddress("255.255.255.254");
    cout << "created IPAddress object: " << b->toString() << endl;

    *b=*b+1;
    cout << "  " <<  b->toString() << endl;
    *b=*b+1;
    cout << "  " <<  b->toString() << endl;
    *b=*b+1;
    cout << "  " <<  b->toString() << endl;

    Netmask  *m=new Netmask("255.255.252.0");
    cout << "created Netmask object: " << m->toString() << endl;
    cout << "  testing getLength: length of this netmask is " << m->getLength() << " bits" << endl;

    AddressRange *range1=AddressRange::cast(
        objdb->create(AddressRange::TYPENAME) );

    range1->setRangeStart( IPAddress("192.168.1.1") );
    range1->setRangeEnd(   IPAddress("192.168.1.11"));

    cout << "Created address range : " << range1->getRangeStart().toString()
         << "-" << range1->getRangeEnd().toString() << endl;

    cout << "  converting addresses to guint32\n";
    cout << "   a1=" << range1->getRangeStart().to32BitInt() << endl;
    cout << "   a2=" << range1->getRangeEnd().to32BitInt() << endl;


    cout << "   Range dimension is " << range1->dimension() << endl;

    cout << endl;

    cout << "   Testing conversion of address range to a set of networks." << endl;
    cout << "   Need two addresses that define address range start and end:" << endl;
    string s;
    cout << " Enter address range start address: ";
    cin  >> s;
    IPAddress *a1=new IPAddress(s);

    cout << " Enter address range end address: ";
    cin  >> s;
    IPAddress *a2=new IPAddress(s);

    guint32 size = htonl(a2->to32BitInt())-htonl(a1->to32BitInt()) + 1;

    cout << "Trying to convert range " 
         << a1->toString() << "-" << a2->toString()
         << " (" << size << " addresses) "
         << " to networks" << endl;

    vector<IPNetwork> vn=libfwbuilder::convertAddressRange(*a1,*a2);

    for (vector<IPNetwork>::iterator i=vn.begin(); i!=vn.end(); i++)
    {
        IPAddress bcast=i->getBroadcastAddress();

        size=ntohl(bcast.to32BitInt()) - ntohl(i->getAddress().to32BitInt()) + 1;

        cout << i->getAddress().toString()
             << "/"
             << i->getNetmask().toString()
             << endl;
        cout << "    "
             << " broadcast: "
             << bcast.toString()
             << ", "
             << size
             << " addresses"
             << endl;
    }
}

