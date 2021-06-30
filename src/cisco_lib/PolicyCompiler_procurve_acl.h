/* 

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

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

#ifndef __POLICYCOMPILER_PROCURVE_ACL_HH
#define __POLICYCOMPILER_PROCURVE_ACL_HH


#include "PolicyCompiler_iosacl.h"

namespace libfwbuilder {
    class FWObjectDatabase;
    class Firewall;
};

namespace fwcompiler {
    class OSConfigurator;
};


namespace fwcompiler {

    class PolicyCompiler_procurve_acl : public PolicyCompiler_iosacl {

	protected:

	virtual std::string myPlatformName();
        virtual std::string printAccessGroupCmd(ciscoACL *acl, bool neg=false);
        virtual ciscoACL* createACLObject(const std::string &n,
                                          libfwbuilder::Interface *intf,
                                          const std::string &d="in",
                                          bool _ip_list=false);
        
	public:

	PolicyCompiler_procurve_acl(libfwbuilder::FWObjectDatabase *_db,
                                    libfwbuilder::Firewall *fw,
                                    bool ipv6_policy,
                                    fwcompiler::OSConfigurator *_oscnf);
        virtual ~PolicyCompiler_procurve_acl() {}

	virtual int prolog();

        virtual std::string printClearCommands();
    };

}

#endif
