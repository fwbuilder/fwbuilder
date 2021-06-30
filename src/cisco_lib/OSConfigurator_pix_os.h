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

#ifndef _OSNETWORKCONFIGURATOR_PIX_OS_HH
#define _OSNETWORKCONFIGURATOR_PIX_OS_HH


#include "fwcompiler/OSConfigurator.h"

#include <map>
#include <QString>

class Configlet;

namespace libfwbuilder {
    class ClusterGroup;
};

namespace fwcompiler {

    class OSConfigurator_pix_os : public OSConfigurator {

        //std::string _printNameif();
        //std::string _printIPAddress();

        void _getFailoverAddresses(libfwbuilder::ClusterGroup *cluster_group,
                                   QString *primary_addr,
                                   QString *primary_netm,
                                   QString *standby_addr);

        void _getAddressConfigurationForInterface(libfwbuilder::Interface *iface,
                                                  QString *addr,
                                                  QString *netm,
                                                  QString *standby_addr);

        std::string _printInterfaceConfiguration();
        std::string _printFailoverConfiguration();

        std::string _printLogging();

        void _configureSNMPServer(Configlet *cnf, int server_num,
                                  const std::string &srv, int poll_trap);
        void _configureNTPServer(Configlet *cnf, int server_num,
                                 const std::string &server, bool pref);

        std::string _printSNMP();
        std::string _printSysopt();
        std::string _printNTP();
        std::string _printServiceTimeout(const std::string &pix_service);
        std::string _printTimeouts();
        std::string _printSSHConfiguration();
        std::string _printFixupCommand(const std::string &fixup_name,
                                       const std::string &sw,
                                       int   arg1, 
                                       int   arg2,
                                       bool  ov);
        std::string _printFixups();
        std::string _printMPFPolicyMap();
        std::string _printPolicyMapTypeInspect();
        
	public:

	virtual ~OSConfigurator_pix_os() {};
	OSConfigurator_pix_os(libfwbuilder::FWObjectDatabase *_db,
			      libfwbuilder::Firewall *fw,
                              bool ipv6_policy) : 
        OSConfigurator(_db, fw, ipv6_policy) {}

	virtual int  prolog();

	virtual std::string myPlatformName();
	virtual void processFirewallOptions();
	virtual void addVirtualAddressForNAT(const libfwbuilder::Address   *addr);
	virtual void addVirtualAddressForNAT(const libfwbuilder::Network   *nw);

        std::string getProtocolInspectionCommands();
    };
};

#endif
