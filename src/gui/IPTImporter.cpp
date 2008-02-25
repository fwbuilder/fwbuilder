/*

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: IPTImporter.cpp,v 1.10 2008/02/11 01:30:39 vkurland Exp $

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

#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"
#include "utils_no_qt.h"
#include "platforms.h"


#include "IPTImporter.h"

#include <ios>
#include <iostream>
#include <algorithm>

#ifndef _WIN32
#  include <netdb.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif

#include "fwbuilder/Resources.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/IPAddress.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/RuleElement.h"

using namespace libfwbuilder;

IPTImporter::IPTImporter(FWObject *lib,
                         std::istringstream &input,
                         Logger *log) : Importer(lib, "iptables", input, log)
{
    service_group_name_seed = 0;
    current_ruleset = NULL;
    current_rule = NULL;
    last_mark_rule = NULL;

    clear();

    icmp_specs["echo-reply"] = std::pair<int,int>(0, 0);

    // all "unreachables"
    icmp_specs["destination-unreachable"] = std::pair<int,int>(3, -1);
    icmp_specs["network-unreachable"] = std::pair<int,int>(3, 0);
    icmp_specs["host-unreachable"] = std::pair<int,int>(3, 1);
    icmp_specs["protocol-unreachable"] = std::pair<int,int>(3, 2);
    icmp_specs["port-unreachable"] = std::pair<int,int>(3, 3);
    icmp_specs["fragmentation-needed"] = std::pair<int,int>(3, 4);
    icmp_specs["source-route-failed"] = std::pair<int,int>(3, 5);
    icmp_specs["network-unknown"] = std::pair<int,int>(3, 6);
    icmp_specs["host-unknown"] = std::pair<int,int>(3, 7);
    icmp_specs["host-isolated"] = std::pair<int,int>(3, 8);
    icmp_specs["network-prohibited"] = std::pair<int,int>(3, 9);
    icmp_specs["host-prohibited"] = std::pair<int,int>(3, 10);
    icmp_specs["TOS-network-unreachable"] = std::pair<int,int>(3, 11);
    icmp_specs["TOS-host-unreachable"] = std::pair<int,int>(3, 12);
    icmp_specs["communication-prohibited"] = std::pair<int,int>(3, 13);
    icmp_specs["host-precedence-violation"] = std::pair<int,int>(3, 14);
    icmp_specs["precedence-cutoff"] = std::pair<int,int>(3, 15);

    icmp_specs["source-quench"] = std::pair<int,int>(4, 0);

    icmp_specs["redirect"] = std::pair<int,int>(5, -1);
    icmp_specs["network-redirect"] = std::pair<int,int>(5, 0);
    icmp_specs["host-redirect"] = std::pair<int,int>(5, 1);
    icmp_specs["TOS-network-redirect"] = std::pair<int,int>(5, 2);
    icmp_specs["TOS-host-redirect"] = std::pair<int,int>(5, 3);

    icmp_specs["echo-request"] = std::pair<int,int>(8, 0);

    icmp_specs["router-advertisement"] = std::pair<int,int>(9, 0);

    icmp_specs["router-solicitation"] = std::pair<int,int>(10, 0);

    icmp_specs["ttl-exceeded"] = std::pair<int,int>(11, 0);
    icmp_specs["time-exceeded"] = std::pair<int,int>(11, 0);
    icmp_specs["ttl-zero-during-transit"] = std::pair<int,int>(11, 0);
    icmp_specs["ttl-zero-during-reassembly"] = std::pair<int,int>(11, 1);

    icmp_specs["parameter-problem"] = std::pair<int,int>(12, 0);
    icmp_specs["ip-header-bad"] = std::pair<int,int>(12, 0);
    icmp_specs["required-option-missing"] = std::pair<int,int>(12, 1);

    icmp_specs["timestamp-request"] = std::pair<int,int>(13, 0);

    icmp_specs["timestamp-reply"] = std::pair<int,int>(14, 0);

    icmp_specs["information-request"] = std::pair<int,int>(15, 0);

    icmp_specs["information-reply"] = std::pair<int,int>(16, 0);

    icmp_specs["address-mask-request"] = std::pair<int,int>(17, 0);

    icmp_specs["address-mask-reply"] = std::pair<int,int>(18, 0);
}



IPTImporter::~IPTImporter()
{
    clear();
}

void IPTImporter::clear()
{
    Importer::clear();
    if (!src_port_list.empty())  src_port_list.clear();
    if (!dst_port_list.empty())  dst_port_list.clear();
    current_state = "";
    i_intf = "";
    o_intf = "";
    target = "";
    tmp_port_range_start = "";
    tmp_port_range_end = "";
    src_neg = dst_neg = srv_neg = intf_neg = false;
    match_mark = "";
    limit_val = "";
    limit_suffix = "";
    limit_burst = "";
    if (!action_params.empty()) action_params.clear();
    nat_addr1 = "";
    nat_addr2 = "";
    nat_nm = "";
    nat_port_range_start = "";
    nat_port_range_end = "";

}

void IPTImporter::startSrcMultiPort()
{
    src_port_list.clear();
}

void IPTImporter::pushTmpPortSpecToSrcPortList()
{
    src_port_list.push_back(
        str_tuple( tmp_port_range_start, tmp_port_range_end ) );
}

void IPTImporter::startDstMultiPort()
{
    dst_port_list.clear();
}

void IPTImporter::pushTmpPortSpecToDstPortList()
{
    dst_port_list.push_back(
        str_tuple( tmp_port_range_start, tmp_port_range_end ) );
}


FWObject* IPTImporter::createAddress(const std::string &addr,
                                     const std::string &netmask)
{
    return Importer::createAddress(addr, netmask);
}

FWObject* IPTImporter::createICMPService()
{
    std::string icmpspec = strip(icmp_spec);
    if (!icmpspec.empty())
    {
        // Cisco is trying to be too helpful, they translate many
        // icmp type/code combinations into stings
        if (icmp_specs.count(icmpspec)!=0)
        {
            std::pair<int,int> pp = icmp_specs[icmpspec];
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

FWObject* IPTImporter::createIPService()
{
    struct protoent *pe = getprotobyname(protocol.c_str());
    if (pe!=NULL)
    {
        std::ostringstream s;
        s << pe->p_proto;
        protocol = s.str();
        //free(pe);
    }
    return Importer::createIPService();
}

std::pair<int,int> IPTImporter::convertPortRange(str_tuple &range,
                                                 const char *proto)
{
    return std::pair<int,int>(convertPort(range.first,proto),
                              convertPort(range.second,proto));
}

int IPTImporter::convertPort(const std::string &port_spec,
                             const char *proto)
{
    int port = 0;
    std::string ps = strip(port_spec);
    if (ps=="") return 0;

    struct servent *se = getservbyname(ps.c_str(), proto);
    if (se!=NULL)
    {
        port = ntohs(se->s_port);
        //free(se);
        return port;
    }

    std::istringstream str1(ps);
    str1.exceptions(std::ios::failbit);
    try
    {
        str1 >> port;
    } catch (const std::exception &ex) {
        // could not convert port_spec to an integer
        markCurrentRuleBad(std::string("Port spec '") + port_spec +
                           "' unknown. Error " + ex.what());
    }
    return port;
}

FWObject* IPTImporter::createTCPUDPService(str_tuple &src_range,
                                           str_tuple &dst_range,
                                           const std::string &proto)
{
    if (fwbdebug)
    {
        qDebug(QString("Creating %1 service").arg(proto.c_str()).toAscii().constData());
        qDebug(QString("src range: %1 - %2")
               .arg(src_range.first.c_str())
               .arg(src_range.second.c_str()).toAscii().constData());
        qDebug(QString("dst range: %1 - %2")
               .arg(dst_range.first.c_str())
               .arg(dst_range.second.c_str()).toAscii().constData());
    }

//     std::string name = proto + " "
//         + src_range.first + "-" + src_range.second +
//         + ":"
//         + dst_range.first + "-" + dst_range.second;

    std::pair<int,int> pr = convertPortRange(src_range, proto.c_str());
    int srs = pr.first;
    int sre = pr.second;

    pr = convertPortRange(dst_range, proto.c_str());
    int drs = pr.first;
    int dre = pr.second;

    FWObject *o;
    if (proto=="tcp")
    {
        o = getTCPService(srs,sre,
                          drs,dre,
                          established,
                          tcp_flags_mask,tcp_flags_comp);
    } else
        o = getUDPService(srs,sre,drs,dre);
    return o;
}

FWObject* IPTImporter::createTCPUDPService(const std::string &proto)
{
    str_tuple empty_range("0","0");

    // use src_port_list and dst_port_list
    // if this is multiport, should only be either src or dst port
    //
    if (src_port_list.size()>1 || dst_port_list.size()>1)
    {
        std::list<FWObject*> olist;
        std::list<std::string> list_names;
        std::list<str_tuple>::iterator i;

        std::list<str_tuple> *list_ptr;
        if (src_port_list.size()>1) list_ptr = &src_port_list;
        else                        list_ptr = &dst_port_list;

        std::string sig;
        if (src_port_list.size()>1) sig = proto + " src ";
        else                        sig = proto + " dst ";
        for (i = list_ptr->begin(); i != list_ptr->end(); ++i)
        {
            sig += (*i).first + ":" + (*i).second + "_";
        }
        if (all_objects.count(sig)!=0) return all_objects[sig];

        for (i = list_ptr->begin(); i != list_ptr->end(); ++i)
        {
            FWObject *o;

            o = createTCPUDPService(
                (list_ptr == &src_port_list) ? *i : empty_range,
                (list_ptr == &dst_port_list) ? *i : empty_range,
                proto);

            olist.push_back(o);
            list_names.push_back(o->getName());
        }

        std::ostringstream s;
        s << service_group_name_seed;
        service_group_name_seed++;
        std::string name = proto + " group " + s.str();

        if (fwbdebug)
            qDebug(QString("Group of %1 services with name '%2', sig '%3'").arg(proto.c_str()).arg(name.c_str()).arg(sig.c_str()).toAscii().constData());

        ServiceGroup *sg = ServiceGroup::cast(createObject(ServiceGroup::TYPENAME, name));
        for (FWObject::iterator j=olist.begin(); j!=olist.end(); ++j)
        {
            sg->addRef(*j);
        }
        all_objects[sig] = sg;
        return sg;

    } else // single tcp/udp object
    {
        return createTCPUDPService(
            (src_port_list.size()>0) ? src_port_list.front() : empty_range,
            (dst_port_list.size()>0) ? dst_port_list.front() : empty_range,
            proto);
    }
}

FWObject* IPTImporter::createTCPService()
{
    return createTCPUDPService("tcp");
}

FWObject* IPTImporter::createUDPService()
{
    return createTCPUDPService("udp");
}

void IPTImporter::addSrv()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    RuleElementSrv* srv = rule->getSrv();
    assert(srv!=NULL);

    if (match_mark.empty())
    {
        Importer::addSrv();
        return;
    }

    srv->addRef( getTagService(match_mark) );
}

void IPTImporter::pushRule()
{
    assert(current_ruleset!=NULL);
    if (current_rule==NULL) return;

    if (current_table=="nat")  pushNATRule();
    else                       pushPolicyRule();
}

void IPTImporter::pushPolicyRule()
{
    // populate all elements of the rule

    PolicyRule *rule = PolicyRule::cast(current_rule);
    rule->setLogging(false);

    FWOptions  *fwopt = getFirewallObject()->getOptionsObject();
    assert(fwopt!=NULL);

    FWOptions  *ropt = current_rule->getOptionsObject();
    assert(ropt!=NULL);

    bool skip_rule = false;

    PolicyRule::Action action = PolicyRule::Unknown;

    if (target=="ACCEPT")   action = PolicyRule::Accept;
    if (target=="DROP")     action = PolicyRule::Deny;
    if (target=="REJECT")
    {
        action = PolicyRule::Reject;
        if (action_params["reject_with"]=="tcp-reset")
            ropt->setStr("action_on_reject", "TCP RST");
        else
            ropt->setStr("action_on_reject", action_params["reject_with"]);
    }
    if (target=="QUEUE")    action = PolicyRule::Pipe;
    if (target=="CLASSIFY") action = PolicyRule::Classify;
    if (target=="LOG")
    {
        action = PolicyRule::Continue;
        rule->setLogging(true);
        ropt->setStr("log_prefix", action_params["log_prefix"]);
        ropt->setStr("log_tcp_seq", action_params["log_tcp_seq"]);
        ropt->setStr("log_tcp_options", action_params["log_tcp_options"]);
        ropt->setStr("log_ip_options", action_params["log_ip_options"]);
        ropt->setStr("log_level", action_params["log_level"]);
        if (!limit_val.empty())
        {
            ropt->setStr("limit_value", limit_val);
            ropt->setStr("limit_suffix", std::string("/")+limit_suffix);
            if (!limit_burst.empty())
                ropt->setStr("limit_burst", limit_burst);
        }
    }
    if (target=="ULOG")
    {
        action = PolicyRule::Continue;
        rule->setLogging(true);
        fwopt->setBool("use_ULOG", true);
        ropt->setStr("log_prefix", action_params["log_prefix"]);
    }
    if (target=="MARK")
    {
        action = PolicyRule::Tag;
        last_mark_rule = rule;
        ropt->setStr("tagvalue", action_params["set_mark"]);
//        if (current_chain=="PREROUTING")
//            ropt->setBool("ipt_mark_prerouting",true);
    }
    if (target=="CONNMARK")        action = PolicyRule::Continue;

    if (target=="ROUTE")
    {
        action = PolicyRule::Route;

        if (!action_params["route_iif"].empty())
            newInterface(action_params["route_iif"]);
        if (!action_params["route_oif"].empty())
            newInterface(action_params["route_oif"]);

        ropt->setStr("ipt_iif",        action_params["route_iif"]);
        ropt->setStr("ipt_oif",        action_params["route_oif"]);
        ropt->setStr("ipt_gw",         action_params["route_gw"]);
        ropt->setBool("ipt_continue", !action_params["route_continue"].empty());
        ropt->setBool("ipt_tee",      !action_params["route_tee"].empty());
    }

    if (target=="RETURN")
    {
        action = PolicyRule::Continue;
    }

    if (target=="TCPMSS" && action_params["clamp-mss-to-pmtu"]=="--clamp-mss-to-pmtu")
    {
        fwopt->setBool("clamp_mss_to_mtu", true);
        skip_rule = true;
        *Importer::logger
            << "Using automatic rule controlled by option "
            << "Clamp MSS to MTU"
            << "\n";
    }

    if (action==PolicyRule::Unknown)
    {
        // unknown target, consider it a branch
        // NOTE:
        // as of fwbuilder v2.1, branch ruleset is a child object of
        // PolicyRule. This means two different rules can not point at the same
        // branch ruleset. This is unfortunate. To fix this we need
        // to change XML DTD and API. Will do this in 3.0
        // Meanwhile, have to check if branch ruleset with requested name
        // already exists and change the name by adding suffix '1', '2' etc
        // to make it different.
        //
        std::string branch_ruleset_name = target;
        bool duplicate_branch = false;
        int cntr = 0;
        action = PolicyRule::Branch;
        UnidirectionalRuleSet *rs = NULL;
        while (true)
        {
            rs = branch_rulesets[branch_ruleset_name];
            if (rs==NULL)
            {
                rs = getUnidirRuleSet(branch_ruleset_name);
                break;
            } else
            {
                std::ostringstream ostr;
                ostr << ++cntr;
                branch_ruleset_name = target + ostr.str();
                duplicate_branch = true;
            }
        }

        current_rule->add(rs->ruleset);
        ropt->setStr("branch_name", branch_ruleset_name);
        getFirewallObject()->remove(rs->ruleset, false);
        branch_rulesets[branch_ruleset_name] = rs;

        if (duplicate_branch)
            markCurrentRuleBad(
                std::string("Rule passes control to branch ") + target +
                std::string(
                    " which \n"
                    "is already used by some rule prior to this one. \n"
                    "fwbuilder 2.1 does not support multiple rules \n"
                    "passing control to the same branch. This will \n"
                    "be fixed in the next major release (v3.0)"));

    }

    rule->setAction(action);

    if (target!="LOG" && !limit_val.empty())
    {
        ropt->setStr("limit_value", limit_val);
        ropt->setStr("limit_suffix", std::string("/") + limit_suffix);
        if (!limit_burst.empty())
            ropt->setStr("limit_burst", limit_burst);
    }

    addSrc();
    addDst();
    addSrv();

/* Recognize some typical rule patterns and set firewall and rule
 * options appropriately
 */
    if (current_state=="NEW")
    {
        ropt->setBool("stateless", false);
    }
    RuleElementSrc      *nsrc;
    RuleElementDst      *ndst;

    rule->getSrc()->setNeg(src_neg);
    rule->getDst()->setNeg(dst_neg);
    rule->getSrv()->setNeg(srv_neg);
    rule->getItf()->setNeg(intf_neg);

    if (rule->getSrc()->isAny() &&
        rule->getDst()->isAny() &&
        rule->getSrv()->isAny() &&
        current_state == "RELATED,ESTABLISHED")
    {
        fwopt->setBool("accept_established", true);
        skip_rule = true;
        *Importer::logger
            << "Using automatic rule controlled by option "
            << "'Accept established,related states' to match "
            << "states RELATED,ESTABLISHED"
            << "\n";
    }

    if (rule->getSrc()->isAny() &&
        rule->getDst()->isAny() &&
        rule->getSrv()->isAny() &&
        current_state == "INVALID")
    {
        if (target=="DROP") fwopt->setBool("drop_invalid", true);
        if (target=="LOG")  fwopt->setBool("log_invalid", true);
        skip_rule = true;
        *Importer::logger
            << "Using automatic rule controlled by option "
            << "'Drop packet that do not match any known connection' to match "
            << "state INVALID"
            << "\n";
    }

    if (target=="CONNMARK" &&
        last_mark_rule != NULL &&
        !action_params["connmark_save_mark"].empty())
    {
        FWOptions  *lmr_ropt = last_mark_rule->getOptionsObject();
        assert(lmr_ropt!=NULL);
        lmr_ropt->setBool("ipt_mark_connections", true);
        skip_rule = true;
        *Importer::logger
            << "Turned option on in previous rule with action Mark "
            << "for '-j CONNMARK --save-mark' "
            << "\n";
    }

    if (target=="CONNMARK" &&
        !action_params["connmark_restore_mark"].empty())
    {
        // this rule is added automatically in
        // MangleTableCompiler_ipt::flushAndSetDefaultPolicy()
        // if we have at least one rule with CONNMARK target in the policy
        skip_rule = true;
        *Importer::logger
            << "Skip command with '-j CONNMARK --restore-mark' "
            << "This rule is generated automatically."
            << "\n";
    }

    if (!skip_rule)
    {
/* we set "firewall_is_part_of_any_and_networks" to False */
        rule_comment += std::string("\n") + "Chain " + current_chain + ". ";

        if (current_chain=="INPUT")
        {
            ndst = rule->getDst();
            if (ndst->isAny()) ndst->addRef(getFirewallObject());
            else
                rule_comment += "Does DST match one of the firewall's addresses?";
        }

        if (current_chain=="OUTPUT")
        {
            nsrc = rule->getSrc();
            if (nsrc->isAny()) nsrc->addRef(getFirewallObject());
            else
                rule_comment += "Does SRC match one of the firewall's addresses?";
        }

        //  add rule to the right ruleset
        std::string ruleset_name = "";
        if (current_chain=="INPUT" ||
            current_chain=="OUTPUT" ||
            current_chain=="FORWARD" ||
            current_chain=="PREROUTING" ||
            current_chain=="POSTROUTING") ruleset_name = "filter";
        else
            ruleset_name = current_chain;

        UnidirectionalRuleSet *rs = getUnidirRuleSet(ruleset_name);
        assert(rs!=NULL);
        rs->ruleset->add(current_rule);

        rule->setDirection(PolicyRule::Both);

        if ( !i_intf.empty() && !o_intf.empty())
        {
            markCurrentRuleBad(
                std::string("Can not set inbound and outbound interface simultaneously. Was: -i ") + i_intf + " -o " + o_intf);
        } else
        {
            if ( !i_intf.empty())
            {
                rule->setDirection(PolicyRule::Inbound);
                newInterface(i_intf);
                Interface *intf = all_interfaces[i_intf];
                RuleElementItf* re =rule->getItf();
                re->addRef(intf);
            }

            if ( !o_intf.empty())
            {
                rule->setDirection(PolicyRule::Outbound);
                newInterface(o_intf);
                Interface *intf = all_interfaces[o_intf];
                RuleElementItf* re =rule->getItf();
                re->addRef(intf);
            }
        }

        current_rule->setComment(rule_comment);

    }

