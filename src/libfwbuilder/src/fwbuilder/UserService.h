/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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




#ifndef __USERSERVICE_HH_FLAG__
#define __USERSERVICE_HH_FLAG__

#include "fwbuilder/Service.h"

#include <map>

namespace libfwbuilder
{

class UserService : public Service
{
    private:
    
    std::string   userid;
    
    public:
    
    UserService();
    virtual ~UserService();
    
    virtual void       fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool cmp(const FWObject *obj, bool recursive=false);
    
    virtual FWObject& shallowDuplicate(const FWObject *obj, bool preserve_id = true);
    
    DECLARE_FWOBJECT_SUBTYPE(UserService);

    DECLARE_DISPATCH_METHODS(UserService);
    
    virtual std::string getProtocolName() const;
    virtual int getProtocolNumber() const;
   
    const std::string& getUserId() const { return userid; } 
    void setUserId(const std::string& uid) { userid = uid; }
};

}

#endif




