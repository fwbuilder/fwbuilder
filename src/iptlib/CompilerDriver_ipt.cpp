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

#include "../../config.h"
#include "../../build_num"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include <assert.h>
#include <cstring>
#include <iomanip>
#include <memory>

#include "Configlet.h"
#include "CompilerDriver_ipt.h"
#include "PolicyCompiler_ipt.h"
#include "PolicyCompiler_secuwall.h"
#include "MangleTableCompiler_ipt.h"
#include "OSConfigurator_linux24.h"
#include "OSConfigurator_ipcop.h"
#include "OSConfigurator_secuwall.h"

#include "fwcompiler/Preprocessor.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/ClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Policy.h"

#include <QStringList>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


CompilerDriver_ipt::CompilerDriver_ipt(FWObjectDatabase *db) :
    CompilerDriver(db)
{
}

// create a copy of itself, including objdb
CompilerDriver* CompilerDriver_ipt::clone()
{
    return new CompilerDriver_ipt(objdb);
}

void CompilerDriver_ipt::assignRuleSetChain(RuleSet *ruleset)
{
    string branch_name = ruleset->getName();
    for (FWObject::iterator r=ruleset->begin(); r!=ruleset->end(); r++)
    {
        Rule *rule = Rule::cast(*r);
        if (rule->isDisabled()) continue;

        //rule->setStr("parent_rule_num", parentRuleNum);

        if (!ruleset->isTop())
            rule->setStr("ipt_chain", branch_name);
        rule->setUniqueId( FWObjectDatabase::getStringId(rule->getId()) );
    }

}

