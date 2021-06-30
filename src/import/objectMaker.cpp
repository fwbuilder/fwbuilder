/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "objectMaker.h"
#include "objectSignature.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/MultiAddress.h"
#include "fwbuilder/Service.h"

// TODO: FWBTree needs to be refactored into an independent module
#include "../libgui/FWBTree.h"

#include <QStringList>
#include <QtDebug>

#include <set>


extern int fwbdebug;

using namespace libfwbuilder;
using namespace std;


void ObjectMakerErrorTracker::registerError(const QString &msg)
{
    if ( ! errors.contains(msg)) errors.append(msg);
    error_status = true;
}

void ObjectMakerErrorTracker::registerError(const string &msg)
{
    QString qs_msg = QString::fromUtf8(msg.c_str());
    if ( ! errors.contains(qs_msg)) errors.append(qs_msg);
    error_status = true;
}

void ObjectMakerErrorTracker::registerWarning(const QString &msg)
{
    if ( ! warnings.contains(msg)) warnings.append(msg);
    warning_status = true;
}

void ObjectMakerErrorTracker::registerWarning(const string &msg)
{
    QString qs_msg = QString::fromUtf8(msg.c_str());
    if ( ! warnings.contains(qs_msg)) warnings.append(qs_msg);
    warning_status = true;
}

//****************************************************************

void ObjectMaker::clear()
{
    last_created = nullptr;
    named_object_registry.clear(); 
    anon_object_registry.clear(); 
}

FWObject* ObjectMaker::findMatchingObject(const ObjectSignature &sig)
{
    QString sig_str = sig.toString();

    if ( ! sig.object_name.isEmpty())
    {
        if (named_object_registry.count(sig_str) > 0)
            return library->getRoot()->findInIndex(
                named_object_registry[sig_str]);
        return nullptr;
    }

    if (anon_object_registry.count(sig_str) > 0)
        return library->getRoot()->findInIndex(anon_object_registry[sig_str]);

    return nullptr;
}

void ObjectMaker::registerNamedObject(const ObjectSignature &sig,
                                      FWObject* obj)
{
    ObjectSignature anon_sig = sig;
    anon_sig.object_name = "";
    QString as = anon_sig.toString();
    if (anon_object_registry.count(as) > 0) anon_object_registry.remove(as);

    named_object_registry[sig.toString()] = (obj!=nullptr) ? obj->getId() : -1;
}

void ObjectMaker::registerAnonymousObject(const ObjectSignature &sig,
                                          FWObject* obj)
{
    ObjectSignature anon_sig = sig;
    anon_sig.object_name = "";
    anon_object_registry[anon_sig.toString()] = (obj!=nullptr) ? obj->getId() : -1;
}

/*
 * take anonymous object @obj and make named object from it. 
 *
 *  - assignin @objName
 *  - check if this object is in standard objects library and if it is, create
 *    a copy since we cant rename objects there
 *
 * Note that this means that returned pointer may point to a new object
 * rather than @obj
 */
FWObject* ObjectMaker::promoteToNamedObject(FWObject *obj,
                                            const std::string &objName)
{
    if (obj->getLibrary()->getId() == FWObjectDatabase::STANDARD_LIB_ID)
    {
        FWObject *new_obj = library->getRoot()->create(obj->getTypeName());
        new_obj->duplicate(obj);
        new_obj->setName(objName);
        ObjectSignature sig(error_tracker);
        new_obj->dispatch(&sig, (void*)nullptr);
        registerNamedObject(sig, new_obj);
        return new_obj;
    } else
    {
        obj->setName(objName);
        ObjectSignature sig(error_tracker);
        obj->dispatch(&sig, (void*)nullptr);
        registerNamedObject(sig, obj);
        return obj;
    }
}

//****************************************************************

FWObject* ObjectMaker::createObject(const std::string &objType,
                                    const std::string &objName)
{
    assert(library!=nullptr);
    FWBTree tree ;
    FWObject *slot = tree.getStandardSlotForObject(library,objType.c_str());
    return createObject(slot, objType, objName);
}

FWObject* ObjectMaker::createObject(FWObject *parent,
                                    const std::string &objType,
                                    const std::string &objName)
{
    assert(library!=nullptr);
    FWObject*  o = library->getRoot()->create(objType);
    if (parent != nullptr)
    {
        if (parent->isReadOnly())
        {     
            QString pn = QString::fromUtf8(parent->getName().c_str());
            if (parent->getLibrary()->isReadOnly())
//                throw ObjectMakerException(
                error_tracker->registerError(
                    QObject::tr("Can not add new objects to folder %1 because "
                                "it belongs to a locked library").arg(pn));
            else
//                throw ObjectMakerException(
                error_tracker->registerError(
                    QObject::tr("Can not add new objects to folder %1 because "
                                "it is locked").arg(pn));
        }
        if (parent->getLibrary()->getId() == FWObjectDatabase::DELETED_OBJECTS_ID)
        {
//            throw ObjectMakerException(
            error_tracker->registerError(
                QObject::tr("Can not add new objects to the \"Deleted Objects\" "
                            "library"));
        }
        parent->add(o);
    }
    o->setName(objName);
    return o;
}

FWObject* ObjectMaker::createObject(ObjectSignature &)
{
    return nullptr;
}

//****************************************************************

/*
 * scan the tree starting at @root and use registerObject to build
 * signatures for all address and service objects in order to be able
 * to use them on import
 */
void ObjectMaker::prepareForDeduplication(FWObject *root)
{
    switch(root->getId()) {
        // Skip dummy objects
    case FWObjectDatabase::DUMMY_ADDRESS_ID:
    case FWObjectDatabase::DUMMY_INTERFACE_ID:
    case FWObjectDatabase::DUMMY_SERVICE_ID:
        return;
    }

    if (RuleSet::cast(root) || Rule::cast(root) ||
        FWReference::cast(root) ||
        Host::cast(root) ||
        FWOptions::cast(root)) return;

    if (Address::cast(root) || MultiAddress::cast(root) || Service::cast(root))
    {
        ObjectSignature sig(error_tracker);

        root->dispatch(&sig, (void*)nullptr);

        registerNamedObject(sig, root);
        registerAnonymousObject(sig, root); // this erases sig.object_name
    }

    if (Library::isA(root) &&
        root->getId() == FWObjectDatabase::DELETED_OBJECTS_ID) return;

    for (FWObject::iterator it=root->begin(); it!=root->end(); ++it)
    {
        prepareForDeduplication(*it);
    }
}
