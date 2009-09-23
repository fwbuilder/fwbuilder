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

#ifndef __PIXOBJECTGROUP_HH
#define __PIXOBJECTGROUP_HH

#include "fwbuilder/FWObject.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/ServiceGroup.h"

typedef enum { UNKNOWN, 
               NETWORK, 
               PROTO, 
               ICMP_TYPE, 
               TCP_SERVICE, 
               UDP_SERVICE } pix_group_type;

class PIXGroup : public libfwbuilder::Group {
    private:
    pix_group_type gt;
    static std::map<std::string,int>  nc;

    protected:
    std::string registerGroupName(const std::string &prefix);
    
    public:
    PIXGroup(pix_group_type _gt=UNKNOWN) : libfwbuilder::Group() { gt=_gt; }
    virtual ~PIXGroup() {};
    DECLARE_FWOBJECT_SUBTYPE(PIXGroup);

    virtual bool  validateChild(FWObject*) { return true; }

    void setPIXGroupType(pix_group_type _gt) { gt=_gt; }
    pix_group_type getPIXGroupType() { return gt; }
    virtual void setName(const std::string &prefix);

    bool isServiceGroup();
    bool isObjectGroup();
    std::string getSrvTypeName();
};

#if 0
class PIXObjectGroup : public libfwbuilder::ObjectGroup
{
    private:
    pix_group_type gt;

    public:
    PIXObjectGroup(pix_group_type _gt=NETWORK) : ObjectGroup() { gt=_gt; }
    virtual ~PIXObjectGroup() {};
    DECLARE_FWOBJECT_SUBTYPE(PIXObjectGroup);

    virtual bool  validateChild(FWObject *o) { if (o==NULL) ; return true; }
    virtual void setName(const std::string &prefix);
};

class PIXServiceGroup : public libfwbuilder::ServiceGroup
{
    private:
    pix_group_type gt;

    public:
    PIXServiceGroup(pix_group_type _gt=UNKNOWN) : ServiceGroup() { gt=_gt; }
    virtual ~PIXServiceGroup() {};
    DECLARE_FWOBJECT_SUBTYPE(PIXServiceGroup);

    virtual bool  validateChild(FWObject *o) { if (o==NULL) ; return true; }
    virtual void setName(const std::string &prefix);
};
#endif

#endif
