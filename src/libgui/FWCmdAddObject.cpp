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

#include "global.h"

#include "FWCmdChange.h"
#include "FWCmdAddObject.h"
#include "FWWindow.h"
#include "events.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/RuleSet.h"

#include <QObject>
#include <QtDebug>


using namespace libfwbuilder;
using namespace std;

/********************************************************
 * FWCmdAddObject
 *
 * This command controls adding an object to another object. This can
 * be adding an object to a system group or adding interface to a host
 * or address to an interface and so on, where object itself becomes a
 * child rather than reference to it gets created.
 *
 ********************************************************/

FWCmdAddObject::FWCmdAddObject(ProjectPanel *project,
                               FWObject *grp,
                               FWObject *mem,
                               QString text,
                               QUndoCommand* macro):
    FWCmdChange(project, grp, text, false, macro)
{
    member = mem;
    require_complete_tree_reload = false;

    if (text.isEmpty())
    {
        setText(QObject::tr("Add object to group"));
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
    if (Firewall::isA(member) && project->getCurrentRuleSet()->isChildOf(member))
    {
        // need to close ruleset view
        project->closeRuleSetPanel();
    }

    QString filename = QString::fromUtf8(grp->getRoot()->getFileName().c_str());

    QCoreApplication::postEvent(
        mw, new removeObjectFromTreeEvent(filename, member->getId()));

    QCoreApplication::postEvent(
        mw, new updateObjectAndSubtreeImmediatelyEvent(filename, grp->getId()));

    // see comment in redo()
    QCoreApplication::sendPostedEvents(mw,  0);

    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(filename, grp->getId()));

    if (mw->isEditorVisible())
        QCoreApplication::postEvent(
            mw, new openObjectInEditorEvent(filename, grp->getId()));

    QCoreApplication::postEvent(
        mw, new showObjectInTreeEvent(filename, grp->getId()));
}

