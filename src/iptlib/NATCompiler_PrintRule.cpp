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

#include "NATCompiler_ipt.h"
#include "OSConfigurator_linux24.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/XMLTools.h"

#include "combinedAddress.h"

#include <assert.h>

#include <QString>
#include <QStringList>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;



/**
 *-----------------------------------------------------------------------
 *                    Methods for printing
 */

/*
 * Prints single --option with argument and negation "!"
 * taking into account the change that happened in iptables 1.4.3.1
 * that causes warning
 * Using intrapositioned negation (`--option ! this`) is deprecated in favor of extrapositioned (`! --option this`).
 */
string NATCompiler_ipt::PrintRule::_printSingleOptionWithNegation(
    const string &option, RuleElement *rel, const string &arg)
{
    ostringstream ostr;
    if (XMLTools::version_compare(version, "1.4.3")>=0)
    {
        ostr  << _printSingleObjectNegation(rel);
        ostr << option << " ";
        ostr << arg << " ";
    } else
    {
        ostr << option << " ";
        ostr  << _printSingleObjectNegation(rel);
        ostr << arg << " ";
    }
    return ostr.str();
}

void NATCompiler_ipt::PrintRule::initializeMinusNTracker()
{
    NATCompiler_ipt *ipt_comp = dynamic_cast<NATCompiler_ipt*>(compiler);
    for (list<string>::const_iterator i =
             NATCompiler_ipt::getStandardChains().begin();
         i != NATCompiler_ipt::getStandardChains().end(); ++i)
    {
        (*(ipt_comp->minus_n_commands))[*i] = true;
    }
    minus_n_tracker_initialized = true;
}

/*
 * Initialize some internal variables. Need to do this in a separate
 * method because pointer to the compiler object is set by
 * RuleProcessor::setContext and is not available in constructor.
 */
void NATCompiler_ipt::PrintRule::initialize()
{
    // retrieve and save version for _printSingleOptionWithNegation and others
    version = compiler->fw->getStr("version");
}

/*
 *  check and create new chain if needed
 */
string NATCompiler_ipt::PrintRule::_createChain(const string &chain)
{
    NATCompiler_ipt *ipt_comp = dynamic_cast<NATCompiler_ipt*>(compiler);
    ostringstream  res;

    if (!minus_n_tracker_initialized) initializeMinusNTracker();

    if ( ipt_comp->minus_n_commands->count(chain)==0 )
    {
        string opt_wait;

        if (XMLTools::version_compare(version, "1.4.20")>=0)
            opt_wait = "-w ";
        else
            opt_wait = "";

        string ipt_cmd = (ipt_comp->ipv6) ? "$IP6TABLES " : "$IPTABLES ";
	res << ipt_cmd << opt_wait << "-t nat -N " << chain << endl;
	(*(ipt_comp->minus_n_commands))[chain] = true;
    }
    return res.str();
}

string NATCompiler_ipt::PrintRule::_startRuleLine()
{
    NATCompiler_ipt *ipt_comp = dynamic_cast<NATCompiler_ipt*>(compiler);
    string res = (ipt_comp->ipv6) ? "$IP6TABLES " : "$IPTABLES ";

    string opt_wait;

    if (XMLTools::version_compare(version, "1.4.20")>=0)
        opt_wait = "-w ";
    else
        opt_wait = "";

    return res + opt_wait + string("-t nat -A ");
}

string NATCompiler_ipt::PrintRule::_endRuleLine()
{
    return string("\n");
}

string NATCompiler_ipt::PrintRule::_printRuleLabel(NATRule *rule)
{
    ostringstream res;

    bool nocomm =
        Resources::os_res[compiler->fw->getStr("host_OS")]->Resources::getResourceBool("/FWBuilderResources/Target/options/suppress_comments");

    // TODO: convert this into virtual function NATCompiler_ipt::printComment()
    string rl=rule->getLabel();
    if (rl != current_rule_label)
    {
        if (!compiler->inSingleRuleCompileMode() && !nocomm)
        {
            res << "# " << endl;
            res << "# Rule " << rl << endl;
            res << "# " << endl;
        }
        res << "echo " << _quote(string("Rule ")+rl) << endl;
        res << "# " << endl;

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
        current_rule_label=rl;
    }

//    string err = rule->getCompilerMessage();
//    if (!err.empty()) res << "# " << err << endl;

    return res.str();
}

/*
 *  check and create new chain if needed
 */

