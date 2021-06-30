/* 

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#include "PolicyCompiler_junosacl.h"
#include "cisco_lib/IOSObjectGroup.h"
// #include "NamedObjectsAndGroupsSupport.h"
#include "cisco_lib/PortRangeConverter.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/XMLTools.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>

#include <assert.h>

#include <QStringList>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


bool PolicyCompiler_junosacl::ClearACLs::processNext()
{
    PolicyCompiler_junosacl *junosacl_comp=dynamic_cast<PolicyCompiler_junosacl*>(compiler);
    string vers = compiler->fw->getStr("version");
    string platform = compiler->fw->getStr("platform");
    string clearACLcmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/")+
        "version_"+vers+"/iosacl_commands/clear_acl");


    slurp();
    if (tmp_queue.size()==0) return false;

    if ( compiler->fw->getOptionsObject()->getBool("iosacl_acl_basic") )
    {
        compiler->output << clearACLcmd << endl;
    }

    if (compiler->fw->getOptionsObject()->getBool("iosacl_acl_substitution"))
    {
        for (map<string,ciscoACL*>::iterator i=junosacl_comp->acls.begin();
             i!=junosacl_comp->acls.end(); ++i)
        {
            ciscoACL *acl=(*i).second;
            compiler->output << clearACLcmd << " " << acl->workName() << endl;
        }
        compiler->output << endl;
    }

    if ( !compiler->fw->getOptionsObject()->getBool("iosacl_acl_no_clear") )
    {
        string clearICMPcmd = Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/")+
            "version_"+vers+"/iosacl_commands/clear_icmp");
        string clearTelnetcmd = Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/")+
            "version_"+vers+"/iosacl_commands/clear_telnet");
        string clearSSHcmd = Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/")+
            "version_"+vers+"/iosacl_commands/clear_ssh");
        //compiler->output << clearICMPcmd    << endl;
        //compiler->output << clearTelnetcmd  << endl;
        //compiler->output << clearSSHcmd     << endl;
    }

    return true;
}

void PolicyCompiler_junosacl::PrintCompleteACLs::printRulesForACL::operator()(
    Rule* rule)
{
    // print rule if it belongs to ACL <acl>

    PolicyRule *prule = PolicyRule::cast(rule);

    string acl_name = prule->getStr("acl");
    assert (acl_name!="");

    ciscoACL *rule_acl = iosacl_comp->acls[acl_name];
    assert(rule_acl!=nullptr);

    if (acl == rule_acl)
    {
        *output << print_acl_p->_printRule(prule);
    }
}

bool PolicyCompiler_junosacl::PrintCompleteACLs::processNext()
{
    PolicyCompiler_junosacl *junosacl_comp=dynamic_cast<PolicyCompiler_junosacl*>(compiler);

    slurp();
    if (tmp_queue.size()==0) return false;

    string addr_family_prefix = "family inet {\n";
    if (junosacl_comp->ipv6) addr_family_prefix = "family inet6 {\n";

    for (map<string,ciscoACL*>::iterator i=junosacl_comp->acls.begin();
         i!=junosacl_comp->acls.end(); ++i)
    {
        ciscoACL *acl=(*i).second;
        compiler->output << "firewall {\n" << "    " << addr_family_prefix
                         << "        " << "replace:\n"
                         << "        " << "filter ";

        string filter_prefix = compiler->fw->getOptionsObject()->getStr("filter_prefix");
        if (filter_prefix.empty()) filter_prefix = "fwbfilter";
        filter_prefix += "_";

        compiler->output<< filter_prefix << acl->workName() << " {\n";
        std::for_each(tmp_queue.begin(), tmp_queue.end(),
                      printRulesForACL(junosacl_comp,
                                       this, acl, &(compiler->output)));
        compiler->output << "        }\n    }\n}" << endl;
    }

    return true;
}

string PolicyCompiler_junosacl::PrintRule::_printRule(PolicyRule *rule)
{
    PolicyCompiler_junosacl *junosacl_comp=dynamic_cast<PolicyCompiler_junosacl*>(compiler);
    ostringstream ruleout;

    string platform = compiler->fw->getStr("platform");
    bool write_comments = compiler->fw->getOptionsObject()->getBool(
        platform + "_include_comments");

    ruleout << "            term " << termNumber++ << " {\n";

    if (write_comments)
        compiler->output << compiler->printComment(
            rule, current_rule_label1, junosacl_comp->comment_symbol);

    RuleElementSrc *src=rule->getSrc();
    RuleElementDst *dst=rule->getDst();
    RuleElementSrv *srv=rule->getSrv();
    FWObject *srvobj = srv->front();

    bool src_contains_any = false;
    bool dst_contains_any = false;
	bool srv_contains_any = false;

    for (FWObject::iterator i1=src->begin(); i1!=src->end(); i1++) {
        if (Address::cast(FWReference::cast(*i1)->getPointer())->isAny()) {
            src_contains_any = true;
            break;
        }
    }

    for (FWObject::iterator i2=dst->begin(); i2!=dst->end(); i2++) {
        if (Address::cast(FWReference::cast(*i2)->getPointer())->isAny()) {
            dst_contains_any = true;
            break;
        }
    }


    for (FWObject::iterator i3=srv->begin(); i3!=srv->end(); i3++) {
        if (Service::cast(FWReference::cast(*i3)->getPointer())->isAny()) {
            srv_contains_any = true;
            break;
        }
    }

    if (!src_contains_any || !dst_contains_any || !srv_contains_any)
        ruleout << "                from {\n";

    if ((src->size() > 0) && !src_contains_any) {
        ruleout << "                    source-address {\n";
        FWObject* o;
        for (FWObject::iterator i1=src->begin(); i1!=src->end(); i1++) {
            o = FWReference::cast(*i1)->getPointer();
            ruleout << "                        " << _printAddr(Address::cast(o)) << ";\n";
        }
        ruleout << "                    }\n"; // source-address {
    }

    if ((dst->size() > 0) && !dst_contains_any) {
        ruleout << "                    destination-address {\n";
        FWObject* o;
        for (FWObject::iterator i2=dst->begin(); i2!=dst->end(); i2++) {
            o = FWReference::cast(*i2)->getPointer();
            ruleout << "                        " << _printAddr(Address::cast(o)) << ";\n";
        }
        ruleout << "                    }\n"; // destination-address {
    }

    // BEGIN SERVICE

    string protocol_command = junosacl_comp->ipv6 ? "next-header " : "protocol ";

    if (srv->size() == 1) {

    if (FWReference::cast(srvobj)!=nullptr)
    {
        srvobj=FWReference::cast(srvobj)->getPointer();
        assert(srvobj);
    }

    if ( compiler->getFirstSrv(rule) && !compiler->getFirstSrv(rule)->isAny() )
        ruleout << "                    " << protocol_command << _printProtocol(Service::cast(srvobj)) << ";\n";

    string serviceStr = _printSrcService( compiler->getFirstSrv(rule) );
    if (serviceStr.size())
        ruleout << "                    " << serviceStr << ";\n";

    serviceStr = "";

    serviceStr = _printDstService( compiler->getFirstSrv(rule) );
    if (serviceStr.size()) {
        if (isdigit(serviceStr.at(0)))
            ruleout << "                    destination-port " << serviceStr << ";\n";
        else
            ruleout << "                    " << serviceStr << ";\n";
    }

    } else {
        ruleout << "                    " << protocol_command << _printProtocol(Service::cast(FWReference::cast(srvobj)->getPointer())) << ";\n";
        ruleout << "                    destination-port [ ";
        FWObject* o;
        for (FWObject::iterator i3=srv->begin(); i3!=srv->end(); i3++) {
            o = FWReference::cast(*i3)->getPointer();
            ruleout << _printDstService(TCPUDPService::cast(o)) << " ";
        }
        ruleout << "];\n";
    }
    // END SERVICE


    if (!src_contains_any || !dst_contains_any || !srv_contains_any)
        ruleout << "                }\n"; // from {

    ruleout << "                then {\n";

    string counter_name = rule->getOptionsObject()->getStr("counter_name");
    if (!counter_name.empty())
        ruleout << "                    count " << counter_name << ";\n";

    if (rule->getLogging())
        ruleout << "                    syslog;\n";

    ruleout << "                    " << _printAction(rule) << ";\n";

    ruleout << "                }\n"; // then {

    ruleout << "            }\n"; // term x {

    //return ruleout.str();

    //FWOptions  *ruleopt =rule->getOptionsObject();

//    ostringstream  ruleout;
    ostringstream  aclstr;

//    if (write_comments)
//        compiler->output << compiler->printComment(
//            rule, current_rule_label1, junosacl_comp->comment_symbol);

    /*
     * all three rule elements contain exactly one object, which can
     * be either group (in case processor CreateObjectGroups created
     * object group for it) or a regular object
     */
