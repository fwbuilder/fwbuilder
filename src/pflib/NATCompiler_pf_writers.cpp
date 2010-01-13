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

#include "NATCompiler_pf.h"

#include "fwbuilder/RuleElement.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"
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

#include <QStringList>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;



/**
 *-----------------------------------------------------------------------
 *                    Methods for printing
 */




NATCompiler_pf::PrintRule::PrintRule(const std::string &name) : NATRuleProcessor(name) 
{ 
    init=true; 
}

bool NATCompiler_pf::PrintRule::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    string version = compiler->fw->getStr("version");

    compiler->output << compiler->printComment(rule, current_rule_label, "#");

    // string err = rule->getStr(".error_msg");
    // if (!err.empty()) compiler->output << "# " << err << endl;

    RuleElementOSrc *osrcrel=rule->getOSrc();
    RuleElementODst *odstrel=rule->getODst();
    RuleElementTSrc *tsrcrel=rule->getTSrc();
    RuleElementTDst *tdstrel=rule->getTDst();

    FWObject *osrc, *odst;

    osrc = osrcrel->front();
    if (osrc && FWReference::cast(osrc)!=NULL)
        osrc=FWReference::cast(osrc)->getPointer();

    odst = odstrel->front();
    if (odst && FWReference::cast(odst)!=NULL)
        odst=FWReference::cast(odst)->getPointer();

    //Address  *osrc=compiler->getFirstOSrc(rule);  //assert(osrc);
    //Address  *odst=compiler->getFirstODst(rule);  //assert(odst);
    Service  *osrv=compiler->getFirstOSrv(rule);  //assert(osrv);
                                      
    Address  *tsrc=compiler->getFirstTSrc(rule);  //assert(tsrc);
    Address  *tdst=compiler->getFirstTDst(rule);  //assert(tdst);
    Service  *tsrv=compiler->getFirstTSrv(rule);  //assert(tsrv);

    char errstr[1024];

    if (osrc==NULL || odst==NULL || osrv==NULL ||
        tsrc==NULL || tdst==NULL || tsrv==NULL) 
    {
        if (osrc==NULL)
            sprintf(errstr,"NAT rule %s: osrc==NULL", rule->getLabel().c_str());
        if (odst==NULL)
            sprintf(errstr,"NAT rule %s: odst==NULL", rule->getLabel().c_str());
        if (osrv==NULL)
            sprintf(errstr,"NAT rule %s: osrv==NULL", rule->getLabel().c_str());

        if (tsrc==NULL)
            sprintf(errstr,"NAT rule %s: tsrc==NULL", rule->getLabel().c_str());
        if (tdst==NULL)
            sprintf(errstr,"NAT rule %s: tdst==NULL", rule->getLabel().c_str());
        if (tsrv==NULL)
            sprintf(errstr,"NAT rule %s: tsrv==NULL", rule->getLabel().c_str());

        compiler->abort(rule, errstr);
    }

    string  iface_name = rule->getInterfaceStr();
