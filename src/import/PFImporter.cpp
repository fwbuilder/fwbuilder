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

#include "../../config.h"

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

    src_neg = false;
    dst_neg = false;
    tmp_neg = false;

    tmp_port_def = "";
    tmp_port_op = "";
    src_port_group.clear();
    dst_port_group.clear();
    tmp_port_group.clear();

    icmp_type_code_group.clear();

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
    RuleElement *re = rule->getSrv();

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
        }

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
                    "",
                    "",
                    false,  // dest.
                    protocol.c_str(),
                    flags_check.c_str(),
                    flags_mask.c_str());

                re->addRef(commitObject(service_maker->createObject(sig)));

            } else
            {
                list<PortSpec>::iterator psi;

                for (psi=src_port_group.begin();
                     psi!=src_port_group.end(); ++psi)
                {
                    PortSpec ps = *psi;
                    ObjectSignature sig(error_tracker);
                    QString port_spec = 
                        QString("%1 %2")
                        .arg(ps.port1.c_str()).arg(ps.port2.c_str());

                    buildTCPUDPObjectSingature(
                        &sig,
                        ps.port_op.c_str(),
                        port_spec,
                        true,
                        protocol.c_str(),
                        flags_check.c_str(),
                        flags_mask.c_str());
            
                    re->addRef(commitObject(service_maker->createObject(sig)));

                    if (ps.port_op == "!=") re->setNeg(true);
                }

                for (psi=dst_port_group.begin();
                     psi!=dst_port_group.end(); ++psi)
                {
                    PortSpec ps = *psi;
                    ObjectSignature sig(error_tracker);
                    QString port_spec = 
                        QString("%1 %2")
                        .arg(ps.port1.c_str()).arg(ps.port2.c_str());

                    buildTCPUDPObjectSingature(
                        &sig,
                        ps.port_op.c_str(),
                        port_spec,
                        false,
                        protocol.c_str(),
                        flags_check.c_str(),
                        flags_mask.c_str());
            
                    re->addRef(commitObject(service_maker->createObject(sig)));

                    if (ps.port_op == "!=") re->setNeg(true);
                }
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

bool PFImporter::buildTCPUDPObjectSingature(ObjectSignature *sig,
                                            const QString &port_op,
                                            const QString &port_spec,
                                            bool source,
                                            const QString &protocol,
                                            const QString &flags_check,
                                            const QString &flags_mask)
{
    if (protocol == "tcp")
        sig->type_name = TCPService::TYPENAME;
    else
        sig->type_name = UDPService::TYPENAME;

    bool range_inclusive = true;
    QString port_op_cisco_style;

    // map port operations from PF to Cisco-like

    if (port_op == "=")  port_op_cisco_style = "eq";
    if (port_op == "!=") port_op_cisco_style = "eq";
    if (port_op == "<=") port_op_cisco_style = "lt";
    if (port_op == ">=") port_op_cisco_style = "gt";

    if (port_op == "<")
    {
        range_inclusive = false;
        port_op_cisco_style = "lt";
    }

    if (port_op == ">")
    {
        range_inclusive = false;
        port_op_cisco_style = "gt";
    }

    if (port_op == "><")
    {
        range_inclusive = false;
        port_op_cisco_style = "range";
    }

    if (port_op == ":")
    {
        range_inclusive = true;
        port_op_cisco_style = "range";
    }

    if (port_op == "<>")
    {
        error_tracker->registerError(
            QObject::tr("'except ranges' ('<>') for port numbers "
                        "are not supported yet."));
        return false;
    }

    sig->port_range_inclusive = range_inclusive;
    if (source)
        sig->setSrcPortRangeFromPortOp(port_op_cisco_style,
                                       port_spec, protocol);
    else
        sig->setDstPortRangeFromPortOp(port_op_cisco_style,
                                       port_spec, protocol);

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
        switch (flags_str.at(i).toAscii())
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
}

FWObject* PFImporter::makeAddressObj(AddressSpec &as)
{
    if (as.at == AddressSpec::ANY) return NULL;

    if (as.at == AddressSpec::INTERFACE_NAME)
    {
        Interface *intf = getInterfaceByName(as.address);
        assert(intf!=NULL);
        return intf;
    }

    if (as.at == AddressSpec::INTERFACE_NETWORK)
    {
        Interface *intf = getInterfaceByName(as.address);
        if (intf == NULL)
        {
            // this interface was never used in "on <intf>" clause before
            newInterface(as.address);
            intf = getInterfaceByName(as.address);
        }

        string name = intf->getName() + "-net";
        ObjectMaker maker(Library::cast(library), error_tracker);
        AttachedNetworks *an =
            AttachedNetworks::cast(
                maker.createObject(intf, AttachedNetworks::TYPENAME, name));
        an->setRunTime(true);
        an->setSourceName(intf->getName());
        address_table_registry[name.c_str()] = an;
        return an;
    }

    if (as.at == AddressSpec::INTERFACE_BROADCAST)
    {
        error_tracker->registerError(
            QString("import of 'interface:broadcast' is not supported."));
        return NULL;
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
            return NULL;
        }
    }

    if (as.at == AddressSpec::TABLE)
    {
        return address_table_registry[as.address.c_str()];
    }

    return NULL;
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
    if (rule_type == NATRule::Unknown)
        pushPolicyRule();
    else
        pushNATRule();

    assert(current_rule!=NULL);

    if (error_tracker->hasErrors())
    {
        QStringList err = error_tracker->getErrors();
        addMessageToLog("Error: " + err.join("\n"));
        markCurrentRuleBad();
    }

    current_rule = NULL;
    rule_comment = "";

    clear();

}

