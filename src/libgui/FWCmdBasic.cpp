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
#include "FWCmdBasic.h"
#include <QDebug>

using namespace libfwbuilder;


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