//    Interface *iface = compiler->getCachedFwInterface(iface_id);
//    string iface_name= (iface!=NULL) ? iface->getName() : "";
    if (iface_name=="nil") iface_name="";

    switch ( rule->getRuleType() ) {
    case NATRule::Continue:
    case NATRule::NONAT:
	compiler->output  << "no nat ";
        if (iface_name!="") compiler->output << "on " << iface_name << " ";
	_printProtocol(osrv);
	compiler->output  << "from ";
	_printREAddr( osrcrel );
	compiler->output  << "to ";
	_printREAddr( odstrel );
	compiler->output  << endl;

	compiler->output  << "no rdr ";
        if (iface_name!="") compiler->output << "on " << iface_name << " ";
	_printProtocol(osrv);
	compiler->output  << "from ";
	_printREAddr( osrcrel );
	compiler->output  << "to ";
	_printREAddr( odstrel );
	compiler->output  << endl;
        break;

    case NATRule::SNAT:
	compiler->output  << "nat ";
        if (iface_name!="") compiler->output << "on " << iface_name << " ";
	_printProtocol(osrv);
	compiler->output  << "from ";
	_printREAddr( osrcrel );
	_printSrcPort(osrv);
	compiler->output  << "to ";
	_printREAddr( odstrel );
        _printPort( osrv, true );

	compiler->output  << "-> ";
	_printREAddr( tsrcrel );
	_printSrcPort(tsrv);
        _printNATRuleOptions(rule);

	compiler->output  << endl;
        break;

    case NATRule::DNAT:
    case NATRule::LB:
	compiler->output  << "rdr ";
        if (iface_name!="") compiler->output << "on " << iface_name << " ";
	_printProtocol(osrv);
	compiler->output  << "from ";
	_printREAddr( osrcrel );
	_printSrcPort(osrv);
	compiler->output  << "to ";
	_printREAddr( odstrel );
	_printPort(osrv, true);
	compiler->output  << "-> ";
	_printREAddr( tdstrel );
	_printPort(tsrv, false);
        _printNATRuleOptions(rule);
	compiler->output  << endl;
        break;

    case NATRule::Redirect:
	compiler->output  << "rdr ";
        if (iface_name!="") compiler->output << "on " << iface_name << " ";
	_printProtocol(osrv);
	compiler->output  << "from ";
	_printREAddr( osrcrel );
	compiler->output  << "to ";
	_printREAddr( odstrel );
	_printPort(osrv, true);
	compiler->output  << "-> ";
	_printREAddr( tdstrel );
	_printPort(tsrv, false);
        _printNATRuleOptions(rule);
	compiler->output  << endl;
        break;

    case NATRule::NATBranch:
    {
        RuleSet *ruleset = rule->getBranch();
        string ruleset_name;
        if (ruleset!=NULL)
        {
             ruleset_name = ruleset->getName();
        } else
        {
            compiler->abort(
                rule, 
                "Branching rule refers ruleset that does not exist");
            // in test mode compiler->abort() does not really abort the program
            ruleset_name = "UNKNOWN";
        }

        if (XMLTools::version_compare(version, "4.6")>=0)
        {
            _printAnchorRule("anchor", ruleset_name, iface_name, rule);
        } else
        {
            _printAnchorRule("nat-anchor", ruleset_name, iface_name, rule);
            _printAnchorRule("rdr-anchor", ruleset_name, iface_name, rule);
        }

    }
    break;

    default: break;
    }

    return true;
}

void NATCompiler_pf::PrintRule::_printAnchorRule(const string &anchor_command,
                                                 const std::string &ruleset_name,
                                                 const std::string &interface_name,
                                                 NATRule *rule)
{
    RuleElementOSrc *osrcrel = rule->getOSrc();
    RuleElementODst *odstrel = rule->getODst();
    RuleElementOSrv *osrvrel = rule->getOSrv();
    Service *osrv = compiler->getFirstOSrv(rule);

    compiler->output << anchor_command << " \"" << ruleset_name << "\" ";
    if (interface_name!="") compiler->output << "on " << interface_name << " ";
    if (!osrvrel->isAny() || !osrcrel->isAny() || !odstrel->isAny())
    {
        _printProtocol(osrv);
        compiler->output  << "from ";
        _printREAddr( osrcrel );
        compiler->output  << "to ";
        _printREAddr( odstrel );
        _printPort(osrv, true);
    }
    compiler->output  << endl;
}

void NATCompiler_pf::PrintRule::_printProtocol(Service *srv)
{
    // CustomService returns protocol name starting with v3.0.4
    if (CustomService::isA(srv))
    {
        // check if the code string for this custom service already includes
        // "proto ..." fragment
        string code = CustomService::cast(srv)->getCodeForPlatform(
            compiler->myPlatformName());
        std::size_t minus_p = code.find("proto ");
        if (minus_p != string::npos) return;
    }

    if ( !TagService::isA(srv))
    {
        string s = srv->getProtocolName();
        if (s=="ip" || s=="any") s="{tcp udp icmp}";
        compiler->output << "proto " <<  s << " ";
    }
}

/*
 * print port numbers for the service. For most platforms that inherit
 * classes for PF this is sufficient, but PF itself also supports
 * TagService and this method prints "tagged" keyword for it as well.
 * Arg <lhs> controls which side of the "->" this service is on.  On
 * the right hand side PF supports shortcut spec 'NNNN:*', but it is
 * not allowed on the left hand side. Also keyword "tagged" is only
 * allowed on the left hand side of "->".
 */
