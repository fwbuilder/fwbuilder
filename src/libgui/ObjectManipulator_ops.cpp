/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

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
#include "platforms.h"
#include "events.h"

#include "ObjectManipulator.h"
#include "ObjectEditor.h"
#include "ObjectTreeViewItem.h"
#include "ObjectTreeView.h"
#include "newGroupDialog.h"
#include "FWObjectClipboard.h"
#include "FindObjectWidget.h"
#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"
#include "FWCmdChange.h"
#include "FWCmdAddObject.h"
#include "FWCmdDeleteObject.h"
#include "FWCmdMoveObject.h"
#include "FWBTree.h"
#include "FWWindow.h"
#include "KeywordsDialog.h"
#include "ProjectPanel.h"
#include "ConfirmDeleteObjectDialog.h"

#include "fwbuilder/AttachedNetworks.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/StateSyncClusterGroup.h"

#include <memory>

#include <memory>

#include <QInputDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QTime>
#include <QtDebug>

#include <memory>
#include <algorithm>


using namespace std;
using namespace libfwbuilder;


/*
 * this method initiates automatic renaming of children objects if necessary
 */
void ObjectManipulator::autoRenameChildren(FWObject *obj,
                                           const QString &oldName)
{
    if (fwbdebug)
        qDebug() << "ObjectManipulator::autoRenameChildren  changing name "
                 << oldName.toLatin1()
                 << "->"
                 << QString::fromUtf8(obj->getName().c_str());
    
    if (oldName == QString::fromUtf8(obj->getName().c_str())) return;
    
#ifndef NDEBUG
    QTreeWidgetItem *itm = allItems[obj];
    assert(itm!=nullptr);
#endif

    if ((QString::fromUtf8(obj->getName().c_str())!=oldName) &&
        (Host::isA(obj) || Firewall::cast(obj)!=nullptr || Interface::isA(obj)))
    {
        autorename(obj);
    }
}

void ObjectManipulator::autorename(FWObject *obj)
{
    if (fwbdebug)
        qDebug() << "ObjectManipulator::autorename  obj=" << obj->getName().c_str();

    if (Host::isA(obj) || Firewall::cast(obj)!=nullptr || Cluster::isA(obj))
    {
        list<FWObject*> il = obj->getByType(Interface::TYPENAME);
        for (list<FWObject*>::iterator i=il.begin(); i!=il.end(); ++i)
            autorename(*i);
        list<FWObject*> obj_list = obj->getByType(StateSyncClusterGroup::TYPENAME);
        for (list<FWObject*>::iterator i=il.begin(); i!=il.end(); ++i)
            autorename(obj_list, StateSyncClusterGroup::TYPENAME, "members");
    }
 
    if (Interface::isA(obj))
    {
        list<FWObject*> subinterfaces = obj->getByType(Interface::TYPENAME);
        list<FWObject*> ipv4_list = obj->getByType(IPv4::TYPENAME);
        list<FWObject*> ipv6_list = obj->getByType(IPv6::TYPENAME);
        list<FWObject*> pa_list = obj->getByType(physAddress::TYPENAME);
        list<FWObject*> failover_list = obj->getByType(FailoverClusterGroup::TYPENAME);
        list<FWObject*> attached_list = obj->getByType(AttachedNetworks::TYPENAME);

        if (ipv4_list.size() || ipv6_list.size() || pa_list.size() ||
            failover_list.size() || subinterfaces.size() || attached_list.size())
        {
            list<FWObject*> vlans;
            for (list<FWObject*>::iterator j=subinterfaces.begin();
                 j!=subinterfaces.end(); ++j)
            {
                Interface *intf = Interface::cast(*j);
                if (intf->getOptionsObject()->getStr("type") == "8021q")
                    vlans.push_back(intf);
            }

            if (vlans.size()) autorenameVlans(vlans);

            for (list<FWObject*>::iterator j=subinterfaces.begin();
                 j!=subinterfaces.end(); ++j)
                autorename(*j);

            autorename(ipv4_list, IPv4::TYPENAME, "ip");
            autorename(ipv6_list, IPv6::TYPENAME, "ip6");
            autorename(pa_list, physAddress::TYPENAME, "mac");
            autorename(failover_list, FailoverClusterGroup::TYPENAME, "members");
            autorename(attached_list, AttachedNetworks::TYPENAME, "attached");
        }
    }
}

