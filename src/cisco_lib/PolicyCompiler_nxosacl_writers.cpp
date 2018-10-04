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

#include "PolicyCompiler_nxosacl.h"
#include "NXOSObjectGroup.h"
#include "NamedObjectsAndGroupsSupport.h"
#include "PortRangeConverter.h"

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


bool PolicyCompiler_nxosacl::ClearACLs::processNext()
{
    PolicyCompiler_nxosacl *nxosacl_comp=dynamic_cast<PolicyCompiler_nxosacl*>(compiler);
    string vers = compiler->fw->getStr("version");
    string platform = compiler->fw->getStr("platform");
    string clearACLcmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/")+
        "version_"+vers+"/nxosacl_commands/clear_acl");


    slurp();
    if (tmp_queue.size()==0) return false;

    if ( compiler->fw->getOptionsObject()->getBool("nxosacl_acl_basic") )
    {
        compiler->output << clearACLcmd << endl;
    }

    if (compiler->fw->getOptionsObject()->getBool("nxosacl_acl_substitution"))
    {
        for (map<string,ciscoACL*>::iterator i=nxosacl_comp->acls.begin();
             i!=nxosacl_comp->acls.end(); ++i) 
        {
            ciscoACL *acl=(*i).second;
            compiler->output << clearACLcmd << " " << acl->workName() << endl;
        }
        compiler->output << endl;
    }

    if ( !compiler->fw->getOptionsObject()->getBool("nxosacl_acl_no_clear") )
    {
        string clearICMPcmd = Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/")+
            "version_"+vers+"/nxosacl_commands/clear_icmp");
        string clearTelnetcmd = Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/")+
            "version_"+vers+"/nxosacl_commands/clear_telnet");
        string clearSSHcmd = Resources::platform_res[platform]->getResourceStr(
            string("/FWBuilderResources/Target/options/")+
            "version_"+vers+"/nxosacl_commands/clear_ssh");
        //compiler->output << clearICMPcmd    << endl;
        //compiler->output << clearTelnetcmd  << endl;
        //compiler->output << clearSSHcmd     << endl;
    }

    return true;
}

void PolicyCompiler_nxosacl::PrintCompleteACLs::printRulesForACL::operator()(
    Rule* rule)
{
    // print rule if it belongs to ACL <acl>

    PolicyRule *prule = PolicyRule::cast(rule);

    string acl_name = prule->getStr("acl");
    assert (acl_name!="");

    ciscoACL *rule_acl = nxosacl_comp->acls[acl_name];
    assert(rule_acl!=nullptr);

    if (acl == rule_acl)
    {
        *output << print_acl_p->_printRule(prule);
    }
}

bool PolicyCompiler_nxosacl::PrintCompleteACLs::processNext()
{
    PolicyCompiler_nxosacl *nxosacl_comp=dynamic_cast<PolicyCompiler_nxosacl*>(compiler);

    slurp();
    if (tmp_queue.size()==0) return false;

    string addr_family_prefix = "ip";
    if (nxosacl_comp->ipv6) addr_family_prefix = "ipv6";

    for (map<string,ciscoACL*>::iterator i=nxosacl_comp->acls.begin();
         i!=nxosacl_comp->acls.end(); ++i) 
    {
        ciscoACL *acl=(*i).second;
        compiler->output << addr_family_prefix
                         << " access-list ";

//        if (!nxosacl_comp->ipv6) compiler->output << "extended ";

        compiler->output<< acl->workName() << endl;
        std::for_each(tmp_queue.begin(), tmp_queue.end(),
                      printRulesForACL(nxosacl_comp,
                                       this, acl, &(compiler->output)));
        compiler->output << "exit" << endl;
        compiler->output << endl;
    }

    return true;
}

string PolicyCompiler_nxosacl::PrintRule::_printRule(PolicyRule *rule)
{
    PolicyCompiler_nxosacl *nxosacl_comp = 
        dynamic_cast<PolicyCompiler_nxosacl*>(compiler);
    string platform = compiler->fw->getStr("platform");
    //FWOptions  *ruleopt =rule->getOptionsObject();
    bool write_comments = compiler->fw->getOptionsObject()->getBool(
        platform + "_include_comments");

    ostringstream  ruleout;
    ostringstream  aclstr;

    if (write_comments)
        compiler->output << compiler->printComment(
            rule, current_rule_label1, nxosacl_comp->comment_symbol);

    /*
     * all three rule elements contain exactly one object, which can
     * be either group (in case processor CreateObjectGroups created
     * object group for it) or a regular object
     */
    RuleElementSrc *src=rule->getSrc();
    RuleElementDst *dst=rule->getDst();
    RuleElementSrv *srv=rule->getSrv();

    assert(src->size()==1);
    assert(dst->size()==1);
    assert(srv->size()==1);

    FWObject *srcobj = src->front();
    FWObject *dstobj = dst->front();
    FWObject *srvobj = srv->front();

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

    ciscoACL *acl = nxosacl_comp->acls[acl_name];
    assert(acl!=nullptr);

    /*
     * Assemble ACL command in aclstr
     */

    aclstr << _printAction(rule);

    NXOSObjectGroup *pgsrc = NXOSObjectGroup::cast(srcobj);
    NXOSObjectGroup *pgdst = NXOSObjectGroup::cast(dstobj);
    NXOSObjectGroup *pgsrv = NXOSObjectGroup::cast(srvobj);

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
    // nxosacl and procurve. This is historical.
    if (compiler->fw->getOptionsObject()->getBool("use_acl_remarks"))
    {
        ruleout << acl->addRemark(rule->getLabel(), rule->getComment());
    }

    ruleout << acl->addLine(aclstr.str());

    return ruleout.str();
}

