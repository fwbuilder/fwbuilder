/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: FWReference.h 975 2006-09-10 22:40:37Z vkurland $


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


#ifndef  __FWREF_HH_FLAG__
#define  __FWREF_HH_FLAG__

#include <fwbuilder/FWObject.h>

namespace libfwbuilder
{

/**
 * This class represents reference reference.
 */
class FWReference : public FWObject
{
    private:

    protected:
    
    FWReference();
    FWReference(const FWObject *root,bool prepopulate);

    public:
    
    DECLARE_FWOBJECT_SUBTYPE(FWReference);
    
    virtual ~FWReference();
    
    virtual void fromXML(xmlNodePtr parent) throw(FWException);

    virtual void add(FWObject *obj);

    virtual FWObject *getPointer();
    virtual const std::string& getPointerId();

    void setPointer(FWObject *o);
    void setPointerId(const std::string& ref_id);

};

}

#endif // _FWREF_HH
