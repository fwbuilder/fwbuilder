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

#include "PolicyCompiler_ipt.h"
#include "OSConfigurator_linux24.h"

#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AttachedNetworks.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/UserService.h"

#include "fwbuilder/Inet6AddrMask.h"

#include "combinedAddress.h"

#include "Configlet.h"

#include <QStringList>
#include <QRegExp>
#include <QtDebug>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;



/**
 *-----------------------------------------------------------------------
 *                    Methods for printing
 */

string PolicyCompiler_ipt::PrintRule::_printSingleObjectNegation(
    RuleElement *rel)
{
    if (rel->getBool("single_object_negation"))   return "! ";
    else return "";
}

/*
 * Prints single --option with argument and negation "!"
 * taking into account the change that happened in iptables 1.4.3.1
 * that causes warning
 * Using intrapositioned negation (`--option ! this`) is deprecated in favor of extrapositioned (`! --option this`).
 */
string PolicyCompiler_ipt::PrintRule::_printSingleOptionWithNegation(
    const string &option, RuleElement *rel, const string &arg)
{
    ostringstream ostr;
    if (XMLTools::version_compare(version, "1.4.3")>=0)
    {
        ostr << _printSingleObjectNegation(rel);
        ostr << option << " ";
        ostr << arg << " ";
    } else
    {
        ostr << option << " ";
        ostr << _printSingleObjectNegation(rel);
        ostr << arg << " ";
    }
    return ostr.str();
}

void PolicyCompiler_ipt::PrintRule::initializeMinusNTracker()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    for (list<string>::const_iterator i =
             PolicyCompiler_ipt::getStandardChains().begin();
         i != PolicyCompiler_ipt::getStandardChains().end(); ++i)
    {
        (*(ipt_comp->minus_n_commands))[*i] = true;
    }
    minus_n_tracker_initialized = true;
}
            

/*
 *  check and create new chain if needed
 */
string PolicyCompiler_ipt::PrintRule::_createChain(const string &chain)
{
    string res;
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);

    if (!minus_n_tracker_initialized) initializeMinusNTracker();

    if ( ipt_comp->minus_n_commands->count(chain)==0 )
    {
        string opt_wait;

        if (XMLTools::version_compare(version, "1.4.20")>=0)
            opt_wait = "-w ";
        else
            opt_wait = "";

	res = string((ipt_comp->ipv6) ? "$IP6TABLES " : "$IPTABLES ") +
            opt_wait + "-N " + chain;
        if (ipt_comp->my_table != "filter") res += " -t " + ipt_comp->my_table;
        res += "\n";
	(*(ipt_comp->minus_n_commands))[chain] = true;
    }
    return res;
}

string PolicyCompiler_ipt::PrintRule::_startRuleLine()
{            
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    string res = (ipt_comp->ipv6) ? "$IP6TABLES " : "$IPTABLES ";
    string opt_wait;

    if (XMLTools::version_compare(version, "1.4.20")>=0)
        opt_wait = "-w ";
    else
        opt_wait = "";

    res += opt_wait;

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

    // TODO: convert this into virtual function PolicyCompiler_ipt::printComment()
    string rl=rule->getLabel();
    if (rl != current_rule_label)
    {
        if (!compiler->inSingleRuleCompileMode())
        {
            if (!nocomm)
            {
                res << "# " << endl;
                res << "# Rule " << rl << endl;
                res << "# " << endl;
            }
            res << "echo " << _quote(string("Rule ")+rl) << endl;
            res << "# " << endl;
        }

/* do not put comment in the script if it is intended for linksys */
        if (!nocomm || compiler->inSingleRuleCompileMode())
        {
            QStringList comm = QString(rule->getComment().c_str()).split("\n", QString::SkipEmptyParts);
            foreach(QString line, comm)
            {
                res << "# " << line.toStdString() << endl;
            }
            //res << "# " << endl;

            string err = compiler->getErrorsForRule(rule, "# ");
            if (!err.empty()) res << err << endl;
        }
    }

    current_rule_label = rl;

//    string err = rule->getCompilerMessage();
//    if (!err.empty()) res << "# " << err << endl;

    return res.str();
}




/**
 *-----------------------------------------------------------------------
 */
