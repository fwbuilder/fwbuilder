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


#include "PFImporter.h"

#include <ios>
#include <iostream>
#include <algorithm>
#include <memory>

#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include "fwbuilder/Address.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AttachedNetworks.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TCPUDPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"

#include "../libgui/platforms.h"

#include <QString>
#include <QtDebug>

extern int fwbdebug;

// TODO: this should move to some common library, together with
// getVersionsForPlatform() it uses. Currently these functions are
// defined in libgui/platforms.cpp

extern QString findBestVersionMatch(const QString &platform,
                                    const QString &discovered_version);

using namespace std;
using namespace libfwbuilder;


PFImporter::PFImporter(FWObject *lib,
                         std::istringstream &input,
                         Logger *log,
                         const std::string &fwname) :
    Importer(lib, "pf", input, log, fwname)
{
    setPlatform("pf");
    address_maker->setInvertedNetmasks(false);

    icmp_codes_to_reject_parameters["net-unr"] = "ICMP net unreachable";
    icmp_codes_to_reject_parameters["0"] = "ICMP net unreachable";
    icmp_codes_to_reject_parameters["host-unr"] = "ICMP host unreachable";
    icmp_codes_to_reject_parameters["1"] = "ICMP host unreachable";
    icmp_codes_to_reject_parameters["proto-unr"] = "ICMP protocol unreachable";
    icmp_codes_to_reject_parameters["2"] = "ICMP protocol unreachable";
    icmp_codes_to_reject_parameters["port-unr"] = "ICMP port unreachable";
    icmp_codes_to_reject_parameters["3"] = "ICMP port unreachable";

    icmp_codes_to_reject_parameters["net-prohib"] = "ICMP net prohibited";
    icmp_codes_to_reject_parameters["9"] = "ICMP net prohibited";
    icmp_codes_to_reject_parameters["host-prohib"] = "ICMP host prohibited";
    icmp_codes_to_reject_parameters["10"] = "ICMP host prohibited";
    icmp_codes_to_reject_parameters["filter-prohib"] = "ICMP admin prohibited";
    icmp_codes_to_reject_parameters["13"] = "ICMP admin prohibited";

}

PFImporter::~PFImporter()
{
}

void PFImporter::clear()
{
    rule_type = NATRule::Unknown;
    quick = false;

    direction = "";
    address_family = "";

    iface_group.clear();
    proto_list.clear();
    tmp_group.clear();
    src_group.clear();
    dst_group.clear();
    nat_group.clear();

    src_neg = false;
    dst_neg = false;
    tmp_neg = false;

    tmp_port_def = "";
    tmp_port_op = "";
    src_port_group.clear();
    dst_port_group.clear();
    nat_port_group.clear();
    tmp_port_group.clear();

    icmp_type_code_group.clear();

    block_action_params.clear();

    queue = "";
    state_op = "";
    logopts = "";
    flags_check = "";
    flags_mask = "";
    tag = "";
    tagged = "";
    tagged_neg = false;

    route_type = UNKNOWN;
    route_group.clear();

    pooltype_opt = "";
    nat_rule_opt_2 = "";

    // Do not clear name-value pairs lists of timeout and limits since
    // they is filled when we parse "set timeout", "set limit"
    // commands and then used in finalize() 

    scrub_rule = false;

    Importer::clear();
}

void PFImporter::clearTempVars()
{
    Importer::clear();
}

void PFImporter::addSrc()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    RuleElement *re = rule->getSrc();

    list<AddressSpec>::iterator it;
    for (it=src_group.begin(); it!=src_group.end(); ++it)
    {
        FWObject *obj = makeAddressObj(*it);
        if (obj) re->addRef(obj);
    }
}

void PFImporter::addDst()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    RuleElement *re = rule->getDst();

    list<AddressSpec>::iterator it;
    for (it=dst_group.begin(); it!=dst_group.end(); ++it)
    {
        FWObject *obj = makeAddressObj(*it);
        if (obj) re->addRef(obj);
    }
}

void PFImporter::addSrv()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    addServiceObjectsToRE(rule->getSrv());
}

void PFImporter::addServiceObjectsToRE(RuleElement *re)
{
    list<string>::iterator it;
    for (it=proto_list.begin(); it!=proto_list.end(); ++it)
    {
        // TODO: need better interface to Importer::makeSrvObj()
        // function and other functions that it uses.
        protocol = *it;
        if (protocol == "icmp")
        {
            list<IcmpSpec>::iterator i1;
            for (i1=icmp_type_code_group.begin();
                 i1!=icmp_type_code_group.end(); ++i1)
            {
                IcmpSpec is = *i1;
                ObjectSignature sig(error_tracker);
                sig.type_name = ICMPService::TYPENAME;

                if ( ! is.icmp_type_name.empty())
                {
                    sig.setIcmpFromName(is.icmp_type_name.c_str());
                } else
                {
                    sig.setIcmpType(is.icmp_type_int.c_str());
                }

                if ( ! is.icmp_code_name.empty())
                {
                    sig.setIcmpCodeFromName(is.icmp_code_name.c_str());
                } else
                {
                    sig.setIcmpCode(is.icmp_code_int.c_str());
                }

                FWObject *s = service_maker->createObject(sig);
                if (s) re->addRef(s);
            }
        } else
        {
            if (protocol == "tcp" || protocol == "udp")
            {
                // TODO: deal with cases where both source and destination
                // ports are matched.
                // See PIXImporter::fixServiceObjectUsedForBothSrcAndDstPorts()

                if (src_port_group.size() == 0 && dst_port_group.size() == 0)
                {
                    // protocol has been defined but not ports to match

                    ObjectSignature sig(error_tracker);

                    buildTCPUDPObjectSingature(
                        &sig,
                        PortSpec(),
                        PortSpec(),
                        protocol.c_str(),
                        flags_check.c_str(),
                        flags_mask.c_str());

                    re->addRef(commitObject(service_maker->createObject(sig)));

                } else
                {
                    addTCPUDPServiceObjectsToRE(re,
                                                protocol,
                                                src_port_group,
                                                dst_port_group,
                                                false);
                }
            } else
            {
                // protocol is not icmp, udp or tcp
                ObjectSignature sig(error_tracker);
                sig.type_name = IPService::TYPENAME;
                sig.setProtocol(protocol.c_str());
                sig.fragments = fragments;
                re->addRef(commitObject(service_maker->createObject(sig)));
            }
        }

    }

    if (! tagged.empty())
    {
        ObjectSignature sig(error_tracker);
        sig.type_name = TagService::TYPENAME;
        sig.tag = tagged.c_str();
        re->addRef( commitObject(service_maker->createObject(sig)) );
        if (tagged_neg) re->setNeg(true);
        tagged = "";
    }

}

