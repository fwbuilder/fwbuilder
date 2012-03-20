/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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

#include <assert.h>
#include <sstream>
#include <iostream>

#include "config.h"
#include "fwbuilder/libfwbuilder-config.h"


#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/TagService.h"

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

void Rule::init(FWObjectDatabase*)
{
}

FWOptions* Rule::getOptionsObject() const { return NULL; }
RuleSet* Rule::getBranch() { return NULL; }
void Rule::setPosition(int n)  { setInt("position", n); }
int Rule::getPosition() const { return getInt("position"); }
void Rule::disable() { setBool("disabled",true); }
void Rule::enable() { setBool("disabled",false); }
bool Rule::isDisabled() const  { return( getBool("disabled")); }
bool Rule::isEmpty() { return false; }

void Rule::setBranch(RuleSet*) {};

string Rule::getRuleGroupName() const { return getStr("group"); }

void Rule::setRuleGroupName(const std::string &group_name)
{
    setStr("group", group_name);
}



FWObject& Rule::shallowDuplicate(const FWObject *x,
                                 bool preserve_id) throw(FWException)
{
    const Rule *rx=Rule::constcast(x);
    fallback = rx->fallback;
    hidden = rx->hidden;
    label = rx->label;
    unique_id = rx->unique_id;
    abs_rule_number = rx->abs_rule_number;
    compiler_message = rx->compiler_message;
    return  FWObject::shallowDuplicate(x,preserve_id);
}

bool Rule::cmp(const FWObject *x, bool recursive) throw(FWException)
{
    const Rule *rx = Rule::constcast(x);
    if (fallback != rx->fallback ||
        hidden != rx->hidden ||
        label != rx->label ||
        unique_id != rx->unique_id) return false;

    return  FWObject::cmp(x, recursive);
}

/***************************************************************************/

const char *PolicyRule::TYPENAME={"PolicyRule"};

PolicyRule::PolicyRule()
{
//    setStr("action","Deny");
    setAction(PolicyRule::Deny);

    src_re = NULL;
    dst_re = NULL;
    srv_re = NULL;
    itf_re = NULL;
    when_re = NULL;
}

void PolicyRule::init(FWObjectDatabase *root)
{
    FWObject *re = getFirstByType(RuleElementSrc::TYPENAME);

    if (re == NULL)
    {
// <!ELEMENT PolicyRule (Src,Dst,Srv?,Itf?,When?,PolicyRuleOptions?)>
        re = root->createRuleElementSrc();  assert(re!=NULL);
        add(re); src_re = RuleElementSrc::cast(re);

        re = root->createRuleElementDst();  assert(re!=NULL);
        add(re); dst_re = RuleElementDst::cast(re);

        re = root->createRuleElementSrv();  assert(re!=NULL);
        add(re); srv_re = RuleElementSrv::cast(re);

        re = root->createRuleElementItf();  assert(re!=NULL);
        add(re); itf_re = RuleElementItf::cast(re);

        re = root->createRuleElementInterval(); assert(re!=NULL);
        add(re); when_re = RuleElementInterval::cast(re);

        add( root->createPolicyRuleOptions() );
    }
}

FWObject& PolicyRule::shallowDuplicate(const FWObject *x,
                                       bool preserve_id) throw(FWException)
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

    return  Rule::shallowDuplicate(x, preserve_id);
}

bool PolicyRule::cmp(const FWObject *x, bool recursive) throw(FWException)
{
    const PolicyRule *rx = PolicyRule::constcast(x);
    if (rx == NULL) return false;
    if (getDirection() != rx->getDirection() ||
        getAction() != rx->getAction() ||
        getLogging() != rx->getLogging()) return false;
    return  Rule::cmp(x, recursive);
}

// <!ELEMENT PolicyRule (Src,Dst,Srv?,Itf?,When?,PolicyRuleOptions?)>
RuleElementSrc*  PolicyRule::getSrc()
{
    if (src_re) return src_re;
    FWObject::iterator i1 = begin();
    src_re = RuleElementSrc::cast(*i1);
    return src_re;
}

RuleElementDst*  PolicyRule::getDst()
{
    if (dst_re) return dst_re;
    FWObject::iterator i1 = begin();
    i1++;
    dst_re = RuleElementDst::cast(*i1);
    return dst_re;
}

RuleElementSrv*  PolicyRule::getSrv()
{
    if (srv_re) return srv_re;
    FWObject::iterator i1 = begin();
    i1++;
    i1++;
    srv_re = RuleElementSrv::cast(*i1);
    return srv_re;
}

