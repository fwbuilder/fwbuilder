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

#include <assert.h>
#include <sstream>
#include <iostream>

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/Firewall.h>
#include <fwbuilder/Rule.h>
#include <fwbuilder/RuleElement.h>
#include <fwbuilder/Policy.h>
#include <fwbuilder/FWOptions.h>
#include <fwbuilder/XMLTools.h>
#include <fwbuilder/Policy.h>
#include <fwbuilder/NAT.h>
#include <fwbuilder/Routing.h>
#include <fwbuilder/TagService.h>

using namespace std;
using namespace libfwbuilder;
 
const char *Rule::TYPENAME={"Rule"};

Rule::Rule()
{
    setInt("position",0);
    enable();
    setFallback(false);
    setHidden(false);
    setRuleGroupName("");
}

Rule::Rule(const FWObject *root,bool prepopulate) : Group(root,prepopulate)
{
    setInt("position",0);
    enable();
    setFallback(false);
    setHidden(false);
}

FWOptions* Rule::getOptionsObject()  {    return NULL;              }
RuleSet*   Rule::getBranch()         {    return NULL;              }
void       Rule::setPosition(int n)  {    setInt("position",n);     }
int        Rule::getPosition() const {    return getInt("position");}
void       Rule::disable()           {    setBool("disabled",true); }
void       Rule::enable()            {    setBool("disabled",false);}
bool       Rule::isDisabled() const  {    return( getBool("disabled") );}
bool       Rule::isEmpty()           {    return false;             }

void Rule::setBranch(RuleSet*) {};

string Rule::getRuleGroupName() const { return getStr("group"); }

void Rule::setRuleGroupName(const std::string &group_name)
{
    setStr("group", group_name);
}



FWObject& Rule::shallowDuplicate(const FWObject *x, bool preserve_id) throw(FWException)
{
    const Rule *rx=Rule::constcast(x);
    fallback=rx->fallback;
    hidden=rx->hidden;
    label=rx->label;
    unique_id=rx->unique_id;
    abs_rule_number=rx->abs_rule_number;

    return  FWObject::shallowDuplicate(x,preserve_id);
}


/***************************************************************************/

const char *PolicyRule::TYPENAME={"PolicyRule"};

PolicyRule::PolicyRule()
{
    setStr("action","Deny");

    src_re = NULL;
    dst_re = NULL;
    srv_re = NULL;
    itf_re = NULL;
    when_re = NULL;
}

PolicyRule::PolicyRule(const FWObject *root,bool prepopulate) : Rule(root,prepopulate)
{
    setStr("action","Deny");

    src_re = NULL;
    dst_re = NULL;
    srv_re = NULL;
    itf_re = NULL;
    when_re = NULL;

    if (prepopulate)
    {
        FWObject         *re;
        FWObjectDatabase *db=(FWObjectDatabase*)root;
        assert(db);

        re=db->create("Src");  assert(re!=NULL);
        add(re); src_re = RuleElementSrc::cast(re);

        re=db->create("Dst");  assert(re!=NULL);
        add(re); dst_re = RuleElementDst::cast(re);

        re=db->create("Srv");  assert(re!=NULL);
        add(re); srv_re = RuleElementSrv::cast(re);

        re=db->create("When"); assert(re!=NULL);
        add(re); when_re = RuleElementInterval::cast(re);

        re=db->create("Itf");  assert(re!=NULL);
        add(re); itf_re = RuleElementItf::cast(re);

        add( db->create(PolicyRuleOptions::TYPENAME) );
    }
}

FWObject& PolicyRule::shallowDuplicate(const FWObject *x, bool preserve_id) throw(FWException)
{
    const PolicyRule *rx=PolicyRule::constcast(x);
    setDirection(rx->getDirection());
    setAction(rx->getAction());
    setLogging(rx->getLogging());

    src_re = NULL;
    dst_re = NULL;
    srv_re = NULL;
    itf_re = NULL;
    when_re = NULL;

    return  Rule::shallowDuplicate(x,preserve_id);
}