void PFImporter::addTCPUDPServiceObjectsToRE(
    RuleElement *re,
    const std::string &protocol,
    const list< PortSpec > &src_port_list,
    const list< PortSpec > &dst_port_list,
    bool for_nat_rhs)
{
    list<PortSpec> src_ports = src_port_list;
    if (src_port_list.size() == 0) src_ports.push_back(PortSpec());

    list<PortSpec> dst_ports = dst_port_list;
    if (dst_port_list.size() == 0) dst_ports.push_back(PortSpec());

    list<PortSpec>::const_iterator psis;
    list<PortSpec>::const_iterator psid;

    for (psis=src_ports.begin(); psis!=src_ports.end(); ++psis)
    {
        PortSpec ps_src = *psis;

        for (psid=dst_ports.begin(); psid!=dst_ports.end(); ++psid)
        {
            PortSpec ps_dst = *psid;

            ObjectSignature sig(error_tracker);

            buildTCPUDPObjectSingature(
                &sig,
                ps_src,
                ps_dst,
                protocol.c_str(),
                (for_nat_rhs) ? "" : flags_check.c_str(),
                (for_nat_rhs) ? "" : flags_mask.c_str());
            
            re->addRef(commitObject(service_maker->createObject(sig)));

            if (!for_nat_rhs && ps_dst.port_op == "!=") re->setNeg(true);
        }
    }

}

void PFImporter::addOSrc()
{
    NATRule *rule = NATRule::cast(current_rule);
    RuleElement *re = rule->getOSrc();

    list<AddressSpec>::iterator it;
    for (it=src_group.begin(); it!=src_group.end(); ++it)
    {
        FWObject *obj = makeAddressObj(*it);
        if (obj) re->addRef(obj);
    }
}

void PFImporter::addODst()
{
    NATRule *rule = NATRule::cast(current_rule);
    RuleElement *re = rule->getODst();

    list<AddressSpec>::iterator it;
    for (it=dst_group.begin(); it!=dst_group.end(); ++it)
    {
        FWObject *obj = makeAddressObj(*it);
        if (obj) re->addRef(obj);
    }
}

void PFImporter::addOSrv()
{
    NATRule *rule = NATRule::cast(current_rule);
    addServiceObjectsToRE(rule->getOSrv());
}

void PFImporter::addTSrc()
{
    NATRule *rule = NATRule::cast(current_rule);
    RuleElement *re = rule->getTSrc();

    list<AddressSpec>::iterator it;
    for (it=nat_group.begin(); it!=nat_group.end(); ++it)
    {
        FWObject *obj = makeAddressObj(*it);
        if (obj) re->addRef(obj);
    }
}

void PFImporter::addTDst()
{
    NATRule *rule = NATRule::cast(current_rule);
    RuleElement *re = rule->getTDst();

    list<AddressSpec>::iterator it;
    for (it=nat_group.begin(); it!=nat_group.end(); ++it)
    {
        FWObject *obj = makeAddressObj(*it);
        if (obj) re->addRef(obj);
    }
}

void PFImporter::addTSrvSNAT()
{
    NATRule *rule = NATRule::cast(current_rule);
    if (protocol == "tcp" || protocol == "udp")
    {
        addTCPUDPServiceObjectsToRE(rule->getTSrv(),
                                    protocol,
                                    nat_port_group,
                                    list< PortSpec >(),
                                    true);  // for_nat_rhs
    }
}

void PFImporter::addTSrvDNAT()
{
    NATRule *rule = NATRule::cast(current_rule);
    if (protocol == "tcp" || protocol == "udp")
    {
        addTCPUDPServiceObjectsToRE(rule->getTSrv(), 
                                    protocol,
                                    list< PortSpec >(),
                                    nat_port_group,
                                    true);  // for_nat_rhs
    }
}

/*
 * Set source and destination port ranges in the object signature
 * object.
 */
