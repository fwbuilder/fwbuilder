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

#include <ios>
#include <iostream>
#include <algorithm>

#include <qstring.h>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/RuleElement.h"

#include "FWWindow.h"
#include "ProjectPanel.h"
#include "FWBTree.h"

using namespace libfwbuilder;

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
                   Logger *log) : input(_input)
{
    library = _lib;
    fw = NULL;
    error_counter = 0;
    logger = log;
    platform = _platform;
    clear();

    current_interface = NULL;
    current_ruleset = NULL;
    current_rule = NULL;

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

    FWObject *nobj = createObject(Firewall::TYPENAME, "New Firewall");

    fw = Firewall::cast(nobj);

    fw->setStr("platform", platform);

    std::map<std::string,std::string> platforms = Resources::getPlatforms();
    std::map<std::string,std::string>::iterator i;
    for (i=platforms.begin(); i!=platforms.end(); i++)
        Resources::setDefaultTargetOptions( i->first , fw );

    std::map<std::string,std::string> OSs = Resources::getOS();
    for (i=OSs.begin(); i!=OSs.end(); i++)
        Resources::setDefaultTargetOptions( i->first  , fw );

    //o->setStr("platform", readPlatform(platform).latin1() );
    //o->setStr("host_OS",  readHostOS(hostOS).latin1() );

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
    *logger << "Host name: " << hn << "\n";
}

void Importer::newInterface(const std::string &name)
{
    if (all_interfaces.count(name)>0) return;
    FWObject *nobj = createObject(fw, Interface::TYPENAME, name);
    current_interface = Interface::cast(nobj);
    current_interface->setUnnumbered(true);
    all_interfaces[name] = current_interface;

    *logger << "Interface: " << name << "\n";
}

void Importer::addInterfaceAddress(const std::string &a,
                                   const std::string &nm)
{
    if (current_interface!=NULL)
    {
        std::string aname = getFirewallObject()->getName() + ":"
            + current_interface->getName() + ":ip";
        FWObject *nobj = createObject(current_interface,
                                      IPv4::TYPENAME,
                                      aname);
        current_interface->setUnnumbered(false);
        IPv4::cast(nobj)->setAddress( InetAddr(a) );
        IPv4::cast(nobj)->setNetmask( InetAddr(nm) );

        *logger << "Interface address: " << a << "/" << nm << "\n";
    }
}

void Importer::addInterfaceComment(const std::string &descr)
{
    assert(current_interface!=NULL);
    current_interface->setComment(descr);
    *logger << "Interface comment: " << descr << "\n";
}

void Importer::addRuleComment(const std::string &comm)
{
    rule_comment += comm;
    *logger << "Rule comment: " << comm << "\n";
}

UnidirectionalRuleSet* Importer::checkUnidirRuleSet(
    const std::string &ruleset_name)
{
    return all_rulesets[ruleset_name];
}

UnidirectionalRuleSet* Importer::getUnidirRuleSet(const std::string &ruleset_name)
{
    if (fwbdebug)
        qDebug("Importer::getUnidirRuleSet %s", ruleset_name.c_str());


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
    if (fwbdebug)
        qDebug("Importer::setInterfaceAndDirectionForRuleSet %s",
               ruleset_name.c_str());

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
    *logger << "Interface " << _intf_name
            << " ruleset " << ruleset_name
            << " direction '" << _dir << "' "
            << "(set to '" << rs->intf_dir[intf] << "')"
            << "\n";
}

void Importer::newUnidirRuleSet(const std::string &ruleset_name)
{
    current_ruleset = getUnidirRuleSet(ruleset_name);  // creates if new

    *logger << "Ruleset: " << ruleset_name << "\n";
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

    *logger << "IP Service object: " << nstr.str() << "\n";
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
    *logger << "ICMP Service object: " << nstr.str() << "\n";
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
    *logger << "TCP Service object: " << nstr.str() << "\n";
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
    nstr << "udp "
         << srs << "-" << sre << ":" << drs << "-" << dre;

    UDPService* s = UDPService::cast(createObject(UDPService::TYPENAME, nstr.str()));
    s->setSrcRangeStart(srs);
    s->setSrcRangeEnd(sre);
    s->setDstRangeStart(drs);
    s->setDstRangeEnd(dre);

    s->setComment(cstr.str());
    all_objects[sstr.str()] = s;
    *logger << "UDP Service object: " << nstr.str() << "\n";
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

    if (all_objects.count(sstr.str())!=0) return TagService::cast(all_objects[sstr.str()]);

    s = TagService::cast(createObject(TagService::TYPENAME, nstr.str()));
    assert(s!=NULL);
    s->setCode(tagcode);
    s->setComment(cstr.str());
    all_objects[sstr.str()] = s;
    *logger << "Tag Service object: " << nstr.str() << "\n";
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
        qDebug(QString("Interface group with name '%1', sig '%2'").arg(name.c_str()).arg(sig.c_str()).toAscii().constData());

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
    std::string comment = "Imported from " + getFirewallObject()->getName() +
        " " + addr + "/" + netmask;
    std::string sig = std::string("addr-") + addr + "/" + netmask;
    if (all_objects.count(sig)!=0) return all_objects[sig];

    if ( netmask == InetAddr::getAllOnes().toString() )
    {
        Address *a;
        std::string name = std::string("h-") + addr;
        a = Address::cast(createObject(IPv4::TYPENAME, name));
        a->setAddress(InetAddr(addr));
        a->setNetmask(InetAddr(InetAddr::getAllOnes()));
        a->setComment(comment);
        all_objects[sig] = a;
        *logger << "Address object: " << name << "\n";
        return a;
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

        net->setComment(comment);
        all_objects[sig] = net;
        *logger << "Network object: " << name << "\n";
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
    *logger << "AddressRange object: " << name << "\n";
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

    QString color = getBadRuleColor().c_str();

    ropt->setStr("color", color.toLatin1().constData());

    if (!rule_comment.empty()) rule_comment += "\n";
    rule_comment += comment;
    //current_rule->setComment(comment);

    *logger << "Parser error: " << comment << "\n";

    error_counter++;
}