RuleElementSrc*  PolicyRule::getSrc()
{
    if (src_re) return src_re;
    src_re = RuleElementSrc::cast(getFirstByType(RuleElementSrc::TYPENAME));
    return src_re;
}

RuleElementDst*  PolicyRule::getDst()
{
    if (dst_re) return dst_re;
    dst_re = RuleElementDst::cast(getFirstByType(RuleElementDst::TYPENAME));
    return dst_re;
}

RuleElementSrv*  PolicyRule::getSrv()
{
    if (srv_re) return srv_re;
    srv_re = RuleElementSrv::cast(getFirstByType(RuleElementSrv::TYPENAME));
    return srv_re;
}

RuleElementItf*  PolicyRule::getItf()
{
    if (itf_re) return itf_re;
    itf_re = RuleElementItf::cast(getFirstByType(RuleElementItf::TYPENAME));
    return itf_re;
}

RuleElementInterval* PolicyRule::getWhen()
{
    if (when_re) return when_re;
    when_re = RuleElementInterval::cast(getFirstByType(RuleElementInterval::TYPENAME));
    return when_re;
}


bool PolicyRule::isEmpty()
{
  return (getSrc()->isAny() && 
          getDst()->isAny() && 
          getSrv()->isAny() && 
          getItf()->isAny());
}

PolicyRule::Action PolicyRule::getAction()  const
{
    string a=getActionAsString();
    if (a=="Accept")     return Accept;
    if (a=="Reject")     return Reject;
    if (a=="Scrub")      return Scrub;
    if (a=="Return")     return Return;
    if (a=="Skip")       return Skip;
    if (a=="Continue")   return Continue;
    if (a=="Accounting") return Accounting;
    if (a=="Modify")     return Modify;
    if (a=="Tag")        return Tag;
    if (a=="Pipe")       return Pipe;
    if (a=="Classify")   return Classify;
    if (a=="Custom")     return Custom;
    if (a=="Branch")     return Branch;
    if (a=="Route")      return Route;
    return Deny;
}

void   PolicyRule::setAction(PolicyRule::Action act)
{
    switch (act) {
    case Accept:     setAction("Accept");   return;
    case Reject:     setAction("Reject");   return;
    case Scrub:      setAction("Scrub");    return;
    case Return:     setAction("Return");   return;
    case Skip:       setAction("Skip");     return;
    case Continue:   setAction("Continue"); return;
    case Accounting: setAction("Accounting"); return;
    case Modify:     setAction("Modify");   return;
    case Tag:        setAction("Tag");      return;
    case Pipe:       setAction("Pipe");     return;
    case Classify:   setAction("Classify"); return;
    case Custom:     setAction("Custom");   return;
    case Branch:     setAction("Branch");   return;
    case Route:      setAction("Route");    return;
    default:         setAction("Deny");     return;
    }
}

string PolicyRule::getActionAsString() const
{
    string res = getStr("action");
    if (res.empty()) res = "Deny";
    return res;
}

void   PolicyRule::setAction(const string& act)
{
    setStr("action",(act.empty())?"Deny":act);
}

PolicyRule::Direction PolicyRule::getDirection() const
{
    string d=getDirectionAsString();
    if (d=="Inbound")   return Inbound;
    if (d=="Outbound")  return Outbound;
    return Both;
}

void   PolicyRule::setDirection(PolicyRule::Direction dir)
{
    switch (dir) {
    case Inbound:   setDirection("Inbound");  return;
    case Outbound:  setDirection("Outbound"); return;
    default  :      setDirection("Both");     return;
    }
}

string PolicyRule::getDirectionAsString() const
{
    string res = getStr("direction");
    if (res.empty()) res = "Both";
    return res;
}

void   PolicyRule::setDirection(const string& dir)
{
    setStr("direction",(dir.empty())?"Both":dir);
}

