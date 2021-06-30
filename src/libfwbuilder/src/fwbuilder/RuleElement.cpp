/* 

                          Firewall Builder

                 Copyright (C) 2000-2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org


                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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

#include <assert.h>



#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Rule.h"

#include "fwbuilder/IPService.h"
#include "fwbuilder/TagService.h"

#include "fwbuilder/Interval.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Firewall.h"

#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/FWIntervalReference.h"

#include "fwbuilder/XMLTools.h"

#include <iostream>

using namespace std;
using namespace libfwbuilder;

const char *RuleElement::TYPENAME={"RuleElement"};

RuleElement::RuleElement() 
{
    setNeg(false);
}

void RuleElement::init(FWObjectDatabase *root)
{
    setRoot(root);

/* we need to add a reference to 'any' to this new rule
   element. However, when objects are being loaded from XML file, ANY
   element may be located at its bottom so at the moment when this
   rule element is created, we may have not read the ANY yet. In that
   case we do not add the reference here because it will be added when
   it is read from the file anyway
*/
    int any_id = getAnyElementId();
    FWObject *any_obj = getById(any_id);
    if (any_obj == nullptr)
    {
        any_obj = root->checkIndex( any_id );
        if (any_obj) FWObject::addRef( any_obj );
    }
}

void RuleElement::fromXML(xmlNodePtr root)
{
    const char *n;

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("neg")));
    if (n)
    {
        negation = (cxx_strcasecmp(n, "1")==0 || cxx_strcasecmp(n , "true")==0);
        XMLTools::FreeXmlBuff(n);
    }

    FWObject::fromXML(root);
}

xmlNodePtr RuleElement::toXML(xmlNodePtr xml_parent_node)
{
    int my_id = getId();
    setId(-1);
    // FWObject::toXML() skips id if it is == -1. RuleElement objects
    // have no ID as per DTD (why?)
    xmlNodePtr me = FWObject::toXML(xml_parent_node, false);
    xmlNewProp(me, XMLTools::ToXmlCast("neg"),
               XMLTools::ToXmlCast(((getNeg()) ? "True" : "False")));
    setId(my_id);
    return me;
}

FWObject& RuleElement::shallowDuplicate(const FWObject *other,
                                        bool preserve_id)
{
    setNeg(RuleElement::constcast(other)->getNeg());
    return FWObject::shallowDuplicate(other, preserve_id);
}


void RuleElement::addRef(FWObject *obj)
{
    FWObject *o=nullptr;
    if (isAny())
    {
	o=(*(begin()));
	o=(FWReference::cast(o))->getPointer();
    }
    FWObject::addRef(obj);
    if (o!=nullptr) removeRef(o);
}

void RuleElement::removeRef(FWObject *obj)
{
    FWObject::removeRef(obj);
    
    if (getChildrenCount()==0)
    {   // there is nothing left
	obj = obj->getRoot()->findInIndex( getAnyElementId() );
	if (obj) addRef(obj);
    }
}

bool RuleElement::isAny() const
{
    if (getChildrenCount()!=1) return(false);
    const FWObject *o = front();
    return ((FWReference::constcast(o))->getPointerIdDirect() == getAnyElementId());
}


void RuleElement::setAnyElement()
{
    int any_id = getAnyElementId();
    FWObject *any = getRoot()->findInIndex( any_id );
    if (any)  addRef( any );
}

void RuleElement::reset()
{
    clearChildren();
    setAnyElement();
    setNeg(false);
}

int RuleElement::getAnyElementId() const
{
    return -1;
}

const char *RuleElementSrc::TYPENAME={"Src"};
RuleElementSrc::RuleElementSrc() {}

int RuleElementSrc::getAnyElementId() const
{
    return FWObjectDatabase::ANY_ADDRESS_ID;
}

xmlNodePtr RuleElementSrc::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}

bool RuleElementSrc::validateChild(FWObject *o)
{
    if (FWObjectReference::cast(o)!=nullptr) return true;
    if ( o->getId() == getAnyElementId()) return true;
    return ObjectGroup::validateChild(o);
}

int RuleElementSrc::getDummyElementId() const
{
    return FWObjectDatabase::DUMMY_ADDRESS_ID;
}