void ObjectManipulator::autorename(list<FWObject*> &obj_list,
                                   const string &objtype,
                                   const string &namesuffix)
{
    for (list<FWObject*>::iterator j=obj_list.begin(); j!=obj_list.end(); ++j)
    {
        FWObject *obj = *j;
        QString old_name = obj->getName().c_str();
        FWObject *parent = obj->getParent();
        QString name = getStandardName(parent, objtype, namesuffix);
        name = makeNameUnique(parent, name, objtype.c_str());
        if (name != old_name)
        {
            FWCmdChange* cmd = new FWCmdChangeName(m_project, obj);
            FWObject* new_state = cmd->getNewState();
            new_state->setName(string(name.toUtf8()));
            m_project->undoStack->push(cmd);
        }
    }
}

void ObjectManipulator::autorenameVlans(list<FWObject*> &obj_list)
{
    for (list<FWObject*>::iterator j=obj_list.begin(); j!=obj_list.end(); ++j)
    {
        FWObject *obj = *j;
        FWObject *parent = obj->getParent();
        FWObject *fw = parent;
        while (fw && Firewall::cast(fw)==nullptr) fw = fw->getParent();
        assert(fw);
        QString obj_name = QString::fromUtf8(obj->getName().c_str());

        std::unique_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(fw));

        if (int_prop->looksLikeVlanInterface(obj_name))
        {
            // even though we only call this function if the type of
            // this interface is 8021q, need to check its naming
            // schema as well. We can't automatically rename
            // interfaces that do not follow known naming convention.

            QString base_name;
            int vlan_id;
            int_prop->parseVlan(obj_name, &base_name, &vlan_id);
            if (base_name != "vlan")
            {
                QString new_name = QString("%1.%2")
                    .arg(QString::fromUtf8(
                             parent->getName().c_str()))
                    .arg(vlan_id);
                if (new_name != QString::fromUtf8(obj->getName().c_str()))
                {
                    FWCmdChange* cmd = new FWCmdChangeName(m_project, obj);
                    FWObject* new_state = cmd->getNewState();
                    new_state->setName(string(new_name.toUtf8()));
                    m_project->undoStack->push(cmd);
                }
            }
        }
    }
}

FWObject* ObjectManipulator::duplicateObject(FWObject *targetLib, FWObject *obj)
{
    if (!isTreeReadWrite(this, targetLib)) return nullptr;

    // we disable copy/cut/paste/duplicate menu items for objects that
    // can't be copied or duplicated in
    // ObjectManipulator::getMenuState() but will check here just in
    // case
    if (AttachedNetworks::isA(obj)) return nullptr;

    openLib(targetLib);
    FWObject *new_parent = FWBTree().getStandardSlotForObject(
        targetLib, obj->getTypeName().c_str());
    if (new_parent == nullptr) new_parent = obj->getParent();
    QString newName =
        makeNameUnique(new_parent,
                       QString::fromUtf8(obj->getName().c_str()),
                       obj->getTypeName().c_str());

    if (!isObjectAllowed(new_parent, obj)) return nullptr;

    return createObject(obj->getTypeName().c_str(), newName, obj);
}

void ObjectManipulator::moveObject(FWObject *targetLib, FWObject *obj)
{
    FWObject *cl=getCurrentLib();
    if (cl==targetLib) return;

    FWObject *grp = nullptr;

    if (FWObjectDatabase::isA(targetLib)) grp = targetLib;
    else
    {
        grp = FWBTree().getStandardSlotForObject(
            targetLib, obj->getTypeName().c_str());
    }
    if (grp==nullptr) grp=targetLib;

    if (!grp->isReadOnly())
    {
        map<int, set<FWObject*> > reference_holders;
        FWCmdMoveObject *cmd = new FWCmdMoveObject(m_project,
                                                   obj->getParent(),
                                                   grp,
                                                   obj,
                                                   reference_holders,
                                                   "Move object");
        m_project->undoStack->push(cmd);
    }
    if (fwbdebug) qDebug("ObjectManipulator::moveObject  all done");
}

