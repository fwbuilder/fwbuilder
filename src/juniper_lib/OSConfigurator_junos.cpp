/* 

                          Firewall Builder

                 Copyright (C) 2007 NetCitadel, LLC

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


#include "OSConfigurator_junos.h"
#include "cisco_lib/Helper.h"
#include "fwbuilder/Resources.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"

#include <list>
#include <algorithm>
#include <functional>
#include <assert.h>
#include <iostream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string OSConfigurator_junos::myPlatformName() { return "junos"; }

int OSConfigurator_junos::prolog()
{
    string host_os = fw->getStr("host_OS");

    if (host_os!="junos")
	abort("Unsupported OS " + host_os );

    return Compiler::prolog();
}


void OSConfigurator_junos::processFirewallOptions()
{
}

string OSConfigurator_junos::_printNameif()
{
    ostringstream res;

    return res.str();
}

string OSConfigurator_junos::_printIPAddress()
{
    ostringstream res;

    return res.str();
}


string OSConfigurator_junos::_printLogging()
{
    ostringstream str;

    return str.str();
}

void OSConfigurator_junos::addVirtualAddressForNAT(const Address*)
{
}

void OSConfigurator_junos::addVirtualAddressForNAT(const Network*)
{
}

