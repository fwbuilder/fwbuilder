/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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

#include <assert.h>



#include "fwbuilder/IPService.h"
#include "fwbuilder/XMLTools.h"

#include <iostream>

using namespace libfwbuilder;
using namespace std;

const char *IPService::TYPENAME={"IPService"};

std::map<int, std::string> IPService::named_protocols;


void IPService::initNamedProtocols()
{
    if (IPService::named_protocols.size() == 0)
    {
        IPService::named_protocols[0] = "ip";
        IPService::named_protocols[1] = "icmp";
        IPService::named_protocols[6] = "tcp";
        IPService::named_protocols[17] = "udp";
    }
}

IPService::IPService() 
{
    setStr("protocol_num", "");
    IPService::initNamedProtocols();
}

IPService::~IPService() {}

void IPService::addNamedProtocol(int proto_num, const std::string &proto_name)
{
    // Call initialize function because we might be trying to register
    // named protocol before any IPService objects have been created.
    IPService::initNamedProtocols();
    IPService::named_protocols[proto_num] = proto_name;
}

void IPService::setProtocolNumber(int n)
{
    setInt("protocol_num", n);
}

string IPService::getProtocolName()  const
{
    int proto_num = getInt("protocol_num");

    if (IPService::named_protocols.count(proto_num) > 0)
        return IPService::named_protocols[proto_num];
    else
        return getStr("protocol_num");
}

int IPService::getProtocolNumber() const
{
    return getInt("protocol_num");
}


void IPService::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);

    const char *n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("protocol_num")));
    assert(n!=nullptr);
    setStr("protocol_num", n);
    XMLTools::FreeXmlBuff(n);

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("fragm")));
    if(n!=nullptr)
    {
        setStr("fragm", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("short_fragm")));
    if(n!=nullptr)
    {
        setStr("short_fragm", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("any_opt")));
    if(n!=nullptr)
    {
        setStr("any_opt", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("lsrr")));
    if(n!=nullptr)
    {
        setStr("lsrr", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("ssrr")));
    if(n!=nullptr)
    {
        setStr("ssrr", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("rr")));
    if(n!=nullptr)
    {
        setStr("rr", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("ts")));
    if(n!=nullptr)
    {
        setStr("ts", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("tos")));
    if(n!=nullptr)
    {
        setStr("tos", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("dscp")));
    if(n!=nullptr)
    {
        setStr("dscp", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("rtralt")));
    if(n!=nullptr)
    {
        setStr("rtralt", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("rtralt_value")));
    if(n!=nullptr)
    {
        setStr("rtralt_value", n);
        XMLTools::FreeXmlBuff(n);
    }

}

xmlNodePtr IPService::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    return me;
}

bool IPService::hasIpOptions() const
{
    return (getBool("any_opt") ||
            getBool("lsrr") ||
            getBool("rr") ||
            getBool("ssrr") ||
            getBool("ts") ||
            getBool("rtralt"));
}

string IPService::getTOSCode() const
{
    return getStr("tos");
}

void IPService::setTOSCode(const string &c)
{
    setStr("tos", c);
}

string IPService::getDSCPCode() const
{
    return getStr("dscp");
}

void IPService::setDSCPCode(const string &c)
{
    setStr("dscp", c);
}








