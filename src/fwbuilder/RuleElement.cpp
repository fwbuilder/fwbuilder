/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: RuleElement.cpp 1024 2007-05-22 20:27:42Z vkurland $


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
    remStr("comment");
    remStr("name");
    remStr("id");

    setBool("neg",false);
}

RuleElement::RuleElement(const FWObject *root,bool prepopulate) : FWObject(root,prepopulate)
{
    remStr("comment");
    remStr("name");
    remStr("id");

    setBool("neg",false);
}

void RuleElement::fromXML(xmlNodePtr root) throw(FWException)
{
    const char *n;

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("neg")));
    if(n)
    {
        setStr("neg",n);
        FREEXMLBUFF(n);
    }

    FWObject::fromXML(root);
}

void RuleElement::addRef(FWObject *obj)
{
    FWObject *o=NULL;
    if (isAny()) {
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
    string any_id=getAnyElementId();
    
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

    string any_id=getAnyElementId();
    FWObject *any_obj=((FWObjectDatabase*)root)->checkIndex(any_id);
    if (any_obj)
        FWObject::addRef( any_obj );
}

string RuleElement::getAnyElementId()
{
    return "";
}

/*
 *   Meaning of "source","destination" and "service" can be negated
 */
bool RuleElement::getNeg()
{
    return ( getBool("neg") );
}

void RuleElement::setNeg(bool f) 
{ 
    if (!empty()) setBool("neg",f);
    else          setBool("neg",false);  // it does not make sence to negate nothing
}

void RuleElement::toggleNeg()
{ 
    bool n;
    n=getBool("neg");
    if (!empty()) setBool("neg",!n );
    else          setBool("neg",false);  // it does not make sence to negate nothing
}

const char *RuleElementSrc::TYPENAME={"Src"};
RuleElementSrc::RuleElementSrc() {}
RuleElementSrc::RuleElementSrc(const FWObject *root,bool prepopulate) :
    ObjectGroup(root,prepopulate),RuleElement(root,prepopulate) 
{
    if (prepopulate)
        _initialize(root);
}

string RuleElementSrc::getAnyElementId() {
    return FWObjectDatabase::getAnyNetworkId();
}



const char *RuleElementDst::TYPENAME={"Dst"};
RuleElementDst::RuleElementDst() {}
RuleElementDst::RuleElementDst(const FWObject *root,bool prepopulate) :
    ObjectGroup(root,prepopulate),RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

string RuleElementDst::getAnyElementId() {
    return FWObjectDatabase::getAnyNetworkId();
}



const char *RuleElementSrv::TYPENAME={"Srv"};
RuleElementSrv::RuleElementSrv() {}
RuleElementSrv::RuleElementSrv(const FWObject *root,bool prepopulate) :
    ServiceGroup(root,prepopulate),RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}


string RuleElementSrv::getAnyElementId() {
    return FWObjectDatabase::getAnyIPServiceId();
}

