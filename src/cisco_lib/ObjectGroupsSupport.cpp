/* 

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#include "config.h"

#include "ObjectGroupsSupport.h"
#include "ObjectGroupFactory.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Firewall.h"

#include "fwcompiler/Compiler.h"

#include <iostream>
#include <algorithm>

#include <assert.h>

#include <QString>
#include <QStringList>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


Group *CreateObjectGroups::object_groups = NULL;


void CreateObjectGroups::init(FWObjectDatabase *db)
{
    object_groups = new Group();
    db->add( object_groups );
}

BaseObjectGroup* CreateObjectGroups::findObjectGroup(RuleElement *re)
{
    list<FWObject*> relement;

    for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = FWReference::getObject(o);
        relement.push_back(obj);
    }


    for (FWObject::iterator i=object_groups->begin(); i!=object_groups->end(); ++i)
    {
        BaseObjectGroup *og=dynamic_cast<BaseObjectGroup*>(*i);
        assert(og!=NULL);

        if (og->size()==0 || (og->size()!=re->size()) ) continue;

        bool match=true;
        for (FWObject::iterator i1=og->begin(); i1!=og->end(); ++i1) 
        {
            FWObject *o   = *i1;
            FWObject *obj = o;
            if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();

            if ( find(relement.begin(), relement.end(), obj)==relement.end() )
            {
                match=false;
                break;
            }
        }
        if (match) return og;
    }
    return NULL;
}

bool CreateObjectGroups::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==NULL) return false;

    Interface *rule_iface = Interface::cast(compiler->dbcopy->findInIndex(
                                                rule->getInterfaceId()));
    assert(rule_iface);

    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));
    if (re->size()==1)  // no need to create object-group since there is single object in the rule element
    {
        tmp_queue.push_back(rule);
        return true;
    }

    string version = compiler->fw->getStr("version");
    string platform = compiler->fw->getStr("platform");
    bool supports_mixed_groups =
        Resources::platform_res[platform]->getResourceBool(
            string("/FWBuilderResources/Target/options/") +
            "version_" + version + "/supports_mixed_service_groups");

    BaseObjectGroup *obj_group = findObjectGroup(re);
    if (obj_group==NULL)
    {
        //obj_group= new BaseObjectGroup();
        obj_group = ObjectGroupFactory::createObjectGroup(compiler->fw);

        FWObject *o = re->front();
        FWObject *obj = FWReference::getObject(o);
        obj_group->setObjectGroupTypeFromFWObject(obj);

        if (obj_group->isServiceGroup() && supports_mixed_groups)
        {
            obj_group->setObjectGroupType(BaseObjectGroup::MIXED_SERVICE);
        }

        QStringList gn;
        if (!rule_iface->getLabel().empty())
            gn.push_back(rule_iface->getLabel().c_str());

        gn.push_back(rule->getUniqueId().c_str());
        gn.push_back(name_suffix.c_str());
        obj_group->setName(gn.join(".").toStdString());

        object_groups->add(obj_group);

        if (libfwbuilder::XMLTools::version_compare(
                compiler->fw->getStr("version"), "8.3")>=0 &&
            re->getTypeName() == RuleElementTSrc::TYPENAME)
        {
            // put all objects inside of the group, except for the interface
            // if it belongs to the firewall
            FWObject *re_interface = NULL;
            for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
            {
                FWObject *o = *i1;
                FWObject *obj = o;
                if (FWReference::cast(o)!=NULL)
                    obj = FWReference::cast(o)->getPointer();
                if (Interface::isA(obj) && obj->isChildOf(compiler->fw))
                {
                    re_interface = obj;
                    continue;
                }
                obj_group->addRef(obj);
            }
            re->clearChildren(false); //do not want to destroy children objects
            if (re_interface)
            {
                // add interface back.
                re->addRef(re_interface);
            }
        } else
        {
            for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
            {
                FWObject *o = *i1;
                FWObject *obj = o;
                if (FWReference::cast(o)!=NULL)
                    obj = FWReference::cast(o)->getPointer();
                obj_group->addRef(obj);
            }
            re->clearChildren(false); //do not want to destroy children objects
        }
    }

    re->addRef(obj_group);

    assert(re->size()==1);

    tmp_queue.push_back(rule);
    return true;
}

bool printObjectGroups::processNext()
{
    slurp();
    if (tmp_queue.size()==0) return false;

    for (FWObject::iterator i=CreateObjectGroups::object_groups->begin();
         i!=CreateObjectGroups::object_groups->end(); ++i)
    {
        BaseObjectGroup *og = dynamic_cast<BaseObjectGroup*>(*i);
        assert(og!=NULL);
        if (og->size()==0) continue;
        compiler->output << endl;
        try
        {
            compiler->output << og->toString();
        } catch (FWException &ex)
        {
            compiler->abort(ex.toString());
        }
    }

    return true;
}