bool PFImporter::buildTCPUDPObjectSingature(ObjectSignature *sig,
                                            const PortSpec &src_port,
                                            const PortSpec &dst_port,
                                            const QString &protocol,
                                            const QString &flags_check,
                                            const QString &flags_mask)
{

    if (protocol == "tcp")
        sig->type_name = TCPService::TYPENAME;
    else
        sig->type_name = UDPService::TYPENAME;

    if (src_port.port_op == "<>" || dst_port.port_op == "<>")
    {
        error_tracker->registerError(
            QObject::tr("'except ranges' ('<>') for port numbers "
                        "are not supported yet."));
        return false;
    }

    if (src_port.port_op == "!=" || dst_port.port_op == "!=")
    {
        error_tracker->registerError(
            QObject::tr("'Port not equal' operation is not supported yet."));
    }

    sig->port_range_inclusive = true;

    sig->setSrcPortRangeFromPortOpForPF(src_port.port_op.c_str(),
                                        src_port.port1.c_str(),
                                        src_port.port2.c_str(),
                                        protocol);
    sig->setDstPortRangeFromPortOpForPF(dst_port.port_op.c_str(),
                                        dst_port.port1.c_str(),
                                        dst_port.port2.c_str(),
                                        protocol);

    if (protocol == "tcp")
    {
        convertTcpFlags(sig->flags_comp, flags_check);
        convertTcpFlags(sig->flags_mask, flags_mask);
    }

    return true;
}

void PFImporter::convertTcpFlags(QList<int> &flags_list,
                                 const QString &flags_str)
{
    for (int i=0; i<flags_str.size(); ++i)
    {
        switch (flags_str.at(i).toLatin1())
        {
        case 'U': flags_list << TCPService::URG; break;
        case 'A': flags_list << TCPService::ACK; break;
        case 'P': flags_list << TCPService::PSH; break;
        case 'R': flags_list << TCPService::RST; break;
        case 'S': flags_list << TCPService::SYN; break;
        case 'F': flags_list << TCPService::FIN; break;
        case 'W': 
        case 'E': 
            error_tracker->registerError(
                QObject::tr("TCP flag matches 'E' and 'W' "
                            "are not supported."));
        }
    }
    qSort(flags_list);
}

FWObject* PFImporter::makeAddressObj(AddressSpec &as)
{
    if (as.at == AddressSpec::ANY) return nullptr;

    if (as.at == AddressSpec::INTERFACE_OR_HOST_NAME)
    {
        interfaceProperties *int_prop =
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                user_choice_host_os);
        if (int_prop->looksLikeInterface(as.address.c_str()))
        {
            Interface *intf = getInterfaceByName(as.address);
            if (intf == nullptr)
            {
                // this interface was never used in "on <intf>" clause before
                newInterface(as.address);
                intf = getInterfaceByName(as.address);
            }
            return intf;
        } else
        {
            QString name = QString::fromUtf8(as.address.c_str());
            if (name.startsWith('$'))
            {
                /*
                 * We perform macro substitutions in
                 * PFImporter::substituteMacros(), however if we get a
                 * host name that starts with a '$' here, then this is
                 * an undefined macro that could not be substituted.
                 * Mark rule as bad but still create run-time DNSName
                 * object.
                 */
                error_tracker->registerWarning(
                    QObject::tr("Macro '%1' was undefined, rule may be broken")
                    .arg(name));
            }
            ObjectSignature sig(error_tracker);
            sig.type_name = DNSName::TYPENAME;
            sig.object_name = name;
            sig.dns_name = name;
            return address_maker->createObject(sig);
        }
    }

    if (as.at == AddressSpec::INTERFACE_NETWORK)
    {
        Interface *intf = getInterfaceByName(as.address);
        if (intf == nullptr)
        {
            // this interface was never used in "on <intf>" clause before
            newInterface(as.address);
            intf = getInterfaceByName(as.address);
        }

        FWObject *o = intf->getFirstByType(AttachedNetworks::TYPENAME);
        if ( o != nullptr )
        {
            return o;
        } else {
            ObjectMaker maker(Library::cast(library), error_tracker);
            string name = intf->getName() + "-net";
            AttachedNetworks *an =
                AttachedNetworks::cast(
                    maker.createObject(intf, AttachedNetworks::TYPENAME, name));
            an->setRunTime(true);
            an->setSourceName(intf->getName());
            address_table_registry[name.c_str()] = an;
            return an;
        }
    }

    if (as.at == AddressSpec::INTERFACE_BROADCAST)
    {
        error_tracker->registerError(
            QObject::tr("import of 'interface:broadcast' is not supported."));
        return nullptr;
    }

    if (as.at == AddressSpec::HOST_ADDRESS)
    {
        return Importer::makeAddressObj(as.address, "");
    }

    if (as.at == AddressSpec::NETWORK_ADDRESS)
    {
        return Importer::makeAddressObj(as.address, as.netmask);
    }

    if (as.at == AddressSpec::SPECIAL_ADDRESS)
    {
        if (as.address == "self") return getFirewallObject();
        {
            error_tracker->registerError(
                QObject::tr("Warning: matching '%1' is not supported")
                .arg(as.address.c_str()));
            return nullptr;
        }
    }

    if (as.at == AddressSpec::TABLE)
    {
        FWObject *at = address_table_registry[as.address.c_str()];
        if (isObjectBroken(at))
        {
            error_tracker->registerError(getBrokenObjectError(at));
        }
        return at;
    }

    return nullptr;
}

void PFImporter::addLogging()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    FWOptions *ropt = rule->getOptionsObject();

