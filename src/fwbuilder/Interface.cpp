/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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
#include <iostream>

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/Interface.h>
#include <fwbuilder/InterfacePolicy.h>
#include <fwbuilder/XMLTools.h>
#include <fwbuilder/IPv4.h>
#include <fwbuilder/IPv6.h>
#include <fwbuilder/FWObjectDatabase.h>

using namespace std;
using namespace libfwbuilder;

const char *Interface::TYPENAME={"Interface"};

Interface::Interface(const Interface &i):Address()
{
    FWObject::operator=(i);

    bcast_bits       = i.bcast_bits       ; 
    ostatus          = i.ostatus          ;
    snmp_type        = i.snmp_type        ;
}

Interface::Interface():Address()
{
    setName("unknown");
    setBool("dyn",false);
    setBool("unnum",false);
    setBool("unprotected",false);
    setBool("bridgeport",false);
    setInt("security_level",0);

    bcast_bits       = 1    ;
    ostatus          = true ;
    snmp_type        = -1   ;     
}

Interface::Interface(const FWObject *root,bool prepopulate) :
    Address(root,prepopulate)
{
    setName("unknown");
    setBool("dyn",false);
    setBool("unnum",false);
    setBool("unprotected",false);
    setBool("bridgeport",false);
    setInt("security_level",0);

    bcast_bits       = 1    ;
    ostatus          = true ;
    snmp_type        = -1   ;     
}

Interface::~Interface() {}

FWObject& Interface::shallowDuplicate(const FWObject *o, bool preserve_id)
    throw(FWException)
{
    FWObject::shallowDuplicate(o,preserve_id);

    if (Interface::isA(o))
    {
        bcast_bits       = Interface::constcast(o)->bcast_bits       ; 
        ostatus          = Interface::constcast(o)->ostatus          ;
        snmp_type        = Interface::constcast(o)->snmp_type        ;
    }
    return *this;
}

FWObject& Interface::duplicate(const FWObject *x, bool preserve_id)
    throw(FWException)
{
    FWObject::duplicate(x, preserve_id);

    const Interface *rx = Interface::constcast(x);
    if (rx!=NULL)
    {
        bcast_bits = rx->bcast_bits;
        ostatus    = rx->ostatus;
        snmp_type  = rx->snmp_type;
    }

    return *this;
}


void Interface::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);

    const char *n;

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("security_level")));
    if (n!=NULL)
    {
        setStr("security_level",n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("dyn")));
    if (n!=NULL)
    {
        setStr("dyn",n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("unnum")));
    if (n!=NULL)
    {
        setStr("unnum",n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("unprotected")));
    if (n!=NULL)
    {
        setStr("unprotected",n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("bridgeport")));
    if (n!=NULL)
    {
        setStr("bridgeport",n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("mgmt")));
    if (n!=NULL)
    {
        setStr("mgmt",n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("label")));
    if (n!=NULL)
    {
        setStr("label",n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("network_zone")));
    if (n!=NULL)
    {
        setStr("network_zone", n);
        FREEXMLBUFF(n);
    }
}

xmlNodePtr Interface::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    FWObject *o;

    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((_getRO()) ? "True" : "False")));

    for(FWObjectTypedChildIterator j1=findByType(IPv4::TYPENAME);
        j1!=j1.end(); ++j1)
    {
        if((o=(*j1))!=NULL )
            o->toXML(me);
    }
    for(FWObjectTypedChildIterator j1=findByType(IPv6::TYPENAME);
        j1!=j1.end(); ++j1)
    {
        if((o=(*j1))!=NULL )
            o->toXML(me);
    }
    for(FWObjectTypedChildIterator j2=findByType(physAddress::TYPENAME);
        j2!=j2.end(); ++j2)
    {
        if((o=(*j2))!=NULL )
            o->toXML(me);
    }
    o=getFirstByType( InterfacePolicy::TYPENAME );
    if (o) o->toXML(me);

    return me;
}



int  Interface::getSecurityLevel() const
{
    return getInt("security_level") ;
}

void Interface::setSecurityLevel(int level)
{
    setInt("security_level",level);
}

bool Interface::isExt() const 
{ 
    return( getInt("security_level")==0 );
}

/**
 *   if parameter value is true, then security level is set to 0 (least secure,
 *   or "outside")
 *   if parameter value is false, then current security level is set to 100
 */
void Interface::setExt(bool external)
{
    setInt("security_level",(external)?0:100);
}

void Interface::setDyn(bool value) { setBool("dyn",value); }
bool Interface::isDyn() const { return(getBool("dyn")); }

void Interface::setUnnumbered(bool value) { setBool("unnum",value); }
bool Interface::isUnnumbered() const { return getBool("unnum"); }

void Interface::setUnprotected(bool value) { setBool("unprotected",value); }
bool Interface::isUnprotected() const { return getBool("unprotected"); }

void Interface::setBridgePort(bool value) { setBool("bridgeport",value); }
bool Interface::isBridgePort() const { return getBool("bridgeport"); }

void Interface::setManagement(bool value) { setBool("mgmt",value); }
bool Interface::isManagement() const { return (getBool("mgmt")); }

void Interface::setOStatus(bool value) { ostatus=value; }

void Interface::setInterfaceType(int _snmp_type) { snmp_type=_snmp_type; }

void Interface::setBroadcastBits(int _val) { bcast_bits=_val; }

bool  Interface::validateChild(FWObject *o)
{
    string otype=o->getTypeName();
    return (otype==IPv4::TYPENAME ||
            otype==IPv6::TYPENAME ||
            otype==physAddress::TYPENAME ||
            otype==InterfacePolicy::TYPENAME );
}

bool Interface::isLoopback() const
{
    const Address *iaddr = getAddressObject();
    return (iaddr && *(iaddr->getAddressPtr()) == InetAddr::getLoopbackAddr());
}

physAddress*  Interface::getPhysicalAddress () const
{
    return physAddress::cast( getFirstByType( physAddress::TYPENAME ) );
}

/*
 * per DTD, there can be 0 or 1 physAddress
 */
void  Interface::setPhysicalAddress(const std::string &paddr)
{
    physAddress *pa=getPhysicalAddress();
    if (pa!=NULL) 
    {
        pa->setPhysAddress(paddr);
    } else
    {
        pa=physAddress::cast( getRoot()->create(physAddress::TYPENAME) );
        pa->setPhysAddress(paddr);
        add(pa);
    }
}

const string& Interface::getLabel() const
{
    return getStr("label");
}

void Interface::setLabel(const string& n)
{
    setStr("label",n);
}

const Address* Interface::getAddressObject() const
{
    Address *res = Address::cast(getFirstByType(IPv4::TYPENAME));
    if (res==NULL)
        res = Address::cast(getFirstByType(IPv6::TYPENAME));
    return res;
}

IPv4*  Interface::addIPv4()
{
    IPv4* ipv4 = IPv4::cast( getRoot()->create(IPv4::TYPENAME) );
    add(ipv4);
    return ipv4;
}

IPv6*  Interface::addIPv6()
{
    IPv6* ipv6 = IPv6::cast( getRoot()->create(IPv6::TYPENAME) );
    add(ipv6);
    return ipv6;
}

