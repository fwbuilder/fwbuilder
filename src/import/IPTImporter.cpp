/*

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

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

#include "../../config.h"

#include "IPTImporter.h"
#include "getProtoByName.h"
#include "getServByName.h"

#include <ios>
#include <iostream>
#include <algorithm>
#include <map>

#include "fwbuilder/Address.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"

#include <QString>
#include <QtDebug>

extern int fwbdebug;

using namespace std;
using namespace libfwbuilder;


IPTImporter::IPTImporter(FWObject *lib,
                         std::istringstream &input,
                         Logger *log,
                         const std::string &fwname) : Importer(lib, "iptables", input, log, fwname)
{
    service_group_name_seed = 0;
    aux_branch_number = 0;
    current_table = "";
    current_chain = "";
    current_state = "";
    current_ruleset = NULL;
    current_rule = NULL;
    last_mark_rule = NULL;

    clear();

    icmp_specs["any"] << "-1" << "-1";
    icmp_specs["echo-reply"] << "0" << "0";
    // all "unreachables"
    icmp_specs["destination-unreachable"] << "3" << "-1";
    icmp_specs["network-unreachable"] << "3" << "0";
    icmp_specs["host-unreachable"] << "3" << "1";
    icmp_specs["protocol-unreachable"] << "3" << "2";
    icmp_specs["port-unreachable"] << "3" << "3";
    icmp_specs["fragmentation-needed"] << "3" << "4";
    icmp_specs["source-route-failed"] << "3" << "5";
    icmp_specs["network-unknown"] << "3" << "6";
    icmp_specs["host-unknown"] << "3" << "7";
    icmp_specs["host-isolated"] << "3" << "8";
    icmp_specs["network-prohibited"] << "3" << "9";
    icmp_specs["host-prohibited"] << "3" << "10";
    icmp_specs["TOS-network-unreachable"] << "3" << "11";
    icmp_specs["TOS-host-unreachable"] << "3" << "12";
    icmp_specs["communication-prohibited"] << "3" << "13";
    icmp_specs["host-precedence-violation"] << "3" << "14";
    icmp_specs["precedence-cutoff"] << "3" << "15";
    icmp_specs["source-quench"] << "4" << "0";
    icmp_specs["redirect"] << "5" << "-1";
    icmp_specs["network-redirect"] << "5" << "0";
    icmp_specs["host-redirect"] << "5" << "1";
    icmp_specs["TOS-network-redirect"] << "5" << "2";
    icmp_specs["TOS-host-redirect"] << "5" << "3";
    icmp_specs["echo-request"] << "8" << "0";
    icmp_specs["router-advertisement"] << "9" << "0";
    icmp_specs["router-solicitation"] << "10" << "0";
    icmp_specs["ttl-exceeded"] << "11" << "0";
    icmp_specs["time-exceeded"] << "11" << "0";
    icmp_specs["ttl-zero-during-transit"] << "11" << "0";
    icmp_specs["ttl-zero-during-reassembly"] << "11" << "1";
    icmp_specs["parameter-problem"] << "12" << "0";
    icmp_specs["ip-header-bad"] << "12" << "0";
    icmp_specs["required-option-missing"] << "12" << "1";
    icmp_specs["timestamp-request"] << "13" << "0";
    icmp_specs["timestamp-reply"] << "14" << "0";
    icmp_specs["information-request"] << "15" << "0";
    icmp_specs["information-reply"] << "16" << "0";
    icmp_specs["address-mask-request"] << "17" << "0";
    icmp_specs["address-mask-reply"] << "18" << "0";

    // mapping between REJECT target argument and our internal name for it.
    // See also comment in IPTImporter::pushPolicyRule()
    reject_action_arg_mapping["icmp-net-unreachable"] =   "ICMP net unreachable";
    reject_action_arg_mapping["net-unreach"] =            "ICMP net unreachable";
    reject_action_arg_mapping["icmp-host-unreachable"] =  "ICMP host unreachable";
    reject_action_arg_mapping["host-unreach"] =           "ICMP host unreachable";
    reject_action_arg_mapping["icmp-proto-unreachable"] = "ICMP protocol unreachable";
    reject_action_arg_mapping["proto-unreach"] =          "ICMP protocol unreachable";
    reject_action_arg_mapping["icmp-port-unreachable"] =  "ICMP port unreachable";
    reject_action_arg_mapping["port-unreach"] =           "ICMP port unreachable";
    reject_action_arg_mapping["icmp-net-prohibited"] =    "ICMP net prohibited";
    reject_action_arg_mapping["net-prohib"] =             "ICMP net prohibited";
    reject_action_arg_mapping["icmp-host-prohibited"] =   "ICMP host prohibited";
    reject_action_arg_mapping["host-prohib"] =            "ICMP host prohibited";
    reject_action_arg_mapping["icmp-admin-prohibited"] =  "ICMP admin prohibited";
    reject_action_arg_mapping["admin-prohib"] =           "ICMP admin prohibited";

}



IPTImporter::~IPTImporter()
{
    clear();
}

void IPTImporter::clear()
{
    Importer::clear();

    if (!src_port_list.empty())   src_port_list.clear();
    if (!dst_port_list.empty())   dst_port_list.clear();
    if (!both_port_list.empty())  both_port_list.clear();
    current_state = "";
    i_intf = "";
    o_intf = "";
    target = "";
    tmp_port_range_start = "";
    tmp_port_range_end = "";
    src_neg = dst_neg = srv_neg = intf_neg = false;
    match_mark = "";
    neg_match_mark = false;
    recent_match = "";
    limit_val = "";
    limit_suffix = "";
    limit_burst = "";
    length_spec = "";
    pkt_type_spec = "";
    if (!action_params.empty()) action_params.clear();
    nat_addr1 = "";
    nat_addr2 = "";
    nat_nm = "";
    nat_port_range_start = "";
    nat_port_range_end = "";

    using_iprange_src = false;
    iprange_src_from = "";
    iprange_src_to = "";
    using_iprange_dst = false;
    iprange_dst_from = "";
    iprange_dst_to = "";
}

string IPTImporter::getBranchName(const std::string &suffix)
{
    ostringstream str;
    str << current_chain << suffix << aux_branch_number;
    aux_branch_number++;
    return str.str();
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

void IPTImporter::startBothMultiPort()
{
    both_port_list.clear();
}

void IPTImporter::pushTmpPortSpecToBothPortList()
{
    both_port_list.push_back(
        str_tuple( tmp_port_range_start, tmp_port_range_end ) );
}


FWObject* IPTImporter::createICMPService()
{
    // TODO: convert icmp_spec to QString
    QString icmpspec = QString(icmp_spec.c_str()).trimmed();
    if ( ! icmpspec.isEmpty())
    {
        // Cisco is trying to be too helpful, they translate many
        // icmp type/code combinations into stings
        if (icmp_specs.count(icmpspec) > 0)
        {
            QStringList pp = icmp_specs[icmpspec];
            icmp_type = pp[0].toStdString();
            icmp_code = pp[1].toStdString();
        } else
        {
            QString err("Import of icmp protocol %1 failed");
            reportError(err.arg(icmpspec));
            icmp_code = "-1";
            icmp_type = "-1";
        }
    }
    icmp_spec = "";

    return Importer::createICMPService();
}

FWObject* IPTImporter::createIPService()
{
    int proto = GetProtoByName::getProtocolByName(protocol.c_str());
    if (proto > -1)
    {
        std::ostringstream s;
        s << proto;
        protocol = s.str();
        //free(pe);
    }
    return Importer::createIPService();

    // struct protoent *pe = getprotobyname(protocol.c_str());
    // if (pe!=NULL)
    // {
    //     std::ostringstream s;
    //     s << pe->p_proto;
    //     protocol = s.str();
    //     //free(pe);
    // }
    // return Importer::createIPService();
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
    QString ps = QString(port_spec.c_str()).trimmed();
    if (ps == "") return 0;
    if (ps == ":") return default_port;

    int port = GetServByName::getPortByName(ps, proto);
    if (port == -1)
    {
        reportError(QString("Port spec %1 is unknown").arg(ps));
        port = 0;
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
        o = service_maker->getTCPService(srs, sre,
                                         drs, dre,
                                         established,
                                         tcp_flags_mask, tcp_flags_comp);
    } else
        o = service_maker->getUDPService(srs, sre, drs, dre);
    return commitObject(o);
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

        ObjectMaker maker(Library::cast(library));
        ServiceGroup *sg = ServiceGroup::cast(
            commitObject(
                commitObject(maker.createObject(ServiceGroup::TYPENAME, name))));

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


FWObject* IPTImporter::makeSrcObj()
{
    if (using_iprange_src)
    {
        return address_maker->createAddressRange(iprange_src_from.c_str(),
                                                 iprange_src_to.c_str());
    } else
        return Importer::makeSrcObj();
}

FWObject* IPTImporter::makeDstObj()
{
    if (using_iprange_dst)
    {
        return address_maker->createAddressRange(iprange_dst_from.c_str(),
                                                 iprange_dst_to.c_str());
    } else
        return Importer::makeDstObj();
}

void IPTImporter::addSrv()
{
    // special case for the multiport module parameter "--ports". This
    // parameter matches source OR destination ports. Will created two
    // separate service objects in the same rule
    if (!both_port_list.empty())
    {
        src_port_list.insert(src_port_list.begin(),
                             both_port_list.begin(), both_port_list.end());
        Importer::addSrv();

        src_port_list.clear();

        dst_port_list.insert(dst_port_list.begin(),
                             both_port_list.begin(), both_port_list.end());

        Importer::addSrv();

        dst_port_list.clear();
    } else
        Importer::addSrv();
}



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

void IPTImporter::processModuleMatches()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    RuleElementSrv* srv = rule->getSrv();
    assert(srv!=NULL);

    FWOptions  *fwopt = getFirewallObject()->getOptionsObject();
    assert(fwopt!=NULL);

    FWOptions  *ropt = current_rule->getOptionsObject();
    assert(ropt!=NULL);

    addAllModuleMatches(rule);

    // functions that addAllModuleMatches() calls actually clear
    // variables match_mark, length_spec etc.

    list<string> module_match_options;
    module_match_options.push_back(match_mark);
    module_match_options.push_back(length_spec);
    module_match_options.push_back(recent_match);
    module_match_options.push_back(pkt_type_spec);

    int branch_depth = 0;
    for(list<string>::iterator it=module_match_options.begin();
        it!=module_match_options.end(); ++it)
    {
        if (!it->empty())
        {
            if (branch_depth)
            {
                // at this time I create branches only one level deep
                QString err = QObject::tr(
                    "Original rule combines match of tcp/udp/icmp \n"
                    "protocols with two or more module matches, such as \n"
                    "module 'mark', 'recent' or 'length'. Use additional \n"
                    "branches to implement this complex match.");
                reportError(err);
                break;
            }

            ostringstream str;
            str << current_chain << "_" << rule->getPosition() << "_mod_match";
            string branch_chain = str.str();
            branch_depth++;

            PolicyRule *new_rule = createPolicyBranch(rule, branch_chain,
                                                      true, true);

            addAllModuleMatches(new_rule);
        }
    }
}

void IPTImporter::addAllModuleMatches(PolicyRule *rule)
{
    addLimitMatch(rule);
    addMarkMatch(rule);
    addLengthMatch(rule);
    addRecentMatch(rule);
    addPktTypeMatch(rule);
}

void IPTImporter::addMarkMatch(PolicyRule *rule)
{
    RuleElementSrv* srv = rule->getSrv();
    assert(srv!=NULL);
    if (rule->getSrv()->isAny() && !match_mark.empty())
    {
        srv->addRef( commitObject(service_maker->getTagService(match_mark.c_str())) );
        if (neg_match_mark) srv->setNeg(true);
        match_mark = "";
    }
}

void IPTImporter::addLengthMatch(PolicyRule *rule)
{
    RuleElementSrv* srv = rule->getSrv();
    assert(srv!=NULL);
    if (rule->getSrv()->isAny() && !length_spec.empty())
    {
        // create custom service with module "length"
        srv->addRef(commitObject(
                        service_maker->getCustomService(
                            "iptables",
                            QString("-m length --length %1").arg(length_spec.c_str()),
                            "")));
        length_spec = "";
    }
}

void IPTImporter::addPktTypeMatch(PolicyRule *rule)
{
    RuleElementSrv* srv = rule->getSrv();
    assert(srv!=NULL);
    if (rule->getSrv()->isAny() && !pkt_type_spec.empty())
    {
        // create custom service with module "pkttype"
        srv->addRef(commitObject(
                        service_maker->getCustomService(
                            "iptables",
                            QString("-m pkttype --pkt-type %1").arg(pkt_type_spec.c_str()),
                            "")));
        pkt_type_spec = "";
    }
}

void IPTImporter::addLimitMatch(PolicyRule *rule)
{
    FWOptions  *ropt = rule->getOptionsObject();
    assert(ropt!=NULL);
    if (target!="LOG" && !limit_val.empty())
    {
        // TODO: this is where we should add support for hashlimit
        ropt->setStr("limit_value", limit_val);
        ropt->setStr("limit_suffix", std::string("/") + limit_suffix);
        if (!limit_burst.empty())
            ropt->setStr("limit_burst", limit_burst);
        limit_val = "";
    }
}

void IPTImporter::addRecentMatch(PolicyRule *rule)
{
    RuleElementSrv* srv = rule->getSrv();
    assert(srv!=NULL);
    if (rule->getSrv()->isAny() && !recent_match.empty())
    {
        // create custom service with module "recent"
        srv->addRef(commitObject(
                        service_maker->getCustomService(
                            "iptables",
                            QString("-m recent %1").arg(recent_match.c_str()),
                            "")));
        recent_match = "";
    }
}

void IPTImporter::addStateMatch(libfwbuilder::PolicyRule *rule, const string &state)
{
    RuleElementSrv* srv = rule->getSrv();
    assert(srv!=NULL);
    if (rule->getSrv()->isAny() && !state.empty())
    {
        // create custom service with module "state"
        srv->addRef(commitObject(
                        service_maker->getCustomService(
                            "iptables", 
                            QString("-m state --state %1").arg(state.c_str()),
                            "")));
        recent_match = "";
    }
}

/**
 * Special method that takes a rule and converts it into a branching
 * rule, creates new rule set object, adds a rule to it and makes this
 * rule a copy of the rule passed as an argument. Returns pointer to
 * the new rule inside the branch rule set. Note that new rule inside
 * the branch rule set is a copy of the original rule, with its action
 * and other attributes. The original rule's action changes however
 * and becomes "Branch".
 */