//    RuleElementSrc *src=rule->getSrc();
//    RuleElementDst *dst=rule->getDst();
//    RuleElementSrv *srv=rule->getSrv();

    //assert(src->size()==1);
    //assert(dst->size()==1);
    //assert(srv->size()==1);

    FWObject *srcobj = src->front();
    FWObject *dstobj = dst->front();
    //FWObject *srvobj = srv->front();

    assert(srcobj);
    assert(dstobj);
    assert(srvobj);

    if (FWReference::cast(srcobj)!=nullptr)
    {
        srcobj=FWReference::cast(srcobj)->getPointer();
        assert(srcobj);
    }
    if (FWReference::cast(dstobj)!=nullptr)
    {
        dstobj=FWReference::cast(dstobj)->getPointer();
        assert(dstobj);
    }
    if (FWReference::cast(srvobj)!=nullptr)
    {
        srvobj=FWReference::cast(srvobj)->getPointer();
        assert(srvobj);
    }

    string acl_name=rule->getStr("acl");
    assert (acl_name!="");

    ciscoACL *acl = junosacl_comp->acls[acl_name];
    assert(acl!=nullptr);

    /*
     * Assemble ACL command in aclstr
     */

    aclstr << _printAction(rule);

    IOSObjectGroup *pgsrc = IOSObjectGroup::cast(srcobj);
    IOSObjectGroup *pgdst = IOSObjectGroup::cast(dstobj);
    IOSObjectGroup *pgsrv = IOSObjectGroup::cast(srvobj);

    /*
     * Possible configurations:
     *
     * permit object-group service_group object-group src_grp object-group dst_grp
     * permit object-group service_group SRC_SPEC DST_SPEC
     * permit <proto> SRC_SPEC <src_ports> DST_SPEC <dst_ports>
     *
     * Where SRC_SPEC and DST_SPEC are
     * obejct-group network_group
     * or traidtional <address> <wildcard_bits>
     *
     */

    if ( pgsrv!=nullptr && pgsrv->isServiceGroup())
    {
        aclstr << "object-group " << pgsrv->getName();
        aclstr << " ";

        if ( pgsrc!=nullptr && pgsrc->isObjectGroup())
        {
            aclstr << "object-group " << pgsrc->getName();
            aclstr << " ";
        } else
        {
            aclstr << _printAddr( compiler->getFirstSrc(rule) );
        }

        if ( pgdst!=nullptr && pgdst->isObjectGroup())
        {
            aclstr << "object-group " << pgdst->getName();
            aclstr << " ";
        } else
        {
            aclstr << _printAddr( compiler->getFirstDst(rule) );
        }
    } else
    {
        // Service is not object group
        aclstr << _printProtocol(Service::cast(srvobj));
        aclstr << _printProtocol(Service::cast(srvobj));
        aclstr << " ";

        if ( pgsrc!=nullptr && pgsrc->isObjectGroup())
        {
            aclstr << "object-group " << pgsrc->getName();
            aclstr << " ";
        } else
        {
            aclstr << _printAddr( compiler->getFirstSrc(rule) );
        }

        aclstr << _printSrcService( compiler->getFirstSrv(rule) );

        if ( pgdst!=nullptr && pgdst->isObjectGroup())
        {
            aclstr << "object-group " << pgdst->getName();
            aclstr << " ";
        } else
        {
            aclstr << _printAddr( compiler->getFirstDst(rule) );
        }

        aclstr << _printDstService( compiler->getFirstSrv(rule) );
    }

    aclstr << _printLog( rule );

    // "fragments" should be the last option in the access-list command
    aclstr << _printIPServiceOptions(rule);

    // Note that option "use_acl_remarks" is set in prolog() because
    // we use different options for this function in GUI dialogs for 
    // iosacl and procurve. This is historical.
    if (compiler->fw->getOptionsObject()->getBool("use_acl_remarks"))
    {
        ruleout << acl->addRemark(rule->getLabel(), rule->getComment());
    }

    acl->addLine(aclstr.str());
    //ruleout << "#" << acl->addLine(aclstr.str());

    return ruleout.str();
}

