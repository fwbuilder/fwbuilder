/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: NATCompiler_ipf_writers.cpp 1028 2006-05-17 02:30:19Z vkurland $

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

#include "NATCompiler_ipf.h"


#include "fwbuilder/RuleElement.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Firewall.h"


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





void NATCompiler_ipf::PrintRule::_printAddr_L(Address  *o, bool print_netmask)
{
    FWOptions* options=compiler->fw->getOptionsObject();

    InetAddr addr=o->getAddress();
    InetNetmask   mask=o->getNetmask();

    if (Interface::cast(o)!=NULL && Interface::cast(o)->isDyn()) 
    {
        if (options->getBool("dynAddr"))
            compiler->output << "(" << o->getName() << ") ";
        else
            compiler->output << "any ";

        return;
    }

    if (Interface::cast(o)!=NULL && ! Interface::cast(o)->isDyn()) 
	mask = InetNetmask(InetAddr::getAllOnes());

    if (dynamic_cast<InetAddrMask*>(o)->dimension()==1)
	mask = InetNetmask(InetAddr::getAllOnes());

    if (addr.isAny() && mask.isAny())
    {
        compiler->output << "any ";
    } else
    {

      compiler->output << addr.toString();

      if (print_netmask)
         compiler->output << "/" << mask.getLength();
      compiler->output  << " ";
   }
}

void NATCompiler_ipf::PrintRule::_printAddr_R(Address  *o, bool print_netmask)
{
    InetAddr addr = o->getAddress();
    InetNetmask mask = o->getNetmask();

    if (Interface::cast(o) != NULL)
	mask = InetNetmask(InetAddr::getAllOnes());

    if (dynamic_cast<InetAddrMask*>(o)->dimension()==1)
	mask = InetNetmask(InetAddr::getAllOnes());

    if (addr.isAny() && print_netmask &&  mask.isHostMask())
    {
        compiler->output  << "0/32 ";
    } else
    {
        compiler->output << addr.toString();
        if (print_netmask)
            compiler->output << "/" << mask.getLength();
        compiler->output  << " ";
    }
}


void NATCompiler_ipf::PrintRule::_printAddr_R_LB(RuleElementTDst *tdst)
{
    bool first=true;
    for(list<FWObject*>::iterator i=tdst->begin(); i!=tdst->end(); ++i) 
    {
        FWObject *o= *i;
	FWObject *obj = NULL;
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();

        Address *a=Address::cast(obj);

        InetAddr addr=a->getAddress();
        
        if (!first) compiler->output << ",";
        compiler->output << addr.toString();
        first=false;
    }
    compiler->output << " ";
}



void NATCompiler_ipf::PrintRule::_printProtocol(Service *srv)
{
    compiler->output << srv->getProtocolName() << " ";
}

/*
 * Note: ipfilter permits "port 0" to the right of "->", in fact, this is useful
 * and recommended construct for rules that should match "any tcp" or "any udp".
 * If this method is called to print port spec. for the part of the rule left
 * of "->", then parameter eq is true, otherwise it is false. We permit port 0
 * only for the right part of the rule, that is, when eq is false
 */
void NATCompiler_ipf::PrintRule::_printPort(Service *srv,bool eq)
{
    if (TCPService::isA(srv) || UDPService::isA(srv)) 
    {
	int drs=srv->getInt("dst_range_start");
        if (!eq || drs!=0)
        {
            compiler->output << "port ";
            if (eq)
                compiler->output << "= ";
            compiler->output<< drs << " ";
        }
    }
}


NATCompiler_ipf::PrintRule::PrintRule(const std::string &name) : NATCompiler_pf::PrintRule(name)
{ 
}