bool   PolicyRule::getLogging() const    { return getBool("log"); }
void   PolicyRule::setLogging(bool flag) { setBool("log",flag);   }


void PolicyRule::fromXML(xmlNodePtr root) throw(FWException)
{
    const char* n;

    FWObject::fromXML(root);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("position")));
    if(n)
    {
        setInt("position",atoi(n));
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("disabled")));
    if(n)
    {
        setStr("disabled",n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("action")));
    if(n)
    {
        setStr("action",n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("log")));
    if(n)
    {
        setStr("log",n);
        FREEXMLBUFF(n);
    }


    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("interface")));
    if(n)
    {
        setStr("interface",n);
        FREEXMLBUFF(n);
    }

    n= FROMXMLCAST(xmlGetProp(root,TOXMLCAST("direction")));
    if(n)
    {
        setStr("direction",n);
        FREEXMLBUFF(n);
    }

    n= FROMXMLCAST(xmlGetProp(root,TOXMLCAST("group")));
    if(n)
    {
        setStr("group",n);
        FREEXMLBUFF(n);
    }

}

xmlNodePtr PolicyRule::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
//    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));

    FWObject *o;
    
    if ( (o=getFirstByType( RuleElementSrc::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementDst::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementSrv::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementItf::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementInterval::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( PolicyRuleOptions::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( Policy::TYPENAME ))!=NULL )
	o->toXML(me);

    return me;
}

FWOptions* PolicyRule::getOptionsObject()
{
    return FWOptions::cast( getFirstByType(PolicyRuleOptions::TYPENAME) );
}

RuleSet*   PolicyRule::getBranch()
{
    FWObject *fw = this;
    while (fw && !Firewall::isA(fw)) fw = fw->getParent();
    assert(fw!=NULL);
    string branch_id = getOptionsObject()->getStr("branch_id");
    if (!branch_id.empty())
    {
        return RuleSet::cast(getRoot()->findInIndex(
                                 FWObjectDatabase::getIntId(branch_id)));
    } else
    {
        string branch_name = getOptionsObject()->getStr("branch_name");
        if (!branch_name.empty())
        {
            return RuleSet::cast(
                fw->findObjectByName(Policy::TYPENAME, branch_name));
        } else
            return NULL;
    }
}

void PolicyRule::setBranch(RuleSet* ruleset)
{
    string branch_id = 
        (ruleset) ? FWObjectDatabase::getStringId(ruleset->getId()) : "";
    getOptionsObject()->setStr("branch_id", branch_id);
}

void PolicyRule::setTagObject(FWObject *tag_object)
{
    string tag_id =
        (tag_object) ? FWObjectDatabase::getStringId(tag_object->getId()) : "";
    getOptionsObject()->setStr("tagobject_id", tag_id);
}

FWObject* PolicyRule::getTagObject()
{
    if (getAction() == Tag)
    {
        string tagobj_id = getOptionsObject()->getStr("tagobject_id");
        if (!tagobj_id.empty())
        {
            return getRoot()->findInIndex(
                FWObjectDatabase::getIntId(tagobj_id));
        }
    }
    return NULL;
}

string PolicyRule::getTagValue()
{
    if (getAction() == Tag)
    {
        TagService *tagobj = TagService::cast(getTagObject());
        if (tagobj)  return tagobj->getCode();
        else         return getOptionsObject()->getStr("tagvalue");
    }
    return "";
}

/**
 * Removes reference to given object among children of 'this'. In case
 * of PolicyRule we should also clear reference to it if action is
 * Branch. Caveat: clear reference to it even if action is not branch
 * right now but was in the past and reference got stuck in options.
 *
 * Do the same for the TagService
 */
void PolicyRule::removeRef(FWObject *obj)
{
    if (RuleSet::cast(obj))
    {
        string branch_id = FWObjectDatabase::getStringId(obj->getId());
        string rule_branch_id = getOptionsObject()->getStr("branch_id");
        if (branch_id == rule_branch_id)
            getOptionsObject()->setStr("branch_id", "");
    }

    if (TagService::cast(obj))
    {
        string tag_id = FWObjectDatabase::getStringId(obj->getId());
        string rule_tag_id = getOptionsObject()->getStr("tagobject_id");
        if (tag_id == rule_tag_id)
            getOptionsObject()->setStr("tagobject_id", "");
    }

    FWObject::removeRef(obj);
}

/***************************************************************************/

const char *NATRule::TYPENAME={"NATRule"};

NATRule::NATRule() : Rule()
{
    rule_type=Unknown;

    osrc_re = NULL;
    odst_re = NULL;
    osrv_re = NULL;
    tsrc_re = NULL;
    tdst_re = NULL;
    tsrv_re = NULL;
    when_re = NULL;
}

NATRule::NATRule(const FWObject *root,bool prepopulate) : Rule(root,prepopulate)
{
    rule_type=Unknown;

    osrc_re = NULL;
    odst_re = NULL;
    osrv_re = NULL;
    tsrc_re = NULL;
    tdst_re = NULL;
    tsrv_re = NULL;
    when_re = NULL;

    if (prepopulate)
    {
        FWObject         *re;
        FWObjectDatabase *db=(FWObjectDatabase*)(root);
        assert(db);

        re=db->create("OSrc");  assert(re!=NULL); add(re);
        re=db->create("ODst");  assert(re!=NULL); add(re);
        re=db->create("OSrv");  assert(re!=NULL); add(re);

        re=db->create("TSrc");  assert(re!=NULL); add(re);
        re=db->create("TDst");  assert(re!=NULL); add(re);
        re=db->create("TSrv");  assert(re!=NULL); add(re);

        add( db->create(NATRuleOptions::TYPENAME) );
    }
}

RuleElementOSrc*  NATRule::getOSrc()
{
    if (osrc_re) return osrc_re;
    osrc_re = RuleElementOSrc::cast(getFirstByType(RuleElementOSrc::TYPENAME));
    return osrc_re;
}

RuleElementODst*  NATRule::getODst()
{
    if (odst_re) return odst_re;
    odst_re = RuleElementODst::cast(getFirstByType(RuleElementODst::TYPENAME));
    return odst_re;
}

RuleElementOSrv*  NATRule::getOSrv()
{
    if (osrv_re) return osrv_re;
    osrv_re = RuleElementOSrv::cast(getFirstByType(RuleElementOSrv::TYPENAME));
    return osrv_re;
}

RuleElementTSrc*  NATRule::getTSrc()
{
    if (tsrc_re) return tsrc_re;
    tsrc_re = RuleElementTSrc::cast(getFirstByType(RuleElementTSrc::TYPENAME));
    return tsrc_re;
}

RuleElementTDst*  NATRule::getTDst()
{
    if (tdst_re) return tdst_re;
    tdst_re = RuleElementTDst::cast(getFirstByType(RuleElementTDst::TYPENAME));
    return tdst_re;
}

RuleElementTSrv*  NATRule::getTSrv()
{
    if (tsrv_re) return tsrv_re;
    tsrv_re = RuleElementTSrv::cast(getFirstByType(RuleElementTSrv::TYPENAME));
    return tsrv_re;
}

RuleElementInterval* NATRule::getWhen()
{
    if (when_re) return when_re;
    when_re = RuleElementInterval::cast(getFirstByType(RuleElementInterval::TYPENAME));
    return when_re;
}

bool NATRule::isEmpty()
{
    RuleElement *osrc=getOSrc();
    RuleElement *odst=getODst();
    RuleElement *osrv=getOSrv();

    RuleElement *tsrc=getTSrc();
    RuleElement *tdst=getTDst();
    RuleElement *tsrv=getTSrv();

    return (osrc->isAny() && odst->isAny() && osrv->isAny() && tsrc->isAny() && tdst->isAny() && tsrv->isAny());
}

void NATRule::fromXML(xmlNodePtr root) throw(FWException)
{
    const char* n;

    FWObject::fromXML(root);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("disabled")));
    if(n)
    {
        setStr("disabled",n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("position")));
    if(n)
    {
        setStr("position",n);
        FREEXMLBUFF(n);
    }

    n= FROMXMLCAST(xmlGetProp(root,TOXMLCAST("group")));
    if(n)
    {
        setStr("group",n);
        FREEXMLBUFF(n);
    }

}

