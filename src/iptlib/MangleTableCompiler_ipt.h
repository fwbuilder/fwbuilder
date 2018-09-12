/* 

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

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

#ifndef __MANGLETABLECOMPILER_IPT_HH
#define __MANGLETABLECOMPILER_IPT_HH

#include "PolicyCompiler_ipt.h"

namespace fwcompiler {

    class MangleTableCompiler_ipt : public PolicyCompiler_ipt {

	protected:

	virtual std::string myPlatformName();

        /**
         * this processor drops all rules except for those that require mangle table
         */
        DECLARE_POLICY_RULE_PROCESSOR(keepMangleTableRules);
        friend class keepMangleTableRules;

	public:

	MangleTableCompiler_ipt(libfwbuilder::FWObjectDatabase *_db,
                                libfwbuilder::Firewall *fw,
                                bool ipv6_policy,
                                fwcompiler::OSConfigurator *_oscnf,
                                std::map<const std::string, bool> *m_n_cmd_map
        ) :
        PolicyCompiler_ipt(_db, fw, ipv6_policy, _oscnf, m_n_cmd_map)
        {
            my_table = "mangle";
        }

	virtual int  prolog();
        virtual void addRuleFilter();
	
        virtual std::string flushAndSetDefaultPolicy();
        virtual std::string printAutomaticRules();

        std::string printAutomaticRulesForMangleTable(
            bool have_connmark, bool have_connmark_in_output);
    };
}

#endif