string PolicyCompiler_ipt::PrintRule::_printChain(PolicyRule *rule)
{
    string s = rule->getStr("ipt_chain");
    if (s.empty()) s = "UNKNOWN";
    // check chain name length per bug report #2507239
    if (s.length() > 30)
    {
        ostringstream str;
        str << "Chain name '" << s << "' ";
        str << "is longer than 30 characters. Rule " << rule->getLabel();
        compiler->abort(rule, str.str());
    }
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
        if (ruleopt!=nullptr && (lim=ruleopt->getInt("limit_value"))>0)
        {
            if (ruleopt->getBool("limit_value_not"))
                ostr << " -m limit \\! --limit " << lim;
            else
                ostr << " -m limit --limit " << lim;

            string ls=ruleopt->getStr("limit_suffix");
            if (!ls.empty()) ostr << ls;

            int lb=ruleopt->getInt("limit_burst");
            if (lb>0) ostr << " --limit-burst " << lb;
        }
    }

    if (ruleopt!=nullptr && (lim=ruleopt->getInt("connlimit_value"))>0)
    {
        if (ruleopt->getBool("connlimit_above_not"))
            ostr << " -m connlimit \\! --connlimit-above " << lim;
        else
            ostr << " -m connlimit --connlimit-above " << lim;

        int ml=ruleopt->getInt("connlimit_masklen");
        if (ml>0) ostr << " --connlimit-mask " << ml;
    }

    if (ruleopt!=nullptr && (lim=ruleopt->getInt("hashlimit_value"))>0)
    {
        string module_name = "hashlimit";
        if (ruleopt->getBool("hashlimit_dstlimit"))
            module_name = "dstlimit";

        ostr << " -m " << module_name << " --" << module_name << " " << lim;

        string ls = ruleopt->getStr("hashlimit_suffix");
        if (!ls.empty()) ostr << ls;
            
        int lb=ruleopt->getInt("hashlimit_burst");
        if (lb>0) ostr << " --" << module_name << "-burst " << lb;

        ls = ruleopt->getStr("hashlimit_mode");
        if (ls.empty())
        {
            /* syntax "--hashlimit-mode srcip,srcport " (i.e. with options
               separated by commas) tested with iptables 1.3.6
            */
            QStringList opts;
            bool f;

            f = ruleopt->getBool("hashlimit_mode_srcip");
            if (f) opts.push_back("srcip");

            f = ruleopt->getBool("hashlimit_mode_dstip");
            if (f) opts.push_back("dstip");

            f = ruleopt->getBool("hashlimit_mode_srcport");
            if (f) opts.push_back("srcport");

            f = ruleopt->getBool("hashlimit_mode_dstport");
            if (f) opts.push_back("dstport");

            if (!opts.isEmpty())
                ostr << " --" << module_name << "-mode " << opts.join(",").toStdString();
        } else 
            // hashlimit_mode is v2.1 option. In v3 we have options
            // hashlimit_mode_srcip
            // hashlimit_mode_dstip
            // hashlimit_mode_srcport
            // hashlimit_mode_dstport
            ostr << " --" << module_name << "-mode " << ls;

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
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    std::ostringstream ostr;

    string target=rule->getStr("ipt_target");
    if (target.empty()) target="UNKNOWN";

    FWOptions *ruleopt =rule->getOptionsObject();

    if (rule->getTagging())
    {
        ostr << " -j MARK";
        ostr << " --set-mark " << rule->getTagValue();
        return ostr.str();
    }

    if (rule->getClassification())
    {
        ostr << " -j CLASSIFY";
        ostr << " --set-class " << ruleopt->getStr("classify_str");
        return ostr.str();
    }

    if (rule->getRouting())
    {
        ostr << " -j ROUTE";

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

        return ostr.str();
    }

    if (target==".CUSTOM")
    {
        ostr << " " << ruleopt->getStr("custom_str");
        return ostr.str();
    }

    if (target==".CONTINUE") // not a real target !
        return ostr.str();


    if (compiler->fw->getStr("host_OS")=="linux317" &&
         compiler->getCachedFwOpt()->getBool("use_ULOG") &&
         target=="LOG") target="NFLOG";

    // there is no ULOG for ip6tables yet
    if (!ipt_comp->ipv6 && compiler->getCachedFwOpt()->getBool("use_ULOG") &&
         target=="LOG") target="ULOG";

    ostr << " -j " << target << " ";

    if (target=="REJECT")
      ostr << _printActionOnReject(rule);

    if (target=="LOG" || target=="ULOG" || target=="NFLOG")
        ostr << _printLogParameters(rule);

    if (target=="CONNMARK")
    {
        ostr << ruleopt->getStr("CONNMARK_arg");
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
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    QStringList res;

    if (rule->getStr(".iface") == "nil") return "";

    RuleElementItf *itfrel = rule->getItf();

    QString iface_name;
    FWObject *rule_iface_obj = nullptr;
    Interface *rule_iface = nullptr;

    if ( ! itfrel->isAny())
    {
        rule_iface_obj = FWObjectReference::getObject(itfrel->front());
        rule_iface = Interface::cast(rule_iface_obj);
        iface_name = rule_iface_obj->getName().c_str();
        if (iface_name.endsWith("*")) iface_name.replace("*", "+");
 
        if (rule_iface && rule_iface->isBridgePort() &&
            (version.empty() ||
             XMLTools::version_compare(version, "1.3.0")>=0))
        {
            /*
              http://www.netfilter.org/projects/iptables/files/changes-iptables-1.2.9.txt
              See SF bug #3439613
              https://sourceforge.net/tracker/index.php?func=detail&aid=3439613&group_id=5314&atid=1129518#
            
              physdev module does not allow --physdev-out for
              non-bridged traffic anymore. We should add
              --physdev-is-bridged to make sure this matches only
              bridged packets.

              Also, adding "-i" / "-o" clause to match parent bridge
              interface. This allows us to correctly match which
              bridge the packet comes through in configurations using
              wildcard bridge port interfaces. For example, when br0
              and br1 have "vnet+" bridge port interface, iptables can
              still correctly match which bridge the packet went
              through using "-o br0" or "-o br1" clause. This can be
              useful in installations with many bridged interfaces
              that get created and destroyed dynamically, e.g.  with
              virtual machines.

              However add "-i br0" / "-o br0" only when there is more
              than one bridge interface _and_ bridge port name ends with
              a wild card symbol "+"
            */

            QString parent_name = rule_iface->getParent()->getName().c_str();

            if (rule->getDirection()==PolicyRule::Inbound)
            {
                if (ipt_comp->bridge_count > 1 && iface_name.endsWith("+"))
                    res << "-i" << parent_name;
                res << "-m physdev --physdev-in"  << iface_name;
            }

            if (rule->getDirection()==PolicyRule::Outbound)
            {
                if (ipt_comp->bridge_count > 1 && iface_name.endsWith("+"))
                    res << "-o" << parent_name;
                res << "-m physdev --physdev-is-bridged --physdev-out" << iface_name;
            }
        } else
        {
            if (rule->getDirection()==PolicyRule::Inbound)   
                res << _printSingleOptionWithNegation(
                    "-i", itfrel, iface_name.toStdString()).c_str();

            if (rule->getDirection()==PolicyRule::Outbound)  
                res << _printSingleOptionWithNegation(
                    "-o", itfrel, iface_name.toStdString()).c_str();
        }

        res << "";
    }

    return res.join(" ").toStdString();
}

string PolicyCompiler_ipt::PrintRule::_printActionOnReject(PolicyRule *rule)
{
    std::ostringstream str;

    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);

//    RuleElementSrv *srvrel=rule->getSrv();
#ifndef NDEBUG
    Service *srv = compiler->getFirstSrv(rule);
    assert(srv);
#endif

    string s = ipt_comp->getActionOnReject(rule);
    if (!s.empty()) 
    {
        if (ipt_comp->isActionOnRejectTCPRST(rule))
            str << " --reject-with tcp-reset";

	if (s.find("ICMP")!=string::npos) 
        {
            if (ipt_comp->ipv6)
            {
                if (s.find("unreachable")!=string::npos) 
                {
                    if (s.find("net")!=string::npos ||
                        s.find("host")!=string::npos)
                        str << " --reject-with icmp6-addr-unreachable";
                    if (s.find("port")!=string::npos ||
                        s.find("proto")!=string::npos)
                        str << " --reject-with icmp6-port-unreachable";
                }
                if (s.find("prohibited")!=string::npos) 
                {
                    str << " --reject-with icmp6-adm-prohibited";
                }
            } else
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
                    if (XMLTools::version_compare(version, "1.2.9")>=0 &&
                        s.find("admin")!=string::npos)
                        str << " --reject-with icmp-admin-prohibited";
                }
            }
        }
    }
    str << " ";
    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printGlobalLogParameters()
{
    return _printLogParameters(nullptr);
}

