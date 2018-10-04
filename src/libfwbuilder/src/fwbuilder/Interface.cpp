/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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



#include "fwbuilder/Interface.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/AttachedNetworks.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Resources.h"

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
    setBool("dyn", false);
    setBool("unnum", false);
    setBool("unprotected", false);
    setBool("dedicated_failover", false);
    setInt("security_level",0);

    bcast_bits       = 1    ;
    ostatus          = true ;
    snmp_type        = -1   ;     
}

Interface::~Interface() {}

/**
 * Add reference to given object. In case of an Interface this only
 * makes sense in terms of adding @obj as a network zone.
 */
void Interface::addRef(FWObject *obj)
{
    setStr("network_zone", FWObjectDatabase::getStringId(obj->getId()));
}

/**
 * Removes reference to given object among children of 'this'. In case
 * of Interface we check if @obj is used as network zone.
 */
void Interface::removeRef(FWObject *obj)
{
    string netzone_id = getStr("network_zone");
    FWObject *netzone = getRoot()->findInIndex(
        FWObjectDatabase::getIntId(netzone_id));
    if (obj == netzone) setStr("network_zone", "");
}

FWObject& Interface::shallowDuplicate(const FWObject *o, bool preserve_id)
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
{
    FWObject::duplicate(x, preserve_id);

    const Interface *rx = Interface::constcast(x);
    if (rx!=nullptr)
    {
        bcast_bits = rx->bcast_bits;
        ostatus    = rx->ostatus;
        snmp_type  = rx->snmp_type;
    }

    return *this;
}

void Interface::duplicateWithIdMapping(const FWObject *src,
                                       map<int,int> &id_mapping, bool preserve_id)
{
    assert(src->getTypeName() == Interface::TYPENAME);

    checkReadOnly();

    shallowDuplicate(src, preserve_id);

    destroyChildren();

    for(list<FWObject*>::const_iterator m=src->begin(); m!=src->end(); ++m) 
    {
        FWObject *src_obj = *m;
        FWObject *dst_obj_copy = addCopyOf(src_obj, preserve_id);
        if (src_obj!=nullptr && dst_obj_copy!=nullptr)
            id_mapping[src_obj->getId()] = dst_obj_copy->getId();
    }

    setDirty(true);
}

bool Interface::cmp(const FWObject *obj, bool recursive)
{
    const Interface *rx = Interface::constcast(obj);
    if (rx == nullptr) return false;
    if (bcast_bits != rx->bcast_bits ||
        ostatus != rx->ostatus ||
        snmp_type != rx->snmp_type) return false;
    return FWObject::cmp(obj, recursive);
}

void Interface::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);

    const char *n;

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("security_level")));
    if (n!=nullptr)
    {
        setStr("security_level",n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("dyn")));
    if (n!=nullptr)
    {
        setStr("dyn",n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("unnum")));
    if (n!=nullptr)
    {
        setStr("unnum",n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("unprotected")));
    if (n!=nullptr)
    {
        setStr("unprotected",n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("dedicated_failover")));
    if (n!=nullptr)
    {
        setStr("dedicated_failover",n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("mgmt")));
    if (n!=nullptr)
    {
        setStr("mgmt",n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("label")));
    if (n!=nullptr)
    {
        setStr("label",n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("network_zone")));
    if (n!=nullptr)
    {
        setStr("network_zone", n);
        XMLTools::FreeXmlBuff(n);
    }
}

/*
 * <!ELEMENT Interface (IPv4*, IPv6*, physAddress?, InterfaceOptions?, Interface*, FailoverClusterGroup?)>
 */
