/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: PolicyCompiler_PrintRule.cpp 1456 2007-12-13 16:38:34Z vk $

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

#include "PolicyCompiler_ipt.h"
#include "OSConfigurator_linux24.h"

#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/DNSName.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/AddressTable.h"

#include "fwbuilder/Inet6AddrMask.h"

#include "combinedAddress.h"


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

/*
 *  check and create new chain if needed
 */
string PolicyCompiler_ipt::PrintRule::_createChain(const string &chain)
{
    string res;
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);

    if ( ! chains[chain] )
    {
	res = string((ipt_comp->ipv6) ? "$IP6TABLES -N " : "$IPTABLES -N ") +
            chain;
        if (ipt_comp->my_table != "filter") res += " -t " + ipt_comp->my_table;
        res += "\n";
	chains[chain]=true;
    }
    return res;
}

string PolicyCompiler_ipt::PrintRule::_startRuleLine()
{            
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    string res = (ipt_comp->ipv6) ? "$IP6TABLES " : "$IPTABLES ";

    if (ipt_comp->my_table != "filter") res += "-t " + ipt_comp->my_table + " ";

    res += "-A ";
    return res;
}

string PolicyCompiler_ipt::PrintRule::_endRuleLine()
{            
    return string("\n");
}

string PolicyCompiler_ipt::PrintRule::_printRuleLabel(PolicyRule *rule)
{
    ostringstream res;

    bool nocomm = Resources::os_res[compiler->fw->getStr("host_OS")]->
        Resources::getResourceBool(
            "/FWBuilderResources/Target/options/suppress_comments");

    string rl=rule->getLabel();
    if (rl!=current_rule_label)
    {
        if (!nocomm)
        {
            res << "# " << endl;
            res << "# Rule " << rl << endl;
            res << "# " << endl;
        }
        res << "echo " << _quote(string("Rule ")+rl) << endl;
        res << "# " << endl;

/* do not put comment in the script if it is intended for linksys */
        if (!nocomm)
        {
            string    comm=rule->getComment();
            string::size_type c1,c2;
            c1=0;
            while ( (c2=comm.find('\n',c1))!=string::npos ) {
                res << "# " << comm.substr(c1,c2-c1) << endl;
                c1=c2+1;
            }
            res << "# " << comm.substr(c1) << endl;
            res << "# " << endl;
        }
        current_rule_label=rl;
    }
    return res.str();
}




/**
 *-----------------------------------------------------------------------
 */
string PolicyCompiler_ipt::PrintRule::_printChain(PolicyRule *rule)
{
    string s=rule->getStr("ipt_chain");
    if (s.empty()) s="UNKNOWN";
    s= s + " ";
    return s;
}

string PolicyCompiler_ipt::PrintRule::_printModules(PolicyRule *rule)
{
    std::ostringstream ostr;

    string target=rule->getStr("ipt_target");
    if (target.empty()) target="UNKNOWN";

    FWOptions *ruleopt =rule->getOptionsObject();
    int lim = 0;

/*
 * Here is what do we do with limits:
 *
 * Limit set globally in 'Firewall' tab of the firewall dialog 
 * applies only to logging
 *
 * Limit set in the rule options dialog applies only to this 
 * rule's target.
 * 
 *    this is so as of 1.0.11 ( 28/06/03 )  --vk
 */
    if (target=="LOG")
    {
        FWOptions *compopt=compiler->getCachedFwOpt();
        if ((lim=compopt->getInt("limit_value"))>0)
        {
            ostr << " -m limit --limit " << lim;
        
            string ls=compopt->getStr("limit_suffix");
            if (!ls.empty()) ostr << ls;

            int lb=compopt->getInt("limit_burst");
            if (lb>0) ostr << " --limit-burst " << lb;
        }
    } else {
        if (ruleopt!=NULL && (lim=ruleopt->getInt("limit_value"))>0)
        {
            ostr << " -m limit --limit " << lim;

            string ls=ruleopt->getStr("limit_suffix");
            if (!ls.empty()) ostr << ls;

            int lb=ruleopt->getInt("limit_burst");
            if (lb>0) ostr << " --limit-burst " << lb;
        }
    }

    if (ruleopt!=NULL && (lim=ruleopt->getInt("connlimit_value"))>0)
    {
        ostr << " -m connlimit --connlimit-above " << lim;

        int ml=ruleopt->getInt("connlimit_masklen");
        if (ml>0) ostr << " --connlimit-mask " << ml;
    }

    if (ruleopt!=NULL && (lim=ruleopt->getInt("hashlimit_value"))>0)
    {
        string module_name = "hashlimit";
        if (ruleopt->getBool("hashlimit_dstlimit"))
            module_name = "dstlimit";

        ostr << " -m " << module_name << " --" << module_name << " " << lim;

        string ls = ruleopt->getStr("hashlimit_suffix");
        if (!ls.empty()) ostr << ls;
            
        int lb=ruleopt->getInt("hashlimit_burst");
        if (lb>0) ostr << " --" << module_name << "-burst " << lb;

        ls=ruleopt->getStr("hashlimit_mode");
        if (!ls.empty()) ostr << " --" << module_name << "-mode " << ls;

        string hl_name = ruleopt->getStr("hashlimit_name");
        if (hl_name.empty())
        {
            std::ostringstream hn;
            hn << "htable_rule_" << rule->getPosition();
            hl_name = hn.str();
        }
        ostr << " --" << module_name << "-name " << hl_name;

        int arg = ruleopt->getInt("hashlimit_size");
        if (arg>0) ostr << " --" << module_name << "-htable-size " << arg;

        arg = ruleopt->getInt("hashlimit_max");
        if (arg>0) ostr << " --" << module_name << "-htable-max " << arg;

        arg = ruleopt->getInt("hashlimit_expire");
        if (arg>0) ostr << " --" << module_name << "-htable-expire " << arg;

        arg = ruleopt->getInt("hashlimit_gcinterval");
        if (arg>0) ostr << " --" << module_name << "-htable-gcinterval " << arg;

    }

    return ostr.str();
}


