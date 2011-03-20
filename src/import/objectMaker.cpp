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
#include "getProtoByName.h"
#include "getServByName.h"

// TODO: FWBTree needs to be refactored into an independent module
#include "../libgui/FWBTree.h"

#include <QStringList>
#include <QtDebug>

#include <set>


extern int fwbdebug;

using namespace libfwbuilder;
using namespace std;


QMap<QString, QPair<int,int> >  ObjectSignature::icmp_names;

ObjectSignature::ObjectSignature()
{
    protocol = 0;

    fragments = false;
    short_fragments = false;
    any_opt = false;
    lsrr = false;
    ssrr = false;
    rr = false;
    ts = false;
    rtralt = false;
    rtralt_value = false;
    
    icmp_type = -1;
    icmp_code = -1;

    src_port_range_start = 0;
    src_port_range_end = 0;
    dst_port_range_start = 0;
    dst_port_range_end = 0;
    
    established = false;
}

ObjectSignature::ObjectSignature(const ObjectSignature &other)
{
    type_name = other.type_name;
    object_name = other.object_name;
    address = other.address;
    netmask = other.netmask;
    address_range_start = other.address_range_start;
    address_range_end = other.address_range_end;
    dns_name = other.dns_name;
    address_table_name = other.address_table_name;
    protocol = other.protocol;
    fragments = other.fragments;
    short_fragments = other.short_fragments;
    any_opt = other.any_opt;
    dscp = other.dscp;
    tos = other.tos;
    lsrr = other.lsrr;
    ssrr = other.ssrr;
    rr = other.rr;
    ts = other.ts;
    rtralt = other.rtralt;
    rtralt_value = other.rtralt_value;
    icmp_type = other.icmp_type;
    icmp_code = other.icmp_code;
    src_port_range_start = other.src_port_range_start;
    src_port_range_end = other.src_port_range_end;
    dst_port_range_start = other.dst_port_range_start;
    dst_port_range_end = other.dst_port_range_end;
    established = other.established;
    flags_mask = other.flags_mask;
    flags_comp = other.flags_comp;
    platform = other.platform;
    protocol_name = other.protocol_name;
    code = other.code;
    tag = other.tag;

    if (icmp_names.size() == 0)
    {
        // Cisco icmp names
        icmp_names["echo-reply"] = QPair<int,int>(0, 0);
        icmp_names["unreachable"] = QPair<int,int>(3, -1); // all "unreachables"
        icmp_names["net-unreachable"] = QPair<int,int>(3, 0);
        icmp_names["host-unreachable"] = QPair<int,int>(3, 1);
        icmp_names["protocol-unreachable"] = QPair<int,int>(3, 2);
        icmp_names["port-unreachable"] = QPair<int,int>(3, 3);
        icmp_names["packet-too-big"] = QPair<int,int>(3, 4);
        icmp_names["source-route-failed"] = QPair<int,int>(3, 5);
        icmp_names["network-unknown"] = QPair<int,int>(3, 6);
        icmp_names["host-unknown"] = QPair<int,int>(3, 7);
        icmp_names["host-isolated"] = QPair<int,int>(3, 8);
        icmp_names["dod-net-prohibited"] = QPair<int,int>(3, 9);
        icmp_names["dod-host-prohibited"] = QPair<int,int>(3, 10);
        icmp_names["net-tos-unreachable"] = QPair<int,int>(3, 11);
        icmp_names["host-tos-unreachable"] = QPair<int,int>(3, 12);
        icmp_names["administratively-prohibited"] = QPair<int,int>(3, 13);
        icmp_names["host-precedence-unreachable"] = QPair<int,int>(3, 14);
        icmp_names["precedence-unreachable"] = QPair<int,int>(3, 15);
        icmp_names["source-quench"] = QPair<int,int>(4, 0);
        icmp_names["net-redirect"] = QPair<int,int>(5, 0);
        icmp_names["host-redirect"] = QPair<int,int>(5, 1);
        icmp_names["net-tos-redirect"] = QPair<int,int>(5, 2);
        icmp_names["host-tos-redirect"] = QPair<int,int>(5, 3);
        icmp_names["echo"] = QPair<int,int>(8, 0);
        icmp_names["router-advertisement"] = QPair<int,int>(9, 0);
        icmp_names["router-solicitation"] = QPair<int,int>(10, 0);
        icmp_names["ttl-exceeded"] = QPair<int,int>(11, 0);
        icmp_names["reassembly-timeout"] = QPair<int,int>(11, 1);
        icmp_names["general-parameter-problem"] = QPair<int,int>(12, 0);
        icmp_names["option-missing"] = QPair<int,int>(12, 1);
        icmp_names["timestamp-request"] = QPair<int,int>(13, 0);
        icmp_names["timestamp-reply"] = QPair<int,int>(14, 0);
        icmp_names["information-request"] = QPair<int,int>(15, 0);
        icmp_names["information-reply"] = QPair<int,int>(16, 0);
        icmp_names["mask-request"] = QPair<int,int>(17, 0);
        icmp_names["mask-reply"] = QPair<int,int>(18, 0);

        // ASA 8.3 icmp names
        icmp_names["alternate-address"] = QPair<int,int>(6, 0);
        icmp_names["conversion-error"] = QPair<int,int>(31, 0);
        icmp_names["echo"] = QPair<int,int>(8, 0);
        icmp_names["echo-reply"] = QPair<int,int>(0, 0);
        icmp_names["information-reply"] = QPair<int,int>(16, 0);
        icmp_names["information-request"] = QPair<int,int>(15, 0);
        icmp_names["mask-reply"] = QPair<int,int>(18, 0);
        icmp_names["mask-request"] = QPair<int,int>(17, 0);
        icmp_names["mobile-redirect"] = QPair<int,int>(32, 0);
        icmp_names["parameter-problem"] = QPair<int,int>(12, 0);
        icmp_names["redirect"] = QPair<int,int>(5, 0);
        icmp_names["router-advertisement"] = QPair<int,int>(9, 0);
        icmp_names["router-solicitation"] = QPair<int,int>(10, 0);
        icmp_names["source-quench"] = QPair<int,int>(4, 0);
        icmp_names["time-exceeded"] = QPair<int,int>(11, 0);
        icmp_names["timestamp-reply"] = QPair<int,int>(14, 0);
        icmp_names["timestamp-request"] = QPair<int,int>(13, 0);
        icmp_names["traceroute"] = QPair<int,int>(30, 0);
        icmp_names["unreachable"] = QPair<int,int>(3, 0);

        // iptables icmp names
        icmp_names["any"] = QPair<int,int>(-1,-1);
        icmp_names["echo-reply"] = QPair<int,int>(0,0);
        // all "unreachables"
        icmp_names["destination-unreachable"] = QPair<int,int>(3,-1);
        icmp_names["network-unreachable"] = QPair<int,int>(3,0);
        icmp_names["host-unreachable"] = QPair<int,int>(3,1);
        icmp_names["protocol-unreachable"] = QPair<int,int>(3,2);
        icmp_names["port-unreachable"] = QPair<int,int>(3,3);
        icmp_names["fragmentation-needed"] = QPair<int,int>(3,4);
        icmp_names["source-route-failed"] = QPair<int,int>(3,5);
        icmp_names["network-unknown"] = QPair<int,int>(3,6);
        icmp_names["host-unknown"] = QPair<int,int>(3,7);
        icmp_names["host-isolated"] = QPair<int,int>(3,8);
        icmp_names["network-prohibited"] = QPair<int,int>(3,9);
        icmp_names["host-prohibited"] = QPair<int,int>(3,10);
        icmp_names["TOS-network-unreachable"] = QPair<int,int>(3,11);
        icmp_names["TOS-host-unreachable"] = QPair<int,int>(3,12);
        icmp_names["communication-prohibited"] = QPair<int,int>(3,13);
        icmp_names["host-precedence-violation"] = QPair<int,int>(3,14);
        icmp_names["precedence-cutoff"] = QPair<int,int>(3,15);
        icmp_names["source-quench"] = QPair<int,int>(4,0);
        icmp_names["redirect"] = QPair<int,int>(5,-1);
        icmp_names["network-redirect"] = QPair<int,int>(5,0);
        icmp_names["host-redirect"] = QPair<int,int>(5,1);
        icmp_names["TOS-network-redirect"] = QPair<int,int>(5,2);
        icmp_names["TOS-host-redirect"] = QPair<int,int>(5,3);
        icmp_names["echo-request"] = QPair<int,int>(8,0);
        icmp_names["router-advertisement"] = QPair<int,int>(9,0);
        icmp_names["router-solicitation"] = QPair<int,int>(10,0);
        icmp_names["ttl-exceeded"] = QPair<int,int>(11,0);
        icmp_names["time-exceeded"] = QPair<int,int>(11,0);
        icmp_names["ttl-zero-during-transit"] = QPair<int,int>(11,0);
        icmp_names["ttl-zero-during-reassembly"] = QPair<int,int>(11,1);
        icmp_names["parameter-problem"] = QPair<int,int>(12,0);
        icmp_names["ip-header-bad"] = QPair<int,int>(12,0);
        icmp_names["required-option-missing"] = QPair<int,int>(12,1);
        icmp_names["timestamp-request"] = QPair<int,int>(13,0);
        icmp_names["timestamp-reply"] = QPair<int,int>(14,0);
        icmp_names["information-request"] = QPair<int,int>(15,0);
        icmp_names["information-reply"] = QPair<int,int>(16,0);
        icmp_names["address-mask-request"] = QPair<int,int>(17,0);
        icmp_names["address-mask-reply"] = QPair<int,int>(18,0);

    }
}