xmlNodePtr NATRule::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
//    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));

    FWObject *o;

    if ( (o=getFirstByType( RuleElementOSrc::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementODst::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementOSrv::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementTSrc::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementTDst::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementTSrv::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementInterval::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( NATRuleOptions::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( NAT::TYPENAME ))!=NULL )
	o->toXML(me);

    return me;
}

FWOptions* NATRule::getOptionsObject()
{
    return FWOptions::cast( getFirstByType(NATRuleOptions::TYPENAME) );
}

RuleSet*   NATRule::getBranch()
{
    FWObject *fw = getParent()->getParent();
    assert(fw!=NULL);
    string branch_id = getOptionsObject()->getStr("branch_id");
    if (!branch_id.empty())
    {
        return RuleSet::cast(getRoot()->findInIndex(
                                 FWObjectDatabase::getIntId(branch_id)));
    } else
    {
        string branch_name = getOptionsObject()->getStr("branch_name");
        if (!branch_name.empty())
            return RuleSet::cast(fw->findObjectByName(NAT::TYPENAME,
                                                  branch_name));
        else
            return NULL;
    }
}

void NATRule::setBranch(RuleSet* ruleset)
{
    string branch_id = 
        (ruleset) ? FWObjectDatabase::getStringId(ruleset->getId()) : "";
    getOptionsObject()->setStr("branch_id", branch_id);
}

NATRule::NATRuleTypes NATRule::getRuleType() const
{ 
    return rule_type;
}

string  NATRule::getRuleTypeAsString() const 
{
    switch (rule_type) {
    case SNAT:     return("SNAT");     
    case DNAT:     return("DNAT");     
    case SDNAT:    return("SDNAT");     
    case Masq:     return("Masq");     
    case SNetnat:  return("SNetnat");  
    case DNetnat:  return("DNetnat");  
    case Redirect: return("Redirect"); 
    case Return:   return("Return");   
    case Skip:     return("Skip");     
    case Continue: return("Continue"); 
    case LB:       return("LB");       
    case NONAT:    return("NONAT");    
    default:       return("Unknown");  
    }
}

void         NATRule::setRuleType(NATRuleTypes rt) 
{ 
    rule_type=rt;
}

FWObject& NATRule::shallowDuplicate(const FWObject *x, bool preserve_id) throw(FWException)
{
    const NATRule *rx=NATRule::constcast(x);
    if (rx!=NULL) rule_type=rx->rule_type;

    osrc_re = NULL;
    odst_re = NULL;
    osrv_re = NULL;
    tsrc_re = NULL;
    tdst_re = NULL;
    tsrv_re = NULL;
    when_re = NULL;

    return  Rule::shallowDuplicate(x,preserve_id);
}



/***************************************************************************/

const char *RoutingRule::TYPENAME={"RoutingRule"};

RoutingRule::RoutingRule() : Rule()
{
    rule_type=Undefined;
    sorted_dst_ids="";
    setMetric(0);
}

RoutingRule::RoutingRule(const FWObject *root,bool prepopulate) : Rule(root,prepopulate)
{
    if (prepopulate)
    {
        FWObject         *re;
        FWObjectDatabase *db=(FWObjectDatabase*)(root);
        assert(db);

        re=db->create("RDst");  assert(re!=NULL); add(re);
        re=db->create("RGtw");  assert(re!=NULL); add(re);
        re=db->create("RItf");  assert(re!=NULL); add(re);
    
        setMetric(0);

        add( db->create(RoutingRuleOptions::TYPENAME) );
    }
}

RuleElementRDst*  RoutingRule::getRDst()  const
{
    return RuleElementRDst::cast(getFirstByType(RuleElementRDst::TYPENAME));
}

RuleElementRGtw*  RoutingRule::getRGtw()  const
{
    return RuleElementRGtw::cast(getFirstByType(RuleElementRGtw::TYPENAME));
}

RuleElementRItf*  RoutingRule::getRItf()  const
{
    return RuleElementRItf::cast(getFirstByType(RuleElementRItf::TYPENAME));
}


bool RoutingRule::isEmpty() const
{
    RuleElement *rdst=getRDst();
    RuleElement *rgtw=getRGtw();
    RuleElement *ritf=getRItf();

    return (rdst->isAny() && rgtw->isAny() && ritf->isAny());
}

int RoutingRule::getMetric() const {
    return getInt("metric");
}

string RoutingRule::getMetricAsString() const {
    
    stringstream s; 
    s << getMetric();
  
    return s.str();
}

void RoutingRule::setMetric(const int metric) {
    setInt("metric", metric);
}

void RoutingRule::setMetric(string metric) {
    
    int imetric = atoi(metric.c_str());
    setInt("metric", imetric);
}

void RoutingRule::fromXML(xmlNodePtr root) throw(FWException)
{
    const char* n;

    FWObject::fromXML(root);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("disabled")));
    if(n)
    {
        setStr("disabled",n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("metric")));
    if(n)
    {
        setStr("metric",n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("position")));
    if(n)
    {
        setStr("position",n);
        FREEXMLBUFF(n);
    }

    n= FROMXMLCAST(xmlGetProp(root,TOXMLCAST("group")));
    if(n)
    {
        setStr("group",n);
        FREEXMLBUFF(n);
    }

}

