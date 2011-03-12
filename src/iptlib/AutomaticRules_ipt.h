/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef __AUTOMATICRULES_IPT_HH__
#define __AUTOMATICRULES_IPT_HH__

#include "AutomaticRules.h"


namespace libfwbuilder
{
    class Address;
    class Firewall;
    class Interface;
    class Service;
};

namespace fwcompiler
{
    
    class AutomaticRules_ipt : public AutomaticRules
    {

    public:

        AutomaticRules_ipt(libfwbuilder::Firewall *fw,
                   libfwbuilder::Library *presistent_objects) : 
            AutomaticRules(fw, presistent_objects) {}
    
        virtual libfwbuilder::PolicyRule* addMgmtRule(
            libfwbuilder::Address* src,
            libfwbuilder::Address* dst,
            libfwbuilder::Service* service,
            libfwbuilder::Interface* iface,
            const libfwbuilder::PolicyRule::Direction direction,
            const libfwbuilder::PolicyRule::Action action,
            const std::string &label,
            bool related = false);

        void addConntrackRule();
        void addFailoverRules();
    
    };

};


#endif
