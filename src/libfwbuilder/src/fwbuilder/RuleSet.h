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


#ifndef __RULESET_HH_FLAG__
#define __RULESET_HH_FLAG__

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/InetAddr.h"

namespace libfwbuilder
{

class RuleSet : public FWObject  
{
    private:

    bool ipv4;
    bool ipv6;
    bool top;

    protected:
    
    public:
    
    RuleSet();
    virtual ~RuleSet();
    
    /**
     * This method should create any standard mandatory child objects
     * the object might need.
     */
    virtual void init(FWObjectDatabase *root);
    
    DECLARE_FWOBJECT_SUBTYPE(RuleSet);

    DECLARE_DISPATCH_METHODS(RuleSet);
    
    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr parent);

    virtual FWOptions* getOptionsObject();

    virtual FWObject& shallowDuplicate(const FWObject *obj,
                                       bool preserve_id = true);
    virtual bool cmp(const FWObject *obj, bool recursive=false);

    // Both ipv4 and ipv6 variables can be set to true, which means
    // this is "dual" rule set. When both are false, this is ipv4-only
    // rule set (for backwards compatibility and to avoid having to
    // increment DTD version number)
    
    bool isV4() const { return (ipv4 || (!ipv4 && !ipv6)); }
    bool isV6() const { return (ipv6); }
    bool isDual() const { return (ipv4 && ipv6); }
    bool matchingAddressFamily(int af)
    {
        if (af == AF_INET && isV4()) return true;
        if (af == AF_INET6 && isV6()) return true;
        return false;
    }
    
    void setV4() { ipv4=true; ipv6=false; }
    void setV6() { ipv4=false; ipv6=true; }
    void setDual() { ipv4=true; ipv6=true; }
    
    bool isTop() const { return top; }
    void setTop(bool f) { top=f; }
    
    Rule* getRuleByNum(int n);
    
    Rule* insertRuleAtTop(bool hidden_rule=false);
    Rule* insertRuleBefore(int rule_n);
    
    Rule* appendRuleAfter(int rule_n);
    Rule* appendRuleAtBottom(bool hidden_rule=false);
    
    bool  deleteRule(int rule_n);
    bool  deleteRule(Rule *r);
    
    bool  moveRuleUp(int rule_n);
    bool  moveRuleDown(int rule_n);
    bool  moveRule(int src_rule_n,int dst_rule_n);
    
    bool  disableRule(int rule_n);
    bool  enableRule(int rule_n);
    bool  isRuleDisabled(int rule_n);

    int  getRuleSetSize();
    
    virtual Rule* createRule() = 0;
    
    virtual bool isPrimaryObject() const { return false; }

    void   renumberRules();
        
    /**
     * scan all rules of all rule sets and call setUniqueId() to set
     * unique string id for each rule. These IDs will be carried
     * through calls to duplicate() when firewall object and its rule
     * sets are cloned. These IDs are used by compilers to generate
     * stable labels for chains and such.
     */
    void assignUniqueRuleIds();

    struct UniqueRuleIdsSetter
    {
        void operator()(FWObject *o)
        {
            RuleSet *rs = RuleSet::cast(o);
            if (rs != nullptr) rs->assignUniqueRuleIds();
        }
    };

    
}; //__RULESET_HH_FLAG__

}

#endif