xmlNodePtr Interface::toXML(xmlNodePtr parent)
{
    // DTD prohibits empty network_zone attribute
    if (exists("network_zone") && getStr("network_zone").empty())
        remStr("network_zone");

    xmlNodePtr me = FWObject::toXML(parent, false);

    FWObject *o;

    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    for(FWObjectTypedChildIterator j1=findByType(IPv4::TYPENAME);
        j1!=j1.end(); ++j1)
    {
        if ((o=(*j1))!=nullptr )
            o->toXML(me);
    }
    for(FWObjectTypedChildIterator j1=findByType(IPv6::TYPENAME);
        j1!=j1.end(); ++j1)
    {
        if ((o=(*j1))!=nullptr )
            o->toXML(me);
    }
    for(FWObjectTypedChildIterator j2=findByType(physAddress::TYPENAME);
        j2!=j2.end(); ++j2)
    {
        if ((o=(*j2))!=nullptr )
            o->toXML(me);
    }

    o = getFirstByType(InterfaceOptions::TYPENAME);
    if (o) o->toXML(me);

    /*
     * serialize sub-interfaces (only for interfaces with advanced interface
     * config mode enabled)
     */
    for(FWObjectTypedChildIterator j1=findByType(Interface::TYPENAME);
        j1!=j1.end(); ++j1)
    {
        if((o=(*j1))!=nullptr)
            o->toXML(me);
    }

    /*
     * serialize ClusterGroup members (if any)
     */
    o = getFirstByType(FailoverClusterGroup::TYPENAME);
    if (o) o->toXML(me);

    o = getFirstByType(AttachedNetworks::TYPENAME);
    if (o) o->toXML(me);

    return me;
}

FWOptions* Interface::getOptionsObject()
{
    FWOptions *iface_opt = FWOptions::cast(getFirstByType(InterfaceOptions::TYPENAME));

    if (iface_opt == nullptr)
    {
        iface_opt = FWOptions::cast(getRoot()->create(InterfaceOptions::TYPENAME));
        add(iface_opt);

        // set default interface options
        const FWObject *parent_host = Host::getParentHost(this);
        if (parent_host != nullptr)
        {
            const string host_OS = parent_host->getStr("host_OS");
            try
            {
                Resources::setDefaultIfaceOptions(host_OS, this);
            } catch (FWException &ex)
            {
                // Resources::setDefaultIfaceOptions throws exception if it can't
                // find resources module for the given host OS.
                ;
            }
        }
    }
    return iface_opt;
}

FWOptions* Interface::getOptionsObjectConst() const
{
    FWOptions *iface_opt = FWOptions::cast(getFirstByType(InterfaceOptions::TYPENAME));
    if (iface_opt == nullptr)
        cerr << "Interface "
             << getName()
             << " ("
             << getPath()
             << ") "
             << " has no options object; late initialization failure"
             << endl;
    return iface_opt;
}

int  Interface::getSecurityLevel() const
{
    return getInt("security_level") ;
}

void Interface::setSecurityLevel(int level)
{
    setInt("security_level",level);
}

void Interface::setDyn(bool value) { setBool("dyn",value); }
bool Interface::isDyn() const { return(getBool("dyn")); }

void Interface::setUnnumbered(bool value) { setBool("unnum",value); }
bool Interface::isUnnumbered() const { return getBool("unnum"); }

void Interface::setUnprotected(bool value) { setBool("unprotected",value); }
bool Interface::isUnprotected() const
{
    return getBool("unprotected") || getBool("dedicated_failover");
}

void Interface::setDedicatedFailover(bool value) { setBool("dedicated_failover",value); }
bool Interface::isDedicatedFailover() const { return getBool("dedicated_failover"); }

void Interface::setManagement(bool value) { setBool("mgmt",value); }
bool Interface::isManagement() const { return (getBool("mgmt")); }

void Interface::setOStatus(bool value) { ostatus=value; }

void Interface::setInterfaceType(int _snmp_type) { snmp_type=_snmp_type; }

void Interface::setBroadcastBits(int _val) { bcast_bits=_val; }

bool  Interface::validateChild(FWObject *o)
{
    string otype=o->getTypeName();
    if (otype==Interface::TYPENAME)
    {
        // Interface with subinterfaces is not allowed (DTD allows only one
        // level of subinterfaces)
        if (Interface::isA(getParent())) return false;
        list<FWObject*> il = o->getByType(Interface::TYPENAME);
        return (il.size() == 0);
    }
            
    return (otype==IPv4::TYPENAME ||
            otype==IPv6::TYPENAME ||
            otype==physAddress::TYPENAME ||
            otype==InterfaceOptions::TYPENAME ||
            otype==FailoverClusterGroup::TYPENAME ||
            otype==AttachedNetworks::TYPENAME);
}