/*
  alerts         Immediate action needed           (severity=1)
  critical       Critical conditions               (severity=2)
  debugging      Debugging messages                (severity=7)
  disable        Disable log option on this ACL element, (no log at all)
  emergencies    System is unusable                (severity=0)
  errors         Error conditions                  (severity=3)
  inactive       Keyword for disabling an ACL element
  informational  Informational messages            (severity=6)
  interval       Configure log interval, default value is 300 sec
  notifications  Normal but significant conditions (severity=5)
  warnings       Warning conditions                (severity=4)
*/
    QMap<QString, QString> logging_levels;

    logging_levels["alerts"] = "alert";
    logging_levels["critical"] = "crit";
    logging_levels["debugging"] = "debug";
    logging_levels["emergencies"] = "";
    logging_levels["errors"] = "error";
    logging_levels["informational"] = "info";
    logging_levels["notifications"] = "notice";
    logging_levels["warnings"] = "warning";
    logging_levels["0"] = "";
    logging_levels["1"] = "alert";
    logging_levels["2"] = "crit";
    logging_levels["3"] = "error";
    logging_levels["4"] = "warning";
    logging_levels["5"] = "notice";
    logging_levels["6"] = "info";
    logging_levels["7"] = "debug";

    // QStringList log_levels = getLogLevels("pix");

    rule->setLogging(logging);

    QString log_level_qs = log_level.c_str();
    if ( ! log_level_qs.isEmpty())
    {
        if (logging_levels.count(log_level_qs) != 0)
            ropt->setStr("log_level", logging_levels[log_level_qs].toStdString());
        else
            ropt->setStr("log_level", log_level);

        if (log_level_qs == "disable" || log_level_qs == "inactive")
            ropt->setBool("disable_logging_for_this_rule", true);
    }

    if ( ! log_interval.empty())
    {
        bool ok = false;
        int log_interval_int = QString(log_interval.c_str()).toInt(&ok);
        if (ok)
            ropt->setInt("log_interval", log_interval_int);
    }
}


void PFImporter::pushRule()
{
    if (PolicyRule::isA(current_rule))
        pushPolicyRule();
    else
        pushNATRule();

    assert(current_rule!=nullptr);

    if (error_tracker->hasWarnings())
    {
        QStringList warn = error_tracker->getWarnings();
        // parser errors and warnings are added to the log by
        // PFCfgParser::reportError() and PFCfgParser::reportWarning()
        // so we dont need to add them again here
        foreach(QString w, warn)
        {
            if (!w.startsWith("Parser warning:")) addMessageToLog("Warning: " + w);
        }
        markCurrentRuleBad();
    }

    if (error_tracker->hasErrors())
    {
        QStringList err = error_tracker->getErrors();
        foreach(QString e, err)
        {
            if (!e.startsWith("Parser error:")) addMessageToLog("Error: " + e);
        }
        markCurrentRuleBad();
    }

    current_rule = nullptr;
    rule_comment = "";

    clear();

}