/*
 *  targetLibName is the name of the target library in Unicode
 */
void ObjectManipulator::moveObject(const QString &targetLibName,
                                   FWObject *obj)
{
    list<FWObject*> ll = m_project->db()->getByType( Library::TYPENAME );
    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        FWObject *lib=*i;
        if (targetLibName==QString::fromUtf8(lib->getName().c_str()))
        {
            if (fwbdebug)
                qDebug("ObjectManipulator::moveObject  found lib %s",
                       lib->getName().c_str() );

            moveObject(lib,obj);
        }
    }
}

FWObject*  ObjectManipulator::pasteTo(FWObject *target, FWObject *obj)
{
    map<int,int> map_ids;
    return actuallyPasteTo(target, obj, map_ids);
}

FWObject* ObjectManipulator::actuallyPasteTo(FWObject *target,
                                              FWObject *obj,
                                              std::map<int,int> &map_ids)
{
    //FWObject *res = nullptr;

    FWObject *ta = prepareForInsertion(target, obj);
    if (ta == nullptr) return nullptr;

    if (!isObjectAllowed(ta, obj)) return nullptr;

    // we disable copy/cut/paste/duplicate menu items for objects that
    // can't be copied or duplicated in
    // ObjectManipulator::getMenuState() but will check here just in
    // case
    if (AttachedNetworks::isA(obj)) return nullptr;

    if (fwbdebug)
        qDebug() << "ObjectManipulator::actuallyPasteTo"
                 << "target=" << target->getPath().c_str()
                 << "ta=" << ta->getPath().c_str();

    QString new_name = makeNameUnique(
        ta, obj->getName().c_str(), obj->getTypeName().c_str());

    try
    {
/* clipboard holds a copy of the object */
        if (obj->getRoot() != ta->getRoot())
        {
            if (fwbdebug) qDebug("Copy object %s (%d) to a different object tree",
                                 obj->getName().c_str(), obj->getId());
            FWCmdAddObject *cmd = new FWCmdAddObject(m_project, target, nullptr,
                                                     QObject::tr("Paste object"));
            FWObject *new_state = cmd->getNewState();
            cmd->setNeedTreeReload(true);
            // recursivelyCopySubtree() needs access to the target tree root
            // when it copies subtree, so have to copy into the actual target
            // tree.
            FWObject *nobj = m_project->db()->recursivelyCopySubtree(target, obj, map_ids);
            if (new_name != nobj->getName().c_str())
                nobj->setName(string(new_name.toUtf8()));
            target->remove(nobj, false);
            new_state->add(nobj);
            m_project->undoStack->push(cmd);
            return nobj;
        }

        Group *grp = Group::cast(ta);
        if (grp!=nullptr && !FWBTree().isSystem(ta))
        {
            if (fwbdebug) qDebug("Copy object %s (%d) to a regular group",
                                 obj->getName().c_str(), obj->getId());
/* check for duplicates. We just won't add an object if it is already there */
            int cp_id = obj->getId();
            list<FWObject*>::iterator j;
            for (j=grp->begin(); j!=grp->end(); ++j)
            {
                FWObject *o1=*j;
                if(cp_id==o1->getId()) return o1;

                FWReference *ref;
                if( (ref=FWReference::cast(o1))!=nullptr &&
                    cp_id==ref->getPointerId()) return o1;
            }
            FWCmdChange *cmd = new FWCmdChange(
                m_project, grp,
                QObject::tr("Paste object"));
            //cmd->setNeedTreeReload(false);
            FWObject *new_state = cmd->getNewState();
            new_state->addRef(obj);
            m_project->undoStack->push(cmd);
            return obj;

        } else
        {
/* add a copy of the object to system group , or
 * add ruleset object to a firewall.
 */
            if (fwbdebug)
                qDebug("Copy object %s (%d) to a system group, "
                       "a ruleset to a firewall or an address to an interface",
                       obj->getName().c_str(), obj->getId());

            FWObject *nobj = m_project->db()->create(obj->getTypeName());
            assert(nobj!=nullptr);
            //nobj->ref();
            nobj->duplicate(obj, true);
            if (new_name != nobj->getName().c_str())
                nobj->setName(string(new_name.toUtf8()));

            // If we paste interface, reset the type of the copy
            // See #299
            if (Interface::isA(obj) && Interface::isA(ta))
            {
                Interface *new_intf = Interface::cast(nobj);
                new_intf->getOptionsObject()->setStr("type", "ethernet");
                // see #391 : need to reset "mamagement" flag in the copy
                // to make sure we do not end up with two management interfaces
                new_intf->setManagement(false);
            }

            FWCmdChange *cmd = new FWCmdAddObject(m_project, ta, nobj,
                                                  QObject::tr("Paste object"));
            FWObject *new_state = cmd->getNewState();

            // adding object to new_state is reduntant but
            // FWCmdAddObject supports this for consistency
            new_state->add(nobj);
            m_project->undoStack->push(cmd);

            return nobj;
        }
    }
    catch(FWException &ex)
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            ex.toString().c_str(),
            "&Continue", QString::null,QString::null,
            0, 1 );
    }

    return nullptr;
}

