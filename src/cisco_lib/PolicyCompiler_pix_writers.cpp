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


bool PolicyCompiler_pix::PrintObjectGroupsAndClearCommands::processNext()
{
    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);
    string vers = compiler->fw->getStr("version");
    string platform = compiler->fw->getStr("platform");
    string clearACLcmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/")+
        "version_"+vers+"/pix_commands/clear_acl");
    string clearOGcmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/")+
        "version_"+vers+"/pix_commands/clear_og");
    string clearICMPcmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/")+
        "version_"+vers+"/pix_commands/clear_icmp");
    string clearTelnetcmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/")+
        "version_"+vers+"/pix_commands/clear_telnet");


    slurp();
    if (tmp_queue.size()==0) return false;

    if (!compiler->inSingleRuleCompileMode())
    {
        // No need to output "clear" commands in single rule compile mode

        if ( compiler->fw->getOptionsObject()->getBool("pix_acl_basic") )
        {
            compiler->output << clearACLcmd << endl;
            compiler->output << clearOGcmd  << endl;
        }

        if (compiler->fw->getOptionsObject()->getBool("pix_acl_substitution"))
        {
            for (map<string,ciscoACL*>::iterator i=pix_comp->acls.begin();
                 i!=pix_comp->acls.end(); ++i) 
            {
                ciscoACL *acl=(*i).second;
                compiler->output << clearACLcmd << " " << acl->workName() << endl;
            }
            compiler->output << clearOGcmd << endl;
            compiler->output << endl;
        }

        if ( !compiler->fw->getOptionsObject()->getBool("pix_acl_no_clear") )
        {
            compiler->output << clearICMPcmd    << endl;
            compiler->output << clearTelnetcmd  << endl;
        }
    }

    for (FWObject::iterator i=pix_comp->object_groups->begin();
         i!=pix_comp->object_groups->end(); ++i)
    {
        PIXGroup *og=dynamic_cast<PIXGroup*>(*i);
        assert(og!=NULL);

        if (og->size()==0) continue;

        pix_comp->output << endl;

        switch (og->getPIXGroupType()) 
        {
        case NETWORK:     
            pix_comp->output << "object-group network "
                             << og->getName() << endl;
            break;
        case PROTO:       
            pix_comp->output << "object-group protocol "
                             << og->getName() << endl;
            break;
        case ICMP_TYPE:   
            pix_comp->output << "object-group icmp-type "
                             << og->getName() << endl;
            break;
        case TCP_SERVICE: 
            pix_comp->output << "object-group service "
                             << og->getName() << " tcp" << endl;
            break;
        case UDP_SERVICE: 
            pix_comp->output << "object-group service "
                             << og->getName() << " udp" << endl;
            break;
        default:
            compiler->abort("Unknown object group type");
        }

        for (FWObject::iterator i1=og->begin(); i1!=og->end(); ++i1)
        {
            FWObject *o   = *i1;
            FWObject *obj = o;
            if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();

            switch (og->getPIXGroupType()) 
            {
            case NETWORK:
            {
                Address *a=Address::cast(obj);
                assert(a!=NULL);
                const InetAddr *addr = a->getAddressPtr();
                pix_comp->output << " network-object ";
                if (Network::cast(obj)!=NULL)
                {
                    const InetAddr *mask = a->getNetmaskPtr();
                    pix_comp->output << addr->toString() << " ";
                    pix_comp->output << mask->toString() << " ";
                } else {
                    pix_comp->output << " host ";
                    pix_comp->output << addr->toString() << " ";
                }
                pix_comp->output << endl;
                break;
            }
            case PROTO:
            {
                pix_comp->output << " protocol-object ";
                Service *s=Service::cast(obj);
                assert(s!=NULL);
                pix_comp->output << s->getProtocolName();
                pix_comp->output << endl;
                break;
            }
            case ICMP_TYPE:
            {
                pix_comp->output << " icmp-object ";
                ICMPService *s=ICMPService::cast(obj);
                assert(s!=NULL);
                if ( s->getInt("type")== -1)
                    pix_comp->output << "any";
                else
                    pix_comp->output << s->getInt("type");
                pix_comp->output << endl;
                break;
            }
            case TCP_SERVICE:
            case UDP_SERVICE:
            {
                pix_comp->output << " port-object ";
                Service *s=Service::cast(obj);
                assert(s!=NULL);

                int rs=TCPUDPService::cast(s)->getDstRangeStart();
                int re=TCPUDPService::cast(s)->getDstRangeEnd();

                if (rs<0) rs=0;
                if (re<0) re=0;

                if (rs>0 || re>0) {
                    if (rs==re)  compiler->output << "eq " << rs;
                    else         compiler->output << "range " << rs << " " << re;
                }
                else compiler->output << "range 0 65535";
                pix_comp->output << endl;
                break;
            }
            default:
                compiler->abort("Unknown object group type");
            }
        }
        pix_comp->output << " exit" << endl << endl;
    }

    return true;
}


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
    string platform=compiler->fw->getStr("platform");
    string vers=compiler->fw->getStr("version");
    if (platform=="pix" && (vers=="6.1" || vers=="6.2")) return "";

