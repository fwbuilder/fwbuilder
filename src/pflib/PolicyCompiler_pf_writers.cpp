/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/AttachedNetworks.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <assert.h>

#include <QStringList>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;





/**
 *-----------------------------------------------------------------------
 *                    Methods for printing
 */
void PolicyCompiler_pf::PrintRule::_printAction(PolicyRule *rule)
{
    FWOptions *ruleopt = rule->getOptionsObject();
#ifndef NDEBUG
    Service *srv = compiler->getFirstSrv(rule);    assert(srv);
#endif
    string version = compiler->fw->getStr("version");

    switch (rule->getAction())
    {
 // case PolicyRule::Classify:  #2367
 // case PolicyRule::Route:     #2367
    case PolicyRule::Accept:  
    case PolicyRule::Accounting:
        compiler->output << "pass ";
        break;

    case PolicyRule::Deny:
        compiler->output << "block ";
        break;

    case PolicyRule::Continue:
        if (XMLTools::version_compare(version, "4.6")>=0)
        {
            compiler->output << "match ";
        }else
        {
            compiler->output << "pass ";
        }
        break;

    case PolicyRule::Reject: 
    {
        string aor = ruleopt->getStr("action_on_reject");
        string code;
        if ( aor.find("ICMP")!=string::npos )
        {
            code = "return-icmp ";
            if (aor.find("unreachable")!=string::npos )
            {
                if (aor.find("net")!=string::npos)      code = code + "( 0 ) ";
                if (aor.find("host")!=string::npos)     code = code + "( 1 ) ";
                if (aor.find("protocol")!=string::npos) code = code + "( 2 ) ";
                if (aor.find("port")!=string::npos)     code = code + "( 3 ) ";
            }
            if (aor.find("prohibited")!=string::npos )
            {
                if (aor.find("net")!=string::npos)      code = code + "( 9 ) ";
                if (aor.find("host")!=string::npos)     code = code + "( 10 ) ";
                if (aor.find("filter")!=string::npos)   code = code + "( 13 ) ";
                if (aor.find("admin")!=string::npos)    code = code + "( 13 ) ";
            }
        } else
        {
            if ( aor.find("TCP")!=string::npos ) code = "return-rst   ";
            else
                code = "return-icmp   ";
        }

        compiler->output << "block " << code;
	break;
    }

    case PolicyRule::Scrub:
    {
        string version = compiler->fw->getStr("version");
        if (XMLTools::version_compare(version, "4.7")>=0)
        {
            compiler->output << "match in all scrub   ";
        } else
        {
            compiler->output << "scrub   ";
        }
        break;
    }

    case PolicyRule::Custom:
        compiler->output << ruleopt->getStr("custom_str") << " ";
        break;

    case PolicyRule::Branch:
    {
        RuleSet *ruleset = rule->getBranch();
        if (ruleset==nullptr)
        {
            compiler->abort(
                    rule, 
                    "Branching rule refers ruleset that does not exist");
            // if we are in test mode or single-rule compile mode
            compiler->output << "anchor \"UNDEFINED\" ";
        }else
        {
            compiler->output << "anchor \"" << ruleset->getName() << "\" ";
        }
        break;
    }

    default:
        compiler->abort(
                rule, string("Unknown action ") + rule->getActionAsString());
    }

    // #2367
    //
    // if (rule->getTagging())
    // {
    //     if (XMLTools::version_compare(version, "4.6")>=0)
    //     {
    //         compiler->output << "match ";
    //     }else
    //     {
    //         compiler->output << "pass ";
    //     }
    //     break;
    // }
}

