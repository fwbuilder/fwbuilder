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

#ifndef FWCMDMOVEOBJECT_H
#define FWCMDMOVEOBJECT_H

#include "FWCmdChange.h"
#include <map>
#include <set>

/********************************************************
 * FWCmdMoveObject
 ********************************************************/

class FWCmdMoveObject : public FWCmdBasic
{
    libfwbuilder::FWObject *old_parent;
    libfwbuilder::FWObject *new_parent;
    libfwbuilder::FWObject *current_parent;
    libfwbuilder::FWObject *obj;
    QString oldUserFolder;
    std::map<int, std::set<libfwbuilder::FWObject*> > reference_holders;

protected:
    virtual void notify();

public:
    FWCmdMoveObject(ProjectPanel *project,
                    libfwbuilder::FWObject *old_parent,
                    libfwbuilder::FWObject *new_parent,
                    libfwbuilder::FWObject *obj,
                    std::map<int, std::set<libfwbuilder::FWObject*> >  &reference_holders,
                    QString text=QString(),
                    QUndoCommand* macro = 0);
    ~FWCmdMoveObject();
    virtual void redo();
    virtual void undo();
};

/*************************************************************/

class FWCmdMoveToFromUserFolder : public FWCmdChange
{
    int m_parentId;
    QString m_oldFolder;
    QString m_newFolder;

public:
    FWCmdMoveToFromUserFolder(ProjectPanel *project,
                              libfwbuilder::FWObject *parent,
                              libfwbuilder::FWObject *obj,
                              const QString &oldFolder,
                              const QString &newFolder,
                              QString text = QString(),
                              QUndoCommand *macro = 0);

    virtual void redo();
    virtual void undo();
};

#endif
