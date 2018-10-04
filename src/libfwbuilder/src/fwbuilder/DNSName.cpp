/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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

#include <assert.h>



#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/dns.h"

#include <iostream>

using namespace libfwbuilder;
using namespace std;

const char *DNSName::TYPENAME={"DNSName"};

DNSName::DNSName() : MultiAddress() 
{
    setRunTime(false);
    setStr("dnsrec", "");
    setStr("dnsrectype", "A");
}

string DNSName::getSourceName()
{
    return getStr("dnsrec");
}

void DNSName::setSourceName(const std::string& source_name)
{
    setStr("dnsrec", source_name);
}

string DNSName::getDNSRecordType()
{
    return getStr("dnsrec");
}

void DNSName::setDNSRecordType(const string& rectype)
{
    setStr("dnsrec", rectype);
}

void DNSName::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);
    const char *n;
    
    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("dnsrec")));
    assert(n!=nullptr);
    setStr("dnsrec", n);
    XMLTools::FreeXmlBuff(n);

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("dnsrectype")));
    if (n!=nullptr)
    {
        setStr("dnsrectype", n);
        XMLTools::FreeXmlBuff(n);
    } else
    {
        setStr("dnsrectype", "A");
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("run_time")));
    assert(n!=nullptr);
    setStr("run_time", n);
    XMLTools::FreeXmlBuff(n);
}

xmlNodePtr DNSName::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    return me;
}


/*
 * take domain name from the "dnsrec" attribute and try to run DNS
 * query. If successful, create corresponding IPv4 or IPv6 object, add
 * it to the object database and add reference to it to @this. If
 * unsuccessful, create dummy object and add it to the database and
 * referece to it, then throw exception.
 *
 * TODO: new object should be added to some kind of special group in
 * the object tree, something with the name "tmp" or similar.
 */
void DNSName::loadFromSource(bool ipv6, FWOptions *options,
                             bool test_mode)
{
    (void) options; // Unused

    int af_type = (ipv6)?AF_INET6:AF_INET;
    try
    {
        list<InetAddr> v = DNS::getHostByName(getSourceName(), af_type);
        for (list<InetAddr>::iterator i=v.begin(); i!=v.end(); ++i)
        {
            //Address *a = Address::cast(
            //    getRoot()->create((ipv6)?IPv6::TYPENAME:IPv4::TYPENAME));
            int af = AF_INET;
            Address *a = nullptr;
            if (ipv6) { a = getRoot()->createIPv6(); af = AF_INET6; }
            else a = getRoot()->createIPv4();
            getRoot()->add(a);
            a->setAddress(*i);
            a->setNetmask(InetAddr::getAllOnes(af));
            addRef(a);
        }
    } catch (const FWException &ex)
    {
        // in test mode we use dummy address but still throw exception.
        // Compiler should print error message but continue.

        ostringstream err;
        string af_type_name = (ipv6)?string("AF_INET6"):string("AF_INET");

        err << "DNSName object \""
            << getName()
            << "\" (compile time) can not resolve dns name \""
            << getSourceName()
            << "\" "
            << "(" << af_type_name << ")"
            << ": "
            << ex.toString();

        if (test_mode)
        {
            err << " Using dummy address in test mode";
            int af = AF_INET;
            Address *a = nullptr;
            if (ipv6)
            {
                a = getRoot()->createIPv6();
                a->setAddress(InetAddr(af_type, "2001:db8::1"));
                af = AF_INET6;
            } else
            {
                a = getRoot()->createIPv4();
                a->setAddress("192.0.2.1");
                a->setNetmask(InetAddr::getAllOnes(af));
            }
            getRoot()->add(a);
            addRef(a);
            a->setBool(".rule_error", true);
            a->setStr(".error_msg", err.str());
        }

        throw(FWException(err.str()));
    }
}

