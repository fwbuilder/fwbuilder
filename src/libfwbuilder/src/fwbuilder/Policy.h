/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef __POLICY_HH_FLAG__
#define __POLICY_HH_FLAG__

#include "fwbuilder/FWObject.h"
#include "fwbuilder/RuleSet.h"

namespace libfwbuilder
{

class Policy : public RuleSet  
{
    public:
    
    Policy();
    virtual ~Policy();
    
    DECLARE_FWOBJECT_SUBTYPE(Policy);

    DECLARE_DISPATCH_METHODS(Policy);
    
/* the following methods provide proper typization */
    PolicyRule* getRuleByNum(int n) {
        return PolicyRule::cast(RuleSet::getRuleByNum(n));
    }
    PolicyRule* insertRuleAtTop() {
        return PolicyRule::cast(RuleSet::insertRuleAtTop());
    }
    PolicyRule* insertRuleBefore(int n){
        return PolicyRule::cast(RuleSet::insertRuleBefore(n));
    }
    PolicyRule* appendRuleAfter(int n){
        return PolicyRule::cast(RuleSet::appendRuleAfter(n));
    }
    PolicyRule* appendRuleAtBottom(){
        return PolicyRule::cast(RuleSet::appendRuleAtBottom());
    }
    
    virtual Rule*   createRule();

};

}

#endif //__POLICY_HH_FLAG__
