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
#include "NATCompiler_ipt.h"
#include "OSConfigurator_linux24.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Library.h"

#include <fstream>
#include <iostream>
#include <memory>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


bool CompilerDriver_ipt::processNatRuleSet(
    Firewall *fw,
    FWObject *ruleset,
    const std::string &single_rule_id,
    std::ostringstream &nat_rules_stream,
    fwcompiler::OSConfigurator_linux24 *oscnf,
    int policy_af,
    std::map<const std::string, bool> &minus_n_commands_nat)
{
    int nat_rules_count  = 0;
    string host_os = fw->getStr("host_OS");
    bool flush_and_set_default_policy = Resources::getTargetOptionBool(
        host_os, "default/flush_and_set_default_policy");
    bool empty_output = true;

    NAT *nat = NAT::cast(ruleset);
    assignRuleSetChain(nat);
    string branch_name = nat->getName();
                
    if (!nat->matchingAddressFamily(policy_af)) return true;

    bool ipv6_policy = (policy_af == AF_INET6);

    // compile NAT rules before policy rules because policy
    // compiler needs to know the number of virtual addresses
    // being created for NAT
    std::unique_ptr<NATCompiler_ipt> nat_compiler(
        new NATCompiler_ipt(objdb, fw, ipv6_policy,
                            oscnf, &minus_n_commands_nat));

    if (!nat->isTop())
        nat_compiler->registerRuleSetChain(branch_name);

    nat_compiler->setSourceRuleSet( nat );
    nat_compiler->setRuleSetName(branch_name);
    nat_compiler->setPersistentObjects(persistent_objects);

    nat_compiler->setSingleRuleCompileMode(single_rule_id);
    nat_compiler->setDebugLevel( dl );
    if (rule_debug_on) nat_compiler->setDebugRule(  drn );
    nat_compiler->setVerbose( (bool)(verbose) );
    nat_compiler->setHaveDynamicInterfaces(have_dynamic_interfaces);
    if (inTestMode()) nat_compiler->setTestMode();
    if (inEmbeddedMode()) nat_compiler->setEmbeddedMode();

    nat_compiler->setRulesetToChainMapping(&branch_ruleset_to_chain_mapping);

    if ( (nat_rules_count=nat_compiler->prolog()) > 0 )
    {
        nat_compiler->compile();
        nat_compiler->epilog();
    }

    have_nat = (have_nat || (nat_rules_count > 0));

    if (nat_compiler->getCompiledScriptLength() > 0)
    {
        if (!single_rule_compile_on)
            nat_rules_stream << "# ================ Table 'nat', "
                             << " rule set "
                             << branch_name << "\n";

        if (nat->isTop())
        {
            if (flush_and_set_default_policy)
                nat_rules_stream << nat_compiler->flushAndSetDefaultPolicy();

            nat_rules_stream << nat_compiler->printAutomaticRules();
        }

        nat_rules_stream << nat_compiler->getCompiledScript();
        nat_rules_stream << "\n";
        empty_output = false;

        branch_ruleset_to_chain_mapping[branch_name] = nat_compiler->getUsedChains();
    }

    if (nat_compiler->haveErrorsAndWarnings())
    {
        all_errors.push_back(nat_compiler->getErrors("").c_str());
    }

    return empty_output;
}