void PolicyCompiler_pf::PrintRule::_printRouteOptions(PolicyRule *rule)
{
    FWOptions *ruleopt =rule->getOptionsObject();

    if (rule->getRouting())
    {
	string prefix = "pf";
	if (compiler->myPlatformName()=="ipf") prefix = "ipf";

	string ro = ruleopt->getStr(prefix+"_route_option");
        if (ruleopt->getBool("pf_fastroute") && ro != "none")
	{
            compiler->abort(
                rule, 
                "Cannot use fastroute and route methods in "
                "the same rule because they are mutually "
                "exclusive.");
	} else if (ruleopt->getBool("pf_fastroute") && ro == "none")
        {
            compiler->output << "fastroute ";
	} else
        {
            string roif = ruleopt->getStr(prefix+"_route_opt_if");
            string roaddr_list = ruleopt->getStr(prefix+"_route_opt_addr");
            string roload = ruleopt->getStr("pf_route_load_option");
            if (!ro.empty())
            {
                if (roif.empty())
                    compiler->abort(
                        rule, 
                        "Interface specification is required "
                        "for action Route.");

                if (ro == "route_through")
                    compiler->output << "route-to ";
                else if (ro == "route_reply_through")
                    compiler->output << "reply-to ";
                else if (ro == "route_copy_through")
                    compiler->output << "dup-to ";
                else
                    compiler->abort(
                        rule, 
                        "Unknown option for rule action Route: '" + 
                        ro + "'");
            		
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
                                    rule, "Illegal IP address for next hop");
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
                                    rule, "Illegal netmask for next hop");
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
                                    rule, "Illegal IP address for next hop");
                            }
                            route_member++;
                        }
                    }
                }
                if (route_member < 1)
                {
                    compiler->abort(
                        rule, 
                        "No router specified rule action Route: '" +  ro + "'");
                }
                if (route_member >= 2 && (roload.empty() || roload == "none"))
                {
                    compiler->abort(
                        rule, 
                        "More than one router specified without load "
                        "balancing for rule action Route: '" + ro + "'");
                }
                if (route_member == 1 && ((!roload.empty()) && roload != "none"))
                {
                    compiler->abort(
                        rule, 
                        "Only one router specified with load balancing "
                        "for rule action Route: '" + ro + "'");
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

    if (rule->getClassification())
    {
        compiler->output << "queue ";
        compiler->output << ruleopt->getStr("pf_classify_str") << " ";
    }
}