string PolicyCompiler_ipt::PrintRule::_printLogPrefix(const string &rule_num,
						      const string &action,
						      const string &interf,
						      const string &chain,
                                                      const string &ruleset,
						      const string& ,
						      const string &prefix)
{
    string s = prefix;

/* deal with our logging macros:
 * %N - rule number  ('2', or '2/3' for rule in a branch)
 * %A - action
 * %I - interface name
 * %C - chain name
 * %R - ruleset name
 */
    string::size_type n;
    if ((n=s.find("%N"))!=string::npos ) 
    {
      s.replace(n, 2, rule_num);
    }
    if ((n=s.find("%A"))!=string::npos ) 
    {
      s.replace(n, 2, action);
    }
    if ((n=s.find("%I"))!=string::npos ) 
    {
      s.replace(n, 2, interf);
    }
    if ((n=s.find("%C"))!=string::npos ) 
    {
      s.replace(n, 2, chain);
    }
    if ((n=s.find("%R"))!=string::npos ) 
    {
      s.replace(n, 2, ruleset);
    }

    if (s.length()>29)
    {
        compiler->warning(
                "Log prefix has been truncated to 29 characters"); 
        s=s.substr(0,29);
    }

    return _quote( s );
}

string PolicyCompiler_ipt::PrintRule::_printLogPrefix(PolicyRule *rule,
                                                      const string &prefix)
{
    FWObject *ruleset = rule->getParent();

    QString action = QString(rule->getStr("stored_action").c_str()).toUpper();

    RuleElementItf *itf_re = rule->getItf(); assert(itf_re!=nullptr);
    FWObject *rule_iface = FWObjectReference::getObject(itf_re->front());
    string rule_iface_name =  rule_iface->getName();

    if (rule_iface_name=="")     rule_iface_name = "global";
    if (rule_iface_name=="Any")  rule_iface_name = "global";

    std::ostringstream s1;
    int pos = rule->getPosition();
    // parent_rule_num is set by processor "Branching" for branch rules
    string ppos = rule->getStr("parent_rule_num");

    if (ppos != "")
        s1 << ppos << "/";
    s1 << pos;

    return _printLogPrefix(s1.str(),
                           action.toStdString(),
                           rule_iface_name,
                           rule->getStr("ipt_chain"),
                           ruleset->getName(),
                           rule->getLabel(),
                           prefix);
}

