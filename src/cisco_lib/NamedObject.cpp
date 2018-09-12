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


#include "NamedObject.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Interface.h"

#include <assert.h>
#include <sstream>

#include <QStringList>
#include <QSet>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

/*
 * Reserved words for all versions of IOS and ASA that support named
 * objects. It does not make sense to maintain sets of reserved words
 * separately for each version because it would take a lot of effort
 * for very little gain. We will maintain super-set of words that
 * corresponds to the version that has most extensive set.
 */
const char* rw[] = {
    "ah",
    "eigrp",
    "esp",
    "gre",
    "icmp",
    "icmp6",
    "igmp",
    "igrp",
    "ip",
    "ipinip",
    "ipsec",
    "nos",
    "ospf",
    "pcp",
    "pim",
    "pptp",
    "snp",
    "tcp",
    "udp",
    "tcp-aol",
    "tcp-bgp",
    "tcp-chargen",
    "tcp-cifs",
    "tcp-citrix-ica",
    "tcp-ctiqbe",
    "tcp-daytime",
    "tcp-discard",
    "tcp-domain",
    "tcp-echo",
    "tcp-exec",
    "tcp-finger",
    "tcp-ftp",
    "tcp-ftp-data",
    "tcp-gopher",
    "tcp-ident",
    "tcp-imap4",
    "tcp-irc",
    "tcp-hostname",
    "tcp-kerberos",
    "tcp-klogin",
    "tcp-kshell",
    "tcp-ldap",
    "tcp-ldaps",
    "tcp-login",
    "tcp-lotusnotes",
    "tcp-nfs",
    "tcp-netbios-ssn",
    "tcp-whois",
    "tcp-nntp",
    "tcp-pcanywhere-data",
    "tcp-pim-auto-rp",
    "tcp-pop2",
    "tcp-pop3",
    "tcp-pptp",
    "tcp-lpd",
    "tcp-rsh",
    "tcp-rtsp",
    "tcp-sip",
    "tcp-smtp",
    "tcp-ssh",
    "tcp-sunrpc",
    "tcp-tacacs",
    "tcp-talk",
    "tcp-telnet",
    "tcp-uucp",
    "tcp-www",
    "tcp-http",
    "tcp-https",
    "tcp-cmd",
    "tcp-sqlnet",
    "tcp-h323",
    "tcp-udp-cifs",
    "tcp-udp-discard",
    "tcp-udp-domain",
    "tcp-udp-echo",
    "tcp-udp-kerberos",
    "tcp-udp-nfs",
    "tcp-udp-pim-auto-rp",
    "tcp-udp-sip",
    "tcp-udp-sunrpc",
    "tcp-udp-tacacs",
    "tcp-udp-www",
    "tcp-udp-http",
    "tcp-udp-talk",
    "udp-biff",
    "udp-bootpc",
    "udp-bootps",
    "udp-cifs",
    "udp-discard",
    "udp-domain",
    "udp-dnsix",
    "udp-echo",
    "udp-www",
    "udp-http",
    "udp-nameserver",
    "udp-kerberos",
    "udp-mobile-ip",
    "udp-nfs",
    "udp-netbios-ns",
    "udp-netbios-dgm",
    "udp-ntp",
    "udp-pcanywhere-status",
    "udp-pim-auto-rp",
    "udp-radius",
    "udp-radius-acct",
    "udp-rip",
    "udp-secureid-udp",
    "udp-sip",
    "udp-snmp",
    "udp-snmptrap",
    "udp-sunrpc",
    "udp-syslog",
    "udp-tacacs",
    "udp-talk",
    "udp-tftp",
    "udp-time",
    "udp-who",
    "udp-xdmcp",
    "udp-isakmp",
    "icmp6-unreachable",
    "icmp6-packet-too-big",
    "icmp6-time-exceeded",
    "icmp6-parameter-problem",
    "icmp6-echo",
    "icmp6-echo-reply",
    "icmp6-membership-query",
    "icmp6-membership-report",
    "icmp6-membership-reduction",
    "icmp6-router-renumbering",
    "icmp6-router-solicitation",
    "icmp6-router-advertisement",
    "icmp6-neighbor-solicitation",
    "icmp6-neighbor-advertisement",
    "icmp6-neighbor-redirect",
    "icmp-echo",
    "icmp-echo-reply",
    "icmp-unreachable",
    "icmp-source-quench",
    "icmp-redirect",
    "icmp-alternate-address",
    "icmp-router-advertisement",
    "icmp-router-solicitation",
    "icmp-time-exceeded",
    "icmp-parameter-problem",
    "icmp-timestamp-request",
    "icmp-timestamp-reply",
    "icmp-information-request",
    "icmp-information-reply",
    "icmp-mask-request",
    "icmp-mask-reply",
    "icmp-traceroute",
    "icmp-conversion-error",
    "icmp-mobile-redirect",
    nullptr
    };