void ObjectManipulator::lockObject()
{
    if (fwbdebug)
        qDebug() << "ObjectManipulator::lockObject selected:"
                 << getCurrentObjectTree()->getNumSelected();

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    try
    {
        FWObject *obj;

        vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
        for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
        {
            obj= *i;
            FWObject *lib = obj->getLibrary();
            // these lbraries are locked anyway, do not let the user
            // lock objects inside because they won't be able to unlock them.
            if (lib->getId()!=FWObjectDatabase::STANDARD_LIB_ID)
            {
                std::unique_ptr<FWCmdLockObject> cmd(
                    new FWCmdLockObject(m_project, obj, tr("Lock object ") +
                                        QString::fromUtf8(obj->getName().c_str())));
                FWObject* new_state = cmd->getNewState();
                new_state->setReadOnly(true);
                if (!cmd->getOldState()->cmp(new_state, true))
                    m_project->undoStack->push(cmd.release());
            }
        }
        // Arguably, locking an object should not change lastModified timestamp
        // because none of the attributes that affect generated policy change.
        //QCoreApplication::postEvent(
        //    mw, new dataModifiedEvent(m_project->getFileName(), 0));
    } catch (FWException &ex)
    {
        qDebug() << ex.toString().c_str();
    }
}

void ObjectManipulator::unlockObject()
{
    if (fwbdebug)
        qDebug() << "ObjectManipulator::unlockObject selected:"
                 << getCurrentObjectTree()->getNumSelected();

    if (getCurrentObjectTree()->getNumSelected()==0) return;

    try
    {
        FWObject *obj;

        vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
        for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
        {
            obj= *i;
            FWObject *lib = obj->getLibrary();
            if (lib->getId()!=FWObjectDatabase::STANDARD_LIB_ID)
            {
                std::unique_ptr<FWCmdLockObject> cmd(
                    new FWCmdLockObject(m_project, obj, tr("Unlock object ") +
                                        QString::fromUtf8(obj->getName().c_str())));
                FWObject* new_state = cmd->getNewState();
                new_state->setReadOnly(false);
                if (!cmd->getOldState()->cmp(new_state, true))
                    m_project->undoStack->push(cmd.release());
            }
        }
    } catch (FWException &ex)
    {
        qDebug() << ex.toString().c_str();
    }
}