string PolicyCompiler_ipt::PrintRule::_printTarget(PolicyRule *rule)
{
    std::ostringstream ostr;

    string target=rule->getStr("ipt_target");
    if (target.empty()) target="UNKNOWN";

    FWOptions *ruleopt =rule->getOptionsObject();

    if (target=="CUSTOM")
    {
        ostr << ruleopt->getStr("custom_str");
        return ostr.str();
    }

    if ( compiler->getCachedFwOpt()->getBool("use_ULOG") &&
         target=="LOG") target="ULOG";

    ostr << " -j " << target << " ";

    if (target=="REJECT")
      ostr << _printActionOnReject(rule);

    if (target=="LOG" || target=="ULOG")    
        ostr << _printLogParameters(rule);
    
    if (target=="MARK")
    {
//        ostr << " --set-mark " << ruleopt->getStr("tagvalue");
        ostr << " --set-mark " << rule->getTagValue();
    }

    if (target=="CONNMARK")
    {
        ostr << ruleopt->getStr("CONNMARK_arg");
    }

    if (target=="CLASSIFY")
    {
        ostr << " --set-class " << ruleopt->getStr("classify_str");
    }

    if (target=="ROUTE")
    {
        string a;
        a = ruleopt->getStr("ipt_iif");
        if (!a.empty()) ostr << " --iif " << a;

        a = ruleopt->getStr("ipt_oif");
        if (!a.empty()) ostr << " --oif " << a;

        a = ruleopt->getStr("ipt_gw");
        if (!a.empty()) ostr << " --gw " << a;

        bool c = ruleopt->getBool("ipt_continue");
        if (c) ostr << " --continue";

        c = ruleopt->getBool("ipt_tee");
        if (c) ostr << " --tee";
    }

    return ostr.str();
}

string PolicyCompiler_ipt::PrintRule::_printMultiport(PolicyRule *rule)
{
    RuleElementSrv *srvrel=rule->getSrv();
    string s;
    if(srvrel->size()>1 && rule->getBool("ipt_multiport"))
	s= " -m multiport ";

    return s;
}

string PolicyCompiler_ipt::PrintRule::_printDirectionAndInterface(PolicyRule *rule)
{
    std::ostringstream ostr;

    string       iface_name = rule->getInterfaceStr();
    if (iface_name.empty() || iface_name=="nil" )   return "";

/* if interface name ends with '*', this is a wildcard
 * interface. Iptables supports wildcard interfaces but uses '+' as a
 * wildcard symbol */

    string::size_type n;
    if ( (n=iface_name.find("*"))!=string::npos)    iface_name[n]='+';

    string version=compiler->fw->getStr("version");

    Interface *rule_iface =
        compiler->getCachedFwInterface(rule->getInterfaceId());

    if (rule_iface && rule_iface->isBridgePort() &&
        (version == "1.3.0" || version == "1.4.0"))
    {
        if (rule->getDirection()==PolicyRule::Inbound)   
            ostr << " -m physdev --physdev-in "  << iface_name; 

        if (rule->getDirection()==PolicyRule::Outbound)  
            ostr << " -m physdev --physdev-out "  << iface_name; 
    } else
    {
        if (rule->getDirection()==PolicyRule::Inbound)   
            ostr << " -i "  << iface_name; 

        if (rule->getDirection()==PolicyRule::Outbound)  
            ostr << " -o "  << iface_name; 
    }

//    if (rule->getDirection()==PolicyRule::Both)      
//        compiler->output << "-i "  << rule_iface->getName() 
//                         << " -o " << rule_iface->getName(); 
    ostr << " ";

    return ostr.str();
}

string PolicyCompiler_ipt::PrintRule::_printActionOnReject(libfwbuilder::PolicyRule *rule)
{
    std::ostringstream str;

    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);

//    RuleElementSrv *srvrel=rule->getSrv();
    Service        *srv   =compiler->getFirstSrv(rule);  assert(srv);

    string version=compiler->fw->getStr("version");
    string s=ipt_comp->getActionOnReject(rule);
    if (!s.empty()) 
    {
        if (ipt_comp->isActionOnRejectTCPRST(rule))
            str << " --reject-with tcp-reset";

	if (s.find("ICMP")!=string::npos) 
        {
	    if (s.find("unreachable")!=string::npos) 
            {
		if (s.find("net")!=string::npos)
                    str << " --reject-with icmp-net-unreachable";
		if (s.find("host")!=string::npos)
                    str << " --reject-with icmp-host-unreachable";
		if (s.find("port")!=string::npos)
                    str << " --reject-with icmp-port-unreachable";
		if (s.find("proto")!=string::npos)
                    str << " --reject-with icmp-proto-unreachable";
	    }
	    if (s.find("prohibited")!=string::npos) 
            {
		if (s.find("net")!=string::npos)
                    str << " --reject-with icmp-net-prohibited";
		if (s.find("host")!=string::npos)
                    str << " --reject-with icmp-host-prohibited";
                if (ipt_comp->newIptables(version) &&
                    s.find("admin")!=string::npos)
                    str << " --reject-with icmp-admin-prohibited";
	    }
	}
    }
    str << " ";
    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printGlobalLogParameters()
{
    return _printLogParameters(NULL);
}

