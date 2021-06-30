/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org


                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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


#ifndef __RULEELEMENT_HH_FLAG__
#define __RULEELEMENT_HH_FLAG__

#include "fwbuilder/Group.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/Service.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/IntervalGroup.h"

namespace libfwbuilder
{

class Rule; 
class FWReference;

class RuleElement : virtual public FWObject {

protected:

    bool negation;
    
public:
    RuleElement();

    /**
     * This method should create any standard mandatory child objects
     * the object might need.
     */
    virtual void init(FWObjectDatabase *root);
    
    DECLARE_FWOBJECT_SUBTYPE(RuleElement);

    DECLARE_DISPATCH_METHODS(RuleElement);
    
    virtual void fromXML(xmlNodePtr parent);
    virtual xmlNodePtr toXML(xmlNodePtr xml_parent_node);

    virtual FWObject& shallowDuplicate(const FWObject *obj,
                                       bool preserve_id = true);
    
    void setAnyElement();
    void reset();

    virtual int getAnyElementId() const;
    bool isAny() const;

    /**
     * Adds reference object pointing to 'obj' as a child of 'this'. If
     * RuleElement contained "any", it is removed
     */
    virtual void addRef(FWObject *obj);
    
    /**
     * Removes reference to given object among children of 'this'. If
     * this object was the last one, appropriate "any" object is added
     * instead
     */
    virtual void removeRef(FWObject *obj);

    bool getNeg() const { return negation; }
    void setNeg(bool flag) { negation = flag; }
    void toggleNeg() { negation = !negation; }

    virtual int getDummyElementId() const { return -1; }
    virtual bool isDummy() const { return false; }

};

class RuleElementSrc : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementSrc);
    DECLARE_DISPATCH_METHODS(RuleElementSrc);
    RuleElementSrc();

    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
    virtual int getDummyElementId() const;
    virtual bool isDummy() const;
};

class RuleElementDst : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementDst);
    DECLARE_DISPATCH_METHODS(RuleElementDst);
    RuleElementDst();

    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
    virtual int getDummyElementId() const;
    virtual bool isDummy() const;
};

class RuleElementSrv : public ServiceGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementSrv);
    DECLARE_DISPATCH_METHODS(RuleElementSrv);
    RuleElementSrv();

    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
    virtual int getDummyElementId() const;
    virtual bool isDummy() const;
};

class RuleElementItf : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementItf);
    DECLARE_DISPATCH_METHODS(RuleElementItf);
    RuleElementItf();
    
    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    bool checkItfChildOfThisFw(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
    virtual int getDummyElementId() const;
    virtual bool isDummy() const;
};

class RuleElementItfInb : public RuleElementItf
{
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementItfInb);
    DECLARE_DISPATCH_METHODS(RuleElementItfInb);
    RuleElementItfInb();
};

class RuleElementItfOutb : public RuleElementItf
{
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementItfOutb);
    DECLARE_DISPATCH_METHODS(RuleElementItfOutb);
    RuleElementItfOutb();
   
};

class RuleElementInterval : public IntervalGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementInterval);
    DECLARE_DISPATCH_METHODS(RuleElementInterval);
    RuleElementInterval();

    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementOSrc : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementOSrc);
    DECLARE_DISPATCH_METHODS(RuleElementOSrc);
    RuleElementOSrc();

    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementODst : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementODst);
    DECLARE_DISPATCH_METHODS(RuleElementODst);
    RuleElementODst();

    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementOSrv : public ServiceGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementOSrv);
    DECLARE_DISPATCH_METHODS(RuleElementOSrv);
    RuleElementOSrv();

    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementTSrc : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementTSrc);
    DECLARE_DISPATCH_METHODS(RuleElementTSrc);
    RuleElementTSrc();

    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementTDst : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementTDst);
    DECLARE_DISPATCH_METHODS(RuleElementTDst);
    RuleElementTDst();

    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementTSrv : public ServiceGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementTSrv);
    DECLARE_DISPATCH_METHODS(RuleElementTSrv);
    RuleElementTSrv();

    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementRDst : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementRDst);
    DECLARE_DISPATCH_METHODS(RuleElementRDst);
    RuleElementRDst();

    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementRGtw : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementRGtw);
    DECLARE_DISPATCH_METHODS(RuleElementRGtw);
    RuleElementRGtw();

    virtual int getAnyElementId() const;
    virtual bool validateChild(FWObject *o);
    bool checkSingleIPAdress(FWObject *o);
    bool checkReachableIPAdress(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementRItf : public RuleElementItf {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementRItf);
    DECLARE_DISPATCH_METHODS(RuleElementRItf);
    RuleElementRItf();

    virtual bool validateChild(FWObject *o);
    virtual bool isPrimaryObject() const { return false; }
};

}

#endif




