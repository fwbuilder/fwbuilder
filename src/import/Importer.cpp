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

/*
 * Trying to avoid dependency on libgui (except for FWBTree, which
 * will be refactored into some other common module in the future).
 */


#include "Importer.h"

#include <string>
#include <ios>
#include <iostream>
#include <algorithm>
#include <memory>

#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include "fwbuilder/Address.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
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
#include <QStringList>
#include <QRegExp>
#include <QtDebug>


extern int fwbdebug;

using namespace libfwbuilder;
using namespace std;

// a functor to join list<string> into a string with separator sep
class join : public std::unary_function<std::string, void>
{
    std::string *result;
    std::string  separator;
public:
    join(std::string *res, const std::string &s)
    { result = res; separator = s; }
    void operator()(std::string &s);
};

void join::operator()(string &s)
{
    if (!result->empty()) *result += separator;
    *result += s;
}



std::string Importer::getBadRuleColor()
{
    return "#C86E6E";
}

void Importer::SaveTmpAddrToSrc()
{
    src_a = tmp_a;
    src_nm = tmp_nm;
}

void Importer::SaveTmpAddrToDst()
{
    dst_a = tmp_a;
    dst_nm = tmp_nm;
}

void Importer::SaveTmpPortToSrc()
{
    src_port_op = tmp_port_op;
    src_port_spec = tmp_port_spec;
}

void Importer::SaveTmpPortToDst()
{
    dst_port_op = tmp_port_op;
    dst_port_spec = tmp_port_spec;
}

Importer::Importer(FWObject *_lib,
                   const std::string &_platform,
                   std::istringstream &_input,
                   Logger *log,
                   const std::string &fwname) : input(_input)
{
    this->fwname = fwname;
    library = _lib;
    fw = nullptr;
    error_counter = 0;
    logger = log;
    platform = _platform;

    current_interface = nullptr;
    current_ruleset = nullptr;
    current_rule = nullptr;

    error_tracker = new ObjectMakerErrorTracker();

    address_maker = new AddressObjectMaker(Library::cast(library), error_tracker);
    service_maker = new ServiceObjectMaker(Library::cast(library), error_tracker);

    clear();

}

void Importer::prepareForDeduplication()
{
    address_maker->prepareForDeduplication(library->getRoot());
    service_maker->prepareForDeduplication(library->getRoot());
}

void Importer::run()
{
    // create and run parsers in derived classes
}


Importer::~Importer()
{
    all_rulesets.clear();
    all_interfaces.clear();

    delete address_maker;
    delete service_maker;

    delete error_tracker;
}

void Importer::clear()
{
    last_comment.clear();

    action = "";

    protocol = "";

    src_a = "";
    src_nm = "";
    src_port_op = "";
    src_port_spec = "";

    dst_a = "";
    dst_nm = "";
    dst_port_op = "";
    dst_port_spec = "";

    tmp_a = "";
    tmp_nm = "";
    tmp_port_op = "";
    tmp_port_spec = "";
    tmp_port_spec_2 = "";
    tmp_range_1 = "";
    tmp_range_2 = "";

    logging = false;
    log_level = "";
    log_interval = "";

    established = false;
    fragments = false;

    icmp_spec = "";
    icmp_code = "";
    icmp_type = "";

    time_range_name = "";

    if (!tcp_flags_mask.empty()) tcp_flags_mask.clear();
    if (!tcp_flags_comp.empty()) tcp_flags_comp.clear();
    if (!tmp_tcp_flags_list.empty()) tmp_tcp_flags_list.clear();

    error_tracker->clear();
}

Firewall* Importer::getFirewallObject()
{
    if (fw!=nullptr) return fw;

    ObjectMaker maker(Library::cast(library), error_tracker);
    FWObject *nobj = commitObject(
        maker.createObject(Firewall::TYPENAME, fwname));

    fw = Firewall::cast(nobj);

    fw->setStr("platform", platform);
    Resources::setDefaultTargetOptions(platform , fw);

    return fw;
}

/*
 * Creates firewall object and sets its name
 *
 * This assumes that configuration clase that declares host name
 * comes first (true for Ciscos, but may not be true for others)
 *
 */
