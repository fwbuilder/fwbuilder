/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: NATCompiler_PrintRule.cpp 1179 2006-10-08 00:10:02Z vkurland $

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
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"

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
string NATCompiler_ipt::PrintRule::_createChain(const string &chain)
{
    ostringstream  res;
    if ( ! chains[chain] )
    {
	res << "$IPTABLES -t nat -N " << chain << endl;
	chains[chain]=true;
    }
    return res.str();
}

string NATCompiler_ipt::PrintRule::_startRuleLine()
{            
    return string("$IPTABLES -t nat -A ");
}

string NATCompiler_ipt::PrintRule::_endRuleLine()
{            
    return string("\n");
}

string NATCompiler_ipt::PrintRule::_printRuleLabel(NATRule *rule)
{
    ostringstream res;

    bool nocomm=Resources::os_res[compiler->fw->getStr("host_OS")]->Resources::getResourceBool("/FWBuilderResources/Target/options/suppress_comments");

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
//            res << "# " << endl;
        }
        current_rule_label=rl;
    }
    return res.str();
}

/*
 *  check and create new chain if needed
 */

/**
 *-----------------------------------------------------------------------
 */
string NATCompiler_ipt::PrintRule::_printChainDirectionAndInterface(NATRule *rule)
{
    std::ostringstream  ostr;

    string       iface_name = rule->getInterfaceStr();
//    Interface *rule_iface = compiler->getCachedFwInterface(iface_id);
//    string iface_name= (rule_iface!=NULL) ? rule_iface->getName() : "";
    if (iface_name=="nil") iface_name="";

/* if interface name ends with '*', this is a wildcard
 * interface. Iptables supports wildcard interfaces but uses '+' as a
 * wildcard symbol */

    string::size_type n;
    if ( (n=iface_name.find("*"))!=string::npos)    iface_name[n]='+';

    ostr << rule->getStr("ipt_chain") << " ";

    switch (rule->getRuleType()) {
    case NATRule::SNAT:  
	if (!iface_name.empty()) ostr << "-o " << iface_name;
	break;
    case NATRule::Masq:  
	if (!iface_name.empty()) ostr << "-o " << iface_name;
	break;
    case NATRule::DNAT:  
	if (!iface_name.empty()) ostr << "-i " << iface_name;
	break;
    case NATRule::Redirect: 
	if (!iface_name.empty()) ostr << "-i " << iface_name;
	break;
    default: break;
    }
    ostr << " ";
    return ostr.str();
}



