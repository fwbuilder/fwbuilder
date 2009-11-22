/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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

#include "global.h"

#include "FWWindow.h"
#include "FWCmdRule.h"

#include "events.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/RuleElement.h"

using namespace libfwbuilder;

/********************************************************
 * FWCmdRule
 ********************************************************/

FWCmdRule::FWCmdRule(ProjectPanel *project, libfwbuilder::RuleSet* ruleset) : FWCmdBasic(project)
{
    this->ruleset = ruleset;
}

RuleSetView* FWCmdRule::getRuleSetView()
{
    RuleSet* crs = project->getCurrentRuleSet();
    if (crs != ruleset)
    {
        project->openRuleSet(ruleset);
    }
    return project->getCurrentRuleSetView();
}

RuleSetModel* FWCmdRule::getRuleSetModel()
{
    return (RuleSetModel*)getRuleSetView()->model();
}

void FWCmdRule::notify()
{
    RuleSetModel* md = getRuleSetModel();
    QCoreApplication::postEvent(mw, new dataModifiedEvent(project->getFileName(), md->getRuleSet()->getId()));
}

/********************************************************
 * FWCmdRuleInsert
 ********************************************************/

FWCmdRuleInsert::FWCmdRuleInsert(ProjectPanel *project, RuleSet* ruleset, Rule* posRule, bool isAfter, Rule* newRule):
        FWCmdRule(project, ruleset)
{
    this->posRule = posRule;
    this->isAfter = isAfter;
    this->newRule = newRule;

    setText(QObject::tr("insert rule"));
}

void FWCmdRuleInsert::redo()
{
    RuleSetModel* md = getRuleSetModel();
    if (posRule == 0)
    {
        newRule = md->insertNewRule();
    } else
    {
        QModelIndex index = md->index(posRule);
        newRule = md->insertNewRule(index);
    }
    notify();
}

void FWCmdRuleInsert::undo()
{
    RuleSetModel* md = getRuleSetModel();
    QModelIndex index = md->index(newRule);
    md->removeRow(index.row(), index.parent());
    notify();
}
