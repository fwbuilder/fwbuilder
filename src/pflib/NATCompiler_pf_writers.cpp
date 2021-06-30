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
#include "fwbuilder/UserService.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AttachedNetworks.h"

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

void NATCompiler_pf::PrintRule::_printInterface(NATRule *rule)
{
    RuleElementItf *intf_re = rule->getItfOutb();
    QStringList rule_interfaces;

    if ( ! intf_re->isAny())
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
                         << string((intf_re->getBool("single_object_negation")) ? "! " : " ")
                         << rule_interfaces.join(" ").toStdString()
                         << " ";
    }
}


bool NATCompiler_pf::PrintRule::processNext()
{
    NATRule *rule=getNext(); if (rule==nullptr) return false;

    tmp_queue.push_back(rule);

    string version = compiler->fw->getStr("version");

    compiler->output << compiler->printComment(rule, current_rule_label, "#");

    RuleElementOSrc *osrcrel = rule->getOSrc();
    RuleElementODst *odstrel = rule->getODst();
    RuleElementTSrc *tsrcrel = rule->getTSrc();
    RuleElementTDst *tdstrel = rule->getTDst();

    FWObject *osrc, *odst;

    osrc = osrcrel->front();
    if (osrc && FWReference::cast(osrc)!=nullptr)
        osrc = FWReference::cast(osrc)->getPointer();

    odst = odstrel->front();
    if (odst && FWReference::cast(odst)!=nullptr)
        odst = FWReference::cast(odst)->getPointer();

    //Address  *osrc=compiler->getFirstOSrc(rule);  //assert(osrc);
    //Address  *odst=compiler->getFirstODst(rule);  //assert(odst);
    Service  *osrv = compiler->getFirstOSrv(rule);  //assert(osrv);
                                      
    Address  *tsrc = compiler->getFirstTSrc(rule);  //assert(tsrc);
    Address  *tdst = compiler->getFirstTDst(rule);  //assert(tdst);
    Service  *tsrv = compiler->getFirstTSrv(rule);  //assert(tsrv);

    if (osrc==nullptr || odst==nullptr || osrv==nullptr ||
        tsrc==nullptr || tdst==nullptr || tsrv==nullptr) 
    {
        QString err;
        if (osrc==nullptr) err = QString("NAT rule %1: osrc==nullptr");
        if (odst==nullptr) err = QString("NAT rule %1: odst==nullptr");
        if (osrv==nullptr) err = QString("NAT rule %1: osrv==nullptr");
        if (tsrc==nullptr) err = QString("NAT rule %1: tsrc==nullptr");
        if (tdst==nullptr) err = QString("NAT rule %1: tdst==nullptr");
        if (tsrv==nullptr) err = QString("NAT rule %1: tsrv==nullptr");
        compiler->abort(rule, err.arg(rule->getLabel().c_str()).toStdString());
    }

    switch ( rule->getRuleType() )
    {
    case NATRule::Continue:
    case NATRule::NONAT:
    {
        if (XMLTools::version_compare(version, "4.7")>=0)
        {
            /* I could not find a better way to implement old "no nat"
             * behavior with 4.7. They seem to suggest that we should
             * implement exceptions to the translations using "pass"
             * or "block" actions. At least this is the only way they
             * show in examples and there is no "no" keyword anymore.
             */
            compiler->output  << "pass in quick ";

            _printInterface(rule);

            _printProtocol(osrv);
            compiler->output  << "from ";
            _printREAddr(osrcrel);
            _printSrcPort(osrv, true);
            compiler->output  << "to ";
            _printREAddr(odstrel);
            _printPort(osrv, true);
            compiler->output  << endl;
        } else
        {
            compiler->output  << "no nat ";

            _printInterface(rule);

            _printProtocol(osrv);
            compiler->output  << "from ";
            _printREAddr(osrcrel);
            compiler->output  << "to ";
            _printREAddr(odstrel);
            compiler->output  << endl;

            compiler->output  << "no rdr ";

            _printInterface(rule);

            _printProtocol(osrv);
            compiler->output  << "from ";
            _printREAddr( osrcrel );
            compiler->output  << "to ";
            _printREAddr( odstrel );
            compiler->output  << endl;
        }
        break;
    }
    case NATRule::SNAT:
    {
        if (XMLTools::version_compare(version, "4.7")>=0)
        {
            compiler->output  << "match out ";

            _printInterface(rule);

            _printProtocol(osrv);
            compiler->output  << "from ";
            _printREAddr( osrcrel );
            _printSrcPort(osrv, true);
            compiler->output  << "to ";
            _printREAddr( odstrel );
            _printPort( osrv, true );

            compiler->output  << "nat-to ";
            _printREAddr( tsrcrel );
            _printSrcPort(tsrv, false);
            _printNATRuleOptions(rule);

            compiler->output  << endl;

        } else
        {
            compiler->output  << "nat ";

            _printInterface(rule);

            _printProtocol(osrv);
            compiler->output  << "from ";
            _printREAddr( osrcrel );
            _printSrcPort(osrv, true);
            compiler->output  << "to ";
            _printREAddr( odstrel );
            _printPort( osrv, true );

            compiler->output  << "-> ";
            _printREAddr( tsrcrel );
            _printSrcPort(tsrv, false);
            _printNATRuleOptions(rule);

            compiler->output  << endl;
        }
        break;
    }

    case NATRule::DNAT:
    case NATRule::LB:
    {
        if (XMLTools::version_compare(version, "4.7")>=0)
        {
            compiler->output  << "match in ";

            _printInterface(rule);

            _printProtocol(osrv);
            compiler->output  << "from ";
            _printREAddr( osrcrel );
            _printSrcPort(osrv, true);        // this is where it is different from NATRule::Redirect
            compiler->output  << "to ";
            _printREAddr( odstrel );
            _printPort(osrv, true);
            compiler->output  << "rdr-to ";
            _printREAddr( tdstrel );
            _printPort(tsrv, false);
            _printNATRuleOptions(rule);
            compiler->output  << endl;
        } else
        {
            compiler->output  << "rdr ";

            _printInterface(rule);

            _printProtocol(osrv);
            compiler->output  << "from ";
            _printREAddr( osrcrel );
            _printSrcPort(osrv, true);        // this is where it is different from NATRule::Redirect
            compiler->output  << "to ";
            _printREAddr( odstrel );
            _printPort(osrv, true);
            compiler->output  << "-> ";
            _printREAddr( tdstrel );
            _printPort(tsrv, false);
            _printNATRuleOptions(rule);
            compiler->output  << endl;
        }
        break;
    }

    case NATRule::Redirect:
    {
        if (XMLTools::version_compare(version, "4.7")>=0)
        {
            compiler->output  << "match in ";

            _printInterface(rule);

            _printProtocol(osrv);
            compiler->output  << "from ";
            _printREAddr( osrcrel );
            compiler->output  << "to ";
            _printREAddr( odstrel );
            _printPort(osrv, true);
            compiler->output  << "rdr-to ";
            _printREAddr( tdstrel );
            _printPort(tsrv, false);
            _printNATRuleOptions(rule);
            compiler->output  << endl;
        } else
        {
            compiler->output  << "rdr ";

            _printInterface(rule);

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
        }
        break;
    }

    case NATRule::NATBranch:
    {
        RuleSet *ruleset = rule->getBranch();
        string ruleset_name;
        if (ruleset!=nullptr)
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
            _printAnchorRule("anchor", ruleset_name, rule);
        } else
        {
            _printAnchorRule("nat-anchor", ruleset_name, rule);
            _printAnchorRule("rdr-anchor", ruleset_name, rule);
        }

    }
    break;

    default: break;
    }

    return true;
}

