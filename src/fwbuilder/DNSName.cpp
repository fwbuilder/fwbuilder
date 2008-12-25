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


void DNSName::loadFromSource(bool ipv6) throw(FWException)
{
    int af_type = (ipv6)?AF_INET6:AF_INET;
    try
    {
        list<InetAddr> v = DNS::getHostByName(getSourceName(), af_type);
        for (list<InetAddr>::iterator i=v.begin(); i!=v.end(); ++i)
        {
            //Address *a = Address::cast(
            //    getRoot()->create((ipv6)?IPv6::TYPENAME:IPv4::TYPENAME));
            Address *a = NULL;
            if (ipv6) a = getRoot()->createIPv6();
            else a = getRoot()->createIPv4();
            a->setAddress( *i );
            addRef(a);
        }
    } catch (const FWException &ex)
    {
        ostringstream err;
        string af_type_name = (ipv6)?string("AF_INET6"):string("AF_INET");

        err << "DNSName object \""
            << getName()
            << "\" (compile time) can not resole dns name \""
            << getSourceName()
            << "\" "
            << "(" << af_type_name << ")"
            << ": "
            << ex.toString()
            << endl;
        throw(FWException(err.str()));
    }
}

