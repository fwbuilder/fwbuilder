/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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

#include <assert.h>

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/RuleElement.h>
#include <fwbuilder/Rule.h>

#include <fwbuilder/IPService.h>
#include <fwbuilder/Interval.h>
#include <fwbuilder/Interface.h>
#include <fwbuilder/Network.h>
#include <fwbuilder/Firewall.h>

#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWServiceReference.h>
#include <fwbuilder/FWIntervalReference.h>

#include <fwbuilder/XMLTools.h>

#include <iostream>

using namespace std;
using namespace libfwbuilder;

const char *RuleElement::TYPENAME={"RuleElement"};

RuleElement::RuleElement() 
{
    setNeg(false);
}

RuleElement::RuleElement(const FWObject *root, bool prepopulate) :
    FWObject(root,prepopulate)
{
    setNeg(false);
    setId(-1);
}

void RuleElement::fromXML(xmlNodePtr root) throw(FWException)
{
    const char *n;

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("neg")));
    if (n)
    {
        if (strncmp(n, "True", strlen(n))==0) setNeg(true);
        else setNeg(false);
        FREEXMLBUFF(n);
    }

    FWObject::fromXML(root);
}

xmlNodePtr RuleElement::toXML(xmlNodePtr xml_parent_node) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(xml_parent_node);
    xmlNewProp(me, TOXMLCAST("neg"),
               TOXMLCAST(((getNeg()) ? "True" : "False")));
    return me;
}

FWObject& RuleElement::shallowDuplicate(const FWObject *other,
                                        bool preserve_id) throw(FWException)
{
    setNeg(RuleElement::constcast(other)->getNeg());
    return FWObject::shallowDuplicate(other, preserve_id);
}


void RuleElement::addRef(FWObject *obj)
{
    FWObject *o=NULL;
    if (isAny())
    {
	o=(*(begin()));
	o=(FWReference::cast(o))->getPointer();
    }
    FWObject::addRef(obj);
    if (o!=NULL) removeRef(o);
}

void RuleElement::removeRef(FWObject *obj)
{
    FWObject::removeRef(obj);
    
    if (getChildrenCount()==0)
    {   // there is nothing left
	obj=obj->getRoot()->findInIndex( getAnyElementId() );
	if (obj) addRef(obj);
    }
}

bool RuleElement::isAny()
{
    if (getChildrenCount()!=1) return(false);

    FWObject *o;
    list<FWObject*>::iterator m=begin();
    if (  (o=(*m))!=NULL ) 
    {
	if ( FWObjectReference::isA(o) ||
	     FWServiceReference::isA(o) ||
	     FWIntervalReference::isA(o) ) 
        {
	    if ((FWReference::cast(o))->getPointerId()==getAnyElementId()) return(true);
	}
    }
    return(false);
}


void RuleElement::setAnyElement()
{
    int any_id = getAnyElementId();
    
    FWObject *any=getRoot()->findInIndex( any_id );
    if (any)  addRef( any );
}

void RuleElement::reset()
{
    clearChildren();
    setAnyElement();
    setNeg(false);
}

/*
 * special method, it is being called from constructor with a
 * parameter 'root' to initialize rule element with appropriate
 * reference to any
 *
 * this method is intended for internal use only
 */
void RuleElement::_initialize(const FWObject *root)
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
    FWObject *any_obj=((FWObjectDatabase*)root)->checkIndex(any_id);
    if (any_obj)
        FWObject::addRef( any_obj );
}

int RuleElement::getAnyElementId()
{
    return -1;
}

const char *RuleElementSrc::TYPENAME={"Src"};
RuleElementSrc::RuleElementSrc() {}
RuleElementSrc::RuleElementSrc(const FWObject *root,bool prepopulate) :
    ObjectGroup(root,prepopulate),RuleElement(root,prepopulate) 
{
    if (prepopulate)
        _initialize(root);
}