void ObjectManipulator::deleteObject(FWObject *obj, QUndoCommand* macro)
{
    bool firstAction = true ;
    Q_UNUSED(firstAction);

    if (fwbdebug)
        qDebug() << "ObjectManipulator::deleteObject"
                 << "obj=" << obj
                 << "name=" << obj->getName().c_str();

    FWObject *object_library = obj->getLibrary();
    FWObject *parent = obj->getParent();
    Q_UNUSED(parent);
    FWObject *deleted_objects_lib = m_project->db()->findInIndex(
        FWObjectDatabase::DELETED_OBJECTS_ID );

    if (deleted_objects_lib == nullptr)
    {
        FWObject *dobj = m_project->db()->createLibrary();
        dobj->setId(FWObjectDatabase::DELETED_OBJECTS_ID);
        dobj->setName("Deleted Objects");
        dobj->setReadOnly(false);
        m_project->db()->add(dobj);
        deleted_objects_lib = dobj;
    }

    if (object_library->getId() == FWObjectDatabase::STANDARD_LIB_ID)
        return;

    if (obj->isReadOnly()) return;

    if (obj->getId() == FWObjectDatabase::STANDARD_LIB_ID ||
        obj->getId() == FWObjectDatabase::DELETED_OBJECTS_ID) return;
    
    bool is_library = Library::isA(obj);
    bool is_firewall = Firewall::cast(obj) != nullptr; // includes Cluster too
    bool is_deleted_object = (deleted_objects_lib!=nullptr && obj->isChildOf(deleted_objects_lib));

    // ruleset_visible == true if 1) we delete firewall object and one of its
    // rulesets is visible in the project panel, or 2) we delete ruleset object
    // which is visible in the project panel
    bool ruleset_visible = (
        (is_firewall && m_project->getCurrentRuleSet()->isChildOf(obj)) ||
        (m_project->getCurrentRuleSet() == obj));

    mw->findObjectWidget->reset();

    QCoreApplication::postEvent(
        mw, new closeObjectEvent(m_project->getFileName(), obj->getId()));

#if 0
    // Remove object we are about to delete from the clipboard.
    // Sequence "delete then paste" is risky if the object is pasted into
    // a group or rule where only reference is added
    FWObjectClipboard::obj_clipboard->remove(obj);
#endif
 
    try
    {    
        if (fwbdebug)
            qDebug() << "ObjectManipulator::deleteObject"
                     << "is_library=" << is_library
                     << "is_firewall= " << is_firewall
                     << "ruleset_visible=" << ruleset_visible
                     << "is_deleted_object="<< is_deleted_object;

        if (is_deleted_object)
        {
            unselect();
            FWCmdDeleteObject *cmd = new FWCmdDeleteObject(
                m_project,
                obj,
                QString("Delete object"),
                macro);
            if (macro==nullptr)
                m_project->undoStack->push(cmd);
            return;
        }

        if (is_library && obj->isReadOnly()) obj->setReadOnly(false);

        if (is_library) parent = m_project->db()->getFirstByType(Library::TYPENAME);

        actuallyDeleteObject(obj, macro);
        
        if (ruleset_visible) m_project->closeRuleSetPanel();
    }
    catch (FWException &ex)
    {
        if (fwbdebug)
            qDebug() << "ObjectManipulator::deleteObject:"
                     << "catch:  restoreOverrideCursor";
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(
            this,"Firewall Builder",
            ex.toString().c_str(),
            "&Continue", QString::null,QString::null,
            0, 1 );
        throw(ex);
    }

    if (fwbdebug) qDebug("ObjectManipulator::deleteObject  done");

    firstAction = false ;
}

/* 
 * Here we build set of dependencies for @obj, create command to
 * delete it and push it to the undo stack.
 */
void ObjectManipulator::actuallyDeleteObject(FWObject *obj, QUndoCommand* macro)
{
    map<int, set<FWObject*> > reference_holders;
    UsageResolver().findAllReferenceHolders(obj, m_project->db(),
                                            reference_holders);

    FWObject *deleted_objects_lib = m_project->db()->findInIndex(
        FWObjectDatabase::DELETED_OBJECTS_ID);

    FWCmdMoveObject *cmd = new FWCmdMoveObject(
        m_project,
        obj->getParent(),
        deleted_objects_lib,
        obj,
        reference_holders,
        QString("Delete object"),
        macro);

    if (macro == nullptr)
        m_project->undoStack->push(cmd);
}