void PFImporter::pushPolicyRule()
{
    RuleSet *ruleset = RuleSet::cast(
        getFirewallObject()->getFirstByType(Policy::TYPENAME));
    
    // this importer does not use UnidirectionalRuleSet objects but
    // base class uses dictionary all_rulesets to do some checks (e.g.
    // countRules()) so we'll create one dummy UnidirectionalRuleSet object
    string ruleset_name = ruleset->getName();
    if (checkUnidirRuleSet(ruleset_name) == nullptr)
    {
        UnidirectionalRuleSet *rs = new UnidirectionalRuleSet();
        rs->name = ruleset_name;
        rs->ruleset = ruleset;
        all_rulesets[ruleset_name] = rs;
    }

    assert(current_rule!=nullptr);
    // populate all elements of the rule

    // Note that standard function
    // setInterfaceAndDirectionForRuleSet() assumes there is only one
    // interface, but here we can have a group.  Information about
    // interfaces (even if there is only one) is stored in the list
    // iface_group
    // 
    // importer->setInterfaceAndDirectionForRuleSet(
    //    "", importer->iface, importer->direction);

    QString message_str = 
        QString("filtering rule: action %1; interfaces: %2");
    
    PolicyRule *rule = PolicyRule::cast(current_rule);

    FWOptions  *ropt = current_rule->getOptionsObject();
    assert(ropt!=nullptr);

    if (action=="pass")
    {
        if (quick)
            rule->setAction(PolicyRule::Accept);
        else
            rule->setAction(PolicyRule::Continue);
        ropt->setBool("stateless", false);
    }

    if (action=="block")
    {
        rule->setAction(PolicyRule::Deny);
        ropt->setBool("stateless", true);

        if (block_action_params.size() > 0)
        {
            string block_return = block_action_params.front();
            block_action_params.pop_front();

            if (block_return == "drop")
            {
                ;  // Action Drop accurately represents "block drop"
            }

            if (block_return == "return")
            {
                rule->setAction(PolicyRule::Reject);

                error_tracker->registerError(
                    QObject::tr(
                        "'block return' is not supported in fwbuilder, "
                        "replacing with 'block return-icmp' "));

                ropt->setStr("action_on_reject", "ICMP admin prohibited");
            }

            if (block_return == "return-rst")
            {
                rule->setAction(PolicyRule::Reject);
                ropt->setStr("action_on_reject", "TCP RST");
            }

            if (block_return == "return-icmp")
            {
                rule->setAction(PolicyRule::Reject);
                ropt->setStr("action_on_reject", "ICMP admin prohibited");
                if (block_action_params.size() > 0)
                {
                    string icmp_code = icmp_codes_to_reject_parameters[
                        block_action_params.front()];
                    block_action_params.pop_front();

                    if (icmp_code.empty())
                        error_tracker->registerError(
                            QObject::tr(
                                "Unrecognized icmp code parameter for "
                                "'block return-icmp' action: %1")
                            .arg(block_action_params.front().c_str()));

                    ropt->setStr("action_on_reject", icmp_code);
                }
            }

        }
    }

    if (direction == "in") rule->setDirection(PolicyRule::Inbound);
    if (direction == "out") rule->setDirection(PolicyRule::Outbound);
    if (direction == "") rule->setDirection(PolicyRule::Both);

    QStringList interfaces;
    list<InterfaceSpec>::iterator it;
    for (it=iface_group.begin(); it!=iface_group.end(); ++it)
    {
        Interface *intf = getInterfaceByName(it->name);
        assert(intf!=nullptr);
        RuleElement *re =rule->getItf();
        re->addRef(intf);
        if (it->neg) re->setNeg(true);
        interfaces << it->name.c_str();
    }

    /*
     * Set state-related rule options using variable state_op
     */

    if (state_op.empty())
    {
        // when "state" keyword is absent, behavior depends on the
        // version because different versions have different
        // defaults. Versions prior to 4.0 treat missing keyword as
        // "no state", while versions past 4.0 treat is as "keep
        // state". See also #2441. Rules with action "block" should
        // always be stateless by default.
        if (XMLTools::version_compare(user_choice_version, "4.0") < 0)
            ropt->setBool("stateless", true);
        else
        {
            if (action == "pass") ropt->setBool("stateless", false);
        }
    }

    if (state_op == "no") ropt->setBool("stateless", true);
    if (state_op == "modulate") ropt->setBool("pf_modulate_state", true);
    if (state_op == "keep") ropt->setBool("stateless", false);
    if (state_op == "synproxy") ropt->setBool("pf_synproxy", true);


    /*
     * Set tagging rule option using variable tag
     */
    if ( ! tag.empty()) 
    {
        ObjectSignature sig(error_tracker);
        sig.type_name = TagService::TYPENAME;
        sig.tag = tag.c_str();
        FWObject *tobj = commitObject(service_maker->createObject(sig));
        rule->setTagging(tobj != nullptr);
        rule->setTagObject(tobj);
    }

    /*
     * Set queueing rule option using variable queue
     */
    if (! queue.empty()) ropt->setStr("pf_classify_str", queue);

    /*
     * route-to options
     *
     */
    if (route_type != UNKNOWN && route_group.size() != 0)
    {
        switch (route_type)
        {
        case ROUTE_TO:
            ropt->setStr("pf_route_option", "route_through"); break;

        case REPLY_TO:
            ropt->setStr("pf_route_option", "route_reply_through"); break;

        case DUP_TO:
            ropt->setStr("pf_route_option", "route_copy_through"); break;

        default: ;
        }

        /*
         * see initialization of routeLoadOptions_pf in function
         * init_platforms() in platforms.cpp for the list of strings
         * recognized by compilers as values of "pf_route_load_option"
         * rule option
         */
        if (pooltype_opt == "bitmask")
            ropt->setStr("pf_route_load_option", "bitmask");

        if (pooltype_opt == "random")
            ropt->setStr("pf_route_load_option", "random");

        if (pooltype_opt == "source-hash")
            ropt->setStr("pf_route_load_option", "source_hash");

        if (pooltype_opt == "round-robin")
            ropt->setStr("pf_route_load_option", "round_robin");


        QStringList route_opt_addr;
        bool has_different_interfaces = false;
        string interface_name;
        QStringList reconstructed_parameter_string;
        list<RouteSpec>::iterator it;
        for (it=route_group.begin(); it!=route_group.end(); ++it)
        {
            RouteSpec &rs = *it;

            Interface *intf = getInterfaceByName(rs.iface);
            if (intf == nullptr)
            {
                // this interface was never used in "on <intf>" clause before
                intf = newInterface(rs.iface);
            }

            if (interface_name.empty()) interface_name = intf->getName();
            if (interface_name != intf->getName())
            {
                has_different_interfaces = true;
            }

            ropt->setStr("pf_route_opt_if", rs.iface);

            QString gw;
            if (rs.netmask.empty())
                gw = rs.address.c_str();
            else
                gw = QString("%1/%2")
                    .arg(rs.address.c_str()).arg(rs.netmask.c_str());

            route_opt_addr << gw;

            reconstructed_parameter_string << QString("(%1 %2)")
                .arg(intf->getName().c_str()).arg(gw);
        }

        if (has_different_interfaces)
        {
            // currently we do not support route-to configuration
            // with multiple interface-gateway pairs. multiple
            // gateway addresses and only one interface
            // are allowed. These rules will be imported partially.
            error_tracker->registerWarning(
                QObject::tr("'route-to' parameters with multiple "
                            "interface-gateway pairs are not supported: \"%1\"")
                .arg(reconstructed_parameter_string.join(", ")));
        }

        ropt->setStr("pf_route_opt_addr", route_opt_addr.join(",").toStdString());

        rule->setRouting( ! ropt->getStr("pf_route_option").empty());
    }

    /*
     * Protocols are in proto_list
     * Source addresses are in src_group
     * Destination addresses are in dst_group
     */

    addSrc();
    addDst();
    addSrv();

    /*
     * Set logging options using variables logging and logopts
     */
    addLogging();

    // then add it to the current ruleset
    ruleset->add(current_rule);

    addStandardImportComment(
        current_rule, QString::fromUtf8(rule_comment.c_str()));

    addMessageToLog(message_str.arg(action.c_str()).arg(interfaces.join(",")));


}
 
