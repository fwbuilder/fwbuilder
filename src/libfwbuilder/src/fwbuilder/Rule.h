/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org


                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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


#ifndef __RULE_HH_FLAG__
#define __RULE_HH_FLAG__

#include "fwbuilder/Group.h"

namespace libfwbuilder
{

    class FWOptions;

    class RuleElementSrc;
    class RuleElementDst;
    class RuleElementSrv;
    class RuleElementItf;
    class RuleElementOSrc;
    class RuleElementODst;
    class RuleElementOSrv;
    class RuleElementTSrc;
    class RuleElementTDst;
    class RuleElementTSrv;
    class RuleElementItfInb;
    class RuleElementItfOutb;
    class RuleElementInterval;
    class RuleElementRDst;
    class RuleElementRGtw;
    class RuleElementRItf;
    class RuleSet;

class Rule : public Group 
{
    /* these members are used only by compilers and are not stored in XML */
    std::string label;
    bool        fallback;
    bool        hidden;
    std::string unique_id;
    int         abs_rule_number;

    /* compilers store warnings and errors associated with this rule here.
     */
    std::string compiler_message;
    
    public:

    Rule();

    /**
     * This method should create any standard mandatory child objects
     * the object might need.
     */
    virtual void init(FWObjectDatabase *root);
    
    DECLARE_FWOBJECT_SUBTYPE(Rule);

    DECLARE_DISPATCH_METHODS(Rule);
    
    void setPosition(int n);
    int  getPosition() const;

    void setCompilerMessage(const std::string &msg) { compiler_message = msg; }
    std::string getCompilerMessage() { return compiler_message; }
    
    void disable();
    void enable();
    bool isDisabled() const;

    bool isFallback() const { return fallback; }
    void setFallback(bool f) { fallback=f; }

    bool isHidden() const { return hidden; }
    void setHidden(bool f) { hidden=f; }

    std::string getUniqueId() const { return unique_id; }
    void setUniqueId(const std::string &str) { unique_id=str; }

    std::string getLabel() const { return label; }
    void setLabel(const std::string &l) { label=l; }

    std::string getRuleGroupName() const;
    void setRuleGroupName(const std::string &group_name);

    int  getAbsRuleNumber() const { return abs_rule_number; }
    void setAbsRuleNumber(int rn) { abs_rule_number=rn; }

    virtual bool isEmpty();
    virtual bool isDummyRule();

    virtual FWObject& shallowDuplicate(const FWObject *obj,
                                       bool preserve_id = true);

    virtual bool cmp(const FWObject *obj, bool recursive=false);
    
    virtual FWOptions* getOptionsObject() const;

    // find branch ruleset for policy rules with action Branch
    // We may support some kind of  branching in NAT in the future, so
    // lets make this method virtual.
    virtual RuleSet* getBranch();
    virtual void setBranch(RuleSet *ruleset);

    virtual bool isPrimaryObject() const { return false; }
};

class PolicyRule : public Rule 
{
    public:
/*
 * Supported policy rule actions:
 *
 *        Accept - accept the packet, analysis terminates
 *
 *        Reject - reject the packet and send ICMP 'unreachable' or
 *                 TCP RST back to sender, analysis terminates
 *
 *        Deny   - drop the packet, nothing is sent back to sender,
 *                 analysis terminates
 *
 *        Scrub  - run the packet through normalizer (see 'scrub' in
 *                 PF), continue analysis
 *
 *        Return - action used internally, meaning may depend on
 *                 implementation of the policy compiler but generally 
 *                 means return from the block of rules
 *
 *        Skip   - skip N rules down and continue analysis. Used
 *                 internally.
 *
 *        Continue - do nothing, continue analysis. Used internally.
 *
 *        Accounting - generate target firewall platform rule to count
 *                 the packet and continue analysis.
 *
 *        Modify - edit the packet (change some header values, like
 *                 TOS bits) or mark it somehow if the kernel supports 
 *                 that (e.g. target MARK in iptables)
 *
 *	  Branch - 
 *
 *        Route -
 *
 */
    typedef enum { Unknown, 
                   Accept, 
                   Reject, 
                   Deny, 
                   Scrub, 
                   Return, 
                   Skip, 
                   Continue, 
                   Accounting,
                   Modify,
                   Pipe,
                   Custom,
                   Branch} Action;

