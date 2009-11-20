/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: FWCmdChange.cpp 1808 2009-11-18 21:22:17Z vadim $

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
#include "FWCmdAddObject.h"
#include "FWWindow.h"
#include "events.h"

#include <QObject>
#include <QtDebug>


using namespace libfwbuilder;
using namespace std;

/********************************************************
 * FWCmdAddObject
 *
 * This command controls adding an object to another object. This can
 * be adding an object to a system group or adding interface to a host
 * or address to an interface and so on where object itself becomes a
 * child rather than reference to it gets created.
 *
 ********************************************************/

FWCmdAddObject::FWCmdAddObject(ProjectPanel *project,
                               FWObject *grp,
                               FWObject *mem,
                               QString text):
    FWCmdChange(project, grp, text)
{
    member = mem;
    require_complete_tree_reload = true;

    if (text.isEmpty())
    {
        setText(QObject::tr("Add %1 to group %2")
                .arg(QString::fromUtf8(member->getName().c_str()))
                .arg(QString::fromUtf8(grp->getName().c_str())));
    } else
    {
        setText(text);
    }
}

FWCmdAddObject::~FWCmdAddObject()
{
    if (member->getRefCounter() <= 1) delete member;
    else member->unref();
}

void FWCmdAddObject::undo()
{
    // Remove object this->member from the group but do not delete it.
    // We still keep pointer to it in this->newState
    FWObject* grp = getObject();
    grp->remove(member, false);
    if (fwbdebug) qDebug() << "FWCmdAddObject::undo() member->ref_counter=" 
                           << member->getRefCounter();
    QString filename = QString::fromUtf8(grp->getRoot()->getFileName().c_str());
    // if (require_complete_tree_reload)
    //     QCoreApplication::postEvent(mw, new reloadObjectTreeEvent(filename));
    // else
    //     QCoreApplication::postEvent(
    //         mw, new updateObjectAndSubtreeInTreeEvent(filename, grp->getId()));

    QCoreApplication::postEvent(mw, new updateObjectAndSubtreeImmediatelyEvent(filename, grp->getId()));
    QCoreApplication::postEvent(mw, new dataModifiedEvent(filename, grp->getId()));
    QCoreApplication::postEvent(mw, new openObjectInEditorEvent(filename, grp->getId()));
    QCoreApplication::postEvent(mw, new showObjectInTreeEvent(filename, grp->getId()));
}

void FWCmdAddObject::redo()
{
    // newState should have the new group member. Add it to the group.
    // This member could have been added to the newState, if so,
    // remove it but increment reference counter.
    FWObject* grp = getObject();
    FWObject *new_grp = getNewState();
    if (member == NULL)
    {
        // Find new object among children of newState.  member can be
        // NULL if FWCmdAddObject object is created before the new
        // object can be created. This is the case when we copy whole
        // subtree from one data file to another in
        // ObjectManipulator::actuallyPasteTo(). There we call
        // FWObjectDatabase::recursivelyCopySubtree which needs parent
        // object as a parameter, so we have to create newState before
        // new object is created.
        for (list<FWObject*>::iterator i=new_grp->begin(); i!=new_grp->end(); ++i)
        {
            FWObject *co = *i;
            if (!grp->hasChild(co))
            {
                member = co;
                break;
            }
        }
    }
    if (new_grp->hasChild(member))
    {
        member->ref();
        new_grp->remove(member, false); // and do not delete
    }
    grp->add(member);
    if (fwbdebug) qDebug() << "FWCmdAddObject::redo() member->ref_counter=" 
                           << member->getRefCounter();
    QString filename = QString::fromUtf8(grp->getRoot()->getFileName().c_str());

    QCoreApplication::postEvent(mw, new updateObjectAndSubtreeImmediatelyEvent(filename, grp->getId()));
    QCoreApplication::postEvent(mw, new dataModifiedEvent(filename, grp->getId()));
    // post openObjectInEditorEvent first so that editor panel opens
    // this matters if the tree needs to scroll to show the object when
    // showObjectInTreeEvent is posted because vertical size of the tree
    // changes when editor opens
    QCoreApplication::postEvent(mw, new openObjectInEditorEvent(filename, member->getId()));
    QCoreApplication::postEvent(mw, new showObjectInTreeEvent(filename, member->getId()));
}

void FWCmdAddObject::notify()
{
}