string PolicyCompiler_nxosacl::PrintRule::_printAction(PolicyRule *rule)
{
    ostringstream  str;

    switch (rule->getAction()) {
    case PolicyRule::Accept:  str << "permit "; break;
    case PolicyRule::Deny:    str << "deny   "; break;
    case PolicyRule::Reject:  str << "deny   "; break;
    default:		      str << rule->getActionAsString() << " ";
    }
    return str.str();
}

string PolicyCompiler_nxosacl::PrintRule::_printACL(PolicyRule *rule)
{
//    PolicyCompiler_nxosacl *nxosacl_comp=dynamic_cast<PolicyCompiler_nxosacl*>(compiler);

    string acl_name=rule->getStr("acl");
    assert (acl_name!="");

    return acl_name+" ";
}

string PolicyCompiler_nxosacl::PrintRule::_printLog(PolicyRule *rule)
{
    if (rule->getLogging())
    {
        FWOptions  *ruleopt =rule->getOptionsObject();
        if (ruleopt->getBool("nxosacl_log_input")) return "log-input ";
        return "log ";
    }
    return "";
}

string PolicyCompiler_nxosacl::PrintRule::_printPortRangeOp(int rs, int re)
{
    return PortRangeConverter(rs, re).toString();
}

string PolicyCompiler_nxosacl::PrintRule::_printSrcService(Service *srv)
{
    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int rs = TCPUDPService::cast(srv)->getSrcRangeStart();
	int re = TCPUDPService::cast(srv)->getSrcRangeEnd();
        return _printPortRangeOp(rs, re);
    }

    return "";
}

string PolicyCompiler_nxosacl::PrintRule::_printIPServiceOptions(PolicyRule *r)
{
    Service *srv = compiler->getFirstSrv(r);
    const IPService *ip;
    if ((ip=IPService::constcast(srv))!=nullptr)
    {
        string version = compiler->fw->getStr("version");

        if (srv->getBool("fragm") || srv->getBool("short_fragm"))
            return "fragments ";

        if (ip->hasIpOptions() && XMLTools::version_compare(version, "12.4")<0)
            compiler->abort(r, "IP options match requires NXOS v12.4 or later.");

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

string PolicyCompiler_nxosacl::PrintRule::_printDstService(Service *srv)
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
        if (srv->getBool("established")) str << "established ";
        else str << _printTCPFlags(TCPService::cast(srv));
    }

    if ((ICMPService::isA(srv) || ICMP6Service::isA(srv)) &&
        srv->getInt("type")!=-1)
    {
        str << srv->getStr("type") << " ";
    }

    if (CustomService::isA(srv)) 
	str << CustomService::cast(srv)->getCodeForPlatform(
            compiler->myPlatformName() ) << " ";

    return str.str();
}

string PolicyCompiler_nxosacl::PrintRule::getTcpFlagName(const TCPService::TCPFlag f)
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

string PolicyCompiler_nxosacl::PrintRule::_printTCPFlags(TCPService *srv)
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

string PolicyCompiler_nxosacl::PrintRule::_printProtocol(Service *srv)
{
    PolicyCompiler_nxosacl *nxosacl_comp = dynamic_cast<PolicyCompiler_nxosacl*>(
        compiler);
    string addr_family_prefix = "ip ";
    if (nxosacl_comp->ipv6) addr_family_prefix = "ipv6 ";

    string proto = srv->getProtocolName();
    if (ICMP6Service::isA(srv)) proto = "icmp";

    if (CustomService::isA(srv)) 
    {
        // special case standard CusctomService objects "ESTABLISHED"
        // and "ESTABLISHED ipv6": these require protocol "tcp" but
        // protocol is set in the Custom Service object for all
        // platforms at once, so we can't have protocol defined only
        // for nxosacl to be used here.
	string srv_code = CustomService::cast(srv)->getCodeForPlatform(
            compiler->myPlatformName());
        if (srv_code == "established") proto = "tcp";
    }

    if (proto=="ip") return addr_family_prefix;
    return proto + " ";
}

string PolicyCompiler_nxosacl::PrintRule::_printAddr(Address  *o)
{
    PolicyCompiler_nxosacl *nxosacl_comp = dynamic_cast<PolicyCompiler_nxosacl*>(compiler);
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
                if (nxosacl_comp->ipv6)
                {
                    str << srcaddr->toString()
                        << "/"
                        << srcmask.getLength() << " ";
                } else
                {
                    str << srcaddr->toString() << " ";
                    // cisco uses "wildcards" instead of netmasks
                    //long nm = srcmask.to32BitInt();
                    //struct in_addr na;
                    //na.s_addr = ~nm;
                    InetAddr nnm( ~srcmask );
                    str << nnm.toString() << " ";                    
                }
            } else
            {
                str << "host " << srcaddr->toString() << " ";
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
bool PolicyCompiler_nxosacl::PrintRule::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    compiler->output << _printRule(rule);

    return true;
}