    typedef enum { Undefined, 
                   Inbound, 
                   Outbound, 
                   Both } Direction;

private:

    libfwbuilder::RuleElementSrc* src_re;
    libfwbuilder::RuleElementDst* dst_re;
    libfwbuilder::RuleElementSrv* srv_re;
    libfwbuilder::RuleElementItf* itf_re;
    libfwbuilder::RuleElementInterval* when_re;
    Action action;
    Direction direction;

protected:
    virtual void updateNonStandardObjectReferences();
    
public:    

    PolicyRule();

    /**
     * This method should create any standard mandatory child objects
     * the object might need.
     */
    virtual void init(FWObjectDatabase *root);

    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML      (xmlNodePtr parent);
    virtual FWObject& shallowDuplicate(const FWObject *obj,
                                       bool preserve_id = true);
    virtual bool cmp(const FWObject *obj, bool recursive=false);

    DECLARE_FWOBJECT_SUBTYPE(PolicyRule);

    DECLARE_DISPATCH_METHODS(PolicyRule);
    
    virtual FWOptions* getOptionsObject() const;

    virtual RuleSet* getBranch();
    virtual void setBranch(RuleSet *ruleset);

    virtual bool isEmpty();
    virtual bool isDummyRule();

    /**
     * Removes reference to given object among
     * children of 'this'.
     */
    virtual void removeRef(FWObject *obj);
    
    /**
     * Add reference to given object to 'this'. In case of a PolicyRule,
     * change action and add parameter if @obj is TagService or RuleSet.
     */
    virtual void addRef(FWObject *obj);
    
    /**
     * if this object has any references as its children, replace IDs
     * these references point to. This overloaded method also replaces
     * references to branch rulesets.
     */
    virtual void replaceReferenceInternal(int old_id, int new_id, int &counter);

    libfwbuilder::RuleElementSrc*  getSrc() ;
    libfwbuilder::RuleElementDst*  getDst() ;
    libfwbuilder::RuleElementSrv*  getSrv() ;
    libfwbuilder::RuleElementItf*  getItf() ;
    libfwbuilder::RuleElementInterval* getWhen() ;

    Action getAction() const { return action; }
    void   setAction(Action act) { action = act; }

    static std::string getActionAsString(int act);
    std::string getActionAsString() const;
    void   setAction(const std::string& act);

    Direction getDirection() const { return direction; }
    void   setDirection(Direction dir) { direction = dir; }
    std::string getDirectionAsString() const;
    void   setDirection(const std::string& dir);

    void setDummySource();
    void setDummyDestination();
    void setDummyService();
    void setDummyInterface();

    bool getLogging() const;
    void setLogging(bool flag);

    // return true if rule does tagging
    bool getTagging() const;
    void setTagging(bool f);

    // return true if rule does routing
    bool getRouting() const;
    void setRouting(bool f);
    
    // return true if rule does classification
    bool getClassification() const;
    void setClassification(bool f);
    
    // find TagService object for rules that do tagging
    FWObject* getTagObject();
    std::string getTagValue();
    void setTagObject(FWObject *tag_object);
};


class NATRule : public Rule 
{
public:

    typedef enum { Translate,
                   Branch } NATAction;

/*
 *  Supported NAT rule types:
 *      
 *      Unknown - Uknown action
 *         
 *      NONAT - Exception to other nat rules
 *
 *      SNAT - Source NAT - Translate Source Address
 *
 *      Masq - Masquerading.
 *
 *      DNAT - Destnation NAT - Translate Destination Address.
 *
 *      SDNAT - Source & Destination NAT - Translate both source and
 *      destination addresses.
 *
 *      SNetNAT - Source Network Translation - Translate source
 *      address network.
 *
 *      DNetNAT - Destination Network Translation - Translate
 *      destination address network.
 *
 *      Redirect - Redirect to firewall - Translate destination to a
 *      firewall address.
 *
 *      Return - Internal use for rule chains.
 *
 *      Skip - 
 *
 *      Continue - Internal use for nat rules with negation.
 *
 *      LB - NAT Rule that does load ballencing.
 *
 *  NOTE: Not all types supported on all platforms.
 *
 */
    typedef enum { Unknown, 
                   NONAT,
                   NATBranch,
                   SNAT, 
                   Masq, 
                   DNAT, 
                   SDNAT,
                   SNetnat, 
                   DNetnat, 
                   Redirect , 
                   Return , 
                   Skip , 
                   Continue , 
                   LB }  NATRuleTypes;
    
private:

