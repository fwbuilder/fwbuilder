/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

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

#include "objectMaker.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/physAddress.h"

#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/FWOptions.h"

#include "QStringListOperators.h"

// TODO: FWBTree needs to be refactored into an independent module
#include "../libgui/FWBTree.h"

#include <QStringList>

#include <set>


extern int fwbdebug;

using namespace libfwbuilder;
using namespace std;


QString ObjectSignature::toString() const
{
    QStringList sig;

    sig << type_name;

    if (type_name == IPv4::TYPENAME || type_name == IPv6::TYPENAME ||
        type_name == Network::TYPENAME || type_name == NetworkIPv6::TYPENAME)
        sig << address << netmask;

    if (type_name == AddressRange::TYPENAME)
        sig << address_range_start << address_range_end;

    if (type_name == DNSName::TYPENAME)
        sig << dns_name;

    if (type_name == AddressTable::TYPENAME)
        sig << address_table_name;

    if (type_name == CustomService::TYPENAME)
        sig << platform << code << protocol_name;

    if (type_name == ICMPService::TYPENAME || type_name == ICMP6Service::TYPENAME)
        sig << icmp_type << icmp_code;

    if (type_name == IPService::TYPENAME)
        sig << protocol << fragments;

    if (type_name == TCPService::TYPENAME)
        sig << src_port_range_start << src_port_range_end
            << dst_port_range_start << dst_port_range_end
            << established
            << flags_mask << flags_comp;

    if (type_name == UDPService::TYPENAME)
        sig << src_port_range_start << src_port_range_end
            << dst_port_range_start << dst_port_range_end;

    if (type_name == TagService::TYPENAME)
        sig << tag;

    return sig.join("|-|");
}

void* ObjectSignature::dispatch(Network *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    address = obj->getAddressPtr()->toString().c_str();
    netmask = obj->getNetmaskPtr()->toString().c_str();
    return this;
}

void* ObjectSignature::dispatch(NetworkIPv6 *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    address = obj->getAddressPtr()->toString().c_str();
    netmask = obj->getNetmaskPtr()->toString().c_str();
    return this;
}

void* ObjectSignature::dispatch(IPv4 *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    address = obj->getAddressPtr()->toString().c_str();
    netmask = InetAddr::getAllOnes().toString().c_str();
    return this;
}

void* ObjectSignature::dispatch(IPv6 *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    address = obj->getAddressPtr()->toString().c_str();
    netmask = InetAddr::getAllOnes(AF_INET6).toString().c_str();
    return this;
}

void* ObjectSignature::dispatch(AddressRange *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    address_range_start = obj->getRangeStart().toString().c_str();
    address_range_end = obj->getRangeEnd().toString().c_str();
    return this;
}

/*
 * Note that we do not track "compile time" / "run time" attribute of
 * the object because on import, only "run time" make sense
 */
void* ObjectSignature::dispatch(AddressTable *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    address_table_name = obj->getSourceName().c_str();
    return this;
}

void* ObjectSignature::dispatch(physAddress *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    address = obj->getPhysAddress().c_str();
    return this;
}

void* ObjectSignature::dispatch(IPService *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    protocol = obj->getProtocolNumber();
    fragments = obj->getBool("fragm") || obj->getBool("short_fragm");
    return this;
}

void* ObjectSignature::dispatch(ICMPService *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    icmp_type = obj->getInt("type");
    icmp_code = obj->getInt("code");
    return this;
}

void* ObjectSignature::dispatch(ICMP6Service *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    icmp_type = obj->getInt("type");
    icmp_code = obj->getInt("code");
    return this;
}

void* ObjectSignature::dispatch(TCPService *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    src_port_range_start = obj->getSrcRangeStart();
    src_port_range_end = obj->getSrcRangeEnd();
    dst_port_range_start = obj->getDstRangeStart();
    dst_port_range_end = obj->getDstRangeEnd();
    established = obj->getEstablished();

    set<TCPService::TCPFlag> flags = obj->getAllTCPFlags();
    set<TCPService::TCPFlag>::iterator it;
    for (it=flags.begin(); it!=flags.end(); ++it) flags_comp << *it;

    flags = obj->getAllTCPFlagMasks();
    for (it=flags.begin(); it!=flags.end(); ++it) flags_mask << *it;
    return this;
}

void* ObjectSignature::dispatch(UDPService *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    src_port_range_start = obj->getSrcRangeStart();
    src_port_range_end = obj->getSrcRangeEnd();
    dst_port_range_start = obj->getDstRangeStart();
    dst_port_range_end = obj->getDstRangeEnd();
    return this;
}

void* ObjectSignature::dispatch(CustomService *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    platform = "";
    code = "";
    list<string> platforms = obj->getAllKnownPlatforms();
    foreach(std::string pl, platforms)
    {
        platform += pl.c_str();
        code += obj->getCodeForPlatform(pl).c_str();
    }
    protocol_name = obj->getProtocol().c_str();
    return this;
}

void* ObjectSignature::dispatch(TagService *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    tag = obj->getStr("tagcode").c_str();
    return this;
}

/*
 * Note that we do not track "compile time" / "run time" attribute of
 * the object because on import, only "run time" make sense
 */
void* ObjectSignature::dispatch(DNSName *obj, void*)
{
    type_name = obj->getTypeName().c_str();
    dns_name = obj->getSourceName().c_str();
    return this;
}


//****************************************************************

void ObjectMaker::clear()
{
    last_created = NULL;
    object_registry.clear(); 
}

FWObject* ObjectMaker::findMatchingObject(const ObjectSignature &sig)
{
    QString signature = sig.toString();
    if (object_registry.count(signature) > 0)
        return library->getRoot()->findInIndex(object_registry[signature]);
    return NULL;
}

void ObjectMaker::registerObject(const ObjectSignature &sig, FWObject* obj)
{
    object_registry[sig.toString()] = (obj!=NULL) ? obj->getId() : -1;
}

//****************************************************************

FWObject* ObjectMaker::createObject(const std::string &objType,
                                    const std::string &objName)
{
    assert(library!=NULL);
    FWBTree tree ;
    FWObject *slot = tree.getStandardSlotForObject(library,objType.c_str());
    return createObject(slot, objType, objName);
}

FWObject* ObjectMaker::createObject(FWObject *parent,
                                    const std::string &objType,
                                    const std::string &objName)
{
    assert(library!=NULL);
    FWObject*  o = library->getRoot()->create(objType);
    if (parent != NULL)
    {
        parent->add(o);
    }
    o->setName(objName);
    return o;
}

//****************************************************************

/*
 * scan the tree starting at @root and use registerObject to build
 * signatures for all address and service objects in order to be able
 * to use them on import
 */
void ObjectMaker::prepareForDeduplication(FWObject *root)
{
    if (RuleSet::cast(root) || Rule::cast(root) ||
        FWReference::cast(root) ||
        Host::cast(root) ||
        FWOptions::cast(root)) return;

    if (Address::cast(root) || Service::cast(root))
    {
        ObjectSignature sig;
        root->dispatch(&sig, (void*)(NULL));
        registerObject(sig, root);
    }

    for (FWObject::iterator it=root->begin(); it!=root->end(); ++it)
    {
        prepareForDeduplication(*it);
    }
}
