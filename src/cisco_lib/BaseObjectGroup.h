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

#include "fwbuilder/FWObject.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/FWException.h"

typedef enum { UNKNOWN, 
               NETWORK, 
               PROTO, 
               ICMP_TYPE, 
               TCP_SERVICE, 
               UDP_SERVICE } pix_group_type;

class BaseObjectGroup : public libfwbuilder::Group {
    private:
    pix_group_type gt;
    static std::map<std::string,int>  nc;

    protected:
    std::string registerGroupName(const std::string &prefix);
    
    public:
    BaseObjectGroup(pix_group_type _gt=UNKNOWN) : libfwbuilder::Group() { gt=_gt; }
    virtual ~BaseObjectGroup() {};
    DECLARE_FWOBJECT_SUBTYPE(BaseObjectGroup);

    virtual bool  validateChild(FWObject*) { return true; }

    void setObjectGroupType(pix_group_type _gt) { gt=_gt; }
    pix_group_type getObjectGroupType() { return gt; }
    virtual void setName(const std::string &prefix);

    bool isServiceGroup();
    bool isObjectGroup();
    std::string getSrvTypeName();
    
    virtual std::string toString() throw(libfwbuilder::FWException);
};


#endif
