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

#include "events.h"


using namespace libfwbuilder;
using namespace std;

void FWCmdChange::notify()
{
    FWObject* obj = object();
    QCoreApplication::postEvent(
    mw, new dataModifiedEvent(QString::fromUtf8(obj->getRoot()->getFileName().c_str()),
                              obj->getId()));
}

FWCmdChange::FWCmdChange(ProjectPanel *project, libfwbuilder::FWObject *obj, FWObjectState *oldState, FWObjectState *newState):
            FWCmdBasic(project)
{
    setObject(obj);
    this->oldState = oldState;
    this->newState = newState;
}

FWCmdChange::~FWCmdChange()
{
    delete (oldState);
    delete (newState);
}
FWCmdChangeTime::FWCmdChangeTime(ProjectPanel *project, libfwbuilder::FWObject *obj, FWObjectState *newState):
            FWCmdChange(project, obj, createState(obj), newState)
{
    setText("edit object");
}

FWObjectState* FWCmdChangeTime::createState(libfwbuilder::FWObject *obj)
{
    FWObjectStateTime* state = new FWObjectStateTime();
    state->save(obj);
    return state;
}

void FWCmdChangeTime::undo()
{
    FWObject* obj = object();

    oldState->restore(obj);
    project->updateObjName(obj, newState->name);
    notify();
}

void FWCmdChangeTime::redo()
{
    FWObject* obj = object();
    newState->restore(obj);
    project->updateObjName(obj, newState->name);
    notify();
}
