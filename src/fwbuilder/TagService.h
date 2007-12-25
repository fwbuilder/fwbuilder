
/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

  $Id: TagService.h 975 2006-09-10 22:40:37Z vkurland $

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


#ifndef __GEN_TAGSERVICE_HH_FLAG__
#define __GEN_TAGSERVICE_HH_FLAG__

#include <fwbuilder/FWObject.h>
#include <fwbuilder/Service.h>

namespace libfwbuilder
{

/**
 * This is base class for all service objects and other objects that
 * can be children of RuleElementSrv (service rule element).  It is
 * never used on it's own, only it's subclasses are used.
 *
 * TODO: we might need to derive ServiceGroup from Service, but this
 * requires lot more testing
 */
class TagService : public Service 
{
    private:

    public:

        std::string getCode();
        void setCode(std::string p);

    DECLARE_FWOBJECT_SUBTYPE(TagService);

    TagService();
    TagService(const FWObject *root,bool prepopulate);
    ~TagService();
    
    
    virtual void fromXML(xmlNodePtr parent) throw(FWException);
    
    virtual std::string getProtocolName();
    virtual int    getProtocolNumber();

    //virtual FWReference* createRef();
    
    //bool isAny() const;
};

}


#endif