void PolicyCompiler_pf::PrintRule::_printUser(PolicyRule *rule)
{
    RuleElementSrv *srvrel = rule->getSrv();
    FWObject *o = srvrel->front();
    if (o && FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
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
	    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
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
    if (rule->getTagging())
        compiler->output << "tag " << rule->getTagValue() << " ";
}

void PolicyCompiler_pf::PrintRule::_printDirection(PolicyRule *rule)
{
    if (rule->getDirection()==PolicyRule::Outbound) compiler->output << "out ";
    if (rule->getDirection()==PolicyRule::Inbound) compiler->output << "in  ";
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
    if (rule && (n=s.find("%N"))!=string::npos )
    {
        std::ostringstream s1;
        s1 << rule->getPosition();
        s.replace(n,2,s1.str());
    }
    if (rule && (n=s.find("%A"))!=string::npos )
    {
        std::ostringstream s1;
        switch (rule->getAction())
        {
        case PolicyRule::Accept:  s1 << "ACCEPT"; break;
        case PolicyRule::Deny:    s1 << "DROP";   break;
        case PolicyRule::Reject:  s1 << "REJECT"; break;
        case PolicyRule::Return:  s1 << "RETURN"; break;
        default: break;
        }
        s.replace(n,2,s1.str());
    }
    if (rule && (n=s.find("%I"))!=string::npos )
    {
        std::ostringstream s1;

        RuleElementItf *intf_re = rule->getItf();
        string rule_interfaces;
        if (!intf_re->isAny())
        {
            for (FWObject::iterator it=intf_re->begin(); it!=intf_re->end(); ++it)
            {
                FWObject *o = FWObjectReference::getObject(*it);
                rule_interfaces += " " + o->getName();
            }
        }
        if (!rule_interfaces.empty()) 
            s.replace(n, 2, rule_interfaces);
        else
            s.replace(n, 2, "global");
    }
    if (rule && (n=s.find("%C"))!=string::npos )
    {
        s.replace(n,2,"");  // there is no chain in PF and friends
    }

    return "\"" + s + "\" ";
}

void PolicyCompiler_pf::PrintRule::_printInterface(PolicyRule *rule)
{
    RuleElementItf *intf_re = rule->getItf();
    QStringList rule_interfaces;

    if (!intf_re->isAny())
    {
        for (FWObject::iterator it=intf_re->begin(); it!=intf_re->end(); ++it)
        {
            FWObject *o = FWObjectReference::getObject(*it);
            rule_interfaces << o->getName().c_str();
        }
        if (rule_interfaces.size() > 1)
        {
            rule_interfaces.push_front("{");
            rule_interfaces.push_back("}");
        }
        compiler->output << "on "
                         << rule_interfaces.join(" ").toStdString()
                         << " ";
    }
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

    if (!neg)
    {
	if (rs>0 || re>0)
        {
            if (rs>re && re==0) re=rs;

	    if (rs==re)  str << rs;   // TODO: do we need '=' here ?
	    else
		if (rs==0 && re!=0)      str << "<= " << re;
		else
		    if (rs!=0 && re==65535)  str << ">= " << rs;
		    else
                    {
/* 
 * port range. Operator '><' defines range in a such way that boundaries
 * are not included. Since we assume it is inclusive, let's move boundaries 
 */
			if (rs>0    ) rs--;
			if (re<65535) re++;
			str << rs << " >< " << re;
		    }
	}
    } else
    {
	if (rs>0 || re>0)
        {
	    if (rs==re)  str << "!= " << rs;
	    else
		if (rs==0 && re!=0)      str << "> " << re;
		else
		    if (rs!=0 && re==65535)  str << "< " << rs;
		    else
                    {
			str << rs << " <> " << re;
		    }
	}

    }
    return str.str();
}

/*
 * we made sure that all services in rel  represent the same protocol. 
 */
void PolicyCompiler_pf::PrintRule::_printSrcService(RuleElement  *rel)
{
/* I do not want to use rel->getFirst because it traverses the tree to
 * find the object. I'd rather use a cached copy in the compiler
 */
    FWObject *o=rel->front();
    if (o && FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

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
	    if (FWReference::cast(o)!=nullptr)
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

void PolicyCompiler_pf::PrintRule::_printDstService(RuleElement  *rel)
{
    FWObject *o=rel->front();
    if (o && FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
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
            if (!tos.empty()) compiler->output << " tos " << tos << " ";
            if (!dscp.empty())
                compiler->abort(
                                    rel->getParent(),
                                    "PF does not support DSCP matching");
        }
    } else 
    {
	string str;
	for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
        {
	    FWObject *o= *i;
	    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
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

void PolicyCompiler_pf::PrintRule::_printAddr(FWObject *o, bool )
{
    MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
    if (atrt!=nullptr)
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
        if (atrt->getSubstitutionTypeName()==AttachedNetworks::TYPENAME)
        {
            compiler->output << atrt->getSourceName() << ":network ";
            return ;
        }

        assert(atrt==nullptr);
    }

    if (o->getBool("pf_table"))
    {
        compiler->output << "<" << o->getName() << "> ";
        return;
    }

    Address *addr_obj = Address::cast(o);
    assert(addr_obj!=nullptr);

    const InetAddr *addr = addr_obj->getAddressPtr();
    InetAddr mask;

    if (Interface::cast(o)!=nullptr)
    {
	Interface *interface_=Interface::cast(o);
	if (interface_->isDyn()) 
        {
	    compiler->output << "(" << interface_->getName() << ") ";
	    return;
	}

	mask = InetAddr(InetAddr::getAllOnes());
    } else
    {
        mask = *(addr_obj->getNetmaskPtr());
    }

    if (addr_obj->dimension()==1) 
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
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
        _printAddr(o , negflag);
    }
    compiler->output << "} ";
}

void PolicyCompiler_pf::PrintRule::_printSrcAddr(RuleElement  *rel)
{
    FWObject *o=rel->front();
    FWReference *oref = FWReference::cast(o);
    if (o && oref!=nullptr) o=oref->getPointer();

    _printNegation(rel);

    if (o==nullptr)
    {
        PolicyRule *rule = PolicyRule::cast(rel->getParent());
        ostringstream errstr;
        errstr << "Broken rule element " 
               << rel->getTypeName()
               << " in rule '"
               << rule->getLabel()
               << "'  rel->front(): "
               << oref->getPointerId();
        compiler->abort(rel->getParent(), errstr.str());
    }

    if (rel->size()==1)
    {
	_printAddr(o, rel->getNeg() );
    } else
    {
        _printAddrList(rel,rel->getNeg());
    }

}

void PolicyCompiler_pf::PrintRule::_printDstAddr(RuleElement  *rel)
{
    FWObject *o=rel->front();
    FWReference *oref = FWReference::cast(o);
    if (o && oref!=nullptr) o=oref->getPointer();

    _printNegation(rel);

    if (o==nullptr)
    {
        PolicyRule *rule = PolicyRule::cast(rel->getParent());
        ostringstream errstr;
        errstr << "Broken rule element "
               << rel->getTypeName()
               << " in rule '"
               << rule->getLabel()
               <<  "'  rel->front(): "
               << oref->getPointerId();
        compiler->abort(rel->getParent(), errstr.str());
    }

    if (rel->size()==1)
    {
	_printAddr(o, rel->getNeg());
    } else
    {
        _printAddrList(rel, rel->getNeg());
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
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    FWOptions  *ruleopt = rule->getOptionsObject();
    string version = compiler->fw->getStr("version");

    tmp_queue.push_back(rule);

    compiler->output << compiler->printComment(rule, current_rule_label, "#");

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

    if (XMLTools::version_compare(version, "4.7")<0)
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
 *
 * 2. modulate state applies only to TCP services. Since we use
 *    groupServicesByProtocol, all services in a rule are of the same
 *    protocol, therefore we can simply check type of srv
 */
    if ( ! ruleopt->getBool("stateless") )
    {

        TCPService *tcpsrv=TCPService::cast(srv);

        if (tcpsrv!=nullptr && ! tcpsrv->inspectFlags() )
        {
            // tcp service, no special flag match

//            if ( version == "4.x")
            if (XMLTools::version_compare(version, "4.0")>=0)
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

        /*
         * in PF "modulate state", "synproxy state", "keep state" are
         * mutually exclusive "keep state" can be used with any
         * protocol, while "modulate state" and "synproxy state" can
         * only be used with tcp.
         */

        bool have_state_option = false;

        /*
         * First, set explicit state tracking parameter, then add
         * stateful tracking options. 
         */
	if (ruleopt->getBool("pf_synproxy") && tcpsrv!=nullptr)
        {
	    compiler->output << "synproxy state ";
            have_state_option = true;
        } else
        {
            if ((ruleopt->getBool("pf_modulate_state") || 
                 compiler->getCachedFwOpt()->getBool("pf_modulate_state")) &&
                tcpsrv!=nullptr) 
            {
                compiler->output << "modulate state ";
                have_state_option = true;
            } else
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
                if (XMLTools::version_compare(version, "4.0") < 0 ||
                     compiler->getCachedFwOpt()->getBool("pf_keep_state"))
                {
                    compiler->output << "keep state ";
                    have_state_option = true;
                }
            }
        }

        /*
         * Stateful tracking options. According to the pf.conf manual,
         * one of keep state, modulate state, or synproxy state must
         * be specified explicitly to apply these options to a rule.
         * Using flags need_state_option and have_state_option for that.
         */

        QStringList options;
        bool need_state_option = false;

        if (ruleopt->getInt("pf_rule_max_state")>0)
        {
            options.push_back(QString("max %1").arg(ruleopt->getInt("pf_rule_max_state")));
            need_state_option = true;
        }

        if (ruleopt->getBool("pf_sloppy_tracker"))
        {
            options.push_back("sloppy");
            need_state_option = true;
        }

        if (ruleopt->getBool("pf_no_sync"))
        {
            options.push_back("no-sync");
            need_state_option = true;
        }

        if (ruleopt->getBool("pf_pflow"))
        {
            options.push_back("pflow");
            need_state_option = true;
        }

        if (ruleopt->getBool("pf_source_tracking"))
        {
            if (ruleopt->getInt("pf_max_src_nodes") > 0)
            {
                options.push_back(QString("max-src-nodes %1").arg(
                                      ruleopt->getInt("pf_max_src_nodes")));
                need_state_option = true;
            }

            if (ruleopt->getInt("pf_max_src_states")>0)
            {
                options.push_back(QString("max-src-states %1").arg(
                                      ruleopt->getInt("pf_max_src_states")));
                need_state_option = true;
            }
        }

        bool check_overload_opts = false;
        if (ruleopt->getInt("pf_max_src_conn")>0)
        {
            options.push_back(QString("max-src-conn %1").arg(
                                  ruleopt->getInt("pf_max_src_conn")));
            check_overload_opts = true;
            need_state_option = true;
        }

        if (ruleopt->getInt("pf_max_src_conn_rate_num")>0 && 
            ruleopt->getInt("pf_max_src_conn_rate_seconds")>0)
        {
            options.push_back(QString("max-src-conn-rate %1/%2")
                              .arg(ruleopt->getInt("pf_max_src_conn_rate_num"))
                              .arg(ruleopt->getInt("pf_max_src_conn_rate_seconds")));
            check_overload_opts = true;
            need_state_option = true;
        }

        if (check_overload_opts)
        {
            QStringList overload_opts;
            if (ruleopt->getStr("pf_max_src_conn_overload_table")!="")
                overload_opts.push_back(
                    QString("overload <%1>").arg(
                        ruleopt->getStr("pf_max_src_conn_overload_table").c_str()));
            if (ruleopt->getBool("pf_max_src_conn_flush"))
                overload_opts.push_back("flush");
            if (ruleopt->getBool("pf_max_src_conn_global"))
                overload_opts.push_back("global");
            if (overload_opts.size() > 0)
                options.push_back(overload_opts.join(" "));
        }

        if (need_state_option && !have_state_option)
        {
            compiler->output << "keep state ";
        }

        // looks like pf.conf syntax requires '(' ')' even if there is
        // only one option
        if (options.size() > 0) compiler->output << "( ";

        compiler->output << options.join(", ").toStdString();

        if (options.size() > 0) compiler->output << " )";

    } else
    {
        // stateless rule
        if (XMLTools::version_compare(version, "4.0")>=0)
        {
            // v4.x, stateless rule
            compiler->output << "no state ";
        }
    }

    if (rule->getBool("allow_opts")) compiler->output << "allow-opts  ";

    _printQueue(rule);
    _printLabel(rule);

    if (XMLTools::version_compare(version, "4.7")>=0)
        _printRouteOptions(rule);

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
