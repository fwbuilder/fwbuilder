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
#include "FWCmdMoveObject.h"
#include "FWWindow.h"
#include "FWBSettings.h"
#include "events.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Library.h"

#include <QObject>
#include <QtDebug>


using namespace libfwbuilder;
using namespace std;

/********************************************************
 * FWCmdMoveObject
 *
 * This command controls moving an object to another place in the
 * tree, including different library. List of pointers to FWObject
 * passed to constructor is used to remove all references to this
 * object. This is done when object is deleted by moving it to the
 * "Deleted objects" library. If the list is empty, no references
 * will be removed.
 *
 * Unlike in FWComdAddObject, the object @obj always belongs to the
 * tree, either in its old place or in the new place. There is no need
 * for us to delete it in the destructor.
 *
 * This command should only operate on moving objects from one place
 * in the tree to another but within the same data file
 *
 ********************************************************/

FWCmdMoveObject::FWCmdMoveObject(
    ProjectPanel *project,
    FWObject *old_p,
    FWObject *new_p,
    FWObject *o,
    map<int, set<FWObject*> > &reference_holder_objects,
    QString text, QUndoCommand* macro)
     : FWCmdBasic(project, macro),
       oldUserFolder(QString::fromUtf8(o->getStr("folder").c_str())),
       reference_holders(reference_holder_objects)
{
    old_parent = old_p;
    new_parent = new_p;
    current_parent = NULL;
    obj = o;

    if (text.isEmpty())
    {
        setText(QObject::tr("Move object"));
    } else
    {
        setText(text);
    }
}

FWCmdMoveObject::~FWCmdMoveObject()
{
}

void FWCmdMoveObject::undo()
{
    obj->setStr("folder", oldUserFolder.toUtf8().constData());
    if (new_parent->hasChild(obj) && !old_parent->hasChild(obj))
    {
        new_parent->remove(obj, false);
        old_parent->add(obj);
        current_parent = old_parent;
    }
    if (fwbdebug) qDebug() << "FWCmdMoveObject::undo() obj->ref_counter=" 
                           << obj->getRefCounter();

    if (reference_holders.size())
    {
        map<int, set<FWObject*> >::iterator it;
        for (it=reference_holders.begin(); it!=reference_holders.end(); ++it)
        {
            int obj_id = it->first;
            foreach(FWObject *o, it->second)
            {
                FWObject *cobj = project->db()->findInIndex(obj_id);
                if (cobj) o->addRef(cobj);
            }
        }
    }
    notify();
}

void FWCmdMoveObject::redo()
{
    obj->setStr("folder", "");
    if (!new_parent->hasChild(obj) && old_parent->hasChild(obj))
    {
        old_parent->remove(obj, false);
        new_parent->add(obj);
        current_parent = new_parent;
    }
    if (fwbdebug) qDebug() << "FWCmdMoveObject::redo()"
                           << "obj: " << obj->getName().c_str()
                           << "(" << obj->getTypeName().c_str() << ")"
                           << "obj->ref_counter=" 
                           << obj->getRefCounter();
    if (reference_holders.size())
    {
        map<int, set<FWObject*> >::iterator it;
        for (it=reference_holders.begin(); it!=reference_holders.end(); ++it)
        {
            int obj_id = it->first;
            foreach(FWObject *o, it->second)
            {
                FWObject *cobj = project->db()->findInIndex(obj_id);
                if (cobj) o->removeRef(cobj);
            }
        }
    }
    notify();
}

