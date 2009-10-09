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

#include <assert.h>

#include "OSConfigurator_freebsd.h"
#include "Configlet.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"

#include <algorithm>
#include <functional>
#include <iostream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string OSConfigurator_freebsd::myPlatformName() { return "FreeBSD"; }

string OSConfigurator_freebsd::printKernelVarsCommands() 
{
    Configlet kernel_vars(fw, "bsd", "kernel_vars");
    kernel_vars.removeComments();
    setKernelVariable(fw, "freebsd_ip_forward", &kernel_vars);
    setKernelVariable(fw, "freebsd_ipv6_forward", &kernel_vars);
    setKernelVariable(fw, "freebsd_ip_sourceroute", &kernel_vars);
    setKernelVariable(fw, "freebsd_ip_redirect", &kernel_vars);
    return kernel_vars.expand().toStdString();
}

int OSConfigurator_freebsd::prolog()
{
    //printPathForAllTools("freebsd");
    //printFunctions();

    //processFirewallOptions();

    //configureInterfaces();

    return 0;
}


