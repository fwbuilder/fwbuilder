/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: OSConfigurator_freebsd.h 749 2005-03-30 07:33:18Z vkurland $

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

#ifndef _OSNETWORKCONFIGURATOR_FREEBSD_HH
#define _OSNETWORKCONFIGURATOR_FREEBSD_HH

#include "config.h"

#include "fwcompiler/OSConfigurator.h"

#include "OSData.h"

namespace fwcompiler {

    class OSConfigurator_freebsd : public OSConfigurator {

        OSData   os_data;

	std::vector<libfwbuilder::InetAddr> virtual_addresses;

        std::string getInterfaceVarName(libfwbuilder::FWObject *iface);
        
	public:

	virtual ~OSConfigurator_freebsd() {};
	OSConfigurator_freebsd(libfwbuilder::FWObjectDatabase *_db,
                               const std::string &fwname,
                               bool ipv6_policy) : 
        OSConfigurator(_db, fwname, ipv6_policy) , os_data() {}

        virtual int prolog();

	virtual std::string myPlatformName();
	virtual void processFirewallOptions();
	virtual void addVirtualAddressForNAT(const libfwbuilder::Address *addr);
	virtual void addVirtualAddressForNAT(const libfwbuilder::Network   *nw);
        void         printPathForAllTools(const std::string &os);
        void         configureInterfaces();
    };
};

#endif
