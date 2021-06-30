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

Preprocessor::~Preprocessor()
{
    dbcopy = nullptr;
}

Preprocessor::Preprocessor(FWObjectDatabase *_db,
                           Firewall *_fw, bool ipv6_policy) :
    Compiler(nullptr, _fw, ipv6_policy)
{
    // This is the main difference between Preprocessor and other
    // compilers.  All compilers create a copy of the whole database
    // and work with it, but Preprocessor works with the original
    // database. Therefore it copies only pointer here.
    dbcopy = _db;

    fw_id = _fw->getId();
    fwopt = _fw->getOptionsObject();

    string fw_str_id = FWObjectDatabase::getStringId(_fw->getId());
    fw = Firewall::cast(
        dbcopy->findInIndex(FWObjectDatabase::getIntId(fw_str_id)));
}

void Preprocessor::convertObject(FWObject *obj)
{
    MultiAddress *adt = MultiAddress::cast(obj);
    if (adt!=nullptr && adt->isCompileTime())
    {
        adt->loadFromSource(ipv6, getCachedFwOpt(), inTestMode());
    }
}

int  Preprocessor::prolog()
{
    return 0;
}

void Preprocessor::findMultiAddressObjectsUsedInRules(FWObject *top)
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
                findMultiAddressObjectsUsedInRules(branch_ruleset);
        }

        FWReference *ref = FWReference::cast(obj);
        if (ref == nullptr)
            findMultiAddressObjectsUsedInRules(obj);
        else
        {
            FWObject *obj_ptr = FWReference::getObject(obj);
            if (obj_ptr->getInt(".loaded") == infinite_recursion_breaker)
                continue;
            obj_ptr->setInt(".loaded", infinite_recursion_breaker);

            try
            {
                convertObject(obj_ptr);
            } catch (FWException &ex) {
                abort(ex.toString());
            }

            // Note that MultiAddress inherits ObjectGroup
            if (Group::cast(obj_ptr))
                findMultiAddressObjectsUsedInRules(obj_ptr);
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
    FWObject *rule_copy = nullptr;

    if (single_rule_mode)
    {
        rule_copy = dbcopy->findInIndex(single_rule_compile_rule->getId());
        findMultiAddressObjectsUsedInRules(rule_copy);
    } else
    {
        FWObject *fwcopy = dbcopy->findInIndex(fw->getId());
        findMultiAddressObjectsUsedInRules(fwcopy);
    }
/* resolving MultiAddress objects */
//    convertObjectsRecursively(dbcopy);
}

void Preprocessor::epilog()
{}

