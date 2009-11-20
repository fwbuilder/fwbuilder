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
#include "events.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/RuleSet.h"

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

FWCmdMoveObject::FWCmdMoveObject(ProjectPanel *project,
                                 FWObject *old_p,
                                 FWObject *new_p,
                                 FWObject *o,
                                 list<FWObject*> &reference_holder_objects,
                                 QString text):
    FWCmdBasic(project),
    reference_holders(reference_holder_objects)
{
    old_parent = old_p;
    new_parent = new_p;
    obj = o;

    if (text.isEmpty())
    {
        setText(QObject::tr("Move %1 to %2")
                .arg(QString::fromUtf8(obj->getName().c_str()))
                .arg(QString::fromUtf8(new_parent->getName().c_str())));
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
    if (new_parent->hasChild(obj) && !old_parent->hasChild(obj))
    {
        new_parent->remove(obj, false);
        old_parent->add(obj);
    }
    if (fwbdebug) qDebug() << "FWCmdMoveObject::undo() obj->ref_counter=" 
                           << obj->getRefCounter();

    if (reference_holders.size())
    {
        foreach(FWObject *o, reference_holders)
        {
            o->addRef(obj);
        }
    }
    notify();
}

void FWCmdMoveObject::redo()
{
    if (!new_parent->hasChild(obj) && old_parent->hasChild(obj))
    {
        old_parent->remove(obj, false);
        new_parent->add(obj);
    }
    if (fwbdebug) qDebug() << "FWCmdMoveObject::redo() obj->ref_counter=" 
                           << obj->getRefCounter();
    if (reference_holders.size())
    {
        foreach(FWObject *o, reference_holders)
        {
            o->removeRef(obj);
        }
    }
    notify();
}

void FWCmdMoveObject::notify()
{
    // This command should only operate on moving objects from one
    // place in the tree to another but within the same data file
    QString filename = QString::fromUtf8(old_parent->getRoot()->getFileName().c_str());
    QCoreApplication::postEvent(mw, new reloadObjectTreeEvent(filename));
    QCoreApplication::postEvent(mw, new dataModifiedEvent(filename, old_parent->getId()));
    QCoreApplication::postEvent(mw, new dataModifiedEvent(filename, new_parent->getId()));
    // post openObjectInEditorEvent first so that editor panel opens.
    // This matters if the tree needs to scroll to show the object when
    // showObjectInTreeEvent is posted because vertical size of the tree
    // changes when editor opens
    QCoreApplication::postEvent(mw, new openObjectInEditorEvent(filename, obj->getId()));
    QCoreApplication::postEvent(mw, new showObjectInTreeEvent(filename, obj->getId()));

    if (Firewall::isA(obj) && project->getCurrentRuleSet()->isChildOf(obj))
    {
        // need to repopen ruleset view
        QCoreApplication::postEvent(mw, new updateObjectInRulesetEvent(filename, obj->getId()));
    }

}