void FWCmdAddObject::prepareStatesForRedo()
{
    // newState should have the new group member. Add it to the group.
    // This member could have been added to the newState, if so,
    // remove it but increment reference counter.
    FWObject* grp = getObject();
    FWObject *new_grp = getNewState();
    if (member == nullptr)
    {
        // Find new object among children of newState.  member can be
        // nullptr if FWCmdAddObject object is created before the new
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
}

void FWCmdAddObject::redo()
{
    prepareStatesForRedo();

    FWObject* grp = getObject();

    grp->add(member);

    if (fwbdebug)
        qDebug() << "FWCmdAddObject::redo()"
                 << member->getName().c_str() << "-->"
                 << grp->getName().c_str()
                 << "member->ref_counter=" 
                 << member->getRefCounter()
                 << "reload=" << require_complete_tree_reload
                 << "editor=" << mw->isEditorVisible();

    QString filename = QString::fromUtf8(grp->getRoot()->getFileName().c_str());

    // updateObjectAndSubtreeImmediatelyEvent updates the part of the
    // tree where object we just added is attached. This rebuilds
    // subtree. However, if the object was copied from another data
    // file and dragged with it some other objects (as dependencies)
    // in other parts of the tree, rebulding the subtree is
    // insufficient, we need to reload the whole tree. The caller should have
    // set flag require_complete_tree_reload to signal that.
    if (require_complete_tree_reload)
        QCoreApplication::postEvent(
            mw, new reloadObjectTreeImmediatelyEvent(filename));
    else
    {
        QCoreApplication::postEvent(
            mw, new insertObjectInTreeEvent(filename, grp->getId(), member->getId()));

        QCoreApplication::postEvent(
            mw, new updateObjectAndSubtreeImmediatelyEvent(
                filename, grp->getId()));
    }

    // when user clicks in the undo stack window, the program executes
    // all undo/redo commands up to the line they clicked on. This
    // means bunch of redo() commands can execute at once. All events
    // they posted, however, will be processed after that. If these
    // redo() commands added or removed objects in the tree, the tree
    // widget in ObjectManipulator gets updated after that has
    // happened and gets out of sync with object
    // tree. sendPostedEvents() should send events posted up to this
    // point to the receiver immediately, which in this case should
    // update widgets to keep them in sync.
    QCoreApplication::sendPostedEvents(mw,  0);

    QCoreApplication::postEvent(mw, new dataModifiedEvent(filename, grp->getId()));

    // post openObjectInEditorEvent first so that editor panel opens
    // this matters if the tree needs to scroll to show the object when
    // showObjectInTreeEvent is posted because vertical size of the tree
    // changes when editor opens

    //if (mw->isEditorVisible())
    QCoreApplication::postEvent(mw, new openObjectInEditorEvent(
                                    filename, member->getId()));
    QCoreApplication::postEvent(mw, new showObjectInTreeEvent(
                                    filename, member->getId()));
}

void FWCmdAddObject::notify()
{
}


/********************************************************
 * FWCmdAddUserFolder
 *
 ********************************************************/

FWCmdAddUserFolder::FWCmdAddUserFolder(ProjectPanel *project,
                                       FWObject *parentFolder,
                                       const QString &userFolder,
                                       QString text,
                                       QUndoCommand *macro)
    : FWCmdChange(project, parentFolder, text, false, macro),
      m_userFolder(userFolder)
{
}


void FWCmdAddUserFolder::redo()
{
    FWCmdChange::redo();

    FWObject *obj = getObject();

    QString fileName = QString::fromUtf8(obj->getRoot()->getFileName().c_str());
    QCoreApplication::postEvent(mw, new addUserFolderEvent
                                (fileName, obj->getId(), m_userFolder));
}


void FWCmdAddUserFolder::undo()
{
    FWCmdChange::undo();

    FWObject *obj = getObject();
    QString fName = QString::fromUtf8(obj->getRoot()->getFileName().c_str());
    QCoreApplication::postEvent(mw,
                                new removeUserFolderEvent(fName, obj->getId(),
                                                          m_userFolder));
}

/* We don't want to display the system folder after we add a user
   folder, so we just swallow the notify. */
void FWCmdAddUserFolder::notify()
{
}

/********************************************************
 * FWCmdAddLibrary
 *
 ********************************************************/

FWCmdAddLibrary::FWCmdAddLibrary(ProjectPanel *project,
                                 FWObject *root,
                                 FWObject *lib,
                                 QString text,
                                 QUndoCommand* macro):
    FWCmdAddObject(project, root, lib, text, macro)
{
    assert(FWObjectDatabase::cast(root)!=nullptr);

    if (text.isEmpty())
    {
        setText(QObject::tr("Add library"));
    } else
    {
        setText(text);
    }
}

FWCmdAddLibrary::~FWCmdAddLibrary() {}

void FWCmdAddLibrary::undo()
{
    FWObject* root = getObject();
    FWObject* lib = member;

    root->remove(lib, false);

    QString filename = QString::fromUtf8(FWObjectDatabase::cast(root)->getFileName().c_str());

    QCoreApplication::postEvent(
        mw, new removeTreePageEvent(filename, lib->getId()));

    // switch to another library
    QCoreApplication::postEvent(
        mw, new showObjectInTreeEvent(filename, 
                                      FWObjectDatabase::STANDARD_LIB_ID));
}

void FWCmdAddLibrary::redo()
{
    prepareStatesForRedo();

    FWObject* root = getObject();
    FWObject* lib = member;

    getObject()->add(lib);

    QString filename = QString::fromUtf8(FWObjectDatabase::cast(root)->getFileName().c_str());

    QCoreApplication::postEvent(
        mw, new addTreePageEvent(filename, lib->getId()));

    QCoreApplication::postEvent(
        mw, new updateObjectAndSubtreeImmediatelyEvent(filename, lib->getId()));

    QCoreApplication::postEvent(mw, new dataModifiedEvent(filename, lib->getId()));

    // post openObjectInEditorEvent first so that editor panel opens
    // this matters if the tree needs to scroll to show the object when
    // showObjectInTreeEvent is posted because vertical size of the tree
    // changes when editor opens

    //if (mw->isEditorVisible())
    QCoreApplication::postEvent(
        mw, new showObjectInTreeEvent(filename, lib->getId()));
    QCoreApplication::postEvent(
        mw, new openObjectInEditorEvent(filename, lib->getId()));
}

