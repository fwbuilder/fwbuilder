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
#include "fwbuilder/Firewall.h"


#include <QDebug>

using namespace libfwbuilder;

/********************************************************
 * FWCmdRule
 ********************************************************/

FWCmdRule::FWCmdRule(ProjectPanel *project, RuleSet* ruleset,
                     QUndoCommand* macro) : FWCmdBasic(project, macro)
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
    return (rsv != nullptr)?(RuleSetModel*)getRuleSetView()->model():nullptr;
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
    if (ruleToInsert) delete ruleToInsert;
    if (insertedRule && insertedRule->getRefCounter() == 1)
    {
        delete insertedRule;
    }
}

void FWCmdRuleInsert::redoOnModel(RuleSetModel *md)
{
    if (insertedRule == 0) {
        if (ruleToInsert == 0)
        {
            if (position == 0 && !isAfter)
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
            if (position!=0 || isAfter) index = md->indexForPosition(position);
            insertedRule = md->insertRule(ruleToInsert, index, isAfter);
        }
        insertedRule->ref();
    } else {
        md->insertRule(insertedRule);
    }

    getRuleSetView()->selectRE(insertedRule,0);

    setDiffType(insertedRule, DiffType::Add);
}

void FWCmdRuleInsert::undoOnModel(RuleSetModel *md)
{
    QModelIndex index = md->index(insertedRule);
    resetDiffType(insertedRule);
    getRuleSetView()->scrollTo(index,  QAbstractItemView::PositionAtCenter);
    getRuleSetView()->unselect();
    md->removeRow(index.row(), index.parent());
}

/********************************************************
 * FWCmdRuleDelete
 ********************************************************/

FWCmdRuleDelete::FWCmdRuleDelete(ProjectPanel *project, RuleSet* ruleset,
                                 QList<Rule*> &rulesToDelete, QUndoCommand* macro) :
        FWCmdRule(project, ruleset, macro)
{
    copyRules(rulesToDelete);


    setText(QObject::tr("delete rules"));
}

void FWCmdRuleDelete::copyRules(QList<Rule*> &rules)
{
    if (fwbdebug)
        qDebug() << "FWCmdRuleDelete::copyRules(QList<Rule*> &rules)";
    QList<int> positions;
    positions.append(-100);

    foreach(Rule* rule, rules)
    {        
        int pos = rule->getPosition();

        for(int i=positions.size()-1; i>=0; i-- )
        {
            if (pos > positions.at(i))
            {
                positions.insert(i+1, pos);
                rule->ref();
                rulesToDelete.insert(i, rule);
                break;
            }
        }
    }

    if (fwbdebug)
        qDebug() << "size:" << rulesToDelete.size();
    row = getRuleSetModel()->index(rulesToDelete.first(),0).row() - 1;
}

FWCmdRuleDelete::~FWCmdRuleDelete()
{
    if (fwbdebug)
        qDebug() << "FWCmdRuleDelete::~FWCmdRuleDelete()";
    foreach(Rule* rule, rulesToDelete)
    {
        if (rule != 0)
        {
            if (rule->getRefCounter() <= 1)
            {
                if (fwbdebug)
                    qDebug() << "* delete rule:" << rule->getId();
                delete rule;
            }
            else
            {
                if (fwbdebug)
                    qDebug() << "* unref rule:" << rule->getId();
                rule->unref();
            }
        }
    }
}

void FWCmdRuleDelete::redoOnModel(RuleSetModel *md)
{
    if (fwbdebug)
        qDebug() << "FWCmdRuleDelete::redoOnModel(RuleSetModel *md)";
    foreach(Rule* rule, rulesToDelete)
    {
        setDiffType(rule, DiffType::Remove);

        QModelIndex index = md->index(rule, 0);
        md->removeRow(index.row(), index.parent());
    }
    // invalidate selection in RuleSetView

    getRuleSetView()->unselect();
}

void FWCmdRuleDelete::undoOnModel(RuleSetModel *md)
{
    md->restoreRules(rulesToDelete);
    foreach(Rule* rule, rulesToDelete) {
        resetDiffType(rule);
    }
}


