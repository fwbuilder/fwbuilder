/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef __OSCONFIGURATOR_HH__
#define __OSCONFIGURATOR_HH__

#include "fwcompiler/Compiler.h"

#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/InetAddr.h"

#include <fstream>
#include <sstream>

namespace fwcompiler {

    class OSConfigurator : public Compiler {

        int num_virtual_addresses_for_nat;

public:

	virtual ~OSConfigurator();
	OSConfigurator(libfwbuilder::FWObjectDatabase *_db,
                       libfwbuilder::Firewall *fw, bool ipv6_policy);

	virtual void processFirewallOptions() {}
	virtual void addVirtualAddressForNAT(const libfwbuilder::Address*) {};
	virtual void addVirtualAddressForNAT(const libfwbuilder::Network*) {};
        void registerVirtualAddressForNat() { num_virtual_addresses_for_nat++; }
        int getNumOfVirtualAddressesForNat() { return num_virtual_addresses_for_nat; }

        virtual std::string printFunctions() { return ""; }
	virtual std::string printKernelVarsCommands() { return ""; }
        virtual std::string configureInterfaces() { return ""; }
    };
}

#endif
