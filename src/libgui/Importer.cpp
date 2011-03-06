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


#include "Importer.h"

#include <string>
#include <ios>
#include <iostream>
#include <algorithm>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/RuleElement.h"

#include "FWWindow.h"
#include "ProjectPanel.h"
#include "FWBTree.h"

#include <QString>
#include <QtDebug>


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



FWObject* Importer::createObject(const std::string &objType,
                                 const std::string &objName)
{
    assert(library!=NULL);
    FWBTree tree ;
    FWObject *slot = tree.getStandardSlotForObject(library,objType.c_str());
    return createObject(slot, objType, objName);
}

FWObject* Importer::createObject(FWObject *parent,
                                 const std::string &objType,
                                 const std::string &objName)
{
    assert(library!=NULL);
    FWObject*  o = library->getRoot()->create(objType);
    if (parent != NULL)
    {
        parent->add(o);
    }
    o->setName(objName);
    return o;
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
    fw = NULL;
    error_counter = 0;
    logger = log;
    platform = _platform;
    clear();

    current_interface = NULL;
    current_ruleset = NULL;
    current_rule = NULL;
    custom_service_code_tracker = 0;

    tcp_flag_names[libfwbuilder::TCPService::URG]="u";
    tcp_flag_names[libfwbuilder::TCPService::ACK]="a";
    tcp_flag_names[libfwbuilder::TCPService::PSH]="p";
    tcp_flag_names[libfwbuilder::TCPService::RST]="r";
    tcp_flag_names[libfwbuilder::TCPService::SYN]="s";
    tcp_flag_names[libfwbuilder::TCPService::FIN]="f";
    tcp_flag_names[98]="N";   // NONE
    tcp_flag_names[99]="A";   // ALL

}

void Importer::run()
{
    // create and run parsers in derived classes
}


Importer::~Importer()
{
    all_rulesets.clear();
    all_interfaces.clear();
}

void Importer::clear()
{
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

    logging = false;
    established = false;
    fragments = false;

    icmp_spec = "";
    icmp_code = "";
    icmp_type = "";

    time_range_name = "";

    if (!tcp_flags_mask.empty()) tcp_flags_mask.clear();
    if (!tcp_flags_comp.empty()) tcp_flags_comp.clear();
    if (!tmp_tcp_flags_list.empty()) tmp_tcp_flags_list.clear();
}


Firewall* Importer::getFirewallObject()
{
    if (fw!=NULL) return fw;

    FWObject *nobj = createObject(Firewall::TYPENAME, fwname);

    fw = Firewall::cast(nobj);

    fw->setStr("platform", platform);
    Resources::setDefaultTargetOptions(platform , fw);

//     std::map<std::string,std::string> platforms = Resources::getPlatforms();
//     std::map<std::string,std::string>::iterator i;
//     for (i=platforms.begin(); i!=platforms.end(); i++)
//         Resources::setDefaultTargetOptions( i->first , fw );

//     std::map<std::string,std::string> OSs = Resources::getOS();
//     for (i=OSs.begin(); i!=OSs.end(); i++)
//         Resources::setDefaultTargetOptions( i->first  , fw );

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
    *logger << "Host name: " + hn + "\n";
}

void Importer::setDiscoveredVersion(const std::string &v)
{
    discovered_version = v;
    *logger << "Version: " + v + "\n";
}

void Importer::newInterface(const std::string &name)
{
    if (all_interfaces.count(name)>0) return;
    FWObject *nobj = createObject(getFirewallObject(), Interface::TYPENAME, name);
    current_interface = Interface::cast(nobj);
    current_interface->setUnnumbered(true);
    all_interfaces[name] = current_interface;
    *logger << "New interface: " + name + "\n";
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
        FWObject *nobj = createObject(intf, IPv4::TYPENAME, aname);
        IPv4::cast(nobj)->setAddress( InetAddr(addr) );
        IPv4::cast(nobj)->setNetmask( InetAddr(netm) );
    }
}