QSet<QString> NamedObject::reserved_words;
map<QString,int> NamedObject::name_disambiguation;


NamedObject::NamedObject(const FWObject *_obj, const QString &_platform)
{
    obj = _obj;
    platform = _platform;
    if (reserved_words.empty())
    {
        const char** cptr = rw;
        while (*cptr!=nullptr)
        {
            reserved_words.insert(QString(*cptr));
            cptr++;
        }
    }
    name = sanitizeObjectName(QString::fromUtf8(obj->getName().c_str()));
}

QString NamedObject::getCommandWord()
{
    if (Address::constcast(obj)!=nullptr && Address::constcast(obj)->isAny())
        return "any";

    if (Service::constcast(obj)!=nullptr && Service::constcast(obj)->isAny())
        return "any";

    if (Interface::constcast(obj))
        return "interface";

    return name;
}

QString NamedObject::sanitizeObjectName(const QString &name)
{
    QString qs = name;
    qs = qs.replace(" ", "_").replace("/", "_").left(64);

    if (reserved_words.contains(qs))
    {
        qs = qs + "_obj";
    }

    int n = name_disambiguation[qs];
    name_disambiguation[qs] = n + 1;
    qs = QString("%1.%2").arg(qs).arg(n);

    return qs;
}

QString NamedObject::createNetworkObjectCommand(const Address *addr_obj)
{
    if (addr_obj == nullptr) return "";
    if (addr_obj->isAny()) return "";
    if (Interface::constcast(obj)) return "";

    QStringList res;

    res << QString("object network %1") .arg(name);

    if (AddressRange::isA(addr_obj))
    {
        const AddressRange *ar = AddressRange::constcast(addr_obj);
        res << QString("  range %1 %2")
            .arg(ar->getRangeStart().toString().c_str())
            .arg(ar->getRangeEnd().toString().c_str());
    } else
    {
        string addr = addr_obj->getAddressPtr()->toString();

        if (IPv4::isA(addr_obj))
        {
            res << QString("  host %1").arg(addr.c_str());
        }

        if (Network::isA(addr_obj))
        {
            string netm = addr_obj->getNetmaskPtr()->toString();
            res << QString("  subnet %1 %2")
                .arg(addr.c_str())
                .arg(netm.c_str());
        }
    }


    res << "exit";
    res << "";
    return res.join("\n");
}

QString NamedObject::printPorts(int rs, int re)
{
    QStringList res;

    if (rs<0) rs = 0;
    if (re<0) re = 0;

    if (rs>0 || re>0)
    {
        if (rs==re)  res << "eq" << QString::number(rs);
        else
            if (rs==0 && re!=0) res << "lt" << QString::number(re);
            else
                if (rs!=0 && re==65535) res << "gt" << QString::number(rs);
                else
                    res << "range " << QString::number(rs)
                        << "" << QString::number(re);
    }
    return res.join(" ");
}

QString NamedObject::createServiceObjectCommand(const Service *serv_obj)
{
    if (serv_obj == nullptr) return "";
    if (serv_obj->isAny()) return "";

    QStringList res;

    QString proto_name = serv_obj->getProtocolName().c_str();

    res << QString("object service %1").arg(name);

    QStringList service_line;

    service_line << "  service";

    if (TCPService::isA(serv_obj) || UDPService::isA(serv_obj))
    {
        service_line << proto_name;

	int rs = TCPUDPService::constcast(serv_obj)->getSrcRangeStart();
	int re = TCPUDPService::constcast(serv_obj)->getSrcRangeEnd();
        if (rs != 0 || re != 0)
        {
            service_line << "source" << printPorts(rs, re);
        }

	rs = TCPUDPService::constcast(serv_obj)->getDstRangeStart();
	re = TCPUDPService::constcast(serv_obj)->getDstRangeEnd();
        if (rs != 0 || re != 0)
        {
            service_line << "destination" << printPorts(rs, re);
        }
    }

    if (ICMPService::isA(serv_obj))
    {
        service_line << proto_name;
        if (serv_obj->getInt("type")!=-1)
            service_line << QString::number(serv_obj->getInt("type"));
    }

    if (CustomService::isA(serv_obj))
    {
        service_line << CustomService::constcast(serv_obj)->getCodeForPlatform(
            platform.toStdString()).c_str();
    }

    res << service_line.join(" ");
    res << "exit";
    res << "";
    return res.join("\n");
}


QString NamedObject::getCommand()
{
    if (Address::constcast(obj)!=nullptr)
        return createNetworkObjectCommand(Address::constcast(obj));

    if (Service::constcast(obj)!=nullptr)
        return createServiceObjectCommand(Service::constcast(obj));

    return "";
}

QString NamedObject::getCommandWhenObjectGroupMember()
{
    if (Address::constcast(obj)!=nullptr) return "network-object object " + name;
    if (Service::constcast(obj)!=nullptr) return "service-object object " + name;
    return "";
}


