/* 

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

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


#include "Preprocessor.h"

#include "fwbuilder/MultiAddress.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/RuleSet.h"

#include <iostream>

#include <stdlib.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

static int infinite_recursion_breaker = 0;
string Preprocessor::myPlatformName() { return "generic_preprocessor"; }

Preprocessor::~Preprocessor() {}

Preprocessor::Preprocessor(FWObjectDatabase *_db,
                           Firewall *fw, bool ipv6_policy) :
    Compiler(_db, fw, ipv6_policy)
{
    // This is the main difference between Preprocessor and other
    // compilers.  All compilers create a copy of the whole database
    // and work with it, but Preprocessor works with the original
    // database. Therefore it copies only pointer here.
    dbcopy = _db;
}

void Preprocessor::convertObject(FWObject *obj)
{
    MultiAddress *adt = MultiAddress::cast(obj);
    if (adt!=NULL && adt->isCompileTime())
    {
        adt->loadFromSource(ipv6, inTestMode());
    }
}

int  Preprocessor::prolog()
{
    return 0;
}

void Preprocessor::findMultiAddressObjectsUsedInRules(FWObject *top,
                                                      set<FWObject*> *resset)
{
    if (top->getInt(".recursion_breaker") == infinite_recursion_breaker)
        return;
    top->setInt(".recursion_breaker", infinite_recursion_breaker);

    for (FWObject::iterator i=top->begin(); i!=top->end(); ++i)
    {
        FWObject *obj = *i;
        PolicyRule *rule = PolicyRule::cast(obj);
        if (rule && rule->getAction() == PolicyRule::Branch)
        {
            RuleSet *branch_ruleset = rule->getBranch();
            if (branch_ruleset)
                findMultiAddressObjectsUsedInRules(branch_ruleset, resset);
        }

        FWReference *ref = FWReference::cast(obj);
        if (ref == NULL)
            findMultiAddressObjectsUsedInRules(obj, resset);
        else
        {
            FWObject *obj_ptr = FWReference::getObject(obj);
            if (MultiAddress::cast(obj_ptr))
                resset->insert(obj_ptr);
            else
            {
                // Note that MultiAddress inherits ObjectGroup
                if (Group::cast(obj_ptr))
                    findMultiAddressObjectsUsedInRules(obj_ptr, resset);
            }
        }
    }
}

void Preprocessor::compile()
{
    // find all MultiAddress objects used in rules of this firewall,
    // directly or as group members. A bit of optimisation:
    // MultiAddress objects (DNSName and AddressTable) can not be used
    // in netowrk zone of interfaces and rule actions.

    // Note: fw belongs to the original object tree rather than dbcopy
    infinite_recursion_breaker++;
    set<FWObject*> resset;
    FWObject *rule_copy = NULL;
    if (single_rule_mode)
    {
        rule_copy = dbcopy->findInIndex(single_rule_compile_rule->getId());
        findMultiAddressObjectsUsedInRules(rule_copy, &resset);
    } else
    {
        FWObject *fwcopy = dbcopy->findInIndex(fw->getId());
        findMultiAddressObjectsUsedInRules(fwcopy, &resset);
    }
    for (set<FWObject*>::iterator it=resset.begin(); it!=resset.end(); ++it)
    {
        try
        {
            convertObject(*it);
        } catch (FWException &ex)
        {
            abort(ex.toString());
        }
    }
/* resolving MultiAddress objects */
//    convertObjectsRecursively(dbcopy);
}

void Preprocessor::epilog()
{}