void Importer::addInterfaceAddress(const std::string &a,
                                   const std::string &nm)
{
    if (current_interface!=NULL)
    {
        addAddressObjectToInterface(current_interface, a, nm);
        *logger << "Interface address: " + a + "/" + nm + "\n";
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
            *logger << "Interface address: " + a + "/" + nm + "\n";
        }
    }
}

void Importer::setInterfaceComment(const std::string &descr)
{
    // current_interface can be NULL if parser encountered command
    // that looked like interface description but in reality was 
    // description of something else. For example this happens when
    // it finds command "description" under "controller" in Cisco router
    // configuration.
    if (current_interface!=NULL)
    {
        current_interface->setComment(descr);
        *logger << "Interface comment: " + descr + "\n";
    }
}

void Importer::setInterfaceLabel(const std::string &descr)
{
    if (current_interface!=NULL)
    {
        current_interface->setLabel(descr);
        *logger << "Interface label: " + descr + "\n";
    }
}

void Importer::setInterfaceParametes(const std::string &phys_intf_or_label,
                                     const std::string &label,
                                     const std::string &sec_level)
{
    *logger << "Interface parameters: " + phys_intf_or_label + " " + label + " " + sec_level + "\n";
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
    if (current_interface!=NULL)
    {
        QString sl(seclevel.c_str());
        current_interface->setSecurityLevel(sl.toInt());
    }
}

void Importer::setInterfaceVlanId(const std::string &vlan_id)
{
    if (current_interface!=NULL)
    {
        FWOptions *ifopt = (Interface::cast(current_interface))->getOptionsObject();
        ifopt->setStr("type", "8021q");
        ifopt->setStr("vlan_id", vlan_id);
    }
}

void Importer::addRuleComment(const std::string &comm)
{
    rule_comment += comm;
    *logger << "Rule comment: " + comm + "\n";
}

UnidirectionalRuleSet* Importer::checkUnidirRuleSet(
    const std::string &ruleset_name)
{
    return all_rulesets[ruleset_name];
}

UnidirectionalRuleSet* Importer::getUnidirRuleSet(const std::string &ruleset_name)
{
    UnidirectionalRuleSet *rs = all_rulesets[ruleset_name];
    if (rs==NULL)
    {
        // got 'ip access-group' command before the access list was defined

        rs = new UnidirectionalRuleSet();
        rs->name = ruleset_name;
        FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
        if (ruleset_name == "nat")
            rs->ruleset = RuleSet::cast(dbroot->create(NAT::TYPENAME));
        else
            rs->ruleset = RuleSet::cast(dbroot->create(Policy::TYPENAME));

        rs->ruleset->setName(ruleset_name);

        all_rulesets[ruleset_name] = rs;

        // add this ruleset to the firewall temporarily
        // because ruleset must belong to the tree somewhere in
        // order for other objects to be added properly.
        getFirewallObject()->add(rs->ruleset);
    }
    return rs;
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
                                                  const std::string &_intf_name,
                                                  const std::string &_dir)
{
    UnidirectionalRuleSet *rs = getUnidirRuleSet(ruleset_name);
    std::string intf;
    if ( !_intf_name.empty()) intf = _intf_name;
    else                      intf = current_interface->getName();

    if (rs->intf_dir.count(intf)==0)
        rs->intf_dir[intf] = _dir;
    else
    {
        // already have this interface with some direction
        // compare direction, if different, switcht to "both"
        if (rs->intf_dir[intf] != "both" && rs->intf_dir[intf] != _dir)
            rs->intf_dir[intf] = "both";
    }
    ostringstream str;
    str << "Interface " << _intf_name
        << " ruleset " << ruleset_name
        << " direction '" << _dir << "' "
        << "(set to '" << rs->intf_dir[intf] << "')"
        << "\n";
    *logger << str.str();
}

void Importer::newUnidirRuleSet(const std::string &ruleset_name)
{
    current_ruleset = getUnidirRuleSet(ruleset_name);  // creates if new
    *logger << "Ruleset: " + ruleset_name + "\n";
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
        default_action_str = "Accept";
    } else current_ruleset->default_action = PolicyRule::Deny;

    *logger << "Default action: " + default_action_str + "\n";
}


