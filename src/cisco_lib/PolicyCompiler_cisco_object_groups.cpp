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

#include "PolicyCompiler_cisco.h"
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

#include <iostream>
#include <assert.h>

#include <QString>
#include <QStringList>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;




BaseObjectGroup* PolicyCompiler_cisco::CreateObjectGroups::findObjectGroup(
    RuleElement *re)
{
    PolicyCompiler_cisco *cisco_comp = dynamic_cast<PolicyCompiler_cisco*>(compiler);

    list<FWObject*> relement;

    for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
    {
        FWObject *o   = *i1;
        FWObject *obj = FWReference::getObject(o);
        relement.push_back(obj);
    }


    for (FWObject::iterator i=cisco_comp->object_groups->begin();
         i!=cisco_comp->object_groups->end(); ++i)
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

bool PolicyCompiler_cisco::CreateObjectGroups::processNext()
{
    PolicyRule *rule = getNext(); if (rule==NULL) return false;
    PolicyCompiler_cisco *cisco_comp = dynamic_cast<PolicyCompiler_cisco*>(compiler);
    Interface *rule_iface = Interface::cast(compiler->dbcopy->findInIndex(
                                                rule->getInterfaceId()));
    assert(rule_iface);

    RuleElement *re = RuleElement::cast(rule->getFirstByType(re_type));
    if (re->size()==1)  // no need to create object-group since there is single object in the rule element
    {
        tmp_queue.push_back(rule);
        return true;
    }

    BaseObjectGroup *obj_group = findObjectGroup(re);
    if (obj_group==NULL)
    {
        //obj_group= new BaseObjectGroup();
        obj_group = ObjectGroupFactory::createObjectGroup(compiler->fw);
        FWObject *o = re->front();
        FWObject *obj = FWReference::getObject(o);

        obj_group->setObjectGroupTypeFromFWObject(obj);
        QStringList gn;
        if (!rule_iface->getLabel().empty())
            gn.push_back(rule_iface->getLabel().c_str());
        gn.push_back(rule->getUniqueId().c_str());
        gn.push_back(name_suffix.c_str());
        obj_group->setName(gn.join(".").toStdString());

        cisco_comp->object_groups->add(obj_group);

        for (FWObject::iterator i1=re->begin(); i1!=re->end(); ++i1) 
        {
            FWObject *o   = *i1;
            FWObject *obj = o;
            if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();
            obj_group->addRef( obj );
        }
    }
    re->clearChildren(false);   // do not want to destroy children objects

    re->addRef(obj_group);

    assert(re->size()==1);

    tmp_queue.push_back(rule);
    return true;
}

bool PolicyCompiler_cisco::printObjectGroups::processNext()
{
    PolicyCompiler_cisco *cisco_comp=dynamic_cast<PolicyCompiler_cisco*>(compiler);

    slurp();
    if (tmp_queue.size()==0) return false;

    for (FWObject::iterator i=cisco_comp->object_groups->begin();
         i!=cisco_comp->object_groups->end(); ++i)
    {
        BaseObjectGroup *og = dynamic_cast<BaseObjectGroup*>(*i);
        assert(og!=NULL);
        if (og->size()==0) continue;
        cisco_comp->output << endl;
        try
        {
            cisco_comp->output << og->toString();
        } catch (FWException &ex)
        {
            compiler->abort(ex.toString());
        }
    }

    return true;
}

