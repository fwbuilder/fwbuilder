/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

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

#ifndef __BASEOBJECTGROUP_HH
#define __BASEOBJECTGROUP_HH

#include "NamedObject.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/FWException.h"

#include <QString>


namespace fwcompiler
{
    class NamedObjectsManager;
    
    class BaseObjectGroup : public libfwbuilder::Group
    {
public:

        typedef enum { UNKNOWN, 
                       NETWORK, 
                       PROTO, 
                       ICMP_TYPE, 
                       TCP_SERVICE, 
                       UDP_SERVICE,
                       TCP_UDP_SERVICE,
                       MIXED_SERVICE } object_group_type;

private:
        object_group_type gt;

public:

        static std::map<QString,int> name_disambiguation;

        static QString registerGroupName(const QString &prefix,
                                         object_group_type gt);
    
        BaseObjectGroup() : libfwbuilder::Group()
        {
            gt = UNKNOWN;
        }

        virtual ~BaseObjectGroup() {};
        DECLARE_FWOBJECT_SUBTYPE(BaseObjectGroup);

        virtual bool  validateChild(FWObject*) { return true; }

        virtual FWObject& shallowDuplicate(const FWObject *obj,
                                           bool preserve_id = true);
        
        void setObjectGroupType(object_group_type _gt) { gt=_gt; }
        object_group_type getObjectGroupType() { return gt; }

        void setObjectGroupTypeFromMembers(NamedObjectsManager *named_obj_manager);
        object_group_type getObjectGroupTypeFromFWObject(
            const libfwbuilder::FWObject *o);
        void setObjectGroupTypeFromFWObject(const libfwbuilder::FWObject *obj);

        bool isServiceGroup();
        bool isObjectGroup();
        virtual std::string getSrvTypeName();
    
        virtual std::string getObjectGroupClass();
        virtual std::string getObjectGroupHeader();
        virtual std::string getObjectGroupFooter();

        virtual QString groupMemberToString(
            libfwbuilder::FWObject *obj, NamedObjectsManager *named_obj_manager);

        virtual QString toString(NamedObjectsManager *named_obj_manager);
    };

}

#endif