void NATCompiler_pf::PrintRule::_printPort(Service *srv, bool lhs)
{
    if (TCPUDPService::cast(srv))
    {
	int drs = TCPUDPService::cast(srv)->getDstRangeStart();
	int dre = TCPUDPService::cast(srv)->getDstRangeEnd();
	if (drs!=0)
        {
            compiler->output << "port " << drs;
            if (dre!=0 && dre!=drs)
            {
                if (lhs)
                    compiler->output << ":" << dre;
                else
                    compiler->output << ":*";
            }
        }
        compiler->output  << " ";
    }

    if (lhs && TagService::isA(srv)) 
    {
	compiler->output << "tagged "
                         << TagService::constcast(srv)->getCode() << " ";
    }
}

/*
 * Print port range spec using source ports of the given service object
 */
void NATCompiler_pf::PrintRule::_printSrcPort(libfwbuilder::Service *srv)
{
    if (TCPUDPService::cast(srv))
    {
	int srs = TCPUDPService::cast(srv)->getSrcRangeStart();
	int sre = TCPUDPService::cast(srv)->getSrcRangeEnd();
	if (srs!=0)
        {
            compiler->output << "port " << srs;
            if (sre != 0 && sre != srs)
            {
                compiler->output << ":" << sre;
            }
            compiler->output  << " ";
        }
    }
}

void NATCompiler_pf::PrintRule::_printNegation(RuleElement  *rel)
{
    if (rel->getNeg())
	compiler->output << "! ";
}

void NATCompiler_pf::PrintRule::_printREAddr(RuleElement *rel)
{
    FWObject *o=rel->front();
    if (o && FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();

    Address *addr= Address::cast(o);

    _printNegation(rel);

    if (rel->size()==1 && ! o->getBool("pf_table") )
    {
	_printAddr( addr );
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


void NATCompiler_pf::PrintRule::_printAddrList(FWObject  *grp,bool )
{
    compiler->output << "{ ";
    for (FWObject::iterator i=grp->begin(); i!=grp->end(); i++)
    {
        if (i!=grp->begin())  compiler->output << ", ";
        FWObject *o= *i;
        if (FWReference::cast(o)!=NULL) o=FWReference::cast(o)->getPointer();
        Address *s=Address::cast( o );
        assert(s);
        _printAddr(s);
    }
    compiler->output << "} ";
}

void NATCompiler_pf::PrintRule::_printAddr(FWObject *o)
{
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

    if (Interface::cast(o)!=NULL)
    {
        Interface *iface=Interface::cast(o);
        if (iface->isDyn())
        {
            compiler->output << "(" << iface->getName() << ") ";
            return;
        }
    }

    Address *a = Address::cast(o);
    const InetAddr *addr = a->getAddressPtr();
    if (addr)
    {
        InetAddr mask = *(a->getNetmaskPtr());

        if (Interface::cast(o)!=NULL || Address::cast(o)->dimension()==1)
        {
            mask = InetAddr(InetAddr::getAllOnes());
        }

        if (addr->isAny() && mask.isAny())
        {
            compiler->output << "any ";
        } else
        {
            compiler->output << addr->toString();
            if (!mask.isHostMask())
            {
                compiler->output << "/" << mask.getLength();
            }
            compiler->output  << " ";
        }
    }
}

void NATCompiler_pf::PrintRule::_printNATRuleOptions(Rule *rule)
{
    FWOptions *ruleopt =rule->getOptionsObject();

    if (ruleopt->getBool("pf_bitmask"))     compiler->output << "bitmask ";
    if (ruleopt->getBool("pf_random"))      compiler->output << "random ";
    if (ruleopt->getBool("pf_source_hash")) compiler->output << "source-hash ";
    if (ruleopt->getBool("pf_round_robin")) compiler->output << "round-robin ";

    if (ruleopt->getBool("pf_static_port")) compiler->output << "static-port ";
}

bool NATCompiler_pf::PrintTables::processNext()
{
    NATCompiler_pf *pf_comp=dynamic_cast<NATCompiler_pf*>(compiler);

    slurp();
    if (tmp_queue.size()==0) return false;

/* print tables */
    compiler->output << pf_comp->tables->PrintTables();

    return true;
}
