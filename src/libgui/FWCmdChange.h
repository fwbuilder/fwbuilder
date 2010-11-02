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

#ifndef FWCMDCHANGE_H
#define FWCMDCHANGE_H

#include "FWCmdBasic.h"

/********************************************************
 * FWCmdChange
 ********************************************************/

class FWCmdChange : public FWCmdBasic
{

    libfwbuilder::FWObject *oldState;
    libfwbuilder::FWObject *newState;
    bool rename_children;
    bool first_time;

protected:
    virtual void notify();

public:
    FWCmdChange(ProjectPanel *project,
                libfwbuilder::FWObject *obj,
                QString text=QString(),
                bool rename_children = false,
                QUndoCommand* macro = 0);
    ~FWCmdChange();

    libfwbuilder::FWObject* getOldState() {return oldState;}
    libfwbuilder::FWObject* getNewState() {return newState;}

    virtual void redo();
    virtual void undo();
};

/********************************************************
 * FWCmdChangeName
 ********************************************************/

class FWCmdChangeName : public FWCmdChange
{
protected:
    virtual void notify();
public:
    FWCmdChangeName(ProjectPanel *project, libfwbuilder::FWObject *obj);
};

/********************************************************
 * FWCmdLockObject
 *
 * This command locks or unlocks object
 ********************************************************/

class FWCmdLockObject : public FWCmdChange
{
protected:
public:
    FWCmdLockObject(ProjectPanel *project, libfwbuilder::FWObject *obj,
                    QString text=QString());
    virtual void redo();
    virtual void undo();
};

/********************************************************
 * FWCmdChangeOptionsObject
 ********************************************************/

class FWCmdChangeOptionsObject : public FWCmdChange
{
protected:
    virtual void notify();

public:
    FWCmdChangeOptionsObject(ProjectPanel *project, libfwbuilder::FWObject *obj);
};


#endif // FWCMDCHANGE_H