void ObjectManipulator::objectMoved(FWObject* obj)
{
    openLibForObject(obj);
}

void ObjectManipulator::groupObjects()
{
    if (getCurrentObjectTree()->getNumSelected()==0) return;

    FWObject *co = getCurrentObjectTree()->getSelectedObjects().front();

    newGroupDialog ngd(this, m_project->db());

    if (ngd.exec()==QDialog::Accepted)
    {
        QString objName = ngd.m_dialog->obj_name->text();
        QString libName = ngd.m_dialog->libs->currentText();

        QString type = ObjectGroup::TYPENAME;
        if (Service::cast(co)!=nullptr)  type=ServiceGroup::TYPENAME;
        if (Interval::cast(co)!=nullptr) type=IntervalGroup::TYPENAME;

        FWObject *parent = nullptr;
        FWObject *newgrp = nullptr;

        list<FWObject*> ll = m_project->db()->getByType( Library::TYPENAME );
        for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
        {
            FWObject *lib=*i;
            if (libName==QString::fromUtf8(lib->getName().c_str()))
            {
/* TODO: need to show a dialog and say that chosen library is
 * read-only.  this is not critical though since newGroupDialog fills
 * the pull-down only with names of read-write libraries
 */
                if (lib->isReadOnly()) return;
                parent = FWBTree().getStandardSlotForObject(lib,type);
                if (parent==nullptr)
                {
                    if (fwbdebug)
                        qDebug("ObjectManipulator::groupObjects(): could not find standard slot for object of type %s in library %s",
                               type.toLatin1().constData(),lib->getName().c_str());

                    return;
                }
                newgrp = m_project->db()->create(type.toStdString());
                newgrp->setName(string(objName.toUtf8().constData()));
                break;
            }
        }
        if (newgrp==nullptr) return;

        FWCmdAddObject *cmd = new FWCmdAddObject(
            m_project, parent, newgrp, QObject::tr("Create new group"));
        FWObject *new_state = cmd->getNewState();
        new_state->add(newgrp);

        vector<FWObject*> so = getCurrentObjectTree()->getSimplifiedSelection();
        for (vector<FWObject*>::iterator i=so.begin();  i!=so.end(); ++i)
            newgrp->addRef(*i);

        m_project->undoStack->push(cmd);
    }
}


static void doKeyword(vector<FWObject *> objs, bool doAdd,
                      const string &keyword, ProjectPanel *project)
{
    vector<FWObject *>::const_iterator iter;
    for (iter = objs.begin(); iter != objs.end(); ++iter) {
        FWObject *obj = *iter;

        FWCmdChange *cmd = new FWCmdChange(project, obj);
        FWObject *newObj = cmd->getNewState();
        if (doAdd) {
            newObj->addKeyword(keyword);
        } else {
            newObj->removeKeyword(keyword);
        }

        if (!obj->cmp(newObj)) {
            project->undoStack->push(cmd);
        } else {
            delete cmd;
        }
    }
}


void ObjectManipulator::addNewKeywordSlot()
{
    QString keyword = QInputDialog::getText(nullptr, tr("Add New Keyword"),
                          tr("Enter new keyword to add to selected objects"));
    keyword = keyword.simplified();
    if (fwbdebug) {
        qDebug() << "ObjectManipulator::addNewKeyword: " << keyword;
    }

    if (!KeywordsDialog::validateKeyword(nullptr, keyword)) return;

    doKeyword(getCurrentObjectTree()->getSelectedObjects(), true, 
              keyword.toUtf8().constData(), m_project);
}


