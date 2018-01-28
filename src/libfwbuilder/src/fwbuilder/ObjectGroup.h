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

#ifndef __OBJECTGROUP_HH_FLAG__
#define __OBJECTGROUP_HH_FLAG__

#include "fwbuilder/Group.h"

namespace libfwbuilder
{

class ObjectGroup : public Group
{

    public:

    ObjectGroup();
    virtual ~ObjectGroup();

    DECLARE_FWOBJECT_SUBTYPE(ObjectGroup);

    DECLARE_DISPATCH_METHODS(ObjectGroup);
    
    virtual xmlNodePtr toXML(xmlNodePtr xml_parent_node);

    /*
     * verify whether given object type is approppriate as a child
     */
    virtual bool    validateChild(FWObject *o);

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

