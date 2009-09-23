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

#include "OSConfigurator_openbsd.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/StateSyncClusterGroup.h"

#include <algorithm>
#include <functional>
#include <iostream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string OSConfigurator_openbsd::myPlatformName() { return "OpenBSD"; }

void OSConfigurator_openbsd::processFirewallOptions() 
{
    FWOptions* options=fw->getOptionsObject();
    string s;

    s=options->getStr("openbsd_ip_directed_broadcast");
    if (!s.empty()) {
	if (s!="0" && s!="1")
	    throw FWException(_("Illegal value for OS parameter openbsd_ip_directed_broadcast: '")+s+"'");

	output << "$SYSCTL -w net.inet.ip.directed-broadcast=" << s << endl;
    }

    s=options->getStr("openbsd_ip_forward");
    if (!s.empty()) {
        if (s=="1" || s=="On" || s=="on") s="1";
        else                              s="0";

	output << "$SYSCTL -w net.inet.ip.forwarding=" << s << endl;
    }

    s=options->getStr("openbsd_ipv6_forward");
    if (!s.empty()) {
        if (s=="1" || s=="On" || s=="on") s="1";
        else                              s="0";

	output << "$SYSCTL -w net.inet6.ip6.forwarding=" << s << endl;
    }

    s=options->getStr("openbsd_ip_sourceroute");
    if (!s.empty()) {
	if (s!="0" && s!="1")
	    throw FWException(_("Illegal value for OS parameter openbsd_ip_sourceroute: '")+s+"'");

	output << "$SYSCTL -w net.inet.ip.sourceroute=" << s << endl;
    }

    s=options->getStr("openbsd_ip_redirect");
    if (!s.empty()) {
	if (s!="0" && s!="1")
	    throw FWException(_("Illegal value for OS parameter openbsd_ip_redirect: '")+s+"'");

	output << "$SYSCTL -w net.inet.ip.redirect=" << s << endl;
    }
}

int OSConfigurator_openbsd::prolog()
{
    printPathForAllTools("openbsd");
    printFunctions();

    processFirewallOptions();

    configureInterfaces();

    return 0;
}

void  OSConfigurator_openbsd::printPathForAllTools(const string &os)
{
    FWOptions* options=fw->getOptionsObject();
    
    string s, path_pfctl, path_sysctl, path_logger;

    s=options->getStr("openbsd_path_pfctl");
    if (!s.empty()) path_pfctl=s;
    else            path_pfctl=os_data.getPathForTool(os,OSData::PFCTL);

    s=options->getStr("openbsd_path_sysctl");
    if (!s.empty()) path_sysctl=s;
    else            path_sysctl=os_data.getPathForTool(os,OSData::SYSCTL);

    s=options->getStr("openbsd_path_logger");
    if (!s.empty()) path_logger=s;
    else            path_logger=os_data.getPathForTool(os,OSData::LOGGER);

    output << "PFCTL=\""  + path_pfctl    + "\"\n";
    output << "SYSCTL=\"" + path_sysctl   + "\"\n";
    output << "LOGGER=\"" + path_logger + "\"\n";
    output << endl;
}