string PolicyCompiler_ipt::PrintRule::_printLogPrefix(const string &rule_num,
						      const string &action,
						      const string &interf,
						      const string &chain,
						      const string &rule_label,
						      const string &prefix)
{
    string s=prefix;

/* deal with our logging macros:
 * %N - rule number  ('2', or '2/3' for rule in a branch)
 * %A - action
 * %I - interface name
 * %C - chain name
 */
    string::size_type n;
    if ((n=s.find("%N"))!=string::npos ) 
    {
      s.replace(n,2,rule_num);
    }
    if ((n=s.find("%A"))!=string::npos ) 
    {
      s.replace(n,2,action);
    }
    if ((n=s.find("%I"))!=string::npos ) 
    {
      s.replace(n,2,interf);
    }
    if ((n=s.find("%C"))!=string::npos ) 
    {
      s.replace(n,2,chain);
    }

    if (s.length()>29)
    {
        compiler->warning(_("Log prefix has been truncated to 29 characters in rule ")+rule_label);
        s=s.substr(0,29);
    }

    return _quote( s );
}

string PolicyCompiler_ipt::PrintRule::_printLogPrefix(PolicyRule *rule,
                                                      const string &prefix)
{
  char action[64];
  strncpy(action,rule->getStr("stored_action").c_str(),sizeof(action));
  for (char *cptr=action; *cptr; cptr++)  *cptr=toupper(*cptr);

  string rule_iface =  rule->getInterfaceStr();
  if (rule_iface=="")  rule_iface = "global";

  std::ostringstream s1;
  int pos=rule->getPosition();
  // parent_rule_num is set by processor "Branching" for branch rules
  string ppos = rule->getStr("parent_rule_num");

  if (ppos != "")
      s1 << ppos << "/";
  s1 << pos;

  return _printLogPrefix(s1.str(),
			 action,
			 rule_iface,
			 rule->getStr("ipt_chain"),
			 rule->getLabel(),
			 prefix);
}