string PolicyCompiler_junosacl::PrintRule::_printAction(PolicyRule *rule)
{
    ostringstream  str;

    switch (rule->getAction()) {
    case PolicyRule::Accept:  str << "accept"; break;
    case PolicyRule::Deny:    str << "discard"; break;
    case PolicyRule::Reject:  str << "reject";
    {
        FWOptions  *ruleopt =rule->getOptionsObject();
        string reason = ruleopt->getStr("action_on_reject");

        if (!reason.empty())
            str << " " << reject_icmp_reason.at(reason);
    }
        break;
    default:		      str << rule->getActionAsString() << "";
    }
    return str.str();
}

string PolicyCompiler_junosacl::PrintRule::_printACL(PolicyRule *rule)
{
//    PolicyCompiler_iosacl *iosacl_comp=dynamic_cast<PolicyCompiler_iosacl*>(compiler);

    string acl_name=rule->getStr("acl");
    assert (acl_name!="");

    return acl_name+" ";
}

string PolicyCompiler_junosacl::PrintRule::_printLog(PolicyRule *rule)
{
    if (rule->getLogging())
    {
        FWOptions  *ruleopt =rule->getOptionsObject();
        if (ruleopt->getBool("iosacl_log_input")) return "log-input ";
        return "log ";
    }
    return "";
}

