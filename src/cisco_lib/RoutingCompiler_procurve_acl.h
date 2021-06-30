/* 

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: RoutingCompiler_procurve.h -1   $

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


#ifndef __ROUTINGCOMPILER_PROCURVE_ACL_HH__
#define __ROUTINGCOMPILER_PROCURVE_ACL_HH__


#include "RoutingCompiler_iosacl.h"

namespace libfwbuilder {
    class RuleElementRDst;
    class RuleElementRItf;
    class RuleElementRGtw;
};


namespace fwcompiler
{

    class RoutingCompiler_procurve_acl : public RoutingCompiler_iosacl
    {
    protected:

	virtual std::string myPlatformName();

    public:

	RoutingCompiler_procurve_acl(libfwbuilder::FWObjectDatabase *_db,
                                     libfwbuilder::Firewall *fw, bool ipv6_policy,
                                     fwcompiler::OSConfigurator *_oscnf) :
        RoutingCompiler_iosacl(_db, fw, ipv6_policy, _oscnf) {}
        
	virtual int  prolog();

    };

}

#endif
