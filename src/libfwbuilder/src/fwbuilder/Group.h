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

#ifndef __GROUP_HH_FLAG__
#define __GROUP_HH_FLAG__

#include <string>
#include <list>
#include <vector>
#include <stdio.h>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectReference.h"

namespace libfwbuilder
{

class Group : virtual public FWObject 
{
    protected:

    public:

    Group();
    virtual ~Group();

    DECLARE_FWOBJECT_SUBTYPE(Group);

    DECLARE_DISPATCH_METHODS(Group);
    
    bool hasMember(FWObject *o);

    /*
     * verify whether given object type is approppriate as a child
     */
    virtual bool validateChild(FWObject *o);

    virtual bool isPrimaryObject() const { return true; }

    int   getSize();

    /**
     * This method copies all attributes of obj plus all reference
     * child objects to reproduce accurate state of this.  In fact, it
     * simply calls duplicate() if this is user-defined group. For system
     * grops that hold actual objects, it calls shallowDuplicate()
     */
    virtual FWObject& duplicateForUndo(const FWObject *obj);

    /**
     * get the list of object type names that can be inserted into
     * given object group. For example, if the group is ObjectGroup,
     * then the list will include all objects but not services. This
     * reflects definition of the group XML element in DTD.
     */
    virtual void getAllowedTypesOfChildren(std::list<std::string> &types_list);
};

}

#endif