string PolicyCompiler_ipt::PrintRule::_printLogParameters(PolicyRule *rule)
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    std::ostringstream str;
    string s;
    FWOptions *ruleopt = (rule!=nullptr) ? 
        rule->getOptionsObject() : compiler->getCachedFwOpt();

    bool use_nflog = (compiler->getCachedFwOpt()->getBool("use_ULOG") &&
                      compiler->fw->getStr("host_OS")=="linux317");

    // there is no ULOG for ip6tables yet
    bool use_ulog = (!ipt_comp->ipv6 &&
                     compiler->getCachedFwOpt()->getBool("use_ULOG"));

    if (use_nflog)
    {
        s=ruleopt->getStr("nflog_group");
        if (s.empty())  s=compiler->getCachedFwOpt()->getStr("ulog_nlgroup");
        if (!s.empty())
            str << " --nflog-group " << s;

        s=ruleopt->getStr("log_prefix");
        if (s.empty())  s=compiler->getCachedFwOpt()->getStr("log_prefix");
        if (!s.empty())
            str << " --nflog-prefix " << _printLogPrefix(rule, s);

        int r=compiler->getCachedFwOpt()->getInt("ulog_cprange");
        if (r!=0)  str << " --nflog-range " << r << " ";
        r=compiler->getCachedFwOpt()->getInt("ulog_qthreshold");
        if (r!=0)  str << " --nflog-threshold " << r << " ";
    } else if (use_ulog)
    {
        s=ruleopt->getStr("ulog_nlgroup");
        if (s.empty())  s=compiler->getCachedFwOpt()->getStr("ulog_nlgroup");
        if (!s.empty()) 
            str << " --ulog-nlgroup " << s;

        s=ruleopt->getStr("log_prefix");
        if (s.empty())  s=compiler->getCachedFwOpt()->getStr("log_prefix");
        if (!s.empty()) 
            str << " --ulog-prefix " << _printLogPrefix(rule, s);

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
            str << " --log-prefix " << _printLogPrefix(rule, s);

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

    if ( (ruleopt!=nullptr && (l=ruleopt->getInt("limit_value"))>0) || 
         (l=compopt->getInt("limit_value"))>0 ) 
    {
	str << "  -m limit --limit " << l;

        if (ruleopt!=nullptr) s=ruleopt->getStr("limit_suffix");
	if (s.empty()) 	   s=compopt->getStr("limit_suffix");
	if (!s.empty()) str << s;
        
        lb=-1;
	if (ruleopt!=nullptr) lb=ruleopt->getInt("limit_burst");
	if (lb<0)          lb=compopt->getInt("limit_burst");
	if (lb>0)          str << " --limit-burst " << lb;
    }

    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printProtocol(Service *srv)
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    string s;
    // CustomService returns protocol name starting with v3.0.4
    // However CustomService can return protocol name "any", which we should
    // just skip.
    if (CustomService::isA(srv))
    {
        // check if the code string for this custom service already includes
        // "-p proto" fragment
        string code = CustomService::cast(srv)->getCodeForPlatform(
            compiler->myPlatformName());
        std::size_t minus_p = code.find("-p ");
        if (minus_p != string::npos) return "";
        string pn = srv->getProtocolName();
        if (pn == "any") return "";
    }

    if (!srv->isAny() && !TagService::isA(srv) && !UserService::isA(srv))
    {
        string pn = srv->getProtocolName();
        if (pn=="ip" || pn=="any") pn = "all";

        if (ipt_comp->ipv6)
        { 
            if (ICMPService::isA(srv))
            {
                compiler->abort(
                    "Can not use ICMPService in ipv6 rule; "
                    "use ICMP6Service object instead");
            }

            if (ICMP6Service::isA(srv))
            {
                s = "-p ipv6-icmp ";
                if (srv->getInt("type")!=-1 &&
                    (version.empty() ||
                     XMLTools::version_compare(version, "1.3.0")>=0))
                    s += " -m icmp6";
            } else
            {
                // ip6tables issues warning for commands using "-p all"
                // Warning: never matched protocol: all. use exension match instead
                // Skip "-p all" if ipv6
                if (pn!="all") s = "-p " + pn + " ";
            }
        } else
        {
            if (ICMP6Service::isA(srv))
            {
                compiler->abort(
                        "Can not use ICMP6Service in ipv4 rule; "
                        "use ICMPService object instead");
            }

            if (ICMPService::isA(srv))
            {
                s = "-p icmp ";
                if (version.empty() ||
                    XMLTools::version_compare(version, "1.2.9")>=0)
                    s += " -m icmp ";
            } else
            {
                s = "-p " + pn + " ";
            }
        }
        if (pn == "tcp")  s += "-m tcp ";
        if (pn == "udp")  s += "-m udp ";
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
	int rs = TCPUDPService::cast(srv)->getSrcRangeStart();
	int re = TCPUDPService::cast(srv)->getSrcRangeEnd();
	str << _printPorts(rs,re);
    }
    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printDstPorts(Service *srv)
{
    std::ostringstream  str;
    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int rs = TCPUDPService::cast(srv)->getDstRangeStart();
	int re = TCPUDPService::cast(srv)->getDstRangeEnd();
	str << _printPorts(rs,re);
    }
    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printICMP(ICMPService *srv)
{
    std::ostringstream  str;
    if (ICMPService::cast(srv) && srv->getInt("type")!=-1)
    {
	str << srv->getStr("type");
	if (srv->getInt("code")!=-1) 
	    str << "/" << srv->getStr("code") << " ";
    }
    return str.str();
}

string PolicyCompiler_ipt::PrintRule::_printIP(IPService *srv, PolicyRule *rule)
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    std::ostringstream  str;
    if (srv->getBool("fragm") || srv->getBool("short_fragm"))
    {
        if (ipt_comp->ipv6) str << " -m frag --fragmore";
        else str << " -f ";
    }

    string tos = srv->getTOSCode();
    string dscp = srv->getDSCPCode();
    if (!tos.empty())
        str << " -m tos --tos " << tos;
    else
        if (!dscp.empty())
        {
            if (dscp.find("BE")==0 || 
                dscp.find("EF")==0 || 
                dscp.find("AF")==0 || 
                dscp.find("CS")==0)
                str << " -m dscp --dscp-class " << dscp;
            else
                str << " -m dscp --dscp " << dscp;
        }
        
    if  (srv->hasIpOptions())
    {
        if (!ipt_comp->ipv6)
        {
            str << " -m ipv4options ";

            if (version.empty() || XMLTools::version_compare(version, "1.4.3")<0)
            {
                // "old" ipv4options module
                if  (srv->getBool("any_opt")) str << " --any-opt";
                else
                {
                    if  (srv->getBool("lsrr")) str << " --lsrr";
                    if  (srv->getBool("ssrr")) str << " --ssrr";
                    if  (srv->getBool("rr")) str << " --rr";
                    if  (srv->getBool("ts")) str << " --ts";
                    if  (srv->getBool("rtralt")) str << " --ra";
                }
            } else
            {
                // "new" ipv4options module
                if  (srv->getBool("any_opt")) str << " --any";
                else
                {
                    QStringList options;
                    if  (srv->getBool("lsrr")) options << "lsrr";
                    if  (srv->getBool("ssrr")) options << "ssrr";
                    if  (srv->getBool("rr")) options << "record-route";
                    if  (srv->getBool("ts")) options << "timestamp";
                    if  (srv->getBool("rtralt")) options << "router-alert";
                    if (options.size() > 0)
                        str << " --flags " << options.join(",").toStdString();
                }
            }
        } else
        {
            compiler->abort(
                    rule, 
                    "IP options match is not supported for IPv6.");
        }
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
    if (o && FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

    Service *srv= Service::cast(o);


    if (rel->size()==1)
    {
	if (UDPService::isA(srv) || TCPService::isA(srv) ||
            TagService::isA(srv))
        {
	    string str=_printSrcPorts( srv );
	    if (! str.empty() ) 
            {
                ostr << _printSingleOptionWithNegation(" --sport", rel, str);
            }
	}
    } else
    {
/* use multiport */

	string str;
	bool  first=true;
	for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++)
        {
	    FWObject *o= *i;
	    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

	    Service *s=Service::cast( o );
	    assert(s);
	    if (UDPService::isA(srv) || TCPService::isA(srv))
            {
		if (!first) str+=",";
		str+= _printSrcPorts( s );
		if (!str.empty()) first=false;
	    }
	}
	if ( !str.empty() ) 
        {
            if (ipt_comp->newIptables(version))
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
    if (o && FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

    Service *srv= Service::cast(o);

    if (rel->size()==1) 
    {
	if (UDPService::isA(srv) || TCPService::isA(srv))
        {
	    string str=_printDstPorts( srv );
	    if (! str.empty() )
            {
                ostr << _printSingleOptionWithNegation(" --dport", rel, str);
            }
	}
	if (TCPService::isA(srv)) 
        {
	    string str=_printTCPFlags(TCPService::cast(srv));
	    if (!str.empty()) 
            {
                ostr << _printSingleOptionWithNegation("", rel, str);
            }
	}
	if (ICMPService::isA(srv) || ICMP6Service::isA(srv)) 
        {
            string icmp_type_str = 
                (ipt_comp->ipv6) ? " --icmpv6-type" : " --icmp-type";

	    string str = _printICMP( ICMPService::cast(srv) );
	    if (str.empty() ) 
            {
                // module icmp6 does not like "--icmp6-type any"
                if ((version.empty() ||
                     XMLTools::version_compare(version, "1.2.6")>0) &&
                    !ipt_comp->ipv6)
                    ostr << icmp_type_str << " any ";
            } else
            {
                ostr << _printSingleOptionWithNegation(icmp_type_str, rel, str);
            }
	}
	if (IPService::isA(srv)) 
        {
	    string str = _printIP(IPService::cast(srv), PolicyRule::cast(rel->getParent()));
	    if (! str.empty() ) 
            {
                ostr  << _printSingleObjectNegation(rel) << str << " ";
            }
	}
	if (CustomService::isA(srv)) 
        {
	    ostr << _printSingleObjectNegation(rel) << " "
                 << CustomService::cast(srv)->getCodeForPlatform( compiler->myPlatformName() ) << " ";
	}
        if (TagService::isA(srv))
        {
	    ostr << "-m mark "
                 << _printSingleObjectNegation(rel) 
                 << "--mark "
                 << TagService::constcast(srv)->getCode() << " ";
        }
        if (UserService::isA(srv))
        {
	    ostr << "-m owner "
                 << _printSingleObjectNegation(rel)
                 << "--uid-owner "
                 << UserService::cast(srv)->getUserId() << " ";
        }
    } else 
    {
/* use multiport */

	string str;
	for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) 
        {
	    FWObject *o= *i;
	    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

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
            if (ipt_comp->newIptables(version))
                ostr << " --dports ";
            else
                ostr << " --destination-port ";
	    ostr << str << " ";
	}
    }
    return ostr.str();
}

string PolicyCompiler_ipt::PrintRule::_printSrcAddr(RuleElement *rel, Address  *o)
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    string res;
    if (AddressRange::cast(o)!=nullptr)
    {
        AddressRange *ar = AddressRange::cast(o);
        const InetAddr &range_start = ar->getRangeStart();
        const InetAddr &range_end = ar->getRangeEnd();

        if (range_start != range_end)
        {
            if (!have_m_iprange) { res = "-m iprange "; have_m_iprange = true; }
            res += _printSingleObjectNegation(rel) + "--src-range ";
            res += range_start.toString() + "-" + range_end.toString() + " ";
        } else
            res += "-s " + range_start.toString() + " ";

        return res;
    }

    MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
    if (atrt!=nullptr && atrt->getSubstitutionTypeName()==AddressTable::TYPENAME &&
        ipt_comp->using_ipset)
    {
        return _printIpSetMatch(o, rel);
    }

    return _printSingleOptionWithNegation(" -s", rel, _printAddr(o));
}

string PolicyCompiler_ipt::PrintRule::_printDstAddr(RuleElement *rel, Address  *o)
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    string res;
    if (AddressRange::cast(o)!=nullptr)
    {
        AddressRange *ar = AddressRange::cast(o);
        const InetAddr &range_start = ar->getRangeStart();
        const InetAddr &range_end = ar->getRangeEnd();
        if (range_start != range_end)
        {
            if (!have_m_iprange) { res = "-m iprange "; have_m_iprange = true; }
            res += _printSingleObjectNegation(rel) + "--dst-range ";
            res += range_start.toString() + "-" + range_end.toString() + " ";
        } else
            res += "-d " + range_start.toString() + " ";

        return res;
    }

    MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
    if (atrt!=nullptr && atrt->getSubstitutionTypeName()==AddressTable::TYPENAME &&
        ipt_comp->using_ipset)
    {
        return _printIpSetMatch(o, rel);
    }

    return _printSingleOptionWithNegation(" -d", rel, _printAddr(o));
}

string PolicyCompiler_ipt::PrintRule::_printIpSetMatch(Address *o, RuleElement *rel)
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    string set_name =
        dynamic_cast<OSConfigurator_linux24*>(ipt_comp->osconfigurator)->normalizeSetName(o->getName());
    string suffix = "dst";
    if (RuleElementSrc::isA(rel)) suffix = "src";
    if (RuleElementDst::isA(rel)) suffix = "dst";

    string set_match_option;
    if (XMLTools::version_compare(version, "1.4.4")>=0)
        set_match_option = "--match-set";
    else
        set_match_option = "--set";

    string set_match = set_match_option + " " + set_name + " " + suffix;
    ostringstream ostr;
    ostr << "-m set " << _printSingleOptionWithNegation("", rel, set_match);
    return ostr.str();
}