void PFImporter::pushNATRule()
{
    RuleSet *ruleset = RuleSet::cast(
        getFirewallObject()->getFirstByType(NAT::TYPENAME));

    // this importer does not use UnidirectionalRuleSet objects but
    // base class uses dictionary all_rulesets to do some checks (e.g.
    // countRules()) so we'll create one dummy UnidirectionalRuleSet object
    string ruleset_name = ruleset->getName();
    if (checkUnidirRuleSet(ruleset_name) == nullptr)
    {
        UnidirectionalRuleSet *rs = new UnidirectionalRuleSet();
        rs->name = ruleset_name;
        rs->ruleset = ruleset;
        all_rulesets[ruleset_name] = rs;
    }

    assert(current_rule!=nullptr);

    QString message_str = 
        QString("nat rule: action %1; interfaces: %2");
    
    NATRule *rule = NATRule::cast(current_rule);

    FWOptions  *ropt = current_rule->getOptionsObject();
    assert(ropt!=nullptr);

    if (action=="nat")   rule->setRuleType(NATRule::SNAT);
    if (action=="rdr")   rule->setRuleType(NATRule::DNAT);
    if (action=="nonat") rule->setRuleType(NATRule::NONAT);

    rule->setAction(NATRule::Translate);

    // remember that even though NATRule has two interface rule elements
    // ("in" and "out"), compiler for PF only uses one, the "outbound" one.
    QStringList interfaces;
    list<InterfaceSpec>::iterator it;
    for (it=iface_group.begin(); it!=iface_group.end(); ++it)
    {
        Interface *intf = getInterfaceByName(it->name);
        assert(intf!=nullptr);
        RuleElement *re =rule->getItfOutb();
        re->addRef(intf);
        if (it->neg) re->setNeg(true);
        interfaces << it->name.c_str();
    }

    addOSrc();
    addODst();
    addOSrv();

    switch (rule->getRuleType())
    {
    case NATRule::SNAT:
        addTSrc();
        addTSrvSNAT();
        break;

    case NATRule::DNAT:
        addTDst();
        addTSrvDNAT();
        break;

    case NATRule::NONAT:
        break;

    default:
        error_tracker->registerError(
            QObject::tr("NAT rules \"%1\" "
                        "are not supported yet.").arg(action.c_str()));
    }

    if (pooltype_opt == "bitmask") ropt->setBool("pf_bitmask", true);
    if (pooltype_opt == "random") ropt->setBool("pf_random", true);
    if (pooltype_opt == "source-hash") ropt->setBool("pf_source_hash", true);
    if (pooltype_opt == "round-robin") ropt->setBool("pf_round_robin", true);

    if (nat_rule_opt_2 == "static-port") ropt->setBool("pf_static_port", true);

    // reset rule type. Rule type is used internally by the compilers
    // and is not stored in the object permanently. Rule type assigned
    // by the compiler may actually differ from what we use here
    // (e.g. some DNAT rules are identified as LB -- load balancing --
    // rules by compilers)
    rule->setRuleType(NATRule::Unknown);

    // then add it to the current ruleset
    ruleset->add(current_rule);

    addStandardImportComment(
        current_rule, QString::fromUtf8(rule_comment.c_str()));

    addMessageToLog(message_str.arg(action.c_str()).arg(interfaces.join(",")));
}

