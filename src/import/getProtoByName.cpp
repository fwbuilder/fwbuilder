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


#include "getProtoByName.h"

#ifndef _WIN32
#  include <netdb.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif


QMap<QString, int> GetProtoByName::protocols;

int GetProtoByName::getProtocolByName(const QString &name)
{
    if (protocols.size() == 0)
    {
        protocols["icmp"] = 1;
        protocols["igmp"] = 2;
        protocols["ggp"] = 3;
        protocols["ipencap"] = 4;
        protocols["st"] = 5;
        protocols["tcp"] = 6;
        protocols["egp"] = 8;
        protocols["igp"] = 9;
        protocols["pup"] = 12;
        protocols["udp"] = 17;
        protocols["hmp"] = 20;
        protocols["xns-idp"] = 22;
        protocols["rdp"] = 27;
        protocols["iso-tp4"] = 29;
        protocols["xtp"] = 36;
        protocols["ddp"] = 37;
        protocols["idpr-cmtp"] = 38;
        protocols["ipv6"] = 41;
        protocols["ipv6-route"] = 43;
        protocols["ipv6-frag"]= 44;
        protocols["idrp"] = 45;
        protocols["rsvp"] = 46;
        protocols["gre"] = 47;
        protocols["esp"] = 50;
        protocols["ah"] =  51;
        protocols["skip"] = 57;
        protocols["ipv6-icmp"] = 58;
        protocols["ipv6-nonxt"] = 59;
        protocols["ipv6-opts"] = 60;
        protocols["rspf"] = 73;
        protocols["vmtp"] = 81;
        protocols["eigrp"] = 88;
        protocols["ospf"] = 89;
        protocols["ax.25"] = 93;
        protocols["ipip"] = 94;
        protocols["etherip"] = 97;
        protocols["encap"] = 98;
        protocols["pim"] = 103;
        protocols["ipcomp"] = 108;
        protocols["vrrp"] = 112;
        protocols["l2tp"] = 115;
        protocols["isis"] = 124;
        protocols["sctp"] = 132;
        protocols["fc"] =  133;
        protocols["udplite"] = 136;
        protocols["mpls-in-ip"] = 137;
        protocols["manet"] = 138;
        protocols["hip"] = 139;

        // these are found in Cisco configs. Some of these names
        // duplicate protocols listed above but a few are extras.
        protocols["ah"] = 51;
        protocols["ahp"] = 51;
        protocols["eigrp"] = 88;
        protocols["esp"] = 50;
        protocols["gre"] = 47;
        protocols["igmp"] = 2;
        protocols["igrp"] = 9;
        protocols["ip"] = 0;
        protocols["ipinip"] = 4;
        protocols["nos"] = 94;
        protocols["ospf"] = 89;
        protocols["pim"] = 103;
        protocols["pcp"] = 108;
        protocols["snp"] = 109;

        // ASA configs use protocol name "ipsec" as an alias for "esp"
        // and "pptp" as an alias for "gre"
        protocols["ipsec"] = 50;
        protocols["pptp"] = 47;

    }

    bool ok = false;
    int protocol = name.toInt(&ok);
    if (ok) return protocol;

    if (protocols.contains(name)) return protocols[name];
    struct protoent *pe = getprotobyname(name.toLatin1().constData());

    if (pe!=nullptr)
        return pe->p_proto;

    return -1;
}

