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




#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/XMLTools.h"

#include <iostream>

using namespace std;
using namespace libfwbuilder;
 
const char *RuleSet::TYPENAME={"Ruleset"};

RuleSet::RuleSet()
{
    setName("RuleSet");
    ipv4 = false;
    ipv6 = false;
    top = false;
}

void RuleSet::init(FWObjectDatabase *root)
{
    FWObject *opt = getFirstByType(RuleSetOptions::TYPENAME);
    if (opt == nullptr) add(root->createRuleSetOptions());
}

RuleSet::~RuleSet() {}

void RuleSet::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);

    const char *n;

    // Both ipv4_rule_set and ipv6_rule_set attributes can be set to
    // true, which means this is "dual" rule set When both are false,
    // this is ipv4-only rule set (for backwards compatibility and to
    // avoid having to increment DTD version number)

    n=XMLTools::FromXmlCast(xmlGetProp(root, XMLTools::ToXmlCast("ipv4_rule_set")));
    if (n!=nullptr)
    {
        ipv4 = (string(n)=="True" || string(n)=="true");
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root, XMLTools::ToXmlCast("ipv6_rule_set")));
    if (n!=nullptr)
    {
        ipv6 = (string(n)=="True" || string(n)=="true");
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("top_rule_set")));
    if (n!=nullptr)
    {
        top = (string(n)=="True" || string(n)=="true");
        XMLTools::FreeXmlBuff(n);
    }
}

xmlNodePtr RuleSet::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));
    xmlNewProp(me, XMLTools::ToXmlCast("ipv4_rule_set"),
               XMLTools::ToXmlCast(((ipv4) ? "True" : "False")));

    xmlNewProp(me, XMLTools::ToXmlCast("ipv6_rule_set"),
               XMLTools::ToXmlCast(((ipv6) ? "True" : "False")));

    xmlNewProp(me, XMLTools::ToXmlCast("top_rule_set"), 
               XMLTools::ToXmlCast(((top) ? "True" : "False")));

    // First all rules, skip options
    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j)
    {
        if (FWOptions::cast(*j) == nullptr) (*j)->toXML(me);
    }

    FWObject *o;
    if ( (o=getFirstByType( RuleSetOptions::TYPENAME ))!=nullptr )
	o->toXML(me);

    return me;
}

FWOptions* RuleSet::getOptionsObject()
{
    return FWOptions::cast(getFirstByType(RuleSetOptions::TYPENAME));
}

FWObject& RuleSet::shallowDuplicate(const FWObject *o, bool preserve_id)
{
    const RuleSet *other = RuleSet::constcast(o);

    FWObject::shallowDuplicate(o, preserve_id);

    ipv4 = other->ipv4;
    ipv6 = other->ipv6;
    top = other->top;

    return *this;
}

bool RuleSet::cmp(const FWObject *obj, bool recursive)
{
    const RuleSet *other = RuleSet::constcast(obj);
    if (other == nullptr) return false;
    if (ipv4 != other->ipv4 || ipv6 != other->ipv6 || top != other->top)
        return false;
    return FWObject::cmp(obj, recursive);
}

/**
 * Add new rule on top of the rule set. If hidden_rule arg is true,
 * force new rule position number ot be negative. Make sure it grows
 * backwards if there are other rules with negative position
 * numbers. If the topmost rule has position > 0 for whatever reason,
 * set hidden rule's position to -1 rather than 0.
 *
 * Hidden rules are those added by compilers automatically, such as
 * automatic rule to permit ssh to the firewall or rules to permit
 * vrrp and other protocols. These rules are constructed by compilers
 * rather than defined by the user in the GUI.
 */
Rule* RuleSet::insertRuleAtTop(bool hidden_rule) 
{
    Rule *r = createRule();
    r->setHidden(hidden_rule);
    if (hidden_rule) r->setPosition(-1);
    else r->setPosition(0);
    insert(begin(), r);
    _adopt(r);
    renumberRules();
    return(r);
}

Rule* RuleSet::insertRuleBefore(int rule_n) 
{
    Rule *old_rule = getRuleByNum(rule_n);
    Rule *r = createRule();
    if (old_rule==nullptr) add(r);
    else insert_before(old_rule, r);
    renumberRules();
    return(r);
}

Rule* RuleSet::appendRuleAtBottom(bool hidden_rule) 
{
    Rule *r = createRule();
    r->setHidden(hidden_rule);
    int last_rule_position;
    Rule *last_rule = Rule::cast(back());
    if (last_rule != nullptr)
    {
        last_rule_position = last_rule->getPosition() + 1000;
    } else
        last_rule_position = 1000;
    if (hidden_rule) r->setPosition(last_rule_position);
    add(r);            // FWObject::add adds to the end of the list
    renumberRules();
    return(r);
}

