/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: PolicyCompiler_ipfw_writers.cpp 1303 2007-05-08 02:11:39Z vkurland $

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

#include "definitions.h"

#include "PolicyCompiler_ipfw.h"
#include "OSData.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/DNSName.h"

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

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;





/**
 *-----------------------------------------------------------------------
 *                    Methods for printing
 */

void PolicyCompiler_ipfw::PrintRule::_printProtocol(Service *srv)
{

    if ( srv->isAny() || srv->getProtocolName()=="ip") 
    {
	compiler->output << "all ";
        return;
    }

    if ( CustomService::isA(srv) )
    {
	string cscode=
            CustomService::cast(srv)->getCodeForPlatform( compiler->myPlatformName() );
/*
 * This is really a hack
 *
 * CustomService object does not imply any specific protocol so
 * generally we can't add protocol name if Custom Service object is
 * used. However, there is one particular case where we have to
 * specify protocol 'tcp' instead of 'all', that is when CustomService
 * object is used to add an option 'established' which is only valid
 * for tcp protocol.
 *
 * Perhaps better solution would be to add optional protocol
 * specification to the CustomService object
 */
        if (cscode=="established")
            compiler->output << "tcp ";
        else
            compiler->output << "all ";
        return;
    }

    compiler->output << srv->getProtocolName();
    compiler->output << " ";
}



string PolicyCompiler_ipfw::PrintRule::_printPort(int rs,int re,bool neg)
{
    ostringstream  str;

    if (rs<0) rs=0;
    if (re<0) re=0;

    if (rs>0 || re>0) 
    {
        if (rs==re)  str << rs;
        else         str << rs << "-" << re;
    }
    return str.str();
}

string PolicyCompiler_ipfw::PrintRule::_printTCPFlags(TCPService *srv)
{
    string str;
    if (srv->getEstablished())  str = "established";
    else
    {
        if (srv->inspectFlags())
        {
            if (srv->getTCPFlagMask(TCPService::FIN))
            {
                if (!srv->getTCPFlag(TCPService::FIN)) str+="!";
                str+="fin";
            } 
            if (srv->getTCPFlagMask(TCPService::SYN))
            {
                str+=",";
                if (!srv->getTCPFlag(TCPService::SYN)) str+="!";
                str+="syn";
            } 
            if (srv->getTCPFlagMask(TCPService::RST))
            {
                str+=",";
                if (!srv->getTCPFlag(TCPService::RST)) str+="!";
                str+="rst";
            } 
            if (srv->getTCPFlagMask(TCPService::PSH))
            {
                str+=",";
                if (!srv->getTCPFlag(TCPService::PSH)) str+="!";
                str+="psh";
            } 
            if (srv->getTCPFlagMask(TCPService::ACK))
            {
                str+=",";
                if (!srv->getTCPFlag(TCPService::ACK)) str+="!";
                str+="ack";
            } 
            if (srv->getTCPFlagMask(TCPService::URG))
            {
                str+=",";
                if (!srv->getTCPFlag(TCPService::URG)) str+="!";
                str+="urg";
            } 
            if (!str.empty())
                return  "tcpflags " + str;
        }
    }
    return str;
}


void PolicyCompiler_ipfw::PrintRule::_printAction(PolicyRule *rule)
{
    FWOptions *ruleopt =rule->getOptionsObject();
    Service *srv=compiler->getFirstSrv(rule);    assert(srv);

    switch (rule->getAction()) {
    case PolicyRule::Skip:
        compiler->output << "skipto " << rule->getInt("skip_to") << " ";
        break;

    case PolicyRule::Accounting:
        compiler->output << "count  ";
        break;

    case PolicyRule::Accept:
        compiler->output << "permit ";
        break;

    case PolicyRule::Deny:
        compiler->output << "drop   ";
        break;

    case PolicyRule::Reject: 
        if (TCPService::isA(srv)) compiler->output << "reset  ";
        else 
        {
            string aor=ruleopt->getStr("action_on_reject");
            if (aor.empty()) aor=compiler->getCachedFwOpt()->getStr("action_on_reject");
            string code;
            if ( aor.find("ICMP")!=string::npos ) 
            {
                code="";
                if (aor.find("unreachable")!=string::npos ) {
                    if (aor.find("net")!=string::npos)      code="net ";
                    if (aor.find("host")!=string::npos)     code="host ";
                    if (aor.find("protocol")!=string::npos) code="protocol ";
                    if (aor.find("port")!=string::npos)     code="port ";
                }
                if (aor.find("prohibited")!=string::npos ) {
                    if (aor.find("net")!=string::npos)      code="net-prohib ";
                    if (aor.find("host")!=string::npos)     code="host-prohib ";
                }
            } else
                code="host-prohib ";

            compiler->output << "unreach " << code;
        }
	break;

    case PolicyRule::Classify:
    {
        int portNum = ruleopt->getInt("ipfw_pipe_queue_num");
        switch (ruleopt->getInt("ipfw_classify_method"))
        {
        case DUMMYNETPIPE:
            compiler->output << "pipe "   << portNum << " ";
            break;
        case DUMMYNETQUEUE:
            compiler->output << "queue "  << portNum << " ";
            break;
        default:
            compiler->output << "divert " << portNum << " ";
            break;
        }
    }
    break;

    case PolicyRule::Pipe:
        compiler->output << "divert " << ruleopt->getInt("ipfw_pipe_port_num") << " ";
        break;

    case PolicyRule::Custom:
        compiler->output << ruleopt->getStr("custom_str") << " ";
        break;

    default:
        compiler->abort(
            string("Unknown action ") + rule->getActionAsString()
            + " in rule " + rule->getLabel()
        );

//   compiler->output << rule->getActionAsString() << " ";
    }
}

