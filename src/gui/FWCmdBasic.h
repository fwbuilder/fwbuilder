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

#include "ProjectPanel.h"

#include "fwbuilder/FWObject.h"

class FWCmdBasic : public QUndoCommand
{
    int id;
protected:

    ProjectPanel *project;
public:
    FWCmdBasic(ProjectPanel *project);
    int objectId() {return id;}
    void setObjectId(libfwbuilder::FWObject *object) {id = object->getId();}
};

#endif // FWCMDBASIC_H
