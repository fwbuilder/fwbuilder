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

#include "PolicyCompiler_iosacl.h"

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
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"

#include <iostream>
#if __GNUC__ > 3 || \
    (__GNUC__ == 3 && (__GNUC_MINOR__ > 2 || (__GNUC_MINOR__ == 2 ) ) ) || \
    _MSC_VER
#  include <streambuf>
#else
#  include <streambuf.h>
#endif
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


bool PolicyCompiler_iosacl::ClearACLs::processNext()
{
    PolicyCompiler_iosacl *iosacl_comp=dynamic_cast<PolicyCompiler_iosacl*>(compiler);
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
        for (map<string,ciscoACL*>::iterator i=iosacl_comp->acls.begin();
             i!=iosacl_comp->acls.end(); ++i) 
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

void PolicyCompiler_iosacl::PrintCompleteACLs::printRulesForACL::operator()(
    Rule* rule)
{
    // print rule if it belongs to ACL <acl>

    PolicyRule *prule = PolicyRule::cast(rule);

    string acl_name = prule->getStr("acl");
    assert (acl_name!="");

    ciscoACL *rule_acl = iosacl_comp->acls[acl_name];
    assert(rule_acl!=NULL);

    if (acl == rule_acl)
    {
        *output << print_acl_p->_printRule(prule);
    }
}

bool PolicyCompiler_iosacl::PrintCompleteACLs::processNext()
{
    PolicyCompiler_iosacl *iosacl_comp=dynamic_cast<PolicyCompiler_iosacl*>(compiler);
    string vers = compiler->fw->getStr("version");
    string platform = compiler->fw->getStr("platform");
    string clearACLCmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/")+
        "version_"+vers+"/iosacl_commands/clear_ip_acl");

    assert( !clearACLCmd.empty());

    slurp();
    if (tmp_queue.size()==0) return false;

    if ( compiler->fw->getOptionsObject()->getBool("iosacl_acl_basic") ||
         compiler->fw->getOptionsObject()->getBool("iosacl_acl_substitution"))
    {
        for (map<string,ciscoACL*>::iterator i=iosacl_comp->acls.begin();
             i!=iosacl_comp->acls.end(); ++i) 
        {
            ciscoACL *acl=(*i).second;
            compiler->output << clearACLCmd << " " << acl->workName() << endl;
        }
        compiler->output << endl;
    }

    string addr_family_prefix = "ip";
    if (iosacl_comp->ipv6) addr_family_prefix = "ipv6";

    for (map<string,ciscoACL*>::iterator i=iosacl_comp->acls.begin();
         i!=iosacl_comp->acls.end(); ++i) 
    {
        ciscoACL *acl=(*i).second;
        compiler->output << addr_family_prefix
                         << " access-list extended "
                         << acl->workName() << endl;
        std::for_each(tmp_queue.begin(), tmp_queue.end(),
                      printRulesForACL(iosacl_comp,
                                       this, acl, &(compiler->output)));
        compiler->output << "exit" << endl;
        compiler->output << endl;
    }

    return true;
}

string PolicyCompiler_iosacl::PrintRule::_printRule(PolicyRule *rule)
{
    PolicyCompiler_iosacl *iosacl_comp = 
        dynamic_cast<PolicyCompiler_iosacl*>(compiler);
    //FWOptions  *ruleopt =rule->getOptionsObject();
    bool write_comments =
        compiler->fw->getOptionsObject()->getBool("iosacl_include_comments");

    ostringstream  ruleout;
    ostringstream  aclstr;

    string rl=rule->getLabel();

    if (write_comments)
    {
        if (rl!=current_rule_label1)
        {
            ruleout << "! " << endl;
            ruleout << "! Rule  " << rl << endl;

            string    comm=rule->getComment();
            string::size_type c1,c2;
            c1=0;
            while ( (c2=comm.find('\n',c1))!=string::npos ) {
                ruleout << "! " << comm.substr(c1,c2-c1) << endl;
                c1=c2+1;
            }
            ruleout << "! " << comm.substr(c1) << endl;
            ruleout << "! " << endl;

            current_rule_label1=rl;
        }
    }

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

    FWObject *srcobj=src->front();
    FWObject *dstobj=dst->front();
    FWObject *srvobj=srv->front();

    assert(srcobj);
    assert(dstobj);
    assert(srvobj);

    if (FWReference::cast(srcobj)!=NULL)
    {
        srcobj=FWReference::cast(srcobj)->getPointer();
        assert(srcobj);
    }
    if (FWReference::cast(dstobj)!=NULL)
    {
        dstobj=FWReference::cast(dstobj)->getPointer();
        assert(dstobj);
    }
    if (FWReference::cast(srvobj)!=NULL)
    {
        srvobj=FWReference::cast(srvobj)->getPointer();
        assert(srvobj);
    }

    string acl_name=rule->getStr("acl");
    assert (acl_name!="");

    ciscoACL *acl = iosacl_comp->acls[acl_name];
    assert(acl!=NULL);

    /*
     * Assemble ACL command in aclstr
     */

    aclstr << _printAction(rule);

    aclstr << _printProtocol(Service::cast(srvobj));
    aclstr << _printAddr( compiler->getFirstSrc(rule) );
    aclstr << _printSrcService( compiler->getFirstSrv(rule) );
    aclstr << _printAddr( compiler->getFirstDst(rule) );
    aclstr << _printDstService( compiler->getFirstSrv(rule) );
    aclstr << _printLog( rule );
    // "fragments" should be the last option in the access-list command
    aclstr << _printFragm( compiler->getFirstSrv(rule) );
    aclstr << _printTOS( compiler->getFirstSrv(rule) );

//    aclstr << endl;

    ruleout << acl->addLine(aclstr.str());

    return ruleout.str();
}