string NATCompiler_ipt::PrintRule::_printProtocol(Service *srv)
{
    std::ostringstream  ostr;
    if (!srv->isAny() && !CustomService::isA(srv) && !TagService::isA(srv))
    {
        string pn=srv->getProtocolName();
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
	int rs=srv->getInt("src_range_start");
	int re=srv->getInt("src_range_end");
	ostr << _printOPorts(rs,re);
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printDstPorts(Service *srv)
{
    std::ostringstream  ostr;
    if (TCPService::isA(srv) || UDPService::isA(srv)) {
	int rs=srv->getInt("dst_range_start");
	int re=srv->getInt("dst_range_end");
	ostr << _printOPorts(rs,re);
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printSNATPorts(Service *srv)
{
    std::ostringstream  ostr;
    if (TCPService::isA(srv) || UDPService::isA(srv)) {
	int rs=srv->getInt("src_range_start");
	int re=srv->getInt("src_range_end");
	ostr << _printTPorts(rs,re);
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printDNATPorts(Service *srv)
{
    std::ostringstream  ostr;
    if (TCPService::isA(srv) || UDPService::isA(srv)) {
	int rs=srv->getInt("dst_range_start");
	int re=srv->getInt("dst_range_end");
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
    if (o && FWReference::cast(o)!=NULL)
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
            string v = compiler->fw->getStr("version");
            if (v.empty() || v=="ge_1.2.6" || v=="1.2.9" || v=="1.3.0")
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
    if (o && FWReference::cast(o)!=NULL)
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
                 << TagService::cast(srv)->getCode() << " ";
        }

    } else {
/* use multiport */

	string str;
	bool   first=true;
	for (FWObject::iterator i=rel->begin(); i!=rel->end(); i++)
        {
	    FWObject *o= *i;
	    if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

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
            string v = compiler->fw->getStr("version");
            if (v.empty() || v=="ge_1.2.6" || v=="1.2.9" || v=="1.3.0")
                ostr << "--dports ";
            else
                ostr << "--destination-port ";
	    ostr << str << " ";
	}
    }
    return ostr.str();
}

string NATCompiler_ipt::PrintRule::_printAddr(Address  *o,bool print_mask,bool print_range)
{
    NATCompiler_ipt *ipt_comp=dynamic_cast<NATCompiler_ipt*>(compiler);
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

    if (print_range && AddressRange::cast(o)!=NULL)
    {
	InetAddr a1 = AddressRange::cast(o)->getRangeStart();
	InetAddr a2 = AddressRange::cast(o)->getRangeEnd();
	ostr << a1.toString() << "-" << a2.toString();
    } else
    {
        const InetAddr& addr = o->getAddress();
        const InetAddr& mask = o->getNetmask();

        Interface *iface = Interface::cast(o);
        if (iface!=NULL)
        {
            if (iface->isDyn() && iface->getBool("use_var_address"))
            {
                ostr << "$" << ipt_comp->getInterfaceVarName(iface) << " ";
                return ostr.str();
            }
            ostr << addr.toString();
            return ostr.str();
        }

	if (addr == InetAddr::getAny() && mask == InetAddr::getAny())
        {
	    ostr << "0/0";
	} else
        {	
	    ostr << addr.toString();

            if (print_mask &&
                Address::cast(o)->dimension()!=1 &&
                !mask.isHostMask())
            {
                ostr << "/" << mask.getLength();
            }
	}
    }
    return ostr.str();
}


NATCompiler_ipt::PrintRule::PrintRule(const std::string &name) : 
    NATRuleProcessor(name)
{
    init=true; 
    print_once_on_top=true;

    chains["POSTROUTING"]  =true;
    chains["PREROUTING"]   =true;
    chains["SNAT"]         =true;
    chains["DNAT"]         =true;
    chains["MASQUERADE"]   =true;
    chains["REDIRECT"]     =true;
    chains["NETMAP"]       =true;
    chains["LOG"]          =true;
    chains["MARK"]         =true;
    chains["ACCEPT"]       =true;
    chains["REJECT"]       =true;
    chains["DROP"]         =true;
    chains["RETURN"]       =true;
    chains["OUTPUT"]       =true;
}

bool NATCompiler_ipt::PrintRule::processNext()
{
    NATRule *rule=getNext(); 
    if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    compiler->output << _printRuleLabel(rule);

    string s;
    std::ostringstream  cmdout;

    compiler->output << _createChain(rule->getStr("ipt_chain"));
    compiler->output << _createChain(rule->getStr("ipt_target"));


//    RuleElementOSrc *osrcrel=rule->getOSrc();
    Address  *osrc=compiler->getFirstOSrc(rule);  assert(osrc);
//    RuleElementODst *odstrel=rule->getODst();
    Address  *odst=compiler->getFirstODst(rule);  assert(odst);
    RuleElementOSrv *osrvrel=rule->getOSrv();
    Service  *osrv=compiler->getFirstOSrv(rule);  assert(osrv);

    Address  *tsrc=compiler->getFirstTSrc(rule);  assert(tsrc);
    Address  *tdst=compiler->getFirstTDst(rule);  assert(tdst);
    Service  *tsrv=compiler->getFirstTSrv(rule);  assert(tsrv);

//    Interface *iface=
//	Interface::cast( rule->getRoot()->getById(rule->getInterfaceId() ,true) );


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
/*
 * fool-proof: this is last resort check for situation when user created IPv4 object
 * for the interface but left it with empty address ( 0.0.0.0 ). 
 */
        if ( ! physaddress.empty() && osrc->getAddress()==InetAddr())
        {
            ;
        } else
        {
            cmdout << " -s ";
            cmdout << _printAddr(osrc);
        }

//	cmdout << " -s ";
//	cmdout << _printAddr(osrc);
    }

    if (!osrv->isAny()) {
	cmdout << " ";
	cmdout << _printSrcService(osrvrel);
    }

    if (!odst->isAny()) {
	cmdout << " -d ";
	cmdout << _printAddr(odst);
    }

    cmdout << " ";
    cmdout << _printDstService(osrvrel);

    cmdout << "-j " << rule->getStr("ipt_target") << " ";

    switch (rule->getRuleType()) {
    case NATRule::SNAT:  
	if (rule->getStr("ipt_target")=="SNAT")
        {
	    cmdout << "--to-source ";
	    cmdout << _printAddr(tsrc,false,true);
	    string ports=_printSNATPorts(tsrv);
	    if (!ports.empty()) cmdout << ":" << ports;
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
            if (!tdst->isAny()) cmdout << _printAddr(tdst,false,true);
	    string ports=_printDNATPorts(tsrv);
	    if (!ports.empty()) cmdout << ":" << ports;
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
    default:	break;
    }
    cmdout << " ";

    cmdout << _endRuleLine();

//    cmdout  << endl;

    compiler->output 
        << dynamic_cast<OSConfigurator_linux24*>(compiler->osconfigurator)->printRunTimeWrappers( rule, cmdout.str());

    return true;
}

string NATCompiler_ipt::PrintRule::_declareTable()
{
    return "";
}

string NATCompiler_ipt::PrintRule::_flushAndSetDefaultPolicy()
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