string PolicyCompiler_ipt::PrintRule::_printLogParameters(libfwbuilder::PolicyRule *rule)
{
    std::ostringstream str;
    string s;
//    int    l;
    FWOptions *ruleopt =(rule!=NULL)?rule->getOptionsObject():compiler->getCachedFwOpt();

    bool   use_ulog=compiler->getCachedFwOpt()->getBool("use_ULOG");

    if (use_ulog)
    {
        s=ruleopt->getStr("ulog_nlgroup");
        if (s.empty())  s=compiler->getCachedFwOpt()->getStr("ulog_nlgroup");
        if (!s.empty()) 
            str << " --ulog-nlgroup " << s;

        s=ruleopt->getStr("log_prefix");
        if (s.empty())  s=compiler->getCachedFwOpt()->getStr("log_prefix");
        if (!s.empty()) 
            str << " --ulog-prefix " << _printLogPrefix(rule,s);

        int r=compiler->getCachedFwOpt()->getInt("ulog_cprange");
        if (r!=0)  str << " --ulog-cprange " << r << " ";
        r=compiler->getCachedFwOpt()->getInt("ulog_qthreshold");
        if (r!=0)  str << " --ulog-qthreshold " << r << " ";
    } else
    {
        bool   numeric_levels;
        numeric_levels=compiler->getCachedFwOpt()->getBool("use_numeric_log_levels");
        s=ruleopt->getStr("log_level");
        if (s.empty())  s=compiler->getCachedFwOpt()->getStr("log_level");
        if (!s.empty()) 
        {
            if ( numeric_levels )
            {
                if (s=="alert")   s="1";
                if (s=="crit")    s="2";
                if (s=="error")   s="3";
                if (s=="warning") s="4";
                if (s=="notice")  s="5";
                if (s=="info")    s="6";
                if (s=="debug")   s="7";
            }
            str << " --log-level " << s;
        }

        s=ruleopt->getStr("log_prefix");
        if (s.empty())  s=compiler->getCachedFwOpt()->getStr("log_prefix");
        if (!s.empty())
            str << " --log-prefix " << _printLogPrefix(rule,s);

        if (ruleopt->getBool("log_tcp_seq") || compiler->getCachedFwOpt()->getBool("log_tcp_seq"))  
            str << " --log-tcp-sequence ";
        if (ruleopt->getBool("log_tcp_opt") || compiler->getCachedFwOpt()->getBool("log_tcp_opt"))  
            str << " --log-tcp-options ";
        if (ruleopt->getBool("log_ip_opt") || compiler->getCachedFwOpt()->getBool("log_ip_opt"))   
            str << " --log-ip-options ";
    }

    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printLimit(libfwbuilder::PolicyRule *rule)
{
    std::ostringstream str;
    string s;
    int    l, lb;
    FWOptions *ruleopt =rule->getOptionsObject();
    FWOptions *compopt =compiler->getCachedFwOpt();

    if ( (ruleopt!=NULL && (l=ruleopt->getInt("limit_value"))>0) || 
         (l=compopt->getInt("limit_value"))>0 ) 
    {
	str << "  -m limit --limit " << l;

        if (ruleopt!=NULL) s=ruleopt->getStr("limit_suffix");
	if (s.empty()) 	   s=compopt->getStr("limit_suffix");
	if (!s.empty()) str << s;
        
        lb=-1;
	if (ruleopt!=NULL) lb=ruleopt->getInt("limit_burst");
	if (lb<0)          lb=compopt->getInt("limit_burst");
	if (lb>0)          str << " --limit-burst " << lb;
    }

    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printProtocol(libfwbuilder::Service *srv)
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    string version=compiler->fw->getStr("version");
    string s;
    if (! srv->isAny() && !CustomService::isA(srv)  && !TagService::isA(srv))
    {
        string pn=srv->getProtocolName();
        if (pn=="ip") pn="all";
        
        s= "-p " + pn + " ";

        if (pn == "icmp")
        {
            if (ipt_comp->newIptables(version))
            {
                s += " -m icmp ";
            }
        } else
        {
            if (pn == "tcp")  s += "-m tcp ";
            if (pn == "udp")  s += "-m udp ";
        }
    }
    return s;
}

string PolicyCompiler_ipt::PrintRule::_printPorts(int rs,int re)
{
    std::ostringstream  str;

    compiler->normalizePortRange(rs,re);

    if (rs>0 || re>0) {
	if (rs==re)  str << rs;
	else
	    if (rs==0 && re!=0)      str << ":" << re;
	    else
                str << rs << ":" << re;
    }
    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printSrcPorts(Service *srv)
{
    std::ostringstream  str;
    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int rs=srv->getInt("src_range_start");
	int re=srv->getInt("src_range_end");
	str << _printPorts(rs,re);
    }
    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printDstPorts(Service *srv)
{
    std::ostringstream  str;
    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int rs=srv->getInt("dst_range_start");
	int re=srv->getInt("dst_range_end");
	str << _printPorts(rs,re);
    }
    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printICMP(ICMPService *srv)
{
    std::ostringstream  str;
    if (ICMPService::isA(srv) && srv->getInt("type")!=-1) {
	str << srv->getStr("type");
	if (srv->getInt("code")!=-1) 
	    str << "/" << srv->getStr("code") << " ";
    }
    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printIP(IPService *srv)
{
    std::ostringstream  str;
    if (IPService::isA(srv) ) {
	if (srv->getBool("fragm") || srv->getBool("short_fragm"))
	    str << " -f ";

        if  (srv->getBool("lsrr") ||
             srv->getBool("ssrr") ||
             srv->getBool("rr") ||
             srv->getBool("ts") ) str << " -m ipv4options ";

        if  (srv->getBool("lsrr")) str << " --lsrr";
        if  (srv->getBool("ssrr")) str << " --ssrr";
        if  (srv->getBool("rr"))   str << " --rr";
        if  (srv->getBool("ts"))   str << " --ts";
    }
    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printTCPFlags(libfwbuilder::TCPService *srv)
{
    string str;
    if (srv->inspectFlags())
    {
        TCPService::TCPFlag f1[2]={ TCPService::SYN };
        TCPService::TCPFlag f2[7]={ TCPService::URG,
                                    TCPService::ACK,
                                    TCPService::PSH,
                                    TCPService::RST,
                                    TCPService::SYN,
                                    TCPService::FIN };

        std::set<TCPService::TCPFlag> none;
        std::set<TCPService::TCPFlag> syn( f1, f1+1 );
        std::set<TCPService::TCPFlag> all_masks( f2 , f2+6 );

        if (srv->getAllTCPFlags()==syn  && srv->getAllTCPFlagMasks()==all_masks)
            str=" --tcp-flags SYN,RST,ACK SYN ";
        else
        {
            str=" --tcp-flags ";
            bool first=true;

            if (srv->getAllTCPFlagMasks()==all_masks) str+="ALL";
            else
            {
                if (srv->getTCPFlagMask(TCPService::URG)) { if (!first) str+=","; str+="URG"; first=false; }
                if (srv->getTCPFlagMask(TCPService::ACK)) { if (!first) str+=","; str+="ACK"; first=false; }
                if (srv->getTCPFlagMask(TCPService::PSH)) { if (!first) str+=","; str+="PSH"; first=false; }
                if (srv->getTCPFlagMask(TCPService::RST)) { if (!first) str+=","; str+="RST"; first=false; }
                if (srv->getTCPFlagMask(TCPService::SYN)) { if (!first) str+=","; str+="SYN"; first=false; }
                if (srv->getTCPFlagMask(TCPService::FIN)) { if (!first) str+=","; str+="FIN"; first=false; }
            }

            str+=" ";

            if (srv->getAllTCPFlags()==none) str+="NONE";
            else
            {
                first=true;
                if (srv->getTCPFlag(TCPService::URG)) { if (!first) str+=","; str+="URG"; first=false; }
                if (srv->getTCPFlag(TCPService::ACK)) { if (!first) str+=","; str+="ACK"; first=false; }
                if (srv->getTCPFlag(TCPService::PSH)) { if (!first) str+=","; str+="PSH"; first=false; }
                if (srv->getTCPFlag(TCPService::RST)) { if (!first) str+=","; str+="RST"; first=false; }
                if (srv->getTCPFlag(TCPService::SYN)) { if (!first) str+=","; str+="SYN"; first=false; }
                if (srv->getTCPFlag(TCPService::FIN)) { if (!first) str+=","; str+="FIN"; first=false; }
            }
        }
    }
    return str;
}

/*
 * we made sure that all services in rel  represent the same protocol
 */
string PolicyCompiler_ipt::PrintRule::_printSrcService(RuleElementSrv  *rel)
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    std::ostringstream  ostr;
/* I do not want to use rel->getFirst because it traverses the tree to
 * find the object. I'd rather use a cached copy in the compiler
 */
    FWObject *o=rel->front();
    if (o && FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    Service *srv= Service::cast(o);


    if (rel->size()==1) {
	if (UDPService::isA(srv) || TCPService::isA(srv)) {
	    string str=_printSrcPorts( srv );
	    if (! str.empty() ) 
            {
                ostr << " --sport ";
                ostr  << _printSingleObjectNegation(rel) << str << " ";
            }
	}
    } else {
/* use multiport */

	string str;
	bool  first=true;
	for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) {
	    FWObject *o= *i;
//	    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
	    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

	    Service *s=Service::cast( o );
	    assert(s);
	    if (UDPService::isA(srv) || TCPService::isA(srv)) {
		if (!first) str+=",";
		str+= _printSrcPorts( s );
		if (!str.empty()) first=false;
	    }
	}
	if ( !str.empty() ) 
        {
            string v=compiler->fw->getStr("version");
            if (ipt_comp->newIptables(v))
                ostr << " --sports ";
            else
                ostr << " --source-port ";

	    ostr << str << " ";
	}
    }
    return ostr.str();
}

string PolicyCompiler_ipt::PrintRule::_printDstService(RuleElementSrv  *rel)
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    std::ostringstream  ostr;
    FWObject *o=rel->front();
    string version=compiler->fw->getStr("version");
    if (o && FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    Service *srv= Service::cast(o);

    if (rel->size()==1) 
    {
	if (UDPService::isA(srv) || TCPService::isA(srv)) 
        {
	    string str=_printDstPorts( srv );
	    if (! str.empty() )
            {
                ostr << " --dport ";
                ostr << _printSingleObjectNegation(rel) << str << " ";
            }
	}
	if (TCPService::isA(srv)) 
        {
	    string str=_printTCPFlags(TCPService::cast(srv));
	    if (!str.empty()) 
            {
                ostr  << _printSingleObjectNegation(rel)
                      << str << " ";
            }
	}
	if (ICMPService::isA(srv)) 
        {
	    string str=_printICMP( ICMPService::cast(srv) );
	    if (str.empty() ) 
            {
                if (ipt_comp->newIptables(version))
                    ostr << " --icmp-type any ";
            } else
            {
                ostr << " --icmp-type " 
                     << _printSingleObjectNegation(rel)
                     << str << " ";
            }
	}
	if (IPService::isA(srv)) 
        {
	    string str=_printIP( IPService::cast(srv) );
	    if (! str.empty() ) 
            {
                ostr  << _printSingleObjectNegation(rel)
                      << str << " ";
            }
	}
	if (CustomService::isA(srv)) 
        {
	    ostr << _printSingleObjectNegation(rel) << " "
                 << CustomService::cast(srv)->getCodeForPlatform( compiler->myPlatformName() ) << " ";
	}
        if (TagService::isA(srv))
        {
	    ostr << "-m mark --mark "
                 << TagService::cast(srv)->getCode() << " ";
        }
    } else 
    {
/* use multiport */

	string str;
	for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
        {
	    FWObject *o= *i;
	    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

	    Service *s=Service::cast( o );
	    assert(s);
	    if (UDPService::isA(srv) || TCPService::isA(srv)) 
            {
		string str1 = _printDstPorts( s );
                if (str!="" && str1!="") str+=",";
                str+=str1;
	    }
	}
	if ( !str.empty() ) 
        {
            string v=compiler->fw->getStr("version");
            if (ipt_comp->newIptables(v))
                ostr << " --dports ";
            else
                ostr << " --destination-port ";
	    ostr << str << " ";
	}
    }
    return ostr.str();
}

string PolicyCompiler_ipt::PrintRule::_printAddr(Address  *o)
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    std::ostringstream  ostr;

    MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
    if (atrt!=NULL)
    {
        if (atrt->getSubstitutionTypeName()==AddressTable::TYPENAME)
        {
            ostr << "$" << ipt_comp->getAddressTableVarName(atrt) << " ";
            return ostr.str();
        }

        if (atrt->getSubstitutionTypeName()==DNSName::TYPENAME)
        {
            return atrt->getSourceName();
        }
        // at this time we only support two types of MultiAddress
        // objects: AddressTable and DNSName. Both should be converted
        // to MultiAddressRunTime at this point. If we get some other
        // kind of MultiAddressRunTime object, we do not know what to do
        // with it so we stop.
        assert(atrt==NULL);
    }

    if (Interface::cast(o)!=NULL)
    {
        Interface *iface=Interface::cast(o);
        if (iface->isDyn())
            ostr << "$" << ipt_comp->getInterfaceVarName(iface) << " ";
        return ostr.str();
    }

    const InetAddr *addr = o->getAddressPtr();
    const InetAddr *mask = o->getNetmaskPtr();

    if (addr==NULL)
    {
        compiler->warning(
            string("Empty inet address in object ") +
            o->getId());
        return ostr.str();
    }

    if (addr->isAny() && mask->isAny())
    {
        ostr << "0/0 ";
    } else 
    {
        ostr << addr->toString();

        if (Interface::cast(o)==NULL &&
            Address::cast(o)->dimension() > 1 &&
            !mask->isHostMask())
        {
            ostr << "/" << mask->getLength();
        }
        ostr << " ";
    }
    return ostr.str();
}


string PolicyCompiler_ipt::PrintRule::_printSingleObjectNegation(RuleElement *rel)
{
    if (rel->getBool("single_object_negation"))   return "! ";
    else return "";
}

string PolicyCompiler_ipt::PrintRule::_printTimeInterval(PolicyRule *r)
{
    std::ostringstream  ostr;

    RuleElementInterval* ri=r->getWhen();
    if (ri==NULL || ri->isAny()) return "";

    std::map<int,std::string>  daysofweek;

    daysofweek[0]="Sun";
    daysofweek[1]="Mon";
    daysofweek[2]="Tue";
    daysofweek[3]="Wed";
    daysofweek[4]="Thu";
    daysofweek[5]="Fri";
    daysofweek[6]="Sat";

    bool first;
    int smin, shour, sday, smonth, syear, sdayofweek;
    int emin, ehour, eday, emonth, eyear, edayofweek;

    Interval *interval=compiler->getFirstWhen(r);
    assert(interval!=NULL);

    interval->getStartTime( &smin, &shour, &sday, &smonth, &syear, &sdayofweek);
    interval->getEndTime(   &emin, &ehour, &eday, &emonth, &eyear, &edayofweek);

    ostr << "-m time ";

    if (shour<0) shour=0;
    if (smin<0)  smin=0;

    if (ehour<0) ehour=23;
    if (emin<0)  emin=59;

    bool use_timestart_timestop = true;

    if (sday>0 && smonth>0 && syear>0)
    {
        ostr << "--datestart " 
             << setw(2) << setfill('0') << syear << ":" 
             << setw(2) << setfill('0') << smonth << ":" 
             << setw(2) << setfill('0') << sday  << ":"
             << setw(2) << setfill('0') << shour << ":"
             << setw(2) << setfill('0') << smin << ":00 ";
        use_timestart_timestop = false;
    }

    if (eday>0 && emonth>0 && eyear>0)
    {
        ostr << "--datestop "
             << setw(2) << setfill('0') << syear << ":"
             << setw(2) << setfill('0') << smonth << ":"
             << setw(2) << setfill('0') << sday  << ":"
             << setw(2) << setfill('0') << ehour << ":"
             << setw(2) << setfill('0') << emin << ":00 ";
        use_timestart_timestop = false;
    }


    if (use_timestart_timestop )
    {
        ostr << " --timestart "
             << setw(2) << setfill('0') << shour << ":"
             << setw(2) << setfill('0') << smin  << " ";
        ostr << " --timestop "
             << setw(2) << setfill('0') << ehour << ":"
             << setw(2) << setfill('0') << emin  << " ";

        if (sdayofweek<0) sdayofweek=0;
        if (sdayofweek>6) sdayofweek=6;

        // if both start and end day are -1, need to
        // generate "sun,mon,tue,wed,thu,fri,sat"
        if (edayofweek<0) edayofweek=6;
        if (edayofweek>6) edayofweek=6;

        ostr << " --days ";
        first=true;

        bool inside_interval = false;
        int  day=0;
        while (1)
        {
            if (!inside_interval && day==sdayofweek) inside_interval=true;
            if (inside_interval)
            {
                if (!first) ostr << ",";
                first=false;
                ostr << daysofweek[day];
                // if sdayofweek==edayofweek print one day
                if (day==edayofweek)  break;
            }
            if (++day>6) day=0;
        }
    }

    return ostr.str();
}

PolicyCompiler_ipt::PrintRule::PrintRule(const std::string &name) : PolicyRuleProcessor(name) 
{ 
    init=true; 
    print_once_on_top=true;

    for (list<string>::const_iterator i =
             PolicyCompiler_ipt::getStandardChains().begin();
         i != PolicyCompiler_ipt::getStandardChains().end(); ++i)
    {
        chains[*i] = true;
    }
}

bool  PolicyCompiler_ipt::PrintRule::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule         *rule    =getNext(); 
    if (rule==NULL) return false;

    string chain = rule->getStr("ipt_chain");
    if (ipt_comp->chain_usage_counter[chain] > 0)
    {

        tmp_queue.push_back(rule);

        compiler->output << _printRuleLabel(rule);
        compiler->output << _createChain(rule->getStr("ipt_chain"));
        compiler->output << _createChain(rule->getStr("ipt_target"));
        compiler->output 
            << dynamic_cast<OSConfigurator_linux24*>(compiler->osconfigurator)->printRunTimeWrappers( rule, PolicyRuleToString(rule) );
    }
    return true;
}

string PolicyCompiler_ipt::PrintRule::PolicyRuleToString(PolicyRule *rule)
{
    FWOptions *ruleopt = rule->getOptionsObject();
    FWObject    *ref;

    RuleElementSrc *srcrel=rule->getSrc();
    ref=srcrel->front();
    Address        *src=Address::cast(FWReference::cast(ref)->getPointer());
    if(src==NULL)
        throw FWException(_("Broken SRC in ")+rule->getLabel());

    RuleElementDst *dstrel=rule->getDst();
    ref=dstrel->front();
    Address        *dst=Address::cast(FWReference::cast(ref)->getPointer());
    if(dst==NULL)
        throw FWException(_("Broken DST in ")+rule->getLabel());

    RuleElementSrv *srvrel=rule->getSrv();
    ref=srvrel->front();
    Service        *srv=Service::cast(FWReference::cast(ref)->getPointer());
    if(srv==NULL)
        throw FWException(_("Broken SRV in ")+rule->getLabel());


    std::ostringstream  command_line;

    command_line << _startRuleLine();

    command_line << _printChain(rule);
    command_line << _printDirectionAndInterface(rule);
    command_line << _printProtocol(srv);
    command_line << _printMultiport(rule);

    if (!src->isAny()) 
    {
        if (physAddress::isA(src) || combinedAddress::isA(src))
        {
            string physaddress = "";

            if (physAddress::isA(src))
            {
                physaddress = physAddress::cast(src)->getPhysAddress();
                if (physaddress.empty())
                {
                    compiler->warning("Empty MAC address in rule " +
                                      rule->getLabel());
                    physaddress = "00:00:00:00:00:00";
                }
            }

            if (combinedAddress::isA(src))
                physaddress = combinedAddress::cast(src)->getPhysAddress();

/* physAddress component of combinedAddress can be empty.  For example
 * this happens when an object with both IP and MAC addresses is found
 * in "source" and rule is determined to go into OUTPUT chain. On the
 * other hand, if physAddress object has no MAC address, it is always
 * an error.
 */
            if (!physaddress.empty())
            {
                command_line << " -m mac --mac-source "
                             << _printSingleObjectNegation(srcrel);
                command_line << physaddress;
            }

/*
 * fool-proof: this is last resort check for situation when user
 * created IPv4 object for the interface but left it with empty
 * address ( 0.0.0.0 ).
 *
 * note that combinedAddress inherits IPv4 and therefore
 * combinedAddress::hasInetAddress returns true;
 *
 */
            if (src->hasInetAddress() && !src->getAddressPtr()->isAny())
            {
                command_line << " -s " << _printSingleObjectNegation(srcrel);
                command_line << _printAddr(src);
            }
        } else
        {
            command_line << " -s " << _printSingleObjectNegation(srcrel);
            command_line << _printAddr(src);
        }
    }
    command_line << _printSrcService(srvrel);

    if (!dst->isAny()) 
    {
	command_line << " -d " << _printSingleObjectNegation(dstrel);
	command_line << _printAddr(dst);
    }
    command_line << _printDstService(srvrel);

/* keeping state does not apply to deny/reject 
   however some rules need state check even if action is Deny


   autoupgrade transformation 2.1.11 -> 2.1.12 adds rule option
   'stateless=True' for rules with action NOT 'Accept', 'Tag' or
   'Route'. No need to check action here, just rely on this option
   and internal flag 'force_state_check'  (05/07/07 --vk)
*/
    if (!ruleopt->getBool("stateless") || rule->getBool("force_state_check") )
    {
        /*
         * But not, when the line already contains a state matching
         */
        if (command_line.str().find("-m state --state", 0) == string::npos)
            command_line << " -m state --state NEW ";
    }

    command_line << _printTimeInterval(rule);

    command_line << _printModules(rule);
    command_line << _printTarget(rule);

    command_line << _endRuleLine();

//    command_line << endl;

    return command_line.str();
}

string PolicyCompiler_ipt::PrintRule::_declareTable()
{
    return "";
}

string PolicyCompiler_ipt::PrintRule::_flushAndSetDefaultPolicy()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    ostringstream res;

    if (!ipt_comp->ipv6)
    {
        res << "$IPTABLES -P OUTPUT  DROP" << endl;
        res << "$IPTABLES -P INPUT   DROP" << endl;
        res << "$IPTABLES -P FORWARD DROP" << endl;

        /*
         * need to flush all tables and chains before setting up any rules
         */
        res << "\n\
cat /proc/net/ip_tables_names | while read table; do\n\
  $IPTABLES -t $table -L -n | while read c chain rest; do\n\
      if test \"X$c\" = \"XChain\" ; then\n\
        $IPTABLES -t $table -F $chain\n\
      fi\n\
  done\n\
  $IPTABLES -t $table -X\n\
done\n";

        res << endl;
        res << endl;
    }

    if (ipt_comp->ipv6)
    {
        /*
         * test if ip6tables is installed and if it works. It may be installed
         * on the system but fail because ipv6 is not compiled into the
         * kernel.
         */
        res << "$IP6TABLES -P OUTPUT  DROP" << endl;
        res << "$IP6TABLES -P INPUT   DROP" << endl;
        res << "$IP6TABLES -P FORWARD DROP" << endl;

        res << "\n\
cat /proc/net/ip6_tables_names | while read table; do\n\
  $IP6TABLES -t $table -L -n | while read c chain rest; do\n\
      if test \"X$c\" = \"XChain\" ; then\n\
        $IP6TABLES -t $table -F $chain\n\
      fi\n\
  done\n\
  $IP6TABLES -t $table -X\n\
done\n";
        res << endl;
        res << endl;
    }
    return res.str();
}