PolicyRule* IPTImporter::createPolicyBranch(
    PolicyRule *rule, const std::string &branch_ruleset_name,
    bool clear_rule_elements, bool make_stateless)
{
    UnidirectionalRuleSet *rs = branch_rulesets[branch_ruleset_name];
    if (rs==NULL)
        rs = getUnidirRuleSet(branch_ruleset_name, Policy::TYPENAME);
    branch_rulesets[branch_ruleset_name] = rs;
    rs->ruleset->setName(branch_ruleset_name);

    FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
    PolicyRule *new_rule = PolicyRule::cast(dbroot->create(PolicyRule::TYPENAME));
    rs->ruleset->add(new_rule);

    new_rule->duplicate(rule);

    rule->setAction(PolicyRule::Branch);
    rule->setBranch(rs->ruleset);

    FWOptions  *ropt = rule->getOptionsObject();
    assert(ropt!=NULL);
    ropt->setBool("stateless", true);

    if (rule->getParent() != NULL)
    {
        ostringstream str1;
        str1 << "Called from ruleset " << rule->getParent()->getName()
             << ", rule " << rule->getPosition();
        new_rule->setComment(str1.str());
    }

    if (clear_rule_elements)
    {
        RuleElement* re;
        re = new_rule->getSrc();   re->reset();
        re = new_rule->getDst();   re->reset();
        re = new_rule->getSrv();   re->reset();
        re = new_rule->getItf();   re->reset();
    }

    if (make_stateless)
    {
        FWOptions  *ropt = new_rule->getOptionsObject();
        assert(ropt!=NULL);
        ropt->setBool("stateless", true);
    }

    QString l("Created branch %1\n");
    *Importer::logger << l.arg(branch_ruleset_name.c_str()).toUtf8().constData();

    return new_rule;
}

