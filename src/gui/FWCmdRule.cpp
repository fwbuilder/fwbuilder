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
#include "FindObjectWidget.h"

#include "events.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/RuleElement.h"


#include <QDebug>

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
    redoOnModel(md);
    notify();
}

void FWCmdRule::undo()
{
    RuleSetModel* md = getRuleSetModel();
    undoOnModel(md);
    notify();
}

/********************************************************
 * FWCmdRuleInsert
 ********************************************************/

FWCmdRuleInsert::FWCmdRuleInsert(ProjectPanel *project, RuleSet* ruleset,
                                 int position, bool isAfter, Rule* ruleToInsert):
        FWCmdRule(project, ruleset)
{
    this->position = position;
    this->isAfter = isAfter;
    this->ruleToInsert = ruleToInsert;
    this->insertedRule = 0;

    setText(QObject::tr("insert rule"));
}

FWCmdRuleInsert::~FWCmdRuleInsert()
{

}

void FWCmdRuleInsert::redoOnModel(RuleSetModel *md)
{
    if (ruleToInsert == 0)
    {
        if (position == 0)
        {
            insertedRule = md->insertNewRule();
        } else
        {
            QModelIndex index = md->indexForPosition(position);
            insertedRule = md->insertNewRule(index, isAfter);
        }
    } else
    {
        QModelIndex index;
        if (position) index = md->indexForPosition(position);
        insertedRule = md->insertRule(ruleToInsert, index, isAfter);
    }
    getRuleSetView()->selectRE(insertedRule,0);
}

void FWCmdRuleInsert::undoOnModel(RuleSetModel *md)
{
    QModelIndex index = md->index(insertedRule);
    getRuleSetView()->scrollTo(index,  QAbstractItemView::PositionAtCenter);
    getRuleSetView()->unselect();
    md->removeRow(index.row(), index.parent());
    // do we need delete insertedRule?
    insertedRule = 0;
}

/********************************************************
 * FWCmdRuleColor
 ********************************************************/

FWCmdRuleColor::FWCmdRuleColor(ProjectPanel *project, RuleSet* ruleset, QList<Rule*> &rules,const QString &newColor):
        FWCmdRule(project, ruleset),newColor(newColor)
{
    foreach(Rule* rule, rules)
    {
        int id = rule->getId();
        FWOptions *ropt = rule->getOptionsObject();
        QString oldColor = QString::fromUtf8(ropt->getStr("color").c_str());

        oldColors[id] = oldColor;
    }

    setText(QObject::tr("change rule(s) color"));
}

void FWCmdRuleColor::redoOnModel(RuleSetModel *md)
{
    QList<QModelIndex> indexes;

    foreach(int ruleId, oldColors.keys())
    {
        Rule* rule = dynamic_cast<Rule*>(getObject(ruleId));
        if (rule != 0)
        {
            indexes.append(md->index(rule, 0));
        }
    }

    md->changeRuleColor(indexes, newColor);
}

void FWCmdRuleColor::undoOnModel(RuleSetModel *md)
{
    QList<QModelIndex> indexes;

    foreach(int ruleId, oldColors.keys())
    {
        indexes.clear();
        Rule* rule = dynamic_cast<Rule*>(getObject(ruleId));
        if (rule != 0)
        {
            indexes.append(md->index(rule, 0));
        }
        md->changeRuleColor(indexes, oldColors[ruleId]);
    }
}

/********************************************************
 * FWCmdRuleMove
 ********************************************************/

FWCmdRuleMove::FWCmdRuleMove(ProjectPanel *project, libfwbuilder::RuleSet* ruleset, int firstId, int lastId, bool direction):
        FWCmdRule(project, ruleset), firstId(firstId), lastId(lastId), direction(direction)
{
    setText((direction)?QObject::tr("move rule up"):QObject::tr("move rule down"));
}

void FWCmdRuleMove::redoOnModel(RuleSetModel *md)
{
    move(md, direction);
}

void FWCmdRuleMove::undoOnModel(RuleSetModel *md)
{
    move(md, !direction);
}