/*
 *  this is almost like the one in PolicyCompiler_pf, except it does
 *  not print interface name for dynamic interface ('cause ipfilter
 *  does not support it)
 */
void PolicyCompiler_ipfw::PrintRule::_printAddr(Address  *o,bool neg)
{
    if (o->getId()==compiler->fw->getId())
    {
        compiler->output << "me ";
        return;
    }

    MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
    if (atrt!=NULL)
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
        assert(atrt==NULL);
    }

    IPAddress addr=o->getAddress();
    Netmask   mask=o->getNetmask();

    if (Interface::cast(o)!=NULL)  mask=Netmask("255.255.255.255");
    if (IPv4::cast(o)!=NULL)       mask=Netmask("255.255.255.255");

    if (addr.toString()=="0.0.0.0" && mask.toString()=="0.0.0.0") 
    {
	compiler->output << "any ";
    } else 
    {
	if (neg) compiler->output << "not ";
	compiler->output << addr.toString();
	if (mask.toString()!="255.255.255.255") {
	    compiler->output << "/" << mask.getLength();
	}
	compiler->output << " ";
    }
}

void PolicyCompiler_ipfw::PrintRule::_printDirection(libfwbuilder::PolicyRule *r)
{
    switch (r->getDirection())
    {
    case PolicyRule::Outbound: compiler->output << "out "; break;
    case PolicyRule::Inbound:  compiler->output << "in  "; break;
    case PolicyRule::Both:     compiler->output << "    "; break;
    default: break;
    }
}

void PolicyCompiler_ipfw::PrintRule::_printInterface(PolicyRule *r)
{
    string       iface_id = r->getInterfaceId();
    if (iface_id!="") 
    {
        switch (r->getDirection())
        {
        case PolicyRule::Outbound: compiler->output << "xmit "; break;
        case PolicyRule::Inbound:  compiler->output << "recv "; break;
        case PolicyRule::Both:     compiler->output << "via  "; break;
        default: break;
        }

	Interface *rule_iface = compiler->getCachedFwInterface( iface_id );
	compiler->output << rule_iface->getName() << " ";
    }
}

void PolicyCompiler_ipfw::PrintRule::_printSrcService(RuleElementSrv  *rel)
{
/* I do not want to use rel->getFirst because it traverses the tree to
 * find the object. I'd rather use a cached copy in the compiler
 */
    FWObject *o=rel->front();
    if (o && FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    Service *s1= Service::cast(o);


    bool tcpudp= (UDPService::isA(s1) || TCPService::isA(s1));

    bool first=true;
    for (list<FWObject*>::iterator i1=rel->begin(); i1!=rel->end(); ++i1) 
    {
        FWObject *o   = *i1;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        Service *srv=Service::cast(o);

        if (tcpudp)
        {
            string str=_printSrcService( srv , false );
            if (! str.empty() )
            {
                if (!first) compiler->output << ",";
                compiler->output << str;
            }
        }
    }
}

string PolicyCompiler_ipfw::PrintRule::_printSrcService(Service *srv,bool neg)
{
    string res;

    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int rs=srv->getInt("src_range_start");
	int re=srv->getInt("src_range_end");
	string s1= _printPort(rs,re,neg);
        if (!s1.empty()) res= s1;
    }
    return res;
}

void PolicyCompiler_ipfw::PrintRule::_printDstService(RuleElementSrv  *rel)
{
    FWObject *o=rel->front();
    if (o && FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    Service *s1= Service::cast(o);


    bool tcpudp= (UDPService::isA(s1) || TCPService::isA(s1));
    bool icmp  = ICMPService::isA(s1);
    bool custom= CustomService::isA(s1);

    if (icmp)   compiler->output << "icmptypes ";

    bool first=true;
    for (list<FWObject*>::iterator i1=rel->begin(); i1!=rel->end(); ++i1) 
    {
        FWObject *o   = *i1;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        Service *srv=Service::cast(o);

        if (tcpudp || custom)
        {
            string str=_printDstService( srv , false );
            if (! str.empty() )
            {
                if (!first) compiler->output << ",";
                compiler->output << str;
            }
        }
        if (icmp)
        {
            if (!first) compiler->output << ",";
            compiler->output << srv->getStr("type");
        }
        first=false;
    }

    compiler->output << " ";
/*
 *  TCP services with flags were separated in rule processor separateTCPWithFlags.
 *  We can count on objects like that being a single object in the SRV.
 */
    if (TCPService::isA(s1)) 
    {
        string str=_printTCPFlags(TCPService::cast(s1));
        if (!str.empty()) compiler->output << str << " ";
    }
    if (IPService::isA(s1) && (s1->getBool("fragm") || s1->getBool("short_fragm")) )
        compiler->output << " frag ";
}

string PolicyCompiler_ipfw::PrintRule::_printDstService(Service *srv,bool neg)
{
    string         res;

    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int rs=srv->getInt("dst_range_start");
	int re=srv->getInt("dst_range_end");
        string s1=_printPort(rs,re,neg);;
        if (!s1.empty()) res= s1;
    }

    if (ICMPService::isA(srv) && srv->getInt("type")!=-1) 
    {
	res= "icmptypes " + srv->getStr("type") + " ";
    }

    if (CustomService::isA(srv)) 
    {
	res= CustomService::cast(srv)->getCodeForPlatform( compiler->myPlatformName() ) + " ";
    }

    return res;
}



