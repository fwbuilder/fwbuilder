/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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


#ifndef __ROUTINGCOMPILER_NXOSACL_HH__
#define __ROUTINGCOMPILER_NXOSACL_HH__


#include "fwcompiler/RoutingCompiler.h"
#include "fwbuilder/RuleElement.h"

#include "RoutingCompiler_cisco.h"

namespace libfwbuilder {
    class RuleElementRDst;
    class RuleElementRItf;
    class RuleElementRGtw;
};


namespace fwcompiler
{

    class RoutingCompiler_nxosacl : public RoutingCompiler_cisco
    {
    protected:

	virtual std::string myPlatformName();

        /**
         *  this inspector replaces references to hosts and firewalls
         *  in dst and gw with references to their interfaces, except
         *  for interfaces of the firewall found in gw, which are left
         *  intact.
         */
        DECLARE_ROUTING_RULE_PROCESSOR(ExpandMultipleAddressesExceptInterface);

        DECLARE_ROUTING_RULE_PROCESSOR(checkRItfAndGw);

        class PrintRule : public RoutingCompiler_cisco::PrintRule
        {
            public:
            PrintRule(const std::string &name);
            virtual bool processNext();
            virtual std::string RoutingRuleToString(libfwbuilder::RoutingRule *r);
            virtual std::string _printRGtw(libfwbuilder::RoutingRule *r);
            virtual std::string _printRItf(libfwbuilder::RoutingRule *r);
        };
        friend class RoutingCompiler_nxosacl::PrintRule;
        
    public:

	RoutingCompiler_nxosacl(libfwbuilder::FWObjectDatabase *_db,
                               libfwbuilder::Firewall *fw, bool ipv6_policy,
                               fwcompiler::OSConfigurator *_oscnf) : RoutingCompiler_cisco(_db, fw, ipv6_policy, _oscnf) {};

	virtual int  prolog();
	virtual void compile();
	virtual void epilog();

    };

}

#endif