void Importer::setHostName(const std::string &hn)
{
    getFirewallObject()->setName(hn);
    addMessageToLog("Host name: " + hn);
}

void Importer::setDiscoveredVersion(const std::string &v)
{
    discovered_version = v;
    addMessageToLog("Version: " + v);
}

void Importer::setDiscoveredPlatform(const std::string &v)
{
    discovered_platform = v;
    addMessageToLog("Platform: " + v);
}

Interface* Importer::newInterface(const std::string &name)
{
    if (all_interfaces.count(name)>0) return all_interfaces[name];
    ObjectMaker maker(Library::cast(library), error_tracker);
    FWObject *nobj = commitObject(
        maker.createObject(getFirewallObject(), Interface::TYPENAME, name));
    current_interface = Interface::cast(nobj);
    current_interface->setUnnumbered(true);
    all_interfaces[name] = current_interface;
    addMessageToLog(QObject::tr("New interface: %1").arg(name.c_str()));
    return current_interface;
}

/*
 * We call this when importer for PIX or IOS encounters interface in
 * state "shutdown"
 */
void Importer::ignoreCurrentInterface()
{
    if (current_interface)
    {
        string name = current_interface->getName();
        current_interface->getParent()->remove(current_interface);
        all_interfaces.erase(name);
        current_interface = nullptr;
    }
}

void Importer::addAddressObjectToInterface(Interface*intf,
                                           const string &addr,
                                           const string &netm)
{
    intf->setUnnumbered(false);
    if (addr == "dhcp") intf->setDyn(true);
    else
    {
        string aname = getFirewallObject()->getName() + ":" + intf->getName() + ":ip";
        ObjectMaker maker(Library::cast(library), error_tracker);
        FWObject *nobj = commitObject(
            maker.createObject(intf, IPv4::TYPENAME, aname));
        IPv4::cast(nobj)->setAddress( InetAddr(addr) );
        IPv4::cast(nobj)->setNetmask( InetAddr(netm) );
    }
}

void Importer::addInterfaceAddress(const std::string &a,
                                   const std::string &nm)
{
    if (current_interface!=nullptr)
    {
        addAddressObjectToInterface(current_interface, a, nm);
        addMessageToLog("Interface address: " + a + "/" + nm);
    }
}

void Importer::addInterfaceAddress(const std::string &label,
                                   const std::string &a,
                                   const std::string &nm)
{
    map<const string,Interface*>::iterator it;
    for (it=all_interfaces.begin(); it!=all_interfaces.end(); ++it)
    {
        Interface *intf = it->second;
        if (intf->getLabel() == label)
        {
            addAddressObjectToInterface(intf, a, nm);
            addMessageToLog("Interface address: " + a + "/" + nm);
        }
    }
}

void Importer::setInterfaceComment(const std::string &descr)
{
    // current_interface can be nullptr if parser encountered command
    // that looked like interface description but in reality was 
    // description of something else. For example this happens when
    // it finds command "description" under "controller" in Cisco router
    // configuration.
    if (current_interface!=nullptr)
    {
        current_interface->setComment(descr);
        addMessageToLog("Interface comment: " + descr);
    }
}

void Importer::setInterfaceLabel(const std::string &descr)
{
    if (current_interface!=nullptr)
    {
        current_interface->setLabel(descr);
        addMessageToLog("Interface label: " + descr);
    }
}

void Importer::setInterfaceParametes(const std::string &phys_intf_or_label,
                                     const std::string &label,
                                     const std::string &sec_level)
{
    addMessageToLog("Interface parameters: " + phys_intf_or_label +
                    " " + label + " " + sec_level);

    if (all_interfaces.count(phys_intf_or_label))
    {
        // since first arg. is physical interface name, this must be pix6
        // "nameif ethernet0 outside security0"
        Interface *intf = all_interfaces[phys_intf_or_label];
        intf->setLabel(label);
        QRegExp pix6_sec_level("security(\\d+)");
        if (pix6_sec_level.indexIn(sec_level.c_str()) > -1)
            intf->setSecurityLevel(pix6_sec_level.cap(1).toInt());
    } else
    {
        // since first arg is not physical interface name, it must be a label
        // as in pix7 config
        //
        // interface Ethernet0.101
        //  vlan 101
        //  nameif outside
        //  security-level 0
        //  ip address 192.0.2.253 255.255.255.0
        setInterfaceLabel(phys_intf_or_label);
    }
}

