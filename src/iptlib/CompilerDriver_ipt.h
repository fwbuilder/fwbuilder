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

#ifndef __COMPILER_DRIVER_IPT_HH__
#define __COMPILER_DRIVER_IPT_HH__

#include "../compiler_lib/CompilerDriver.h"

#include "PolicyCompiler_ipt.h"
#include "OSConfigurator_linux24.h"

#include <string>
#include <sstream>
#include <map>
#include <list>

#include <QString>
#include <QMap>


namespace libfwbuilder
{
    class FWObjectDatabase;
    class Cluster;
    class ClusterGroup;
    class Firewall;
    class RuleSet;
    class Interface;
    class Address;
    class PolicyRule;
};

namespace fwcompiler
{

    class CompilerDriver_ipt : public CompilerDriver
    {

        // commands that pass control to branch chains should go into
        // POSTROUTING or PREROUTING chains depending on the targets used
        // inside the branch. Branches that use mixed rules (both SNAT
        // and DNAT) will be split so that two separate chains are created, one
        // for all SNAT rules and another for all DNAT rules. Rules in
        // the top NAT ruleset that pass control to them will be placed into
        // PREROUTING or POSTROUTING chain depending on the target in the branch.
        // The following maps targets used in the branch to the ruleset name.
        // By convention, the chain created for the branch rules will be named
        // using combination of the ruleset name and word "PREROUTING"
        // or "POSTROUTING"
        std::map<std::string, std::list<std::string> > branch_ruleset_to_chain_mapping;
       
        std::unique_ptr<PolicyCompiler_ipt> createPolicyCompiler(
            libfwbuilder::Firewall *fw,
            bool ipv6_policy,
            fwcompiler::OSConfigurator *_oscnf,
            std::map<const std::string, bool> *m_n_commands_map);

        bool have_connmark;
        bool have_connmark_in_output;

public:

        CompilerDriver_ipt(libfwbuilder::FWObjectDatabase *db);
        virtual ~CompilerDriver_ipt();
        
        // create a copy of itself, including objdb
        virtual CompilerDriver* clone();
    
        virtual QString run(const std::string &cluster_id,
                            const std::string &firewall_id,
                            const std::string &single_rule_id);

        void assignRuleSetChain(libfwbuilder::RuleSet *ruleset);
        void findBranchesInMangleTable(libfwbuilder::Firewall*,
                                       std::list<libfwbuilder::FWObject*> &all_policies);

        std::string dumpScript(libfwbuilder::Firewall *fw,
                               const std::string& automatic_rules_script,
                               const std::string& automatic_mangle_script,
                               const std::string& nat_script,
                               const std::string& mangle_script,
                               const std::string& filter_script,
                               bool ipv6_policy);

        bool processPolicyRuleSet(
            libfwbuilder::Firewall *fw,
            libfwbuilder::FWObject *ruleset,
            const std::string &single_rule_id,
            std::ostringstream &filter_table_stream,
            std::ostringstream &mangle_table_stream,
            std::ostringstream &automatic_rules_stream,
            std::ostringstream &automatic_mangle_stream,
            fwcompiler::OSConfigurator_linux24 *oscnf,
            int policy_af,
            std::map<const std::string, bool> &minus_n_commands_filter,
            std::map<const std::string, bool> &minus_n_commands_mangle);

        bool processNatRuleSet(
            libfwbuilder::Firewall *fw,
            libfwbuilder::FWObject *ruleset,
            const std::string &single_rule_id,
            std::ostringstream &nat_stream,
            fwcompiler::OSConfigurator_linux24 *oscnf,
            int policy_af,
            std::map<const std::string, bool> &minus_n_commands_nat);

    };
};

#endif