string PolicyCompiler_ipt::PrintRule::_printAddr(Address  *o)
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    std::ostringstream  ostr;

    MultiAddressRunTime *atrt = MultiAddressRunTime::cast(o);
    if (atrt!=nullptr)
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

        if (atrt->getSubstitutionTypeName()==AttachedNetworks::TYPENAME)
        {
            ostr << "$i_" << atrt->getSourceName() << "_network";
            return ostr.str();
        }


        // at this time we only support two types of MultiAddress
        // objects: AddressTable and DNSName. Both should be converted
        // to MultiAddressRunTime at this point. If we get some other
        // kind of MultiAddressRunTime object, we do not know what to do
        // with it so we stop.
        assert(atrt==nullptr);
    }

    if (Interface::cast(o)!=nullptr)
    {
        Interface *iface=Interface::cast(o);
        if (iface->isDyn())
            ostr << "$" << ipt_comp->getInterfaceVarName(iface, ipt_comp->ipv6)
                 << " ";
        return ostr.str();
    }

    const InetAddr *addr = o->getAddressPtr();
    const InetAddr *mask = o->getNetmaskPtr();

    if (addr==nullptr)
    {
        compiler->warning(
            string("Empty inet address in object ") +
            o->getName() + "(" +
            FWObjectDatabase::getStringId(o->getId())
            + ")");
        return ostr.str();
    }

    // Note that mask can be nullptr, for example if o is AddressRange.
    if (addr->isAny() && (mask==nullptr || mask->isAny()))
    {
        ostr << "0/0 ";
    } else 
    {
        ostr << addr->toString();

        if (Interface::cast(o)==nullptr &&
            Address::cast(o)->dimension() > 1 &&
            !mask->isHostMask())
        {
            ostr << "/" << mask->getLength();
        }
        ostr << " ";
    }
    return ostr.str();
}