    libfwbuilder::RuleElementOSrc*      osrc_re;
    libfwbuilder::RuleElementODst*      odst_re;
    libfwbuilder::RuleElementOSrv*      osrv_re;
    libfwbuilder::RuleElementTSrc*      tsrc_re;
    libfwbuilder::RuleElementTDst*      tdst_re;
    libfwbuilder::RuleElementTSrv*      tsrv_re;
    libfwbuilder::RuleElementItfInb*    itf_inb_re;
    libfwbuilder::RuleElementItfOutb*   itf_outb_re;
    libfwbuilder::RuleElementInterval* when_re;
    NATAction action;
    NATRuleTypes  rule_type;

public:

    NATRule();

    /**
     * This method should create any standard mandatory child objects
     * the object might need.
     */
    virtual void init(FWObjectDatabase *root);
    
    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML      (xmlNodePtr parent);

    DECLARE_FWOBJECT_SUBTYPE(NATRule);

    DECLARE_DISPATCH_METHODS(NATRule);
    
    virtual FWOptions* getOptionsObject() const;

    virtual RuleSet* getBranch();
    virtual void setBranch(RuleSet *ruleset);

    virtual bool isEmpty();

    /**
     * Removes reference to given object among
     * children of 'this'.
     */
    virtual void removeRef(FWObject *obj);
    
    /**
     * Add reference to given object to 'this'. In case of a PolicyRule,
     * change action and add parameter if @obj is TagService or RuleSet.
     */
    virtual void addRef(FWObject *obj);
        
    libfwbuilder::RuleElementOSrc* getOSrc();
    libfwbuilder::RuleElementODst* getODst();
    libfwbuilder::RuleElementOSrv* getOSrv();
    libfwbuilder::RuleElementTSrc* getTSrc();
    libfwbuilder::RuleElementTDst* getTDst();
    libfwbuilder::RuleElementTSrv* getTSrv();
    libfwbuilder::RuleElementItfInb* getItfInb();
    libfwbuilder::RuleElementItfOutb* getItfOutb();
    libfwbuilder::RuleElementInterval* getWhen();

    NATAction getAction() const { return action; }
    void setAction(NATAction act) { action = act; }

    static std::string getActionAsString(int act);
    std::string getActionAsString() const;
    void setAction(const std::string& act);
    
    NATRuleTypes getRuleType() const;
    std::string  getRuleTypeAsString() const;
    void setRuleType(NATRuleTypes rt);

    virtual FWObject& shallowDuplicate(const FWObject *obj, bool preserve_id = true);
    virtual bool cmp(const FWObject *obj, bool recursive=false);
};

class RoutingRule : public Rule 
{
    public:
        
    typedef enum { Undefined,
                   SinglePath,
                   MultiPath }  RoutingRuleTypes;

    private:

    RoutingRuleTypes rule_type;
    std::string sorted_dst_ids;
    // int ecmp_id; // Unused
    
    public:
                    
    RoutingRule();

    /**
     * This method should create any standard mandatory child objects
     * the object might need.
     */
    virtual void init(FWObjectDatabase *root);
    
    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr parent);

    DECLARE_FWOBJECT_SUBTYPE(RoutingRule);

    DECLARE_DISPATCH_METHODS(RoutingRule);
    
    virtual FWOptions* getOptionsObject() const;
    virtual RuleSet* getBranch();
    virtual bool isEmpty();
    
    int  getMetric() const;
    void setMetric(int metric);
    void setMetric(std::string metric);
    std::string getMetricAsString() const;

    libfwbuilder::RuleElementRDst* getRDst() const;
    libfwbuilder::RuleElementRGtw* getRGtw() const;
    libfwbuilder::RuleElementRItf* getRItf() const;
    
    RoutingRuleTypes getRuleType() const;
    std::string      getRuleTypeAsString() const;
    void             setRuleType(RoutingRuleTypes rt);
    
    void        setSortedDstIds(const std::string& ids);
    std::string getSortedDstIds() const;

    virtual FWObject& duplicate(const FWObject *obj, bool preserve_id = true);
};

}

#endif //__RULE_HH_FLAG__