NATRule* IPTImporter::createNATBranch(
    NATRule *rule, const std::string &branch_ruleset_name,
    bool clear_rule_elements)
{
    UnidirectionalRuleSet *rs = branch_rulesets[branch_ruleset_name];
    if (rs==NULL)
        rs = getUnidirRuleSet(branch_ruleset_name, NAT::TYPENAME);
    branch_rulesets[branch_ruleset_name] = rs;
    rs->ruleset->setName(branch_ruleset_name);

    FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
    NATRule *new_rule = NATRule::cast(dbroot->create(NATRule::TYPENAME));
    rs->ruleset->add(new_rule);

    new_rule->duplicate(rule);

    rule->setRuleType(NATRule::NATBranch);
    rule->setBranch(rs->ruleset);

    if (rule->getParent() != NULL)
    {
        ostringstream str1;
        str1 << "Called from ruleset " << rule->getParent()->getName()
             << ", rule " << rule->getPosition();
        new_rule->setComment(str1.str());
    }

    if (clear_rule_elements)
    {
        RuleElement* re;
        re = new_rule->getOSrc();   re->reset();
        re = new_rule->getODst();   re->reset();
        re = new_rule->getOSrv();   re->reset();

        re = new_rule->getTSrc();   re->reset();
        re = new_rule->getTDst();   re->reset();
        re = new_rule->getTSrv();   re->reset();

        re = new_rule->getItfInb();   re->reset();
        re = new_rule->getItfOutb();  re->reset();
    }

    QString l("Created branch %1\n");
    *Importer::logger << l.arg(branch_ruleset_name.c_str()).toUtf8().constData();

    return new_rule;
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
        {
            /*
             * for historical reasons, the argument for action Reject
             * is stored in our XML as a human readable string such as
             * "ICMP admin prohibited" instead of some kind of a
             * computer readable code. Function getActionsOnReject() (see platforms.cpp)
             * returns a list of strings where every even string is one of
             * these codes and every odd string is translatable display name
             * (that by currently is the same string, except it can be translated).
             * Values in the map reject_action_arg_mapping must match internal
             * (untranslated) codes that we get from getActionsOnReject(), keys
             * in reject_action_arg_mapping must match iptables arguments for
             * the target REJECT
             */
            QString iptables_reject_arg = action_params["reject_with"].c_str();
            QString action_on_reject_code;
            if (reject_action_arg_mapping.count(iptables_reject_arg) > 0)
                action_on_reject_code =
                    reject_action_arg_mapping[iptables_reject_arg];
            else
            {
                action_on_reject_code = "ICMP admin prohibited";

                QString err = QObject::tr(
                    "Warning: Line %1: Unknown parameter of target REJECT: %2.\n")
                    .arg(getCurrentLineNumber())
                    .arg(iptables_reject_arg);
                ropt->setStr("color", getBadRuleColor());
                rule_comment += string(err.toUtf8().constData());
                *Importer::logger << err.toUtf8().constData();
            }

            ropt->setStr("action_on_reject", action_on_reject_code.toStdString());
        }
    }

    if (target=="QUEUE")    action = PolicyRule::Pipe;

    if (target=="CLASSIFY") action = PolicyRule::Classify;

    if (target=="LOG")
    {
        action = PolicyRule::Continue;
        rule->setLogging(true);
        QString log_prefix = action_params["log_prefix"].c_str();
        log_prefix.replace("\"", "");
        ropt->setStr("log_prefix", log_prefix.toStdString());
        ropt->setStr("log_tcp_seq", action_params["log_tcp_seq"]);
        ropt->setStr("log_tcp_options", action_params["log_tcp_options"]);
        ropt->setStr("log_ip_options", action_params["log_ip_options"]);

        string slevel = action_params["log_level"];
        int llevel;
        std::istringstream str1(slevel);
        str1.exceptions(std::ios::failbit);
        try
        {
            str1 >> llevel;
            // log level defined as a number
            map<int,string> levels;
            levels[0] = "";
            levels[1] = "alert";
            levels[2] = "crit";
            levels[3] = "error";
            levels[4] = "warning";
            levels[5] = "notice";
            levels[6] = "info";
            levels[7] = "debug";
            if (llevel <= 7)
                ropt->setStr("log_level", levels[llevel]);
            else
            {
                reportError(std::string("Unrecognized log level '") + slevel);
            }

        } catch (const std::exception &ex) {
            // not an integer
            ropt->setStr("log_level", slevel);
        }

        //ropt->setStr("log_level", action_params["log_level"]);
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
        QString log_prefix = action_params["log_prefix"].c_str();
        log_prefix.replace("\"", "");
        ropt->setStr("log_prefix", log_prefix.toStdString());
    }

    if (target=="MARK")
    {
        action = PolicyRule::Tag;
        last_mark_rule = rule;
        std::string mark_code = action_params["set_mark"];
        FWObject *tag_service = commitObject(
            service_maker->getTagService(mark_code.c_str()));
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
        QString err("Warning: Line %1: Using automatic rule controlled by option "
                    "Clamp MSS to MTU\n");
        *Importer::logger << err.arg(getCurrentLineNumber()).toStdString();
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
            rs = getUnidirRuleSet(branch_ruleset_name, Policy::TYPENAME);

        branch_rulesets[branch_ruleset_name] = rs;

        rs->ruleset->setName(target);
        rule->setBranch(rs->ruleset);
        ropt->setBool("stateless", true);
    }

    rule->setAction(action);

    addSrc();
    addDst();
    addSrv();

    RuleElementSrc      *nsrc;
    RuleElementDst      *ndst;

    rule->getSrc()->setNeg(src_neg);
    rule->getDst()->setNeg(dst_neg);
    rule->getSrv()->setNeg(srv_neg);
    rule->getItf()->setNeg(intf_neg);