Firewall* PFImporter::finalize()
{
    // scan all UnidirectionalRuleSet objects, set interface and
    // direction in all rules of corresponding RuleSet and merge all
    // UnidirectionalRuleSet into one RuleSet object. Attach this
    // object to the firewall.

    if (fwbdebug) qDebug("PFImporter::finalize()");

    if (haveFirewallObject())
    {
        Firewall *fw = Firewall::cast(getFirewallObject());

        // We can not "discover" host OS just by reading pf.conf file.
        // Assume FreeBSD

        fw->setStr("platform", "pf");

        string host_os = "freebsd";

        fw->setStr("host_OS", host_os);
        Resources::setDefaultTargetOptions(host_os , fw);

        FWOptions* options = fw->getOptionsObject();

        // We may be able to infer at least something about the version
        // from the pf.conf file in the future.
        string version = findBestVersionMatch(
            "pf", discovered_version.c_str()).toStdString();

        if ( ! version.empty()) fw->setStr("version", version);

        rearrangeVlanInterfaces();

        list<FWObject*> l1 = fw->getByType(Policy::TYPENAME);
        for (list<FWObject*>::iterator i=l1.begin(); i!=l1.end(); ++i)
        {
            RuleSet *rs = RuleSet::cast(*i);
            rs->renumberRules();
        }

        // Deal with NAT ruleset
        list<FWObject*> l2 = fw->getByType(NAT::TYPENAME);
        for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
        {
            RuleSet *rs = RuleSet::cast(*i);
            rs->renumberRules();
        }

        // We can not deduce ip addresses of interfaces from just
        // looking at pf.conf so lets just mark them all "dynamic"
        list<FWObject*> l3 = fw->getByTypeDeep(Interface::TYPENAME);
        for (list<FWObject*>::iterator i=l3.begin(); i!=l3.end(); ++i)
        {
            Interface *iface = Interface::cast(*i);
            iface->setUnnumbered(false);
            iface->setDyn(true);
        }

        // Log lines from now on should not start with original file line numbers 
        setCurrentLineNumber(-1);

        // configure timeouts
        
        // mapping between PF timeout names and our option names
        map<string, string> timeout_option_names;

        timeout_option_names["tcp.first"] = "pf_tcp_first";
        timeout_option_names["tcp.opening"] = "pf_tcp_opening";
        timeout_option_names["tcp.established"] = "pf_tcp_established";
        timeout_option_names["tcp.closing"] = "pf_tcp_closing";
        timeout_option_names["tcp.finwait"] = "pf_tcp_finwait";
        timeout_option_names["tcp.closed"] = "pf_tcp_closed";
        timeout_option_names["udp.first"] = "pf_udp_first";
        timeout_option_names["udp.single"] = "pf_udp_single";
        timeout_option_names["udp.multiple"] = "pf_udp_multiple";
        timeout_option_names["icmp.first"] = "pf_icmp_first";
        timeout_option_names["icmp.error"] = "pf_icmp_error";
        timeout_option_names["other.first"] = "pf_other_first";
        timeout_option_names["other.single"] = "pf_other_single";
        timeout_option_names["other.multiple"] = "pf_other_multiple";
        timeout_option_names["adaptive.start"] = "pf_adaptive_start";
        timeout_option_names["adaptive.end"] = "pf_adaptive_end";

        timeout_option_names["frag"] = "pf_timeout_frag";
        timeout_option_names["interval"] = "pf_timeout_interval";

        // looks like we do not support src.track as of 4.3
        // timeout_option_names["src.track"] = "pf_src_track";

        // mapping between PF timeout names and boolean option names that
        // activate setting of the corresponding timeout
        map<string, string> timeout_activation_names;

        timeout_activation_names["tcp.first"] = "pf_set_tcp_first";
        timeout_activation_names["tcp.opening"] = "pf_set_tcp_opening";
        timeout_activation_names["tcp.established"] = "pf_set_tcp_established";
        timeout_activation_names["tcp.closing"] = "pf_set_tcp_closing";
        timeout_activation_names["tcp.finwait"] = "pf_set_tcp_finwait";
        timeout_activation_names["tcp.closed"] = "pf_set_tcp_closed";
        timeout_activation_names["udp.first"] = "pf_set_udp_first";
        timeout_activation_names["udp.single"] = "pf_set_udp_single";
        timeout_activation_names["udp.multiple"] = "pf_set_udp_multiple";
        timeout_activation_names["icmp.first"] = "pf_set_icmp_first";
        timeout_activation_names["icmp.error"] = "pf_set_icmp_error";
        timeout_activation_names["other.first"] = "pf_set_other_first";
        timeout_activation_names["other.single"] = "pf_set_other_single";
        timeout_activation_names["other.multiple"] = "pf_set_other_multiple";
        timeout_activation_names["adaptive.start"] = "pf_set_adaptive";
        timeout_activation_names["adaptive.end"] = "pf_set_adaptive";

        timeout_activation_names["frag"] = "pf_do_timeout_frag";
        timeout_activation_names["interval"] = "pf_do_timeout_interval";

        if (timeouts.size() > 0)
        {
            addMessageToLog(QObject::tr("Configuring timeouts:\n"));

            list<str_tuple>::iterator it;
            for (it=timeouts.begin(); it!=timeouts.end(); ++it)
            {        
                string name = it->first;
                bool ok = false;
                int value = QString(it->second.c_str()).toInt(&ok);

                addMessageToLog(QString("set timeout %1 %2\n")
                                .arg(name.c_str()).arg(value));

                if (timeout_activation_names.count(name) == 0)
                {
                    addMessageToLog(
                        QObject::tr("Error: Unknown timeout name %1\n")
                        .arg(name.c_str()));
                } else
                {
                    options->setBool(timeout_activation_names[name], true);
                    options->setInt(timeout_option_names[name], value);
                }
            }
        }

        // configure limits
        map<string, string> limit_option_names;

        limit_option_names["frags"] = "pf_limit_frags";
        limit_option_names["states"] = "pf_limit_states";
        limit_option_names["src-nodes"] = "pf_limit_src_nodes";
        limit_option_names["tables"] = "pf_limit_tables";
        limit_option_names["tables-entries"] = "pf_limit_table_entries";

        // mapping between PF limit names and boolean option names that
        // activate setting of the corresponding limit
        map<string, string> limit_activation_names;

        limit_activation_names["frags"] = "pf_do_limit_frags";
        limit_activation_names["states"] = "pf_do_limit_states";
        limit_activation_names["src-nodes"] = "pf_do_limit_src_nodes";
        limit_activation_names["tables"] = "pf_do_limit_tables";
        limit_activation_names["tables-entries"] = "pf_do_limit_table_entries";

        if (limits.size() > 0)
        {
            addMessageToLog(QObject::tr("Configuring limits:\n"));

            list<str_tuple>::iterator it;
            for (it=limits.begin(); it!=limits.end(); ++it)
            {        
                string name = it->first;
                bool ok = false;
                int value = QString(it->second.c_str()).toInt(&ok);

                addMessageToLog(QString("set limit %1 %2\n")
                                .arg(name.c_str()).arg(value));

                if (limit_activation_names.count(name) == 0)
                {
                    addMessageToLog(
                        QObject::tr("Error: Unknown limit name %1\n")
                        .arg(name.c_str()));
                } else
                {
                    options->setBool(limit_activation_names[name], true);
                    options->setInt(limit_option_names[name], value);
                }
            }
        }

        // cofigure other "set" commands
        // addMessageToLog(QObject::tr("Configuring set commands:\n"));

        if ( ! set_optimization.empty())
        {
            options->setStr("pf_optimization", set_optimization);
            addMessageToLog(QString("set optimization %1\n")
                            .arg(set_optimization.c_str()));
        }

        if ( ! set_block_policy.empty())
        {
            options->setStr("pf_block_policy", set_block_policy);
            addMessageToLog(QString("set block-policy %1\n")
                            .arg(set_block_policy.c_str()));
        }

        if ( ! set_state_policy.empty())
        {
            options->setStr("pf_state_policy", set_state_policy);
            addMessageToLog(QString("set state-policy %1\n")
                            .arg(set_state_policy.c_str()));
        }

        if (set_skip_on.size() != 0)
        {
            for (list<string>::iterator it=set_skip_on.begin();
                 it!=set_skip_on.end(); ++it)
            {
                string skip_interface_name = *it;

                interfaceProperties *int_prop =
                    interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                        user_choice_host_os);
                if (int_prop->looksLikeInterface(skip_interface_name.c_str()))
                {
                    Interface *intf = getInterfaceByName(skip_interface_name);
                    if (intf == nullptr)
                    {
                        // this interface was never used in "on
                        // <intf>" clause before
                        newInterface(skip_interface_name);
                        intf = getInterfaceByName(skip_interface_name);
                        intf->setUnprotected(true);
                        addMessageToLog(QString("set skip on %1\n")
                                        .arg(intf->getName().c_str()));
                    }
                } else
                {
                    addMessageToLog(
                        QObject::tr("Error: In 'set skip on %1' argument "
                                    "does not look like an interface name\n")
                        .arg(skip_interface_name.c_str()));
                }
            }
        }

        if ( ! set_debug.empty())
        {
            options->setStr("pf_set_debug", set_debug);
            addMessageToLog(QString("set debug %1\n").arg(set_debug.c_str()));
        }

        // Scrub options
        if (scrub_options.size() > 0)
        {
            options->setBool("pf_do_scrub", true);

            list<str_tuple>::iterator it;
            for (it=scrub_options.begin(); it!=scrub_options.end(); ++it)
            {        
                string name = it->first;
                string arg = it->second;

                addMessageToLog(QString("scrub %1 %2\n")
                                .arg(name.c_str()).arg(arg.c_str()));

                if (name == "fragment")
                {
                    if (arg == "reassemble")
                        options->setBool("pf_scrub_reassemble", true);
                    if (arg == "crop")
                        options->setBool("pf_scrub_fragm_crop", true);
                    if (arg == "drop-ovl")
                        options->setBool("pf_scrub_fragm_drop_ovl", true);
                }

                if (name == "reassemble")
                    options->setBool("pf_scrub_reassemble_tcp", true);

                if (name == "no-df")
                    options->setBool("pf_scrub_no_df", true);

                if (name == "min-ttl")
                {
                    options->setBool("pf_scrub_use_minttl", true);
                    options->setStr("pf_scrub_minttl", arg);
                }

                if (name == "max-mss")
                {
                    options->setBool("pf_scrub_use_maxmss", true);
                    options->setStr("pf_scrub_maxmss", arg);
                }

                if (name == "random-id")
                    options->setBool("pf_scrub_random_id", true);
            }
        }

        return fw;
    }
    else
    {
        return nullptr;
    }
}

