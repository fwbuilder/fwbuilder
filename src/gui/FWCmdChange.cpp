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

#include "FWCmdChange.h"
#include "FWWindow.h"
#include "fwbuilder/FWObjectDatabase.h"

#include "events.h"

#include <QObject>

using namespace libfwbuilder;
using namespace std;

/********************************************************
 * FWCmdChange
 ********************************************************/

FWCmdChange::FWCmdChange(ProjectPanel *project, FWObject *obj, QString text):
    FWCmdBasic(project)
{
    setObject(obj);

    FWObjectDatabase* db = obj->getRoot();

    this->oldState = db->create(obj->getTypeName());
    this->newState = db->create(obj->getTypeName());

    this->oldState->duplicateForUndo(obj);
    this->newState->duplicateForUndo(obj);

    if (text.isEmpty())
    {
        setText(QObject::tr("Edit ") + QString::fromUtf8(obj->getName().c_str()));
    } else
    {
        setText(text);
    }
}

FWCmdChange::~FWCmdChange()
{
    delete (oldState);
    delete (newState);
}

void FWCmdChange::undo()
{
    FWObject* obj = getObject();
    obj->duplicateForUndo(oldState);
    notify();
}

void FWCmdChange::redo()
{
    FWObject* obj = getObject();
    obj->duplicateForUndo(newState);
    notify();
}

void FWCmdChange::notify()
{
    FWObject* obj = getObject();
    QString filename = QString::fromUtf8(obj->getRoot()->getFileName().c_str());
    QCoreApplication::postEvent(
        mw, new updateObjectInTreeEvent(filename, obj->getId()));
    QCoreApplication::postEvent(
        mw, new updateObjectInRulesetEvent(filename, obj->getId()));

    mw->openEditor(obj);
}

/********************************************************
 * FWCmdChangeRuleAction
 ********************************************************/

FWCmdChangeRuleAction::FWCmdChangeRuleAction(ProjectPanel *project, FWObject *obj) :
    FWCmdChange(project, obj, QObject::tr("Edit Rule Action"))
{}

void FWCmdChangeRuleAction::notify()
{
    FWObject* obj = getObject();
    mw->openOptEditor(obj, ObjectEditor::optAction);
}