/* Recognize some typical rule patterns and set firewall and rule
 * options appropriately
 */
    if (current_state == "NEW")
    {
        ropt->setBool("stateless", false);
        current_state = "";
    }

    if (current_state == "RELATED,ESTABLISHED" ||
        current_state == "ESTABLISHED,RELATED")
    {
        RuleElementSrv *srv = rule->getSrv();
        std::string protocol = "";
        FWObject *estab = NULL;

        FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
        FWObject *std_obj = dbroot->findInIndex(FWObjectDatabase::STANDARD_LIB_ID);
        estab = std_obj->findObjectByName(CustomService::TYPENAME, "ESTABLISHED");
        if (estab == NULL)
        {
            estab = commitObject(
                service_maker->getCustomService(
                    "iptables", "-m state --state RELATED,ESTABLISHED", ""));
        }

        if (!rule->getSrv()->isAny())
        {
            string branch_ruleset_name = getBranchName("_established_");

            // two boolean args of createPolicyBranch() clear all rule elements
            // of the rule in the branch rule set and make it stateless
            PolicyRule *new_rule = createPolicyBranch(rule, branch_ruleset_name,
                                                      true, true);

            new_rule->setDirection(PolicyRule::Both);
            RuleElement* re = new_rule->getSrv();
            re->addRef(estab);
        } else
        {
            srv->clearChildren();
            srv->addRef(estab);
        }

        QString err(
            "Warning: Line %1: Rule matches states 'RELATED,ESTABLISHED'. "
            "Consider using "
            "automatic rule controlled by the checkbox in the firewall "
            "settings dialog. Automatic rule matches in all standard chains "
            "which may be different from the original imported configuration. "
            "This requires manual checking."
            "\n");
        *Importer::logger << err.arg(getCurrentLineNumber()).toStdString();

        current_state = "";
    }

    if (rule->getSrc()->isAny() &&
        rule->getDst()->isAny() &&
        rule->getSrv()->isAny() &&
        current_state == "INVALID")
    {
        if (target=="DROP") fwopt->setBool("drop_invalid", true);
        if (target=="LOG")  fwopt->setBool("log_invalid", true);
        skip_rule = true;

        QString err("Warning: Line %1: Using automatic rule controlled by option "
                    "'Drop packet that do not match any known connection' to match "
                    "state INVALID"
                    "\n");
        *Importer::logger << err.arg(getCurrentLineNumber()).toStdString();

        current_state = "";
    }

    // finally, process unrecognized combination of states
    if ( ! current_state.empty())
    {
        RuleElementSrv *srv = rule->getSrv();

        FWObject *state_match_srv = commitObject(
            service_maker->getCustomService(
                "iptables",
                QString("-m state --state %1").arg(current_state.c_str()),
                ""));

        if ( ! rule->getSrv()->isAny())
        {
            string branch_ruleset_name = getBranchName("_state_match_");

            // two boolean args of createPolicyBranch() clear all rule elements
            // of the rule in the branch rule set and make it stateless
            PolicyRule *new_rule = createPolicyBranch(rule, branch_ruleset_name,
                                                      true, true);

            new_rule->setDirection(PolicyRule::Both);
            RuleElement* re = new_rule->getSrv();
            re->addRef(state_match_srv);
        } else
        {
            srv->clearChildren();
            srv->addRef(state_match_srv);
        }

        // no need to make rule stateless since compiler is smart enough to drop
        // --state NEW when service object adds its own state match
        // ropt->setBool("stateless", false);

        QString err(
            "Warning: Line %1: Rule matches combination of states '%2'. "
            "Iptables rules generated by fwbuilder can be stateless (match "
            "no state) or stateful (match state NEW). Fwbuilder also adds "
            "a rule at the top of the script to match states "
            "ESTABLISHED,RELATED. Combination of states '%3' does not fit "
            "these standard cases and to match it, the program created "
            "new Custom Service object.  This may require manual checking."
            "\n");
        *Importer::logger << err
            .arg(getCurrentLineNumber())
            .arg(current_state.c_str())
            .arg(current_state.c_str())
            .toStdString();

        current_state = "";
    }



    if (target=="CONNMARK" &&
        last_mark_rule != NULL &&
        !action_params["connmark_save_mark"].empty())
    {
        FWOptions  *lmr_ropt = last_mark_rule->getOptionsObject();
        assert(lmr_ropt!=NULL);
        lmr_ropt->setBool("ipt_mark_connections", true);
        skip_rule = true;
        QString err("Warning: Line %1: Turned option on in previous rule "
                    "with action Mark "
                    "for '-j CONNMARK --save-mark' "
                    "\n");
        *Importer::logger << err.arg(getCurrentLineNumber()).toStdString();
    }

    if (target=="CONNMARK" &&
        !action_params["connmark_restore_mark"].empty())
    {
        // this rule is added automatically in
        // MangleTableCompiler_ipt::flushAndSetDefaultPolicy()
        // if we have at least one rule with CONNMARK target in the policy
        skip_rule = true;
        QString err("Warning: Line %1: Skip command with '-j CONNMARK --restore-mark' "
                    "This rule is generated automatically."
                    "\n");
        *Importer::logger << err.arg(getCurrentLineNumber()).toStdString();
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
        RuleSet *ruleset = NULL;
        std::string ruleset_name = "";
        if (isStandardChain(current_chain))
        {
            ruleset = RuleSet::cast(
                getFirewallObject()->getFirstByType(Policy::TYPENAME));
            assert(ruleset!=NULL);
            ruleset->add(current_rule);
        } else
        {
            UnidirectionalRuleSet *rs = getUnidirRuleSet(current_chain, Policy::TYPENAME);
            assert(rs!=NULL);
            rs->ruleset->add(current_rule);
            ruleset = rs->ruleset;
        }

        // renumber to clean-up rule positions
        ruleset->renumberRules();

        rule->setDirection(PolicyRule::Both);

        if ( !i_intf.empty() && !o_intf.empty())
        {
            // The rule defines inbound and outbound interfaces simultaneously.
            // -i i_intf
            // -o o_intf
            // Making this rule inbound on i_intf, with action Branch
            // Branch points to a new rule set where we put a rule with
            // direction outbount on o_intf

            action = PolicyRule::Branch;

            string branch_ruleset_name = ruleset->getName() + "_" + o_intf;

            // note that this new rule only matches interface and
            // direction, everything else has been matched by the main
            // rule. There is no need for the rule in the branch to be stateful
            // (that is what the last bool argument for createPolicyBranch() is for)
            PolicyRule *new_rule = createPolicyBranch(rule, branch_ruleset_name,
                                                      true, true);

            // Important: at this point we have assembled the
            // current_rule completely. This means all rule elements,
            // its action and options have been set above. By
            // duplicating it into new_rule, we set the same action in
            // the new_rule. We will change interface, direction and
            // action in the current_rule below.

            RuleElement* re;
            
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

            QString interfaces = QString("-i %1 -o %2").arg(i_intf.c_str()).arg(o_intf.c_str());

            rule_comment += QString(
                " Both inbound and outbound interfaces "
                "in original iptables command: %1").arg(interfaces).toStdString();

            QString log_str("Warning: Line %1: Creating branch ruleset '%2' to "
                            "match inbound and outbound interfaces %3\n");
            *Importer::logger << log_str
                .arg(getCurrentLineNumber())
                .arg(branch_ruleset_name.c_str()).arg(interfaces)
                .toUtf8().constData();
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

        processModuleMatches();

        addStandardImportComment(current_rule, QString::fromUtf8(rule_comment.c_str()));
    }

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

    NATRule::NATRuleTypes rule_type = NATRule::Unknown;

    if (target=="ACCEPT")
    {
        rule_type = NATRule::NONAT;
    }

    if (target=="MASQUERADE")
    {
        rule_type = NATRule::Masq;

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
        rule_type = NATRule::SNAT;

        FWObject *tsrc = NULL;
        if (nat_addr1!=nat_addr2)
            tsrc = commitObject(
                address_maker->createAddressRange(nat_addr1.c_str(), nat_addr2.c_str()));
        else
            tsrc = commitObject(
                address_maker->createAddress(nat_addr1.c_str(), nat_nm.c_str()));

        RuleElementTSrc *re = rule->getTSrc();
        assert(re!=NULL);
        re->addRef(tsrc);

        if (!nat_port_range_start.empty())
        {
            str_tuple empty_range("0", "0");
            str_tuple nat_port_range(nat_port_range_start, nat_port_range_end);
            FWObject *s = createTCPUDPService(nat_port_range, empty_range, protocol);
            RuleElementTSrv *re = rule->getTSrv();
            assert(re!=NULL);
            re->addRef(s);
        }

        if (!o_intf.empty())
        {
            RuleElement *itf_o_re = rule->getItfOutb();
            assert(itf_o_re!=NULL);
            newInterface(o_intf);
            Interface *intf = all_interfaces[o_intf];
            itf_o_re->addRef(intf);
        }

    }

    if (target=="DNAT")
    {
        rule_type = NATRule::DNAT;

        // if chain is "OUTPUT", put fw object in OSrc
        if (current_chain == "OUTPUT")
        {
            RuleElementOSrc *re = rule->getOSrc();
            assert(re!=NULL);
            re->addRef(getFirewallObject());
        }

        FWObject *tdst = NULL;
        if (nat_addr1!=nat_addr2)
            tdst = commitObject(
                address_maker->createAddressRange(nat_addr1.c_str(), nat_addr2.c_str()));
        else
            tdst = commitObject(
                address_maker->createAddress(nat_addr1.c_str(), nat_nm.c_str()));

        RuleElementTDst *re = rule->getTDst();
        assert(re!=NULL);
        re->addRef(tdst);

        if (!nat_port_range_start.empty())
        {
            str_tuple empty_range("0", "0");
            str_tuple nat_port_range(nat_port_range_start, nat_port_range_end);
            FWObject *s = createTCPUDPService(empty_range, nat_port_range, protocol);
            RuleElementTSrv *re = rule->getTSrv();
            assert(re!=NULL);
            re->addRef(s);
        }

        if (!i_intf.empty())
        {
            RuleElement *itf_i_re = rule->getItfInb();
            assert(itf_i_re!=NULL);
            newInterface(i_intf);
            Interface *intf = all_interfaces[i_intf];
            itf_i_re->addRef(intf);
        }
    }

    if (target=="REDIRECT")
    {
        rule_type = NATRule::Redirect;

        RuleElementTDst *re = rule->getTDst();
        assert(re!=NULL);
        re->addRef(getFirewallObject());

        if (!nat_port_range_start.empty())
        {
            str_tuple empty_range("0", "0");
            str_tuple nat_port_range(nat_port_range_start, nat_port_range_end);
            FWObject *s = createTCPUDPService(empty_range, nat_port_range, protocol);
            RuleElementTSrv *re = rule->getTSrv();
            assert(re!=NULL);
            re->addRef(s);
        }

        if ( ! o_intf.empty())
        {
            RuleElement *itf_o_re = rule->getItfOutb();
            assert(itf_o_re!=NULL);
            newInterface(o_intf);
            Interface *intf = all_interfaces[o_intf];
            itf_o_re->addRef(intf);
        }
    }

    if (target=="NETMAP")
    {
        FWObject *o = NULL;

        if (!src_a.empty())
        {
            rule_type = NATRule::SNetnat;

            RuleElementTSrc *tsrc = rule->getTSrc();
            assert(tsrc!=NULL);
            o = commitObject(
                address_maker->createAddress(nat_addr1.c_str(), nat_nm.c_str()));
            tsrc->addRef(o);
        }

        if (!dst_a.empty())
        {
            rule_type = NATRule::DNetnat;

            RuleElementTDst *tdst = rule->getTDst();
            assert(tdst!=NULL);
            o = commitObject(
                address_maker->createAddress(nat_addr1.c_str(), nat_nm.c_str()));
            tdst->addRef(o);
        }
    }

    if (rule_type==NATRule::Unknown)
    {
        if (fwbdebug)
            qDebug("Unknown target %s, creating branch", target.c_str());

        // unknown target, consider it a branch
        //
        std::string branch_ruleset_name = target;

        rule_type = NATRule::NATBranch;
        rule->setAction(NATRule::Branch);

        UnidirectionalRuleSet *rs = branch_rulesets[branch_ruleset_name];
        if (rs==NULL)
        {
            rs = getUnidirRuleSet(branch_ruleset_name, NAT::TYPENAME);
            branch_rulesets[branch_ruleset_name] = rs;
        }

        rs->ruleset->setName(target);

        rule->setBranch(rs->ruleset);
    }

    rule->setRuleType(rule_type);

    //  add rule to the right ruleset
    RuleSet *ruleset = NULL;
    std::string ruleset_name = "";
    if (isStandardChain(current_chain))
    {
        ruleset = RuleSet::cast(
            getFirewallObject()->getFirstByType(NAT::TYPENAME));
        assert(ruleset!=NULL);
        ruleset->add(current_rule);
    } else
    {
        UnidirectionalRuleSet *rs = getUnidirRuleSet(current_chain, NAT::TYPENAME);
        assert(rs!=NULL);
        rs->ruleset->add(current_rule);
        ruleset = rs->ruleset;
    }

    // renumber to clean-up rule positions
    ruleset->renumberRules();

    addStandardImportComment(current_rule, QString::fromUtf8(rule_comment.c_str()));

    // RuleSet *nat = RuleSet::cast(
    //     getFirewallObject()->getFirstByType(NAT::TYPENAME));
    // assert( nat!=NULL );
    // nat->add(current_rule);

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
        Firewall *fw = Firewall::cast(getFirewallObject());

        if (fwbdebug)
            qDebug() << "fw=" << fw;

        fw->setStr("host_OS", "linux24");
        Resources::setDefaultTargetOptions("linux24" , fw);
        fw->setStr("version", "");  // default version "any"
        fw->getManagementObject(); // creates management obj

        FWOptions  *fwopt = fw->getOptionsObject();
        assert(fwopt!=NULL);

        fwopt->setBool("firewall_is_part_of_any_and_networks", false);

        // scan all UnidirectionalRuleSet objects and take care of
        // their default action
        std::map<const string, UnidirectionalRuleSet*>::iterator it;
        for (it=all_rulesets.begin(); it!=all_rulesets.end(); ++it)
        {
            // rs_index is a string composed of the table name and chain name
            // like "filter / FORWARD" or "mangle / PREROUTING"
            // This string is created in IPTImporter::getUnidirRuleSet()
            string rs_index = it->first;
            UnidirectionalRuleSet* rs = it->second;
            if (Policy::isA(rs->ruleset) && rs->default_action == PolicyRule::Accept)
            {
                FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
                PolicyRule *rule = PolicyRule::cast(
                    dbroot->create(PolicyRule::TYPENAME));

                // check if all child objects were populated properly
                FWOptions  *ropt = rule->getOptionsObject();
                assert(ropt != NULL);
                ropt->setBool("stateless", true);

                rule->setAction(PolicyRule::Accept);
                rule->setLogging(false);

                ostringstream str1;
                str1 << "Default iptables policy in " << rs_index;

                rule->setComment(str1.str());

                if (rs->name == "FORWARD")
                {
                    rule->setDirection(PolicyRule::Both);

                    if (rs_index.find("mangle") != string::npos)
                    {
                        QString err = QObject::tr(
                            "Warning: Line %1: Can not reproduce default "
                            "action in table 'mangle' chain 'FORWARD'.\n"
                            "(Generated rule may not generate equivalent "
                            "iptables command when compiled)"
                        );
                        ropt->setStr("color", getBadRuleColor());

                        rule->setComment(
                            err.arg(
                                rs->default_action_line_number).toUtf8().constData());
                        *Importer::logger << err.arg(
                            rs->default_action_line_number).toUtf8().constData();

                    }
                }

                if (rs->name == "INPUT")
                {
                    RuleElementDst* dst = rule->getDst();
                    assert(dst!=NULL);
                    dst->addRef(fw);
                    rule->setDirection(PolicyRule::Inbound);

                    if (rs_index.find("mangle") != string::npos)
                    {
                        QString err = QObject::tr(
                            "Warning: Line %1: Can not reproduce default "
                            "action in table 'mangle' chain 'INPUT'.\n"
                            "(Generated rule may not generate equivalent "
                            "iptables command when compiled)"
                        );
                        ropt->setStr("color", getBadRuleColor());

                        rule->setComment(
                            err.arg(
                                rs->default_action_line_number).toUtf8().constData());
                        *Importer::logger << err.arg(
                            rs->default_action_line_number).toUtf8().constData();

                    }
                }

                if (rs->name == "OUTPUT")
                {
                    RuleElementSrc* src = rule->getSrc();
                    assert(src!=NULL);
                    src->addRef(fw);
                    rule->setDirection(PolicyRule::Outbound);
                }

                if (rs->name == "PREROUTING")
                {
                    rule->setDirection(PolicyRule::Inbound);
                }

                if (rs->name == "POSTROUTING")
                {
                    rule->setDirection(PolicyRule::Outbound);
                }

                rs->ruleset->add(rule);

                QString l("Warning: Line %1: Added rule to reproduce default "
                          "policy ACCEPT in %2\n");

                *Importer::logger <<
                    l.arg(rs->default_action_line_number)
                    .arg(rs_index.c_str()).toUtf8().constData();
            }
        }

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
    {
        return NULL;
    }
}