bool RuleElementSrc::isDummy() const
{
    for (FWObject::const_iterator i1 = begin(); i1 != end(); ++i1) {
        if (FWObject *obj = FWObjectReference::getObject(*i1))
            if ((obj->getId() == FWObjectDatabase::DUMMY_ADDRESS_ID)
                    && FWObjectDatabase::getStringId(obj->getId()) == "dummyaddressid0")
                return true;
    }
    return false;
}



const char *RuleElementDst::TYPENAME={"Dst"};
RuleElementDst::RuleElementDst() {}

int RuleElementDst::getAnyElementId() const 
{
    return FWObjectDatabase::ANY_ADDRESS_ID;
}

xmlNodePtr RuleElementDst::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}

bool RuleElementDst::validateChild(FWObject *o)
{
    if (FWObjectReference::cast(o)!=nullptr) return true;
    if ( o->getId() == getAnyElementId()) return true;
    return ObjectGroup::validateChild(o);
}

int RuleElementDst::getDummyElementId() const
{
    return FWObjectDatabase::DUMMY_ADDRESS_ID;
}

bool RuleElementDst::isDummy() const
{
    for (FWObject::const_iterator i1 = begin(); i1 != end(); ++i1) {
        if (FWObject *obj = FWObjectReference::getObject(*i1))
            if ((obj->getId() == FWObjectDatabase::DUMMY_ADDRESS_ID)
                    && FWObjectDatabase::getStringId(obj->getId()) == "dummyaddressid0")
                return true;
    }
    return false;
}



const char *RuleElementSrv::TYPENAME={"Srv"};
RuleElementSrv::RuleElementSrv() {}

int RuleElementSrv::getAnyElementId() const
{
    return FWObjectDatabase::ANY_SERVICE_ID;
}

xmlNodePtr RuleElementSrv::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}

bool RuleElementSrv::validateChild(FWObject *o)
{
    if (FWServiceReference::cast(o)!=nullptr) return true;
    if ( o->getId() == getAnyElementId()) return true;
    return ServiceGroup::validateChild(o);
}

int RuleElementSrv::getDummyElementId() const
{
    return FWObjectDatabase::DUMMY_SERVICE_ID;
}

bool RuleElementSrv::isDummy() const
{
    for (FWObject::const_iterator i1 = begin(); i1 != end(); ++i1) {
        if (FWObject *obj = FWObjectReference::getObject(*i1))
            if ((obj->getId() == FWObjectDatabase::DUMMY_SERVICE_ID)
                    && FWObjectDatabase::getStringId(obj->getId()) == "dummyserviceid0")
                return true;
    }
    return false;
}


const char *RuleElementItf::TYPENAME={"Itf"};
RuleElementItf::RuleElementItf() {}

int RuleElementItf::getAnyElementId() const
{
    return FWObjectDatabase::ANY_ADDRESS_ID;
}

xmlNodePtr RuleElementItf::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}


bool RuleElementItf::validateChild(FWObject *o)
{
    if (FWObjectReference::cast(o)!=nullptr) return true;

    if (o->getId() == getAnyElementId()) return true;

    if (Interface::cast(o)!=nullptr) return true;

    if (ObjectGroup::cast(o)!=nullptr && o->size() > 0)
    {
        bool all_intf = true;
        for (FWObject::iterator i=o->begin(); i!=o->end(); ++i)
        {
            FWObject *o1 = FWReference::getObject(*i);
            if (!Interface::isA(o1)) { all_intf = false; break; }
        }
        return all_intf;  // group is allowed if all members are interfaces
    }
    return false;
}

int RuleElementItf::getDummyElementId() const
{
    return FWObjectDatabase::DUMMY_INTERFACE_ID;
}

bool RuleElementItf::isDummy() const
{
    for (FWObject::const_iterator i1 = begin(); i1 != end(); ++i1) {
        if (FWObject *obj = FWObjectReference::getObject(*i1))
            if ((obj->getId() == FWObjectDatabase::DUMMY_INTERFACE_ID)
                    && FWObjectDatabase::getStringId(obj->getId()) == "dummyinterfaceid0")
                return true;
    }
    return false;
}


