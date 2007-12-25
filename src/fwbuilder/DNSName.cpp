/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

  $Id: DNSName.cpp 987 2006-11-06 06:20:40Z vkurland $

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

#include <assert.h>

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/DNSName.h>
#include <fwbuilder/FWException.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/IPAddress.h>
#include <fwbuilder/IPv4.h>
#include <fwbuilder/dns.h>

#include <iostream>

using namespace libfwbuilder;
using namespace std;

const char *DNSName::TYPENAME={"DNSName"};

DNSName::DNSName() : MultiAddress() 
{
    registerSourceAttributeName("dnsrec");
    setSourceName("localhost");
    setRunTime(false);
}

DNSName::DNSName(const FWObject *root,bool prepopulate) :
    MultiAddress(root,prepopulate) 
{
    registerSourceAttributeName("dnsrec");
    setSourceName("localhost");
    setRunTime(false);
}


void DNSName::loadFromSource() throw(FWException)
{
    try
    {
        list<IPAddress> v = DNS::getHostByName(getSourceName() );
        for (list<IPAddress>::iterator i=v.begin(); i!=v.end(); ++i)
        {
            IPv4 *a = IPv4::cast(getRoot()->create(IPv4::TYPENAME));
            a->setAddress( i->toString() );
            addRef(a);
        }
    } catch (const FWException &ex)
    {
        cerr << "Error resolving dns name " << getSourceName() << ": '"
             << ex.toString() << "'" << endl;
    }
}