UnidirectionalRuleSet* IPTImporter::checkUnidirRuleSet(
    const std::string &ruleset_name)
{
    string all_rulesets_index = current_table + "/" + ruleset_name;
    return all_rulesets[all_rulesets_index];
}

UnidirectionalRuleSet* IPTImporter::getUnidirRuleSet(
    const std::string &ruleset_name, const string &ruleset_type_name)
{
    string all_rulesets_index = current_table + "/" + ruleset_name;
    UnidirectionalRuleSet *rs = all_rulesets[all_rulesets_index];
    if (rs == NULL)
    {
        RuleSet *ruleset = NULL;
        FWObjectDatabase *dbroot = getFirewallObject()->getRoot();

        if (isStandardChain(ruleset_name))
        {
            if (ruleset_type_name == NAT::TYPENAME)
                ruleset = RuleSet::cast(
                    getFirewallObject()->getFirstByType(NAT::TYPENAME));
            else
            {
                list<FWObject*> policies = getFirewallObject()->getByType(Policy::TYPENAME);

                if (current_table == "mangle")
                {
                    for (list<FWObject*>::iterator it=policies.begin();
                         it!=policies.end(); ++it)
                    {
                        RuleSet *rs = RuleSet::cast(*it);
                        FWOptions *rulesetopt = rs->getOptionsObject();
                        if (rulesetopt->getBool("mangle_only_rule_set"))
                        {
                            ruleset = rs;
                            break;
                        }
                    }
                    if (ruleset == NULL)
                    {
                        ruleset = RuleSet::cast(dbroot->create(Policy::TYPENAME));
                        FWOptions *rulesetopt = ruleset->getOptionsObject();
                        rulesetopt->setBool("mangle_only_rule_set", true);
                        ruleset->setTop(true);

                        ruleset->setName("Mangle");
                        getFirewallObject()->add(ruleset);
                    }
                } 

                if (current_table == "filter")
                {
                    for (list<FWObject*>::iterator it=policies.begin();
                         it!=policies.end(); ++it)
                    {
                        RuleSet *rs = RuleSet::cast(*it);
                        FWOptions *rulesetopt = rs->getOptionsObject();
                        if (rs->getName() == "Policy" && 
                            !rulesetopt->getBool("mangle_only_rule_set"))
                        {
                            ruleset = rs;
                            break;
                        }
                    }
                    if (ruleset == NULL)
                    {
                        ruleset = RuleSet::cast(dbroot->create(Policy::TYPENAME));
                        FWOptions *rulesetopt = ruleset->getOptionsObject();
                        rulesetopt->setBool("mangle_only_rule_set", false);
                        ruleset->setTop(true);

                        ruleset->setName("Policy");
                        getFirewallObject()->add(ruleset);
                    }
                }
            }

        } else
        {
            ruleset = RuleSet::cast(dbroot->create(ruleset_type_name));
            ruleset->setName(ruleset_name);
            getFirewallObject()->add(ruleset);
        }

        rs = new UnidirectionalRuleSet();
        rs->name = ruleset_name;
        rs->ruleset = ruleset;
        all_rulesets[all_rulesets_index] = rs;
    }

    return rs;
}

void IPTImporter::newUnidirRuleSet(const string &chain_name,
                                   const string &ruleset_type)
{
    current_ruleset = getUnidirRuleSet(chain_name, ruleset_type);  // creates if new
    QString l("Ruleset: %1 / %2\n");
    *Importer::logger << l.arg(current_table.c_str()).arg(current_ruleset->name.c_str())
        .toStdString();
}


bool IPTImporter::isStandardChain(const std::string &ipt_chain)
{
    return (ipt_chain == "INPUT" ||
            ipt_chain == "OUTPUT" ||
            ipt_chain == "FORWARD" ||
            ipt_chain == "PREROUTING" ||
            ipt_chain == "POSTROUTING");
}