void FWCmdMoveObject::notify()
{
    // This command should only operate on moving objects from one
    // place in the tree to another but within the same data file
    QString filename = project->getFileName();
    // QCoreApplication::postEvent(
    //     mw, new reloadObjectTreeImmediatelyEvent(filename));

    if (Library::isA(obj) && FWObjectDatabase::isA(obj->getParent()))
    {
        // library was undeleted abd moved from Deleted obj library to root

        QCoreApplication::postEvent(
            mw, new removeObjectFromTreeEvent(filename, obj->getId()));

        QCoreApplication::postEvent(
            mw, new addTreePageEvent(filename, obj->getId()));

        QCoreApplication::postEvent(
            mw, new updateObjectAndSubtreeImmediatelyEvent(filename, obj->getId()));

    } else
    {
        QCoreApplication::postEvent(
            mw, new removeObjectFromTreeEvent(filename, obj->getId()));
        QCoreApplication::postEvent(
            mw, new insertObjectInTreeEvent(filename, current_parent->getId(),
                                            obj->getId()));
    }

    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(filename, old_parent->getId()));
    QCoreApplication::postEvent(
        mw, new dataModifiedEvent(filename, new_parent->getId()));

    FWObject *new_obj = NULL;
    if (current_parent->getId()==FWObjectDatabase::DELETED_OBJECTS_ID)
    {
        if (Library::isA(obj))
        {
            // See #1740
            // looks like the object that moved into Deleted Objects is
            // another library. Show Deleted Objects library if it is enabled.
            // 
            if (fwbdebug)
                qDebug() << "Moved library to Deleted objects"
                         << "old_parent=" << old_parent;
            
            if (st->getBool("UI/ShowDeletedObjects"))
                new_obj = current_parent; // should be "deleted objects" lib
            else
            {
//                new_obj = old_parent;  // this does not work!
                new_obj = project->m_panel->om->getNextUserLib(obj);
                if (new_obj == NULL)
                {
                    // no user libraries left, show "Standard"
                    new_obj = old_parent->getRoot()->findInIndex(
                        FWObjectDatabase::getIntId("syslib000"));
                }
                if (new_obj == NULL)
                    new_obj = old_parent->getRoot()->front();
                if (fwbdebug) qDebug() << "FWCmdMoveObject::notify() new_obj="
                                       << new_obj;
            }
        } else
            new_obj = old_parent;
    } else
    {
        new_obj = obj;
    }

    QCoreApplication::postEvent(mw, new openLibraryForObjectEvent(
                                    filename, new_obj->getId()));

    // post openObjectInEditorEvent first so that editor panel opens.
    // This matters if the tree needs to scroll to show the object when
    // showObjectInTreeEvent is posted because vertical size of the tree
    // changes when editor opens
    if (mw->isEditorVisible())
        QCoreApplication::postEvent(mw, new openObjectInEditorEvent(
                                        filename, new_obj->getId()));

    QCoreApplication::postEvent(mw, new showObjectInTreeEvent(
                                    filename, new_obj->getId()));
}

/****************************************************************/

FWCmdMoveToFromUserFolder::FWCmdMoveToFromUserFolder(ProjectPanel *project,
                                                     FWObject *parent,
                                                     FWObject *obj,
                                                     const QString &oldFolder,
                                                     const QString &newFolder,
                                                     QString text,
                                                     QUndoCommand *macro)
    : FWCmdChange(project, obj, text, false, macro),
      m_parentId(parent->getId()),
      m_oldFolder(oldFolder),
      m_newFolder(newFolder)
{
}


void FWCmdMoveToFromUserFolder::redo()
{
    FWCmdChange::redo();

    FWObject *obj = getObject();
    QString fileName = QString::fromUtf8(obj->getRoot()->getFileName().c_str());

    QCoreApplication::postEvent(mw, new moveToFromUserFolderEvent
                                (fileName, m_parentId, obj->getId(),
                                 m_oldFolder, m_newFolder));
}


void FWCmdMoveToFromUserFolder::undo()
{
    FWCmdChange::undo();

    FWObject *obj = getObject();
    QString fileName = QString::fromUtf8(obj->getRoot()->getFileName().c_str());

    QCoreApplication::postEvent(mw, new moveToFromUserFolderEvent
                                (fileName, m_parentId, obj->getId(),
                                 m_newFolder, m_oldFolder));
}