//     *Importer::logger << "Rule: " << rule->getActionAsString() << " "
//                       << "protocol=" << protocol << " "
//                       << "src=" << src_a << "/" << src_nm << " ";
//      if (dst_a!="")
//          *Importer::logger << "dst=" <<  dst_a << "/" << dst_nm << " ";
//      *Importer::logger << "\n";

    current_rule = NULL;
    rule_comment = "";

    clear();
}

void IPTImporter::pushNATRule()
{
    // populate all elements of the rule

    NATRule *rule = NATRule::cast(current_rule);

    FWOptions  *fwopt = getFirewallObject()->getOptionsObject();
    assert(fwopt!=NULL);

    FWOptions  *ropt = current_rule->getOptionsObject();
    assert(ropt!=NULL);

    addOSrc();
    addODst();
    addOSrv();

    if (src_nm.empty()) src_nm = "255.255.255.255";
    if (dst_nm.empty()) dst_nm = "255.255.255.255";
    if (nat_nm.empty()) nat_nm = "255.255.255.255";

    if (target=="ACCEPT")
    {
        rule->setRuleType(NATRule::NONAT);
    }
    if (target=="MASQUERADE")
    {
        rule->setRuleType(NATRule::Masq);
        RuleElementTSrc *re = rule->getTSrc();
        assert(re!=NULL);
        if ( !o_intf.empty() )
        {
            newInterface(o_intf);
            Interface *intf = all_interfaces[o_intf];
            re->addRef(intf);
        } else
        {
            re->addRef(getFirewallObject());
        }
    }
    if (target=="SNAT")
    {
        rule->setRuleType(NATRule::SNAT);
        FWObject *tsrc = NULL;
        if (nat_addr1!=nat_addr2)
            tsrc = createAddressRange(nat_addr1, nat_addr2);
        else
            tsrc = createAddress(nat_addr1, nat_nm);

        RuleElementTSrc *re = rule->getTSrc();
        assert(re!=NULL);
        re->addRef(tsrc);

        if (!nat_port_range_start.empty())
        {
            str_tuple empty_range("0", "0");
            str_tuple nat_port_range(nat_port_range_start, nat_port_range_end);
            FWObject *s = createTCPUDPService(nat_port_range, empty_range,
                                              protocol);
            RuleElementTSrv *re = rule->getTSrv();
            assert(re!=NULL);
            re->addRef(s);
        }
        if (!o_intf.empty())
            markCurrentRuleBad(
                std::string("Original rule defines outbound interface '") + o_intf + "'.\n Replace address in TSrc with matching interface of the firewall.");

    }
    if (target=="DNAT")
    {
        rule->setRuleType(NATRule::DNAT);
        FWObject *tdst = NULL;
        if (nat_addr1!=nat_addr2)
            tdst = createAddressRange(nat_addr1, nat_addr2);
        else
            tdst = createAddress(nat_addr1, nat_nm);

        RuleElementTDst *re = rule->getTDst();
        assert(re!=NULL);
        re->addRef(tdst);

        if (!nat_port_range_start.empty())
        {
            str_tuple empty_range("0", "0");
            str_tuple nat_port_range(nat_port_range_start, nat_port_range_end);
            FWObject *s = createTCPUDPService(empty_range, nat_port_range,
                                              protocol);
            RuleElementTSrv *re = rule->getTSrv();
            assert(re!=NULL);
            re->addRef(s);
        }
        if (!i_intf.empty())
            markCurrentRuleBad(
                std::string("Original rule defines inbound interface '") + i_intf + "'.\n Replace address in ODst with matching interface of the firewall.");

    }
    if (target=="NETMAP")
    {
        FWObject *o = NULL;
        if (!src_a.empty())
        {
            rule->setRuleType(NATRule::SNetnat);
            o = createAddress(src_a, src_nm);
            RuleElementOSrc *osrc = rule->getOSrc();
            osrc->addRef(o);
            RuleElementTSrc *tsrc = rule->getTSrc();
            assert(tsrc!=NULL);
            o = createAddress(nat_addr1, nat_nm);
            tsrc->addRef(o);
        }
        if (!dst_a.empty())
        {
            rule->setRuleType(NATRule::DNetnat);
            o = createAddress(dst_a, dst_nm);
            RuleElementOSrc *odst = rule->getOSrc();
            odst->addRef(o);
            RuleElementTDst *tdst = rule->getTDst();
            assert(tdst!=NULL);
            o = createAddress(nat_addr1, nat_nm);
            tdst->addRef(o);
        }
    }

    current_rule->setComment(rule_comment);

    UnidirectionalRuleSet *rs = getUnidirRuleSet("nat");
    assert(rs!=NULL);
    rs->ruleset->add(current_rule);


    current_rule = NULL;
    rule_comment = "";

    clear();
}