string PolicyCompiler_ipt::PrintRule::_commit()
{
    return "";
}

string PolicyCompiler_ipt::PrintRule::_printOptionalGlobalRules()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    ostringstream res;
    bool isIPv6 = ipt_comp->ipv6;

/*
 * bug #1092141: "irritating FORWARD rule for established connections"
 * Need rules in FORWARD chain only if ip forwarding is on or set to
 * "no change"
 */
    bool ipforward = false;
    string s = compiler->getCachedFwOpt()->getStr("linux24_ip_forward");
    ipforward= (s.empty() || s=="1" || s=="On" || s=="on");

    if ( compiler->getCachedFwOpt()->getBool("clamp_mss_to_mtu") && ipforward)
    {
        res << _startRuleLine()
            << "FORWARD -p tcp -m tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu"
            << _endRuleLine();

        res << endl;
    }

    if ( compiler->getCachedFwOpt()->getBool("accept_established") &&
         ipt_comp->my_table=="filter") 
    {
        res << _startRuleLine()
            << "INPUT   -m state --state ESTABLISHED,RELATED -j ACCEPT"
            << _endRuleLine();

        res << _startRuleLine()
            << "OUTPUT  -m state --state ESTABLISHED,RELATED -j ACCEPT"
            << _endRuleLine();

        if (ipforward)
            res << _startRuleLine()
                << "FORWARD -m state --state ESTABLISHED,RELATED -j ACCEPT"
                << _endRuleLine();

        res << endl;
    }

