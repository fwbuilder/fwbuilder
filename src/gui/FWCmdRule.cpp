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

FWCmdRuleInsert::FWCmdRuleInsert(ProjectPanel *project, RuleSet* ruleset, int position, bool isAfter, Rule* ruleToInsert):
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
    } else {

        QModelIndex index = md->indexForPosition(position);

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

FWCmdRuleColor::FWCmdRuleColor(ProjectPanel *project, libfwbuilder::RuleSet* ruleset, QList<Rule*> &rules,const QString &newColor):
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
 * FWCmdRuleChange
 ********************************************************/

void FWCmdRuleChange::redo()
{
    prepareRuleSetView();
    FWCmdChange::redo();
    project->getCurrentRuleSetView()->selectRE(getRule(), 0);
}

void FWCmdRuleChange::undo()
{
    prepareRuleSetView();
    FWCmdChange::undo();
    project->getCurrentRuleSetView()->selectRE(getRule(), 0);
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
    return dynamic_cast<Rule*> (getObject());
}
