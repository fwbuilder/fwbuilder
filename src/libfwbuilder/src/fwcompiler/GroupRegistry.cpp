/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

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

#include "GroupRegistry.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"


using namespace libfwbuilder;
using namespace std;


GroupRegistry::GroupRegistry()
{}

/*
 * Generate stable key to be used as a key in rule_element_groups map.
 * This key should not change when rule processors create copies of
 * rules, this means we can't use rule and rule element ID.
 */
string GroupRegistry::getREKey(libfwbuilder::RuleElement *re)
{
    Rule *rule = Rule::cast(re->getParent());
    return rule->getLabel() + "_" + re->getTypeName();
}

void GroupRegistry::registerGroup(FWObject *grp, const list<FWObject*> &objects)
{
    for (list<FWObject*>::const_iterator it=objects.begin();
         it!=objects.end(); ++it)
    {
        FWObject *o = FWReference::getObject(*it);
        string str_id = FWObjectDatabase::getStringId(o->getId());
        group_registry[str_id].insert(grp->getName());
        setGroupRegistryKey(o, str_id);
    }
}

/*
 * register a group as a member of given rule element. 
 */
void GroupRegistry::registerGroupInRE(RuleElement *re, FWObject *grp)
{
    string key_str = getREKey(re);
    rule_element_groups[key_str].insert(grp->getName());
}

set<string> GroupRegistry::getGroupsForRE(RuleElement *re)
{
    string key_str = getREKey(re);
    return rule_element_groups[key_str];
}

set<string> GroupRegistry::getGroupsForObject(FWObject *obj)
{
    return group_registry[getGroupRegistryKey(obj)];
}

string GroupRegistry::getGroupRegistryKey(FWObject *obj)
{
    return obj->getStr(".group_registry_key");
}

void GroupRegistry::setGroupRegistryKey(FWObject *obj, const std::string &key)
{
    obj->setStr(".group_registry_key", key);
    for (FWObject::iterator i=obj->begin(); i!=obj->end(); i++)
    {
        setGroupRegistryKey(*i, key);
    }
}