xmlNodePtr RoutingRule::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
//    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));

    FWObject *o;

    if ( (o=getFirstByType( RuleElementRDst::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementRGtw::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementRItf::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RoutingRuleOptions::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( Routing::TYPENAME ))!=NULL )
	o->toXML(me);

    return me;
}

FWOptions* RoutingRule::getOptionsObject()
{
    return FWOptions::cast( getFirstByType(RoutingRuleOptions::TYPENAME) );
}

RuleSet*   RoutingRule::getBranch()
{
    FWObject *fw = getParent()->getParent();
    assert(fw!=NULL);
    string branch_id = getOptionsObject()->getStr("branch_id");
    if (!branch_id.empty())
    {
        return RuleSet::cast(getRoot()->findInIndex(
                                 FWObjectDatabase::getIntId(branch_id)));
    } else
    {
        string branch_name = getOptionsObject()->getStr("branch_name");
        if (!branch_name.empty())
            return RuleSet::cast(fw->findObjectByName(Routing::TYPENAME,
                                                      branch_name));
        else
            return NULL;
    }
}

RoutingRule::RoutingRuleTypes RoutingRule::getRuleType() const
{ 
    return rule_type;
}

string  RoutingRule::getRuleTypeAsString() const 
{
    switch (rule_type) {
    case Undefined:   return("Undefined");
    case SinglePath:  return("Single Path");     
    case MultiPath:   return("Multi Path");        
    default:          return("Unknown");  
    }
}

void RoutingRule::setRuleType(RoutingRuleTypes rt) 
{ 
    rule_type=rt;
}

FWObject& RoutingRule::duplicate(const FWObject *x, bool preserve_id) throw(FWException)
{
    Rule::duplicate(x,preserve_id);
    const RoutingRule *rx=RoutingRule::constcast(x);
    if (rx!=NULL) {
        
        rule_type=rx->rule_type;
        sorted_dst_ids=rx->sorted_dst_ids;
    }
    return *this;
}

void RoutingRule::setSortedDstIds(const string& ids)
{
    sorted_dst_ids = ids;
}

string RoutingRule::getSortedDstIds() const
{
    return sorted_dst_ids;
}
