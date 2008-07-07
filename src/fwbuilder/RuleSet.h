
/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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


#ifndef __RULESET_HH_FLAG__
#define __RULESET_HH_FLAG__

#include <fwbuilder/FWObject.h>
#include <fwbuilder/Rule.h>

namespace libfwbuilder
{

class RuleSet : public FWObject  
{
    private:

    bool ipv6;

    protected:
    
    void   renumberRules();
        
    public:
    
    RuleSet();
    RuleSet(const FWObject *root,bool prepopulate);
    virtual ~RuleSet();
    
    DECLARE_FWOBJECT_SUBTYPE(RuleSet);

    virtual void fromXML(xmlNodePtr parent) throw(FWException);
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);

    virtual FWObject& shallowDuplicate(const FWObject *obj,
                                       bool preserve_id = true)
        throw(FWException);
    
    bool isV6() const { return ipv6; }
    void setV6(bool f) { ipv6=f; }
    
    Rule* getRuleByNum(int n);
    
    Rule* insertRuleAtTop();
    Rule* insertRuleBefore(int rule_n);
    
    Rule* appendRuleAfter(int rule_n);
    Rule* appendRuleAtBottom();
    
    bool  deleteRule(int rule_n);
    bool  deleteRule(Rule *r);
    
    bool  moveRuleUp(int rule_n);
    bool  moveRuleDown(int rule_n);
    bool  moveRule(int src_rule_n,int dst_rule_n);
    
    bool  disableRule(int rule_n);
    bool  enableRule(int rule_n);
    bool  isRuleDisabled(int rule_n);

    int  getRuleSetSize();
    
    virtual Rule*   createRule()  =0;
    
}; //__RULESET_HH_FLAG__

}

#endif