/********************************************************
 * FWCmdRuleDeleteFromGroup
 ********************************************************/

FWCmdRuleDeleteFromGroup::FWCmdRuleDeleteFromGroup(
    ProjectPanel *project,
    RuleSet* ruleset, QList<libfwbuilder::Rule*> rulesToDelete,
    QUndoCommand* macro) : FWCmdRuleDelete(project, ruleset, rulesToDelete, macro)
{
    setText(QObject::tr("delete rules from group"));
}

void FWCmdRuleDeleteFromGroup::undoOnModel(RuleSetModel *md)
{
    md->restoreRules(rulesToDelete, false);
    foreach(Rule* rule, rulesToDelete) {
        resetDiffType(rule);
    }
}

/********************************************************
 * FWCmdRuleColor
 ********************************************************/

FWCmdRuleColor::FWCmdRuleColor(
    ProjectPanel *project,
    RuleSet* ruleset, QList<Rule*> &rules,
    const QString &newColor) : FWCmdRule(project, ruleset), newColor(newColor)
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
    QModelIndexList indexes;

    foreach(int ruleId, oldColors.keys())
    {
        Rule* rule = Rule::cast(getObject(ruleId));
        if (rule != 0)
        {
            indexes.append(md->index(rule, 0));
        }
    }

    md->changeRuleColor(indexes, newColor);
}

