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
#include "FWObjectState.h"

/********************************************************
 * FWCmdChange
 ********************************************************/

class FWCmdChange : public FWCmdBasic
{
protected:
    FWObjectState *oldState;
    FWObjectState *newState;

    void notify();

public:
    FWCmdChange(ProjectPanel *project, libfwbuilder::FWObject *obj, FWObjectState *oldState, FWObjectState *newState);
    ~FWCmdChange();

    virtual FWObjectState* createState(libfwbuilder::FWObject *object)=0;

};

/********************************************************
 * FWCmdChangeTime
 ********************************************************/

class FWCmdChangeTime : public FWCmdChange
{

    FWObjectState* createState(libfwbuilder::FWObject *object);
public:
    FWCmdChangeTime(ProjectPanel *project, libfwbuilder::FWObject *obj, FWObjectState *newState);

    void redo();
    void undo();

};


#endif // FWCMDCHANGE_H