void Importer::newPolicyRule()
{
    FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
    FWObject *nobj = dbroot->create(PolicyRule::TYPENAME);
    current_rule = Rule::cast(nobj);

    // check if all child objects were populated properly
    FWOptions  *ropt = current_rule->getOptionsObject();
    assert(ropt!=NULL);
    ropt->setBool("stateless",true);
}

void Importer::newNATRule()
{
    FWObjectDatabase *dbroot = getFirewallObject()->getRoot();
    FWObject *nobj = dbroot->create(NATRule::TYPENAME);
    current_rule = Rule::cast(nobj);
}

void Importer::pushRule()
{
    assert(current_ruleset!=NULL);
    assert(current_rule!=NULL);
    // populate all elements of the rule

    PolicyRule *rule = PolicyRule::cast(current_rule);

    if (action=="permit") rule->setAction(PolicyRule::Accept);
    if (action=="deny")   rule->setAction(PolicyRule::Deny);

    addSrc();
    addDst();
    addSrv();

    rule->setLogging(logging);

    // then add it to the current ruleset
    current_ruleset->ruleset->add(current_rule);
    current_rule->setComment(rule_comment);

//     *logger << "Rule: " << action << " "
//             << protocol << " "
//             << src_a << "/" << src_nm << " ";
//     if (dst_a!="")
//         *logger << dst_a << "/" << dst_nm << " ";
//     *logger << "\n";

    current_rule = NULL;
    rule_comment = "";

    clear();
}

FWObject* Importer::makeSrcObj()
{
    if ( (src_a=="" && src_nm=="") || 
         (src_a==InetAddr::getAny().toString() &&
          src_nm==InetAddr::getAny().toString()))
        return NULL;  // this is 'any'
    if (src_nm=="") src_nm=InetAddr::getAllOnes().toString();
    return createAddress(src_a, src_nm);
}

FWObject* Importer::makeDstObj()
{
    if ( (dst_a=="" && dst_nm=="") || 
         (dst_a==InetAddr::getAny().toString() &&
          dst_nm==InetAddr::getAny().toString()))
        return NULL;  // this is 'any'
    if (dst_nm=="") dst_nm=InetAddr::getAllOnes().toString();
    return createAddress(dst_a, dst_nm);
}

FWObject* Importer::makeSrvObj()
{
    if (protocol=="") return NULL; // this is 'any'
    FWObject *s;
    if (protocol=="icmp")  s = createICMPService();
    else
        if (protocol=="tcp")   s = createTCPService();
        else
            if (protocol=="udp")   s = createUDPService();
            else
                s = createIPService();
    // if create*Service returns NULL, this is 'any'
    return s;
}

void Importer::addSrc()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    RuleElementSrc* src = rule->getSrc();
    assert(src!=NULL);
    FWObject *s = makeSrcObj();
    if (s) src->addRef( s );
}

void Importer::addDst()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    RuleElementDst* dst = rule->getDst();
    assert(dst!=NULL);
    FWObject *s = makeDstObj();
    if (s) dst->addRef( s );
}

void Importer::addSrv()
{
    PolicyRule *rule = PolicyRule::cast(current_rule);
    RuleElementSrv* srv = rule->getSrv();
    assert(srv!=NULL);
    FWObject *s = makeSrvObj();
    if (s) srv->addRef( s );
}

void Importer::addOSrc()
{
    NATRule *rule = NATRule::cast(current_rule);
    RuleElementOSrc* src = rule->getOSrc();
    assert(src!=NULL);
    FWObject *s = makeSrcObj();
    if (s) src->addRef( s );
}

void Importer::addODst()
{
    NATRule *rule = NATRule::cast(current_rule);
    RuleElementODst* dst = rule->getODst();
    assert(dst!=NULL);
    FWObject *s = makeDstObj();
    if (s) dst->addRef( s );
}

void Importer::addOSrv()
{
    NATRule *rule = NATRule::cast(current_rule);
    RuleElementOSrv* srv = rule->getOSrv();
    assert(srv!=NULL);
    FWObject *s= makeSrvObj();
    if (s) srv->addRef( s );
}

Firewall* Importer::finalize()
{
    return fw;
}