void Importer::setInterfaceSecurityLevel(const std::string &seclevel)
{
    if (current_interface!=nullptr)
    {
        QString sl(seclevel.c_str());
        current_interface->setSecurityLevel(sl.toInt());
    }
}

void Importer::setInterfaceVlanId(const std::string &vlan_id)
{
    if (current_interface!=nullptr)
    {
        FWOptions *ifopt = (Interface::cast(current_interface))->getOptionsObject();
        ifopt->setStr("type", "8021q");
        ifopt->setStr("vlan_id", vlan_id);
    }
}

void Importer::addRuleComment(const std::string &comm)
{
    rule_comment += comm;
    addMessageToLog("Rule comment: " + comm);
}

UnidirectionalRuleSet* Importer::checkUnidirRuleSet(
    const std::string &ruleset_name)
{
    return all_rulesets[ruleset_name];
}

UnidirectionalRuleSet* Importer::getUnidirRuleSet(
    const std::string &ruleset_name, const string &ruleset_type_name)
{
    UnidirectionalRuleSet *rs = all_rulesets[ruleset_name];
    if (rs==nullptr)
    {
        // got 'ip access-group' command before the access list was defined
        rs = new UnidirectionalRuleSet();
        rs->name = ruleset_name;
        FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
        rs->ruleset = RuleSet::cast(dbroot->create(ruleset_type_name));
        rs->ruleset->setName(ruleset_name);
        all_rulesets[ruleset_name] = rs;
        // add this ruleset to the firewall temporarily
        // because ruleset must belong to the tree somewhere in
        // order for other objects to be added properly.
        getFirewallObject()->add(rs->ruleset);
    }
    return rs;
}

void Importer::setInterfaceAndDirectionForRuleSet(
        Interface *intf, const std::string &ruleset_name, const std::string &dir)
{
    UnidirectionalRuleSet *rs = getUnidirRuleSet(ruleset_name, Policy::TYPENAME);
    string intf_name = intf->getName();
    if (rs->intf_dir.count(intf_name)==0) rs->intf_dir[intf_name] = dir;
    else
    {
        // already have this interface with some direction
        // compare direction, if different, switcht to "both"
        if (rs->intf_dir[intf_name] != "both" && rs->intf_dir[intf_name] != dir)
            rs->intf_dir[intf_name] = "both";
    }
    QString l("Interface %1 ruleset %2 direction '%3'");
    addMessageToLog(
        l.arg(intf_name.c_str()).arg(ruleset_name.c_str()).arg(dir.c_str()));
}

/*
 * associate ruleset <ruleset_name> with interface <intf_name>
 * and direction <dir>
 *
 * if <intf_name> is empty, use current_interface
 *
 * Note that a ruleset may be associated with multiple interfaces
 * and each association can have different direction.
 */
void Importer::setInterfaceAndDirectionForRuleSet(const std::string &ruleset_name,
                                                  const std::string &intf_name,
                                                  const std::string &dir)
{
    Interface *intf = nullptr;
    if ( ! intf_name.empty())
    {
        intf = all_interfaces[intf_name];
    } else
    {
        if (current_interface) intf = current_interface;
    }

    if (intf == nullptr)
    {
        // current_interface is nullptr and _intf_name is empty. Not enough
        // information to associate ruleset with an interface.
        QString err("Can not associate rule set %1 with any interface\n");
        addMessageToLog(err.arg(QString::fromUtf8(ruleset_name.c_str())));
    } else
        setInterfaceAndDirectionForRuleSet(intf, ruleset_name, dir);
}

void Importer::newUnidirRuleSet(const string &ruleset_name,
                                const string &ruleset_type)
{
    current_ruleset = getUnidirRuleSet(ruleset_name, ruleset_type);  // creates if new
    current_ruleset->created_from_line_number = getCurrentLineNumber();
    //*logger << "Ruleset: " + ruleset_name + "\n";
}

