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

#include "config.h"

#include "MangleTableCompiler_ipt.h"
#include "OSConfigurator_linux24.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Rule.h"

#include <iostream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string MangleTableCompiler_ipt::myPlatformName() { return "iptables"; }

int MangleTableCompiler_ipt::prolog()
{
    return PolicyCompiler_ipt::prolog();

    int n = 0;

    for(FWObject::iterator i=combined_ruleset->begin();
        i!=combined_ruleset->end(); i++)
    {
	PolicyRule *r = PolicyRule::cast( *i );
        FWOptions *ruleopt = r->getOptionsObject();
	if (r->isDisabled()) continue;
        if (r->getAction() == PolicyRule::Tag ||
            r->getAction() == PolicyRule::Classify) n++;
        if (r->getAction() == PolicyRule::Branch &&
            ruleopt->getBool("ipt_branch_in_mangle")) n++;
    }
    return n;
}

bool MangleTableCompiler_ipt::keepMangleTableRules::processNext()
{
    PolicyRule *rule=getNext(); if (rule==NULL) return false;
    FWOptions *ruleopt =rule->getOptionsObject();

    if (rule->getAction() == PolicyRule::Branch &&
        ruleopt->getBool("ipt_branch_in_mangle"))
    {
        PolicyRule* r;
        
        // this is a branching rule for mangle table. Need to put it
        // into PREROUTING and POSTROUTING chains as well because some
        // targets that work with mangle table can only go into these
        // chains, yet we do not know what kind of rules will user
        // place in the branch

        if (rule->getDirection()==PolicyRule::Undefined ||
            rule->getDirection()==PolicyRule::Both ||
            rule->getDirection()==PolicyRule::Inbound)
        {
            r= PolicyRule::cast(compiler->dbcopy->create(PolicyRule::TYPENAME));
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setStr("ipt_chain","PREROUTING");
            tmp_queue.push_back(r);
        }

        if (rule->getDirection()==PolicyRule::Undefined ||
            rule->getDirection()==PolicyRule::Both ||
            rule->getDirection()==PolicyRule::Outbound)
        {
            r= PolicyRule::cast(compiler->dbcopy->create(PolicyRule::TYPENAME));
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setStr("ipt_chain","POSTROUTING");
            tmp_queue.push_back(r);
        }

        tmp_queue.push_back(rule);
    }

    if (rule->getAction() == PolicyRule::Tag ||
        rule->getAction() == PolicyRule::Route ||
        rule->getAction() == PolicyRule::Classify ||
        ruleopt->getBool("put_in_mangle_table")) tmp_queue.push_back(rule);

    return true;
}


void MangleTableCompiler_ipt::addRuleFilter()
{
    add(new keepMangleTableRules("keep only rules that require mangle table"));
}

string MangleTableCompiler_ipt::flushAndSetDefaultPolicy()
{
    return "";
}


string MangleTableCompiler_ipt::printAutomaticRulesForMangleTable(
    bool have_connmark, bool have_connmark_in_output)
{
    ostringstream res;

    PolicyCompiler_ipt::PrintRule *prp = createPrintRuleProcessor();

    res << prp->_declareTable();

    if (have_connmark)
    {
        res << prp->_startRuleLine()
            << "PREROUTING -j CONNMARK --restore-mark"
            << prp->_endRuleLine();
    }
    if (have_connmark_in_output)
    {
        res << prp->_startRuleLine()
            << "OUTPUT -j CONNMARK --restore-mark"
            << prp->_endRuleLine();
        res << endl;
    }

    string version = fw->getStr("version");
    if (version == "1.3.0" || version == "1.4.0")
        res << prp->_clampTcpToMssRule();

    return res.str();
}