FWObject* Importer::getCustomService(const std::string &platform,
                                     const std::string &code,
                                     const std::string &protocol)
{
    // this assumes protocol is represented by a number
    if (custom_service_codes.count(platform + code)==0)
    {
        custom_service_codes[platform + code] = custom_service_code_tracker;
        custom_service_code_tracker++;
    }

    std::ostringstream nstr, cstr, sstr;
    nstr << "cust-" << custom_service_codes[platform + code];
    if (!protocol.empty()) nstr << "-" << protocol;
    sstr << "cust-" << custom_service_codes[platform + code] << "-" << protocol;
    cstr << "Imported from " << getFirewallObject()->getName() << "\n"
         << "code: " << code;
    if (!protocol.empty()) cstr << "\n" << "protocol: " << protocol;

    if (all_objects.count(sstr.str())!=0) return all_objects[sstr.str()];

    CustomService *s = CustomService::cast(
        createObject(CustomService::TYPENAME, nstr.str()));
    if (!protocol.empty()) s->setProtocol(protocol);
    s->setCodeForPlatform(platform, code);
    s->setComment(cstr.str());
    all_objects[sstr.str()] = s;
    ostringstream str;
    str << "Custom Service object: " << nstr.str()
        << ": "
        << platform
        << ": "
        << code
        << "\n";
    *logger << str.str();
    return s;
}

FWObject* Importer::getIPService(int proto)
{
    // this assumes protocol is represented by a number
    std::ostringstream nstr, cstr, sstr;
    nstr << "ip-" << proto;
    sstr << "ip-" << proto;
    if (fragments)
    {
        nstr << " fragm";
        sstr << "-fragm";
    }
    cstr << "Imported from " << getFirewallObject()->getName() << "\n"
         << "protocol " << proto;

    if (all_objects.count(sstr.str())!=0) return all_objects[sstr.str()];

    if ( proto==0 && !fragments)
        return NULL;  // any

    IPService *s = IPService::cast(createObject(IPService::TYPENAME, nstr.str()));
    s->setInt("protocol_num", proto);
    s->setBool("fragm", fragments);
    s->setComment(cstr.str());
    all_objects[sstr.str()] = s;

    *logger << "IP Service object: " + nstr.str() + "\n";
    return s;
}

FWObject* Importer::getICMPService(int type, int code)
{
    std::ostringstream nstr;
    nstr << "icmp " << type << "/" << code;

    std::ostringstream cstr;
    cstr << "Imported from " << getFirewallObject()->getName()
         << " type " << type << " code " << code;

    std::ostringstream sstr;
    sstr << "icmp-" << type << "/" << code;

    if (all_objects.count(sstr.str())!=0) return all_objects[sstr.str()];

    ICMPService *s = ICMPService::cast( createObject(ICMPService::TYPENAME,nstr.str()));
    s->setInt("type", type);
    s->setInt("code", code);
    s->setComment(cstr.str());
    all_objects[sstr.str()] = s;
    *logger << "ICMP Service object: " + nstr.str() + "\n";
    return s;
}

