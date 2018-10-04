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


#include "PIXImporter.h"

#include <ios>
#include <iostream>
#include <algorithm>
#include <memory>

#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/AddressRange.h"
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
#include "fwbuilder/Library.h"
#include "fwbuilder/TCPUDPService.h"

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


PIXImporter::PIXImporter(FWObject *lib,
                         std::istringstream &input,
                         Logger *log,
                         const std::string &fwname) :
    IOSImporter(lib, input, log, fwname)
{
    setPlatform("pix");
    address_maker->setInvertedNetmasks(false); // since parent class sets to true
}



PIXImporter::~PIXImporter()
{
    global_pools.clear();
}

/*
 * do not clear named_objects_registry because this function is called
 * when we start processing each named object, object-group and some other
 * lines in the config
 */
void PIXImporter::clear()
{
    Importer::clear();

    current_named_object = nullptr;
    named_object_name = "";
    named_object_comment = "";

    current_object_group = nullptr;
    object_group_name = "";
    object_group_comment = "";
    object_group_service_protocol = "";

    rule_type = NATRule::Unknown;
    prenat_interface = "";
    postnat_interface = "";

    real_a = "";
    real_nm = "";
    mapped_a = "";
    mapped_nm = "";
    real_addr_acl = "";
    mapped_port_spec = "";
    real_port_spec = "";
    static_max_conn = "";
    static_max_emb_conn = "";

    nat_num = "";
    nat_a = "";
    nat_nm = "";
    nat_acl = "";

    tmp_global_pool = GlobalPool();
}

/*
 * this clears temporary variables inside Importer but does not touch
 * current_named_object and current_object_group
 */
void PIXImporter::clearTempVars()
{
    Importer::clear();
}

Interface* PIXImporter::getInterfaceByLabel(const string &label)
{
    map<const string,Interface*>::iterator it;
    for (it=all_interfaces.begin(); it!=all_interfaces.end(); ++it)
    {
        Interface *intf = it->second;
        if (intf->getLabel() == label)
        {
            return intf;
        }
    }
    return nullptr;
}
    

FWObject* PIXImporter::makeSrcObj()
{
    if (src_nm == "interface")
    {
        Interface *intf = getInterfaceByLabel(src_a);
        if (intf) return intf;
//        throw ImporterException(
        reportError(
            QString("Cannot find interface with label '%1'").arg(src_a.c_str()));
    }

    if (named_objects_registry.count(src_a.c_str()) > 0)
    {
        return named_objects_registry[src_a.c_str()];
    }
    return Importer::makeSrcObj();
}

FWObject* PIXImporter::makeDstObj()
{
    if (dst_nm == "interface")
    {
        Interface *intf = getInterfaceByLabel(dst_a);
        if (intf) return intf;
//        throw ImporterException(
        reportError(
            QString("Cannot find interface with label '%1'").arg(dst_a.c_str()));
    }

    if (named_objects_registry.count(dst_a.c_str()) > 0)
    {
        return named_objects_registry[dst_a.c_str()];
    }
    return Importer::makeDstObj();
}

FWObject* PIXImporter::makeSrvObj()
{
    if (protocol=="tcp" || protocol=="udp")
    {
        if (!src_port_spec.empty() &&
            named_objects_registry.count(src_port_spec.c_str()) > 0)
            return named_objects_registry[src_port_spec.c_str()];

        if (!dst_port_spec.empty() &&
            named_objects_registry.count(dst_port_spec.c_str()) > 0)
            return named_objects_registry[dst_port_spec.c_str()];
    }

    if (protocol == "icmp")
    {
        if (!icmp_spec.empty() &&
            named_objects_registry.count(icmp_spec.c_str()) > 0)
            return named_objects_registry[icmp_spec.c_str()];
    }

    if (named_objects_registry.count(protocol.c_str()) > 0)
        return named_objects_registry[protocol.c_str()];

    return Importer::makeSrvObj();
}

