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


#include "NATCompiler_ipfw.h"

#include "fwbuilder/FWObjectDatabase.h"
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
#include "fwbuilder/Firewall.h"

#include <iostream>

#include <assert.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string NATCompiler_ipfw::myPlatformName() { return "ipfw"; }


int NATCompiler_ipfw::prolog()
{

    int n=NATCompiler_pf::prolog();

    return n;
}

void NATCompiler_ipfw::compile()
{
    info(" Compiling NAT rules for " + fw->getName());

    Compiler::compile();

    add( new Begin());
    add( new printTotalNumberOfRules() );

    add( new simplePrintProgress() );

    runRuleProcessors();

}


void NATCompiler_ipfw::epilog()
{
}