/*
 * Grammar must ensure the call to setDefaultAction() happens
 * after the call to newUnidirRuleSet()
 * 
 */
void Importer::setDefaultAction(const std::string &iptables_action_name)
{
    string default_action_str = "Deny";
    if (iptables_action_name == "ACCEPT")
    {
        current_ruleset->default_action = PolicyRule::Accept;
        current_ruleset->default_action_line_number = getCurrentLineNumber();
        default_action_str = "Accept";
    } else current_ruleset->default_action = PolicyRule::Deny;

    addMessageToLog("Default action: " + default_action_str);
}

void Importer::newPolicyRule()
{
    if (fwbdebug) qDebug() << "Importer::newPolicyRule()";

    FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
    FWObject *nobj = dbroot->create(PolicyRule::TYPENAME);
    current_rule = Rule::cast(nobj);

    // check if all child objects were populated properly
    FWOptions  *ropt = current_rule->getOptionsObject();
    assert(ropt!=nullptr);
    ropt->setBool("stateless", true);
}

void Importer::newNATRule()
{
    if (fwbdebug) qDebug() << "Importer::newNATRule()";

    FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
    FWObject *nobj = dbroot->create(NATRule::TYPENAME);
    current_rule = Rule::cast(nobj);

    if (fwbdebug) qDebug() << "current_rule=" << current_rule;
}

void Importer::pushRule()
{
    assert(current_ruleset!=nullptr);
    assert(current_rule!=nullptr);
    // populate all elements of the rule

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

    addSrc();
    addDst();
    addSrv();

    addLogging();

    // then add it to the current ruleset
    current_ruleset->ruleset->add(current_rule);

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

    addStandardImportComment(
        current_rule, QString::fromUtf8(rule_comment.c_str()));

    current_rule = nullptr;
    rule_comment = "";

    clear();
}

void Importer::setSrcSelf()
{
    src_a = "self";
}

void Importer::setDstSelf()
{
    dst_a = "self";
}    

FWObject* Importer::makeAddressObj(const std::string addr, const std::string netm)
{
    if (addr == "self")
    {
        return getFirewallObject();
    }

    if ( (addr=="" && netm=="") || 
         (addr==InetAddr::getAny().toString() &&
          netm==InetAddr::getAny().toString()))
        return nullptr;  // this is 'any'

    ObjectSignature sig(error_tracker);
    sig.type_name = Address::TYPENAME;
    sig.setAddress(addr.c_str());
    if (netm=="") 
        sig.setNetmask(InetAddr::getAllOnes().toString().c_str(),
                       address_maker->getInvertedNetmasks());
    else
        sig.setNetmask(netm.c_str(), address_maker->getInvertedNetmasks());

    return commitObject(address_maker->createObject(sig));
}

FWObject* Importer::makeSrcObj()
{
    return makeAddressObj(src_a, src_nm);
}

FWObject* Importer::makeDstObj()
{
    return makeAddressObj(dst_a, dst_nm);
}

FWObject* Importer::makeSrvObj()
{
    if (protocol=="") return nullptr; // this is 'any'
    FWObject *s;
    if (protocol=="icmp")
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
        s = service_maker->createObject(sig);
    } else
    {
        if (protocol=="tcp")
        {
            s = createTCPService();
        } else
        {
            if (protocol=="udp")
            {
                s = createUDPService();
            } else
            {
                ObjectSignature sig(error_tracker);
                sig.type_name = IPService::TYPENAME;
                sig.setProtocol(protocol.c_str());
                sig.fragments = fragments;
                s = service_maker->createObject(sig);
            }
        }
    }
    // if create*Service returns nullptr, this is 'any'
    return commitObject(s);
}

void Importer::addSrc()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    RuleElementSrc* src = rule->getSrc();
    assert(src!=nullptr);
    FWObject *s = makeSrcObj();
    if (s) src->addRef( s );
}

void Importer::addDst()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    RuleElementDst* dst = rule->getDst();
    assert(dst!=nullptr);
    FWObject *s = makeDstObj();
    if (s) dst->addRef( s );
}