/*
 * this method checks if an object 'o' is a child of the same firewall
 * this rule element belongs to
 *
 * Note: when an object is being inserted into a rule using clipboard
 * (copy/paste), the object 'o' is in fact a copy of the original
 * object.  This causes problems because the copy does not have any
 * parent and hence we can not simply traverse up the tree using
 * getParent(). We have to take its ID, find the original (clipboard
 * copy has the same ID as the original) and then walk up the tree
 * starting from it. This also means that if an object was placed into
 * clipboard using 'cut', it won't validate because the original has
 * lost its parent.
 * 
 * After implementing undo(), we now run into the same problem with
 * the rule element object because the GUI tries to call addRef()
 * using a copy of the RuleElement object (the copy is a part of the
 * undo command) and this copy does not have any parent either. The
 * trick with using object Id does not work in this case though
 * because the copy of RuleElement stored in the command has different
 * ID. Will call this function from the GUI to do additional check
 * where appropriate, but validateChild() will accept any Interface
 * object.
 */
bool RuleElementItf::checkItfChildOfThisFw(FWObject *o)
{
    if (Group::cast(o) != nullptr)
    {
        for (FWObject::iterator i=o->begin(); i!=o->end(); ++i)
        {
            FWObject *o1 = FWReference::getObject(*i);
            if (!checkItfChildOfThisFw(o1)) return false;
        }
        return true;
    }

    FWObject* o_tmp = getRoot()->findInIndex(o->getId());
    FWObject* o_tmp2 = getRoot()->findInIndex(this->getId());

    FWObject *fw1 = o_tmp;
    while (fw1 && Firewall::cast(fw1) == nullptr) fw1 = fw1->getParent();
    FWObject *fw2 = o_tmp2;
    while (fw2 && Firewall::cast(fw2) == nullptr) fw2 = fw2->getParent();

    return (fw1 != nullptr && fw1 == fw2);
}

const char *RuleElementItfInb::TYPENAME={"ItfInb"};
RuleElementItfInb::RuleElementItfInb() {}

const char *RuleElementItfOutb::TYPENAME={"ItfOutb"};
RuleElementItfOutb::RuleElementItfOutb() {}


const char *RuleElementOSrc::TYPENAME={"OSrc"};
RuleElementOSrc::RuleElementOSrc() {}

int RuleElementOSrc::getAnyElementId() const
{
    return FWObjectDatabase::ANY_ADDRESS_ID;
}


xmlNodePtr RuleElementOSrc::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}

bool RuleElementOSrc::validateChild(FWObject *o)
{
    if (FWObjectReference::cast(o)!=nullptr) return true;
    if ( o->getId() == getAnyElementId()) return true;
    return ObjectGroup::validateChild(o);
}


const char *RuleElementODst::TYPENAME={"ODst"};
RuleElementODst::RuleElementODst() {}

int RuleElementODst::getAnyElementId() const
{
    return FWObjectDatabase::ANY_ADDRESS_ID;
}


xmlNodePtr RuleElementODst::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}

bool RuleElementODst::validateChild(FWObject *o)
{
    if (FWObjectReference::cast(o)!=nullptr) return true;
    if ( o->getId() == getAnyElementId()) return true;
    return ObjectGroup::validateChild(o);
}


const char *RuleElementOSrv::TYPENAME={"OSrv"};
RuleElementOSrv::RuleElementOSrv() {}

int RuleElementOSrv::getAnyElementId() const
{
    return FWObjectDatabase::ANY_SERVICE_ID;
}


xmlNodePtr RuleElementOSrv::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}

bool RuleElementOSrv::validateChild(FWObject *o)
{
    if (FWServiceReference::cast(o)!=nullptr) return true;
    if ( o->getId() == getAnyElementId()) return true;
    return ServiceGroup::validateChild(o);
}




const char *RuleElementTSrc::TYPENAME={"TSrc"};
RuleElementTSrc::RuleElementTSrc() {}

int RuleElementTSrc::getAnyElementId() const
{
    return FWObjectDatabase::ANY_ADDRESS_ID;
}


xmlNodePtr RuleElementTSrc::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}

bool RuleElementTSrc::validateChild(FWObject *o)
{
    if (FWObjectReference::cast(o)!=nullptr) return true;
    if ( o->getId() == getAnyElementId()) return true;
    return ObjectGroup::validateChild(o);
}


const char *RuleElementTDst::TYPENAME={"TDst"};
RuleElementTDst::RuleElementTDst() {}

int RuleElementTDst::getAnyElementId() const
{
    return FWObjectDatabase::ANY_ADDRESS_ID;
}


