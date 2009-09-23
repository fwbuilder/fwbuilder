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

#include "OSConfigurator_linux24.h"

#include <string>
#include <sstream>

#include <QString>
#include <QMap>


namespace libfwbuilder {
    class FWObjectDatabase;
    class Cluster;
    class ClusterGroup;
    class Firewall;
    class RuleSet;
    class Interface;
};

namespace fwcompiler {

    class CompilerDriver_ipt : public CompilerDriver {

public:

        CompilerDriver_ipt(libfwbuilder::FWObjectDatabase *db);

        // create a copy of itself, including objdb
        virtual CompilerDriver* clone();
    
        virtual std::string run(const std::string &cluster_id,
                                const std::string &firewall_id,
                                const std::string &single_rule_id);

        void assignRuleSetChain(libfwbuilder::RuleSet *ruleset);
        void findBranchesInMangleTable(libfwbuilder::Firewall*,
                                       std::list<libfwbuilder::FWObject*> &all_policies);

        std::string dumpScript(libfwbuilder::Firewall *fw,
                               const std::string& reset_script,
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
            fwcompiler::OSConfigurator_linux24 *oscnf,
            int policy_af,
            std::map<const std::string, bool> &minus_n_commands_filter,
            std::map<const std::string, bool> &minus_n_commands_mangle);

        virtual void processStateSyncGroups(libfwbuilder::Cluster *cluster,
                                            libfwbuilder::Firewall *member_fw);

    };
};

#endif
