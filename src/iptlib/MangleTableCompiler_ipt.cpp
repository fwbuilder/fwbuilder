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


#include "MangleTableCompiler_ipt.h"
#include "OSConfigurator_linux24.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Rule.h"

#include <iostream>
#include <list>
#include <algorithm>
#include <iterator>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

string MangleTableCompiler_ipt::myPlatformName() { return "iptables"; }


int MangleTableCompiler_ipt::prolog()
{
    return PolicyCompiler_ipt::prolog();
}

bool MangleTableCompiler_ipt::keepMangleTableRules::processNext()
{
    PolicyRule *rule = getNext(); if (rule==nullptr) return false;
    FWOptions *ruleopt = rule->getOptionsObject();
    PolicyCompiler_ipt *ipt_comp = dynamic_cast<PolicyCompiler_ipt*>(compiler);

    string ruleset_name = compiler->getRuleSetName();

    FWOptions *rulesetopts = ipt_comp->getSourceRuleSet()->getOptionsObject();
    if (rulesetopts->getBool("mangle_only_rule_set"))
        tmp_queue.push_back(rule);
    else
    {
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
                r= compiler->dbcopy->createPolicyRule();
                compiler->temp_ruleset->add(r);
                r->duplicate(rule);
                r->setStr("ipt_chain","PREROUTING");
                tmp_queue.push_back(r);
            }

            if (rule->getDirection()==PolicyRule::Undefined ||
                rule->getDirection()==PolicyRule::Both ||
                rule->getDirection()==PolicyRule::Outbound)
            {
                r= compiler->dbcopy->createPolicyRule();
                compiler->temp_ruleset->add(r);
                r->duplicate(rule);
                r->setStr("ipt_chain","POSTROUTING");
                tmp_queue.push_back(r);
            }

            // ticket #1415 User reports that only packets that went
            // through the FORWARD chain can match inbound "-i" and
            // outbound "-o" interface at the same time. Since we do
            // not allow both in and out interface matches in one rule
            // and have to use branch to do this, need to branch in
            // FORWARD chain as well so that inbound interface can be
            // matched in the branching rule and outbound interface
            // can be matched in a rule in the branch
            //
            // This is ugly, this means the branch will inspect the
            // packet at least twice - in PREROUTING and FORWARD, or
            // FORWARD and POSTROUTING chains.
            //
            // I mention above that some targets can only be used in
            // PREROUTING or POSTROUTING chains. It would help if
            // these tagrets worked in FORWARD chain, in that case we
            // could just branch in FORWARD instead of all thress chains.
            //
            r= compiler->dbcopy->createPolicyRule();
            compiler->temp_ruleset->add(r);
            r->duplicate(rule);
            r->setStr("ipt_chain","FORWARD");
            tmp_queue.push_back(r);

            // tmp_queue.push_back(rule);
            return true;
        }

        if (rule->getTagging() ||
            rule->getRouting() ||
            rule->getClassification() ||
            ruleopt->getBool("put_in_mangle_table")) tmp_queue.push_back(rule);
    }

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

// mangle table compiler is special, it needs additional parameters to
// generate automatic rules correctly. But virtual function
// printAutomaticRules() has no parameters so we have another one
// that takes parameters: printAutomaticRulesForMangleTable()
string MangleTableCompiler_ipt::printAutomaticRules()
{
    return "";
}

string MangleTableCompiler_ipt::printAutomaticRulesForMangleTable(
    bool have_connmark, bool have_connmark_in_output)
{
    ostringstream res;

    PolicyCompiler_ipt::PrintRule *prp = createPrintRuleProcessor();

    // res << prp->_declareTable();

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

    // iptables accepted TCPMSS target in filter table, FORWARD chain 
    // in the older versions, but requires it to be in mangle filter
    // starting somewhere 1.3.x
    string version = fw->getStr("version");
    if (XMLTools::version_compare(version, "1.3.0")>=0)
        res << prp->_clampTcpToMssRule();

    delete prp;

    return res.str();
}

