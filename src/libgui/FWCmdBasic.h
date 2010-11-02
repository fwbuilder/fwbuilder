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

#ifndef FWCMDBASIC_H
#define FWCMDBASIC_H

#include <QUndoCommand>
#include <QUndoStack>
#include <QDebug>

#include "ProjectPanel.h"

#include "fwbuilder/FWObject.h"

class FWCmdMacro : public QUndoCommand
{
public:
    FWCmdMacro(QString text) : QUndoCommand() {setText(text);}
    int id() const {return 1;}
    bool mergeWith(const QUndoCommand *other);
};

class FWCmdBasic : public QUndoCommand
{
    int obj_id;
protected:

    ProjectPanel *project;

public:
    FWCmdBasic(ProjectPanel *project, QUndoCommand* macro = 0);
    int objectId() {return obj_id;}
    void setObject(libfwbuilder::FWObject *object) {obj_id = object->getId();}
    libfwbuilder::FWObject* getObject();
    libfwbuilder::FWObject* getObject(int id);
    int id() const {return 1;}
    bool mergeWith(const QUndoCommand *other);

};

class FWCmdTerm : public QUndoCommand
{
public:
    FWCmdTerm() : QUndoCommand() {setText("Terminator");}
    int id() const {return 1;}
};

void undoAndRemoveLastCommand(QUndoStack* undoStack);



#endif // FWCMDBASIC_H
