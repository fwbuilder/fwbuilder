/*

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

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

#include "../../config.h"
#include "global.h"
#include "utils_no_qt.h"
#include "platforms.h"


#include "IOSImporter.h"

#include <ios>
#include <iostream>
#include <algorithm>

#include "fwbuilder/Resources.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/RuleElement.h"

#include <QtDebug>

using namespace std;
using namespace libfwbuilder;

IOSImporter::IOSImporter(FWObject *lib,
                         std::istringstream &input,
                         Logger *log,
                         const std::string &fwname) : Importer(lib, "iosacl", input, log, fwname)
{
    ios_icmp_specs["echo-reply"] = std::pair<int,int>(0, 0);
    ios_icmp_specs["unreachable"] = std::pair<int,int>(3, -1); // all "unreachables"
    ios_icmp_specs["net-unreachable"] = std::pair<int,int>(3, 0);
    ios_icmp_specs["host-unreachable"] = std::pair<int,int>(3, 1);
    ios_icmp_specs["protocol-unreachable"] = std::pair<int,int>(3, 2);
    ios_icmp_specs["port-unreachable"] = std::pair<int,int>(3, 3);
    ios_icmp_specs["packet-too-big"] = std::pair<int,int>(3, 4);
    ios_icmp_specs["source-route-failed"] = std::pair<int,int>(3, 5);
    ios_icmp_specs["network-unknown"] = std::pair<int,int>(3, 6);
    ios_icmp_specs["host-unknown"] = std::pair<int,int>(3, 7);
    ios_icmp_specs["host-isolated"] = std::pair<int,int>(3, 8);
    ios_icmp_specs["dod-net-prohibited"] = std::pair<int,int>(3, 9);
    ios_icmp_specs["dod-host-prohibited"] = std::pair<int,int>(3, 10);
    ios_icmp_specs["net-tos-unreachable"] = std::pair<int,int>(3, 11);
    ios_icmp_specs["host-tos-unreachable"] = std::pair<int,int>(3, 12);
    ios_icmp_specs["administratively-prohibited"] = std::pair<int,int>(3, 13);
    ios_icmp_specs["host-precedence-unreachable"] = std::pair<int,int>(3, 14);
    ios_icmp_specs["precedence-unreachable"] = std::pair<int,int>(3, 15);
    ios_icmp_specs["source-quench"] = std::pair<int,int>(4, 0);
    ios_icmp_specs["net-redirect"] = std::pair<int,int>(5, 0);
    ios_icmp_specs["host-redirect"] = std::pair<int,int>(5, 1);
    ios_icmp_specs["net-tos-redirect"] = std::pair<int,int>(5, 2);
    ios_icmp_specs["host-tos-redirect"] = std::pair<int,int>(5, 3);
    ios_icmp_specs["echo"] = std::pair<int,int>(8, 0);
    ios_icmp_specs["router-advertisement"] = std::pair<int,int>(9, 0);
    ios_icmp_specs["router-solicitation"] = std::pair<int,int>(10, 0);
    ios_icmp_specs["ttl-exceeded"] = std::pair<int,int>(11, 0);
    ios_icmp_specs["reassembly-timeout"] = std::pair<int,int>(11, 1);
    ios_icmp_specs["general-parameter-problem"] = std::pair<int,int>(12, 0);
    ios_icmp_specs["option-missing"] = std::pair<int,int>(12, 1);
    ios_icmp_specs["timestamp-request"] = std::pair<int,int>(13, 0);
    ios_icmp_specs["timestamp-reply"] = std::pair<int,int>(14, 0);
    ios_icmp_specs["information-request"] = std::pair<int,int>(15, 0);
    ios_icmp_specs["information-reply"] = std::pair<int,int>(16, 0);
    ios_icmp_specs["mask-request"] = std::pair<int,int>(17, 0);
    ios_icmp_specs["mask-reply"] = std::pair<int,int>(18, 0);

    ios_proto_specs["ah"] = 51;
    ios_proto_specs["ahp"] = 51;
    ios_proto_specs["eigrp"] = 88;
    ios_proto_specs["esp"] = 50;
    ios_proto_specs["gre"] = 47;
    ios_proto_specs["igmp"] = 2;
    ios_proto_specs["igrp"] = 9;
    ios_proto_specs["ip"] = 0;
    ios_proto_specs["ipinip"] = 4;
    ios_proto_specs["nos"] = 94;
    ios_proto_specs["ospf"] = 89;
    ios_proto_specs["pim"] = 103;
    ios_proto_specs["pcp"] = 108;
    ios_proto_specs["snp"] = 109;

    ios_tcp_specs["bgp"] = 179;
    ios_tcp_specs["chargen"] = 19;
    ios_tcp_specs["cmd"] = 514;
    ios_tcp_specs["daytime"] = 13;
    ios_tcp_specs["discard"] = 9;
    ios_tcp_specs["domain"] = 53;
    ios_tcp_specs["echo"] = 7;
    ios_tcp_specs["exec"] = 512;
    ios_tcp_specs["finger"] = 79;
    ios_tcp_specs["ftp"] = 21;
    ios_tcp_specs["ftp-data"] = 20;
    ios_tcp_specs["gopher"] = 70;
    ios_tcp_specs["hostname"] = 101;
    ios_tcp_specs["ident"] = 113;
    ios_tcp_specs["irc"] = 194;
    ios_tcp_specs["klogin"] = 543;
    ios_tcp_specs["kshell"] = 544;
    ios_tcp_specs["login"] = 513;
    ios_tcp_specs["lpd"] = 515;
    ios_tcp_specs["nntp"] = 119;
    ios_tcp_specs["pop2"] = 109;
    ios_tcp_specs["pop3"] = 110;
    ios_tcp_specs["smtp"] = 25;
    ios_tcp_specs["sunrpc"] = 111;
    ios_tcp_specs["syslog"] = 514;
    ios_tcp_specs["tacacs"] = 49;
    ios_tcp_specs["tacacs-ds"] = 63;
    ios_tcp_specs["talk"] = 517;
    ios_tcp_specs["telnet"] = 23;
    ios_tcp_specs["time"] = 37;
    ios_tcp_specs["uucp"] = 540;
    ios_tcp_specs["whois"] = 43;
    ios_tcp_specs["www"] = 80;

    ios_udp_specs["biff"] =         512;
    ios_udp_specs["bootpc"] =       68;
    ios_udp_specs["bootps"] =       67;
    ios_udp_specs["discard"] =      9;
    ios_udp_specs["dnsix"] =        195;
    ios_udp_specs["domain"] =       53;
    ios_udp_specs["echo"] =         7;
    ios_udp_specs["isakmp"] =       500;
    ios_udp_specs["mobile-ip"] =    434;
    ios_udp_specs["nameserver"] =   42;
    ios_udp_specs["netbios-dgm"] =  138;
    ios_udp_specs["netbios-ns"] =   137;
    ios_udp_specs["netbios-ss"] =   139;
    ios_udp_specs["ntp"] =          123;
    ios_udp_specs["pim-auto-rp"] =  496;
    ios_udp_specs["rip"] =          520;
    ios_udp_specs["snmp"] =         161;
    ios_udp_specs["snmptrap"] =     162;
    ios_udp_specs["sunrpc"] =       111;
    ios_udp_specs["syslog"] =       514;
    ios_udp_specs["tacacs"] =       49;
    ios_udp_specs["talk"] =         517;
    ios_udp_specs["tftp"] =         69;
    ios_udp_specs["time"] =         37;
    ios_udp_specs["who"] =          513;
    ios_udp_specs["xdmcp"] =        177;

}



IOSImporter::~IOSImporter()
{
    all_rulesets.clear();
    all_interfaces.clear();
}

void IOSImporter::setInterfaceAndDirectionForRuleSet(
    const std::string &ruleset_name,
    const std::string &_intf_name,
    const std::string &_dir)
{
    Importer::setInterfaceAndDirectionForRuleSet(
        ruleset_name, _intf_name, _dir);

}

FWObject* IOSImporter::createAddress(const std::string &addr,
                                     const std::string &netmask)
{
    std::string correct_nm = netmask;

    // invert netmask (this is IOS)
    try
    {
        InetAddr orig_nm(netmask);
        correct_nm = (~orig_nm).toString();
        return Importer::createAddress(addr, correct_nm);
    } catch (FWException &ex)
    {
        markCurrentRuleBad(
            std::string("Error converting netmask '") + netmask + "' (address " + addr + ")");
        return Importer::createAddress(addr, InetAddr::getAllOnes().toString());
    }

}

FWObject* IOSImporter::createICMPService()
{
    std::string icmpspec = strip(icmp_spec);
    if (!icmpspec.empty())
    {
        // Cisco is trying to be too helpful, they translate many
        // icmp type/code combinations into stings
        if (ios_icmp_specs.count(icmpspec)!=0)
        {
            std::pair<int,int> pp = ios_icmp_specs[icmpspec];
            std::ostringstream s1, s2;
            s1 << pp.first;
            icmp_type = s1.str();
            s2 << pp.second;
            icmp_code = s2.str();
        } else
        {
            markCurrentRuleBad(
                std::string("Import of icmp protocol '") + icmp_spec + "' failed");
            icmp_code = "-1";
            icmp_type = "-1";
        }
    }
    icmp_spec = "";

    return Importer::createICMPService();
}

FWObject* IOSImporter::createIPService()
{
    if (ios_proto_specs.count(protocol)!=0)
    {
        std::ostringstream s;
        s << ios_proto_specs[protocol];
        protocol = s.str();
    }
    return Importer::createIPService();
}

int IOSImporter::convertPort(const std::string &port_str,
                             std::map<std::string, int> &port_map)
{
    int port = 0;
    std::string ps = strip(port_str);
    if (port_map.count(ps)>0) port = port_map[ps];
    else
    {
        if (ps=="") return 0;
        std::istringstream str1(ps);
        str1.exceptions(std::ios::failbit);
        try
        {
            str1 >> port;
        } catch (const std::exception &ex) {
            // could not convert port_spec to an integer
            markCurrentRuleBad(std::string("Port spec '") + port_str +
                               "' unknown. Error " + ex.what());
        }
    }
    return port;
}

std::pair<int,int> IOSImporter::convertPortSpec(const std::string &port_op,
                                                const std::string &port_spec,
                                        std::map<std::string, int> &port_map)
{
    int range_start;
    int range_end;
    std::string s1,s2;
    std::string portop = strip(port_op);
    std::string portspec = strip(port_spec);

    if (fwbdebug)
        qDebug() << QString("Convert TCP/UDP port spec: port_op=%1 port_spec=%2").
            arg(port_op.c_str()).arg(port_spec.c_str());

    if (portop=="" && portspec=="") return std::pair<int,int>(0, 0);

    std::string::size_type n = portspec.find(' ');
    if (n!=std::string::npos)
    {
        s1 = portspec.substr(0, n);
        s2 = portspec.substr(n);
    } else
    {
        s1 = portspec;
        s2 = portspec;
    }
    range_start = convertPort(s1, port_map);
    range_end   = convertPort(s2, port_map);

    if (portop=="lt")  range_start = 0;
    if (portop=="gt")  range_end = 65535;
    if (portop=="eq")
    {
        range_start = range_end;
    }
    if (portop=="range")
    {
        // range_start and range_end have been set
        ;
    }

    return std::pair<int,int>(range_start, range_end);
}

FWObject* IOSImporter::createTCPService()
{
    // use src_port_op, src_port_spec, dst_port_op, dst_port_spec
    // port_op can be: lt (less than), gt (greater than), eq (equal),
    // neq (not equal), and range (inclusive range).
    // here we assume src_port_spec and dst_port_spec are
    // both numeric and represent a single port.

    std::string name = "tcp " + src_port_spec + " " + dst_port_spec;

    std::pair<int,int> pr =
        convertPortSpec(src_port_op, src_port_spec, ios_tcp_specs);
    int srs = pr.first;
    int sre = pr.second;

    pr = convertPortSpec(dst_port_op, dst_port_spec, ios_tcp_specs);
    int drs = pr.first;
    int dre = pr.second;

    return getTCPService(srs,sre,
                         drs,dre,
                         established,tcp_flags_mask,tcp_flags_comp);
}

FWObject* IOSImporter::createUDPService()
{
    // use src_port_op, src_port_spec, dst_port_op, dst_port_spec
    // port_op can be: lt (less than), gt (greater than), eq (equal),
    // neq (not equal), and range (inclusive range).
    // here we assume src_port_spec and dst_port_spec are
    // both numeric and represent a single port.

    std::string name = "udp " + src_port_spec + " " + dst_port_spec;

    std::pair<int,int> pr =
        convertPortSpec(src_port_op, src_port_spec, ios_udp_specs);
    int srs = pr.first;
    int sre = pr.second;

    pr = convertPortSpec(dst_port_op, dst_port_spec, ios_udp_specs);
    int drs = pr.first;
    int dre = pr.second;

    return getUDPService(srs,sre,drs,dre);
}

void IOSImporter::merge_rule::operator()(FWObject* r)
{
    PolicyRule *rule = PolicyRule::cast(r);
    // Note that Policy object can have children that are objects of
    // classes PolicyRule and RuleSetOptions. If r does not cast to
    // PolicyRule, then it must be RuleSetOptions and we should just
    // skip it.
    if (rule==NULL) return;

    target_ruleset->add(rule);
//    target_ruleset->renumberRules();

    RuleElementItf* re =rule->getItf();
    re->addRef(intf);
    rule->setDirection(dir);

    std::string prev_comment = rule->getComment();
    rule->setComment(
        std::string("Imported from ") + ruleset_name + "\n" + prev_comment);
}

Firewall* IOSImporter::finalize()
{
    // scan all UnidirectionalRuleSet objects, set interface and
    // direction in all rules of corresponding RuleSet and merge all
    // UnidirectionalRuleSet into one RuleSet object. Attach this
    // object to the firewall.

    if (fwbdebug) qDebug("IOSImporter::finalize()");

    if (haveFirewallObject())
    {
        Firewall *fw = Firewall::cast(getFirewallObject());
        fw->setStr("host_OS", "ios");
        Resources::setDefaultTargetOptions("ios" , fw);

        FWObject *policy = getFirewallObject()->getFirstByType(Policy::TYPENAME);
        assert( policy!=NULL );

        if (all_rulesets.size()!=0)
        {
            if (fwbdebug)
            {
                qDebug() << "Setting interface and direction for all rules";
                qDebug() << "all_rulesets.size()=" << all_rulesets.size();
            }

            std::map<const std::string,UnidirectionalRuleSet*>::iterator i;
            for (i=all_rulesets.begin(); i!=all_rulesets.end(); ++i)
            {
                UnidirectionalRuleSet *irs = (*i).second;

                if (fwbdebug)
                {
                    qDebug() << "  irs->name=" << irs->name.c_str();
                    qDebug() << "  irs->intf_dir.size()=" << irs->intf_dir.size();
                    qDebug() << "  irs->ruleset->size()=" << irs->ruleset->size();
                }

                // optimization: If we have several interfaces for
                // the ruleset, create a group
                // But first group interfaces by direction so
                // that later we can merge rules into the policy
                // with proper combination of interface group and
                // direction. Remember that the same access list
                // can be used with multiple interfaces with different
                // directions each time. The same list can be applied
                // to the same interface both in and out (although in
                // this case we have already switched direction to "both")
                //
                if (irs->intf_dir.size()>1)
                {
                    std::list<std::string> all_in;
                    std::list<std::string> all_out;
                    std::list<std::string> all_both;

                    std::map<std::string,std::string>::iterator i;
                    for (i = irs->intf_dir.begin();
                         i != irs->intf_dir.end(); ++i)
                    {
                        if ( (*i).second=="in")
                            all_in.push_back( (*i).first );
                        if ( (*i).second=="out")
                            all_out.push_back( (*i).first );
                        if ( (*i).second=="both")
                            all_both.push_back( (*i).first );
                    }
                    FWObject *og;
                    if (all_in.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_in);
                        std::for_each(irs->ruleset->begin(),
                                      irs->ruleset->end(),
                                      merge_rule(irs->name,
                                                 og,
                                                 PolicyRule::Inbound,
                                                 policy)
                        );
                    }

                    if (all_out.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_out);
                        std::for_each(irs->ruleset->begin(),
                                      irs->ruleset->end(),
                                      merge_rule(irs->name,
                                                 og,
                                                 PolicyRule::Outbound,
                                                 policy)
                        );
                    }

                    if (all_both.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_both);
                        std::for_each(irs->ruleset->begin(),
                                      irs->ruleset->end(),
                                      merge_rule(irs->name,
                                                 og,
                                                 PolicyRule::Both,
                                                 policy)
                        );
                    }

                }
                else
                {
                    std::map<std::string,std::string>::iterator j;
                    for (j=irs->intf_dir.begin(); j!=irs->intf_dir.end(); ++j)
                    {
                        Interface *intf = all_interfaces[ (*j).first ];
                        std::string _dir = (*j).second;
                        PolicyRule::Direction direction = PolicyRule::Both;
                        if (_dir=="in")  direction = PolicyRule::Inbound;
                        if (_dir=="out") direction = PolicyRule::Outbound;

                        // not all access lists are associated with interfaces
                        if (intf!=NULL)
                        {
                            if (fwbdebug)
                                qDebug() << "    interface=" 
                                         << intf->getName().c_str();

                            std::for_each(
                                irs->ruleset->begin(),
                                irs->ruleset->end(),
                                merge_rule(irs->name,
                                           intf,
                                           direction,
                                           policy)
                            );
                        }
                    }
                }
                qDebug("ruleset done");

                // call clearChidren() not recursive because children objects
                // of all rules should not be deleted
                irs->ruleset->clearChildren(false);
                getFirewallObject()->remove(irs->ruleset, false);
                delete irs->ruleset;
            }
        }

        list<FWObject*> l2 = fw->getByType(Policy::TYPENAME);
        for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
        {
            RuleSet *rs = RuleSet::cast(*i);
            rs->renumberRules();
        }

        return getFirewallObject();
    }
    else
        return NULL;
}