void ObjectSignature::setProtocol(const QString &s)
{
    // this assumes protocol is represented by a number
    bool ok = false;
    protocol = QString(s).toInt(&ok);
    if ( ! ok)
    {
        // could not convert protocol number

        protocol = GetProtoByName::getProtocolByName(s);
        if (protocol == -1)
        {
            protocol = 0;
            throw ObjectMakerException(QString("Protocol '%1' is unknown").arg(s));
        }
    }
}

void ObjectSignature::setIcmpFromName(const QString &s)
{
    if (icmp_names.count(s) > 0)
    {
        QPair<int,int> p = icmp_names[s];
        icmp_type = p.first;
        icmp_code = p.second;
    } else
        throw ObjectMakerException(
            QString("ICMP service name '%1' is unknown").arg(s));
}

void ObjectSignature::setIcmpType(const QString &s)
{
    if (s.isEmpty()) icmp_type = -1;
    else
    {
        bool ok = false;
        icmp_type = s.toInt(&ok);
        if (!ok)
        {
            // could not convert
            icmp_type = -1;
            throw ObjectMakerException(QString("ICMP type '%1' is unusable").arg(s));
        }
    }
}

void ObjectSignature::setIcmpCode(const QString &s)
{
    if (s.isEmpty()) icmp_code = -1;
    else
    {
        bool ok = false;
        icmp_code = s.toInt(&ok);
        if (!ok)
        {
            // could not convert
            icmp_code = -1;
            throw ObjectMakerException(QString("ICMP code '%1' is unusable").arg(s));
        }
    }
}

