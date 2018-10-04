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

#ifndef _OSNETWORKCONFIGURATOR_SOLARIS_HH
#define _OSNETWORKCONFIGURATOR_SOLARIS_HH


#include "fwbuilder/InetAddr.h"

#include <vector>

#include "OSConfigurator_bsd.h"
#include "OSData_pf.h"

/*
 * Of course Solaris has nothing to do with BSD. Class
 * OSConfigurator_solaris inherits OSConfigurator_bsd only because the
 * latter is the base class for all OSConfigurator classes for the
 * pf-ipf-ipfw family. TODO: rename OSConfigurator_bsd to use more
 * generic name, something like OSConfigurator_generic_pf_ipf_family
 */

namespace fwcompiler
{

    class OSConfigurator_solaris : public OSConfigurator_bsd
    {

        OSData_pf os_data;

	std::vector<libfwbuilder::InetAddr> virtual_addresses;

	public:

	virtual ~OSConfigurator_solaris() {};
	OSConfigurator_solaris(libfwbuilder::FWObjectDatabase *_db,
                               libfwbuilder::Firewall *fw,
                               bool ipv6_policy) : 
        OSConfigurator_bsd(_db, fw, ipv6_policy) , os_data() {}

        virtual int prolog();

	virtual std::string myPlatformName();
	virtual std::string printKernelVarsCommands();
        
	virtual void addVirtualAddressForNAT(const libfwbuilder::Address *addr);
	virtual void addVirtualAddressForNAT(const libfwbuilder::Network   *nw);

        virtual std::string configureInterfaces();
    };
};

#endif
