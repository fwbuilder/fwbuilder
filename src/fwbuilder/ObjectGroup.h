/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: ObjectGroup.h 975 2006-09-10 22:40:37Z vkurland $


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

#include <fwbuilder/Group.h>

namespace libfwbuilder
{

class ObjectGroup : public Group
{

    public:

    ObjectGroup();
    ObjectGroup(const FWObject *root,bool prepopulate);
    virtual ~ObjectGroup();

    DECLARE_FWOBJECT_SUBTYPE(ObjectGroup);

    /*
     * verify whether given object type is approppriate as a child
     */
    virtual bool    validateChild(FWObject *o);

};

}

#endif

