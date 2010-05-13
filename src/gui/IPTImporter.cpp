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


#include "IPTImporter.h"

#include <ios>
#include <iostream>
#include <algorithm>
#include <map>

#ifndef _WIN32
#  include <netdb.h>
#  include <netinet/in.h>
#else
#  include <winsock2.h>
#endif

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/CustomService.h"

#include <QString>
#include <QtDebug>


using namespace std;
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
    length_spec = "";
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
    return std::pair<int,int>(convertPort(range.first, proto, 0),
                              convertPort(range.second, proto, 65535));
}

int IPTImporter::convertPort(const std::string &port_spec,
                             const char *proto,
                             int default_port)
{
    int port = 0;
    std::string ps = strip(port_spec);
    if (ps == "") return 0;
    if (ps == ":") return default_port;

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
        qDebug() << "Creating service " << proto.c_str();
        qDebug() << "src range: "
                 << src_range.first.c_str()
                 << " - "
                 << src_range.second.c_str();
        qDebug() << "dst range: "
                 << dst_range.first.c_str()
                 << " - "
                 << dst_range.second.c_str();
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
            qDebug() <<
                QString("Group of %1 services with name '%2', sig '%3'")
                .arg(proto.c_str()).arg(name.c_str()).arg(sig.c_str());

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

    /*
     * Importer::addSrv() adds regular (IP/ICMP/UDP/TCP) service
     * object.  If we have mark module match, implement it as
     * TagService object only if there is no IP/ICMP/UDP/TCP service
     * as well. Other modules, such as length, are added only if there
     * is nothing else. If we have more than one service to deal with,
     * mark rule as bad and issue warning.
     *
     * I check and issue warning after I try to add TagService because
     * I want to add it in case when there are no regular services
     * but there is "mark" and some other module in the original rule.
     * Priorities: 1) IP/ICMP/UDP/TCP service 2) TagService (module mark)
     * 3) any other module
     *
     */
    Importer::addSrv();

    int count_services = 0;
    if (!rule->getSrv()->isAny()) count_services++;
    if (!match_mark.empty()) count_services++;
    if (!length_spec.empty()) count_services++;

    if (rule->getSrv()->isAny() && !match_mark.empty())
    {
        srv->addRef( getTagService(match_mark) );
    }

    if (rule->getSrv()->isAny() && !length_spec.empty())
    {
        // create custom service with module "length"
        srv->addRef(getCustomService(
                        "iptables", "-m length --length " + length_spec, ""));
    }

    if (count_services > 1)
    {
        QString err;
        if (!match_mark.empty())
            err = QObject::tr("Original rule combines match of tcp/udp/icmp \n"
                              "protocols with one or more module matches, such as \n"
                              "module 'mark' or 'length'. Can not translate it \n"
                              "into one fwbuilder rule, you need to reimplement \n"
                              "this complex match using branch and additional \n"
                              "rule.");
        markCurrentRuleBad(err.toUtf8().constData());
    }
}