const char *RuleElementItf::TYPENAME={"Itf"};
RuleElementItf::RuleElementItf() {}
RuleElementItf::RuleElementItf(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

string RuleElementItf::getAnyElementId()
{
    return FWObjectDatabase::getAnyNetworkId();
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
    // Check if o is child of a firewall
    FWObject* o_tmp = getRoot()->findInIndex(o->getId());
    bool child_of_firewall = false;
    while( o_tmp && 
           o_tmp->getRoot() != o_tmp && 
           Firewall::cast(o_tmp) == NULL) o_tmp = o_tmp->getParent();
    if( Firewall::cast(o_tmp)!=NULL) child_of_firewall = true;
    
    // Check if the Firewall the Interface belongs to is the same this RuleElement belongs to
    bool same_firewall = false;
    if( child_of_firewall)
    {
        FWObject* o_tmp2 = this;
        while( Firewall::cast(o_tmp2) == NULL) o_tmp2 = o_tmp2->getParent();
      
        if( Firewall::cast(o_tmp) == Firewall::cast(o_tmp2)) same_firewall = true;
    }
    
    return (child_of_firewall && same_firewall);
}




const char *RuleElementOSrc::TYPENAME={"OSrc"};
RuleElementOSrc::RuleElementOSrc() {}
RuleElementOSrc::RuleElementOSrc(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

string RuleElementOSrc::getAnyElementId() {
    return FWObjectDatabase::getAnyNetworkId();
}


const char *RuleElementODst::TYPENAME={"ODst"};
RuleElementODst::RuleElementODst() {}
RuleElementODst::RuleElementODst(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

string RuleElementODst::getAnyElementId() {
    return FWObjectDatabase::getAnyNetworkId();
}


const char *RuleElementOSrv::TYPENAME={"OSrv"};
RuleElementOSrv::RuleElementOSrv() {}
RuleElementOSrv::RuleElementOSrv(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

string RuleElementOSrv::getAnyElementId() {
    return FWObjectDatabase::getAnyIPServiceId();
}




const char *RuleElementTSrc::TYPENAME={"TSrc"};
RuleElementTSrc::RuleElementTSrc() {}
RuleElementTSrc::RuleElementTSrc(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

string RuleElementTSrc::getAnyElementId() {
    return FWObjectDatabase::getAnyNetworkId();
}


const char *RuleElementTDst::TYPENAME={"TDst"};
RuleElementTDst::RuleElementTDst() {}
RuleElementTDst::RuleElementTDst(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

string RuleElementTDst::getAnyElementId() {
    return FWObjectDatabase::getAnyNetworkId();
}


const char *RuleElementTSrv::TYPENAME={"TSrv"};
RuleElementTSrv::RuleElementTSrv()  {}
RuleElementTSrv::RuleElementTSrv(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

string RuleElementTSrv::getAnyElementId() {
    return FWObjectDatabase::getAnyIPServiceId();
}




const char *RuleElementInterval::TYPENAME={"When"};
RuleElementInterval::RuleElementInterval() {}
RuleElementInterval::RuleElementInterval(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

string RuleElementInterval::getAnyElementId() {
    return FWObjectDatabase::getAnyIntervalId();
}




const char *RuleElementRDst::TYPENAME={"RDst"};
RuleElementRDst::RuleElementRDst() {}
RuleElementRDst::RuleElementRDst(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

string RuleElementRDst::getAnyElementId() {
    return FWObjectDatabase::getAnyNetworkId();
}

const char *RuleElementRGtw::TYPENAME={"RGtw"};
RuleElementRGtw::RuleElementRGtw() {}
RuleElementRGtw::RuleElementRGtw(const FWObject *root,bool prepopulate) :
    RuleElement(root,prepopulate)
{
    if (prepopulate)
        _initialize(root);
}

string RuleElementRGtw::getAnyElementId() {
    return FWObjectDatabase::getAnyNetworkId();
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

// the IP address of the gateway has to be in a local network of the firewall
bool RuleElementRGtw::checkReachableIPAdress(FWObject *o)
{
    FWObject* o_tmp = this;
    while( Firewall::cast(o_tmp) == NULL) o_tmp = o_tmp->getParent();
    // let's walk over all interfaces of this firewall
    FWObjectTypedChildIterator j=o_tmp->findByType(Interface::TYPENAME);


    if( Host::cast(o) != NULL)
    {
        Host *host=Host::cast(o);
        InetAddr ip_host = host->getAddress();
        for ( ; j!=j.end(); ++j )
        {
            Interface *i_firewall=Interface::cast(*j);
            for(FWObjectTypedChildIterator fw_ips=i_firewall->findByType(IPv4::TYPENAME); fw_ips!=fw_ips.end(); ++fw_ips) {
                IPv4 *ipv4_obj_firewall = IPv4::cast(*fw_ips);

                InetAddrMask fw_net(ipv4_obj_firewall->getAddress(),
                                 ipv4_obj_firewall->getNetmask());
                if (fw_net.belongs(ip_host))
                    return true;
            }
        }
        return false;

    } else if( Interface::cast(o) != NULL)
    {
        Interface *gw_interface=Interface::cast(o);
        InetAddr ip_gateway = gw_interface->getAddress();

        // walk over all interfaces of this firewall
        for ( ; j!=j.end(); ++j )
        {
            Interface *if_firewall=Interface::cast(*j);
            FWObjectTypedChildIterator addresses=if_firewall->findByType(IPv4::TYPENAME);

            // check all IPv4 addresses of this firewall interface
            for ( ; addresses!=addresses.end(); ++addresses ) {
                IPv4 *ipv4_obj_firewall = IPv4::cast(*addresses);
                InetAddrMask fw_net(ipv4_obj_firewall->getAddress(),
                                 ipv4_obj_firewall->getNetmask());
                if (fw_net.belongs(ip_gateway))
                    return true;

            }
        }
        return false;

    } else if( IPv4::cast(o) != NULL) {

        IPv4 *ipv4=IPv4::cast(o);
        InetAddr ip_ipv4 = ipv4->getAddress();

        for ( ; j!=j.end(); ++j ) {
            Interface *if_firewall=Interface::cast(*j);
            FWObjectTypedChildIterator addresses=if_firewall->findByType(IPv4::TYPENAME);

            // check all IPv4 addresses of this firewall interface
            for ( ; addresses!=addresses.end(); ++addresses ) {
                IPv4 *ipv4_obj_firewall = IPv4::cast(*addresses);
                
                InetAddrMask fw_net(ipv4_obj_firewall->getAddress(),
                                 ipv4_obj_firewall->getNetmask());
                if (fw_net.belongs(ip_ipv4))
                    return true;
            }
        } return false;

    } else return true;

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