//    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);
    FWOptions  *ruleopt =rule->getOptionsObject();
    ostringstream  str;

    if (ruleopt->getBool("disable_logging_for_this_rule"))
        return "log disable ";

    if (rule->getLogging())
    {
        string level=ruleopt->getStr("log_level");
        int    logint=ruleopt->getInt("log_interval");
/*
 *  PIX always adds logging interval in "show * access-list" command,
 *  so we should always add it, too. Otherwise ACL lines look
 *  different when diff is generated.
 */
        if (logint<=0)
            logint=Resources::platform_res[platform]->getResourceInt(
                string("/FWBuilderResources/Target/options/")+
                "version_"+compiler->fw->getStr("version")+
                "/pix_default_logint");

        if (level.empty()) level= compiler->fw->getOptionsObject()->getStr("pix_logging_trap_level");

        if (!level.empty())
        {
            if (level=="alert")   level="1";
            if (level=="crit")    level="2";
            if (level=="error")   level="3";
            if (level=="warning") level="4";
            if (level=="notice")  level="5";
            if (level=="info")    level="6";
            if (level=="debug")   level="7";
            str << "log " << level << " ";
            if (logint>0 || platform=="pix") // can't use "interval 0" on fwsm
                str << "interval " << logint << " ";
        }
    }
    return str.str();
}