void CompilerDriver_ipt::findBranchesInMangleTable(Firewall*,
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
                FWOptions *ruleopt = rule->getOptionsObject();
                if (rule->getAction() == PolicyRule::Branch &&
                    ruleopt->getBool("ipt_branch_in_mangle"))
                {
                    RuleSet *ruleset = rule->getBranch();
                    for (list<FWObject*>::iterator br=ruleset->begin();
                         br!=ruleset->end(); ++br)
                    {
                        Rule *b_rule = Rule::cast(*br);
                        ruleopt = b_rule->getOptionsObject();
                        ruleopt->setBool("put_in_mangle_table", true);
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
                                      const string& reset_script,
                                      const string& nat_script,
                                      const string& mangle_script,
                                      const string& filter_script,
                                      bool ipv6_policy)
{
    ostringstream res;
    ostringstream script;
    string prolog_place = fw->getOptionsObject()->getStr("prolog_place");

    Configlet *conf = NULL;
    bool have_reset = !reset_script.empty();

    string host_os = fw->getStr("host_OS");
    string os_family = Resources::os_res[host_os]->
        getResourceStr("/FWBuilderResources/Target/family");

    if (single_rule_compile_on)
    {
        have_reset = false;
        conf = new Configlet(os_family, "linux24", "script_body_single_rule");
        conf->collapseEmptyStrings(true);
    } else
    {
        if (fw->getOptionsObject()->getBool("use_iptables_restore"))
        {
            conf = new Configlet(os_family, "linux24", "script_body_iptables_restore");
        } else
            conf = new Configlet(os_family, "linux24", "script_body_single_rule");
    }

    conf->setVariable("reset", have_reset);
    conf->setVariable("reset_script", reset_script.c_str());

    conf->setVariable("filter", !filter_script.empty());
    conf->setVariable("filter_script", filter_script.c_str());

    conf->setVariable("mangle", !mangle_script.empty());
    conf->setVariable("mangle_script", mangle_script.c_str());
        
    conf->setVariable("nat", !nat_script.empty());
    conf->setVariable("nat_script", nat_script.c_str());

    bool have_script = (have_reset ||
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

bool CompilerDriver_ipt::processPolicyRuleSet(
    Firewall *fw,
    FWObject *ruleset,
    const string &single_rule_id,
    ostringstream &filter_table_stream,
    ostringstream &mangle_table_stream,
    ostringstream &automatic_rules_stream,
    OSConfigurator_linux24 *oscnf,
    int policy_af,
    std::map<const std::string, bool> &minus_n_commands_filter,
    std::map<const std::string, bool> &minus_n_commands_mangle)
{
    int policy_rules_count  = 0;
    int mangle_rules_count  = 0;
    bool have_connmark = false;
    bool have_connmark_in_output = false;
    bool empty_output = true;
    string prolog_place = fw->getOptionsObject()->getStr("prolog_place");
    string platform = fw->getStr("platform");
    string host_os = fw->getStr("host_OS");
    bool flush_and_set_default_policy = Resources::getTargetOptionBool(
        host_os, "default/flush_and_set_default_policy");
    string platform_family = Resources::platform_res[platform]->
        getResourceStr("/FWBuilderResources/Target/family");
    string os_family = Resources::os_res[host_os]->
        getResourceStr("/FWBuilderResources/Target/family");


    Policy *policy = Policy::cast(ruleset);
    assignRuleSetChain(policy);
    string branch_name = policy->getName();

    if (!policy->matchingAddressFamily(policy_af)) return true;

    bool ipv6_policy = (policy_af == AF_INET6);

    std::auto_ptr<MangleTableCompiler_ipt> mangle_compiler(
        new MangleTableCompiler_ipt(objdb , fw,
                                    ipv6_policy , oscnf,
                                    &minus_n_commands_mangle ));

    if (!policy->isTop())
        mangle_compiler->registerRuleSetChain(branch_name);

    mangle_compiler->setSourceRuleSet( policy );
    mangle_compiler->setRuleSetName(branch_name);

    mangle_compiler->setSingleRuleCompileMode(single_rule_id);
    mangle_compiler->setDebugLevel( dl );
    if (rule_debug_on) mangle_compiler->setDebugRule(  drp );
    mangle_compiler->setVerbose( (bool)(verbose) );
    mangle_compiler->setHaveDynamicInterfaces(have_dynamic_interfaces);
    if (inTestMode()) mangle_compiler->setTestMode();
    if (inEmbeddedMode()) mangle_compiler->setEmbeddedMode();

    if ( (mangle_rules_count = mangle_compiler->prolog()) > 0 )
    {
        mangle_compiler->compile();
        mangle_compiler->epilog();

        // We need to generate automatic rules in mangle
        // table (-j CONNMARK --restore-mark) if CONNMARK
        // target is present in any ruleset, not only in
        // the top-level ruleset. So we keep global
        // boolean flags for this condition which will
        // become true if any ruleset has such
        // rules. We'll call
        // MangleTableCompiler_ipt::flushAndSetDefaultPolicy
        // later if either of these flags is true after
        // all rulesets have been processed.

        have_connmark |= mangle_compiler->haveConnMarkRules();
        have_connmark_in_output |= mangle_compiler->haveConnMarkRulesInOutput();

        long m_str_pos = mangle_table_stream.tellp();

        if (policy->isTop())
        {
            ostringstream tmp;

            if (flush_and_set_default_policy)
                tmp << mangle_compiler->flushAndSetDefaultPolicy();

            tmp << mangle_compiler->printAutomaticRules();

            if (tmp.tellp() > 0)
            {
                if (!single_rule_compile_on)
                {
                    mangle_table_stream << "# ================ Table 'mangle', ";
                    mangle_table_stream << "automatic rules";
                    mangle_table_stream << "\n";
                }
                mangle_table_stream << tmp.str();
            }
        }

        if (mangle_compiler->getCompiledScriptLength() > 0)
        {
            ostringstream tmp;
            if (mangle_compiler->haveErrorsAndWarnings())
            {
                all_errors.push_back(mangle_compiler->getErrors("").c_str());
//                tmp << "# Policy compiler errors and warnings:" << "\n";
//                tmp << mangle_compiler->getErrors("# ");
            }

            tmp << mangle_compiler->getCompiledScript();

            if (tmp.tellp() > 0)
            {
                if (!single_rule_compile_on)
                {
                    mangle_table_stream << "# ================ Table 'mangle', ";
                    mangle_table_stream << "rule set " << branch_name << "\n";
                }
                mangle_table_stream << tmp.str();
            }
        }

        if (m_str_pos!=mangle_table_stream.tellp())
        {
            mangle_table_stream << "\n";
            empty_output = false;
        }
    }

    std::auto_ptr<PolicyCompiler_ipt> policy_compiler;

    if (fw->getStr("host_OS") == "secuwall") {
        policy_compiler = std::auto_ptr<PolicyCompiler_ipt>(
            new PolicyCompiler_secuwall(objdb,fw, ipv6_policy, oscnf,
                                        &minus_n_commands_filter));
    } else {
        policy_compiler = std::auto_ptr<PolicyCompiler_ipt>(
            new PolicyCompiler_ipt(objdb,fw, ipv6_policy, oscnf,
                                   &minus_n_commands_filter));
    }

    if (policy_compiler.get()==NULL)
        throw FWException("Unrecognized firewall platform " +
                          fw->getStr("platform") +
                          "  (family " + platform_family+")");

    if (!policy->isTop())
        policy_compiler->registerRuleSetChain(branch_name);

    policy_compiler->setSourceRuleSet( policy );
    policy_compiler->setRuleSetName(branch_name);

    policy_compiler->setSingleRuleCompileMode(single_rule_id);
    policy_compiler->setDebugLevel( dl );
    if (rule_debug_on) policy_compiler->setDebugRule(  drp );
    policy_compiler->setVerbose( (bool)(verbose) );
    policy_compiler->setHaveDynamicInterfaces(have_dynamic_interfaces);
    if (inTestMode()) policy_compiler->setTestMode();
    if (inEmbeddedMode()) policy_compiler->setEmbeddedMode();
    
    if ( (policy_rules_count=policy_compiler->prolog()) > 0 )
    {
        policy_compiler->compile();
        policy_compiler->epilog();

        if (policy_compiler->getCompiledScriptLength() > 0)
        {
            ostringstream tmp;

            if (policy_compiler->haveErrorsAndWarnings())
            {
                all_errors.push_back(policy_compiler->getErrors("").c_str());
//                tmp << "# Policy compiler errors and warnings:" << "\n";
//                tmp << policy_compiler->getErrors("# ");
            }
            tmp << policy_compiler->getCompiledScript();

            if (tmp.tellp() > 0)
            {
                empty_output = false;
                if (!single_rule_compile_on)
                {
                    filter_table_stream << "# ================ Table 'filter', ";
                    filter_table_stream << "rule set " << branch_name << "\n";
                }
                filter_table_stream << tmp.str();
            }
        }
    }

    /* bug #2550074: "Automatic rules for filter table included twice
     * in iptables". If user had two policy ruleset objects marked as
     * "top" rule set, then automaitc rules were added twice. Since we
     * add rules to automatic_rules_stream only in this one place, it
     * is sufficient to check if the stream is empty to avoid
     * duplication.  Note that on windows tellp() seems to return -1
     * if no data has ever been written to the stream.
     */
    long auto_rules_stream_position = automatic_rules_stream.tellp();

    if (policy->isTop() && auto_rules_stream_position <= 0)
    {
        ostringstream tmp;

        if (flush_and_set_default_policy)
            tmp << policy_compiler->flushAndSetDefaultPolicy();

        if (!prolog_done && prolog_place == "after_flush" &&
            !fw->getOptionsObject()->getBool("use_iptables_restore"))
        {
            tmp << "prolog_commands" << endl;
            prolog_done = true;
        }

        tmp << policy_compiler->printAutomaticRules();

        if (tmp.tellp() > 0)
        {
            empty_output = false;
            if (!single_rule_compile_on)
            {
                automatic_rules_stream
                    << "# ================ Table 'filter', automatic rules"
                    << "\n";
            }
            automatic_rules_stream << tmp.str();
        }
    }
    return empty_output;
}