RuleElementItf*  PolicyRule::getItf()
{
    if (itf_re) return itf_re;
    FWObject::iterator i1 = begin();
    i1++;
    i1++;
    i1++;
    itf_re = RuleElementItf::cast(*i1);
    return itf_re;
}

RuleElementInterval* PolicyRule::getWhen()
{
    if (when_re) return when_re;
    FWObject::iterator i1 = begin();
    i1++;
    i1++;
    i1++;
    i1++;
    when_re = RuleElementInterval::cast(*i1);
    return when_re;
}

bool PolicyRule::isEmpty()
{
  return (getSrc()->isAny() && 
          getDst()->isAny() && 
          getSrv()->isAny() && 
          getItf()->isAny());
}

string PolicyRule::getActionAsString() const
{
    return getActionAsString(action);
}

// static method
string PolicyRule::getActionAsString(int act)
{
    switch (act) {
    case Accept:     return "Accept";
    case Deny:       return "Deny";
    case Reject:     return "Reject";
    case Scrub:      return "Scrub";
    case Return:     return "Return";
    case Skip:       return "Skip";
    case Continue:   return "Continue";
    case Accounting: return "Accounting";
    case Modify:     return "Modify";
    case Pipe:       return "Pipe";
    case Custom:     return "Custom";
    case Branch:     return "Branch";
    default:         return "Unknown";
    }
    return "Deny";
}

void PolicyRule::setAction(const string& act)
{
    if (act=="Accept")     { setAction(Accept); return; }
    if (act=="Deny")       { setAction(Deny); return; }
    if (act=="Reject")     { setAction(Reject); return; }
    if (act=="Scrub")      { setAction(Scrub); return; }
    if (act=="Return")     { setAction(Return); return; }
    if (act=="Skip")       { setAction(Skip); return; }
    if (act=="Continue")   { setAction(Continue); return; }
    if (act=="Accounting") { setAction(Accounting); return; }
    if (act=="Modify")     { setAction(Modify); return; }
    if (act=="Pipe")       { setAction(Pipe); return; }
    if (act=="Custom")     { setAction(Custom); return; }
    if (act=="Branch")     { setAction(Branch); return; }
    setAction(Deny);
}

string PolicyRule::getDirectionAsString() const
{
    switch (direction)
    {
    case Inbound:   return "Inbound";
    case Outbound:  return "Outbound";
    default:        return "Both";
    }
    return "Both";
}

void PolicyRule::setDirection(const string& dir)
{
    if (dir=="Inbound")   { setDirection(Inbound); return; }
    if (dir=="Outbound")  { setDirection(Outbound); return; }
    setDirection(Both);
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
        setAction(string(n));
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
        setDirection(string(n));
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
    xmlNewProp(me, TOXMLCAST("action"), STRTOXMLCAST(getActionAsString()));
    xmlNewProp(me, TOXMLCAST("direction"),STRTOXMLCAST(getDirectionAsString()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));

    FWObject *o;

    /*
     * Save children to XML file in just this order (src, dst, srv).
     * PolicyCompiler::checkForShadowing depends on it!
     * But after all, DTD requires this order.
     *
     <!ELEMENT PolicyRule (Src,Dst,Srv?,Itf?,When?,PolicyRuleOptions?)>
     */
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

    // there should be no children Policy objects in v3
    if ( (o=getFirstByType( Policy::TYPENAME ))!=NULL )
	o->toXML(me);

    return me;
}

FWOptions* PolicyRule::getOptionsObject() const
{
    return FWOptions::cast( getFirstByType(PolicyRuleOptions::TYPENAME) );
}

/*
 * FWObjectDatabase::setPredictableIds() calls this method after it
 * has updated string ID of all objects, including rule sets.
 */
void PolicyRule::updateNonStandardObjectReferences()
{
    if (getAction() == PolicyRule::Branch)
    {
        RuleSet *branch_ruleset = getBranch();
        setBranch(branch_ruleset);
    }

    if (getTagging())
    {
        FWObject *tag_object = getTagObject();
        setTagObject(tag_object);
    }
}

