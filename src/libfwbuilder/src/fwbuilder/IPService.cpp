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

#include "config.h"
#include "fwbuilder/libfwbuilder-config.h"


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


void IPService::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);

    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("protocol_num")));
    assert(n!=NULL);
    setStr("protocol_num", n);
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("fragm")));
    if(n!=NULL)
    {
        setStr("fragm", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("short_fragm")));
    if(n!=NULL)
    {
        setStr("short_fragm", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("any_opt")));
    if(n!=NULL)
    {
        setStr("any_opt", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("lsrr")));
    if(n!=NULL)
    {
        setStr("lsrr", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("ssrr")));
    if(n!=NULL)
    {
        setStr("ssrr", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("rr")));
    if(n!=NULL)
    {
        setStr("rr", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("ts")));
    if(n!=NULL)
    {
        setStr("ts", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("tos")));
    if(n!=NULL)
    {
        setStr("tos", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("dscp")));
    if(n!=NULL)
    {
        setStr("dscp", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("rtralt")));
    if(n!=NULL)
    {
        setStr("rtralt", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("rtralt_value")));
    if(n!=NULL)
    {
        setStr("rtralt_value", n);
        FREEXMLBUFF(n);
    }

}

xmlNodePtr IPService::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));

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








