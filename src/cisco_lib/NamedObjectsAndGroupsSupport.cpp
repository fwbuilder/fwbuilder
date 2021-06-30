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


#include "NamedObjectsAndGroupsSupport.h"
#include "NamedObjectsManager.h"
#include "NamedObject.h"

#include "PIXObjectGroup.h"
#include "ASA8ObjectGroup.h"
#include "IOSObjectGroup.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Firewall.h"

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




CreateObjectGroups::~CreateObjectGroups()
{
}

BaseObjectGroup* CreateObjectGroups::findObjectGroup(RuleElement *re)
{
    list<FWObject*> relement;

    for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
        relement.push_back(FWReference::getObject(*i1));

    FWObject *object_groups = named_objects_manager->getObjectGroupsGroup();
    for (FWObject::iterator i=object_groups->begin(); i!=object_groups->end(); ++i)
    {
        BaseObjectGroup *og = dynamic_cast<BaseObjectGroup*>(*i);
        assert(og!=nullptr);

        if (og->size()==0 || (og->size()!=re->size()) ) continue;

        bool match = true;
        for (FWObject::iterator i1=og->begin(); i1!=og->end(); ++i1) 
        {
            if ( find(relement.begin(), relement.end(),
                      FWReference::getObject(*i1))==relement.end() )
            {
                match = false;
                break;
            }
        }
        if (match) return og;
    }
    return nullptr;
}

bool CreateObjectGroups::processNext()
{
    Rule *rule = prev_processor->getNextRule(); if (rule==nullptr) return false;
    string version = compiler->fw->getStr("version");
    string platform = compiler->fw->getStr("platform");

    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));

    if (re->size()==1)
    {
        /* create object group if the object in the RE is AddressRange
         * because IOS normally does not support ranges in ACLs, but
         * supports them in groups
         */
        FWObject *re_obj = FWReference::getObject(re->front());
        if ( ! AddressRange::isA(re_obj))
        {
            tmp_queue.push_back(rule);
            return true;
        }
    }

    BaseObjectGroup *obj_group = findObjectGroup(re);
    if (obj_group==nullptr)
    {
        obj_group = named_objects_manager->createObjectGroup();
        named_objects_manager->getObjectGroupsGroup()->add(obj_group);

        packObjects(re, obj_group);

        obj_group->setObjectGroupTypeFromMembers(named_objects_manager);

        QStringList group_name_prefix;
        group_name_prefix.push_back(rule->getUniqueId().c_str());
        group_name_prefix.push_back(name_suffix.c_str());

        QString reg_name = BaseObjectGroup::registerGroupName(
            group_name_prefix.join("."),
            obj_group->getObjectGroupType());

        obj_group->setName(reg_name.toUtf8().constData());
    } else
    {
        re->clearChildren(false); //do not want to destroy children objects
        re->addRef(obj_group);
    }

    tmp_queue.push_back(rule);
    return true;
}

void CreateObjectGroups::packObjects(RuleElement *re, BaseObjectGroup *obj_group)
{
    for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *obj = FWReference::getObject(*i1);
        obj_group->addRef(obj);
    }
    re->clearChildren(false); //do not want to destroy children objects
    re->addRef(obj_group);
}

void CreateObjectGroupsForTSrc::packObjects(RuleElement *re,
                                            BaseObjectGroup *obj_group)
{
    if (libfwbuilder::XMLTools::version_compare(
            compiler->fw->getStr("version"), "8.3")>=0)
    {
        // put all objects inside of the group, except for the interface
        // if it belongs to the firewall
        FWObject *re_interface = nullptr;
        for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
        {
            FWObject *o = *i1;
            FWObject *obj = o;
            if (FWReference::cast(o)!=nullptr)
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
        re->addRef(obj_group);
    } else
    {
        CreateObjectGroups::packObjects(re, obj_group);
    }
}

void createNamedObjectsCommon::printObjectsForRE(FWObject *re)
{
    if (RuleElement::cast(re)!=nullptr && RuleElement::cast(re)->isAny()) return;

    for (FWObject::iterator it=re->begin(); it!=re->end(); ++it)
    {
        FWObject *obj = FWReference::getObject(*it);
        if (Interface::isA(obj)) continue;
        if (BaseObjectGroup::cast(obj)!=nullptr) printObjectsForRE(obj);
        else named_objects_manager->addNamedObject(obj);
    }
}

/*
 * We only need named objects for address ranges in policy. At least
 * at this time, we have decided to not create named objects for
 * everything and use them only in cases where it is inevitable.
 */
void createNamedObjectsForPolicy::printObjectsForRE(FWObject *re)
{
    if (RuleElement::cast(re)!=nullptr && RuleElement::cast(re)->isAny()) return;

    for (FWObject::iterator it=re->begin(); it!=re->end(); ++it)
    {
        FWObject *obj = FWReference::getObject(*it);
        if (Interface::isA(obj)) continue;
        if (BaseObjectGroup::cast(obj)!=nullptr) printObjectsForRE(obj);
        if (AddressRange::isA(obj)) named_objects_manager->addNamedObject(obj);
    }
}

/*
 * We support named objects only for ASA 8.3 and in policy rules, only for
 * address ranges.
 *
 * See #1962, it looks like ASA 8.3 does not support named objects or
 * object-groups in place of port specification in access-list commands.
 */
bool createNamedObjectsForPolicy::processNext()
{
    slurp();
    if (tmp_queue.size()==0) return false;

    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        PolicyRule *policy_rule = PolicyRule::cast( *k );
        if (policy_rule)
        {
            RuleElementSrc *src_re = policy_rule->getSrc();  assert(src_re);
            printObjectsForRE(src_re);

            RuleElementDst *dst_re = policy_rule->getDst();  assert(dst_re);
            printObjectsForRE(dst_re);

            //RuleElementSrv *srv_re = policy_rule->getSrv();  assert(srv_re);
            //printObjectsForRE(srv_re);
        }
    }

    return true;
}


bool createNamedObjectsForNAT::processNext()
{
    slurp();
    if (tmp_queue.size()==0) return false;

    for (deque<Rule*>::iterator k=tmp_queue.begin(); k!=tmp_queue.end(); ++k) 
    {
        NATRule *nat_rule = NATRule::cast( *k );
        if (nat_rule)
        {
            RuleElementOSrc *osrc_re = nat_rule->getOSrc();  assert(osrc_re);
            printObjectsForRE(osrc_re);

            RuleElementODst *odst_re = nat_rule->getODst();  assert(odst_re);
            printObjectsForRE(odst_re);

            RuleElementOSrv *osrv_re = nat_rule->getOSrv();  assert(osrv_re);
            printObjectsForRE(osrv_re);

            RuleElementTSrc *tsrc_re = nat_rule->getTSrc();  assert(tsrc_re);
            printObjectsForRE(tsrc_re);

            RuleElementTDst *tdst_re = nat_rule->getTDst();  assert(tdst_re);
            printObjectsForRE(tdst_re);

            RuleElementTSrv *tsrv_re = nat_rule->getTSrv();  assert(tsrv_re);
            printObjectsForRE(tsrv_re);
        }

    }

    return true;
}

