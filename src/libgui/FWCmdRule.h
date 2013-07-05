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

#ifndef FWCMDRULE_H
#define FWCMDRULE_H

#include "FWCmdBasic.h"
#include "RuleSetView.h"
#include "RuleSetModel.h"
#include "FWCmdChange.h"

#include <QHash>
#include <QList>

namespace libfwbuilder
{
    class RuleSet;
    class RuleElement;
    class Rule;
    class FWObject;
}

/********************************************************
 * FWCmdRule
 ********************************************************/

class FWCmdRule : public FWCmdBasic
{

protected:
    libfwbuilder::RuleSet* ruleset;

    RuleSetView* getRuleSetView();
    RuleSetModel* getRuleSetModel();
    virtual void notify();

public:
    FWCmdRule(ProjectPanel *project, libfwbuilder::RuleSet* ruleset, QUndoCommand* macro=0);

    virtual void redo();
    virtual void undo();

    virtual void redoOnModel(RuleSetModel *md)=0;
    virtual void undoOnModel(RuleSetModel *md)=0;

};

/********************************************************
 * FWCmdRuleInsert
 ********************************************************/

class FWCmdRuleInsert : public FWCmdRule
{
    int position;

    libfwbuilder::Rule * ruleToInsert;
    libfwbuilder::Rule * insertedRule;
    bool isAfter;

public:
    FWCmdRuleInsert(ProjectPanel *project, libfwbuilder::RuleSet* ruleset, int position=0, bool isAfter=false, libfwbuilder::Rule* ruleToInsert=0);
    ~FWCmdRuleInsert();

    void redoOnModel(RuleSetModel *md);
    void undoOnModel(RuleSetModel *md);
    const libfwbuilder::Rule* getInsertedRule() const { return insertedRule; }
};

/********************************************************
 * FWCmdRuleDelete
 ********************************************************/

class FWCmdRuleDelete : public FWCmdRule
{    
    int row;

    void copyRules(QList<libfwbuilder::Rule*> &rules);
protected:
    QList<libfwbuilder::Rule*> rulesToDelete;

public:
    FWCmdRuleDelete(ProjectPanel *project, libfwbuilder::RuleSet* ruleset, QList<libfwbuilder::Rule*> &rulesToDelete, QUndoCommand* macro=0);
    ~FWCmdRuleDelete();

    virtual void redoOnModel(RuleSetModel *md);
    virtual void undoOnModel(RuleSetModel *md);
    const QList<libfwbuilder::Rule*> getDeletedRules() const { return this->rulesToDelete; }
};

/********************************************************
 * FWCmdRuleDeleteFromGroup
 ********************************************************/

class FWCmdRuleDeleteFromGroup : public FWCmdRuleDelete
{
//    int ruleId;
//    int row;
//    libfwbuilder::Rule* deletedRule;
//    QList<libfwbuilder::Rule*> rulesToDelete;

public:
    FWCmdRuleDeleteFromGroup(ProjectPanel *project, libfwbuilder::RuleSet* ruleset, QList<libfwbuilder::Rule*> rulesToDelete, QUndoCommand* macro=0);


//    void redoOnModel(RuleSetModel *md);
    void undoOnModel(RuleSetModel *md);
};

/********************************************************
 * FWCmdRuleColor
 ********************************************************/

class FWCmdRuleColor : public FWCmdRule
{
    QString newColor;
    QHash<int,QString> oldColors;
    QList<int> ruleIds;

public:
    FWCmdRuleColor(ProjectPanel *project, libfwbuilder::RuleSet* ruleset, QList<libfwbuilder::Rule*> &rules,const QString &newColor);

    void redoOnModel(RuleSetModel *md);
    void undoOnModel(RuleSetModel *md);
};

/********************************************************
 * FWCmdRuleRenameGroup
 ********************************************************/

class FWCmdRuleRenameGroup : public FWCmdRule
{    
    QString oldName;
    QString newName;

public:
    FWCmdRuleRenameGroup(ProjectPanel *project, libfwbuilder::RuleSet* ruleset, QString oldName, QString newName);

    void redoOnModel(RuleSetModel *md);
    void undoOnModel(RuleSetModel *md);
};

/********************************************************
 * FWCmdRuleRemoveFromGroup
 ********************************************************/

class FWCmdRuleRemoveFromGroup : public FWCmdRule
{    
    libfwbuilder::Rule* firstRule;
    libfwbuilder::Rule* lastRule;
    QString groupName;

public:
    FWCmdRuleRemoveFromGroup(ProjectPanel* project, libfwbuilder::RuleSet* ruleset, libfwbuilder::Rule* firstRule, libfwbuilder::Rule* lastRule,
                             const QString groupName, QUndoCommand* macro=0);

