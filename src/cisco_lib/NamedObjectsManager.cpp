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


#include "NamedObjectsManager.h"
#include "NamedObject.h"

#include "PIXObjectGroup.h"
#include "ASA8ObjectGroup.h"
#include "IOSObjectGroup.h"
#include "NXOSObjectGroup.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"

#include "fwcompiler/Compiler.h"

#include <iostream>
#include <algorithm>

#include <assert.h>

#include <QString>
#include <QStringList>
#include <QtDebug>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

FWObject* create_NXOSObjectGroup(int id)
{
    FWObject *nobj = new NXOSObjectGroup();
    if (id > -1) nobj->setId(id);
    return nobj;
}

FWObject* create_IOSObjectGroup(int id)
{
    FWObject *nobj = new IOSObjectGroup();
    if (id > -1) nobj->setId(id);
    return nobj;
}

FWObject* create_PIXObjectGroup(int id)
{
    FWObject *nobj = new PIXObjectGroup();
    if (id > -1) nobj->setId(id);
    return nobj;
}

FWObject* create_ASA8ObjectGroup(int id)
{
    FWObject *nobj = new ASA8ObjectGroup();
    if (id > -1) nobj->setId(id);
    return nobj;
}

NamedObjectsManager::NamedObjectsManager(Library *persistent_objects,
                                         Firewall *_fw)
{
    fw = _fw;
    version = fw->getStr("version");
    platform = fw->getStr("platform");

    this->persistent_objects = persistent_objects;

    Group *object_groups = new Group();
    object_groups->setName("Object Groups");
    persistent_objects->add( object_groups );
    object_groups_group_id = FWObjectDatabase::getStringId(object_groups->getId());

    BaseObjectGroup::name_disambiguation.clear();
    NamedObject::name_disambiguation.clear();

    FWObjectDatabase::registerObjectType(NXOSObjectGroup::TYPENAME,
                                         &create_NXOSObjectGroup);
    FWObjectDatabase::registerObjectType(IOSObjectGroup::TYPENAME,
                                         &create_IOSObjectGroup);
    FWObjectDatabase::registerObjectType(PIXObjectGroup::TYPENAME,
                                         &create_PIXObjectGroup);
    FWObjectDatabase::registerObjectType(ASA8ObjectGroup::TYPENAME,
                                         &create_ASA8ObjectGroup);
}

NamedObjectsManager::~NamedObjectsManager()
{
    std::map<int, NamedObject*>::iterator it1;
    for (it1=named_objects.begin(); it1!=named_objects.end(); ++it1)
    {
        delete it1->second;
    }
    named_objects.clear();
}

void NamedObjectsManager::addNamedObject(const FWObject *obj)
{
    if (getNamedObject(obj) == nullptr)
        named_objects[obj->getId()] = new NamedObject(obj, platform.c_str());
}

NamedObject* NamedObjectsManager::getNamedObject(const FWObject *obj)
{
    if (named_objects.count(obj->getId()) == 0) return nullptr;
    else   
        return named_objects[obj->getId()];
}

bool NamedObjectsManager::haveNamedObjects()
{
    return (named_objects.size() > 0);
}

bool NamedObjectsManager::haveObjectGroups()
{
    FWObject *object_groups = persistent_objects->getRoot()->findInIndex(
        FWObjectDatabase::getIntId(object_groups_group_id));
    return (object_groups->size() > 0);
}

string NamedObjectsManager::getNamedObjectsDefinitions()
{
    QStringList output;
    map<int, NamedObject*>::iterator it;

    for (it=named_objects.begin(); it!=named_objects.end(); ++it)
    {
        NamedObject *nobj = it->second;
        if (nobj==nullptr) continue;
        output << nobj->getCommand();
    }

    FWObject *object_groups = persistent_objects->getRoot()->findInIndex(
        FWObjectDatabase::getIntId(object_groups_group_id));

    for (FWObject::iterator i=object_groups->begin();
         i!=object_groups->end(); ++i)
    {
        BaseObjectGroup *og = dynamic_cast<BaseObjectGroup*>(*i);
        assert(og!=nullptr);
        if (og->size()==0) continue;
        output << og->toString(this); // ends with an empty line
    }

    return output.join("\n").toUtf8().constData();
}

string NamedObjectsManager::getClearCommands()
{
    return "";
}

BaseObjectGroup* NamedObjectsManager::createObjectGroup()
{
    BaseObjectGroup *grp = nullptr;
    if (platform == "pix")
    {
        if (XMLTools::version_compare(version, "8.0")<0)
            grp = new PIXObjectGroup();
        else
            grp = new ASA8ObjectGroup();
    }

    if (platform == "fwsm") grp = new PIXObjectGroup();

    if (platform == "iosacl") grp = new IOSObjectGroup();

    assert(grp!=nullptr);
    
    return grp;
}

Group* NamedObjectsManager::getObjectGroupsGroup()
{
    return Group::cast(persistent_objects->getRoot()->findInIndex(
                           FWObjectDatabase::getIntId(object_groups_group_id)));
}