/*
 * See #2291
 *
 * Service group can be defined to match dstination ports but used to
 * match source ports in the access-list command. In ASA 8.0 and 8.3 the
 * following configuration is valid:
 *
 * object-group service test-service-1 tcp
 *   port-object eq www
 *
 * Group test-service-1 can be used in the position in an access-list
 * command where it would match source ports.
 *
 */
void PIXImporter::fixServiceObjectUsedForSrcPorts()
{
    if ((protocol=="tcp" || protocol=="udp") && ! src_port_spec.empty() &&
        named_objects_registry.count(src_port_spec.c_str()) > 0)
    {
        FWObject *obj = named_objects_registry[src_port_spec.c_str()];
        FWObject *new_obj = mirrorServiceObjectRecursively(obj);
        src_port_spec = new_obj->getName();
    }
}

/*
 * see #2265 and 2290. If access-list command uses object groups
 * and/or in-line port matches for both source and destination
 * ports, we need to create several new TCPService or UDPService
 * objects to match all combinations of ports. However this is only
 * necessary when at least one of them (source or destination port match)
 * uses object-group or named object because configuration with two in-line
 * port matches is taken care in IOSImporter::createTCPService()
 * and IOSImporter::createUDPService()
 */
void PIXImporter::fixServiceObjectUsedForBothSrcAndDstPorts()
{
    if (protocol=="tcp" || protocol=="udp")
    {
        // empty port_spec means no corresponding port match (either inline or
        // named object/object group)
        if (src_port_spec.empty() || dst_port_spec.empty()) return;

        FWObject *src_port_obj = nullptr;
        FWObject *dst_port_obj = nullptr;
        
        if (!src_port_spec.empty() &&
            named_objects_registry.count(src_port_spec.c_str()) > 0)
            src_port_obj = named_objects_registry[src_port_spec.c_str()];

        if (!dst_port_spec.empty() &&
            named_objects_registry.count(dst_port_spec.c_str()) > 0)
            dst_port_obj =  named_objects_registry[dst_port_spec.c_str()];

        // if both src_port_obj and dst_port_obj are nullptr, this means
        // both port operations are in-line port matches that will be
        // taken are of in the base class functions
        if (src_port_obj == nullptr && dst_port_obj == nullptr) return;

        // If only one of the two is nullptr, use base class functions to
        // fill it in from its port_op and port_spec variables
        if (dst_port_obj == nullptr)
        {
            src_port_spec = "";
            src_port_op = "";
            
            if (protocol=="tcp") dst_port_obj = createTCPService();
            else dst_port_obj = createUDPService();
        }

        if (src_port_obj == nullptr)
        {
            dst_port_spec = "";
            dst_port_op = "";
            
            if (protocol=="tcp") src_port_obj = createTCPService();
            else src_port_obj = createUDPService();
        }

        // now we have service objects or groups of service objects for
        // both source and destination port match

        string group_name =
            QString("%1 port match line %2").arg(protocol.c_str())
            .arg(getCurrentLineNumber()).toStdString();

        newObjectGroupService(group_name);

        mixServiceObjects(src_port_obj, dst_port_obj, current_object_group);

        src_port_spec = "";
        dst_port_spec = group_name;
    }
}