Interface* PFImporter::getInterfaceByName(const string &name)
{
    map<const string,Interface*>::iterator it;
    for (it=all_interfaces.begin(); it!=all_interfaces.end(); ++it)
    {
        Interface *intf = it->second;
        if (intf->getName() == name)
        {
            return intf;
        }
    }
    return nullptr;
}
    
void PFImporter::newAddressTableObject(const string &name, const string &file)
{
    addMessageToLog(QString("Address Table: <%1> file %2")
                    .arg(QString::fromUtf8(name.c_str()))
                    .arg(QString::fromUtf8(file.c_str())));

    ObjectMaker maker(Library::cast(library), error_tracker);
    ObjectSignature sig(error_tracker);
    sig.type_name = AddressTable::TYPENAME;
    sig.object_name = QString::fromUtf8(name.c_str());

    QString filename = QString::fromUtf8(file.c_str());
    filename.replace("\"", "");

    sig.address_table_name = filename;
    FWObject *at = address_maker->createObject(sig);

    address_table_registry[sig.object_name] = at;
}

void PFImporter::newAddressTableObject(const string &name,
                                       list<AddressSpec> &addresses)
{
    bool has_negations = false;
    QStringList addr_list;
    list<AddressSpec>::iterator it;
    for (it=addresses.begin(); it!=addresses.end(); ++it)
    {
        if (it->neg) has_negations = true;
        QString nm = QString(it->netmask.c_str()).trimmed();
        if (!nm.isEmpty()) nm = "/" + nm;
        addr_list << QString("%1%2%3")
            .arg((it->neg)?"!":"").arg(it->address.c_str()).arg(nm);
    }

    addMessageToLog(QString("Address Table: <%1>:  %2")
                    .arg(QString::fromUtf8(name.c_str()))
                    .arg(addr_list.join(", ")));

    ObjectMaker maker(Library::cast(library), error_tracker);
    FWObject *og =
        commitObject(maker.createObject(ObjectGroup::TYPENAME, name.c_str()));
    assert(og!=nullptr);
    address_table_registry[name.c_str()] = og;

    if (has_negations)
    {
        // can not use error_tracker->registerError() here because
        // tables are created before importer encounters any rules and
        // so this error can not be associated with a rule.
        QString err =
            QObject::tr("Error: import of table definition with negated "
                        "addresses is not supported.");
        addMessageToLog(err);

        err =
            QObject::tr("Address table '%1' has a mix of negated and non-negated "
                        "addresses in the original file.");
        registerBrokenObject(og, err.arg(QString::fromUtf8(name.c_str())));
    }

    for (it=addresses.begin(); it!=addresses.end(); ++it)
    {
        FWObject *obj = makeAddressObj(*it);
        if (obj) og->addRef(obj);
    }
}