bool NATCompiler_ipf::PrintRule::processNext()
{
    NATRule *rule=getNext(); if (rule==NULL) return false;

    tmp_queue.push_back(rule);

    string rl=rule->getLabel();
    if (rl!=current_rule_label) {

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


    Address  *osrc=compiler->getFirstOSrc(rule);  assert(osrc);
    Address  *odst=compiler->getFirstODst(rule);  assert(odst);
    Service  *osrv=compiler->getFirstOSrv(rule);  assert(osrv);
                                             
    Address  *tsrc=compiler->getFirstTSrc(rule);  assert(tsrc);
    Address  *tdst=compiler->getFirstTDst(rule);  assert(tdst);
    Service  *tsrv=compiler->getFirstTSrv(rule);  assert(tsrv);

//    Interface *iface=
//	Interface::cast( rule->getRoot()->getById(rule->getInterfaceId() ,true) );

    if (rule->getRuleType()==NATRule::NONAT) 
    {
	compiler->output  << "map "
                          << rule->getInterfaceStr()
                          << " ";
	compiler->output  << "from ";
	_printAddr_L( osrc );
	compiler->output  << "to ";
	_printAddr_L( odst );

	if ( rule->getStr("nat_rule_proxy")=="") _printPort(osrv,true);

	compiler->output  << "-> 0/0";
//	_printAddr_R( tsrc );
	compiler->output  << " ";

	if ( rule->getStr("nat_rule_proxy")!="") compiler->output << rule->getStr("nat_rule_proxy");

	compiler->output  << endl;
    }

    if (rule->getRuleType()==NATRule::SNAT) 
    {
	compiler->output  << "map "
                          << rule->getInterfaceStr()
                          << " ";
	compiler->output  << "from ";
	_printAddr_L( osrc );
	compiler->output  << "to ";
	_printAddr_L( odst );

	if ( rule->getStr("nat_rule_proxy")=="") _printPort(osrv,true);

	compiler->output  << "-> ";
	_printAddr_R( tsrc );
	compiler->output  << " ";

	if ( rule->getStr("nat_rule_proxy")!="") compiler->output << rule->getStr("nat_rule_proxy");
        else
        {
            if (osrv->getTypeName()==TCPService::TYPENAME || 
                osrv->getTypeName()==UDPService::TYPENAME ||
                rule->getBool("needs_portmap") )
                compiler->output << " portmap tcp/udp auto ";
        }

	compiler->output  << endl;
    }

    if (rule->getRuleType()==NATRule::DNAT) 
    {
	compiler->output  << "rdr " << rule->getInterfaceStr() << " ";

        compiler->output  << "from ";    _printAddr_L( osrc , true );
        compiler->output  << "to ";      _printAddr_L( odst , true );
        _printPort(osrv,true);
	
        compiler->output  << "-> ";      _printAddr_R( tdst , false );
	_printPort(tsrv,false);

        if ( ! osrv->isAny()) _printProtocol(osrv);
	compiler->output  << endl;
    }

    if (rule->getRuleType()==NATRule::LB) 
    {
	compiler->output  << "rdr " << rule->getInterfaceStr() << " ";

        compiler->output  << "from ";   _printAddr_L( osrc , true );
        compiler->output  << "to ";     _printAddr_L( odst , true );
	_printPort(osrv,true);

	compiler->output  << "-> ";     _printAddr_R_LB( rule->getTDst() );
	_printPort(tsrv,false);

        if ( ! osrv->isAny()) _printProtocol(osrv);
        compiler->output << " round-robin ";
	compiler->output << endl;
    }

    if (rule->getRuleType()==NATRule::Redirect) 
    {
	compiler->output  << "rdr " << rule->getInterfaceStr() << " ";

        compiler->output  << "from ";    _printAddr_L( osrc , true );
        compiler->output  << "to ";      _printAddr_L( odst , true );
	_printPort(osrv,true);

	compiler->output  << "-> ";      _printAddr_R( tdst , false );
	_printPort(tsrv,false);
	_printProtocol(osrv);

	compiler->output  << endl;
    }

    return true;
}