void Importer::addSrv()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    RuleElementSrv* srv = rule->getSrv();
    assert(srv!=nullptr);
    FWObject *s = makeSrvObj();
    if (s) srv->addRef( s );
}

void Importer::addOSrc()
{
    NATRule *rule = NATRule::cast(current_rule);
    RuleElementOSrc* src = rule->getOSrc();
    assert(src!=nullptr);
    FWObject *s = makeSrcObj();
    if (s) src->addRef( s );
}

void Importer::addODst()
{
    NATRule *rule = NATRule::cast(current_rule);
    RuleElementODst* dst = rule->getODst();
    assert(dst!=nullptr);
    FWObject *s = makeDstObj();
    if (s) dst->addRef( s );
}

void Importer::addOSrv()
{
    NATRule *rule = NATRule::cast(current_rule);
    RuleElementOSrv* srv = rule->getOSrv();
    assert(srv!=nullptr);
    FWObject *s= makeSrvObj();
    if (s) srv->addRef( s );
}

void Importer::addLogging()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    rule->setLogging(logging);
    // log_level
    // log_interval
}

Firewall* Importer::finalize()
{
    return fw;
}

FWObject* Importer::createTCPService(const QString &)
{
    // Default implementation
    return nullptr;
}

FWObject* Importer::createUDPService(const QString &)
{
    // Default implementation
    return nullptr;
}

FWObject* Importer::createGroupOfInterfaces(
    const std::string &ruleset_name, std::list<std::string> &interfaces)
{
    std::string name = "intf-" + ruleset_name;
    // by including ruleset name (==acl name) into the signature we
    // force import to create separate interface group for each access list
    // even if interface set is the same as for some other access list.
    // This decision is rather arbitrary but it feels less confusing
    // compared to the case when interface groups cretaed from different
    // access lists are merged. If they are merged, the name refers to one
    // access list which looks weird in the GUI since rules may have been
    // imported from another access list.
    std::string sig = ruleset_name + "_";
    std::for_each(interfaces.begin(), interfaces.end(), join(&sig, "_"));

    if (fwbdebug)
        qDebug() << QString("Interface group with name '%1', sig '%2'")
            .arg(name.c_str()).arg(sig.c_str());

    if (all_objects.count(sig)!=0) return all_objects[sig];

    ObjectMaker maker(Library::cast(library), error_tracker);
    ObjectGroup *og = ObjectGroup::cast(
        commitObject(
            maker.createObject(ObjectGroup::TYPENAME, name)));

    for (std::list<std::string>::iterator j=interfaces.begin(); j!=interfaces.end(); ++j)
    {
        Interface *intf = all_interfaces[*j];
        og->addRef(intf);
    }
    all_objects[sig] = og;
    return og;
}

/**
 *   set color of the current rule  (use red) and add comment
 *   to indicate that the rule could not be properly parsed
 */
void Importer::markCurrentRuleBad()
{
    FWOptions  *ropt = current_rule->getOptionsObject();
    assert(ropt!=nullptr);
    ropt->setStr("color", getBadRuleColor());

    QStringList comment;
    if ( ! current_rule->getComment().empty())
        comment.append(QString::fromUtf8(current_rule->getComment().c_str()));

    foreach(QString err, error_tracker->getWarnings())
        comment.append(err);

    foreach(QString err, error_tracker->getErrors())
        comment.append(err);

    current_rule->setComment(comment.join("\n").toUtf8().constData());
}

void Importer::reportError(const std::string &comment)
{
    reportError(QString::fromUtf8(comment.c_str()));
}

void Importer::reportError(const QString &comment)
{
    error_counter++;
    QString err = QObject::tr("Error: %1").arg(comment);
    addMessageToLog(err);
    error_tracker->registerError(err);
}

int Importer::countRules()
{
    int n = 0;
    std::map<const string, UnidirectionalRuleSet*>::iterator it;
    for (it=all_rulesets.begin(); it!=all_rulesets.end(); ++it)
    {
        // rs_index is a string composed of the table name and chain name
        // like "filter / FORWARD" or "mangle / PREROUTING"
        // This string is created in IPTImporter::getUnidirRuleSet()
        string rs_index = it->first;
        UnidirectionalRuleSet* rs = it->second;
        if (rs->ruleset)
            n += rs->ruleset->getRuleSetSize();
    }
    return n;
}