Firewall* IPTImporter::finalize()
{
    // scan all UnidirectionalRuleSet objects, set interface and
    // direction in all rules of corresponding RuleSet and merge all
    // UnidirectionalRuleSet into one RuleSet object. Attach this
    // object to the firewall.

    if (fwbdebug) qDebug("IPTImporter::finalize()");

    if (haveFirewallObject())
    {
        FWObject *f = getFirewallObject();
        f->setStr("host_OS", "linux24");

        FWOptions  *fwopt = Firewall::cast(f)->getOptionsObject();
        assert(fwopt!=NULL);

        fwopt->setBool("firewall_is_part_of_any_and_networks", false);

        FWObject *policy =
            getFirewallObject()->getFirstByType(Policy::TYPENAME);
        assert( policy!=NULL );

        UnidirectionalRuleSet *rs = getUnidirRuleSet("filter");
        assert(rs!=NULL);

        FWObject::iterator i;
        for (i=rs->ruleset->begin(); i!=rs->ruleset->end(); ++i)
        {
            policy->add(*i);
        }

        // call clearChidren() not recursive because children objects
        // of all rules should not be deleted
        rs->ruleset->clearChildren(false);
        getFirewallObject()->remove(rs->ruleset, false);
        delete rs->ruleset;


        FWObject *nat =
            getFirewallObject()->getFirstByType(NAT::TYPENAME);
        assert( nat!=NULL );

        rs = getUnidirRuleSet("nat");
        if (rs!=NULL)
        {
            for (i=rs->ruleset->begin(); i!=rs->ruleset->end(); ++i)
            {
                nat->add(*i);
            }

            rs->ruleset->clearChildren(false);
            getFirewallObject()->remove(rs->ruleset, false);
            delete rs->ruleset;
        }

        return getFirewallObject();
    }
    else
        return NULL;
}
