/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

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

#include "PolicyCompiler_pix.h"
#include "PIXObjectGroup.h"
#include "NamedObjectsManager.h"
#include "PortRangeConverter.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"

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


string PolicyCompiler_pix::PrintRule::_printAction(PolicyRule *rule)
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

string PolicyCompiler_pix::PrintRule::_printACL(PolicyRule *rule)
{
//    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);

    string acl_name=rule->getStr("acl");
    assert (acl_name!="");

    return acl_name+" ";
}

string PolicyCompiler_pix::PrintRule::_printLog(PolicyRule *rule)
{
    string platform = compiler->fw->getStr("platform");
    string vers = compiler->fw->getStr("version");
    if (platform=="pix" && (vers=="6.1" || vers=="6.2")) return "";

//    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);
    FWOptions  *ruleopt =rule->getOptionsObject();
    QStringList str;

    if (ruleopt->getBool("disable_logging_for_this_rule"))
        return "log disable ";

    if (rule->getLogging())
    {
        string level = ruleopt->getStr("log_level");
        int    logint = ruleopt->getInt("log_interval");
/*
 *  PIX always adds logging interval in "show * access-list" command,
 *  so we should always add it, too. Otherwise ACL lines look
 *  different when diff is generated.
 */
        if (logint<=0)
            logint = Resources::platform_res[platform]->getResourceInt(
                string("/FWBuilderResources/Target/options/") +
                "version_" + compiler->fw->getStr("version") +
                "/pix_default_logint");

        if (level.empty())
            level = compiler->fw->getOptionsObject()->getStr(
                "pix_logging_trap_level");

        if (level.empty())
            level = Resources::platform_res[platform]->getResourceStr(
                string("/FWBuilderResources/Target/options/") +
                "version_" + compiler->fw->getStr("version") +
                "/pix_default_loglevel");

        if (level=="alert")   level = "1";
        if (level=="crit")    level = "2";
        if (level=="error")   level = "3";
        if (level=="warning") level = "4";
        if (level=="notice")  level = "5";
        if (level=="info")    level = "6";
        if (level=="debug")   level = "7";

        str << "log" << level.c_str();

        if (logint>0 || platform=="pix") // can't use "interval 0" on fwsm
        {
            str << "interval" << QString().setNum(logint);
        }
    }
    return str.join(" ").toStdString();
}

string PolicyCompiler_pix::PrintRule::_printPortRangeOp(int rs, int re)
{
    return PortRangeConverter(rs, re).toString();
}

string PolicyCompiler_pix::PrintRule::_printSrcService(Service *srv)
{
    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int rs = TCPUDPService::cast(srv)->getSrcRangeStart();
	int re = TCPUDPService::cast(srv)->getSrcRangeEnd();
        return _printPortRangeOp(rs, re);
    }
    return "";
}

string PolicyCompiler_pix::PrintRule::_printDstService(Service *srv)
{
    ostringstream  str;

    if (TCPService::isA(srv) || UDPService::isA(srv))
    {
	int rs=TCPUDPService::cast(srv)->getDstRangeStart();
	int re=TCPUDPService::cast(srv)->getDstRangeEnd();
        str <<  _printPortRangeOp(rs, re);
    }

    if (ICMPService::isA(srv) && srv->getInt("type")!=-1)
    {
        str << srv->getStr("type") << " ";
    }

    if (CustomService::isA(srv)) 
    {
	str << CustomService::cast(srv)->getCodeForPlatform(
            compiler->myPlatformName() ) << " ";
    }

    const IPService *ip_srv = IPService::constcast(srv);
    if (ip_srv && ip_srv->hasIpOptions())
        compiler->warning("PIX can not match IP options");

    return str.str();
}