FWObject* Importer::getTCPService(int srs, int sre,
                                  int drs, int dre,
                                  bool established,
                                  std::list<int> &flags_mask,
                                  std::list<int> &flags_comp)
{
    std::list<int>::iterator li;

    std::ostringstream sstr;
    sstr << "tcp-" << srs << "-" << sre << ":" << drs << "-" << dre;
    if (established) sstr << "-est";

    if (!flags_mask.empty() && !flags_comp.empty())
    {
        // TCP flags
        for (li = flags_mask.begin(); li != flags_mask.end(); ++li)
            sstr << "f" << *li;

        for (li = flags_comp.begin(); li != flags_comp.end(); ++li)
            sstr << "f" << *li;
    }

    if (all_objects.count(sstr.str())!=0) return all_objects[sstr.str()];

    std::ostringstream cstr;
    cstr << "Imported from " << getFirewallObject()->getName() << "\n"
         << srs << "-" << sre << ":" << drs << "-" << dre;
    if (established) cstr << " est";
    if (!flags_mask.empty() && !flags_comp.empty()) cstr << " flags";

    std::ostringstream nstr;
    nstr << "tcp " ;
    if (srs!=0 || sre!=0)  nstr << srs << "-" << sre << ":";
    if (drs!=0 || dre!=0)  nstr << drs << "-" << dre;
    if (established) nstr << " est";
    if (!flags_mask.empty() && !flags_comp.empty())
    {
        for (li = flags_mask.begin(); li != flags_mask.end(); ++li)
            nstr << tcp_flag_names[*li];
        nstr << "/";
        for (li = flags_comp.begin(); li != flags_comp.end(); ++li)
            nstr << tcp_flag_names[*li];
    }

    TCPService* s = TCPService::cast(
        createObject(TCPService::TYPENAME, nstr.str()));
    s->setSrcRangeStart(srs);
    s->setSrcRangeEnd(sre);
    s->setDstRangeStart(drs);
    s->setDstRangeEnd(dre);


    if (!flags_mask.empty() && !flags_comp.empty())
    {
        // TCP flags
        for (li = flags_mask.begin(); li != flags_mask.end(); ++li)
        {
            switch (*li)
            {
            case 99:    // ALL
                s->setAllTCPFlagMasks();
                break;
            case 98:    // NONE
                s->clearAllTCPFlagMasks();
                break;
            default:
                s->setTCPFlagMask( TCPService::TCPFlag(*li), true);
            }
        }
        for (li = flags_comp.begin(); li != flags_comp.end(); ++li)
        {
            switch (*li)
            {
            case 99:    // ALL
                 s->setTCPFlag( TCPService::URG, true);
                 s->setTCPFlag( TCPService::ACK, true);
                 s->setTCPFlag( TCPService::PSH, true);
                 s->setTCPFlag( TCPService::RST, true);
                 s->setTCPFlag( TCPService::SYN, true);
                 s->setTCPFlag( TCPService::FIN, true);
                break;
            case 98:    // NONE
                s->clearAllTCPFlags();
                break;
            default:
                 s->setTCPFlag( TCPService::TCPFlag(*li), true);
            }
        }
    }

    s->setEstablished(established);
    s->setComment(cstr.str());
    all_objects[sstr.str()] = s;
    *logger << "TCP Service object: " + nstr.str() + "\n";
    return s;
}

FWObject* Importer::getUDPService(int srs, int sre, int drs, int dre)
{
    std::ostringstream sstr;
    sstr << "udp-" << srs << "-" << sre << ":" << drs << "-" << dre;
    if (all_objects.count(sstr.str())!=0) return all_objects[sstr.str()];

    std::ostringstream cstr;
    cstr << "Imported from " << getFirewallObject()->getName() << "\n"
         << srs << "-" << sre << ":" << drs << "-" << dre;

    std::ostringstream nstr;
    nstr << "udp " ;
    if (srs!=0 || sre!=0)  nstr << srs << "-" << sre << ":";
    if (drs!=0 || dre!=0)  nstr << drs << "-" << dre;

    UDPService* s = UDPService::cast(
        createObject(UDPService::TYPENAME, nstr.str()));
    s->setSrcRangeStart(srs);
    s->setSrcRangeEnd(sre);
    s->setDstRangeStart(drs);
    s->setDstRangeEnd(dre);

    s->setComment(cstr.str());
    all_objects[sstr.str()] = s;
    *logger << "UDP Service object: " + nstr.str() + "\n";
    return s;
}

FWObject* Importer::getTagService(const std::string &tagcode)
{
    TagService *s = NULL;

    std::ostringstream nstr, cstr, sstr;
    nstr << "tag-" << tagcode;
    sstr << "tag-" << tagcode;
    cstr << "Imported from " << getFirewallObject()->getName() << "\n"
         << "mark " << tagcode;

    if (all_objects.count(sstr.str())!=0)
        return TagService::cast(all_objects[sstr.str()]);

    s = TagService::cast(createObject(TagService::TYPENAME, nstr.str()));
    assert(s!=NULL);
    s->setCode(tagcode);
    s->setComment(cstr.str());
    all_objects[sstr.str()] = s;
    *logger << "Tag Service object: " + nstr.str() + "\n";
    return s;
}