void FWCmdRuleColor::undoOnModel(RuleSetModel *md)
{
    QModelIndexList indexes;

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

FWCmdRuleMove::FWCmdRuleMove(ProjectPanel *project, RuleSet* ruleset,
                             int firstId, int lastId, bool direction) :
    FWCmdRule(project, ruleset),
    firstId(firstId), lastId(lastId), direction(direction)
{
    setText((direction)?QObject::tr("move rule up"):QObject::tr("move rule down"));
}

void FWCmdRuleMove::redoOnModel(RuleSetModel *md)
{
    move(md, direction);
    for (int id = firstId; id <= lastId; ++id)
        setDiffType(Rule::cast(getObject(id)), DiffType::Move);
}

void FWCmdRuleMove::undoOnModel(RuleSetModel *md)
{
    move(md, !direction);
    for (int id = firstId; id <= lastId; ++id)
        resetDiffType(Rule::cast(getObject(id)));
}

void FWCmdRuleMove::move(RuleSetModel *md, bool direction)
{
    Rule* firstRule = Rule::cast(getObject(firstId));
    Rule* lastRule = Rule::cast(getObject(lastId));

    QModelIndex index = md->index(firstRule, 0);
    QModelIndex parent = index.parent();
    int first = index.row();
    index = md->index(lastRule, 0);
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
    
    project->getCurrentRuleSetView()->setSelectedRows(
            md->index(firstRule, 0),
            md->index(lastRule, 0));
}

/********************************************************
 * FWCmdRuleRenameGroup
 ********************************************************/

FWCmdRuleRenameGroup::FWCmdRuleRenameGroup(
    ProjectPanel *project, RuleSet* ruleset, QString oldName, QString newName) :
    FWCmdRule(project, ruleset), oldName(oldName), newName(newName)
{
    setText(QObject::tr("Rename group of rules"));
}

void FWCmdRuleRenameGroup::redoOnModel(RuleSetModel *md)
{
    project->getRenamedGroups().insert(project->getRenamedGroups().key(oldName, oldName),
                                    newName);

    QModelIndex grp = md->index(oldName);
    md->renameGroup(grp, newName);
}

void FWCmdRuleRenameGroup::undoOnModel(RuleSetModel *md)
{
    project->getRenamedGroups().insert(project->getRenamedGroups().key(newName),
                                    oldName);

    QModelIndex grp =  md->index(newName);
    md->renameGroup(grp, oldName);
}

/********************************************************
 * FWCmdRuleRemoveFromGroup
 ********************************************************/

FWCmdRuleRemoveFromGroup::FWCmdRuleRemoveFromGroup(
    ProjectPanel* project, RuleSet* ruleset,
    Rule* firstRule, Rule* lastRule, const QString groupName, QUndoCommand* macro) :
    FWCmdRule(project, ruleset, macro),
    firstRule(firstRule), lastRule(lastRule), groupName(groupName)
{
    setText(QObject::tr("remove object(s) from group ")+groupName);
}

void FWCmdRuleRemoveFromGroup::redoOnModel(RuleSetModel *md)
{
    for (int id = firstRule->getId(); id <= lastRule->getId(); ++id)
        setDiffType(Rule::cast(getObject(id)), DiffType::Move);

    QModelIndex group = md->index(groupName);
    QModelIndex first = md->index(firstRule, 0);
    QModelIndex last = md->index(lastRule, 0);
    md->removeFromGroup(group, first.row(), last.row());
}

void FWCmdRuleRemoveFromGroup::undoOnModel(RuleSetModel *md)
{
    for (int id = firstRule->getId(); id <= lastRule->getId(); ++id)
        resetDiffType(Rule::cast(getObject(id)));

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

FWCmdRuleNewGroup::FWCmdRuleNewGroup(
    ProjectPanel* project, RuleSet* ruleset,
    Rule* firstRule, Rule* lastRule, const QString groupName) :
    FWCmdRule(project, ruleset), firstRule(firstRule), lastRule(lastRule)
{
    this->groupName = getRuleSetModel()->findUniqueNameForGroup(groupName);

    setText(QObject::tr("create new group ")+this->groupName);
}

void FWCmdRuleNewGroup::redoOnModel(RuleSetModel *md)
{
    for (int id = firstRule->getId(); id <= lastRule->getId(); ++id)
        setDiffType(Rule::cast(getObject(id)), DiffType::Move);

    QModelIndex first = md->index(firstRule, 0);
    QModelIndex last = md->index(lastRule, 0);
    QModelIndex index = md->createNewGroup(groupName, first.row(), last.row());

    project->getCurrentRuleSetView()->setFirstColumnSpanned(index.row(), QModelIndex(), true);
}

void FWCmdRuleNewGroup::undoOnModel(RuleSetModel *md)
{
    for (int id = firstRule->getId(); id <= lastRule->getId(); ++id)
        resetDiffType(Rule::cast(getObject(id)));

    QModelIndex group = md->index(groupName);
    QModelIndex first = md->index(firstRule, 0);
    QModelIndex last = md->index(lastRule, 0);

    md->removeFromGroup(group, first.row(), last.row());
}

/********************************************************
 * FWCmdRuleAddToGroup
 ********************************************************/

FWCmdRuleAddToGroup::FWCmdRuleAddToGroup(
    ProjectPanel* project, RuleSet* ruleset,
    Rule* firstRule, Rule* lastRule, bool isAbove) :
    FWCmdRule(project, ruleset),
    firstRule(firstRule), lastRule(lastRule), isAbove(isAbove)
{
    setText((isAbove)?QObject::tr("add to group above"):QObject::tr("add to group below"));
}

void FWCmdRuleAddToGroup::redoOnModel(RuleSetModel *md)
{
    for (int id = firstRule->getId(); id <= lastRule->getId(); ++id)
        setDiffType(Rule::cast(getObject(id)), DiffType::Move);

    QModelIndex first = md->index(firstRule, 0);
    QModelIndex last = md->index(lastRule, 0);

    groupName = (isAbove)?
                md->addToGroupAbove(first.row(), last.row()):
                md->addToGroupBelow(first.row(), last.row());
}

void FWCmdRuleAddToGroup::undoOnModel(RuleSetModel *md)
{
    for (int id = firstRule->getId(); id <= lastRule->getId(); ++id)
        resetDiffType(Rule::cast(getObject(id)));

    QModelIndex group = md->index(groupName);
    QModelIndex first = md->index(firstRule, 0);
    QModelIndex last = md->index(lastRule, 0);

    md->removeFromGroup(group, first.row(), last.row());
}

/********************************************************
 * FWCmdRuleChange
 ********************************************************/

FWCmdRuleChange::FWCmdRuleChange(
    ProjectPanel *project, RuleSet* ruleset,
    FWObject *obj, QString text, QUndoCommand* macro) :
    FWCmdChange(project, obj, text, false, macro), ruleset(ruleset)
{
}

Rule* FWCmdRuleChange::selectAffectedRule()
{
    RuleSetView* rsv = project->getCurrentRuleSetView();
    RuleSetModel* md = (RuleSetModel*)rsv->model();

    Rule* currentRule = md->getRule(rsv->currentIndex());
    if(currentRule == 0 || (currentRule->getId() != getRule()->getId())) rsv->selectRE(getRule(), 0);
    return currentRule;
}

void FWCmdRuleChange::redo()
{
    prepareRuleSetView();
    FWCmdChange::redo();
    Rule *affectedRule = selectAffectedRule();
    if (affectedRule)
        setDiffType(affectedRule, DiffType::Edit);
}

void FWCmdRuleChange::undo()
{
    prepareRuleSetView();
    FWCmdChange::undo();
    Rule *affectedRule = selectAffectedRule();
    if (affectedRule)
        resetDiffType(affectedRule);
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
 * FWCmdRuleChangeAction
 *
 * This command is used when user modifies parameters of an action, not
 * when they change action of a rule.
 *
 ********************************************************/

FWCmdRuleChangeAction::FWCmdRuleChangeAction(
    ProjectPanel *project, FWObject *obj) :
    FWCmdRuleChange(project, RuleSet::cast(obj->getParent()),
                    obj, QObject::tr("Edit Rule Action"))
{};

void FWCmdRuleChangeAction::notify()
{
    FWCmdRuleChange::notify();

    if (mw->isEditorVisible())
    {
        QCoreApplication::postEvent(
            mw, new openOptObjectInEditorEvent(project->getFileName(),
                                               getRule()->getId(),
                                               ObjectEditor::optAction));
    }
}

/********************************************************
 * FWCmdRuleChangeComment
 ********************************************************/

FWCmdRuleChangeComment::FWCmdRuleChangeComment(
    ProjectPanel *project, FWObject *obj) :
    FWCmdRuleChange(project, RuleSet::cast(obj->getParent()),
                    obj, QObject::tr("Edit Rule Comment"))
{
}

void FWCmdRuleChangeComment::notify()
{
    FWCmdRuleChange::notify();

    if (mw->isEditorVisible())
    {
        QCoreApplication::postEvent(
            mw, new openOptObjectInEditorEvent(
                project->getFileName(),
                getRule()->getId(), ObjectEditor::optComment));
    }
}

/********************************************************
 * FWCmdRuleChangeOptions
 ********************************************************/

FWCmdRuleChangeOptions::FWCmdRuleChangeOptions(
    ProjectPanel *project, FWObject *obj) :
    FWCmdRuleChange(project,
                    RuleSet::cast(obj->getParent()),
                    obj, QObject::tr("Edit Rule Options"))
{};

void FWCmdRuleChangeOptions::notify()
{
    FWCmdRuleChange::notify();

    if (mw->isEditorVisible())
    {
        QCoreApplication::postEvent(
            mw, new openObjectInEditorEvent(
                project->getFileName(), getRule()->getId()));

    }
}

/********************************************************
 * FWCmdRuleChangeRe
 ********************************************************/

FWCmdRuleChangeRe::FWCmdRuleChangeRe(
    ProjectPanel *project, RuleSet* ruleset,
    FWObject *obj, int position,
    int column, int number, QString text, QUndoCommand* macro) :
    FWCmdRuleChange(project, ruleset, obj, text, macro)
{
    this->column = column;
    this->number = number;
    this->position = position;
}

Rule* FWCmdRuleChangeRe::getRule()
{
    return Rule::cast(getObject()->getParent());
}

void FWCmdRuleChangeRe::notify()
{
    FWCmdRuleChange::notify();
    project->getCurrentRuleSetView()->selectObject(position, column, number);
    mw->findObjectWidget->reset();
}

/********************************************************
 * FWCmdRuleNegateRE
 ********************************************************/

FWCmdRuleNegateRE::FWCmdRuleNegateRE(
    ProjectPanel *project,
    RuleSet* ruleset, RuleElement* ruleElement, int position, int column) :
    FWCmdRuleChangeRe(project, ruleset, ruleElement, position, column, 0, QObject::tr("Negate"))
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
    notify();
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
    notify();
}

