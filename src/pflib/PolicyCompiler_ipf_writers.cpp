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

#include "PolicyCompiler_ipf.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/DNSName.h"

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;





/**
 *-----------------------------------------------------------------------
 *                    Methods for printing
 */

string PolicyCompiler_ipf::PrintRule::_printPort(int rs,int re,bool neg)
{
    ostringstream  str;

    if (rs<0) rs=0;
    if (re<0) re=0;

    if (!neg) {

	if (rs>0 || re>0)
        {
            if (rs>re && re==0) re=rs;

	    if (rs==re)  str << "= " << rs;
	    else
		if (rs==0 && re!=0)      str << "<= " << re;
		else
		    if (rs!=0 && re==65535)  str << ">= " << rs;
		    else {
/* 
 * port range. Operator '><' defines range in a such way that boundaries
 * are not included. Since we assume it is inclusive, let's move boundaries 
 */
			if (rs>0    ) rs--;
			if (re<65535) re++;
			str << rs << " >< " << re;
		    }
	}
    } else {

	if (rs>0 || re>0) {
	    if (rs==re)  str << "!= " << rs;
	    else
		if (rs==0 && re!=0)      str << "> " << re;
		else
		    if (rs!=0 && re==65535)  str << "< " << rs;
		    else {
			str << rs << " <> " << re;
		    }
	}

    }
    return str.str();
}