Rule* RuleSet::appendRuleAfter(int rule_n) 
{
    Rule *old_rule = getRuleByNum(rule_n);
    Rule *r = createRule();
    if (old_rule==nullptr) add(r);
    else insert_after(old_rule,r);
    renumberRules();
    return(r);
}

bool RuleSet::deleteRule(int rule_n) 
{
    return deleteRule( getRuleByNum(rule_n) );
}

bool RuleSet::deleteRule(Rule *r) 
{
    if (r!=nullptr)
    {
	remove(r, true); // and delete the rule if ref counter == 0
	renumberRules();
	return(true);
    }
    return(false);
}

bool RuleSet::moveRuleUp(int rule_n) 
{
    if (rule_n==0) return(false);

    FWObject* o = getRuleByNum( rule_n );
    FWObject* prev = getRuleByNum( rule_n-1 );

    swapObjects(prev,o);
    renumberRules();
    return(true);
}

bool RuleSet::moveRuleDown(int rule_n) 
{
    if (rule_n > (getRuleSetSize() - 1) ) return(false);

    FWObject* o = getRuleByNum( rule_n );
    FWObject* next = getRuleByNum( rule_n+1 );

    swapObjects(o,next);
    renumberRules();
    return(true);
}

bool RuleSet::disableRule(int rule_n)
{
    FWObject* o   =getRuleByNum( rule_n );

    if (o) {
  	(Rule::cast(o))->disable();
  	return true;
    }
    return false;
}

bool RuleSet::enableRule(int rule_n)
{
    FWObject* o   =getRuleByNum( rule_n );

    if (o) {
  	(Rule::cast(o))->enable();
  	return true;
    }
    return false;
}

bool  RuleSet::isRuleDisabled(int rule_n)
{
    FWObject* o   =getRuleByNum( rule_n );

    if (o)
  	return((Rule::cast(o))->isDisabled());
    return false;
}


/*
 * Rule number src_rule_n moves right above rule number dst_rule_n
 */
bool RuleSet::moveRule(int src_rule_n, int dst_rule_n) 
{
      FWObject* src =getRuleByNum( src_rule_n );
      FWObject* dst =getRuleByNum( dst_rule_n );

      if (src!=nullptr &&  dst!=nullptr && src!=dst ) {

  	FWObject *o;
  	list<FWObject*>::iterator m, m1, m2;
  	for (m=begin(); m!=end(); ++m) {
  	    if ( (o=(*m))!=nullptr ) {
  		if ( o==src ) m1=m;
  		if ( o==dst ) m2=m;
  	    }
  	}
  	if ( (*m1)!=nullptr && (*m2)!=nullptr ) {
  	    erase(m1);
  	    insert(m2,src);
  	}
  	renumberRules();
  	return(true);
      }
    return(false);
}

void RuleSet::renumberRules()
{
    if (size() == 0) return;

    list<FWObject*>::iterator m, it;
    int rn;

    // first pass: find first not hidden rule; it will become rule #0
    // Even though RuleSet object an only have Rule objects as
    // children, I am checking for the child type everywhere in case
    // we ever add another type of child.
    it = begin();
    for (; it!=end(); ++it)
    {
        Rule *rule = Rule::cast(*it);
        if (rule && !rule->isHidden()) break;
    }

    list<FWObject*>::reverse_iterator rev_it(it);
    list<FWObject*>::reverse_iterator rev_end(begin());
    // rev_it points to the last hidden rule
    rn = 0;
    for (; it != end(); it++)
    {
        Rule *rule = Rule::cast(*it);
        // there can be hidden rules at the bottom of policy
        // Just do not change their position numbers.
        if (rule && !rule->isHidden())
        {
            rule->setPosition(rn);
            rn++;
        }
    }
    rn = -1;
    for (; rev_it != rev_end; rev_it++)
    {
        Rule *rule = Rule::cast(*rev_it);
        if (rule && rule->isHidden())
        {
            rule->setPosition(rn);
            rn--;
        }
    }
}

Rule* RuleSet::getRuleByNum(int n)
{
    for(list<FWObject*>::iterator m=begin(); m!=end(); ++m) 
    {
        FWObject *o;
	if ( (o=(*m))!=nullptr ) 
        {
	    Rule *r = Rule::cast(o);
	    if (r && r->getPosition()==n) 
                return r;
	}
    }
    return nullptr;
}

int RuleSet::getRuleSetSize()
{
    return getChildrenCount() - 1;
}

void RuleSet::assignUniqueRuleIds()
{
    for (FWObject::iterator it=begin(); it!=end(); ++it)
    {
        Rule *r = Rule::cast(*it);
        if (r != nullptr && r->getUniqueId().empty())
            r->setUniqueId(FWObjectDatabase::getStringId((*it)->getId()) );

    }
}


