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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/DNSName.h>
#include <fwbuilder/FWException.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/InetAddr.h>
#include <fwbuilder/IPv4.h>
#include <fwbuilder/IPv6.h>
#include <fwbuilder/dns.h>

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

DNSName::DNSName(const FWObjectDatabase *root,bool prepopulate) :
    MultiAddress(root,prepopulate) 
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

void DNSName::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);
    const char *n;
    
    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("dnsrec")));
    assert(n!=NULL);
    setStr("dnsrec", n);
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("dnsrectype")));
    if (n!=NULL)
    {
        setStr("dnsrectype", n);
        FREEXMLBUFF(n);
    } else
    {
        setStr("dnsrectype", "A");
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("run_time")));
    assert(n!=NULL);
    setStr("run_time", n);
    FREEXMLBUFF(n);
}

xmlNodePtr DNSName::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));

    return me;
}


void DNSName::loadFromSource(bool ipv6, bool test_mode) throw(FWException)
{
    int af_type = (ipv6)?AF_INET6:AF_INET;
    try
    {
        list<InetAddr> v = DNS::getHostByName(getSourceName(), af_type);
        for (list<InetAddr>::iterator i=v.begin(); i!=v.end(); ++i)
        {
            //Address *a = Address::cast(
            //    getRoot()->create((ipv6)?IPv6::TYPENAME:IPv4::TYPENAME));
            int af = AF_INET;
            Address *a = NULL;
            if (ipv6) { a = getRoot()->createIPv6(); af = AF_INET6; }
            else a = getRoot()->createIPv4();
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
            Address *a = NULL;
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
            addRef(a);
            a->setBool(".rule_error", true);
            a->setStr(".error_msg", err.str());
        }

        throw(FWException(err.str()));
    }
}

