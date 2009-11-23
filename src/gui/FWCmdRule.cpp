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
    if (crs != ruleset) project->openRuleSet(ruleset, true);
    return project->getCurrentRuleSetView();
}

RuleSetModel* FWCmdRule::getRuleSetModel()
{
    RuleSetView* rsv = getRuleSetView();
    return (rsv != NULL)?(RuleSetModel*)getRuleSetView()->model():NULL;
}

void FWCmdRule::notify()
{
    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(project->getFileName(), ruleset->getId()));
}

void FWCmdRule::redo()
{
    RuleSetModel* md = getRuleSetModel();
    if (md != NULL)
        redoOnModel(md);
    else
        redoOnBase();

    notify();
}

void FWCmdRule::undo()
{
    RuleSetModel* md = getRuleSetModel();
    if (md != NULL)
        undoOnModel(md);
    else
        undoOnBase();

    notify();
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

void FWCmdRuleInsert::redoOnModel(RuleSetModel *md)
{
    if (posRule == 0)
    {
        newRule = md->insertNewRule();
    } else
    {
        QModelIndex index = md->index(posRule);
        newRule = md->insertNewRule(index);
    }
}

/*
 * TODO: there is a problem with creating rules by calling
 * RuleSet::insertRuleAtTop because the new rule does not get initialized
 * in the same way as is done by RuleSetModel::initRule(). So re-doing
 * this operation while some other rule set is visible in the GUI creates
 * a rule which is different from the rule created when this operation was
 * executed for the first time.
 */
void FWCmdRuleInsert::redoOnBase()
{
    if (posRule == 0)
    {
        ruleset->insertRuleAtTop();
    } else
    {
        if (isAfter)
            ruleset->appendRuleAfter(posRule->getPosition());
        else
            ruleset->insertRuleBefore(posRule->getPosition());
    }

    QCoreApplication::postEvent(
        mw, new openRulesetEvent(project->getFileName(), ruleset->getId()));
    QCoreApplication::postEvent(
        mw, new selectRuleElementEvent(project->getFileName(),
                                       newRule->getId(),
                                       ColDesc::GroupHandle));
}

void FWCmdRuleInsert::undoOnModel(RuleSetModel *md)
{
    QModelIndex index = md->index(newRule);
    md->removeRow(index.row(), index.parent());
}

void FWCmdRuleInsert::undoOnBase()
{
    ruleset->remove(newRule);
    QCoreApplication::postEvent(
        mw, new openRulesetEvent(project->getFileName(), ruleset->getId()));
}