xmlNodePtr RuleElementTDst::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}

bool RuleElementTDst::validateChild(FWObject *o)
{
    if (FWObjectReference::cast(o)!=nullptr) return true;
    if ( o->getId() == getAnyElementId()) return true;
    return ObjectGroup::validateChild(o);
}


const char *RuleElementTSrv::TYPENAME={"TSrv"};
RuleElementTSrv::RuleElementTSrv()  {}

int RuleElementTSrv::getAnyElementId() const
{
    return FWObjectDatabase::ANY_SERVICE_ID;
}


xmlNodePtr RuleElementTSrv::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}

bool RuleElementTSrv::validateChild(FWObject *o)
{
    if (FWServiceReference::cast(o)!=nullptr) return true;

    if ( o->getId() == getAnyElementId()) return true;

    // TagService is not allowed in translated service
    if (TagService::isA(o)) return false;
    if (ServiceGroup::cast(o)!=nullptr)
    {
        for (FWObject::iterator i=o->begin(); i!=o->end(); ++i)
        {
            FWObject *o1 = FWReference::getObject(*i);
            if (!validateChild(o1)) return false;
        }
    }
    return ServiceGroup::validateChild(o);
}




const char *RuleElementInterval::TYPENAME={"When"};
RuleElementInterval::RuleElementInterval() {}

int RuleElementInterval::getAnyElementId() const
{
    return FWObjectDatabase::ANY_INTERVAL_ID;
}


xmlNodePtr RuleElementInterval::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}

bool RuleElementInterval::validateChild(FWObject *o)
{
    if (FWIntervalReference::cast(o)!=nullptr)  return true;
    if ( o->getId() == getAnyElementId()) return true;
    return (Interval::cast(o)!=nullptr || IntervalGroup::cast(o)!=nullptr);
}



const char *RuleElementRDst::TYPENAME={"RDst"};
RuleElementRDst::RuleElementRDst() {}

int RuleElementRDst::getAnyElementId() const
{
    return FWObjectDatabase::ANY_ADDRESS_ID;
}

xmlNodePtr RuleElementRDst::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}

bool RuleElementRDst::validateChild(FWObject *o)
{
    if (FWObjectReference::cast(o)!=nullptr) return true;
    if ( o->getId() == getAnyElementId()) return true;
    return ObjectGroup::validateChild(o);
}


const char *RuleElementRGtw::TYPENAME={"RGtw"};
RuleElementRGtw::RuleElementRGtw() {}

int RuleElementRGtw::getAnyElementId() const
{
    return FWObjectDatabase::ANY_ADDRESS_ID;
}

xmlNodePtr RuleElementRGtw::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = RuleElement::toXML(parent);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}


bool RuleElementRGtw::validateChild(FWObject *o)
{
    if (FWObjectReference::cast(o)!=nullptr) return true;
    if( getChildrenCount() > 0 && !isAny()) return false;
    return checkSingleIPAdress(o);
}

// check if the gateway has only one interface with only one ipv4 adress
bool RuleElementRGtw::checkSingleIPAdress(FWObject *o)
{
    if( Host::cast(o) != nullptr)
    {
        list<FWObject*> obj_list = o->getByType("Interface");
        if( obj_list.size() == 1)
        {
            obj_list = (obj_list.front())->getByType("IPv4");
            if( obj_list.size() == 1)
            {
                return true;
            } else return false;
        } else return false;
    } else if( Interface::cast(o) != nullptr)
    {
        list<FWObject*> obj_list = o->getByType("IPv4");
        if( obj_list.size() == 1)
        {
            return true;
        } else return false;
    }
    return ( o->getId() == getAnyElementId() ||
            (FWObject::validateChild(o) && 
            (IPv4::cast(o)!=nullptr || FWObjectReference::cast(o)!=nullptr)));
}

const char *RuleElementRItf::TYPENAME={"RItf"};
RuleElementRItf::RuleElementRItf()  {}

bool RuleElementRItf::validateChild(FWObject *o)
{
    if (FWObjectReference::cast(o)!=nullptr) return true;

    if (getChildrenCount() > 0 && !isAny()) return false;

    if ( o->getId() == getAnyElementId()) return true;

    return (Interface::cast(o)!=nullptr);
}