void PolicyCompiler_ipf::PrintRule::_printDstService(RuleElement  *rel)
{
    FWObject *o=rel->front();
    if (o && FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

    Service *srv= Service::cast(o);
    IPService *ip_srv = IPService::cast(srv);

    if (ip_srv)
    {
        return;
    } else
        PolicyCompiler_pf::PrintRule::_printDstService(rel);
}


void PolicyCompiler_ipf::PrintRule::_printAction(PolicyRule *rule)
{
/*
 * difference between this and PolicyCompiler_pf::_printAction is as follows:
 *
 * 1. there is no support for action Scrub in ipf
 * 2. there is support for return-icmp-as-dest in ipf
 * 3. there is support for action Skip in ipf
 */

    FWOptions *ruleopt =rule->getOptionsObject();
    Service *srv=compiler->getFirstSrv(rule);    assert(srv);

    switch (rule->getAction()) {
    case PolicyRule::Skip:
        compiler->output << "skip " << rule->getInt("no_to_skip") << " ";
        break;

    case PolicyRule::Accept:
        compiler->output << "pass  ";
        break;

    case PolicyRule::Accounting:
        compiler->output << "count ";
        break;

    case PolicyRule::Deny:
        compiler->output << "block ";
        break;

    case PolicyRule::Reject: 
        if (rule->getDirection()==PolicyRule::Inbound)
        {
            if (TCPService::isA(srv)) compiler->output << "block return-rst ";
            else {
                string aor=ruleopt->getStr("action_on_reject");
                if (aor.empty()) aor=compiler->getCachedFwOpt()->getStr("action_on_reject");
                string code;
                if ( aor.find("ICMP")!=string::npos ) {
                    if (ruleopt->getBool("ipf_return_icmp_as_dest") ||
                        compiler->getCachedFwOpt()->getBool("ipf_return_icmp_as_dest") 
                    )
                        code="return-icmp-as-dest ";
                    else
                        code="return-icmp ";
                    if (aor.find("unreachable")!=string::npos ) {
                        if (aor.find("net")!=string::npos)      code=code+"(0) ";
                        if (aor.find("host")!=string::npos)     code=code+"(1) ";
                        if (aor.find("protocol")!=string::npos) code=code+"(2) ";
                        if (aor.find("port")!=string::npos)     code=code+"(3) ";
                    }
                    if (aor.find("prohibited")!=string::npos ) {
                        if (aor.find("net")!=string::npos)      code=code+"(9) ";
                        if (aor.find("host")!=string::npos)     code=code+"(10) ";
                    }
                } else
                    code="return-icmp   ";

                compiler->output << "block " << code;

            }
        } else
            compiler->output << "block ";
	break;

    case PolicyRule::Custom:
        compiler->output << ruleopt->getStr("custom_str") << " ";
        break;

    default:
        compiler->abort(
            
                rule, 
                string("Unknown action ") + rule->getActionAsString());

//   compiler->output << rule->getActionAsString() << " ";
    }
}

void PolicyCompiler_ipf::PrintRule::_printWith(libfwbuilder::Service *srv)
{
    IPService *ip_srv = IPService::cast(srv);
    if (ip_srv)
    {
        if (ip_srv->getBool("any_opt"))
            compiler->warning("ipfilter can not match \"any IP option\" ");

        bool with=true;
	if ( srv->getBool("short_fragm") ) 
        {
            if (with) { compiler->output << " with"; with=false; }
	    compiler->output << " short";
	}

	if ( srv->getBool("fragm") ) 
        {
            if (with) { compiler->output << " with"; with=false; }
	    compiler->output << " frag";
	}

	if (srv->getBool("rr")   ) 
        {
            if (with) { compiler->output << " with"; with=false; }
            compiler->output << " opt rr";
        }
	if (srv->getBool("lsrr") ) 
        {
            if (with) { compiler->output << " with"; with=false; }
            compiler->output << " opt lsrr";
        }
	if (srv->getBool("ssrr") ) 
        {
            if (with) { compiler->output << " with"; with=false; }
            compiler->output << " opt ssrr";
        }
	if (srv->getBool("ts")   ) 
        {
            if (with) { compiler->output << " with"; with=false; }
            compiler->output << " opt ts";
        }
    }
}

/*
 *  this is almost like the one in PolicyCompiler_pf, except it does
 *  not print interface name for dynamic interface ('cause ipfilter
 *  does not support it)
 */
void PolicyCompiler_ipf::PrintRule::_printAddr(Address  *o,bool neg)
{
    FWOptions* options=compiler->fw->getOptionsObject();

    MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
    if (atrt!=nullptr)
    {
        if (atrt->getSubstitutionTypeName()==DNSName::TYPENAME)
        {
            compiler->output <<  atrt->getSourceName() << " ";
            return;
        }
        // at this time we only support two types of MultiAddress
        // objects: AddressTable and DNSName. Both should be converted
        // to MultiAddressRunTime at this point. If we get some other
        // kind of MultiAddressRunTime object, we do not know what to do
        // with it so we stop.
        assert(atrt==nullptr);
    }

    if (options->getBool("dynAddr") &&
        Interface::cast(o)!=nullptr && Interface::cast(o)->isDyn()) 
    {
        if (neg) compiler->output << "! ";
        compiler->output << "(" << o->getName() << ") ";
        return;
    }

    const InetAddr *addr = o->getAddressPtr();
    if (Interface::cast(o)!=nullptr && addr==nullptr)
    {
        compiler->output << "<thishost> ";
    }
    if (addr)
    {
        InetAddr mask = *(o->getNetmaskPtr());

        if (Interface::cast(o)!=nullptr) {
            mask = InetAddr(InetAddr::getAllOnes());
        }

        if (o->dimension()==1)
        {
            mask = InetAddr(InetAddr::getAllOnes());
        }

        if (addr->isAny() && mask.isAny())
        {
            compiler->output << "any ";
        } else {	
            if (neg) compiler->output << "! ";
            compiler->output << addr->toString();
            if (!mask.isHostMask())
            {
                compiler->output << "/" << mask.getLength();
            }
            compiler->output << " ";
        }
    }
}

PolicyCompiler_ipf::PrintRule::PrintRule(const std::string &name) : PolicyCompiler_pf::PrintRule(name)
{ 
}

bool PolicyCompiler_ipf::PrintRule::processNext()
{
    PolicyRule *rule=getNext(); if (rule==nullptr) return false;
    FWOptions  *ruleopt =rule->getOptionsObject();

    tmp_queue.push_back(rule);

    compiler->output << compiler->printComment(rule, current_rule_label, "#");

    RuleElementSrc *srcrel=rule->getSrc();
#ifndef NDEBUG
    Address        *src   =compiler->getFirstSrc(rule);  assert(src);
    Address        *dst   =compiler->getFirstDst(rule);  assert(dst);
#endif
    RuleElementDst *dstrel=rule->getDst();
    RuleElementSrv *srvrel=rule->getSrv();
    Service        *srv   =compiler->getFirstSrv(rule);  assert(srv);

    _printAction(rule);
    _printDirection(rule);

    if (rule->getLogging()) 
    {
	compiler->output << " log ";

	if (compiler->getCachedFwOpt()->getBool("ipf_log_or_block") &&
            rule->getAction()==PolicyRule::Accept) 
            compiler->output << " or-block";

	if (compiler->getCachedFwOpt()->getBool("ipf_log_body"))     
            compiler->output << " body";

        string facility=ruleopt->getStr("ipf_log_facility");
        if (facility.empty()) facility = compiler->getCachedFwOpt()->getStr("ipf_log_facility");

        string level=ruleopt->getStr("log_level");
        if (level.empty())    level=compiler->getCachedFwOpt()->getStr("ipf_log_level");
	if (level!="") 
        {
	    compiler->output << " level ";
	    if (facility!="") compiler->output << facility << ".";
	    compiler->output << level;
	}
        compiler->output << " ";
    }

    if ( rule->getBool("quick") ) compiler->output << "quick ";
    _printInterface(rule);

    _printRouteOptions(rule);

    _printProtocol(srv);

    compiler->output << " from ";
    _printSrcAddr(srcrel);
    _printSrcService(srvrel);

    compiler->output << " to ";
    _printDstAddr(dstrel);
    _printDstService(srvrel);

    _printWith(srv);

/* keeping state does not apply to deny/reject */
    if ( ! ruleopt->getBool("stateless") )
    {  
/*
 * this is per advice from Darren Reed  http://false.net/ipfilter/2002_12/0176.html
 *
 * Feature req. #653803: Implement flags for TCP keep state
 *
 * If "keep state" option is given, the rule matches only first packet
 * in the session. To make the rule more secure, we also match on TCP
 * flags (if TCP service is used) looking for the correct session
 * opener packet which should have only SYN flag set and all other
 * flags cleared.
 *
 * However, if option "Accept tcp sessions opened prior to firewall
 * restart" is ON, we do not need to add "flags S" here ( bug #725853 ).
 *
 */
        TCPService *tcpsrv=TCPService::cast(srv);

        if ( ! compiler->getCachedFwOpt()->getBool("accept_new_tcp_with_no_syn") &&
             tcpsrv!=nullptr && !tcpsrv->inspectFlags() )
            compiler->output << "flags S ";


	compiler->output << "keep state ";
    }

/* keep frags option */
    if ( ruleopt->getBool("ipf_keep_frags") && rule->getAction()==PolicyRule::Accept) 
    {  
	compiler->output << "keep frags ";
    }

    compiler->output << endl;

    return true;
}