string PolicyCompiler_iosacl::PrintRule::_printAction(PolicyRule *rule)
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

string PolicyCompiler_iosacl::PrintRule::_printACL(PolicyRule *rule)
{
//    PolicyCompiler_iosacl *iosacl_comp=dynamic_cast<PolicyCompiler_iosacl*>(compiler);

    string acl_name=rule->getStr("acl");
    assert (acl_name!="");

    return acl_name+" ";
}

string PolicyCompiler_iosacl::PrintRule::_printLog(PolicyRule *rule)
{
    if (rule->getLogging())
    {
        FWOptions  *ruleopt =rule->getOptionsObject();
        if (ruleopt->getBool("iosacl_log_input")) return "log-input ";
        return "log ";
    }
    return "";
}

string PolicyCompiler_iosacl::PrintRule::_printSrcService(Service *srv)
{
    ostringstream  str;

    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int rs=TCPUDPService::cast(srv)->getSrcRangeStart();
	int re=TCPUDPService::cast(srv)->getSrcRangeEnd();

        if (rs<0) rs=0;
        if (re<0) re=0;

	if (rs>0 || re>0) {
	    if (rs==re)  str << "eq " << rs << " ";
	    else
		if (rs==0 && re!=0)      str << "lt " << re << " ";
		else
		    if (rs!=0 && re==65535)  str << "gt " << rs << " ";
		    else
			str << "range " << rs << " " << re << " ";
	}
    }

    return str.str();
}

string PolicyCompiler_iosacl::PrintRule::_printFragm(Service *srv)
{
    if (IPService::isA(srv) && (
            srv->getBool("fragm") || srv->getBool("short_fragm")))
        return "fragments ";
    
    return "";
}

string PolicyCompiler_iosacl::PrintRule::_printTOS(Service *srv)
{
    const IPService *ip;
    if ((ip=IPService::constcast(srv))!=NULL)
    {
        string tos = ip->getTOSCode();
        string dscp = ip->getDSCPCode();
        if (!dscp.empty()) return string("dscp ") + dscp;
        else
            if (!tos.empty()) return string("tos ") + tos;
    }
    return "";
}

string PolicyCompiler_iosacl::PrintRule::_printDstService(Service *srv)
{
    ostringstream  str;

    if (TCPService::isA(srv) || UDPService::isA(srv))
    {
	int rs=TCPUDPService::cast(srv)->getDstRangeStart();
	int re=TCPUDPService::cast(srv)->getDstRangeEnd();

        if (rs<0) rs=0;
        if (re<0) re=0;

	if (rs>0 || re>0) {
	    if (rs==re)  str << "eq " << rs << " ";
	    else
		if (rs==0 && re!=0)      str << "lt " << re << " ";
		else
		    if (rs!=0 && re==65535)  str << "gt " << rs << " ";
		    else
			str << "range " << rs << " " << re << " ";
	}
    }
    if (TCPService::isA(srv) && srv->getBool("established"))
        str << "established ";

    if (ICMPService::isA(srv) && srv->getInt("type")!=-1)
	    str << srv->getStr("type") << " ";

    if (CustomService::isA(srv)) 
	str << CustomService::cast(srv)->getCodeForPlatform(
            compiler->myPlatformName() ) << " ";

    return str.str();
}

string PolicyCompiler_iosacl::PrintRule::_printProtocol(Service *srv)
{
    PolicyCompiler_iosacl *iosacl_comp = dynamic_cast<PolicyCompiler_iosacl*>(
        compiler);
    string addr_family_prefix = "ip ";
    if (iosacl_comp->ipv6) addr_family_prefix = "ipv6 ";

    string proto = srv->getProtocolName();
    if (proto=="ip") return addr_family_prefix;
    return proto + " ";
}

string PolicyCompiler_iosacl::PrintRule::_printAddr(Address  *o)
{
    PolicyCompiler_iosacl *iosacl_comp=dynamic_cast<PolicyCompiler_iosacl*>(compiler);
    if (Interface::cast(o)!=NULL)
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
        const InetAddr srcmask = *(o->getNetmaskPtr());

        if (srcaddr->isAny() && srcmask.isAny())
        {
            str << "any  ";
        } else 
        {
            if (Interface::cast(o)==NULL &&
                Interface::cast(o->getParent())==NULL &&
                !srcmask.isHostMask())
            {
                if (iosacl_comp->ipv6)
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
bool PolicyCompiler_iosacl::PrintRule::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    compiler->output << _printRule(rule);

    return true;
}

