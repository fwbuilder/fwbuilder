/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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

#include <iostream>



#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/XMLTools.h"

#include <algorithm>


using namespace libfwbuilder;
using namespace std;

const char *Group::TYPENAME={"Group"};

/*
 *  Group should inherit a list of allowed types from its parent upon creation
 */
Group::Group()  {}

Group::~Group() 
{
}

bool  Group::validateChild(FWObject *o)
{ 
    return FWObject::validateChild(o);
}

int Group::getSize()
{
    return getChildrenCount();
}

bool Group::hasMember(FWObject *o)
{
    // unlike FWObject::hasChild(), Group::hasMember() also checks references
    int o_id = o->getId();
    for (list<FWObject*>::iterator it=begin(); it!=end(); ++it)
    {
        if (FWReference::cast(*it))
        {
            if (FWReference::cast(*it)->getPointerId() == o_id) return true;
        } else
        {
            if ((*it)->getId() == o_id) return true;
        }
    }
    return false;
}

/*
 * if this is user-defined group, it holds references to objects and
 * we need to copy it with all these child objects to accurately
 * reproduce its state for undo. If this is system group, we only copy
 * this object and its attributes. However we should never really need
 * to execute undo/redo for system groups anyway.
 *
 * Important assumption: groups never have a mix of references and
 * actual objects, it is either one or another. We can check the kind
 * of the group by looking at the first child object.
 *
 * Caveat: This breaks FWObject::tree_iterator which is used in long
 * operations that walk the whole tree so the iterator holds the state
 * that is broken when objects are added or removed from groups in the
 * middle of iteration.
 */
FWObject& Group::duplicateForUndo(const FWObject *obj)
{
    setRO(false);
    if ((obj->size() && FWReference::cast(obj->front())!=nullptr) ||
        (this->size() && FWReference::cast(this->front())!=nullptr))
    {
        destroyChildren();
        for(list<FWObject*>::const_iterator m=obj->begin(); m!=obj->end(); ++m)
        {
            if (FWReference::cast(*m))
            {
                FWObject *object = FWReference::getObject(*m);
                addRef(object);
            }
        }
    }
    FWObject::duplicateForUndo(obj);
    return *this;
}

/*
 * this is virtual function. Overloaded functions in derived classes
 * return some meaningful list of types.
 */
void Group::getAllowedTypesOfChildren(std::list<std::string>&)
{
}