string PolicyCompiler_ipt::PrintRule::_printTimeInterval(PolicyRule *r)
{
    std::ostringstream  ostr;

    RuleElementInterval* ri=r->getWhen();
    if (ri==nullptr || ri->isAny()) return "";

    std::map<int,std::string>  daysofweek;

    daysofweek[0]="Sun";
    daysofweek[1]="Mon";
    daysofweek[2]="Tue";
    daysofweek[3]="Wed";
    daysofweek[4]="Thu";
    daysofweek[5]="Fri";
    daysofweek[6]="Sat";
    daysofweek[7]="Sun";

    int smin, shour, sday, smonth, syear, sdayofweek;
    int emin, ehour, eday, emonth, eyear, edayofweek;
    string days_of_week;

    Interval *interval = compiler->getFirstWhen(r);
    assert(interval!=nullptr);

    interval->getStartTime( &smin, &shour, &sday, &smonth, &syear, &sdayofweek);
    interval->getEndTime(   &emin, &ehour, &eday, &emonth, &eyear, &edayofweek);
    days_of_week = interval->getDaysOfWeek();

    if (shour<0) shour=0;
    if (smin<0)  smin=0;

    if (ehour<0) ehour=23;
    if (emin<0)  emin=59;

    ostr << "-m time ";

    bool use_timestart_timestop = true;

    if (XMLTools::version_compare(version, "1.4.0")>=0)
    {
        /* in 1.4.0 date format has changed, it is now ISO 8601
         * http://www.w3.org/TR/NOTE-datetime
         *
         * --datestart YYYY[-MM[-DD[Thh[:mm[:ss]]]]]
         *
         * --datestop YYYY[-MM[-DD[Thh[:mm[:ss]]]]]
         */
        if (sday>0 && smonth>0 && syear>0)
        {
            ostr << "--datestart " 
                 << setw(2) << setfill('0') << syear << "-" 
                 << setw(2) << setfill('0') << smonth << "-" 
                 << setw(2) << setfill('0') << sday  << "T"
                 << setw(2) << setfill('0') << shour << ":"
                 << setw(2) << setfill('0') << smin << ":00 ";
            use_timestart_timestop = false;
        }

        if (eday>0 && emonth>0 && eyear>0)
        {
            ostr << "--datestop "
                 << setw(2) << setfill('0') << eyear << "-"
                 << setw(2) << setfill('0') << emonth << "-"
                 << setw(2) << setfill('0') << eday  << "T"
                 << setw(2) << setfill('0') << ehour << ":"
                 << setw(2) << setfill('0') << emin << ":00 ";
            use_timestart_timestop = false;
        }

        if (use_timestart_timestop)
        {
            ostr << " --timestart "
                 << setw(2) << setfill('0') << shour << ":"
                 << setw(2) << setfill('0') << smin  << " ";

            ostr << " --timestop "
                 << setw(2) << setfill('0') << ehour << ":"
                 << setw(2) << setfill('0') << emin  << " ";
        }

        if (!days_of_week.empty() && days_of_week != "0,1,2,3,4,5,6")
        {
            ostr << " --weekdays ";
            istringstream istr(days_of_week);
            bool first= true;
            while (!istr.eof())
            {
                if (!first) ostr << ',';
                first = false;
                int d;
                istr >> d;
                ostr << daysofweek[d];
                char sep;
                istr >> sep;
            }
        }

        if ( (XMLTools::version_compare(version, "1.4.11") >=0 ) && compiler->getCachedFwOpt()->getBool("use_kerneltz")) {
            ostr << " --kerneltz";
        }

    } else
    {
        /* "old" iptables time module

TIME v1.2.11 options:
 --timestart value --timestop value --days listofdays
          timestart value : HH:MM
          timestop  value : HH:MM
          listofdays value: a list of days to apply -> ie. Mon,Tue,Wed,Thu,Fri.
          Case sensitive

         */

        ostr << " --timestart "
             << setw(2) << setfill('0') << shour << ":"
             << setw(2) << setfill('0') << smin  << " ";

        ostr << " --timestop "
             << setw(2) << setfill('0') << ehour << ":"
             << setw(2) << setfill('0') << emin  << " ";

        if (!days_of_week.empty() && days_of_week != "0,1,2,3,4,5,6")
        {
            ostr << " --days ";
            istringstream istr(days_of_week);
            bool first= true;
            while (!istr.eof())
            {
                if (!first) ostr << ',';
                first = false;
                int d;
                istr >> d;
                ostr << daysofweek[d];
                char sep;
                istr >> sep;
            }
        }

    }
    return ostr.str();
}

