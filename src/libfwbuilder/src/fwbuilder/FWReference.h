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


#ifndef  __FWREF_HH_FLAG__
#define  __FWREF_HH_FLAG__

#include "fwbuilder/FWObject.h"

namespace libfwbuilder
{

/**
 * This class represents reference reference.
 */
class FWReference : public FWObject
{
private:

    std::string str_ref;
    int int_ref;

protected:
    
    FWReference();

public:
    
    DECLARE_FWOBJECT_SUBTYPE(FWReference);

    DECLARE_DISPATCH_METHODS(FWReference);
    
    virtual ~FWReference();
    
    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr parent);

    virtual FWObject& shallowDuplicate(
        const FWObject *obj, bool preserve_id = true);

    virtual bool cmp(const FWObject *obj, bool recursive=false);

    virtual void add(FWObject *obj,bool validate=true);

    virtual FWObject *getPointer();
    virtual int getPointerId();

    void setPointer(FWObject *o);
    void setPointerId(int ref_id);

    virtual void dump(std::ostream &f,bool recursive,bool brief,int offset=0) const;

    virtual bool isPrimaryObject() const { return false; }

    /**
     * If obj is FWReference object, return pointer to the object this
     * reference points to. Otherwise return obj. This is a typical
     * operation repeated in many places where we loop over children
     * of some object.
     */
    static FWObject* getObject(FWObject* obj);

    /**
     * Unlike getPointerId(), this method does not use cache when
     * returns reference ID. This is faster and makes it possible to
     * be const method but late initialization does not work when it
     * is used.
     */
    int getPointerIdDirect() const { return int_ref; }

};

}

#endif // _FWREF_HH
