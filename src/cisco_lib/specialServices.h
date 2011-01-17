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


#ifndef __SPECIALSERVICES_HH
#define __SPECIALSERVICES_HH

#include "fwbuilder/RuleElement.h"
#include "fwcompiler/RuleProcessor.h"

namespace libfwbuilder {
    class Address;
    class Rule;
};

namespace fwcompiler
{

    /**
     * this processor checks for the services which require
     * special treatment.  Some of these will be checking for
     * source or destination object as well because special
     * command may need to be generated in case source or
     * destination is a firewall itself. Therefore this processor
     * should be called after converting to atomic rules, but
     * before interface addresses in source and destination are
     * expanded.
     */
    class SpecialServices : public BasicRuleProcessor
    {
        std::string re_type;
public:
        SpecialServices(const std::string &name, const std::string &_type) :
            BasicRuleProcessor(name) {re_type=_type; }
        virtual bool processNext();
    };
}

#endif