void NATCompiler_pf::PrintRule::_printAnchorRule(const string &anchor_command,
                                                 const std::string &ruleset_name,
                                                 NATRule *rule)
{
    RuleElementOSrc *osrcrel = rule->getOSrc();
    RuleElementODst *odstrel = rule->getODst();
    RuleElementOSrv *osrvrel = rule->getOSrv();
    Service *osrv = compiler->getFirstOSrv(rule);

    compiler->output << anchor_command << " \"" << ruleset_name << "\" ";

    _printInterface(rule);

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

    if (!srv->isAny() && !TagService::isA(srv) && !UserService::isA(srv) && 
        srv->getProtocolName()!="ip")
    {
	compiler->output << "proto ";
	compiler->output << srv->getProtocolName();
	compiler->output << " ";
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
void NATCompiler_pf::PrintRule::_printSrcPort(Service *srv, bool lhs)
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
                if (lhs)
                    compiler->output << ":" << sre;
                else
                    compiler->output << ":*";
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
    if (o && FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();

//    Address *addr= Address::cast(o);

    _printNegation(rel);

    if (rel->size()==1)
    {
	_printAddr(o);
    } else
    {
        _printAddrList(rel, rel->getNeg());
    }
}


void NATCompiler_pf::PrintRule::_printAddrList(FWObject  *grp,bool )
{
    compiler->output << "{ ";
    for (FWObject::iterator i=grp->begin(); i!=grp->end(); i++)
    {
        if (i!=grp->begin())  compiler->output << ", ";
        FWObject *o= *i;
        if (FWReference::cast(o)!=nullptr) o=FWReference::cast(o)->getPointer();
        _printAddr(o);
    }
    compiler->output << "} ";
}

void NATCompiler_pf::PrintRule::_printAddr(FWObject *o)
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

    if (Interface::cast(o)!=nullptr)
    {
        compiler->output << "(" << o->getName() << ") ";
        return;
    }

    if (o->getBool("pf_table"))
    {
        compiler->output << "<" << o->getName() << "> ";
        return;
    }

    Address *addr_obj = Address::cast(o);
    assert(addr_obj!=nullptr);

    const InetAddr *addr = addr_obj->getAddressPtr();
    if (addr)
    {
        InetAddr mask = *(addr_obj->getNetmaskPtr());

        if (Interface::cast(o)!=nullptr || Address::cast(o)->dimension()==1)
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