int ObjectSignature::portFromString(const QString &port_spec, const QString &proto,
                                    int default_port)
{
    QString ps = port_spec.trimmed();
    if (ps == "") return 0;
    if (ps == ":") return default_port;

    int port = GetServByName::getPortByName(ps, proto);
    if (port == -1)
    {
        throw ObjectMakerException(QString("Port name '%1' is unknown").arg(ps));
        port = 0;
    }
    return port;
}
 
void ObjectSignature::setSrcPortRange(const QString &range_start_spec,
                                      const QString &range_end_spec,
                                      const QString &proto)
{
    src_port_range_start = portFromString(range_start_spec, proto, 0);
    src_port_range_end = portFromString(range_end_spec, proto, 65535);
}

void ObjectSignature::setDstPortRange(const QString &range_start_spec,
                                      const QString &range_end_spec,
                                      const QString &proto)
{
    dst_port_range_start = portFromString(range_start_spec, proto, 0);
    dst_port_range_end = portFromString(range_end_spec, proto, 65535);
}

/*
 * Sets source port range from cisco-like port operation:
 *
 *   eq www
 *   gt smtp
 *   lt 1024
 *   range 10000 10010
 *
 *   @port_op is operation ("lt", "gt", "eq", "range")
 *   @port_spec is port number of service name
 *   @proto is protocol name used for GetServByName::GetPortByName()
 */
