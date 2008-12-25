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


#ifndef __RULEELEMENT_HH_FLAG__
#define __RULEELEMENT_HH_FLAG__

#include <fwbuilder/Group.h>
#include <fwbuilder/Address.h>
#include <fwbuilder/Service.h>
#include <fwbuilder/Interval.h>
#include <fwbuilder/ObjectGroup.h>
#include <fwbuilder/ServiceGroup.h>
#include <fwbuilder/IntervalGroup.h>

#ifdef _WIN32
// disable "warning C4250: 'libfwbuilder::RuleElementTDst' : inherits 'libfwbuilder::RuleElement::addRef' via dominance"
#  pragma warning(disable:4250)
#endif

namespace libfwbuilder
{

class Rule; 
class FWReference;

class RuleElement : virtual public FWObject {

protected:

    bool negation;
    
    void  _initialize(const FWObjectDatabase *root);

public:
    RuleElement();
    RuleElement(const FWObjectDatabase *root,bool prepopulate);
    DECLARE_FWOBJECT_SUBTYPE(RuleElement);

    virtual void fromXML(xmlNodePtr parent) throw(FWException);
    virtual xmlNodePtr toXML(xmlNodePtr xml_parent_node) throw(FWException);

    virtual FWObject& shallowDuplicate(const FWObject *obj,
                                       bool preserve_id = true)
        throw(FWException);
    
    void setAnyElement();
    void reset();
    virtual int getAnyElementId();
    bool isAny();

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

};

class RuleElementSrc : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementSrc);
    RuleElementSrc();
    RuleElementSrc(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementDst : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementDst);
    RuleElementDst();
    RuleElementDst(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementSrv : public ServiceGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementSrv);
    RuleElementSrv();
    RuleElementSrv(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementItf : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementItf);
    RuleElementItf();
    RuleElementItf(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual bool validateChild(FWObject *o);
    bool checkItfChildOfThisFw(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementInterval : public IntervalGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementInterval);
    RuleElementInterval();
    RuleElementInterval(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementOSrc : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementOSrc);
    RuleElementOSrc();
    RuleElementOSrc(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementODst : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementODst);
    RuleElementODst();
    RuleElementODst(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementOSrv : public ServiceGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementOSrv);
    RuleElementOSrv();
    RuleElementOSrv(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementTSrc : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementTSrc);
    RuleElementTSrc();
    RuleElementTSrc(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementTDst : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementTDst);
    RuleElementTDst();
    RuleElementTDst(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementTSrv : public ServiceGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementTSrv);
    RuleElementTSrv();
    RuleElementTSrv(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementRDst : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementRDst);
    RuleElementRDst();
    RuleElementRDst(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementRGtw : public ObjectGroup, public RuleElement {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementRGtw);
    RuleElementRGtw();
    RuleElementRGtw(const FWObjectDatabase *root,bool prepopulate);
    virtual int getAnyElementId();
    virtual bool validateChild(FWObject *o);
    bool checkSingleIPAdress(FWObject *o);
    bool checkReachableIPAdress(FWObject *o);
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    virtual bool isPrimaryObject() const { return false; }
};

class RuleElementRItf : public RuleElementItf {
    public:
    DECLARE_FWOBJECT_SUBTYPE(RuleElementRItf);
    RuleElementRItf();
    RuleElementRItf(const FWObjectDatabase *root,bool prepopulate);
    virtual bool validateChild(FWObject *o);
    virtual bool isPrimaryObject() const { return false; }
};

}

#endif




