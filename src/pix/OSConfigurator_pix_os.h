/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: OSConfigurator_pix_os.h,v 1.1 2008/03/06 06:49:00 vkurland Exp $

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

#ifndef _OSNETWORKCONFIGURATOR_PIX_OS_HH
#define _OSNETWORKCONFIGURATOR_PIX_OS_HH

#include "config.h"

#include "fwcompiler/OSConfigurator.h"

#include <map>

namespace fwcompiler {

    class OSConfigurator_pix_os : public OSConfigurator {

        std::string _printNameif();
        std::string _printIPAddress();

        std::string _printLogging();
        std::string _printSNMPServer(const std::string &srv,int poll_trap);
        std::string _printSNMP();
        std::string _printSysopt();
        std::string _printNTPServer(const std::string &srv,bool pref);
        std::string _printNTP();
        std::string _printServiceTimeout(const std::string &pix_service);
        std::string _printTimeouts();
        std::string _printFixupCommand(const std::string &fixup_name,
                                       const std::string &sw,
                                       int   arg1, 
                                       int   arg2,
                                       bool  ov);
        std::string _printFixups();
        std::string _printMPF();

	public:

	virtual ~OSConfigurator_pix_os() {};
	OSConfigurator_pix_os(libfwbuilder::FWObjectDatabase *_db,
			      const std::string &fwname,
                              bool ipv6_policy) : 
        OSConfigurator(_db, fwname, ipv6_policy) {}

	virtual int  prolog();

	virtual std::string myPlatformName();
	virtual void processFirewallOptions();
	virtual void addVirtualAddressForNAT(const libfwbuilder::Address   *addr);
	virtual void addVirtualAddressForNAT(const libfwbuilder::Network   *nw);

        std::string getProtocolInspectionCommands();
    };
};

#endif
