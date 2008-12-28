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

#include "PolicyCompiler_pf.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"

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
void PolicyCompiler_pf::PrintRule::_printAction(PolicyRule *rule)
{
    FWOptions *ruleopt =rule->getOptionsObject();
    Service *srv=compiler->getFirstSrv(rule);    assert(srv);

    switch (rule->getAction()) {
    case PolicyRule::Accept:  
    case PolicyRule::Tag:
    case PolicyRule::Classify:
    case PolicyRule::Accounting:
    case PolicyRule::Route:
        compiler->output << "pass ";
        break;

    case PolicyRule::Deny:
        compiler->output << "block ";
        break;

    case PolicyRule::Reject: 
	if (TCPService::isA(srv)) compiler->output << "block return-rst ";
	else {
	    string aor=ruleopt->getStr("action_on_reject");
	    string code;
	    if ( aor.find("ICMP")!=string::npos ) {
		code="return-icmp ";
		if (aor.find("unreachable")!=string::npos ) {
 		    if (aor.find("net")!=string::npos)      code=code+"( 0 ) ";
		    if (aor.find("host")!=string::npos)     code=code+"( 1 ) ";
		    if (aor.find("protocol")!=string::npos) code=code+"( 2 ) ";
		    if (aor.find("port")!=string::npos)     code=code+"( 3 ) ";
		}
		if (aor.find("prohibited")!=string::npos ) {
 		    if (aor.find("net")!=string::npos)      code=code+"( 9 ) ";
		    if (aor.find("host")!=string::npos)     code=code+"( 10 ) ";
		}
	    } else
		code="return-icmp   ";

	    compiler->output << "block " << code;
	}
	break;
    case PolicyRule::Scrub:
        compiler->output << "scrub   ";
        break;
    case PolicyRule::Custom:
        compiler->output << ruleopt->getStr("custom_str") << " ";
        break;
    case PolicyRule::Branch:
    {
        RuleSet *ruleset = rule->getBranch();
        if (ruleset==NULL)
            compiler->abort(string("Branching rule ") + rule->getLabel() +
                            " refers ruleset that does not exist");
        compiler->output << "anchor " << ruleset->getName() << " ";
        break;
    }
    default:
        compiler->abort(
            string("Unknown action '") + rule->getActionAsString()
            + "' in rule " + rule->getLabel()
        );
//        compiler->output << rule->getActionAsString() << " ";
    }
}

