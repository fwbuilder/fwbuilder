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


#ifndef __RULE_HH_FLAG__
#define __RULE_HH_FLAG__

#include <fwbuilder/Group.h>

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

    public:

    Rule();
    Rule(const FWObject *root,bool prepopulate);

    DECLARE_FWOBJECT_SUBTYPE(Rule);

    void setPosition(int n);
    int  getPosition() const;

    void disable();
    void enable();
    bool isDisabled() const;

    bool isFallback() const { return fallback; }
    void setFallback(bool f) { fallback=f; }

    bool isHidden() const { return hidden; }
    void setHidden(bool f) { hidden=f; }

    int getInterfaceId() const { return getInt("interface_id"); }
    void setInterfaceId(int iface_id) { setInt("interface_id", iface_id); }

    std::string getInterfaceStr() const { return getStr("interface_str"); }
    void setInterfaceStr(const std::string &iface_str)
    { setStr("interface_str", iface_str); }

    std::string getUniqueId() const { return unique_id; }
    void setUniqueId(const std::string &str) { unique_id=str; }

    std::string getLabel() const { return label; }
    void setLabel(const std::string &l) { label=l; }

    std::string getRuleGroupName() const;
    void setRuleGroupName(const std::string &group_name);

    int  getAbsRuleNumber() const { return abs_rule_number; }
    void setAbsRuleNumber(int rn) { abs_rule_number=rn; }

    virtual bool isEmpty();

    virtual FWObject& shallowDuplicate(const FWObject *obj,
                                       bool preserve_id = true)
        throw(FWException);

    virtual FWOptions* getOptionsObject();

    // find branch ruleset for policy rules with action Branch
    // We may support some kind of  branching in NAT in the future, so
    // lets make this method virtual.
    virtual RuleSet* getBranch();
    virtual void setBranch(RuleSet *ruleset);

};

class PolicyRule : public Rule 
{
    libfwbuilder::RuleElementSrc*      src_re;
    libfwbuilder::RuleElementDst*      dst_re;
    libfwbuilder::RuleElementSrv*      srv_re;
    libfwbuilder::RuleElementItf*      itf_re;
    libfwbuilder::RuleElementInterval* when_re;
    
    
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
                   Tag,
                   Classify,
                   Custom,
                   Branch,
		   Route} Action;

    typedef enum { Undefined, 
                   Inbound, 
                   Outbound, 
                   Both } Direction;



    PolicyRule();
    PolicyRule(const FWObject *root,bool prepopulate);
    virtual void fromXML(xmlNodePtr parent) throw(FWException);
    virtual xmlNodePtr toXML      (xmlNodePtr parent) throw(FWException);
    virtual FWObject& shallowDuplicate(const FWObject *obj, bool preserve_id = true) throw(FWException);

    DECLARE_FWOBJECT_SUBTYPE(PolicyRule);
    virtual FWOptions* getOptionsObject();

    virtual RuleSet* getBranch();
    virtual void setBranch(RuleSet *ruleset);

    virtual bool isEmpty();

    /**
     * Removes reference to given object among
     * children of 'this'.
     */
    virtual void removeRef(FWObject *obj);
    
    libfwbuilder::RuleElementSrc*  getSrc() ;
    libfwbuilder::RuleElementDst*  getDst() ;
    libfwbuilder::RuleElementSrv*  getSrv() ;
    libfwbuilder::RuleElementItf*  getItf() ;
    libfwbuilder::RuleElementInterval* getWhen() ;

    Action getAction() const;
    void   setAction(Action act);
    std::string getActionAsString() const;
    void   setAction(const std::string& act);

    Direction getDirection() const;
    void   setDirection(Direction dir);
    std::string getDirectionAsString() const;
    void   setDirection(const std::string& dir);

    bool   getLogging() const;
    void   setLogging(bool flag);

    // find TagService object for rules with action Tag
    FWObject* getTagObject();
    std::string getTagValue();
    void setTagObject(FWObject *tag_object);
};

class NATRule : public Rule 
{
    libfwbuilder::RuleElementOSrc*      osrc_re;
    libfwbuilder::RuleElementODst*      odst_re;
    libfwbuilder::RuleElementOSrv*      osrv_re;
    libfwbuilder::RuleElementTSrc*      tsrc_re;
    libfwbuilder::RuleElementTDst*      tdst_re;
    libfwbuilder::RuleElementTSrv*      tsrv_re;
    libfwbuilder::RuleElementInterval* when_re;
    
    public:

    typedef enum { Unknown, 
                   NONAT, 
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

    NATRuleTypes  rule_type;

    public:

    NATRule();
    NATRule(const FWObject *root,bool prepopulate);
    virtual void fromXML(xmlNodePtr parent) throw(FWException);
    virtual xmlNodePtr toXML      (xmlNodePtr parent) throw(FWException);

    DECLARE_FWOBJECT_SUBTYPE(NATRule);
    virtual FWOptions* getOptionsObject();

    virtual RuleSet* getBranch();
    virtual void setBranch(RuleSet *ruleset);

    virtual bool isEmpty();

    libfwbuilder::RuleElementOSrc* getOSrc();
    libfwbuilder::RuleElementODst* getODst();
    libfwbuilder::RuleElementOSrv* getOSrv();
    libfwbuilder::RuleElementTSrc* getTSrc();
    libfwbuilder::RuleElementTDst* getTDst();
    libfwbuilder::RuleElementTSrv* getTSrv();
    libfwbuilder::RuleElementInterval* getWhen();

    NATRuleTypes getRuleType() const;
    std::string  getRuleTypeAsString() const;
    void         setRuleType(NATRuleTypes rt);

    virtual FWObject& shallowDuplicate(const FWObject *obj, bool preserve_id = true) throw(FWException);
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
    int ecmp_id;
    
    public:
                    
    RoutingRule();
    RoutingRule(const FWObject *root,bool prepopulate);
    virtual void fromXML(xmlNodePtr parent) throw(FWException);
    virtual xmlNodePtr toXML      (xmlNodePtr parent) throw(FWException);

    DECLARE_FWOBJECT_SUBTYPE(RoutingRule);
    virtual FWOptions* getOptionsObject();
    virtual RuleSet* getBranch();
    virtual bool isEmpty() const;
    
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

    virtual FWObject& duplicate(const FWObject *obj, bool preserve_id = true) throw(FWException);
};

}

#endif //__RULE_HH_FLAG__
