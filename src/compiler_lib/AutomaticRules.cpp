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

#include "AutomaticRules.h"

#include "fwbuilder/Address.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Policy.h"


using namespace fwcompiler;
using namespace libfwbuilder;
using namespace std;


AutomaticRules::AutomaticRules(Firewall *fw, Library *persistent_objects)
{
    this->fw = fw;
    this->persistent_objects = persistent_objects;
    ruleset = nullptr;

    list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);
    for (FWObject::iterator it=all_policies.begin(); it!=all_policies.end(); ++it)
    {
        Policy *policy = Policy::cast(*it);

        FWOptions *rulesetopts = policy->getOptionsObject();
        if (rulesetopts->getBool("mangle_only_rule_set")) continue;

        if (policy->isTop()) 
        {
            ruleset = policy;
            break;
        }
    }
}

PolicyRule* AutomaticRules::addMgmtRule(
    Address* src,
    Address* dst,
    Service* service,
    Interface* iface,
    const PolicyRule::Direction direction,
    const PolicyRule::Action action,
    const string &label,
    bool related)
{
    (void) related; // Unused

    if (ruleset == nullptr) return nullptr;

    /* Insert PolicyRules at top so they do not get shadowed by other
     * rules. Call insertRuleAtTop() with hidden_rule argument true to
     * make sure this rule gets negative position number and does not
     * shift positions of other rules. See ticket #16. Also, hidden
     * rules are not considered for shadowing.
     */

    PolicyRule* rule = PolicyRule::cast(ruleset->insertRuleAtTop(true));
    assert(rule != nullptr);

    ostringstream str;
    str << rule->getPosition() << " " << label << " (automatic)" ;
    rule->setLabel(str.str());

    FWObject *re;
    re = rule->getSrc();  assert(re!=nullptr);
    RuleElementSrc::cast(re)->reset();
    if(src != nullptr)
        re->addRef(src);

    re = rule->getDst();  assert(re!=nullptr);
    RuleElementDst::cast(re)->reset();
    if(dst != nullptr)
        re->addRef(dst);

    re = rule->getSrv();  assert(re!=nullptr);
    RuleElementSrv::cast(re)->reset();
    if(service != nullptr)
        re->addRef(service);

    re = rule->getWhen(); assert(re!=nullptr);
    RuleElementInterval::cast(re)->reset();

    re = rule->getItf(); assert(re!=nullptr);
    RuleElementItf::cast(re)->reset();
    if(iface != nullptr)
    {
        re->addRef(iface);
//        rule->setInterfaceId(iface->getId());
    }

    rule->add(ruleset->getRoot()->create(PolicyRuleOptions::TYPENAME));
    rule->setLogging(false);
    rule->enable();
    rule->setAction(action);
    rule->setDirection(direction);
    // Use firewall object ID to generate uique ID for this management rule
    // to make it stable across different runs of the compiler
    rule->setUniqueId(
        ruleset->getRoot()->getPredictableId(
            FWObjectDatabase::getStringId(fw->getId()) + "."
        ));

    return rule;
}