void PIXImporter::mixServiceObjects(FWObject *src_ports,
                                    FWObject *dst_ports,
                                    FWObject *service_group)
{
    if (Group::cast(src_ports)!=nullptr)
    {
        for (FWObject::iterator i1=src_ports->begin(); i1!=src_ports->end(); ++i1)
        {
            FWObject *o1 = FWReference::getObject(*i1);
            mixServiceObjects(o1, dst_ports, service_group);
        }
        return;
    }

    if (Group::cast(dst_ports)!=nullptr)
    {
        for (FWObject::iterator i1=dst_ports->begin(); i1!=dst_ports->end(); ++i1)
        {
            FWObject *o1 = FWReference::getObject(*i1);
            mixServiceObjects(src_ports, o1, service_group);
        }
        return;
    }

    if (src_ports->getTypeName() != dst_ports->getTypeName())
    {
        // this should not happen since ASA would not have allowed
        // access list rule with different source and destination
        // protocols but it does not hurt to check
        reportError(
            QString("Source and destination protocols do not match: '%1', '%2'")
            .arg(src_ports->getTypeName().c_str())
            .arg(dst_ports->getTypeName().c_str()));
        return;
    }

    ObjectSignature sig(error_tracker);
    sig.type_name = src_ports->getTypeName().c_str();
    sig.port_range_inclusive = false;

    sig.src_port_range_start = TCPUDPService::cast(src_ports)->getSrcRangeStart();
    sig.src_port_range_end = TCPUDPService::cast(src_ports)->getSrcRangeEnd();

    sig.dst_port_range_start = TCPUDPService::cast(dst_ports)->getDstRangeStart();
    sig.dst_port_range_end = TCPUDPService::cast(dst_ports)->getDstRangeEnd();

    service_group->addRef(commitObject(service_maker->createObject(sig)));
}

FWObject* PIXImporter::mirrorServiceObjectRecursively(FWObject *obj)
{
    FWObject *res = nullptr;
    string new_name = obj->getName() + "-mirror";

    if (Service::cast(obj) != nullptr)
    {
        FWObject *new_obj = service_maker->getMirroredServiceObject(obj);
        if (new_obj)
            named_objects_registry[QString::fromUtf8(new_name.c_str())] = new_obj;
        res = new_obj;
    } else
    {
        // newObjectGroupService creates new group object,
        // registers it as a named object and assigns pointer to
        // it to current_object_group
        newObjectGroupService(new_name);

        // if this group includes another group, we'll end up calling
        // mirrorServiceObjectRecursively() again and at this very
        // point will overwrite current_object_group with a pointer to
        // that group's mirror
        FWObject *new_group = current_object_group;

        for (FWObject::iterator it=obj->begin(); it!=obj->end(); ++it)
        {
            FWObject *new_obj = mirrorServiceObjectRecursively(
                FWReference::getObject(*it));
            if (new_obj)
                new_group->addRef(commitObject(new_obj));
        }
        res = new_group;
        current_object_group = new_group;
    }

    return res;
}

void PIXImporter::setInterfaceAndDirectionForRuleSet(
    const string &ruleset_name, const string &interface_label, const string &dir)
{
    map<const string,Interface*>::iterator it;
    for (it=all_interfaces.begin(); it!=all_interfaces.end(); ++it)
    {
        Interface *intf = it->second;
        if (intf->getLabel() == interface_label)
        {
            Importer::setInterfaceAndDirectionForRuleSet(intf, ruleset_name, dir);
            return;
        }
    }
    QString err("Can not associate rule set %1 with any interface\n");
    addMessageToLog(err.arg(QString::fromUtf8(ruleset_name.c_str())));
}

void PIXImporter::addLogging()
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