void PFImporter::pushPolicyRule()
{
    RuleSet *ruleset = RuleSet::cast(
        getFirewallObject()->getFirstByType(Policy::TYPENAME));

    assert(current_rule!=NULL);
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
    assert(ropt!=NULL);

    if (action=="pass")
    {
        if (quick)
            rule->setAction(PolicyRule::Accept);
        else
            rule->setAction(PolicyRule::Continue);
        ropt->setBool("stateless", false);
    }

    if (action=="drop")
    {
        rule->setAction(PolicyRule::Deny);
        ropt->setBool("stateless", true);
    }

    if (direction == "in") rule->setDirection(PolicyRule::Inbound);
    if (direction == "out") rule->setDirection(PolicyRule::Outbound);
    if (direction == "") rule->setDirection(PolicyRule::Both);

    QStringList interfaces;
    list<InterfaceSpec>::iterator it;
    for (it=iface_group.begin(); it!=iface_group.end(); ++it)
    {
        Interface *intf = getInterfaceByName(it->name);
        assert(intf!=NULL);
        RuleElement *re =rule->getItf();
        re->addRef(intf);
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
        rule->setTagging(tobj != NULL);
        rule->setTagObject(tobj);
    }

    /*
     * Set queueing rule option using variable queue
     */
    if (! queue.empty()) ropt->setStr("pf_classify_str", queue);

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

    assert(current_rule!=NULL);
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

        return fw;
    }
    else
    {
        return NULL;
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
    return NULL;
}
    
void PFImporter::newAddressTableObject(const string &name, const string &file)
{
    ObjectMaker maker(Library::cast(library), error_tracker);
    AddressTable *at =  AddressTable::cast(
        commitObject(maker.createObject(AddressTable::TYPENAME, name.c_str())));
    assert(at!=NULL);
    at->setRunTime(true);
    at->setSourceName(file);
    address_table_registry[name.c_str()] = at;

    addMessageToLog(QString("Address Table: <%1> file %2")
                    .arg(name.c_str()).arg(file.c_str()));
}

void PFImporter::newAddressTableObject(const string &name,
                                       list<AddressSpec> &addresses)
{
    ObjectMaker maker(Library::cast(library), error_tracker);
    FWObject *og =  
        commitObject(maker.createObject(ObjectGroup::TYPENAME, name.c_str()));
    assert(og!=NULL);
    address_table_registry[name.c_str()] = og;

    list<AddressSpec>::iterator it;
    for (it=addresses.begin(); it!=addresses.end(); ++it)
    {
        FWObject *obj = makeAddressObj(*it);
        if (obj) og->addRef(obj);
    }
}