/*
 * I get options obect directly instead of calling getOptionsObject()
 * because that method tries to add options object if it is missing,
 * which means @this can not be const.
 */
bool Interface::isBridgePort() const
{
    string my_type;
    FWOptions *iface_opt = getOptionsObjectConst();
    if (iface_opt) my_type = iface_opt->getStr("type");
    Interface *parent = Interface::cast(getParent());
    return ((my_type.empty() || my_type == "ethernet") &&
            parent && parent->getOptionsObject()->getStr("type") == "bridge");
}

bool Interface::isSlave() const
{
    string my_type;
    FWOptions *iface_opt = getOptionsObjectConst();
    if (iface_opt) my_type = iface_opt->getStr("type");
    Interface *parent = Interface::cast(getParent());
    return ((my_type.empty() || my_type == "ethernet") &&
            parent && parent->getOptionsObject()->getStr("type") == "bonding");
}

bool Interface::isLoopback() const
{
    const Address *iaddr = getAddressObject();
    if (iaddr)
    {
        return (iaddr && *(iaddr->getAddressPtr()) == InetAddr::getLoopbackAddr());
    }
    /* just a little flexibility in case this is a cluster interface: it
     * should be considered loopback if corresponding member
     * interfaces are loopbacks themselves even if it has no address
     */
    if (isFailoverInterface())
    {
        FailoverClusterGroup *failover_group =
            FailoverClusterGroup::cast(
                getFirstByType(FailoverClusterGroup::TYPENAME));
        if (failover_group)
        {
            bool all_loopbacks = true;
            for (FWObjectTypedChildIterator it =
                     failover_group->findByType(FWObjectReference::TYPENAME);
                 it != it.end(); ++it)
            {
                Interface *iface = Interface::cast(FWObjectReference::getObject(*it));
                assert(iface);
                if (!iface->isLoopback()) { all_loopbacks = false; break; }
            }
            return all_loopbacks;
        }
    }
    return false;
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
    if (pa!=nullptr) 
    {
        pa->setPhysAddress(paddr);
    } else
    {
        pa = getRoot()->createphysAddress();
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
    if (res==nullptr)
        res = Address::cast(getFirstByType(IPv6::TYPENAME));
    return res;
}

IPv4*  Interface::addIPv4()
{
    IPv4* ipv4 = getRoot()->createIPv4();
    add(ipv4);
    return ipv4;
}

IPv6*  Interface::addIPv6()
{
    IPv6* ipv6 = getRoot()->createIPv6();
    add(ipv6);
    return ipv6;
}

int Interface::countInetAddresses(bool skip_loopback) const
{
    if (skip_loopback && isLoopback()) return 0;
    int res = 0;
    for(FWObjectTypedChildIterator j=findByType(IPv4::TYPENAME);
        j!=j.end(); ++j) res++;
    for(FWObjectTypedChildIterator j=findByType(IPv6::TYPENAME);
        j!=j.end(); ++j) res++;
    return res;
}

bool Interface::isFailoverInterface() const
{
    return getFirstByType(FailoverClusterGroup::TYPENAME) != nullptr;
}

void Interface::replaceReferenceInternal(int old_id, int new_id, int &counter)
{
    if (old_id == new_id) return;

    FWObject::replaceReferenceInternal(old_id, new_id, counter);

    string nzid = getStr("network_zone");
    if (!nzid.empty())
    {
        int nzid_int = FWObjectDatabase::getIntId(nzid);
        if (nzid_int == old_id)
        {
            setStr("network_zone", FWObjectDatabase::getStringId(new_id));
            counter++;
        }
    }
}

/*
 * finds all interfaces of the host (or firewall, since class Firewall
 * inherits Host) without scanning whole tree rooted at this. This is
 * more efficient than calling getByTypeDeep() when firewall has lots
 * of rules.
 */
void Interface::findAllInterfaces(FWObject *obj, list<FWObject*> &interfaces)
{
    for (FWObjectTypedChildIterator it = obj->findByType(Interface::TYPENAME);
         it != it.end(); ++it)
    {
        interfaces.push_back(*it);
        findAllInterfaces(*it, interfaces);
    }
}