QString NATCompiler_ipt::PrintRule::getInterfaceName(RuleElement *itf_re)
{
    if (itf_re->isAny()) return "";
    FWObject *iface = FWObjectReference::getObject(itf_re->front());
    QString iface_name = iface->getName().c_str();
    if (iface_name.endsWith("*")) iface_name.replace("*", "+");
    return iface_name;
}

/**
 *-----------------------------------------------------------------------
 */
string NATCompiler_ipt::PrintRule::_printChainDirectionAndInterface(NATRule *rule)
{
    QStringList res;

    RuleElementItfInb *itf_in_re = rule->getItfInb(); assert(itf_in_re!=nullptr);
    RuleElementItfOutb *itf_out_re = rule->getItfOutb(); assert(itf_out_re!=nullptr);

    QString iface_in_name = getInterfaceName(itf_in_re);
    QString iface_out_name = getInterfaceName(itf_out_re);

    if (rule->getStr(".iface_in") == "nil") iface_in_name = "";
    if (rule->getStr(".iface_out") == "nil") iface_out_name = "";

    res << rule->getStr("ipt_chain").c_str();

    if ( ! iface_in_name.isEmpty())
    {
        res << _printSingleOptionWithNegation(
            "-i", itf_in_re, iface_in_name.toStdString()).c_str();
    }

    if ( ! iface_out_name.isEmpty())
    {
        res << _printSingleOptionWithNegation(
            "-o", itf_out_re, iface_out_name.toStdString()).c_str();
    }

    res << "";

    return res.join(" ").toStdString();
}