void ObjectSignature::setSrcPortRangeFromPortOp(const QString &port_op,
                                                const QString &port_spec,
                                                const QString &proto)
{
    QString portop = port_op.trimmed();
    QString portspec = port_spec.trimmed();

    src_port_range_start = 0;
    src_port_range_end = 0;

    QString range_start;
    QString range_end;
    QStringList sl = portspec.split(" ");
    if (sl.size() > 1)
    {
        range_start = sl[0];
        range_end = sl[1];
    } else
    {
        range_start = portspec;
        range_end = portspec;
    }

    src_port_range_start = portFromString(range_start, proto, 0);
    src_port_range_end = portFromString(range_end, proto, 65535);

    if (portop == "lt") src_port_range_start = 0;
    if (portop=="gt")  src_port_range_end = 65535;
    if (portop=="eq")
    {
        src_port_range_start = src_port_range_end;
    }
    if (portop=="range")
    {
        // range_start and range_end have been set
        ;
    }
}

void ObjectSignature::setDstPortRangeFromPortOp(const QString &port_op,
                                                const QString &port_spec,
                                                const QString &proto)
{
    QString portop = port_op.trimmed();
    QString portspec = port_spec.trimmed();

    dst_port_range_start = 0;
    dst_port_range_end = 0;

    QString range_start;
    QString range_end;
    QStringList sl = portspec.split(" ");
    if (sl.size() > 1)
    {
        range_start = sl[0];
        range_end = sl[1];
    } else
    {
        range_start = portspec;
        range_end = portspec;
    }

    dst_port_range_start = portFromString(range_start, proto, 0);
    dst_port_range_end = portFromString(range_end, proto, 65535);

    if (portop == "lt") dst_port_range_start = 0;
    if (portop=="gt")  dst_port_range_end = 65535;
    if (portop=="eq")
    {
        dst_port_range_start = dst_port_range_end;
    }
    if (portop=="range")
    {
        // range_start and range_end have been set
        ;
    }
}

QString ObjectSignature::ObjectSignature::toString() const
{
    QStringList sig;

    sig << type_name;

    if ( ! object_name.isEmpty()) sig << object_name;

    if (type_name == IPv4::TYPENAME || type_name == IPv6::TYPENAME ||
        type_name == Network::TYPENAME || type_name == NetworkIPv6::TYPENAME ||
        type_name == Address::TYPENAME)
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
        sig << protocol
            << fragments
            << short_fragments
            << any_opt
            << dscp
            << tos
            << lsrr
            << ssrr
            << rr
            << ts
            << rtralt
            << rtralt_value;

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

    return sig.join("| |");
}

void* ObjectSignature::dispatch(Network *obj, void*)
{
    object_name = QString::fromUtf8(obj->getName().c_str());
    type_name = obj->getTypeName().c_str();
    address = obj->getAddressPtr()->toString().c_str();
    netmask = obj->getNetmaskPtr()->toString().c_str();
    return this;
}

void* ObjectSignature::dispatch(NetworkIPv6 *obj, void*)
{
    object_name = QString::fromUtf8(obj->getName().c_str());
    type_name = obj->getTypeName().c_str();
    address = obj->getAddressPtr()->toString().c_str();
    netmask = obj->getNetmaskPtr()->toString().c_str();
    return this;
}

void* ObjectSignature::dispatch(IPv4 *obj, void*)
{
    object_name = QString::fromUtf8(obj->getName().c_str());
    type_name = obj->getTypeName().c_str();
    address = obj->getAddressPtr()->toString().c_str();
    netmask = InetAddr::getAllOnes().toString().c_str();
    return this;
}

void* ObjectSignature::dispatch(IPv6 *obj, void*)
{
    object_name = QString::fromUtf8(obj->getName().c_str());
    type_name = obj->getTypeName().c_str();
    address = obj->getAddressPtr()->toString().c_str();
    netmask = InetAddr::getAllOnes(AF_INET6).toString().c_str();
    return this;
}

void* ObjectSignature::dispatch(AddressRange *obj, void*)
{
    object_name = QString::fromUtf8(obj->getName().c_str());
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
    object_name = QString::fromUtf8(obj->getName().c_str());
    type_name = obj->getTypeName().c_str();
    address_table_name = obj->getSourceName().c_str();
    return this;
}

void* ObjectSignature::dispatch(physAddress *obj, void*)
{
    object_name = QString::fromUtf8(obj->getName().c_str());
    type_name = obj->getTypeName().c_str();
    address = obj->getPhysAddress().c_str();
    return this;
}