    void redoOnModel(RuleSetModel *md);
    void undoOnModel(RuleSetModel *md);
};

/********************************************************
 * FWCmdRuleNewGroup
 ********************************************************/

class FWCmdRuleNewGroup : public FWCmdRule
{
    libfwbuilder::Rule* firstRule;
    libfwbuilder::Rule* lastRule;
    QString groupName;

public:
    FWCmdRuleNewGroup(ProjectPanel* project, libfwbuilder::RuleSet* ruleset, libfwbuilder::Rule* firstRule, libfwbuilder::Rule* lastRule,
                             const QString groupName);

    void redoOnModel(RuleSetModel *md);
    void undoOnModel(RuleSetModel *md);
};

/********************************************************
 * FWCmdRuleAddToGroup
 ********************************************************/

class FWCmdRuleAddToGroup : public FWCmdRule
{
    libfwbuilder::Rule* firstRule;
    libfwbuilder::Rule* lastRule;
    bool isAbove;
    QString groupName;

public:
    FWCmdRuleAddToGroup(ProjectPanel* project, libfwbuilder::RuleSet* ruleset, libfwbuilder::Rule* firstRule, libfwbuilder::Rule* lastRule,
                             bool isAbove = true);

    void redoOnModel(RuleSetModel *md);
    void undoOnModel(RuleSetModel *md);
};

/********************************************************
 * FWCmdRuleMove
 ********************************************************/

class FWCmdRuleMove : public FWCmdRule
{

    int firstId;
    int lastId;
    bool direction; // true - up, false - down;

    void move(RuleSetModel *md, bool direction);

public:
    FWCmdRuleMove(ProjectPanel *project, libfwbuilder::RuleSet* ruleset, int firstId, int lastId, bool direction=true);

    void redoOnModel(RuleSetModel *md);
    void undoOnModel(RuleSetModel *md);

};

/********************************************************
 * FWCmdRuleChange
 ********************************************************/

class FWCmdRuleChange : public FWCmdChange
{
    libfwbuilder::RuleSet* ruleset;

protected:
    void prepareRuleSetView();
    libfwbuilder::Rule* selectAffectedRule();
    virtual libfwbuilder::Rule* getRule();

public:
    FWCmdRuleChange(ProjectPanel *project, libfwbuilder::RuleSet* ruleset,
                    libfwbuilder::FWObject *obj,
                    QString text=QString(), QUndoCommand* macro = 0);

    virtual void redo();
    virtual void undo();
    virtual void notify();
};

/********************************************************
 * FWCmdRuleChangeAction
 ********************************************************/

class FWCmdRuleChangeAction : public FWCmdRuleChange
{

public:
    FWCmdRuleChangeAction(ProjectPanel *project, libfwbuilder::FWObject *obj);

    virtual void notify();
};

/********************************************************
 * FWCmdRuleChangeComment
 ********************************************************/

class FWCmdRuleChangeComment : public FWCmdRuleChange
{

public:
    FWCmdRuleChangeComment(ProjectPanel *project, libfwbuilder::FWObject *obj);

    virtual void notify();
};

/********************************************************
 * FWCmdRuleChangeOptions
 ********************************************************/

class FWCmdRuleChangeOptions : public FWCmdRuleChange
{

public:
    FWCmdRuleChangeOptions(ProjectPanel *project, libfwbuilder::FWObject *obj);

    virtual void notify();
};

/********************************************************
 * FWCmdRuleChangeRe
 ********************************************************/

class FWCmdRuleChangeRe : public FWCmdRuleChange
{
    int position;
    int column;
    int number;
protected:
    virtual libfwbuilder::Rule* getRule();

public:
    FWCmdRuleChangeRe(ProjectPanel *project, libfwbuilder::RuleSet* ruleset,
                      libfwbuilder::FWObject *obj, int position, int column, int number, QString text=QString(), QUndoCommand* macro = 0);

    virtual void notify();

};

/********************************************************
 * FWCmdRuleNegateRE
 ********************************************************/

class FWCmdRuleNegateRE : public FWCmdRuleChangeRe
{

public:
    FWCmdRuleNegateRE(ProjectPanel *project, libfwbuilder::RuleSet* ruleset,
                      libfwbuilder::RuleElement* ruleElement,
                      int position, int column);

    virtual void redo();
    virtual void undo();

};

#endif // FWCMDRULE_H