void FWCmdRuleMove::move(RuleSetModel *md, bool direction)
{
    QModelIndex index = md->index(Rule::cast(getObject(firstId)), 0);
    QModelIndex parent = index.parent();
    int first = index.row();
    index = md->index(Rule::cast(getObject(lastId)), 0);
    int last = index.row();

    if (direction)
    {
        // up
        md->moveRuleUp(parent , first, last);

    } else
    {
        // down
        md->moveRuleDown(parent , first, last);
    }
}

/********************************************************
 * FWCmdRuleRenameGroup
 ********************************************************/

FWCmdRuleRenameGroup::FWCmdRuleRenameGroup(ProjectPanel *project, RuleSet* ruleset, QString oldName, QString newName):
        FWCmdRule(project, ruleset), oldName(oldName), newName(newName)
{
    setText(QObject::tr("negate rule element"));
}

void FWCmdRuleRenameGroup::redoOnModel(RuleSetModel *md)
{
    QModelIndex grp = md->index(oldName);
    md->renameGroup(grp, newName);
}

void FWCmdRuleRenameGroup::undoOnModel(RuleSetModel *md)
{
    QModelIndex grp =  md->index(newName);
    md->renameGroup(grp, oldName);
}

/********************************************************
 * FWCmdRuleRemoveFromGroup
 ********************************************************/

FWCmdRuleRemoveFromGroup::FWCmdRuleRemoveFromGroup(ProjectPanel* project, RuleSet* ruleset, Rule* firstRule, Rule* lastRule, const QString groupName):
        FWCmdRule(project, ruleset), firstRule(firstRule), lastRule(lastRule), groupName(groupName)
{
    setText(QObject::tr("remove object(s) from group ")+groupName);
}

void FWCmdRuleRemoveFromGroup::redoOnModel(RuleSetModel *md)
{
    QModelIndex group = md->index(groupName);
    QModelIndex first = md->index(firstRule, 0);
    QModelIndex last = md->index(lastRule, 0);
    md->removeFromGroup(group, first.row(), last.row());
}

void FWCmdRuleRemoveFromGroup::undoOnModel(RuleSetModel *md)
{
    QModelIndex group = md->index(groupName);
    QModelIndex first = md->index(firstRule, 0);
    QModelIndex last = md->index(lastRule, 0);

    if (group.isValid())
    {
        // Group still present in the ruleset. rules need to be added to rhis group.
        if (first.row() - 1 == group.row())
        {
            md->addToGroupAbove(first.row(), last.row());
        } else
        {
            md->addToGroupBelow(first.row(), last.row());
        }
    } else
    {
        // Group was deleted. It should be created again.
        md->createNewGroup(groupName, first.row(), last.row());
    }
}

/********************************************************
 * FWCmdRuleNewGroup
 ********************************************************/

FWCmdRuleNewGroup::FWCmdRuleNewGroup(ProjectPanel* project, RuleSet* ruleset, Rule* firstRule, Rule* lastRule, const QString groupName):
        FWCmdRule(project, ruleset), firstRule(firstRule), lastRule(lastRule)
{
    this->groupName = getRuleSetModel()->findUniqueNameForGroup(groupName);

    setText(QObject::tr("create new group ")+this->groupName);
}

void FWCmdRuleNewGroup::redoOnModel(RuleSetModel *md)
{
    QModelIndex first = md->index(firstRule, 0);
    QModelIndex last = md->index(lastRule, 0);
    QModelIndex index = md->createNewGroup(groupName, first.row(), last.row());

    project->getCurrentRuleSetView()->setFirstColumnSpanned(index.row(), QModelIndex(), true);
}

void FWCmdRuleNewGroup::undoOnModel(RuleSetModel *md)
{
    QModelIndex group = md->index(groupName);
    QModelIndex first = md->index(firstRule, 0);
    QModelIndex last = md->index(lastRule, 0);

    md->removeFromGroup(group, first.row(), last.row());
}

/********************************************************
 * FWCmdRuleAddToGroup
 ********************************************************/