FWObject* Importer::createICMPService()
{
    int type, code;
    std::istringstream s1(icmp_type), s2(icmp_code);
    s1.exceptions(std::ios::failbit);
    s2.exceptions(std::ios::failbit);

    if (strip(icmp_type).empty()) type = -1;
    else
    {
        try
        {
            s1 >> type;
        } catch (std::exception& e)
        {
            // could not convert
            type = -1;
            markCurrentRuleBad(std::string("ICMP type '") + icmp_type + "' unknown");
        }
    }

    if (strip(icmp_code).empty()) code = -1;
    else
    {
        try
        {
            s2 >> code;
        } catch (std::exception& e)
        {
            // could not convert
            code = -1;
            markCurrentRuleBad(std::string("ICMP code '") + icmp_code + "' unknown");
        }
    }

    return getICMPService(type,code);
}

FWObject* Importer::createIPService()
{
    // this assumes protocol is represented by a number
    std::istringstream str(protocol);
    str.exceptions(std::ios::failbit);
    int proto_num;
    try
    {
        str >> proto_num;
    } catch (std::exception& e)
    {
        // could not convert protocol number
        proto_num = 0;
        markCurrentRuleBad(std::string("Protocol '") + protocol + "' unknown");
    }
    return getIPService(proto_num);
}

FWObject* Importer::createTCPService()
{
    // Default implementation
    //
    // use src_port_spec, dst_port_spec
    //
    // here we assume src_port_spec and dst_port_spec are
    // both numeric and represent a single port.

    std::string name = "tcp " + src_port_spec + " " + dst_port_spec;

    std::istringstream src_str(src_port_spec);
    std::istringstream dst_str(dst_port_spec);
    int sport, dport;

    src_str >> sport;
    dst_str >> dport;

    return getTCPService(sport,sport,
                         dport,dport,
                         established,tcp_flags_mask,tcp_flags_comp);
}

FWObject* Importer::createUDPService()
{
    // Default implementation
    //
    // use src_port_spec, dst_port_spec
    //
    // here we assume src_port_spec and dst_port_spec are
    // both numeric and represent a single port.

    std::string name = "udp " + src_port_spec + " " + dst_port_spec;

    std::istringstream src_str(src_port_spec);
    std::istringstream dst_str(dst_port_spec);
    int sport, dport;

    src_str >> sport;
    dst_str >> dport;

    return getUDPService(sport,sport,dport,dport);
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

    ObjectGroup *og = ObjectGroup::cast(createObject(ObjectGroup::TYPENAME, name));
    for (std::list<std::string>::iterator j=interfaces.begin(); j!=interfaces.end(); ++j)
    {
        Interface *intf = all_interfaces[*j];
        og->addRef(intf);
    }
    all_objects[sig] = og;
    return og;
}

