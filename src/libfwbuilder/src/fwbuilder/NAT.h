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


#ifndef __NAT_HH_FLAG__
#define __NAT_HH_FLAG__

#include "fwbuilder/FWObject.h"
#include "fwbuilder/RuleSet.h"

namespace libfwbuilder
{

class NAT : public RuleSet  
{
    public:
    
    NAT();
    virtual ~NAT();
    
    DECLARE_FWOBJECT_SUBTYPE(NAT);

    DECLARE_DISPATCH_METHODS(NAT);
    
/* the following methods provide proper typization */
    NATRule* getRuleByNum(int n) { return NATRule::cast(RuleSet::getRuleByNum(n)); }
    NATRule* insertRuleAtTop() { return NATRule::cast(RuleSet::insertRuleAtTop()); }
    NATRule* insertRuleBefore(int n){ return NATRule::cast(RuleSet::insertRuleBefore(n));}
    NATRule* appendRuleAfter(int n){ return NATRule::cast(RuleSet::appendRuleAfter(n));}
    NATRule* appendRuleAtBottom(){ return NATRule::cast(RuleSet::appendRuleAtBottom());}
    
    virtual Rule*   createRule();
};

}

#endif //__NAT_HH_FLAG__