string PolicyCompiler_pix::PrintRule::_printAddr(libfwbuilder::Address  *o)
{
    if (Interface::cast(o)!=nullptr)
    {
	Interface *interface_=Interface::cast(o);
	if (interface_->isDyn())
        {
	    return string("interface ") + interface_->getLabel() + " ";
	}
    }

    ostringstream  str;

    const InetAddr *srcaddr = o->getAddressPtr();
    if (srcaddr)
    {
        InetAddr srcmask = *(o->getNetmaskPtr());

        if (Interface::cast(o)!=nullptr)
            srcmask = InetAddr(InetAddr::getAllOnes());

        if (IPv4::cast(o)!=nullptr) 
            srcmask = InetAddr(InetAddr::getAllOnes());


        if (srcaddr->isAny() && srcmask.isAny())
        {
            str << "any ";
        } else {
            if (srcmask.isHostMask())
            {
                str << "host " << srcaddr->toString() << " ";
            } else
            {
                str << srcaddr->toString() << " ";
                str << srcmask.toString() << " ";
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
    return ""; // to make compiler happy
}

bool PolicyCompiler_pix::PrintRule::suppressDuplicateICMPCommands(const string &cmd)
{
    list<string>::iterator i;
    i = std::find(seen_icmp_commands.begin(), seen_icmp_commands.end(), cmd);
    if (i!=seen_icmp_commands.end()) return true;
    seen_icmp_commands.push_back(cmd);
    return false;
}

string PolicyCompiler_pix::PrintRule::_printICMPCommand(PolicyRule *rule)
{
    ostringstream  str;
    Address *src = compiler->getFirstSrc(rule);
    RuleElementSrv *srvrel = rule->getSrv();
    FWObject *srv = srvrel->front();
    if (FWReference::cast(srv)!=nullptr) srv = FWReference::cast(srv)->getPointer();

    // Interface *rule_iface =
    //     Interface::cast(compiler->dbcopy->findInIndex(rule->getInterfaceId()));

    RuleElementItf *intf_re = rule->getItf();
    Interface *rule_iface = Interface::cast(
        FWObjectReference::getObject(intf_re->front()));

    assert(rule_iface);

    if (PIXObjectGroup::cast(srv)!=nullptr)
    {
        for (FWObject::iterator i1=srv->begin(); i1!=srv->end(); ++i1)
        {
            ICMPService *s = ICMPService::cast(FWReference::getObject(*i1));
            assert(s!=nullptr);

            ostringstream  str1;
            str1 << "icmp ";
            str1 << _printAction(rule);
            str1 << _printAddr( src );
            str1 << s->getStr("type");
            str1 << " ";
            str1 << rule_iface->getLabel();
            str1 << endl;

            if ( ! suppressDuplicateICMPCommands(str1.str()))  str << str1.str();
        }
        return str.str();

    } else
    {
        str << "icmp ";
        str << _printAction(rule);
        str << _printAddr( src );
        str << _printDstService( Service::cast(srv) );
        str << " ";
        str << rule_iface->getLabel();
        str << endl;

        if ( ! suppressDuplicateICMPCommands(str.str()))  return str.str();
    }

    return "";
}

string PolicyCompiler_pix::PrintRule::_printSSHTelnetCommand(PolicyRule *rule)
{
    ostringstream  str;
    int port;

    RuleElementSrc *rel = rule->getSrc();
    Service *srv = compiler->getFirstSrv(rule);

    RuleElementItf *intf_re = rule->getItf();
    Interface *rule_iface = Interface::cast(
        FWObjectReference::getObject(intf_re->front()));
    assert(rule_iface);
    
    port = TCPUDPService::cast(srv)->getDstRangeStart();

    for (FWObject::iterator i=rel->begin(); i!=rel->end(); ++i)
    {
        FWObject *o = FWReference::getObject(*i);

        if (dynamic_cast<PIXObjectGroup*>(o)!=nullptr)
        {
            for (FWObject::iterator j=o->begin(); j!=o->end(); ++j)
            {
                Address *a = Address::cast(FWReference::getObject(*j));
                assert(a!=nullptr);
                str << _printSingleSSHTelnetCommand(port, a, rule_iface->getLabel());
            }
        } else
        {
            Address *a = Address::cast(o);
            assert(a!=nullptr);
            str << _printSingleSSHTelnetCommand(port, a, rule_iface->getLabel());
        }
    }

    return str.str();
}

string PolicyCompiler_pix::PrintRule::_printSingleSSHTelnetCommand(
    int port, Address *a, const string &interfaceLabel)
{
    string res;

    if (port==22) res = "ssh ";
    if (port==23) res = "telnet ";
    if (port==80) res = "http ";

    if (!res.empty())
    {
        res += a->getAddressPtr()->toString() + " " 
            + a->getNetmaskPtr()->toString() + " "
            + interfaceLabel     + "\n";
    }

    return res;
}

/*
 *   the following additional attributes should have been defined by now:
 *
 *   "acl"  - string, name of the access list
 *            choices are: outside-in, outside-out, inside-in, indside-out,
 *                         dmz-in, dmz-out etc. 
 *            General rule for the acl name: "iface_name-{in,out}"
 */
bool PolicyCompiler_pix::PrintRule::processNext()
{
    PolicyCompiler_pix *pix_comp = dynamic_cast<PolicyCompiler_pix*>(compiler);
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    ostringstream comment;
    
    compiler->output << compiler->printComment(rule, current_rule_label1, "!");

    if (rule->getBool("icmp_cmd"))
    {
        compiler->output << _printICMPCommand(rule);
// need to generate access list command as well as icmp command
// in order to properly serve icmp  through nat
//   04/21/06 --vk
//        return true;
    } 

    if (rule->getBool("tcp_service_to_fw"))
    {
        compiler->output << _printSSHTelnetCommand(rule);
        return true;
    } 

    /*
     * all three rule elements contain exactly one object, which can
     * be either group (in case processor CreateObjectGroups created
     * object group for it) or a regular object
     */
    RuleElementSrc *src = rule->getSrc();
    RuleElementDst *dst = rule->getDst();
    RuleElementSrv *srv = rule->getSrv();

    assert(src->size()==1);
    assert(dst->size()==1);
    assert(srv->size()==1);

    FWObject *srcobj = FWReference::getObject(src->front());
    FWObject *dstobj = FWReference::getObject(dst->front());
    FWObject *srvobj = FWReference::getObject(srv->front());

    assert(srcobj);
    assert(dstobj);
    assert(srvobj);

    ostringstream  aclstr;

    string acl_name = rule->getStr("acl");
    assert(acl_name!="");

    ciscoACL *acl = pix_comp->acls[acl_name];
    assert(acl!=nullptr);

    if (compiler->fw->getOptionsObject()->getBool("pix_use_acl_remarks"))
    {
        compiler->output << acl->addRemark(rule->getLabel(), rule->getComment());
    }

    /*
     * Assemble ACL command in aclstr
     */

    aclstr << _printAction(rule);

    /*
     * processor groupServicesByProtocol guaranties that rule has
     * services of the same type (that is, the same protocol, like all
     * tcp, all udp, all icmp or all IP with the same protocol
     * number). PIX can use object-group for protocol only if protocol
     * numbers are different and these are not icmp/tcp/udp
     * protocols. This means that because of processor
     * groupServicesByProtocol we never use object-group in protocol
     * part of ACL.
     */

    PIXObjectGroup *pgsrv = PIXObjectGroup::cast(srvobj);
    PIXObjectGroup *pgsrc = PIXObjectGroup::cast(srcobj);
    PIXObjectGroup *pgdst = PIXObjectGroup::cast(dstobj);
    Service *srv_s = Service::cast(srvobj);
    assert(pgsrv!=nullptr || srv_s!=nullptr);

    if ( pgsrv!=nullptr && pgsrv->isServiceGroup())
    {
        aclstr << pgsrv->getSrvTypeName();
    } else
        aclstr << srv_s->getProtocolName();

    aclstr << " ";

    NamedObject* asa8_object;

    asa8_object = pix_comp->named_objects_manager->getNamedObject(srcobj);
    if (asa8_object)
    {
        aclstr << "object " << asa8_object->getCommandWord().toStdString() << " ";
    } else
    {
        if (pgsrc!=nullptr)
        {
            aclstr << "object-group " << srcobj->getName() << " ";
        } else
        {
            aclstr << _printAddr(Address::cast(srcobj));
        }
    }

    if ( pgsrv==nullptr ) 
        aclstr << _printSrcService( compiler->getFirstSrv(rule) );

    asa8_object = pix_comp->named_objects_manager->getNamedObject(dstobj);
    if (asa8_object)
    {
        aclstr << "object "  << asa8_object->getCommandWord().toStdString() << " ";
    } else
    {
        if (pgdst!=nullptr)
        {
            aclstr << "object-group " << dstobj->getName() << " ";
        } else
        {
            aclstr << _printAddr(Address::cast(dstobj));
        }
    }

    if (pgsrv!=nullptr)
    {
        aclstr << "object-group " << srvobj->getName() << " ";
    } else
    {
        aclstr << _printDstService(Service::cast(srvobj));
    }

    aclstr << _printLog( rule );

//    aclstr << endl;

    compiler->output << acl->addLine(aclstr.str());

    return true;
}