void PolicyCompiler_pf::PrintRule::_printRouteOptions(PolicyRule *rule)
{
    FWOptions *ruleopt =rule->getOptionsObject();

    if (rule->getAction() == PolicyRule::Route)
    {
	string prefix = "pf";
	if (compiler->myPlatformName()=="ipf")
		prefix="ipf";
	string ro = ruleopt->getStr(prefix+"_route_option");
        if (ruleopt->getBool("pf_fastroute") && ro != "none")
	{
            compiler->abort("Cannot use fastroute and route method in same rule they are mutually exclusive in rule "+rule->getLabel());
	} else if (ruleopt->getBool("pf_fastroute") && ro == "none" ) {
            compiler->output << "fastroute ";
	} else {
            string roif = ruleopt->getStr(prefix+"_route_opt_if");
            string roaddr_list = ruleopt->getStr(prefix+"_route_opt_addr");
            string roload = ruleopt->getStr("pf_route_load_option");
            if (!ro.empty())
            {
                if (roif.empty())
                    compiler->abort("Interface specification is required for action Route in rule "+rule->getLabel());

                if (ro == "route_through")
                    compiler->output << "route-to ";
                else if (ro == "route_reply_through")
                    compiler->output << "reply-to ";
                else if (ro == "route_copy_through")
                    compiler->output << "dup-to ";
                else
                    compiler->abort("Unknown option for rule action Route: '" + 
                                    ro + "' in rule "+rule->getLabel());
            		
                compiler->output << "{ ";

                int route_member = 0;
	    
                std::istringstream buf(roaddr_list);
                string roaddr;
                while (std::getline(buf, roaddr, ','))
                {
                    if (!roaddr.empty())
                    {
                        if (route_member > 0 )
                        {
                            compiler->output << ", ";
                        }
                        compiler->output << "( ";
                        compiler->output << roif << " ";
                        compiler->output << roaddr << " ";
                        compiler->output << ") ";
                        std::string::size_type sp = roaddr.find('/');
                        if (sp!=std::string::npos) 
                        {
                            // roaddr is addr/netmask
                            try 
                            {
                                string a = roaddr.substr(0,sp);
                                InetAddr roaddr_addr = InetAddr(a);
                            } catch (FWException &ex)
                            {
                                compiler->abort(
        "Illegal IP address for next hop in rule "+rule->getLabel());
                            }
                            try
                            {
                                InetAddr roaddr_netmask;
                                string n = roaddr.substr(sp+1);
                                if (n.find('.')!=std::string::npos)
                                {
                                    roaddr_netmask = InetAddr(n);
                                } else
                                {
                                    roaddr_netmask = InetAddr(
                                        atoi(n.c_str()));
                                }
                                if (roaddr_netmask.getLength()==32)
                                    route_member++;
                                else
                                    // lame way to tell compiler that
                                    // we actually have several addresses for
                                    // the next hop. We do not exactly care
                                    // how many there are, as long as it is
                                    // greater than 1.
                                    route_member += 2;
                            } catch (FWException &ex)
                            {
                                compiler->abort(
        "Illegal netmask for next hop in rule "+rule->getLabel());
                            }
                        } else
                        {
                            // roaddr is just an addres
                            try 
                            {
                                InetAddr roaddr_addr = InetAddr(roaddr);
                            } catch (FWException &ex)
                            {
                                compiler->abort(
        "Illegal IP address for next hop in rule "+rule->getLabel());
                            }
                            route_member++;
                        }
                    }
                }
                if (route_member < 1)
                {
                    compiler->abort("No router specified rule action Route: '"+ 
                                    ro + "' in rule "+rule->getLabel());
                }
                if (route_member >= 2 && (roload.empty() || roload == "none"))
                {
                    compiler->abort("More than one router specified without load balancing for rule action Route: '" + 
                                    ro + "' in rule "+rule->getLabel());
                }
                if (route_member == 1 && ((!roload.empty()) && roload != "none"))
                {
                    compiler->abort("Only one router specified with load balancing for rule action Route: '" + 
                                    ro + "' in rule "+rule->getLabel());
                }

                compiler->output << "} ";
			
                if (!roload.empty())
                {
                    if (roload == "bitmask")
                        compiler->output << "bitmask ";
                    else if (roload == "random")
                        compiler->output << "random ";
                    else if (roload == "source_hash")
                        compiler->output << "source-hash ";
                    else if (roload == "round_robin")
                        compiler->output << "round-robin ";
                }
            }
	}
    }
}

void PolicyCompiler_pf::PrintRule::_printQueue(PolicyRule *rule)
{
    FWOptions *ruleopt =rule->getOptionsObject();

    if (rule->getAction() == PolicyRule::Classify)
        compiler->output << "queue " << ruleopt->getStr("classify_str") << " ";
}

