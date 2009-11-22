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
    FWCmdRule(ProjectPanel *project, libfwbuilder::RuleSet* ruleset);

    virtual void redo();
    virtual void undo();

    virtual void redoOnModel(RuleSetModel *md)=0;
    virtual void redoOnBase()=0;

    virtual void undoOnModel(RuleSetModel *md)=0;
    virtual void undoOnBase()=0;

};

/********************************************************
 * FWCmdRuleInsert
 ********************************************************/

class FWCmdRuleInsert : public FWCmdRule
{
    libfwbuilder::Rule * posRule;
    libfwbuilder::Rule * newRule;
    bool isAfter;

protected:
    libfwbuilder::RuleSet* ruleset;

public:
    FWCmdRuleInsert(ProjectPanel *project, libfwbuilder::RuleSet* ruleset,libfwbuilder::Rule* posRule=0, bool isAfter=false, libfwbuilder::Rule* newRule=0);

    void redoOnModel(RuleSetModel *md);
    void redoOnBase();

    void undoOnModel(RuleSetModel *md);
    void undoOnBase();
};

#endif // FWCMDRULE_H