void IPTImporter::pushRule()
{
//    assert(current_ruleset!=NULL);
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
        std::string mark_code = action_params["set_mark"];
        FWObject *tag_service = getTagService(mark_code);
        rule->setTagObject(tag_service);
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

    if (target=="TOS")
    {
        // special-case target TOS, create custom action
        // this is not very useful though because compiler can not properly
        // put such rule in POSTROUTING chain.
        action = PolicyRule::Custom;
        ropt->setStr("custom_str",
                     "-j TOS --set-tos " + action_params["set_tos"]);
    }

    if (action==PolicyRule::Unknown)
    {
        if (fwbdebug)
            qDebug("Unknown target %s, creating branch", target.c_str());

        // unknown target, consider it a branch
        //
        std::string branch_ruleset_name = target;
        action = PolicyRule::Branch;
        UnidirectionalRuleSet *rs = branch_rulesets[branch_ruleset_name];
        if (rs==NULL)
            rs = getUnidirRuleSet(branch_ruleset_name);

        branch_rulesets[branch_ruleset_name] = rs;

        //current_rule->add(rs->ruleset);
        //ropt->setStr("branch_name", branch_ruleset_name);
        //getFirewallObject()->remove(rs->ruleset, false);

        rs->ruleset->setName(target);
        rule->setBranch(rs->ruleset);
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

    if (current_state == "RELATED,ESTABLISHED")
    {
        if (rule->getSrc()->isAny() &&
            rule->getDst()->isAny() &&
            rule->getSrv()->isAny())
        {
            fwopt->setBool("accept_established", true);
            skip_rule = true;
            *Importer::logger
                << "Using automatic rule controlled by option "
                << "'Accept established,related states' to match "
                << "states RELATED,ESTABLISHED"
                << "\n";
        } else
        {
            RuleElementSrv *srv = rule->getSrv();
            std::string protocol = "";
            if (!rule->getSrv()->isAny())
            {
                Service *srv_obj = Service::cast(FWServiceReference::getObject(
                                                     srv->front()));
                protocol = srv_obj->getProtocolName();
            }
            FWObject *established = getCustomService(
                "iptables", "-m state --state RELATED,ESTABLISHED", protocol);
            srv->clearChildren();
            srv->addRef(established);
        }
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
        rule_comment += "Chain " + current_chain + ". ";

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
        RuleSet *current_ruleset = NULL;
        std::string ruleset_name = "";
        if (isStandardChain(current_chain))
        {
            current_ruleset = RuleSet::cast(
                getFirewallObject()->getFirstByType(Policy::TYPENAME));
            assert(current_ruleset!=NULL);
            current_ruleset->add(current_rule);
        } else
        {
            UnidirectionalRuleSet *rs = getUnidirRuleSet(current_chain);
            assert(rs!=NULL);
            rs->ruleset->add(current_rule);
            current_ruleset = rs->ruleset;
        }
    
        rule->setDirection(PolicyRule::Both);

        if ( !i_intf.empty() && !o_intf.empty())
        {
            // The rule defines inbound and outbound interfaces simultaneously.
            // -i i_intf
            // -o o_intf
            // Making this rule inbound on i_intf, with action Branch
            // Branch points to a new rule set where we put a rule with
            // direction outbount on o_intf

            string branch_ruleset_name = current_ruleset->getName() + "_" + o_intf;

            action = PolicyRule::Branch;

            UnidirectionalRuleSet *rs = branch_rulesets[branch_ruleset_name];
            if (rs==NULL)
                rs = getUnidirRuleSet(branch_ruleset_name);
            branch_rulesets[branch_ruleset_name] = rs;
            rs->ruleset->setName(branch_ruleset_name);

            FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
            PolicyRule *new_rule = PolicyRule::cast(dbroot->create(PolicyRule::TYPENAME));
            FWOptions  *ropt = new_rule->getOptionsObject();
            assert(ropt!=NULL);
            // note that this new rule only matches interface and
            // direction, everything else has been matched by the main
            // rule. There is no need for the rule in the branch to be stateful.
            ropt->setBool("stateless", true);
            rs->ruleset->add(new_rule);

            // Important: at this point we have assembled the
            // current_rule completely. This means all rule elements,
            // its action and options have been set above. By
            // duplicating it into new_rule, we set the same action in
            // the new_rule. We will change interface, direction and
            // action in the current_rule below.
            new_rule->duplicate(rule);
            RuleElement* re;
            re = new_rule->getSrc();   re->reset();
            re = new_rule->getDst();   re->reset();
            re = new_rule->getSrv();   re->reset();
            re = new_rule->getItf();   re->reset();
            
            new_rule->setDirection(PolicyRule::Outbound);
            newInterface(o_intf);
            Interface *intf = all_interfaces[o_intf];
            re = new_rule->getItf();
            re->addRef(intf);

            rule->setDirection(PolicyRule::Inbound);
            newInterface(i_intf);
            intf = all_interfaces[i_intf];
            re =rule->getItf();
            re->addRef(intf);
            rule->setAction(PolicyRule::Branch);
            rule->setBranch(rs->ruleset);

            QString interfaces = QString("-i %1 -o %2").arg(i_intf.c_str()).arg(o_intf.c_str());

            rule_comment += QString(
                " Both inbound and outbound interfaces "
                "in original iptables command: %1").arg(interfaces).toStdString();

            QString log_str = QString("Creating branch ruleset '%1' to "
                                      "match inbound and outbound interfaces %2")
                .arg(branch_ruleset_name.c_str()).arg(interfaces);

            *Importer::logger << log_str.toStdString() << "\n";

            // markCurrentRuleBad(
            //     std::string("Can not set inbound and outbound interface simultaneously. Was: -i ") + i_intf + " -o " + o_intf);
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

    if (src_nm.empty()) src_nm = InetAddr::getAllOnes().toString();
    if (dst_nm.empty()) dst_nm = InetAddr::getAllOnes().toString();
    if (nat_nm.empty()) nat_nm = InetAddr::getAllOnes().toString();

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

    RuleSet *nat = RuleSet::cast(
        getFirewallObject()->getFirstByType(NAT::TYPENAME));
    assert( nat!=NULL );
    nat->add(current_rule);

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
        FWObject *fw = getFirewallObject();
        fw->setStr("host_OS", "linux24");

        FWOptions  *fwopt = Firewall::cast(fw)->getOptionsObject();
        assert(fwopt!=NULL);

        fwopt->setBool("firewall_is_part_of_any_and_networks", false);

        list<FWObject*> l2 = fw->getByType(Policy::TYPENAME);
        for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
        {
            RuleSet *rs = RuleSet::cast(*i);
            rs->renumberRules();
        }

        l2 = fw->getByType(NAT::TYPENAME);
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

void IPTImporter::newUnidirRuleSet(const std::string &ruleset_name)
{
    if (!isStandardChain(ruleset_name))
        Importer::newUnidirRuleSet(ruleset_name);
}


bool IPTImporter::isStandardChain(const std::string &ipt_chain)
{
    return (ipt_chain == "INPUT" ||
            ipt_chain == "OUTPUT" ||
            ipt_chain == "FORWARD" ||
            ipt_chain == "PREROUTING" ||
            ipt_chain == "POSTROUTING");
}

