/* 

                          Firewall Builder

                 Copyright (C) 2002-2011 NetCitadel, LLC

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

#ifndef __SPLIT_BY_NETWORK_ZONES_FOR_RE_HH
#define __SPLIT_BY_NETWORK_ZONES_FOR_RE_HH

#include "fwbuilder/RuleElement.h"
#include "fwcompiler/RuleProcessor.h"

namespace libfwbuilder {
    class Address;
    class Rule;
};

namespace fwcompiler
{

/**
 * this processor splits rules if objects in rule element
 * re_type belong to different network zones
 */
    class splitByNetworkZonesForRE : public BasicRuleProcessor
    {
        std::string re_type;
        std::map<int,libfwbuilder::Rule*> rules;
        void AddToInterface(int interface_id,
                            libfwbuilder::Address *addr,
                            libfwbuilder::Rule *rule);
public:
        splitByNetworkZonesForRE(const std::string &name,const std::string &_type) :
            BasicRuleProcessor(name) {re_type=_type; }
        virtual bool processNext();
    };


}

#endif
