/* 

                          Firewall Builder

                 Copyright (C) 2002-2011 NetCitadel, LLC

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

#include "splitByNetworkZonesForRE.h"
#include "Helper.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Interface.h"

#include "fwcompiler/Compiler.h"


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


/*
 * create new rule and associate it with given interface. If we
 * already have a rule associated with it, then just add Address to
 * the rule element of that existing rule.
 */
void splitByNetworkZonesForRE::AddToInterface(
    int interface_id, Address *addr, Rule *rule)
{
    Rule  *new_rule;
    RuleElement *new_re;

    new_rule = rules[interface_id];
    if (new_rule==nullptr) 
    {
        new_rule = Rule::cast(compiler->dbcopy->create(rule->getTypeName()));
        compiler->temp_ruleset->add(new_rule);
        new_rule->duplicate(rule);
        rules[interface_id] = new_rule;
        new_re = RuleElement::cast(new_rule->getFirstByType(re_type));
        new_re->clearChildren();
        new_re->setAnyElement();
    }
    new_re = RuleElement::cast(new_rule->getFirstByType(re_type));
    new_re->addRef( addr );
}

bool splitByNetworkZonesForRE::processNext()
{
    Helper helper(compiler);
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));

    if (re->size()==1) 
    {
        tmp_queue.push_back(rule);
        return true;
    }
    
    rules.clear();

    std::list<FWObject*> cl;
    for (list<FWObject*>::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        Address *a = Address::cast(FWReference::getObject(*i1));
        assert(a!=nullptr);

        try
        {
            int interface_id = helper.findInterfaceByNetzone(a);
            AddToInterface(interface_id, a, rule);
        } catch (string err)
        {
            // could not find interface with netzone to match address 'a'
            // will assign rule to all interfaces. Act as if all interfaces
            // had network zone 'any' and each matches this address.

            // issue warning only if platform uses netwrk zones.

            bool supports_network_zones =
                Resources::getTargetCapabilityBool(
                    compiler->fw->getStr("platform"), "network_zones");

            if (supports_network_zones)
                compiler->warning(rule, err);

            FWObjectTypedChildIterator i =
                compiler->fw->findByType(Interface::TYPENAME);
            for ( ; i!=i.end(); ++i)
            {
                Interface  *ifs = Interface::cast(*i);
                AddToInterface(ifs->getId(), a, rule);
            }
        }
    }
    for (std::map<int,Rule*>::iterator i=rules.begin();
         i!=rules.end(); ++i)
    {
        tmp_queue.push_back((*i).second);        
    }
    return true;
}

