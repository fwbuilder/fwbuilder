/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef FWCMDDELETEOBJECT_H
#define FWCMDDELETEOBJECT_H

#include "FWCmdChange.h"

/********************************************************
 * FWCmdDeleteObject
 ********************************************************/

class FWCmdDeleteObject : public FWCmdChange
{
    libfwbuilder::FWObject *parent;
    libfwbuilder::FWObject *delobj;

protected:
    virtual void notify();

public:
    FWCmdDeleteObject(ProjectPanel *project,
                      libfwbuilder::FWObject *obj,
                      QString text=QString(),
                      QUndoCommand* macro=0);
    ~FWCmdDeleteObject();
    virtual void redo();
    virtual void undo();
};


class FWCmdRemoveUserFolder : public FWCmdChange
{
    QString m_userFolder;

public:
    FWCmdRemoveUserFolder(ProjectPanel *project,
                          libfwbuilder::FWObject *parentFolder,
                          const QString &userFolder,
                          QString text = QString(),
                          QUndoCommand *macro = 0);

    virtual void redo();
    virtual void undo();
    virtual void notify();
};


#endif