FWObject* Importer::createAddress(const std::string &addr,
                                  const std::string &netmask)
{
    std::string sig = std::string("addr-") + addr + "/" + netmask;
    if (all_objects.count(sig)!=0) return all_objects[sig];

    if ( netmask == InetAddr::getAllOnes().toString() )
    {
        string name;
        try
        {
            InetAddr obj_addr(addr); // testing if string converts to an address
            name = std::string("h-") + addr;
            Address *a = Address::cast(createObject(IPv4::TYPENAME, name));
            a->setAddress(obj_addr);
            a->setNetmask(InetAddr(InetAddr::getAllOnes()));
            string comment = "Imported from " + getFirewallObject()->getName() +
                " " + addr + "/" + netmask;
            a->setComment(comment);
            all_objects[sig] = a;
            *logger << "Address object: " + name + "\n";
            return a;
        } catch(FWException &ex)
        {
            // address text line can not be converted to ipv4 address.
            // Since parsers do not understand ipv6 yet, assume this
            // is a host address and create DNSName object
            name = addr;
            DNSName *da = DNSName::cast(createObject(DNSName::TYPENAME, name));
            da->setSourceName(addr);
            da->setRunTime(true);
            string comment = "Imported from " + getFirewallObject()->getName() +
                " " + addr;
            da->setComment(comment);
            all_objects[sig] = da;
            *logger << "DNSName object: " + name + "\n";
            return da;
        }

    } else
    {
        Network *net;
        std::string name = std::string("net-") + addr + "/" + netmask;
        net = Network::cast(createObject(Network::TYPENAME, name));
        try
        {
            net->setAddress( InetAddr(addr) );
        } catch (FWException &ex)
        {
            markCurrentRuleBad(
                std::string("Error converting address '") + addr + "'");
        }

        try
        {
            net->setNetmask( InetAddr(netmask) );
        } catch (FWException &ex)
        {
            if (netmask.find('.')!=std::string::npos)
            {
                // netmask has '.' in it but conversion failed.
                markCurrentRuleBad(
                    std::string("Error converting netmask '") + netmask + "'");
            } else
            {
                // no dot in netmask, perhaps it is specified by its length?
                // if netmask is specified by length, need to use special
                // constructor for class Netmask to convert
                std::istringstream str(netmask);
                str.exceptions(std::ios::failbit);
                int nm_len;
                try
                {
                    str >> nm_len;
                    net->setNetmask( InetAddr(nm_len) );
                } catch (std::exception& e)
                {
                    // could not convert netmask as simple integer
                    markCurrentRuleBad(
                        std::string("Error converting netmask '") + netmask + "'");
                }
            }
        }

        string comment = "Imported from " + getFirewallObject()->getName() +
            " " + addr + "/" + netmask;

        net->setComment(comment);
        all_objects[sig] = net;
        *logger << "Network object: " + name + "\n";
        return net;
    }
    return NULL;
}

FWObject* Importer::createAddressRange(const std::string &addr1,
                                       const std::string &addr2)
{
    std::string comment = "Imported from " + getFirewallObject()->getName() +
        " " + addr1 + "-" + addr2;
    std::string sig = std::string("ar-") + addr1 + "-" + addr2;
    if (all_objects.count(sig)!=0) return all_objects[sig];

    AddressRange *ar;
    std::string name = std::string("range-") + addr1 + "-" + addr2;
    ar = AddressRange::cast(createObject(AddressRange::TYPENAME, name));

    try
    {
        ar->setRangeStart( InetAddr(addr1) );
    } catch (FWException &ex)
    {
        markCurrentRuleBad(
            std::string("Error converting address '") + addr1 + "'");
    }

    try
    {
        ar->setRangeEnd( InetAddr(addr2) );
    } catch (FWException &ex)
    {
        markCurrentRuleBad(
            std::string("Error converting address '") + addr2 + "'");
    }

    ar->setComment(comment);
    all_objects[sig] = ar;
    *logger << "AddressRange object: " + name + "\n";
    return ar;
}

/**
 *   set color of the current rule  (use red) and add comment
 *   to indicate that the rule could not be properly parsed
 */
void Importer::markCurrentRuleBad(const std::string &comment)
{
    FWOptions  *ropt = current_rule->getOptionsObject();
    assert(ropt!=NULL);

    ropt->setStr("color", getBadRuleColor());

    if (!rule_comment.empty()) rule_comment += "\n";
    rule_comment += comment;
    //current_rule->setComment(comment);

    QString err = QObject::tr("Parser error: Line %1: %2\n")
        .arg(getCurrentLineNumber())
        .arg(QString::fromUtf8(comment.c_str()));
    *logger << err.toUtf8().constData();
    error_counter++;
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
        n += rs->ruleset->getRuleSetSize();
    }
    return n;
}

int Importer::countInterfaces()
{
    if (haveFirewallObject())
    {
        Firewall *fw = Firewall::cast(getFirewallObject());
        list<FWObject*> all_interfaces =  fw->getByType(Interface::TYPENAME);
        return all_interfaces.size();
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
        "'iptables-restore' command and Cisco routers (IOS) "
        "configurations saved with 'show run' command. Import "
        "of cisco ASA (PIX) configuration is not supported "
        "at this time");
}

void Importer::addMessageToLog(const std::string &msg)
{
    *logger << msg + "\n";
}