string NATCompiler_ipt::PrintRule::_printProtocol(Service *srv)
{
    std::ostringstream  ostr;
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
        if (pn=="ip") pn="all";
        ostr << "-p " <<  pn << " ";
        if (pn == "tcp")  ostr << "-m tcp ";
        if (pn == "udp")  ostr << "-m udp ";
        if (pn == "icmp") ostr << "-m icmp ";
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printMultiport(NATRule *rule)
{
    std::ostringstream  ostr;
    if( rule->getBool("ipt_multiport"))
	ostr << "-m multiport ";
    return ostr.str();
}


string NATCompiler_ipt::PrintRule::_printOPorts(int rs,int re)
{
    std::ostringstream  ostr;

    if (rs<0) rs=0;
    if (re<0) re=0;

    if (rs>0 || re>0) {
	if (rs==re)  ostr << rs;
	else
	    if (rs==0 && re!=0)      ostr << ":" << re;
	    else
                ostr << rs << ":" << re;
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printTPorts(int rs,int re)
{
    std::ostringstream  ostr;

    if (rs<0) rs=0;
    if (re<0) re=0;

    if (rs>0 || re>0) {
	if (rs==re)  ostr << rs;
	else
	    if (rs==0 && re!=0)      ostr << "-" << re;
	    else
                ostr << rs << "-" << re;
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printICMP(ICMPService *srv)
{
    std::ostringstream  ostr;
    if (ICMPService::isA(srv) && srv->getInt("type")!=-1) {
	ostr << srv->getStr("type");
	if (srv->getInt("code")!=-1)
	    ostr << "/" << srv->getStr("code") << " ";
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printIP(IPService *srv)
{
    std::ostringstream  ostr;
    if (IPService::isA(srv) ) {
	if (srv->getBool("fragm") || srv->getBool("short_fragm"))
	    ostr << "-f ";

        if  (srv->getBool("lsrr") ||
             srv->getBool("ssrr") ||
             srv->getBool("rr") ||
             srv->getBool("ts") ) ostr << " -m ipv4options ";

        if  (srv->getBool("lsrr")) ostr << " --lsrr";
        if  (srv->getBool("ssrr")) ostr << " --ssrr";
        if  (srv->getBool("rr"))   ostr << " --rr";
        if  (srv->getBool("ts"))   ostr << " --ts";
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printSrcPorts(Service *srv)
{
    std::ostringstream  ostr;
    if (TCPService::isA(srv) || UDPService::isA(srv)) {
	int rs=TCPUDPService::cast(srv)->getSrcRangeStart();
	int re=TCPUDPService::cast(srv)->getSrcRangeEnd();
	ostr << _printOPorts(rs,re);
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printDstPorts(Service *srv)
{
    std::ostringstream  ostr;
    if (TCPService::isA(srv) || UDPService::isA(srv)) {
	int rs=TCPUDPService::cast(srv)->getDstRangeStart();
	int re=TCPUDPService::cast(srv)->getDstRangeEnd();
	ostr << _printOPorts(rs,re);
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printSNATPorts(Service *srv)
{
    std::ostringstream  ostr;
    if (TCPService::isA(srv) || UDPService::isA(srv)) {
	int rs=TCPUDPService::cast(srv)->getSrcRangeStart();
	int re=TCPUDPService::cast(srv)->getSrcRangeEnd();
	ostr << _printTPorts(rs,re);
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printDNATPorts(Service *srv)
{
    std::ostringstream  ostr;
    if (TCPService::isA(srv) || UDPService::isA(srv)) {
	int rs=TCPUDPService::cast(srv)->getDstRangeStart();
	int re=TCPUDPService::cast(srv)->getDstRangeEnd();
	ostr << _printTPorts(rs,re);
    }
    return ostr.str();
}

/*
 * we made sure that all services in rel  represent the same protocol
 */
string NATCompiler_ipt::PrintRule::_printSrcService(RuleElementOSrv  *rel)
{
    std::ostringstream  ostr;

/* I do not want to use rel->getFirst because it traverses the tree to
 * find the object. I'd rather use a cached copy in the compiler
 */
    FWObject *o=rel->front();
    if (o && FWReference::cast(o)!=nullptr)
        o=FWReference::cast(o)->getPointer();

    Service *srv= Service::cast(o);

    if (rel->size()==1) {
	if (UDPService::isA(srv) || TCPService::isA(srv)) {
	    string str=_printSrcPorts( srv );
	    if (! str.empty() ) ostr << "--sport " << str << " ";
	}
    } else {
/* use multiport */

	string str;
	bool  first=true;
	for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++) {
	    FWObject *o= *i;
	    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

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
            if (version.empty() || version=="ge_1.2.6" ||
                XMLTools::version_compare(version, "1.2.6")>=0)
                ostr << "--sports ";
            else
                ostr << "--source-port ";
	    ostr << str << " ";
	}
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printDstService(RuleElementOSrv  *rel)
{
    std::ostringstream  ostr;

    FWObject *o=rel->front();
    if (o && FWReference::cast(o)!=nullptr)
        o=FWReference::cast(o)->getPointer();

    Service *srv= Service::cast(o);

    if (rel->size()==1)
    {
	if (UDPService::isA(srv) || TCPService::isA(srv))
        {
	    string str=_printDstPorts( srv );
	    if (! str.empty() ) ostr << "--dport " << str << " ";
	}
	if (ICMPService::isA(srv))
        {
	    string str=_printICMP( ICMPService::cast(srv) );
	    if (! str.empty() ) ostr << "--icmp-type " << str << " ";
	}
	if (IPService::isA(srv))
        {
	    string str=_printIP( IPService::cast(srv) );
	    if (! str.empty() ) ostr << str << " ";
	}
	if (CustomService::isA(srv))
        {
	    ostr << CustomService::cast(srv)->getCodeForPlatform( compiler->myPlatformName() ) << " ";
	}
        if (TagService::isA(srv))
        {
	    ostr << "-m mark --mark "
                 << TagService::constcast(srv)->getCode() << " ";
        }

    } else {
/* use multiport */

	string str;
	bool   first=true;
	for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++)
        {
	    FWObject *o= *i;
	    if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

	    Service *s=Service::cast( o );
	    assert(s);
	    if (UDPService::isA(srv) || TCPService::isA(srv))
            {
		if (!first) str+=",";
		str+= _printDstPorts( s );
		if (!str.empty()) first=false;
	    }
	}
	if ( !str.empty() )
        {
            if (version.empty() || version=="ge_1.2.6" ||
                XMLTools::version_compare(version, "1.2.6")>=0)
                ostr << "--dports ";
            else
                ostr << "--destination-port ";
	    ostr << str << " ";
	}
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printIpSetMatch(Address  *o, RuleElement *rel)
{
    NATCompiler_ipt *ipt_comp=dynamic_cast<NATCompiler_ipt*>(compiler);
    string set_name =
        dynamic_cast<OSConfigurator_linux24*>(ipt_comp->osconfigurator)->normalizeSetName(o->getName());
    string suffix = "dst";
    if (RuleElementOSrc::isA(rel)) suffix = "src";
    if (RuleElementODst::isA(rel)) suffix = "dst";

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

// Note print_mask is true by default, print_range is false by default.
string NATCompiler_ipt::PrintRule::_printAddr(Address  *o,
                                              bool ,
                                              bool print_range)
{
    NATCompiler_ipt *ipt_comp=dynamic_cast<NATCompiler_ipt*>(compiler);
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
        // at this time we only support two types of MultiAddress
        // objects: AddressTable and DNSName. Both should be converted
        // to MultiAddressRunTime at this point. If we get some other
        // kind of MultiAddressRunTime object, we do not know what to do
        // with it so we stop.
        assert(atrt==nullptr);
    }

    if (print_range && AddressRange::cast(o)!=nullptr)
    {
	InetAddr a1 = AddressRange::cast(o)->getRangeStart();
	InetAddr a2 = AddressRange::cast(o)->getRangeEnd();
	ostr << a1.toString() << "-" << a2.toString();
    } else
    {
        Interface *iface = Interface::cast(o);
        if (iface!=nullptr && iface->isDyn() && iface->getBool("use_var_address"))
        {
            ostr << "$" << ipt_comp->getInterfaceVarName(iface, ipt_comp->ipv6)
                 << " ";
            return ostr.str();
        }

        const InetAddr* addr = o->getAddressPtr();
        const InetAddr* mask = o->getNetmaskPtr();

        if (addr==nullptr)
        {
            compiler->warning(
                    string("Empty inet address in object ") +
                    FWObjectDatabase::getStringId(o->getId()));
            return ostr.str();
        }

        if (addr->isAny() && mask->isAny())
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
            // do not add space after address because there might be
            // :port spec coming right after it.
        }
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printSingleObjectNegation(
    RuleElement *rel)
{
    if (rel->getBool("single_object_negation"))   return "! ";
    else return "";
}

NATCompiler_ipt::PrintRule::PrintRule(const std::string &name) :
    NATRuleProcessor(name)
{
    init = true;
    print_once_on_top = true;
    minus_n_tracker_initialized = false;
}

bool NATCompiler_ipt::PrintRule::processNext()
{
    NATCompiler_ipt *ipt_comp = dynamic_cast<NATCompiler_ipt*>(compiler);
    NATRule *rule=getNext();
    if (rule==nullptr) return false;

    FWOptions *ropt = rule->getOptionsObject();

    string chain = rule->getStr("ipt_chain");
    if (ipt_comp->chain_usage_counter[chain] == 0)
    {
        return true;
    }

    tmp_queue.push_back(rule);

    compiler->output << _printRuleLabel(rule);

    string s;
    std::ostringstream  cmdout;

    compiler->output << _createChain(rule->getStr("ipt_chain"));
    compiler->output << _createChain(rule->getStr("ipt_target"));


    RuleElementOSrc *osrcrel = rule->getOSrc();
    Address  *osrc = compiler->getFirstOSrc(rule);  assert(osrc);
    RuleElementODst *odstrel = rule->getODst();
    Address  *odst = compiler->getFirstODst(rule);  assert(odst);
    RuleElementOSrv *osrvrel=rule->getOSrv();
    Service  *osrv = compiler->getFirstOSrv(rule);  assert(osrv);

    Address  *tsrc = compiler->getFirstTSrc(rule);  assert(tsrc);
    Address  *tdst = compiler->getFirstTDst(rule);  assert(tdst);
    Service  *tsrv = compiler->getFirstTSrv(rule);  assert(tsrv);

    cmdout << _startRuleLine();

    cmdout << _printChainDirectionAndInterface(rule);

    if (! osrv->isAny() )
	cmdout << _printProtocol(osrv);

    cmdout << _printMultiport(rule);

    if (!osrc->isAny())
    {
        string physaddress="";

        if (physAddress::isA(osrc))
            physaddress= physAddress::cast(osrc)->getPhysAddress();

        if (combinedAddress::isA(osrc))
            physaddress= combinedAddress::cast(osrc)->getPhysAddress();

        if ( ! physaddress.empty())
        {
	    cmdout << " -m mac --mac-source "
                             << physaddress;
        }

        const InetAddr *osrc_addr = osrc->getAddressPtr();
        if (osrc_addr==nullptr || !osrc_addr->isAny())
        {
            MultiAddressRunTime *atrt = MultiAddressRunTime::cast(osrc);
            if (atrt!=nullptr && atrt->getSubstitutionTypeName()==AddressTable::TYPENAME &&
                ipt_comp->using_ipset)
            {
                cmdout << _printIpSetMatch(osrc, osrcrel);
            } else
            {
                string osrc_out = _printAddr(osrc);
                if (!osrc_out.empty())
                    cmdout << _printSingleOptionWithNegation(" -s", osrcrel, osrc_out);
            }
        }

//	cmdout << " -s ";
//	cmdout << _printAddr(osrc);
    }

    if (!osrv->isAny())
    {
	cmdout << " ";
	cmdout << _printSrcService(osrvrel);
    }

    if (!odst->isAny())
    {
        MultiAddressRunTime *atrt = MultiAddressRunTime::cast(odst);
        if (atrt!=nullptr && atrt->getSubstitutionTypeName()==AddressTable::TYPENAME &&
            ipt_comp->using_ipset)
        {
            cmdout << _printIpSetMatch(odst, odstrel);
        } else
            cmdout << _printSingleOptionWithNegation(" -d", odstrel, _printAddr(odst));
    }

    cmdout << " ";
    cmdout << _printDstService(osrvrel);

    cmdout << "-j " << rule->getStr("ipt_target") << " ";

    switch (rule->getRuleType())
    {
    case NATRule::Masq:
        if (ropt->getBool("ipt_nat_random")) cmdout << " --random";
        break;

    case NATRule::SNAT:
	if (rule->getStr("ipt_target")=="SNAT")
        {
	    cmdout << "--to-source ";
            // if TSrc is "any" and this is SNAT rule, then this rule only
            // translates source port. Skip address part.
            // Note for #1693: _printAddr() returns string that ends with a space,
            // but there should be no space if the address is followed by port
            // spec.
            QString addr_part;
            if (!tsrc->isAny())
            {
                addr_part = _printAddr(tsrc, false, true).c_str();
            }
	    string ports = _printSNATPorts(tsrv);
	    if (!ports.empty())
            {
                cmdout << addr_part.trimmed().toStdString()
                       << ":" << ports;
            } else
                cmdout << addr_part.toStdString();

            if (ropt->getBool("ipt_nat_random")) cmdout << " --random";
            if (XMLTools::version_compare(version, "1.4.3")>=0)
                if (ropt->getBool("ipt_nat_persistent")) cmdout << " --persistent";
	}
	break;
/*
 *  if rule type is DNAT and TDst is any, this rule only does port
 *  translation and does not change addresses. Iptables accepts
 *  "--to-destination :80" (no address in front of the ':') and seems
 *  to do the right thing.
 */
    case NATRule::DNAT:
	if (rule->getStr("ipt_target")=="DNAT")
        {
	    cmdout << "--to-destination ";
            // if TDst is "any" and this is DNAT rule, then this rule only
            // translates source port. Skip address part.
            QString addr_part;
            if (!tdst->isAny())
                addr_part = _printAddr(tdst, false, true).c_str();

	    string ports = _printDNATPorts(tsrv);
	    if (!ports.empty())
            {
                cmdout << addr_part.trimmed().toStdString()
                       << ":" << ports;
            } else
            {
                cmdout << addr_part.toStdString();
            }

            if (ropt->getBool("ipt_nat_random")) cmdout << " --random";
            if (XMLTools::version_compare(version, "1.4.3")>=0)
                if (ropt->getBool("ipt_nat_persistent")) cmdout << " --persistent";
	}
	break;

    case NATRule::SNetnat:
	if (rule->getStr("ipt_target")=="NETMAP")
        {
            cmdout << "--to ";
            cmdout << _printAddr(tsrc,true,false);
	}
        break;

    case NATRule::DNetnat:
	if (rule->getStr("ipt_target")=="NETMAP")
        {
            cmdout << "--to ";
            cmdout << _printAddr(tdst,true,false);
	}
        break;

    case NATRule::Redirect:
	if (rule->getStr("ipt_target")=="REDIRECT")
        {
	    string ports=_printDNATPorts(tsrv);
	    if (!ports.empty()) cmdout << "--to-ports " << ports;
	}
	break;

    case NATRule::NATBranch:
	break;

    default:	break;
    }
    cmdout << " ";

    cmdout << _endRuleLine();

//    cmdout  << endl;

    compiler->output
        << dynamic_cast<OSConfigurator_linux24*>(compiler->osconfigurator)->
        printRunTimeWrappers( rule, cmdout.str(), ipt_comp->ipv6);

    return true;
}

string NATCompiler_ipt::PrintRule::_declareTable()
{
    return "";
}

string NATCompiler_ipt::PrintRule::_commit()
{
    return "";
}


string NATCompiler_ipt::PrintRule::_quote(const string &s)
{
    return "\"" + s + "\"";
}