void* ObjectSignature::dispatch(IPService *obj, void*)
{
    object_name = QString::fromUtf8(obj->getName().c_str());
    type_name = obj->getTypeName().c_str();
    protocol = obj->getProtocolNumber();
    fragments = obj->getBool("fragm");
    short_fragments = obj->getBool("short_fragm");
    any_opt = obj->getBool("any_opt");
    dscp = obj->getStr("dscp").c_str();
    tos = obj->getStr("tos").c_str();
    lsrr = obj->getBool("lsrr");
    ssrr = obj->getBool("ssrr");
    rr = obj->getBool("rr");
    ts = obj->getBool("ts");
    rtralt = obj->getBool("rtralt");
    rtralt_value = obj->getBool("rtralt_value");
    return this;
}

void* ObjectSignature::dispatch(ICMPService *obj, void*)
{
    object_name = QString::fromUtf8(obj->getName().c_str());
    type_name = obj->getTypeName().c_str();
    icmp_type = obj->getInt("type");
    icmp_code = obj->getInt("code");
    return this;
}

void* ObjectSignature::dispatch(ICMP6Service *obj, void*)
{
    object_name = QString::fromUtf8(obj->getName().c_str());
    type_name = obj->getTypeName().c_str();
    icmp_type = obj->getInt("type");
    icmp_code = obj->getInt("code");
    return this;
}

void* ObjectSignature::dispatch(TCPService *obj, void*)
{
    object_name = QString::fromUtf8(obj->getName().c_str());
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
    object_name = QString::fromUtf8(obj->getName().c_str());
    type_name = obj->getTypeName().c_str();
    src_port_range_start = obj->getSrcRangeStart();
    src_port_range_end = obj->getSrcRangeEnd();
    dst_port_range_start = obj->getDstRangeStart();
    dst_port_range_end = obj->getDstRangeEnd();
    return this;
}

void* ObjectSignature::dispatch(CustomService *obj, void*)
{
    object_name = QString::fromUtf8(obj->getName().c_str());
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
    object_name = QString::fromUtf8(obj->getName().c_str());
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
    object_name = QString::fromUtf8(obj->getName().c_str());
    type_name = obj->getTypeName().c_str();
    dns_name = obj->getSourceName().c_str();
    return this;
}


//****************************************************************

void ObjectMaker::clear()
{
    last_created = NULL;
    named_object_registry.clear(); 
    anon_object_registry.clear(); 
}

FWObject* ObjectMaker::findMatchingObject(const ObjectSignature &sig)
{
    QString signature = sig.toString();
    if (named_object_registry.count(signature) > 0)
        return library->getRoot()->findInIndex(named_object_registry[signature]);

    ObjectSignature anon_sig = sig;
    anon_sig.object_name = "";
    QString anon_signature = anon_sig.toString();
    if (anon_object_registry.count(anon_signature) > 0)
        return library->getRoot()->findInIndex(anon_object_registry[anon_signature]);

    return NULL;
}

void ObjectMaker::registerNamedObject(const ObjectSignature &sig, FWObject* obj)
{
    ObjectSignature anon_sig = sig;
    anon_sig.object_name = "";
    QString as = anon_sig.toString();
    if (anon_object_registry.count(as) > 0) anon_object_registry.remove(as);

    named_object_registry[sig.toString()] = (obj!=NULL) ? obj->getId() : -1;
}

void ObjectMaker::registerAnonymousObject(const ObjectSignature &sig, FWObject* obj)
{
    ObjectSignature anon_sig = sig;
    anon_sig.object_name = "";
    anon_object_registry[anon_sig.toString()] = (obj!=NULL) ? obj->getId() : -1;
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
        if (parent->isReadOnly())
        {     
            QString pn = QString::fromUtf8(parent->getName().c_str());
            if (parent->getLibrary()->isReadOnly())
                throw ObjectMakerException(
                    QObject::tr("Can not add new objects to folder %1 because "
                                "it belongs to a locked library").arg(pn));
            else
                throw ObjectMakerException(
                    QObject::tr("Can not add new objects to folder %1 because "
                                "it is locked").arg(pn));
        }
        parent->add(o);
    }
    o->setName(objName);
    return o;
}

FWObject* ObjectMaker::createObject(ObjectSignature &)
{
    return NULL;
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

        registerNamedObject(sig, root);
        registerAnonymousObject(sig, root); // this erases sig.object_name
    }

    if (Library::isA(root) && root->getId() == FWObjectDatabase::DELETED_OBJECTS_ID)
        return;

    for (FWObject::iterator it=root->begin(); it!=root->end(); ++it)
    {
        prepareForDeduplication(*it);
    }
}