void PolicyCompiler_pf::PrintRule::_printUser(PolicyRule *rule)
{
    RuleElementSrv *srvrel = rule->getSrv();
    FWObject *o = srvrel->front();
    if (o && FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
    Service *srv= Service::cast(o);
    if (!UserService::isA(srv)) return;
    ostringstream str;

    if (srvrel->size()==1) 
    {
        str << "user ";
        if (srvrel->getNeg()) str << "!= ";
        str << UserService::constcast(srv)->getUserId() << " ";
        compiler->output << str.str() << " ";
    } else 
    {
        int counter = 0;
	for (FWObject::iterator i=srvrel->begin(); i!=srvrel->end(); i++) 
        {
	    FWObject *o= *i;
	    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	    Service *s=Service::cast( o );
	    assert(s);
            if (counter > 0) str << ",";
            str << " ";
            if (srvrel->getNeg()) str << "!= ";
            str << UserService::constcast(s)->getUserId();
            counter++;
	}
	if ( counter )
        {
            compiler->output << "user {" << str.str() << " } ";
	}
    }
}

void PolicyCompiler_pf::PrintRule::_printTag(PolicyRule *rule)
{
    if (rule->getAction() == PolicyRule::Tag)
        compiler->output << "tag " << rule->getTagValue() << " ";
//        compiler->output << "tag " << ruleopt->getStr("tagvalue") << " ";
}

void PolicyCompiler_pf::PrintRule::_printDirection(PolicyRule *rule)
{
    if (rule->getDirection()==PolicyRule::Outbound)  compiler->output << "out "; 
    else	                                     compiler->output << "in  "; 
}

void PolicyCompiler_pf::PrintRule::_printLogging(PolicyRule *rule)
{
    if (rule->getAction() != PolicyRule::Branch && 
        rule->getLogging()) compiler->output << " log ";
}

void PolicyCompiler_pf::PrintRule::_printLabel(PolicyRule *rule)
{
    FWOptions *ruleopt =rule->getOptionsObject();
    string s=ruleopt->getStr("log_prefix");
    if (s.empty())  s=compiler->getCachedFwOpt()->getStr("log_prefix");
    if (!s.empty())
        compiler->output << " label " << _printLogPrefix(rule,s) << " ";
}

string PolicyCompiler_pf::PrintRule::_printLogPrefix(PolicyRule *rule,
                                                     const string &prefix)
{
    string s=prefix;

/* deal with our logging macros:
 * %N - rule number
 * %A - action
 * %I - interface name
 * %C - chain name
 */
    string::size_type n;
    if (rule && (n=s.find("%N"))!=string::npos ) {
        std::ostringstream s1;
        s1 << rule->getPosition();
        s.replace(n,2,s1.str());
    }
    if (rule && (n=s.find("%A"))!=string::npos ) {
        std::ostringstream s1;
        switch (rule->getAction()) {
        case PolicyRule::Accept:  s1 << "ACCEPT"; break;
        case PolicyRule::Deny:    s1 << "DROP";   break;
        case PolicyRule::Reject:  s1 << "REJECT"; break;
        case PolicyRule::Return:  s1 << "RETURN"; break;
        default: break;
        }
        s.replace(n,2,s1.str());
    }
    if (rule && (n=s.find("%I"))!=string::npos ) {
        std::ostringstream s1;
        string rule_iface =  rule->getInterfaceStr();
        if (rule_iface!="") 
        {
            s1 << rule_iface;
            s.replace(n,2,s1.str());
        } else
            s.replace(n,2,"global");
    }
    if (rule && (n=s.find("%C"))!=string::npos ) {
        s.replace(n,2,"");  // there is no chain in PF and friends
    }

    return "\"" + s + "\" ";
}


void PolicyCompiler_pf::PrintRule::_printInterface(PolicyRule *rule)
{
    string       iface_name = rule->getInterfaceStr();
    if (iface_name!="") 
	compiler->output << "on " << iface_name << " ";
}

// print address family
void PolicyCompiler_pf::PrintRule::_printAF(PolicyRule*)
{
    PolicyCompiler_pf *pf_comp=dynamic_cast<PolicyCompiler_pf*>(compiler);
    if (pf_comp->ipv6) compiler->output << "inet6 ";
    else compiler->output << "inet ";
}

void PolicyCompiler_pf::PrintRule::_printProtocol(Service *srv)
{
    // CustomService returns protocol name starting with v3.0.4
    // However CustomService can return protocol name "any", which we should
    // just skip.

    // CustomService returns protocol name starting with v3.0.4
    if (CustomService::isA(srv))
    {
        // check if the code string for this custom service already includes
        // "proto ..." fragment
        string code = CustomService::cast(srv)->getCodeForPlatform(
            compiler->myPlatformName());
        std::size_t minus_p = code.find("proto ");
        if (minus_p != string::npos) return;
        string pn = srv->getProtocolName();
        if (pn == "any") return;
    }

    if (!srv->isAny() && !TagService::isA(srv) && !UserService::isA(srv) && 
        srv->getProtocolName()!="ip")
    {
	compiler->output << "proto ";
	compiler->output << srv->getProtocolName();
	compiler->output << " ";
    }
}

string PolicyCompiler_pf::PrintRule::_printPort(int rs,int re,bool neg)
{
    ostringstream  str;

    if (rs<0) rs=0;
    if (re<0) re=0;

    if (!neg) {

	if (rs>0 || re>0)
        {
            if (rs>re && re==0) re=rs;

	    if (rs==re)  str << rs;   // TODO: do we need '=' here ?
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

/*
 * we made sure that all services in rel  represent the same protocol. 
 */
void PolicyCompiler_pf::PrintRule::_printSrcService(RuleElementSrv  *rel)
{
/* I do not want to use rel->getFirst because it traverses the tree to
 * find the object. I'd rather use a cached copy in the compiler
 */
    FWObject *o=rel->front();
    if (o && FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    Service *srv= Service::cast(o);
    string prefix = "";
    if (UDPService::isA(srv) || TCPService::isA(srv)) prefix = "port ";

    if (rel->size()==1)
    {
        if (UDPService::isA(srv) || TCPService::isA(srv))
        {
	    string str=_printSrcService( srv , rel->getNeg());
            if (! str.empty() ) compiler->output << prefix << str << " ";
        }
    } else
    {
	string str;
	for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++)
        {
	    FWObject *o= *i;
	    if (FWReference::cast(o)!=NULL)
                o=FWReference::cast(o)->getPointer();
	    Service *s = Service::cast( o );
	    assert(s);
            string str1;

	    if (UDPService::isA(srv) || TCPService::isA(srv))
                str1 = _printSrcService(s , rel->getNeg() );

            if (! str.empty() && ! str1.empty() )  str = str + ", ";
            str = str + str1;
	}
	if ( !str.empty() )
	    compiler->output << prefix << "{ " << str << "} ";
    }
}

string PolicyCompiler_pf::PrintRule::_printSrcService(Service *srv, bool neg)
{
    ostringstream  str;
    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int rs=TCPUDPService::cast(srv)->getSrcRangeStart();
	int re=TCPUDPService::cast(srv)->getSrcRangeEnd();
	str << _printPort(rs,re,neg);
    }
    return str.str();
}

void PolicyCompiler_pf::PrintRule::_printDstService(RuleElementSrv  *rel)
{
    FWObject *o=rel->front();
    if (o && FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
    Service *srv= Service::cast(o);

    if (rel->size()==1) 
    {
	string str=_printDstService( srv , rel->getNeg());
	if ( ! str.empty() ) 
        {
	    if (UDPService::isA(srv) || TCPService::isA(srv)) 
		compiler->output << "port " << str << " ";
            else
            {
                if (ICMPService::isA(srv)) 
                    compiler->output << "icmp-type " << str << " ";
                else
                    if (ICMP6Service::isA(srv)) 
                        compiler->output << "icmp6-type " << str << " ";
                    else
                        compiler->output << str << " ";
            }
	}
	if (TCPService::isA(srv)) 
        {
	    str=_printTCPFlags(TCPService::cast(srv));
	    if (!str.empty()) compiler->output << "flags " << str << " ";
	}
        if (IPService::isA(srv))
        {
            if (srv->getBool("fragm") || srv->getBool("short_fragm"))
                compiler->output << " fragment ";
            const IPService *ip = IPService::constcast(srv);
            string tos = ip->getTOSCode();
            string dscp = ip->getDSCPCode();
            if (!tos.empty()) compiler->output << " tos " << tos;
            if (!dscp.empty())
                compiler->abort("PF does not support DSCP matching");
        }
    } else 
    {
	string str;
	for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
        {
	    FWObject *o= *i;
	    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	    Service *s=Service::cast( o );
	    assert(s);
	    string str1= _printDstService(s , rel->getNeg() );
	    if (! str.empty() && ! str1.empty() )  str = str + ", ";
	    str = str + str1;
	}
	if ( !str.empty() ) 
        {
	    if (UDPService::isA(srv) || TCPService::isA(srv)) 
		compiler->output << "port { " << str << " } ";
            else
            {
                if (ICMPService::isA(srv)) 
                    compiler->output << "icmp-type { " << str << " } ";
                else
                {
                    if (ICMP6Service::isA(srv)) 
                        compiler->output << "icmp6-type { " << str << " } ";
                    else
                        compiler->output << str << " " << endl;
                }
            }
	}
    }
}

string PolicyCompiler_pf::PrintRule::_printDstService(Service *srv, bool neg)
{
    ostringstream  str;
    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int rs=TCPUDPService::cast(srv)->getDstRangeStart();
	int re=TCPUDPService::cast(srv)->getDstRangeEnd();
	str << _printPort(rs,re,neg);
    }

    if (ICMPService::isA(srv) && srv->getInt("type")!=-1) 
    {
	str << srv->getStr("type") << " ";
	if (srv->getInt("code")!=-1) 
	    str << "code " << srv->getStr("code") << " ";
    }

    if (CustomService::isA(srv)) 
    {
	str << CustomService::cast(srv)->getCodeForPlatform(
            compiler->myPlatformName() ) << " ";
    }

    if (TagService::isA(srv)) 
    {
	str << "tagged " << TagService::constcast(srv)->getCode() << " ";
    }

    return str.str();
}

string PolicyCompiler_pf::PrintRule::_printTCPFlags(libfwbuilder::TCPService *srv)
{
    string str;
    if (srv->inspectFlags())
    {
        if (srv->getTCPFlag(TCPService::URG)) str+="U";
        if (srv->getTCPFlag(TCPService::ACK)) str+="A";
        if (srv->getTCPFlag(TCPService::PSH)) str+="P";
        if (srv->getTCPFlag(TCPService::RST)) str+="R";
        if (srv->getTCPFlag(TCPService::SYN)) str+="S";
        if (srv->getTCPFlag(TCPService::FIN)) str+="F";
        str+="/";
        if (srv->getTCPFlagMask(TCPService::URG)) str+="U";
        if (srv->getTCPFlagMask(TCPService::ACK)) str+="A";
        if (srv->getTCPFlagMask(TCPService::PSH)) str+="P";
        if (srv->getTCPFlagMask(TCPService::RST)) str+="R";
        if (srv->getTCPFlagMask(TCPService::SYN)) str+="S";
        if (srv->getTCPFlagMask(TCPService::FIN)) str+="F";
    }
    return str;
}

void PolicyCompiler_pf::PrintRule::_printAddr(Address  *o,bool )
{
    MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
    if (atrt!=NULL)
    {
        if (atrt->getSubstitutionTypeName()==DNSName::TYPENAME)
        {
            compiler->output <<  atrt->getSourceName() << " ";
            return;
        }
        if (atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
        {
            compiler->output << "<" << o->getName() << "> ";
            return;
        }
        assert(atrt==NULL);
    }

    const InetAddr *addr = o->getAddressPtr();
    InetAddr mask = *(o->getNetmaskPtr());

    if (Interface::cast(o)!=NULL)
    {
	Interface *interface_=Interface::cast(o);
	if (interface_->isDyn()) 
        {
	    compiler->output << "(" << interface_->getName() << ") ";
	    return;
	}

	mask = InetAddr(InetAddr::getAllOnes());
    }

    if (o->dimension()==1) 
    {
	mask = InetAddr(InetAddr::getAllOnes());
    }

    if (addr->isAny() && mask.isAny()) 
    {
	compiler->output << "any ";
    } else 
    {
//	if (neg) compiler->output << "! ";
	compiler->output << addr->toString();
	if (!mask.isHostMask())
        {
	    compiler->output << "/" << mask.getLength();
	}
	compiler->output << " ";
    }
}

void PolicyCompiler_pf::PrintRule::_printAddrList(FWObject  *grp,bool negflag)
{
    compiler->output << "{ ";
    for (FWObject::iterator i=grp->begin(); i!=grp->end(); i++)
    {
        if (i!=grp->begin())  compiler->output << ", ";
        FWObject *o= *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        Address *s=Address::cast( o );
        assert(s);
        _printAddr(s , negflag);
    }
    compiler->output << "} ";
}

void PolicyCompiler_pf::PrintRule::_printSrcAddr(RuleElementSrc  *rel)
{
    FWObject *o=rel->front();
    FWReference *oref = FWReference::cast(o);
    if (o && oref!=NULL) o=oref->getPointer();

    Address *src= Address::cast(o);

    _printNegation(rel);

    if (o==NULL)
    {
        PolicyRule *rule = PolicyRule::cast(rel->getParent());
        ostringstream errstr;
        errstr << "Broken rule element " 
               << rel->getTypeName()
               << " in rule '"
               << rule->getLabel()
               << "'  rel->front(): "
               << oref->getPointerId();
        compiler->abort(errstr.str());
    }

    if (rel->size()==1 && ! o->getBool("pf_table") )
    {
	_printAddr( src , rel->getNeg() );
    } else
    {
        if (o->getBool("pf_table"))
        {
            compiler->output << "<" << o->getName() << "> ";
        } else
        {
            _printAddrList(rel,rel->getNeg());
        }
    }

}

void PolicyCompiler_pf::PrintRule::_printDstAddr(RuleElementDst  *rel)
{
    FWObject *o=rel->front();
    FWReference *oref = FWReference::cast(o);
    if (o && oref!=NULL) o=oref->getPointer();

    Address *dst= Address::cast(o);

    _printNegation(rel);

    if (o==NULL)
    {
        PolicyRule *rule = PolicyRule::cast(rel->getParent());
        ostringstream errstr;
        errstr << "Broken rule element "
               << rel->getTypeName()
               << " in rule '"
               << rule->getLabel()
               <<  "'  rel->front(): "
               << oref->getPointerId();
        compiler->abort(errstr.str());
    }

    if (rel->size()==1 && ! o->getBool("pf_table") )
    {
	_printAddr( dst , rel->getNeg());
    } else
    {
        if (o->getBool("pf_table"))
        {
            compiler->output << "<" << o->getName() << "> ";
        } else
        {
            _printAddrList(rel,rel->getNeg());
        }
    }
}

void PolicyCompiler_pf::PrintRule::_printNegation(libfwbuilder::RuleElement  *rel)
{
    if (rel->getNeg())
	compiler->output << "! ";
}


PolicyCompiler_pf::PrintRule::PrintRule(const std::string &name) : PolicyRuleProcessor(name) 
{ 
    init=true; 
}

bool PolicyCompiler_pf::PrintRule::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    FWOptions  *ruleopt =rule->getOptionsObject();
    string version=compiler->fw->getStr("version");

//    cerr << endl;
//    cerr << "Rule " << rule->getPosition() << endl;

    tmp_queue.push_back(rule);

    string rl=rule->getLabel();
    if (rl!=current_rule_label)
    {
        
        compiler->output << "# " << endl;
        compiler->output << "# Rule  " << rl << endl;

        string    comm=rule->getComment();
        string::size_type c1,c2;
        c1=0;
        while ( (c2=comm.find('\n',c1))!=string::npos ) {
            compiler->output << "# " << comm.substr(c1,c2-c1) << endl;
            c1=c2+1;
        }
        compiler->output << "# " << comm.substr(c1) << endl;
        compiler->output << "# " << endl;

        current_rule_label=rl;
    }

//    cerr << "CP 1" << endl;



    RuleElementSrc *srcrel=rule->getSrc();
//    Address        *src   =compiler->getFirstSrc(rule);  assert(src);
    RuleElementDst *dstrel=rule->getDst();
//    Address        *dst   =compiler->getFirstDst(rule);  assert(dst);
    RuleElementSrv *srvrel=rule->getSrv();
    Service        *srv   =compiler->getFirstSrv(rule);  assert(srv);

    _printAction(rule);
    _printDirection(rule);
    _printLogging(rule);

    if ( rule->getBool("quick") ) compiler->output << " quick ";

    _printInterface(rule);

    _printRouteOptions(rule);

    _printAF(rule);

    _printProtocol(srv);

//    cerr << "CP 2" << endl;

    compiler->output << " from ";
    _printSrcAddr(srcrel);
    _printSrcService(srvrel);

    compiler->output << " to ";
    _printDstAddr(dstrel);
    _printDstService(srvrel);

    _printTag(rule);
    _printUser(rule);

/* 
 * Dealing with "keep state" and "modulate state" flags
 *
 * 1. both flags do not apply to deny/reject rules.
 * 2. modulate state applies only to TCP services. Since we use splitServices,
 *    all services in a rule are of the same protocol, therefore we can simply
 *    check type of srv
 */
    if ( ! ruleopt->getBool("stateless") )
    {

        TCPService *tcpsrv=TCPService::cast(srv);

        if (tcpsrv!=NULL && ! tcpsrv->inspectFlags() )
        {
            // tcp service, no special flag match

            if ( version == "4.x")
            {
                if (compiler->getCachedFwOpt()->getBool(
                        "accept_new_tcp_with_no_syn") )
                    // v4.x, accept connections opened prior to restart
                    compiler->output << "flags any ";
                // else - no 'flags' option since in 4.x
                // 'flags S/SA' is the default
                if (ruleopt->getBool("pf_keep_state") )
                    compiler->output << "keep state ";
            } else
            {
                // v3.x
                if ( compiler->getCachedFwOpt()->getBool(
                         "accept_new_tcp_with_no_syn") )
                {
                    // no 'flags ' option needed
                    ;
                } else
                    // v3.x, stateful
                    compiler->output << "flags S/SA ";
            }
        }

        // in PF "modulate state", "synproxy state", "keep state" are mutually
        // exclusive
        // "keep state" can be used with any protocol, while "modulate state"
        // and "synproxy state" can only be used with tcp.

	if (compiler->getCachedFwOpt()->getBool("pf_synproxy") &&
            tcpsrv!=NULL)
	    compiler->output << "synproxy state ";
        else
        {
            if (compiler->getCachedFwOpt()->getBool("pf_modulate_state") &&
                tcpsrv!=NULL)
                compiler->output << "modulate state ";
            else
            {
                /*
                 * "flags S/SA keep state" is implicit in 4.x
                 * However see section "1.2. Operational changes" in
                 *  http://www.openbsd.org/faq/upgrade41.html
                 *
                 * Quote:
                 *
                 * In particular care should be taken with the enc0
                 * interface, as floating states are a potential problem
                 * for filtering IPsec traffic: states need to be
                 * interface bound, to avoid permitting unencrypted
                 * traffic should isakmpd(8) exit. Therefore all rules on
                 * the enc0 interface should explicitly set keep state
                 * (if-bound).
                 *
                 * This seems to imply that even though "keep state" is
                 * the default, it should be explicitly used with enc0
                 * interface. Adding rule option "Set 'keep state'
                 * explicitly" to cope with this.
                 */
                if ( version != "4.x" ||
                     compiler->getCachedFwOpt()->getBool("pf_keep_state"))
                    compiler->output << "keep state ";
            }
        }

        int nopt=0;
        if (ruleopt->getInt("pf_rule_max_state")>0)        nopt++;
        if (ruleopt->getBool("pf_source_tracking"))        nopt+=2;
        if (ruleopt->getInt("pf_max_src_conn")>0)          nopt++;
        if (ruleopt->getStr("pf_max_src_conn_overload_table")!="") nopt++;
        if (ruleopt->getInt("pf_max_src_conn_rate_num")>0) nopt++;

        bool not_the_first = false;
        if (nopt)
        {
            if (nopt>1) compiler->output << " ( ";

            if (ruleopt->getInt("pf_rule_max_state")>0)
            {
                compiler->output << " max "
                                 << ruleopt->getInt("pf_rule_max_state");
                not_the_first = true;
            }

            if (ruleopt->getBool("pf_source_tracking"))
            {
                if (not_the_first) compiler->output << ",";

                if (ruleopt->getInt("pf_max_src_nodes") > 0)
                    compiler->output << " max-src-nodes "
                                     << ruleopt->getInt("pf_max_src_nodes");

                if (ruleopt->getInt("pf_max_src_states")>0)
                    compiler->output << ", max-src-states "
                                     << ruleopt->getInt("pf_max_src_states");

                not_the_first = true;
            }

            bool check_overload_opts = false;
            if (ruleopt->getInt("pf_max_src_conn")>0)
            {
                if (not_the_first) compiler->output << ",";

                compiler->output << " max-src-conn "
                                 << ruleopt->getInt("pf_max_src_conn");
                not_the_first = true;
                check_overload_opts = true;
            }

            if (ruleopt->getInt("pf_max_src_conn_rate_num")>0 && 
                ruleopt->getInt("pf_max_src_conn_rate_seconds")>0)
            {
                if (not_the_first) compiler->output << ",";

                compiler->output << " max-src-conn-rate "
                                 << ruleopt->getInt("pf_max_src_conn_rate_num")
                                 << "/"
                                 << ruleopt->getInt(
                                     "pf_max_src_conn_rate_seconds");
                check_overload_opts = true;
            }
            if (check_overload_opts)
            {
                if (ruleopt->getStr("pf_max_src_conn_overload_table")!="")
                    compiler->output << ", overload <"
                                     << ruleopt->getStr(
                                         "pf_max_src_conn_overload_table")
                                     << ">";
                if (ruleopt->getBool("pf_max_src_conn_flush"))
                    compiler->output << " flush";
                if (ruleopt->getBool("pf_max_src_conn_global"))
                    compiler->output << " global";
            }
            if (nopt>1) compiler->output << " ) ";
        }
    } else
    {
        // stateless rule
        if ( version == "4.x")
            // v4.x, stateless rule
            compiler->output << "no state ";
    }

    if (rule->getBool("allow_opts")) compiler->output << "allow-opts  ";

    _printQueue(rule);
    _printLabel(rule);

    compiler->output << endl;

    return true;
}

bool PolicyCompiler_pf::PrintTables::processNext()
{
    PolicyCompiler_pf *pf_comp=dynamic_cast<PolicyCompiler_pf*>(compiler);

    slurp();
    if (tmp_queue.size()==0) return false;

/* print tables */
    compiler->output << pf_comp->tables->PrintTables();

    return true;
}