int RuleElementSrc::getAnyElementId() {
    return FWObjectDatabase::ANY_ADDRESS_ID;
}

xmlNodePtr RuleElementSrc::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}



const char *RuleElementDst::TYPENAME={"Dst"};
RuleElementDst::RuleElementDst() {}
RuleElementDst::RuleElementDst(const FWObject *root,bool prepopulate) :
    ObjectGroup(root,prepopulate),RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

int RuleElementDst::getAnyElementId() {
    return FWObjectDatabase::ANY_ADDRESS_ID;
}

xmlNodePtr RuleElementDst::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}



const char *RuleElementSrv::TYPENAME={"Srv"};
RuleElementSrv::RuleElementSrv() {}
RuleElementSrv::RuleElementSrv(const FWObject *root,bool prepopulate) :
    ServiceGroup(root,prepopulate),RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}


int RuleElementSrv::getAnyElementId() {
    return FWObjectDatabase::ANY_SERVICE_ID;
}

xmlNodePtr RuleElementSrv::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}


const char *RuleElementItf::TYPENAME={"Itf"};
RuleElementItf::RuleElementItf() {}
RuleElementItf::RuleElementItf(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

int RuleElementItf::getAnyElementId()
{
    return FWObjectDatabase::ANY_ADDRESS_ID;
}

xmlNodePtr RuleElementItf::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}