PolicyCompiler_ipt::PrintRule::PrintRule(const std::string &name) :
    PolicyRuleProcessor(name) 
{ 
    init = true; 
    print_once_on_top = true;
    // use delayed initialization for ipt_comp->minus_n_commands
    // because it requires pointer to the compiler which has not been
    // initialized yet when this constructor is executed.
    minus_n_tracker_initialized = false;
}

/*
 * Initialize some internal variables. Need to do this in a separate
 * method because pointer to the compiler object is set by
 * RuleProcessor::setContext and is not available in constructor.
 */
void PolicyCompiler_ipt::PrintRule::initialize()
{
    // retrieve and save version for _printSingleOptionWithNegation and others
    version = compiler->fw->getStr("version");
}

bool  PolicyCompiler_ipt::PrintRule::processNext()
{
    PolicyCompiler_ipt *ipt_comp=dynamic_cast<PolicyCompiler_ipt*>(compiler);
    PolicyRule         *rule    =getNext(); 
    if (rule==nullptr) return false;

    string chain = rule->getStr("ipt_chain");
    if (ipt_comp->chain_usage_counter[chain] > 0)
    {
        tmp_queue.push_back(rule);

        compiler->output << _printRuleLabel(rule);
        compiler->output << _createChain(rule->getStr("ipt_chain"));

        string target = rule->getStr("ipt_target");
        if (target[0] != '.') compiler->output << _createChain(target);

        compiler->output 
            << dynamic_cast<OSConfigurator_linux24*>(
                compiler->osconfigurator)->printRunTimeWrappers(
                    rule, PolicyRuleToString(rule), ipt_comp->ipv6);
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
    if(src==nullptr)
        compiler->abort(rule, string("Broken SRC in ") + rule->getLabel());

    RuleElementDst *dstrel=rule->getDst();
    ref=dstrel->front();
    Address        *dst=Address::cast(FWReference::cast(ref)->getPointer());
    if(dst==nullptr)
        compiler->abort(rule, string("Broken DST in ") + rule->getLabel());

    RuleElementSrv *srvrel=rule->getSrv();
    ref=srvrel->front();
    Service        *srv=Service::cast(FWReference::cast(ref)->getPointer());
    if(srv==nullptr)
        compiler->abort(rule, string("Broken SRV in ") + rule->getLabel());


    std::ostringstream  command_line;

    have_m_iprange = false;

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
                    compiler->warning(
                        rule, 
                        "Empty MAC address in rule");
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
                command_line << " -m mac";
                command_line << _printSingleOptionWithNegation(" --mac-source",
                                                               srcrel,
                                                               physaddress);
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
                command_line << _printSrcAddr(srcrel, src);

        } else
            command_line << _printSrcAddr(srcrel, src);

    }
    command_line << _printSrcService(srvrel);

    if (!dst->isAny()) 
	command_line << _printDstAddr(dstrel, dst);

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
        string state_module_option;
        /*
         * But not, when the line already contains a state matching
         */
        if (XMLTools::version_compare(version, "1.4.4")>=0)
            state_module_option = "conntrack --ctstate";
        else
            state_module_option = "state --state";

        if (command_line.str().find("-m " + state_module_option, 0) == string::npos)
            command_line << " -m " << state_module_option << " NEW ";
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

string PolicyCompiler_ipt::PrintRule::_commit()
{
    return "";
}

string PolicyCompiler_ipt::PrintRule::_clampTcpToMssRule()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    ostringstream res;

    if ( compiler->getCachedFwOpt()->getBool("clamp_mss_to_mtu"))
    {
        bool ipforw;
        if (ipt_comp->ipv6)
        {
            string s = compiler->getCachedFwOpt()->getStr("linux24_ipv6_forward");
            ipforw = (s.empty() || s=="1" || s=="On" || s=="on");
            // bug #2477775: target TCPMSS is not available in ip6tables
            // before 1.4.0 In fact I am not sure of the minimal required
            // version. According to the netfilter git log, it was added in
            // 1.3.8
            if (XMLTools::version_compare(version, "1.3.8")<0)
            {
                if (ipforw)
                {
                    res << "target TCPMSS is not supported by ip6tables before v1.3.8";
                    compiler->warning(res.str());
                    return "# " + res.str() + "\n\n";
                } else return "";
            }
        } else
        {
            string s = compiler->getCachedFwOpt()->getStr("linux24_ip_forward");
            ipforw = (s.empty() || s=="1" || s=="On" || s=="on");
        }

        if (ipforw)
        {
            res << _startRuleLine()
                << "FORWARD -p tcp -m tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu"
                << _endRuleLine();
            res << endl;
        }
    }
    return res.str();
}