int Importer::countInterfaces()
{
    if (haveFirewallObject())
    {
        Firewall *fw = Firewall::cast(getFirewallObject());
        list<FWObject*> all_interface_objects = fw->getByType(Interface::TYPENAME);
        return all_interface_objects.size();
    } else
        return 0;
}

QString Importer::noFirewallErrorMessage()
{
    return QObject::tr(
        "Could not find enough information in the data file "
        "to create firewall object."
        "\n\n"
    );
}

QString Importer::noRulesErrorMessage()
{
    return QObject::tr(
        "Could not find enough information in the data file "
        "to create any firewall rules."
        "\n\n"
    );
}

QString Importer::noInterfacesErrorMessage()
{
    return QObject::tr(
        "Could not find enough information in the data file "
        "to create firewall interface objects."
        "\n\n"
    );
}

/*
 * This is a common error message shown by the importer when it fails
 * to create firewall object. Keeping it in the base class since it is
 * used in the finalize() function of all importer classes.
 */
QString Importer::commonFailureErrorMessage()
{
    return QObject::tr(
        "Please check that the "
        "file you are trying to import is in one of supported "
        "formats. Currently fwbuilder can only import "
        "iptables configuration saved with "
        "'iptables-restore' command, Cisco routers (IOS), "
        "Cisco ASA, FWSM and PIX configurations saved with "
        "'show run' command and PF configuration from a pf.conf file."
    );
}

void Importer::addMessageToLog(const std::string &msg)
{
    addMessageToLog(QString::fromUtf8(msg.c_str()));
}

void Importer::addMessageToLog(const QString &msg)
{
    if (getCurrentLineNumber() >= 0)
    {
        QString log_line("%1: %2\n");
        *logger << log_line.arg(getCurrentLineNumber()).arg(msg).toUtf8().constData();
    } else
    {
        *logger << msg.toUtf8().constData();
    }
}

/**
 * This function adds "standard" comment to the object, plus text
 * passed as @additional_comment argument. If the object already has
 * some comment, it is preserved and new text is appended to it. If
 * flag add_standard_comments is false, then comment referring to the
 * line number in the original file is not added, but
 * @additional_comment is added anyway. Note that we also add comments
 * to rules in case of errors but those are not suppressed by the flag
 * add_standard_comments
 */
void Importer::addStandardImportComment(FWObject *obj,
                                        const QString &additional_comment)
{
    if (obj == nullptr) return;

    // what if this object has been found in a read-only library?
    if (obj->isReadOnly()) return;

    // this function may get called again if object is being reused
    if ( obj->getBool(".import-commited")) return;

    QStringList comment;

    if ( ! obj->getComment().empty())
        comment << QString::fromUtf8(obj->getComment().c_str());

    if ( ! additional_comment.isEmpty()) comment << additional_comment;

    if (add_standard_comments)
    {
        QString file_and_line("Created during import of %1 line %2");
        comment << file_and_line
            .arg(QString::fromUtf8(input_file_name.c_str()))
            .arg(getCurrentLineNumber());
    }

    obj->setComment(comment.join("\n").toUtf8().constData());
    obj->setBool(".import-commited", true);
}

FWObject* Importer::commitObject(FWObject *obj)
{
    // what if this object has been found in a read-only library?
    if (obj->isReadOnly()) return obj;
    if (obj) addStandardImportComment(obj, "");
    return obj;
}

/*
 * Rearrange vlan interfaces. Importer creates all interfaces as
 * children of the firewall. Vlan interfaces should become
 * subinterfaces of the corresponding physical interfaces.
 */
void Importer::rearrangeVlanInterfaces()
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

void Importer::registerBrokenObject(FWObject *obj, const QString &err)
{
    broken_objects[obj] = err;
}

bool Importer::isObjectBroken(FWObject *obj)
{
    return broken_objects.count(obj) != 0;
}

QString Importer::getBrokenObjectError(FWObject *obj)
{
    return broken_objects[obj];
}