void ObjectManipulator::processKeywordSlot()
{
    const QObject *qObj = sender();
    if (qObj == nullptr) return;
    const QAction *qAct = dynamic_cast<const QAction *>(qObj);
    QStringList list = qAct->data().toStringList();
    if (list.size() != 2) return;

    if (fwbdebug) {
        qDebug() << "ObjectManipulator::processKeyword:" << list;
    }

    doKeyword(getCurrentObjectTree()->getSelectedObjects(), (list[0] == "add"),
              list[1].toUtf8().constData(), m_project);
}


void ObjectManipulator::addSubfolderSlot()
{
    const QAction *qAct = dynamic_cast<const QAction *>(sender());
    if (qAct == nullptr) return;

    FWObject *obj = getCurrentObjectTree()->getCurrentObject();
    assert(obj->getId() == qAct->data().toInt());

    QString folder = QInputDialog::getText(nullptr, tr("Add Subfolder"),
                                           tr("Enter new subfolder name"));
    folder = folder.simplified();
    if (folder.isEmpty()) return;
    if (folder.contains(',')) {
        QMessageBox::warning(this, "Firewall Builder",
                             tr("Subfolder cannot contain a comma"), "&OK",
                             QString::null, QString::null, 0, 1);
        return;
    }

    /* See if the subfolder already exists */
    string folderStr = folder.toUtf8().constData();
    set<string> folders = stringToSet(obj->getStr("subfolders"));
    if (folders.find(folderStr) != folders.end()) return;
    folders.insert(folderStr);

    if (fwbdebug) {
        qDebug() << "ObjectManipulator::addSubfolder: " << folder;
    }

    FWCmdAddUserFolder *cmd = new FWCmdAddUserFolder(m_project, obj, folder,
                                                     tr("Add subfolder"));
    FWObject *newObj = cmd->getNewState();
    newObj->setStr("subfolders", setToString(folders));

    m_project->undoStack->push(cmd);
}


void ObjectManipulator::removeUserFolder()
{
    ObjectTreeViewItem *item = dynamic_cast<ObjectTreeViewItem *>
        (getCurrentObjectTree()->currentItem());
    if (item == nullptr || item->getUserFolderParent() == nullptr) return;
    ObjectTreeViewItem *parent = dynamic_cast<ObjectTreeViewItem *>
        (item->parent());
    assert(parent != nullptr);

    vector<FWObject *> objs;
    for (int ii = 0; ii < item->childCount(); ii++) {
        ObjectTreeViewItem *child = dynamic_cast<ObjectTreeViewItem *>
            (item->child(ii));

        FWObject *obj = child->getFWObject();
        if (obj->getRO()) {
            QMessageBox::critical(this, "Firewall Builder",
                                  tr("Folder with locked object "
                                     "cannot be deleted"));
            return;
        }

        objs.push_back(obj);
    }

    if (objs.size() > 0) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        ConfirmDeleteObjectDialog confirm(this);
        confirm.load(objs);
        QApplication::restoreOverrideCursor();
        if (confirm.exec() == QDialog::Rejected) return;
    }

    FWCmdMacro *macro = new FWCmdMacro(tr("Delete user folder"));

    QList<QTreeWidgetItem *> children = item->takeChildren();
    while (!children.isEmpty()) {
        ObjectTreeViewItem *child = dynamic_cast<ObjectTreeViewItem *>
            (children.takeFirst());
        assert(child != nullptr);

        FWObject *obj = child->getFWObject();
        if (mw->isEditorVisible() && mw->getOpenedEditor() == obj) {
            mw->hideEditor();
        }

        deleteObject(obj, macro);
    }

    FWCmdRemoveUserFolder *cmd =
        new FWCmdRemoveUserFolder(m_project, parent->getFWObject(),
                                  item->getUserFolderName(), "", macro);
    FWObject *newObj = cmd->getNewState();
    set<string> folders = stringToSet(newObj->getStr("subfolders"));
    folders.erase(item->getUserFolderName().toUtf8().constData());
    newObj->setStr("subfolders", setToString(folders));

    m_project->undoStack->push(macro);
}
