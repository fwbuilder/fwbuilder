/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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


#include "Configlet.h"
#include "CompilerDriver_ipt.h"
#include "PolicyCompiler_ipt.h"
#include "PolicyCompiler_secuwall.h"

#include "fwbuilder/Address.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/IPService.h"

#include <fstream>
#include <iostream>

#include <assert.h>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


CompilerDriver_ipt::CompilerDriver_ipt(FWObjectDatabase *db) :
    CompilerDriver(db)
{
    have_connmark = false;
    have_connmark_in_output = false;
}

CompilerDriver_ipt::~CompilerDriver_ipt()
{
}

// create a copy of itself, including objdb
CompilerDriver* CompilerDriver_ipt::clone()
{
    CompilerDriver_ipt* new_cd = new CompilerDriver_ipt(objdb);
    if (inEmbeddedMode()) new_cd->setEmbeddedMode();
    return new_cd;
}

void CompilerDriver_ipt::assignRuleSetChain(RuleSet *ruleset)
{
    string branch_name = ruleset->getName();
    for (FWObject::iterator r=ruleset->begin(); r!=ruleset->end(); r++)
    {
        Rule *rule = Rule::cast(*r);
        if (rule == nullptr) continue; // skip RuleSetOptions object
        if (rule->isDisabled()) continue;

        if (!ruleset->isTop())
            rule->setStr("ipt_chain", branch_name);
// ???
//        rule->setUniqueId( FWObjectDatabase::getStringId(rule->getId()) );
    }
}

void CompilerDriver_ipt::findBranchesInMangleTable(Firewall *fw,
                                                   list<FWObject*> &all_policies)
{
    // special but common case: if we only have one policy, there is
    // no need to check if we have to do branching in mangle table
    // since we do not have any branching rules in that case.
    if (all_policies.size() > 1)
    {
        for (list<FWObject*>::iterator i=all_policies.begin();
             i!=all_policies.end(); ++i)
        {
            for (list<FWObject*>::iterator r=(*i)->begin();
                 r!=(*i)->end(); ++r)
            {
                PolicyRule *rule = PolicyRule::cast(*r);
                if (rule == nullptr) continue; // skip RuleSetOptions object
                FWOptions *ruleopt = rule->getOptionsObject();
                if (rule->getAction() == PolicyRule::Branch &&
                    ! ruleopt->getBool("ipt_branch_in_mangle"))
                {
                    RuleSet *ruleset = rule->getBranch();
                    if (ruleset == nullptr)
                    {
                        abort(fw, *i, rule,
                              "Action branch does not point to any rule set");
                    }

                    for (list<FWObject*>::iterator br=ruleset->begin();
                         br!=ruleset->end(); ++br)
                    {
                        PolicyRule *b_rule = PolicyRule::cast(*br);
                        if (b_rule == nullptr) continue;
                        if (b_rule->getTagging() || b_rule->getClassification())
                            ruleopt->setBool("ipt_branch_in_mangle", true);
                    }
                }
            }
        }
    }
}

/*
 * TODO: use configlet to define structure of generated script. Need 2
 * configlets: for the shell script format and iptables-restore format
 * However in order to use configlets in an efficient manner, we need
 * to be able to use if-then-else statements there. This will help
 * implement logic that skips filter or mangle or nat segments if
 * there are no rules in them.
 *
 * Also will need either special configlets for the single-rule
 * compile or more if-then-else in configlet code.
 */
string CompilerDriver_ipt::dumpScript(Firewall *fw,
                                      const string& automatic_rules_script,
                                      const string& automatic_mangle_script,
                                      const string& nat_script,
                                      const string& mangle_script,
                                      const string& filter_script,
                                      bool ipv6_policy)
{

    // cerr << "nat script" << endl;
    // cerr << "\"" << nat_script << "\"" << endl;

    ostringstream res;
    ostringstream script;
    string prolog_place = fw->getOptionsObject()->getStr("prolog_place");

    Configlet *conf = nullptr;
    bool have_auto = !automatic_rules_script.empty() || !automatic_mangle_script.empty();

    if (single_rule_compile_on)
    {
        have_auto = false;
        conf = new Configlet(fw, "linux24", "script_body_single_rule");
        conf->collapseEmptyStrings(true);
    } else
    {
        if (fw->getOptionsObject()->getBool("use_iptables_restore"))
        {
            conf = new Configlet(fw, "linux24", "script_body_iptables_restore");
        } else
            conf = new Configlet(fw, "linux24", "script_body_iptables_shell");
    }

    conf->setVariable("auto", have_auto);

    conf->setVariable("iptables_restore_format",
                      fw->getOptionsObject()->getBool("use_iptables_restore"));

    conf->setVariable("filter", !filter_script.empty());
    conf->setVariable("filter_or_auto", have_auto || !filter_script.empty());
    conf->setVariable("filter_auto_script", automatic_rules_script.c_str());
    conf->setVariable("filter_script", filter_script.c_str());

    conf->setVariable("mangle", !mangle_script.empty());
    conf->setVariable("mangle_or_auto", !mangle_script.empty() || !automatic_mangle_script.empty());
    conf->setVariable("mangle_auto_script", automatic_mangle_script.c_str());
    conf->setVariable("mangle_script", mangle_script.c_str());

    conf->setVariable("nat", !nat_script.empty());
    conf->setVariable("nat_script", nat_script.c_str());

    bool have_script = (have_auto ||
                        !filter_script.empty() ||
                        !mangle_script.empty() ||
                        !nat_script.empty());

    conf->setVariable("have_script", have_script);
    conf->setVariable("ipv4", !ipv6_policy);
    conf->setVariable("ipv6",  ipv6_policy);

    res << conf->expand().toStdString();
    delete conf;

    return res.str();
}

std::unique_ptr<PolicyCompiler_ipt> CompilerDriver_ipt::createPolicyCompiler(
    Firewall *fw,
    bool ipv6_policy,
    OSConfigurator *oscnf,
    std::map<const std::string, bool> *minus_n_commands_filter)
{
    string platform = fw->getStr("platform");
    string platform_family = Resources::platform_res[platform]->
        getResourceStr("/FWBuilderResources/Target/family");

    std::unique_ptr<PolicyCompiler_ipt> policy_compiler;

    if (fw->getStr("host_OS") == "secuwall") {
        policy_compiler = std::unique_ptr<PolicyCompiler_ipt>(
            new PolicyCompiler_secuwall(objdb,fw, ipv6_policy, oscnf,
                                        minus_n_commands_filter));
    } else {
        policy_compiler = std::unique_ptr<PolicyCompiler_ipt>(
            new PolicyCompiler_ipt(objdb,fw, ipv6_policy, oscnf,
                                   minus_n_commands_filter));
    }

    if (policy_compiler.get()==nullptr)
        abort("Unrecognized firewall platform " +
              fw->getStr("platform") +
              "  (family " + platform_family+")");

    return policy_compiler;
}