PolicyCompiler_ipfw::PrintRule::PrintRule(const std::string &name) : PolicyCompiler_pf::PrintRule(name)
{ 
    print_once_on_top=true;
}

bool PolicyCompiler_ipfw::PrintRule::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    FWOptions*  options = compiler->fw->getOptionsObject();
    FWOptions  *ruleopt = rule->getOptionsObject();

    tmp_queue.push_back(rule);

/* need to quote $IPFW because it may contain space, this happens on
 * Mac more often than anywhere else */

    string quote = "\"";

    if (print_once_on_top) 
    {
        compiler->output << quote << "$IPFW" << quote
                         << " set disable 1" << endl;

        /* checking if option add_check_state_rule is absent to
         * provide for backward compatibility: before 2.1.6 build 131
         * this option did not exist and compiler alawys generated
         * check-state rule
         */
        if (options->getStr("add_check_state_rule").empty() ||
            options->getBool("add_check_state_rule"))
            compiler->output << quote << "$IPFW" << quote
                             << " add 1 set 1 check-state ip from any to any"  << endl;

        compiler->output << endl;

        print_once_on_top=false;
    }


    string rl=rule->getLabel();
    if (rl!=current_rule_label) 
    {
        compiler->output << "# " << endl;
        compiler->output << "# Rule " << rl << endl;

        string    comm=rule->getComment();
        string::size_type c1,c2;
        c1=0;
        while ( (c2=comm.find('\n',c1))!=string::npos ) 
        {
            compiler->output << "# " << comm.substr(c1,c2-c1) << endl;
            c1=c2+1;
        }
        compiler->output << "# " << comm.substr(c1) << endl;
        compiler->output << "# " << endl;

        current_rule_label=rl;
    }


    RuleElementSrc *srcrel=rule->getSrc();
    Address        *src   =compiler->getFirstSrc(rule);  assert(src);
    RuleElementDst *dstrel=rule->getDst();
    Address        *dst   =compiler->getFirstDst(rule);  assert(dst);
    RuleElementSrv *srvrel=rule->getSrv();
    Service        *srv   =compiler->getFirstSrv(rule);  assert(srv);

    if (rule->getBool("needs_established"))
    {
// ipfw_num is assigned with a step of 10, so it is safe to substract 1 here
        compiler->output <<  quote << "$IPFW" << quote
                         << " add " << rule->getInt("ipfw_num")-1 << " set 1 ";

        _printAction(rule);

        if (rule->getLogging())       compiler->output << " log ";

        _printProtocol(srv);

        compiler->output << " from ";
        _printSrcAddr(srcrel);
        _printSrcService(srvrel);

        compiler->output << " to ";
        _printDstAddr(dstrel);
        _printDstService(srvrel);

        _printDirection(rule);
        _printInterface(rule);

	compiler->output << "established ";

        compiler->output << endl;
    }

    compiler->output <<  quote << "$IPFW" << quote
                     << " add " << rule->getInt("ipfw_num") << " set 1 ";

    _printAction(rule);

    if (rule->getLogging())       compiler->output << " log ";

    _printProtocol(srv);

    compiler->output << " from ";
    _printSrcAddr(srcrel);
    _printSrcService(srvrel);

    compiler->output << " to ";
    _printDstAddr(dstrel);
    _printDstService(srvrel);

    _printDirection(rule);
    _printInterface(rule);

/* keeping state does not apply to deny/reject */
    if ( ! ruleopt->getBool("stateless"))
    {  
/*
 * this is per advice from Darren Reed  http://false.net/ipfilter/2002_12/0176.html
 *
 * Feature req. #653803: Implement flags for TCP keep state
 *
 * In ipfw the equivalend is 
 *
 *  setup   TCP packets only.  Match packets that have the SYN bit
 *          set but no ACK bit.
 *
 */
        TCPService *tcpsrv=TCPService::cast(srv);
        if ( tcpsrv!=NULL &&
             !tcpsrv->inspectFlags() &&
             !tcpsrv->getEstablished()  ) compiler->output << "setup ";

	compiler->output << "keep-state ";
    }

    compiler->output << " || exit 1" << endl;
    compiler->output << endl;

    return true;
}