RuleSet* PolicyRule::getBranch()
{
    if (getAction() != PolicyRule::Branch) return NULL;
    FWObject *fw = this;
    while (fw && Firewall::cast(fw) == NULL) fw = fw->getParent();
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

bool PolicyRule::getRouting() const
{
    return getOptionsObject()->getBool("routing");
}

void PolicyRule::setRouting(bool f)
{
    getOptionsObject()->setBool("routing", f);
}

bool PolicyRule::getClassification() const
{
    return getOptionsObject()->getBool("classification");
}

void PolicyRule::setClassification(bool f)
{
    getOptionsObject()->setBool("classification", f);
}


bool PolicyRule::getTagging() const
{
    return getOptionsObject()->getBool("tagging");
}

void PolicyRule::setTagging(bool f)
{
    getOptionsObject()->setBool("tagging", f);
}

void PolicyRule::setTagObject(FWObject *tag_object)
{
    string tag_id =
        (tag_object) ? FWObjectDatabase::getStringId(tag_object->getId()) : "";
    getOptionsObject()->setStr("tagobject_id", tag_id);
    setTagging(tag_object && ! tag_id.empty());
}

FWObject* PolicyRule::getTagObject()
{
    if (getTagging())
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
    if (getTagging())
    {
        TagService *tagobj = TagService::cast(getTagObject());
        if (tagobj)  return tagobj->getCode();
        else         return getOptionsObject()->getStr("tagvalue");
    }
    return "";
}

/**
 * Add reference to given object. In case of PolicyRule this only
 * makes sense in terms of adding reference to this object as an
 * argument for actions Branch and Tag.
 */
void PolicyRule::addRef(FWObject *obj)
{
    if (RuleSet::cast(obj))
    {
        setBranch(RuleSet::cast(obj));
    }

    if (TagService::cast(obj))
    {
        setTagObject(TagService::cast(obj));
    }
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

void PolicyRule::replaceReferenceInternal(int old_id, int new_id, int &counter)
{
    if (old_id == new_id) return;

    FWObject::replaceReferenceInternal(old_id, new_id, counter);

    string branch_id = getOptionsObject()->getStr("branch_id");
    if (!branch_id.empty())
    {
        int branch_id_int = FWObjectDatabase::getIntId(branch_id);
        if (branch_id_int == old_id)
        {
            getOptionsObject()->setStr("branch_id",
                                       FWObjectDatabase::getStringId(new_id));
            counter++;
        }
    }
}

/***************************************************************************/

const char *NATRule::TYPENAME={"NATRule"};

NATRule::NATRule() : Rule()
{
    rule_type = Unknown;
    setAction(NATRule::Translate);

    osrc_re = NULL;
    odst_re = NULL;
    osrv_re = NULL;
    tsrc_re = NULL;
    tdst_re = NULL;
    tsrv_re = NULL;
    itf_inb_re = NULL;
    itf_outb_re = NULL;
    when_re = NULL;
}

void NATRule::init(FWObjectDatabase *root)
{
    FWObject *re = getFirstByType(RuleElementOSrc::TYPENAME);
    if (re == NULL)
    {
        re = root->createRuleElementOSrc();  assert(re!=NULL);
        add(re); osrc_re = RuleElementOSrc::cast(re);

        re = root->createRuleElementODst();  assert(re!=NULL);
        add(re); odst_re = RuleElementODst::cast(re);

        re = root->createRuleElementOSrv();  assert(re!=NULL);
        add(re); osrv_re = RuleElementOSrv::cast(re);
    
        re = root->createRuleElementTSrc();  assert(re!=NULL);
        add(re); tsrc_re = RuleElementTSrc::cast(re);

        re = root->createRuleElementTDst();  assert(re!=NULL);
        add(re); tdst_re = RuleElementTDst::cast(re);

        re = root->createRuleElementTSrv();  assert(re!=NULL);
        add(re); tsrv_re = RuleElementTSrv::cast(re);

        re = root->createRuleElementItfInb();  assert(re!=NULL);
        add(re); itf_inb_re = RuleElementItfInb::cast(re);

        re = root->createRuleElementItfOutb();  assert(re!=NULL);
        add(re); itf_outb_re = RuleElementItfOutb::cast(re);

        add( root->createNATRuleOptions() );
    }
}

/**
 * Add reference to given object. In case of NATRule this only
 * makes sense in terms of adding reference to this object as an
 * argument for action Branch.
 */
void NATRule::addRef(FWObject *obj)
{
    if (RuleSet::cast(obj))
    {
        setBranch(RuleSet::cast(obj));
    }
}

/**
 * Removes reference to given object among children of 'this'. In case
 * of NATRule we should also clear reference to it if action is
 * Branch. Caveat: clear reference to it even if action is not branch
 * right now but was in the past and reference got stuck in options.
 */
void NATRule::removeRef(FWObject *obj)
{
    if (RuleSet::cast(obj))
    {
        string branch_id = FWObjectDatabase::getStringId(obj->getId());
        string rule_branch_id = getOptionsObject()->getStr("branch_id");
        if (branch_id == rule_branch_id)
            getOptionsObject()->setStr("branch_id", "");
    }
    FWObject::removeRef(obj);
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

RuleElementItfInb* NATRule::getItfInb()
{
    if (itf_inb_re) return itf_inb_re;
    itf_inb_re = RuleElementItfInb::cast(getFirstByType(RuleElementItfInb::TYPENAME));
    return itf_inb_re;
}

RuleElementItfOutb* NATRule::getItfOutb()
{
    if (itf_outb_re) return itf_outb_re;
    itf_outb_re = RuleElementItfOutb::cast(getFirstByType(RuleElementItfOutb::TYPENAME));
    return itf_outb_re;
}


string NATRule::getActionAsString() const
{
    return getActionAsString(action);
}

// static method
string NATRule::getActionAsString(int act)
{
    if (act == Branch) return "NATBranch";
    return "Translate";
}

void NATRule::setAction(const string& act)
{
    if (act=="Translate")  { setAction(Translate); return; }
    if (act=="Branch")     { setAction(Branch); return; }
    if (act=="NATBranch")  { setAction(Branch); return; }
    setAction(Translate);
}

bool NATRule::isEmpty()
{
    RuleElement *osrc = getOSrc();
    RuleElement *odst = getODst();
    RuleElement *osrv = getOSrv();

    RuleElement *tsrc = getTSrc();
    RuleElement *tdst = getTDst();
    RuleElement *tsrv = getTSrv();

    RuleElement *itf_inb = getItfInb();
    RuleElement *itf_outb = getItfOutb();

    return (osrc->isAny() && odst->isAny() && osrv->isAny() &&
            tsrc->isAny() && tdst->isAny() && tsrv->isAny() &&
            itf_inb->isAny() && itf_outb->isAny());
}

void NATRule::fromXML(xmlNodePtr root) throw(FWException)
{
    const char* n;

    FWObject::fromXML(root);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("action")));
    if(n)
    {
        setAction(string(n));
        FREEXMLBUFF(n);
    }

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
    xmlNewProp(me, TOXMLCAST("action"), STRTOXMLCAST(getActionAsString()));
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

    if ( (o=getFirstByType( RuleElementItfInb::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementItfOutb::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( RuleElementInterval::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( NATRuleOptions::TYPENAME ))!=NULL )
	o->toXML(me);

    if ( (o=getFirstByType( NAT::TYPENAME ))!=NULL )
	o->toXML(me);

    return me;
}

FWOptions* NATRule::getOptionsObject() const
{
    return FWOptions::cast( getFirstByType(NATRuleOptions::TYPENAME) );
}

RuleSet* NATRule::getBranch()
{
    if (getAction() != NATRule::Branch) return NULL;
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

string NATRule::getRuleTypeAsString() const 
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
    case NATBranch: return("NATBranch");    
    default:       return("Unknown");  
    }
}

void NATRule::setRuleType(NATRuleTypes rt) 
{ 
    rule_type=rt;
}

FWObject& NATRule::shallowDuplicate(const FWObject *x,
                                    bool preserve_id) throw(FWException)
{
    const NATRule *rx = NATRule::constcast(x);
    if (rx!=NULL) rule_type = rx->rule_type;
    setAction(rx->getAction());

    osrc_re = NULL;
    odst_re = NULL;
    osrv_re = NULL;
    tsrc_re = NULL;
    tdst_re = NULL;
    tsrv_re = NULL;
    itf_inb_re = NULL;
    itf_outb_re = NULL;
    when_re = NULL;

    return  Rule::shallowDuplicate(x, preserve_id);
}

bool NATRule::cmp(const FWObject *x, bool recursive) throw(FWException)
{
    const NATRule *rx = NATRule::constcast(x);
    if (rx == NULL) return false;
    if (getAction() != rx->getAction()) return false;
    return  Rule::cmp(x, recursive);
}

/***************************************************************************/

const char *RoutingRule::TYPENAME={"RoutingRule"};

RoutingRule::RoutingRule() : Rule()
{
    rule_type=Undefined;
    sorted_dst_ids="";
    setMetric(0);
}

void RoutingRule::init(FWObjectDatabase *root)
{
    FWObject *re = getFirstByType(RuleElementRDst::TYPENAME);
    if (re == NULL)
    {
        re = root->createRuleElementRDst();  assert(re!=NULL); add(re);
        re = root->createRuleElementRGtw();  assert(re!=NULL); add(re);
        re = root->createRuleElementRItf();  assert(re!=NULL); add(re);
        add( root->createRoutingRuleOptions() );
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

FWOptions* RoutingRule::getOptionsObject() const
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

FWObject& RoutingRule::duplicate(const FWObject *x,
                                 bool preserve_id) throw(FWException)
{
    Rule::duplicate(x,preserve_id);
    const RoutingRule *rx = RoutingRule::constcast(x);
    if (rx!=NULL)
    {
        rule_type = rx->rule_type;
        sorted_dst_ids = rx->sorted_dst_ids;
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