string PolicyCompiler_ipt::PrintRule::_printOptionalGlobalRules()
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    ostringstream res;
    bool isIPv6 = ipt_comp->ipv6;
    string state_module_option;

    string s = compiler->getCachedFwOpt()->getStr("linux24_ip_forward");
    bool ipforward= (s.empty() || s=="1" || s=="On" || s=="on");
    s = compiler->getCachedFwOpt()->getStr("linux24_ipv6_forward");
    bool ip6forward= (s.empty() || s=="1" || s=="On" || s=="on");
    bool ipforw = ((!ipt_comp->ipv6 && ipforward) ||
                   (ipt_comp->ipv6 && ip6forward));

    Configlet configlet(compiler->fw, "linux24", "automatic_rules");
    configlet.removeComments();
    configlet.collapseEmptyStrings(true);

    configlet.setVariable("begin_rule", _startRuleLine().c_str());
    configlet.setVariable("end_rule", _endRuleLine().c_str());

    configlet.setVariable("ipforw", ipforw);
                              
    configlet.setVariable("accept_established", 
                          compiler->getCachedFwOpt()->getBool("accept_established") &&
                          ipt_comp->my_table=="filter");

    if (XMLTools::version_compare(version, "1.4.4")>=0)
        state_module_option = "conntrack --ctstate";
    else
        state_module_option = "state --state";

    configlet.setVariable("state_module_option", state_module_option.c_str());

    list<FWObject*> ll = compiler->fw->getByTypeDeep(Interface::TYPENAME);
    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        Interface *intf = Interface::cast( *i );
        if (intf->isManagement())
        {
            configlet.setVariable("management_interface", intf->getName().c_str());
            break;
        }
    }

    _printBackupSSHAccessRules(&configlet);

    configlet.setVariable(
        "drop_new_tcp_with_no_syn",
        ! compiler->getCachedFwOpt()->getBool("accept_new_tcp_with_no_syn")); 

    configlet.setVariable(
        "bridging_firewall",
        compiler->getCachedFwOpt()->getBool("bridging_fw"));

    configlet.setVariable(
        "add_rules_for_ipv6_neighbor_discovery",
        isIPv6 && 
        compiler->getCachedFwOpt()->getBool("add_rules_for_ipv6_neighbor_discovery")); 


    configlet.setVariable("drop_invalid",
                          compiler->getCachedFwOpt()->getBool("drop_invalid") &&
                          !compiler->getCachedFwOpt()->getBool("log_invalid"));

    configlet.setVariable("drop_invalid_and_log",
                          compiler->getCachedFwOpt()->getBool("drop_invalid") &&
                          compiler->getCachedFwOpt()->getBool("log_invalid"));

    configlet.setVariable("create_drop_invalid_chain",
                          _createChain("drop_invalid").c_str());


    if (compiler->getCachedFwOpt()->getBool("log_invalid") &&
        !isIPv6 &&
        compiler->getCachedFwOpt()->getBool("use_ULOG"))
    {  
        configlet.setVariable("use_ulog", 1);

        string s = compiler->getCachedFwOpt()->getStr("ulog_nlgroup");
        configlet.setVariable("use_nlgroup", !s.empty());
        configlet.setVariable("nlgroup", s.c_str());

        int r = compiler->getCachedFwOpt()->getInt("ulog_cprange");
        configlet.setVariable("use_cprange", r!=0);
        configlet.setVariable("cprange", r);

        r = compiler->getCachedFwOpt()->getInt("ulog_qthreshold");
        configlet.setVariable("use_qthreshold", r!=0);
        configlet.setVariable("qthreshold", r);
    } else
        configlet.setVariable("not_use_ulog", 1);

    configlet.setVariable("invalid_match_log_prefix",
                          _printLogPrefix("-1",
                                          "DENY",
                                          "global",
                                          "drop_invalid",
                                          "Policy",
                                          "BLOCK INVALID",
                                          "INVALID state -- DENY ").c_str());

    return configlet.expand().toStdString();
}

void PolicyCompiler_ipt::PrintRule::_printBackupSSHAccessRules(Configlet *conf)
{
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);
    bool isIPv6 = ipt_comp->ipv6;
    if ( compiler->getCachedFwOpt()->getBool("mgmt_ssh") &&
         ! compiler->getCachedFwOpt()->getStr("mgmt_addr").empty() )
    {
        string addr_str = compiler->getCachedFwOpt()->getStr("mgmt_addr");
        InetAddrMask *inet_addr = nullptr;
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
                QString err("Backup ssh access rule could not be added "
                            "to IPv6 policy because specified address "
                            "'%1' is invalid");
                compiler->warning(err.arg(addr_str.c_str()).toStdString());
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
                QString err("Backup ssh access rule could not be added "
                            "to IPv4 policy because specified address "
                            "'%1' is invalid");
                compiler->warning(err.arg(addr_str.c_str()).toStdString());
            }
        }
        if (addr_is_good)
        {
            conf->setVariable("begin_rule", _startRuleLine().c_str());
            conf->setVariable("end_rule", _endRuleLine().c_str());
            conf->setVariable("mgmt_access", 1);
            conf->setVariable("ssh_management_address",
                              inet_addr->toString().c_str());
        }
    }
}

string PolicyCompiler_ipt::PrintRule::_quote(const string &s)
{
    return "\"" + s + "\"";
}