/*
 * it helps to add backup ssh access rule as early as possible so that
 * ssh session opened from the management station won't break after
 * all chains are flushed. The installation process may stall if
 * stdout buffer gets filled with diagnostic or progress output from
 * this script printed after chains are flushed but before a rule
 * permitting ssh is installed. This may happen if script debugging is
 * on or there are many NAT rules (so it prints a lot of "Rule NN
 * (NAT)" lines).
 */
    if ( compiler->getCachedFwOpt()->getBool("mgmt_ssh") &&
         ! compiler->getCachedFwOpt()->getStr("mgmt_addr").empty() )
    {
        string addr_str = compiler->getCachedFwOpt()->getStr("mgmt_addr");
        InetAddrMask *inet_addr = NULL;
        bool addr_is_good = true;
        if (isIPv6)
        {
            // check if given address is ipv6
            try
            {
                inet_addr = new Inet6AddrMask(addr_str);
            } catch(const FWException &ex)  {
                // address does not parse as ipv6, skip this rule.
                addr_is_good = false;
            }
        } else
        {
            // check if given address parses as ipv4
            try
            {
                inet_addr = new InetAddrMask(addr_str);
            } catch(const FWException &ex)  {
                // address does not parse
                addr_is_good = false;
            }
        }
        if (addr_is_good)
        {
            res << "# backup ssh access" << endl;
            res << "#" << endl;
/* bug #1106701: 'backup ssh access' and statefulness interation
 * Need to add rules with ESTABLISHED and RELATED to make sure backup ssh access
 * works even when global rule that accepts ESTABLISHED and RELATED is disabled
 */
            res << _startRuleLine() << "INPUT  -p tcp -m tcp  -s "
                << inet_addr->toString()
                << "  --dport 22  -m state --state NEW,ESTABLISHED -j ACCEPT"
                << _endRuleLine();

            res << _startRuleLine() << "OUTPUT  -p tcp -m tcp  -d "
                << inet_addr->toString()
                << "  --sport 22  -m state --state ESTABLISHED,RELATED -j ACCEPT"
                << _endRuleLine();

            res << endl;
        }
    }

    if ( ! compiler->getCachedFwOpt()->getBool("accept_new_tcp_with_no_syn") ) 
    {
        res << "# drop TCP sessions opened prior firewall restart" 
            << endl;

        res << "#"    << endl;

        res << _startRuleLine()
            << "INPUT   -p tcp -m tcp ! --tcp-flags SYN,RST,ACK SYN -m state --state NEW -j DROP"
            << _endRuleLine();

        res << _startRuleLine()
            << "OUTPUT  -p tcp -m tcp ! --tcp-flags SYN,RST,ACK SYN -m state --state NEW -j DROP"
            << _endRuleLine();

        if (ipforward)
            res << _startRuleLine()
                << "FORWARD -p tcp -m tcp ! --tcp-flags SYN,RST,ACK SYN -m state --state NEW -j DROP"
                << _endRuleLine();

        res << endl;
    }

    if ( compiler->getCachedFwOpt()->getBool("drop_invalid") ) 
    {
        res << "# drop packets that do not match any valid state "
               << endl;
        res << "#"    << endl;

        if ( !compiler->getCachedFwOpt()->getBool("log_invalid"))
        {
            res << _startRuleLine()
                << "OUTPUT   -m state --state INVALID  -j DROP"
                << _endRuleLine();

            res << _startRuleLine()
                << "INPUT    -m state --state INVALID  -j DROP"
                << _endRuleLine();

            if (ipforward)
                res << _startRuleLine()
                    << "FORWARD  -m state --state INVALID  -j DROP"
                    << _endRuleLine();
        } else
        {
            res << _createChain("drop_invalid");

            res << _startRuleLine()
                << "OUTPUT   -m state --state INVALID  -j drop_invalid"
                << _endRuleLine();

            res << _startRuleLine()
                << "INPUT    -m state --state INVALID  -j drop_invalid"
                << _endRuleLine();

            if (ipforward)
                res << _startRuleLine()
                    << "FORWARD  -m state --state INVALID  -j drop_invalid"
                    << _endRuleLine();


            res << _startRuleLine();


	    if (compiler->getCachedFwOpt()->getBool("use_ULOG")) 
	    {  
	    	string s = compiler->getCachedFwOpt()->getStr("ulog_nlgroup");

                res  << "drop_invalid -j ULOG ";

        	if (!s.empty()) 
            	    res << "--ulog-nlgroup " << s << " ";

		int r;
        	if ((r = compiler->getCachedFwOpt()->getInt("ulog_cprange"))!=0)  
		     res << "--ulog-cprange " << r << " ";

        	if ((r = compiler->getCachedFwOpt()->getInt("ulog_qthreshold"))!=0)  
	  	     res << " --ulog-qthreshold " << r << " ";
               
	        res << "--ulog-prefix "; 

	    } else {
                   res << "drop_invalid -j LOG "
		       << "--log-level debug --log-prefix ";
            }

	    string s = compiler->getCachedFwOpt()->getStr("log_prefix");
	    if (s.empty())
	        s = "INVALID state -- DENY ";

	    res << _printLogPrefix("-1", "DENY","global","drop_invalid","BLOCK INVALID",s)
	        << _endRuleLine()
                << _startRuleLine() << "drop_invalid  -j DROP" << _endRuleLine();
 
	}
        res << endl;
    }

    return res.str();
}

string PolicyCompiler_ipt::PrintRule::_quote(const string &s)
{
    return "\"" + s + "\"";
}


