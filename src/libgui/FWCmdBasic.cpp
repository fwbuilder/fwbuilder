/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>


                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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
#include "FWCmdBasic.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Firewall.h"
#include <QDebug>

using namespace libfwbuilder;

#include "FWCmdRule.h"

FWCmdBasic::FWCmdBasic(ProjectPanel *project, QUndoCommand* macro):QUndoCommand(macro)
{
    this->project = project;
}

FWObject* FWCmdBasic::getObject()
{
    return getObject(obj_id);
}

FWObject* FWCmdBasic::getObject(int id)
{
    return project->db()->findInIndex(id);
}

bool FWCmdBasic::mergeWith(const QUndoCommand *other)
{
    if (fwbdebug)
    {
        qDebug() << "FWCmdBasic::mergeWith(const QUndoCommand *other) other="
                 << other;
        qDebug() << "cmd:" << other->text();
    }
    const FWCmdTerm* term = dynamic_cast<const FWCmdTerm*>(other);
    return term != 0;
}

bool FWCmdMacro::mergeWith(const QUndoCommand *other)
{
    if (fwbdebug)
    {
        qDebug() << "FWCmdMacro::mergeWith(const QUndoCommand *other) other="
                 << other;
        qDebug() << "cmd:" << other->text();
    }
    const FWCmdTerm* term = dynamic_cast<const FWCmdTerm*> (other);
    return term != 0;
}

void undoAndRemoveLastCommand(QUndoStack* undoStack)
{
    if (fwbdebug) qDebug() << "undoAndRemoveLastCommand(QUndoStack undoStack)";

    undoStack->undo();

    if (fwbdebug) qDebug() << "count:" << undoStack->count();

    if (undoStack->count() == 1) {
        undoStack->clear();
    } else {
        FWCmdTerm* cmd = new FWCmdTerm();
        undoStack->push(cmd);
    }
}

void FWCmdBasic::setDiffType(Rule *rule, DiffType::Type diffType)
{
    if (!rule) return;

    DiffType::Type origDiffType = static_cast<DiffType::Type>(project->getDiffType(rule->getId()));
    m_diffLog.insert(rule->getId(), origDiffType);

    addStatistics(rule, diffType, origDiffType);

    // If we do changes to an inserted rule, we ignore that
    // We want to know that we added a rule.
    // Exception: if we remove it ...
    // If we move an edited rule, we ignore that too :)
    if (((origDiffType == DiffType::Add) && (diffType != DiffType::Remove))
            || ((origDiffType == DiffType::Edit) && (diffType == DiffType::Move))) {

            diffType = origDiffType;
    }

    project->setDiffType(rule->getId(), diffType);
}

void FWCmdBasic::resetDiffType(Rule *rule)
{
    if (!rule) return;

    DiffType::Type currentDiffType = static_cast<DiffType::Type>(project->getDiffType(rule->getId()));
    DiffType::Type origDiffType = static_cast<DiffType::Type>(m_diffLog.value(rule->getId()));
    removeStatistics(rule, currentDiffType, origDiffType);
    project->setDiffType(rule->getId(), origDiffType);
}

void FWCmdBasic::addStatistics(Rule *rule, DiffType::Type diffType, DiffType::Type origDiffType)
{
    foreach (Firewall *fw, UsageResolver().findFirewallsForObject(rule, project->db())) {
        std::list<RuleSet *> rulesets;
        rulesets.push_back(fw->getPolicy());
        rulesets.push_back(fw->getNAT());
        rulesets.push_back(fw->getRouting());

        foreach(RuleSet *ruleSet, rulesets) {
            if (ruleSet && rule->isChildOf(ruleSet)) {

                // We only set the move DiffType if that's the only change
                // That a rule is added or edited is more important
                if ((diffType == DiffType::Move) && (origDiffType != DiffType::None)) continue;

                if (diffType == DiffType::Edit) {
                    if (origDiffType == DiffType::Move)
                        modifyStatistics(ruleSet->getId(), origDiffType, false);
                    // We only set the edit DiffType on existing rules
                    // That we edit a newly created rule is normal - but we want to know that it was added
                    if (origDiffType == DiffType::Add) continue;
                }

                // When deleting rules that are added - we only remove the add statistics
                if (diffType == DiffType::Remove) {
                    modifyStatistics(ruleSet->getId(), origDiffType, false);
                    if (origDiffType == DiffType::Add) continue;
                }
                // The actual update
                modifyStatistics(ruleSet->getId(), diffType, true);
            }
        }
    }
}

void FWCmdBasic::removeStatistics(Rule *rule, DiffType::Type diffType, DiffType::Type origDiffType)
{
    foreach (Firewall *fw, UsageResolver().findFirewallsForObject(rule, project->db())) {
        std::list<RuleSet *> rulesets;
        rulesets.push_back(fw->getPolicy());
        rulesets.push_back(fw->getNAT());
        rulesets.push_back(fw->getRouting());

        foreach(RuleSet *ruleSet, rulesets) {
            if (ruleSet && rule->isChildOf(ruleSet)) {
                if ((origDiffType == DiffType::Add) && (diffType == DiffType::Remove)) {
                    // When reinserting a previously removed rule, that was added to the rule set.
                    // we only need to update the add statistics
                    modifyStatistics(ruleSet->getId(), origDiffType, true);
                    continue;
                }
                // The actual update
                modifyStatistics(ruleSet->getId(), diffType, false);
                modifyStatistics(ruleSet->getId(), origDiffType, true);
            }
        }
    }
}

void FWCmdBasic::modifyStatistics(int id, DiffType::Type diffType, bool increment)
{
    if (diffType == DiffType::None) return;

    int n = project->getStatistics().value(
                QPair<int, DiffType::Type>(id, diffType));

    project->getStatistics().insert(
                QPair<int, DiffType::Type>(id, diffType), increment ? ++n : --n);
}
