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

#ifndef _OSNETWORKCONFIGURATOR_NXOS_HH
#define _OSNETWORKCONFIGURATOR_NXOS_HH


#include "fwcompiler/OSConfigurator.h"

#include <map>

namespace fwcompiler {

    class OSConfigurator_nxos : public OSConfigurator {

        std::string _printNameif();
        std::string _printIPAddress();
        std::string _printLogging();

	public:

	virtual ~OSConfigurator_nxos() {};
	OSConfigurator_nxos(libfwbuilder::FWObjectDatabase *_db,
                           libfwbuilder::Firewall *fw,
                           bool ipv6_policy) : OSConfigurator(_db, fw, ipv6_policy) {}

	virtual int  prolog();

	virtual std::string myPlatformName();
	virtual void processFirewallOptions();
	virtual void addVirtualAddressForNAT(const libfwbuilder::Address   *addr);
	virtual void addVirtualAddressForNAT(const libfwbuilder::Network   *nw);
    };
};

#endif