string PolicyCompiler_junosacl::PrintRule::_printPortRangeOp(int rs, int re)
{
    std::ostringstream  str;

    if (rs<0) rs = 0;
    if (re<0) re = 0;

    if (rs>0 || re>0)
    {
        if (rs==re)  str << rs;
        else
        {
            if (rs==0 && re!=0)
            {
                str << "1-" << re + 1;
            } else
            {
                if (rs!=0 && re==65535)
                {
                    str << rs << "-65535";
                } else
                {
                    str << rs << "-" << re;
                }
            }
        }
    }

    return str.str();

    return PortRangeConverter(rs, re).toString();
}

string PolicyCompiler_junosacl::PrintRule::_printSrcService(Service *srv)
{
    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int rs = TCPUDPService::cast(srv)->getSrcRangeStart();
	int re = TCPUDPService::cast(srv)->getSrcRangeEnd();
        string outstr = _printPortRangeOp(rs, re);
        if (outstr.size())
            return "source-port " + outstr;
    }

    return "";
}

string PolicyCompiler_junosacl::PrintRule::_printIPServiceOptions(PolicyRule *r)
{
    Service *srv = compiler->getFirstSrv(r);
    const IPService *ip;
    if ((ip=IPService::constcast(srv))!=nullptr)
    {
        string version = compiler->fw->getStr("version");

        if (srv->getBool("fragm") || srv->getBool("short_fragm"))
            return "fragments ";

        if (ip->hasIpOptions() && XMLTools::version_compare(version, "12.4")<0)
            compiler->abort(r, "IP options match requires IOS v12.4 or later.");

        if (ip->getBool("lsrr")) return "option lsr";
        if (ip->getBool("ssrr")) return "option ssr";
        if (ip->getBool("rr")) return "option record-route";
        if (ip->getBool("rtralt")) return "option router-alert";
        if (ip->getBool("any_opt")) return "option any-options ";

        string tos = ip->getTOSCode();
        string dscp = ip->getDSCPCode();
        if (!dscp.empty()) return string("dscp ") + dscp;
        else
            if (!tos.empty()) return string("tos ") + tos;
    }
    return "";
}

string PolicyCompiler_junosacl::PrintRule::_printDstService(Service *srv)
{
    ostringstream  str;

    if (TCPService::isA(srv) || UDPService::isA(srv))
    {
	int rs = TCPUDPService::cast(srv)->getDstRangeStart();
	int re = TCPUDPService::cast(srv)->getDstRangeEnd();
        str << _printPortRangeOp(rs, re);
    }

    if (TCPService::isA(srv))
    {
        if (srv->getBool("established")) str << "tcp-established";
        else str << _printTCPFlags(TCPService::cast(srv));
    }

    if ((ICMPService::isA(srv) || ICMP6Service::isA(srv)) &&
        srv->getInt("type")!=-1)
    {
        str << srv->getStr("type") << " ";
    }

    if (CustomService::isA(srv)) 
	str << CustomService::cast(srv)->getCodeForPlatform(
            compiler->myPlatformName() );

    return str.str();
}

string PolicyCompiler_junosacl::PrintRule::getTcpFlagName(const TCPService::TCPFlag f)
{
    switch (f)
    {
    case TCPService::URG: return "urg";
    case TCPService::ACK: return "ack";
    case TCPService::PSH: return "psh";
    case TCPService::RST: return "rst";
    case TCPService::SYN: return "syn";
    case TCPService::FIN: return "fin";
    default: return "";
    }
    return "";
}