void PIXImporter::pushRule()
{
    if (rule_type == NATRule::Unknown)
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

void PIXImporter::pushPolicyRule()
{
    assert(current_ruleset!=nullptr);
    assert(current_rule!=nullptr);
    // populate all elements of the rule

    addMessageToLog(
        QString("filtering rule: access list %1, action %2")
        .arg(QString::fromUtf8(current_ruleset->ruleset->getName().c_str()))
        .arg(action.c_str()));
    
    PolicyRule *rule = PolicyRule::cast(current_rule);

    FWOptions  *ropt = current_rule->getOptionsObject();
    assert(ropt!=nullptr);

    if (action=="permit")
    {
        rule->setAction(PolicyRule::Accept);
        ropt->setBool("stateless", false);
    }

    if (action=="deny")
    {
        rule->setAction(PolicyRule::Deny);
        ropt->setBool("stateless", true);
    }

    rule->setDirection(PolicyRule::Both);

    // named service object or a group can be defined to match dstination
    // ports but used to match source ports in the access-list command.
    fixServiceObjectUsedForSrcPorts();

    fixServiceObjectUsedForBothSrcAndDstPorts();

    // special exception for rules with "neq" port operator in both
    // source and destination. #2297. We have decided to just issue a
    // warning at this time and let user fix the rule manually. We
    // should handle this case properly some day.
    if (src_port_op == "neq" && dst_port_op == "neq")
    {
        error_tracker->registerError(
            QObject::tr("Rule matches tcp or udp ports using \"neq\" port operator in "
                        "both source and destination. This configuration is "
                        "not supported by import at this time, please fix manually"));
    }

    addSrc();
    addDst();
    addSrv();

    addLogging();

    // then add it to the current ruleset
    current_ruleset->ruleset->add(current_rule);
    addStandardImportComment(
        current_rule, QString::fromUtf8(rule_comment.c_str()));
}

 
/*
 * Rearrange vlan interfaces. Importer creates all interfaces as
 * children of the firewall. Vlan interfaces should become
 * subinterfaces of the corresponding physical interfaces.
 */
void PIXImporter::rearrangeVlanInterfaces()
{
    std::unique_ptr<interfaceProperties> int_prop(
        interfacePropertiesObjectFactory::getInterfacePropertiesObject(
            getFirewallObject()));

    list<FWObject*> all_interface_objects =
        getFirewallObject()->getByTypeDeep(Interface::TYPENAME);
    list<FWObject*> vlans;
    list<FWObject*>::iterator it;
    for (it=all_interface_objects.begin(); it!=all_interface_objects.end(); ++it)
    {
        Interface *intf = Interface::cast(*it);
        FWOptions *ifopt = intf->getOptionsObject();
        
        if (int_prop->looksLikeVlanInterface(intf->getName().c_str()) &&
            ifopt->getStr("type")=="8021q")
        {
            qDebug() << "Found vlan interface" << intf->getName().c_str();
            vlans.push_back(intf);
        }
    }

    for (it=vlans.begin(); it!=vlans.end(); ++it)
    {
        Interface *vlan_intf = Interface::cast(*it);

        qDebug() << "VLAN " << vlan_intf->getName().c_str();

        QString base_name;
        int vlan_id;
        int_prop->parseVlan(vlan_intf->getName().c_str(), &base_name, &vlan_id);

        qDebug() << "base name" << base_name;

        if ( ! base_name.isEmpty())
        {
            getFirewallObject()->remove(vlan_intf, false); // do not delete

            list<FWObject*>::iterator it2;
            for (it2=all_interface_objects.begin(); it2!=all_interface_objects.end(); ++it2)
            {
                if (base_name == (*it2)->getName().c_str())
                {
                    (*it2)->add(vlan_intf, false);
                    break;
                }
            }
        }
    }

}

bool compare_ruleset_names(string a, string b)
{
    if (a.find("ssh_commands") == 0) return true;
    if (a.find("telnet_commands") == 0) return true;
    if (a.find("icmp_commands") == 0) return true;
    if (a.find("http_commands") == 0) return true;
    return a < b;
}

Firewall* PIXImporter::finalize()
{
    // scan all UnidirectionalRuleSet objects, set interface and
    // direction in all rules of corresponding RuleSet and merge all
    // UnidirectionalRuleSet into one RuleSet object. Attach this
    // object to the firewall.

    if (fwbdebug) qDebug("PIXImporter::finalize()");

    if (haveFirewallObject())
    {
        Firewall *fw = Firewall::cast(getFirewallObject());

        if (! discovered_platform.empty())
        {
            QString pl = QString(discovered_platform.c_str()).toLower();

            if (pl == "asa") pl = "pix";

            fw->setStr("platform", pl.toStdString());

            string host_os;

            if (pl == "pix") host_os = "pix_os";
            if (pl == "fwsm") host_os = "fwsm_os";

            if (! host_os.empty())
            {
                fw->setStr("host_OS", host_os);
                Resources::setDefaultTargetOptions(host_os , fw);
            }

            string version = findBestVersionMatch(
                pl, discovered_version.c_str()).toStdString();

            if ( ! version.empty()) fw->setStr("version", version);
        }

        rearrangeVlanInterfaces();

        FWObject *policy = getFirewallObject()->getFirstByType(Policy::TYPENAME);
        assert( policy!=nullptr );

        FWObject *nat = getFirewallObject()->getFirstByType(NAT::TYPENAME);
        assert( nat!=nullptr );

        if (all_rulesets.size()!=0)
        {
            if (fwbdebug)
            {
                qDebug() << "Setting interface and direction for all rules";
                qDebug() << "all_rulesets.size()=" << all_rulesets.size();
            }

            list<string> ruleset_names;
            std::map<const std::string,UnidirectionalRuleSet*>::iterator i;
            for (i=all_rulesets.begin(); i!=all_rulesets.end(); ++i)
            {
                ruleset_names.push_back(i->first);
            }

            // sort rule sets by name, making sure "ssh_commands_*",
            // "telnet_commands_*" and "icmp_commands_*" stay on top
            ruleset_names.sort(compare_ruleset_names);

            list<string>::iterator it;
            for (it=ruleset_names.begin(); it!=ruleset_names.end(); ++it)
            {
                string ruleset_name = *it;
                if (ruleset_name == "nat") continue;

                UnidirectionalRuleSet *irs = all_rulesets[ruleset_name];
                if (irs->to_be_deleted)
                {
                    irs->ruleset->clearChildren(false);
                    getFirewallObject()->remove(irs->ruleset, false);
                    delete irs->ruleset;
                    continue;
                }

                if (fwbdebug)
                {
                    qDebug() << "  irs->name=" << irs->name.c_str();
                    qDebug() << "  irs->intf_dir.size()=" << irs->intf_dir.size();
                    qDebug() << "  irs->ruleset->getName()="
                             << irs->ruleset->getName().c_str();
                    qDebug() << "  irs->ruleset->size()=" << irs->ruleset->size();
                    FWObject *p = irs->ruleset->getParent();
                    qDebug() << "  irs->ruleset->getParent()="  << p;
                    if (p)
                        qDebug() << "                            "  << p->getName().c_str();
                    qDebug() << "  fw=" << fw;
                    qDebug() << "  policy=" << policy;
                }

                if (irs->intf_dir.size() == 0)
                {
                    // no interface and direction information for this rule set
                    // Perhaps no access-group command ?
                    FWObjectDatabase *dbroot = fw->getRoot();
                    FWObject *new_ruleset = dbroot->create(
                        irs->ruleset->getTypeName());
                    fw->add(new_ruleset);
                    new_ruleset->duplicate(irs->ruleset);
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
                    for (i = irs->intf_dir.begin(); i != irs->intf_dir.end(); ++i)
                    {
                        if ( (*i).second=="in") all_in.push_back( (*i).first );
                        if ( (*i).second=="out") all_out.push_back( (*i).first );
                        if ( (*i).second=="both") all_both.push_back( (*i).first );
                    }

                    FWObject *og;
                    if (all_in.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_in);

                        MergeRules mr(irs->name, og, PolicyRule::Inbound, policy);
                        while (irs->ruleset->size() > 0)
                            mr.move(irs->ruleset->front());
                    }

                    if (all_out.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_out);

                        MergeRules mr(irs->name, og, PolicyRule::Outbound, policy);
                        while (irs->ruleset->size() > 0)
                            mr.move(irs->ruleset->front());
                    }

                    if (all_both.size()>0)
                    {
                        og = createGroupOfInterfaces(irs->name, all_both);

                        MergeRules mr(irs->name, og, PolicyRule::Both, policy);
                        while (irs->ruleset->size() > 0)
                            mr.move(irs->ruleset->front());
                    }

                }
                else
                {
                    std::map<std::string,std::string>::iterator j;
                    for (j=irs->intf_dir.begin(); j!=irs->intf_dir.end(); ++j)
                    {
                        string intf_name = (*j).first;

                        Interface *intf = all_interfaces[intf_name];
                        std::string _dir = (*j).second;
                        PolicyRule::Direction direction = PolicyRule::Both;
                        if (_dir=="in")  direction = PolicyRule::Inbound;
                        if (_dir=="out") direction = PolicyRule::Outbound;

                        if (fwbdebug)
                            qDebug() << "Interface: " << intf
                                     << "dir: " << _dir.c_str();

                        // not all access lists are associated with interfaces
                        if (intf != nullptr)
                        {
                            if (fwbdebug)
                                qDebug() << "    interface: " 
                                         << intf->getName().c_str();

                            MergeRules mr(irs->name, intf, direction, policy);
                            while (irs->ruleset->size() > 0)
                                mr.move(irs->ruleset->front());
                        }
                    }
                }

                if (fwbdebug) qDebug("ruleset done");

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

        // Deal with NAT ruleset
        UnidirectionalRuleSet *nat_rs = all_rulesets["nat"];
        if (nat_rs)
        {
            while (nat_rs->ruleset->size() > 0)
            {
                FWObject *r = nat_rs->ruleset->front();
                nat->reparent(r);
            }

            NAT::cast(nat)->renumberRules();

            nat_rs->ruleset->clearChildren(false);
            getFirewallObject()->remove(nat_rs->ruleset, false);
            delete nat_rs->ruleset;
        }

        return fw;
    }
    else
    {
        return nullptr;
    }
}

/*
 * Named objects
 *
 * At least in the case of Cisco configurations, I can only create an
 * object after I saw the line "host ... ", "subnet ..." or "range
 * ..." so I know its type. This means things like the name and
 * comment are known before the type. I use methods
 * commitNamed*Object() to create objects once all information is available.
 *
 * I other platforms information about named objects may not be
 * arranged in this way, for example in PF configs named objects are
 * represented by macros which do not have explicit type and have all
 * information on one line. Still, in that case the same commit*()
 * method will work if called by the grammar after all variables have
 * been parsed and values assigned to temporary member variables
 * inside the Importer object.
 */

void PIXImporter::newNamedObjectAddress(const string &name)
{
    named_object_name = QString::fromUtf8(name.c_str());
    named_object_comment = "";
    addMessageToLog("Named object (address) " + name);
}

void PIXImporter::newNamedObjectService(const string &name)
{
    named_object_name = QString::fromUtf8(name.c_str());
    named_object_comment = "";
    addMessageToLog("Named object (service) " + name);
}

void PIXImporter::commitNamedAddressObject()
{
    ObjectSignature sig(error_tracker);
    sig.object_name = named_object_name;
    sig.type_name = Address::TYPENAME;
    sig.address = tmp_a.c_str();
    sig.netmask = tmp_nm.c_str();
    current_named_object = commitObject(address_maker->createObject(sig));
    named_objects_registry[named_object_name] = current_named_object;
}

void PIXImporter::commitNamedAddressRangeObject()
{
    ObjectSignature sig(error_tracker);
    sig.object_name = named_object_name;
    sig.type_name = AddressRange::TYPENAME;
    sig.setAddressRangeStart(tmp_range_1.c_str());
    sig.setAddressRangeEnd(tmp_range_2.c_str());
    current_named_object = commitObject(address_maker->createObject(sig));
    named_objects_registry[named_object_name] = current_named_object;
}

void PIXImporter::commitNamedIPServiceObject()
{
    ObjectSignature sig(error_tracker);
    sig.object_name = named_object_name;
    sig.type_name = IPService::TYPENAME;
    sig.setProtocol(protocol.c_str());
    sig.fragments = fragments;
    current_named_object = commitObject(service_maker->createObject(sig));
    named_objects_registry[named_object_name] = current_named_object;
}

void PIXImporter::commitNamedICMPServiceObject()
{
    ObjectSignature sig(error_tracker);
    sig.object_name = named_object_name;
    sig.type_name = ICMPService::TYPENAME;

    if ( ! icmp_spec.empty())
    {
        sig.setIcmpFromName(icmp_spec.c_str());
    } else
    {
        sig.setIcmpType(icmp_type.c_str());
        sig.setIcmpCode(icmp_code.c_str());
    }

    current_named_object = commitObject(service_maker->createObject(sig));
    named_objects_registry[named_object_name] = current_named_object;
}

void PIXImporter::commitNamedTCPUDPServiceObject()
{
    FWObject *obj = nullptr;
    // have to use createTCPService because it processes "neq" port
    // operators and may create a group with two services nstead of
    // just tcp service. Same for udp.
    if (protocol == "tcp") obj = createTCPService(named_object_name);
    if (protocol == "udp") obj = createUDPService(named_object_name);

    current_named_object = commitObject(obj);

    named_objects_registry[named_object_name] = current_named_object;
}

FWObject* PIXImporter::commitObject(FWObject *obj)
{
    return Importer::commitObject(obj);
}

FWObject* PIXImporter::setNameOfNamedObject(FWObject *obj)
{
    if (obj->isReadOnly()) return obj;

    if ( ! named_object_name.isEmpty())
        obj->setName(named_object_name.toUtf8().constData());

    if ( ! object_group_name.isEmpty())
        obj->setName(object_group_name.toUtf8().constData());
    
    return obj;
}

/*
 * it looks like "description" line is always the last in the named
 * object block output of "show run" command on ASA, however
 * "description" is optional and we create the object when we see
 * "subnet", "host" or "service" line. This function adds description
 * to existing named object if it exists or just sets the variable
 * named_object_comment. I dont want to lose the ability to parse the
 * description if it happens to appear first in the named object
 * block.
 */
void PIXImporter::setNamedObjectDescription(const std::string &txt)
{
    named_object_comment = QString::fromUtf8(txt.c_str());

    if (current_named_object != nullptr && ! named_object_name.isEmpty())
    {
        current_named_object->setBool(".import-commited", false);
        current_named_object->setComment("");
        addStandardImportComment(current_named_object, named_object_comment);
    }
}

/************************************************************************/

void PIXImporter::newObjectGroupNetwork(const string &name)
{
    object_group_name = QString::fromUtf8(name.c_str());
    object_group_comment = "";

    ObjectMaker maker(Library::cast(library), error_tracker);
    current_object_group = 
        setNameOfNamedObject(
            commitObject(
                maker.createObject(ObjectGroup::TYPENAME, name)));
    named_objects_registry[object_group_name] = current_object_group;

    addMessageToLog("Object Group (network) " + name);
}

void PIXImporter::newObjectGroupService(const string &name)
{
    object_group_name = QString::fromUtf8(name.c_str());
    object_group_comment = "";

    ObjectMaker maker(Library::cast(library), error_tracker);
    current_object_group = 
        setNameOfNamedObject(
            commitObject(
                maker.createObject(ServiceGroup::TYPENAME, name)));
    named_objects_registry[object_group_name] = current_object_group;

    object_group_service_protocol = "";

    addMessageToLog("Object Group (service) " + name);
}

bool PIXImporter::isKnownServiceGroupName(const string &object_group_name)
{
    QString qs_object_group_name = QString::fromUtf8(object_group_name.c_str());
    if (fwbdebug) qDebug() << "isKnownServiceGroupName:" << qs_object_group_name;

    if (named_objects_registry.count(qs_object_group_name) > 0)
    {
        FWObject *grp = named_objects_registry[qs_object_group_name];
        return ServiceGroup::isA(grp);
    }
    return false;
}

void PIXImporter::newObjectGroupProtocol(const string &name)
{
    object_group_name = QString::fromUtf8(name.c_str());
    object_group_comment = "";

    ObjectMaker maker(Library::cast(library), error_tracker);
    current_object_group = 
        setNameOfNamedObject(
            commitObject(
                maker.createObject(ServiceGroup::TYPENAME, name)));
    named_objects_registry[object_group_name] = current_object_group;

    addMessageToLog("Object Group (protocol) " + name);
}

void PIXImporter::newObjectGroupICMP(const string &name)
{
    object_group_name = QString::fromUtf8(name.c_str());
    object_group_comment = "";

    ObjectMaker maker(Library::cast(library), error_tracker);
    current_object_group = 
        setNameOfNamedObject(
            commitObject(
                maker.createObject(ServiceGroup::TYPENAME, name)));
    named_objects_registry[object_group_name] = current_object_group;

    addMessageToLog("Object Group (icmp) " + name);
}

void PIXImporter::setObjectGroupDescription(const std::string &descr)
{
    object_group_comment = QString::fromUtf8(descr.c_str());
    if (current_object_group != nullptr && ! object_group_name.isEmpty())
    {
        current_object_group->setBool(".import-commited", false);
        current_object_group->setComment("");
        addStandardImportComment(current_object_group, object_group_comment);
    }
}

void PIXImporter::addNetworkToObjectGroup()
{
    ObjectSignature sig(error_tracker);
    sig.type_name = Address::TYPENAME;
    sig.setAddress(tmp_a.c_str());
    sig.setNetmask(tmp_nm.c_str());
    current_object_group->addRef(
        commitObject(address_maker->createObject(sig)));
}

void PIXImporter::addNamedObjectToObjectGroup(const std::string &object_name)
{
    QString no_name = QString::fromUtf8(object_name.c_str());
    if (named_objects_registry.count(no_name) > 0)
    {
        current_object_group->addRef(named_objects_registry[no_name]);
    } else
//        throw ImporterException(
        reportError(
            QString("Attempt to add yet undefined named object '%1' "
                    "to object group '%2'").arg(no_name).arg(object_group_name));
}

void PIXImporter::addIPServiceToObjectGroup()
{
    ObjectSignature sig(error_tracker);
    sig.type_name = IPService::TYPENAME;
    sig.setProtocol(protocol.c_str());
    sig.fragments = fragments;
    FWObject *s = service_maker->createObject(sig);
    current_object_group->addRef(commitObject(s));
}

void PIXImporter::addTCPUDPServiceToObjectGroup()
{
    FWObject *new_obj = nullptr;
    if (protocol.empty() && ! object_group_service_protocol.isEmpty())
        protocol = object_group_service_protocol.toStdString();

    if (protocol == "tcp") new_obj = createTCPService();
    if (protocol == "udp") new_obj = createUDPService();
    if (protocol == "tcp-udp") new_obj = createTCPUDPServicePair();

    if (new_obj)
        current_object_group->addRef(commitObject(new_obj));
}

void PIXImporter::addICMPServiceToObjectGroup()
{
    ObjectSignature sig(error_tracker);
    sig.type_name = ICMPService::TYPENAME;

    if ( ! icmp_spec.empty())
    {
        sig.setIcmpFromName(icmp_spec.c_str());
    } else
    {
        sig.setIcmpType(icmp_type.c_str());
        sig.setIcmpCode(icmp_code.c_str());
    }

    FWObject *s = service_maker->createObject(sig);
    current_object_group->addRef(commitObject(s));
}

