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
#include "FWCmdDeleteObject.h"
#include "FWWindow.h"
#include "events.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/RuleSet.h"

#include <QObject>
#include <QtDebug>


using namespace libfwbuilder;
using namespace std;

/********************************************************
 * FWCmdDeleteObject
 *
 * This command really deletes the object from the object tree, it
 * does not move it Deleted Objects library. We use this command when
 * user deletes objects from Deleted Objects.
 *
 ********************************************************/

FWCmdDeleteObject::FWCmdDeleteObject(ProjectPanel *project,
                                     FWObject *obj,
                                     QString text,
                                     QUndoCommand* macro):
    FWCmdChange(project, obj, text, false, macro)
{
    delobj = obj;
    delobj->ref();
    parent = delobj->getParent();
    
    if (text.isEmpty())
    {
        setText(QObject::tr("Delete object"));
    } else
    {
        setText(text);
    }
}

FWCmdDeleteObject::~FWCmdDeleteObject()
{
    if (delobj)
    {
        if (delobj->getRefCounter() <= 1) delete delobj;
        else delobj->unref();
    }
}

void FWCmdDeleteObject::undo()
{
    QString filename = project->getFileName();

    if (delobj && !parent->hasChild(delobj))
    {
        parent->add(delobj);
        if (fwbdebug)
            qDebug() << "FWCmdDeleteObject::undo()"
                     << "delobj->getRefCounter()=" << delobj->getRefCounter();

        QCoreApplication::postEvent(
            mw, new insertObjectInTreeEvent(filename, parent->getId(), delobj->getId()));

        QCoreApplication::postEvent(
            mw, new updateObjectAndSubtreeImmediatelyEvent(filename, parent->getId()));

        QCoreApplication::postEvent(
            mw, new dataModifiedEvent(filename, parent->getId()));

        if (mw->isEditorVisible())
            QCoreApplication::postEvent(
                mw, new openObjectInEditorEvent(filename, delobj->getId()));

        QCoreApplication::postEvent(
            mw, new showObjectInTreeEvent(filename, delobj->getId()));
    }
}

void FWCmdDeleteObject::redo()
{
    QString filename = project->getFileName();

    QCoreApplication::postEvent(
        mw, new removeObjectFromTreeEvent(filename, delobj->getId()));

    QCoreApplication::postEvent(
        mw, new updateObjectAndSubtreeImmediatelyEvent(filename, parent->getId()));

    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(filename, parent->getId()));

    if (mw->isEditorVisible())
        QCoreApplication::postEvent(
            mw, new openObjectInEditorEvent(filename, parent->getId()));

    QCoreApplication::postEvent(
        mw, new showObjectInTreeEvent(filename, parent->getId()));

    parent->remove(delobj, false);

    if (fwbdebug)
        qDebug() << "FWCmdDeleteObject::redo()"
                 << "delobj->getRefCounter()=" << delobj->getRefCounter();

}

void FWCmdDeleteObject::notify()
{
}


/********************************************************************/

FWCmdRemoveUserFolder::FWCmdRemoveUserFolder(ProjectPanel *project,
                                             FWObject *parentFolder,
                                             const QString &userFolder,
                                             QString text,
                                             QUndoCommand *macro)
    : FWCmdChange(project, parentFolder, text, false, macro),
      m_userFolder(userFolder)
{
}


void FWCmdRemoveUserFolder::redo()
{
    FWCmdChange::redo();

    FWObject *obj = getObject();
    QString fName = QString::fromUtf8(obj->getRoot()->getFileName().c_str());
    QCoreApplication::postEvent(mw,
                                new removeUserFolderEvent(fName, obj->getId(),
                                                          m_userFolder));
}


void FWCmdRemoveUserFolder::undo()
{
    FWCmdChange::undo();

    FWObject *obj = getObject();
    QString fName = QString::fromUtf8(obj->getRoot()->getFileName().c_str());
    QCoreApplication::postEvent(mw,
                                new addUserFolderEvent(fName, obj->getId(),
                                                       m_userFolder));
}


/* We don't want to display the system folder after we remove a user
   folder, so we just swallow the notify. */
void FWCmdRemoveUserFolder::notify()
{
}