string PolicyCompiler_junosacl::PrintRule::_printTCPFlags(TCPService *srv)
{
    if (srv->inspectFlags())
    {
        // We check the version and call compiler->abort() if its
        // wrong in SpecialServices rule processor. Here we should just execute.
        string version = compiler->fw->getStr("version");
        if (XMLTools::version_compare(version, "12.4")>=0)
        {
            std::set<TCPService::TCPFlag> flags = srv->getAllTCPFlags();
            std::set<TCPService::TCPFlag> masks = srv->getAllTCPFlagMasks();
            std::set<TCPService::TCPFlag>::iterator mit = masks.begin();

            QStringList match_specs;
            for (; mit!=masks.end(); mit++)
            {
                if (flags.count(*mit) > 0)
                    match_specs.push_back(QString("+%1").arg(getTcpFlagName(*mit).c_str()));
                else
                    match_specs.push_back(QString("-%1").arg(getTcpFlagName(*mit).c_str()));
            }
            if (!match_specs.empty())
                match_specs.push_front("match-all");
            return match_specs.join(" ").toStdString() + " ";
        }
    }
    return "";
}

string PolicyCompiler_junosacl::PrintRule::_printProtocol(Service *srv)
{
    PolicyCompiler_junosacl *junosacl_comp = dynamic_cast<PolicyCompiler_junosacl*>(
        compiler);
    string addr_family_prefix = "ip ";
    if (junosacl_comp->ipv6) addr_family_prefix = "ipv6 ";

    string proto = srv->getProtocolName();
    if (ICMP6Service::isA(srv)) proto = "icmp6";

    if (CustomService::isA(srv)) 
    {
        // special case standard CusctomService objects "ESTABLISHED"
        // and "ESTABLISHED ipv6": these require protocol "tcp" but
        // protocol is set in the Custom Service object for all
        // platforms at once, so we can't have protocol defined only
        // for junosacl to be used here.
	string srv_code = CustomService::cast(srv)->getCodeForPlatform(
            compiler->myPlatformName());
        if (srv_code == "tcp-established") proto = "tcp";
    }

    if (proto=="ip") return addr_family_prefix;
    return proto;
}

string PolicyCompiler_junosacl::PrintRule::_printAddr(Address  *o)
{
    PolicyCompiler_junosacl *junosacl_comp = dynamic_cast<PolicyCompiler_junosacl*>(compiler);
    if (Interface::cast(o)!=nullptr)
    {
	Interface *interface_ = Interface::cast(o);
	if (interface_->isDyn())
        {
	    return string("interface ") + interface_->getLabel() + " ";
	}
    }

    ostringstream  str;

    const InetAddr *srcaddr = o->getAddressPtr();
    if (srcaddr)
    {
        const InetAddr *nm = o->getNetmaskPtr();
        InetAddr srcmask;

        if (nm != nullptr)
        {
            srcmask = *nm;
        } else
        {
            cerr << "Address object "
                 << o
                 << " "
                 << o->getName()
                 << " (" << o->getTypeName() << ") "
                 << " has no netmask"
                 << endl;
            srcmask = InetAddr(InetAddr::getAllOnes(srcaddr->addressFamily()));
        }

//        const InetAddr srcmask = *(o->getNetmaskPtr());

        if (srcaddr->isAny() && srcmask.isAny())
        {
            str << "any  ";
        } else
        {
            if (Interface::cast(o)==nullptr &&
                Interface::cast(o->getParent())==nullptr &&
                o->dimension() > 1 &&
                !srcmask.isHostMask())
            {
                if (junosacl_comp->ipv6)
                {
                    str << srcaddr->toString()
                        << "/"
                        << srcmask.getLength();
                } else
                {
                    str << srcaddr->toString()
                        << "/"
                        << srcmask.getLength();
                }
            } else
            {
                if (srcaddr->isV4())
                    str << srcaddr->toString() << "/" << 32;
                else
                    str << srcaddr->toString() << "/" << 128;
//                    str << srcaddr->toString() << "/" << srcmask.getLength();
            }
        }
        return str.str();
    }

    ostringstream errstr;
    errstr << "Object "
           << o->getName()
           << " (id="
           << o->getId()
           << ") "
           << " has no ip address and can not be used "
           << "in the rule.";
    compiler->abort(errstr.str());
    return "";  // to make compiler happy
}

/*
 *   the following additional attributes should have been defined by now:
 *
 *   "acl"  - string, name of the access list
 *            choices are: outside-in, outside-out, inside-in, indside-out,
 *                         dmz-in, dmz-out etc. 
 *            General rule for the acl name: "iface_name-{in,out}"
 */
bool PolicyCompiler_junosacl::PrintRule::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    compiler->output << _printRule(rule);

    return true;
}