bool RuleElementItf::validateChild(FWObject *o)
{
  if (FWObjectReference::cast(o)!=NULL) return true;  //I dont like this
  if ( o->getId() == getAnyElementId()) return true;
  if (Interface::cast(o)!=NULL) return checkItfChildOfThisFw(o);
  if (ObjectGroup::cast(o)!=NULL)
  {
      bool all_intf = true;
      for (FWObject::iterator i=o->begin(); i!=o->end(); ++i)
      {
          FWObject *o1= *i;
          if (FWReference::cast(o1)!=NULL)
              o1=FWReference::cast(o1)->getPointer();
          if (!Interface::isA(o1)) { all_intf = false; break; }
      }
      return all_intf;  // group is allowed if all members are interfaces
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
 */
bool RuleElementItf::checkItfChildOfThisFw(FWObject *o)
{
    FWObject* o_tmp = getRoot()->findInIndex(o->getId());
    FWObject* o_tmp2 = this;

    while (o_tmp)
    {
        if (Firewall::cast(o_tmp))
        {
            // Check if the Firewall the Interface belongs to is the same this
            // RuleElement belongs to
            while (o_tmp2)
            {
                if (Firewall::cast(o_tmp2) && o_tmp->getId()==o_tmp2->getId())
                    return true;
                o_tmp2 = o_tmp2->getParent();
            }
            return false;
        }
        o_tmp = o_tmp->getParent();
    }
    return false;
}


const char *RuleElementOSrc::TYPENAME={"OSrc"};
RuleElementOSrc::RuleElementOSrc() {}
RuleElementOSrc::RuleElementOSrc(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

int RuleElementOSrc::getAnyElementId() {
    return FWObjectDatabase::ANY_ADDRESS_ID;
}


xmlNodePtr RuleElementOSrc::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}


const char *RuleElementODst::TYPENAME={"ODst"};
RuleElementODst::RuleElementODst() {}
RuleElementODst::RuleElementODst(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

int RuleElementODst::getAnyElementId() {
    return FWObjectDatabase::ANY_ADDRESS_ID;
}


xmlNodePtr RuleElementODst::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}


const char *RuleElementOSrv::TYPENAME={"OSrv"};
RuleElementOSrv::RuleElementOSrv() {}
RuleElementOSrv::RuleElementOSrv(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

int RuleElementOSrv::getAnyElementId() {
    return FWObjectDatabase::ANY_SERVICE_ID;
}


xmlNodePtr RuleElementOSrv::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}




const char *RuleElementTSrc::TYPENAME={"TSrc"};
RuleElementTSrc::RuleElementTSrc() {}
RuleElementTSrc::RuleElementTSrc(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

int RuleElementTSrc::getAnyElementId() {
    return FWObjectDatabase::ANY_ADDRESS_ID;
}


xmlNodePtr RuleElementTSrc::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}


const char *RuleElementTDst::TYPENAME={"TDst"};
RuleElementTDst::RuleElementTDst() {}
RuleElementTDst::RuleElementTDst(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

int RuleElementTDst::getAnyElementId() {
    return FWObjectDatabase::ANY_ADDRESS_ID;
}


xmlNodePtr RuleElementTDst::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}


const char *RuleElementTSrv::TYPENAME={"TSrv"};
RuleElementTSrv::RuleElementTSrv()  {}
RuleElementTSrv::RuleElementTSrv(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

int RuleElementTSrv::getAnyElementId() {
    return FWObjectDatabase::ANY_SERVICE_ID;
}


xmlNodePtr RuleElementTSrv::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}




const char *RuleElementInterval::TYPENAME={"When"};
RuleElementInterval::RuleElementInterval() {}
RuleElementInterval::RuleElementInterval(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

int RuleElementInterval::getAnyElementId() {
    return FWObjectDatabase::ANY_INTERVAL_ID;
}


xmlNodePtr RuleElementInterval::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}




const char *RuleElementRDst::TYPENAME={"RDst"};
RuleElementRDst::RuleElementRDst() {}
RuleElementRDst::RuleElementRDst(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

int RuleElementRDst::getAnyElementId() {
    return FWObjectDatabase::ANY_ADDRESS_ID;
}

xmlNodePtr RuleElementRDst::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}


const char *RuleElementRGtw::TYPENAME={"RGtw"};
RuleElementRGtw::RuleElementRGtw() {}
RuleElementRGtw::RuleElementRGtw(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

int RuleElementRGtw::getAnyElementId() {
    return FWObjectDatabase::ANY_ADDRESS_ID;
}

xmlNodePtr RuleElementRGtw::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        (*j)->toXML(me);

    return me;
}


bool RuleElementRGtw::validateChild(FWObject *o) {

    if( getChildrenCount() > 0 && !isAny()) return false;
    
    return checkSingleIPAdress(o);
}

// check if the gateway has only one interface with only one ipv4 adress
bool RuleElementRGtw::checkSingleIPAdress(FWObject *o) {

    if( Host::cast(o) != NULL) {

        list<FWObject*> obj_list = o->getByType("Interface");

        if( obj_list.size() == 1) {

            obj_list = (obj_list.front())->getByType("IPv4");

            if( obj_list.size() == 1) {

                return true;

            } else return false;

        } else return false;

    } else if( Interface::cast(o) != NULL) {

        list<FWObject*> obj_list = o->getByType("IPv4");

        if( obj_list.size() == 1) {

            return true;

        } else return false;
    }
    
    return ( o->getId() == getAnyElementId() ||
            (FWObject::validateChild(o) && 
            (IPv4::cast(o)!=NULL || FWObjectReference::cast(o)!=NULL)));
}

const char *RuleElementRItf::TYPENAME={"RItf"};
RuleElementRItf::RuleElementRItf()  {}
RuleElementRItf::RuleElementRItf(const FWObject *root,bool prepopulate) :
    RuleElementItf(root,prepopulate) {}

/**
 * Unlike RuleElementItf, RuleElementRItf can hold only single object.
 * Both RuleElementItf and RuleElementRItf can only hold Interface objects
 * which must belong to the same firewall the rule element belongs to.
 * Method RuleElementItf::validateChild checks for these conditions.
 */
bool RuleElementRItf::validateChild(FWObject *o)
{
  if( getChildrenCount() > 0 && !isAny()) return false;
  return RuleElementItf::validateChild(o);
}