string PolicyCompiler_pix::PrintRule::_printSrcService(Service *srv)
{
    ostringstream  str;

    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int rs=TCPUDPService::cast(srv)->getSrcRangeStart();
	int re=TCPUDPService::cast(srv)->getSrcRangeEnd();

        if (rs<0) rs=0;
        if (re<0) re=0;

	if (rs>0 || re>0)
        {
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

string PolicyCompiler_pix::PrintRule::_printDstService(Service *srv)
{
    ostringstream  str;

    if (TCPService::isA(srv) || UDPService::isA(srv))
    {
	int rs=TCPUDPService::cast(srv)->getDstRangeStart();
	int re=TCPUDPService::cast(srv)->getDstRangeEnd();

        if (rs<0) rs=0;
        if (re<0) re=0;

	if (rs>0 || re>0)
        {
	    if (rs==re)  str << "eq " << rs << " ";
	    else
		if (rs==0 && re!=0)      str << "lt " << re << " ";
		else
		    if (rs!=0 && re==65535)  str << "gt " << rs << " ";
		    else
			str << "range " << rs << " " << re << " ";
	}
    }
    if (ICMPService::isA(srv) && srv->getInt("type")!=-1)
	    str << srv->getStr("type") << " ";

    const IPService *ip_srv = IPService::constcast(srv);
    if (ip_srv && ip_srv->hasIpOptions())
        compiler->warning("PIX can not match IP options");

    return str.str();
}

string PolicyCompiler_pix::PrintRule::_printAddr(libfwbuilder::Address  *o)
{
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
        InetAddr srcmask = *(o->getNetmaskPtr());

        if (Interface::cast(o)!=NULL)
            srcmask = InetAddr(InetAddr::getAllOnes());

        if (IPv4::cast(o)!=NULL) 
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
    i=std::find(seen_icmp_commands.begin(),seen_icmp_commands.end(),cmd);
    if (i!=seen_icmp_commands.end()) return true;
    seen_icmp_commands.push_back(cmd);
    return false;
}

string PolicyCompiler_pix::PrintRule::_printICMPCommand(PolicyRule *rule)
{
    ostringstream  str;

    Address *src   =compiler->getFirstSrc(rule);

    RuleElementSrv *srvrel=rule->getSrv();
    FWObject       *srv=srvrel->front();
    if (FWReference::cast(srv)!=NULL) srv=FWReference::cast(srv)->getPointer();

    Interface *rule_iface = Interface::cast(compiler->dbcopy->findInIndex(rule->getInterfaceId()));
    assert(rule_iface);

    if ( PIXGroup::cast(srv)!=NULL && 
         PIXGroup::cast(srv)->getPIXGroupType()==ICMP_TYPE)
    {
        for (FWObject::iterator i1=srv->begin(); i1!=srv->end(); ++i1)
        {
            FWObject *o   = *i1;
            FWObject *obj = o;
            if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();

            ICMPService *s=ICMPService::cast(obj);
            assert(s!=NULL);

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

string    PolicyCompiler_pix::PrintRule::_printSSHTelnetCommand(PolicyRule *rule)
{
//    Helper helper(this);

    ostringstream  str;
    int port;

    RuleElementSrc *rel=rule->getSrc();
    Service        *srv=compiler->getFirstSrv(rule);

    Interface *rule_iface = Interface::cast(compiler->dbcopy->findInIndex(rule->getInterfaceId()));
    assert(rule_iface);
    
    port=TCPUDPService::cast(srv)->getDstRangeStart();

    for (FWObject::iterator i=rel->begin(); i!=rel->end(); ++i)
    {
        FWObject *o = *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
//        Address *a;

        if (dynamic_cast<PIXGroup*>(o)!=NULL)
        {
            for (FWObject::iterator j=o->begin(); j!=o->end(); ++j)
            {
                FWObject *o1 = *j;
                if (FWReference::cast(o1)!=NULL) 
                    o1=FWReference::cast(o1)->getPointer();
                Address *a=Address::cast(o1);
                assert(a!=NULL);
                str << _printSingleSSHTelnetCommand(
                    port, a, rule_iface->getLabel());
            }
        } else
        {
            Address *a=Address::cast(o);
            assert(a!=NULL);
            str << _printSingleSSHTelnetCommand(
                port, a, rule_iface->getLabel());
        }
    }

    return str.str();
}

string PolicyCompiler_pix::PrintRule::_printSingleSSHTelnetCommand(int port,
                                                Address *a,
                                                const string &interfaceLabel)
{
    string res;

    if (port==23) 
    {
        res += "telnet ";
        res += a->getAddressPtr()->toString() + " " 
            + a->getNetmaskPtr()->toString() + " "
            + interfaceLabel     + "\n";
    }
    if (port==22) 
    {
        res += "ssh    ";
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
    PolicyCompiler_pix *pix_comp=dynamic_cast<PolicyCompiler_pix*>(compiler);
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
//    FWOptions  *ruleopt =rule->getOptionsObject();
    bool write_comments= compiler->fw->getOptionsObject()->getBool("pix_include_comments");

    tmp_queue.push_back(rule);

    ostringstream  comment;
    
    string rl=rule->getLabel();

    if (write_comments && !compiler->inSingleRuleCompileMode())
    {
        if (rl!=current_rule_label1)
        {
            comment << "! " << endl;
            comment << "! Rule  " << rl << endl;

            string    comm=rule->getComment();
            string::size_type c1,c2;
            c1=0;
            while ( (c2=comm.find('\n',c1))!=string::npos ) {
                comment << "! " << comm.substr(c1,c2-c1) << endl;
                c1=c2+1;
            }
            comment << "! " << comm.substr(c1) << endl;
            comment << "! " << endl;

            current_rule_label1 = rl;
            compiler->output << comment.str();
        }
    }

    string err = rule->getStr(".error_msg");
    if (!err.empty()) compiler->output << "! " << err << endl;

    if (rule->getBool("icmp_cmd"))
    {
        compiler->output << _printICMPCommand(rule);
// need to generate access list command as well as icmp command
// in order to properly serve icmp  through nat
//   04/21/06 --vk
//        return true;
    } 

    if (rule->getBool("ssh_telnet_cmd"))
    {
        compiler->output << _printSSHTelnetCommand(rule);
        return true;
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

    ostringstream  aclstr;

    string acl_name=rule->getStr("acl");
    assert (acl_name!="");

    ciscoACL *acl = pix_comp->acls[acl_name];
    assert(acl!=NULL);

    if (compiler->fw->getOptionsObject()->getBool("pix_use_acl_remarks"))
    {
        compiler->output << acl->addRemark(rule->getLabel(), rule->getComment());
    }

    /*
     * Assemble ACL command in aclstr
     */

    aclstr << _printAction(rule);

    /*
     * processor splitServices guaranties that rule has services of
     * the same type (that is, the same protocol, like all tcp, all
     * udp, all icmp or all IP with the same protocol number). PIX can
     * use object-group for protocol only if protocol numbers are
     * different and these are not icmp/tcp/udp protocols. This means
     * that because of processor splitServices we never use
     * object-group in protocol part of ACL.
     */

    PIXGroup *pgsrv = PIXGroup::cast(srvobj);
    PIXGroup *pgsrc = PIXGroup::cast(srcobj);
    PIXGroup *pgdst = PIXGroup::cast(dstobj);

    if ( pgsrv!=NULL && pgsrv->isServiceGroup())
        aclstr << pgsrv->getSrvTypeName();
    else
        aclstr << Service::cast(srvobj)->getProtocolName();

    aclstr << " ";

    if ( pgsrc!=NULL && pgsrc->isObjectGroup())
    {
        aclstr << "object-group " << srcobj->getName();
        aclstr << " ";
    } else
    {
        aclstr << _printAddr( compiler->getFirstSrc(rule) );
    }

    if ( pgsrv==NULL ) 
        aclstr << _printSrcService( compiler->getFirstSrv(rule) );

    if ( pgdst!=NULL && pgdst->isObjectGroup())
    {
        aclstr << "object-group " << dstobj->getName();
        aclstr << " ";
    } else 
        aclstr << _printAddr( compiler->getFirstDst(rule) );


    if ( pgsrv!=NULL )
    {
        aclstr << "object-group " << srvobj->getName();
        aclstr << " ";
    } else 
        aclstr << _printDstService( compiler->getFirstSrv(rule) );

    aclstr << _printLog( rule );

//    aclstr << endl;

    compiler->output << acl->addLine(aclstr.str());

    return true;
}