FWCmdRuleAddToGroup::FWCmdRuleAddToGroup(ProjectPanel* project, RuleSet* ruleset, Rule* firstRule, Rule* lastRule, bool isAbove):
        FWCmdRule(project, ruleset), firstRule(firstRule), lastRule(lastRule), isAbove(isAbove)
{
    setText((isAbove)?QObject::tr("add to group above"):QObject::tr("add to group below"));
}

void FWCmdRuleAddToGroup::redoOnModel(RuleSetModel *md)
{
    QModelIndex first = md->index(firstRule, 0);
    QModelIndex last = md->index(lastRule, 0);

    groupName = (isAbove)?
                md->addToGroupAbove(first.row(), last.row()):
                md->addToGroupBelow(first.row(), last.row());
}

void FWCmdRuleAddToGroup::undoOnModel(RuleSetModel *md)
{
    QModelIndex group = md->index(groupName);
    QModelIndex first = md->index(firstRule, 0);
    QModelIndex last = md->index(lastRule, 0);

    md->removeFromGroup(group, first.row(), last.row());
}

/********************************************************
 * FWCmdRuleChange
 ********************************************************/

void FWCmdRuleChange::selectAffectedRule()
{
    RuleSetView* rsv = project->getCurrentRuleSetView();
    RuleSetModel* md = (RuleSetModel*)rsv->model();

    Rule* currentRule = md->getRule(rsv->currentIndex());
    if(currentRule == 0 || (currentRule->getId() != getRule()->getId())) rsv->selectRE(getRule(), 0);
}

void FWCmdRuleChange::redo()
{
    prepareRuleSetView();
    FWCmdChange::redo();
    selectAffectedRule();

}

void FWCmdRuleChange::undo()
{
    prepareRuleSetView();
    FWCmdChange::undo();
    selectAffectedRule();
}

void FWCmdRuleChange::notify()
{
    RuleSetView* rsv = project->getCurrentRuleSetView();
    RuleSetModel* md = (RuleSetModel*)rsv->model();

    Rule* rule = getRule();

    md->rowChanged(md->index(rule, 0));
//    rsv->updateColumnSizeForIndex(md->index(rule, 0));
    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(project->getFileName(), ruleset->getId()));
}

void FWCmdRuleChange::prepareRuleSetView()
{
    RuleSet* crs = project->getCurrentRuleSet();
    if (crs != ruleset) project->openRuleSet(ruleset, true);

}

libfwbuilder::Rule* FWCmdRuleChange::getRule()
{
    return Rule::cast(getObject());
}

/********************************************************
 * FWCmdRuleChangeRe
 ********************************************************/

Rule* FWCmdRuleChangeRe::getRule()
{
    return Rule::cast(getObject()->getParent());
}

void FWCmdRuleChangeRe::notify()
{
    FWCmdRuleChange::notify();

    project->getCurrentRuleSetView()->unselect();
    mw->findObjectWidget->reset();
}

/********************************************************
 * FWCmdRuleNegateRE
 ********************************************************/

FWCmdRuleNegateRE::FWCmdRuleNegateRE(ProjectPanel *project,
                                     RuleSet* ruleset, RuleElement* ruleElement):
    FWCmdRuleChangeRe(project, ruleset, ruleElement, QObject::tr("Negate"))
{
}

void FWCmdRuleNegateRE::redo()
{
    prepareRuleSetView();
    RuleElement* ruleElement = RuleElement::cast(getObject());
    ruleElement->toggleNeg();

    RuleSetView* rsv = project->getCurrentRuleSetView();
    RuleSetModel* md = (RuleSetModel*)rsv->model();
    md->rowChanged(md->index(getRule(), 0));

    selectAffectedRule();
}

void FWCmdRuleNegateRE::undo()
{
    prepareRuleSetView();
    RuleElement* ruleElement = RuleElement::cast(getObject());
    ruleElement->toggleNeg();

    RuleSetView* rsv = project->getCurrentRuleSetView();
    RuleSetModel* md = (RuleSetModel*)rsv->model();
    md->rowChanged(md->index(getRule(), 0));

    selectAffectedRule();
}

