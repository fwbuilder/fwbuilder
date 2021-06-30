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


#include "CompilerDriver_ipt.h"
#include "MangleTableCompiler_ipt.h"
#include "PolicyCompiler_ipt.h"
#include "PolicyCompiler_secuwall.h"
#include "OSConfigurator_linux24.h"

#include "Configlet.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Library.h"

#include <fstream>
#include <iostream>
#include <memory>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

// we always first process all non-top rule sets, then all top rule
// sets
bool CompilerDriver_ipt::processPolicyRuleSet(
    Firewall *fw,
    FWObject *ruleset,
    const string &single_rule_id,
    ostringstream &filter_rules_stream,
    ostringstream &mangle_rules_stream,
    ostringstream &automatic_rules_stream,
    ostringstream &automatic_mangle_stream,
    OSConfigurator_linux24 *oscnf,
    int policy_af,
    std::map<const std::string, bool> &minus_n_commands_filter,
    std::map<const std::string, bool> &minus_n_commands_mangle)
{
    int policy_rules_count  = 0;
    int mangle_rules_count  = 0;
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

    std::unique_ptr<MangleTableCompiler_ipt> mangle_compiler(
        new MangleTableCompiler_ipt(objdb , fw,
                                    ipv6_policy , oscnf,
                                    &minus_n_commands_mangle ));

    if (!policy->isTop())
        mangle_compiler->registerRuleSetChain(branch_name);

    mangle_compiler->setSourceRuleSet( policy );
    mangle_compiler->setRuleSetName(branch_name);
    mangle_compiler->setPersistentObjects(persistent_objects);

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

        long m_str_pos = mangle_rules_stream.tellp();

        if (mangle_compiler->getCompiledScriptLength() > 0)
        {
            ostringstream tmp;

            tmp << mangle_compiler->getCompiledScript();

            if (tmp.tellp() > 0)
            {
                if (!single_rule_compile_on)
                {
                    mangle_rules_stream << "# ================ Table 'mangle', ";
                    mangle_rules_stream << "rule set " << branch_name << "\n";
                }
                mangle_rules_stream << tmp.str();
            }
        }

        if (mangle_compiler->haveErrorsAndWarnings())
        {
            all_errors.push_back(mangle_compiler->getErrors("").c_str());
            mangle_compiler->clearErrors();
        }

        if (m_str_pos!=mangle_rules_stream.tellp())
        {
            //mangle_rules_stream << "\n";
            empty_output = false;
        }

    }

    std::unique_ptr<PolicyCompiler_ipt> policy_compiler = createPolicyCompiler(
        fw, ipv6_policy, oscnf,  &minus_n_commands_filter);

    policy_compiler->setSingleRuleCompileMode(single_rule_id);
    policy_compiler->setDebugLevel( dl );
    if (rule_debug_on) policy_compiler->setDebugRule(  drp );
    policy_compiler->setVerbose( (bool)(verbose) );
    policy_compiler->setHaveDynamicInterfaces(have_dynamic_interfaces);
    if (inTestMode()) policy_compiler->setTestMode();
    if (inEmbeddedMode()) policy_compiler->setEmbeddedMode();
    if (!policy->isTop()) policy_compiler->registerRuleSetChain(branch_name);

    policy_compiler->setSourceRuleSet( policy );
    policy_compiler->setRuleSetName(branch_name);
    policy_compiler->setPersistentObjects(persistent_objects);

    if ( (policy_rules_count=policy_compiler->prolog()) > 0 )
    {
        policy_compiler->compile();
        policy_compiler->epilog();

        if (policy_compiler->getCompiledScriptLength() > 0)
        {
            ostringstream tmp;

            tmp << policy_compiler->getCompiledScript();

            if (tmp.tellp() > 0)
            {
                empty_output = false;
                if (!single_rule_compile_on)
                {
                    filter_rules_stream << "# ================ Table 'filter', ";
                    filter_rules_stream << "rule set " << branch_name << "\n";
                }
                filter_rules_stream << tmp.str();
            }
        }

        if (policy_compiler->haveErrorsAndWarnings())
        {
            all_errors.push_back(policy_compiler->getErrors("").c_str());
            policy_compiler->clearErrors();
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

        /*
         * commented out for #1707
         *

        if (!prolog_done && prolog_place == "after_flush" &&
            !fw->getOptionsObject()->getBool("use_iptables_restore"))
        {
            tmp << "prolog_commands" << endl;
            prolog_done = true;
        }
        */

        tmp << policy_compiler->printAutomaticRules();

        // printAutomaticRules() can generate errors and warnings
        if (policy_compiler->haveErrorsAndWarnings())
        {
            all_errors.push_back(policy_compiler->getErrors("").c_str());
            policy_compiler->clearErrors();
        }

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

    long auto_mangle_stream_position = automatic_mangle_stream.tellp();
    if (policy->isTop() && auto_mangle_stream_position <= 0)
    {
        // Note that we process non-top rule sets first and then
        // deal with the top rule set. By the time we get here the
        // have_connmark flags reflect the state of all other rule
        // sets and the top one.

        ostringstream tmp_m;
        tmp_m << mangle_compiler->printAutomaticRulesForMangleTable(
            have_connmark, have_connmark_in_output);

        // printAutomaticRulesForMangleTable() can generate errors and warnings
        if (mangle_compiler->haveErrorsAndWarnings())
        {
            all_errors.push_back(mangle_compiler->getErrors("").c_str());
            mangle_compiler->clearErrors();
        }

        if (tmp_m.tellp() > 0)
        {
            if (!single_rule_compile_on)
            {
                automatic_mangle_stream << "# ================ Table 'mangle', ";
                automatic_mangle_stream << "automatic rules";
                automatic_mangle_stream << "\n";
            }
            automatic_mangle_stream << tmp_m.str();
        }
    }

    return empty_output;
}
