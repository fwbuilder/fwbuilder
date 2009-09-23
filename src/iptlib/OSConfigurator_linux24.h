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

#ifndef _OSCONFIGURATOR_LINUX24_HH
#define _OSCONFIGURATOR_LINUX24_HH

#include "config.h"

#include "fwcompiler/OSConfigurator.h"

#include "OSData.h"

namespace libfwbuilder {
    class FWObject;
    class MultiAddressRunTime;
};

class Configlet;

namespace fwcompiler {

    class OSConfigurator_linux24 : public OSConfigurator {

        OSData os_data;
        
        std::map<std::string,std::string> address_table_objects;

        // this vector is used to avoid duplication of virtual addresses for nat
        std::vector<libfwbuilder::InetAddr> virtual_addresses;
        // map of virt. addresses for nat for each interface
        std::map<std::string, std::string> virtual_addresses_for_nat;

        std::string getInterfaceVarName(libfwbuilder::FWObject *iface,
                                        bool v6=false);

        std::string getPathForATool(const std::string &os_variant, OSData::tools tool_name);
        void setConfigletMacroForOptionStr(libfwbuilder::FWOptions *opt,
                                           Configlet *c,
                                           const char *option_name);
        void setConfigletMacroForOptionInt(libfwbuilder::FWOptions *opt,
                                           Configlet *c,
                                           const char *option_name);

public:

	virtual ~OSConfigurator_linux24() {};
	OSConfigurator_linux24(libfwbuilder::FWObjectDatabase *_db,
                               libfwbuilder::Firewall *fw, bool ipv6_policy);

	virtual std::string myPlatformName();

        virtual int  prolog();
        virtual void epilog();

        /*
         * Try to find conflicts in subinterface types and unsupported
         * interface configurations.
         */
        virtual bool validateInterfaces();

	virtual void processFirewallOptions();
        virtual std::string generateCodeForProtocolHandlers(bool have_nat);

	virtual void addVirtualAddressForNAT(const libfwbuilder::Address *addr);
	virtual void addVirtualAddressForNAT(const libfwbuilder::Network *nw);

        virtual void registerMultiAddressObject(libfwbuilder::MultiAddressRunTime *at);
        virtual void printChecksForRunTimeMultiAddress();
        virtual std::string printShellFunctions();
        virtual std::string printPrologEpilogFunctions();
        virtual std::string printPathForAllTools(const std::string &os);
        virtual std::string printIPForwardingCommands();
        virtual std::string printConfigureInterfacesCommands();
        virtual std::string printRunTimeWrappers(libfwbuilder::FWObject *rule,
                                                 const std::string &command,
                                                 bool ipv6=false);

        virtual std::string printVerifyInterfacesCommands();
        virtual std::string printInterfaceConfigurationCommands();
        virtual std::string printVlanInterfaceConfigurationCommands();
        virtual std::string printBridgeInterfaceConfigurationCommands();
        virtual std::string printBondingInterfaceConfigurationCommands();
        virtual std::string printDynamicAddressesConfigurationCommands();

        virtual std::list<std::string> getGeneratedFiles() const;
    };
};

#endif
