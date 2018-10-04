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

#include <time.h>
#include <assert.h>



#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/XMLTools.h"

#include <iostream>
#include <algorithm>

using namespace std;
using namespace libfwbuilder;

const char *Firewall::TYPENAME={"Firewall"};

Firewall::Firewall()
{
    setStr("platform","unknown");
    setStr("host_OS" ,"unknown");
    setInt("lastModified" ,0);
    setInt("lastInstalled" ,0);
    setInt("lastCompiled" ,0);
   
}

void Firewall::init(FWObjectDatabase *root)
{
    FWObject *opt = getFirstByType(FirewallOptions::TYPENAME);
    if (opt == nullptr)
    {
        add( root->createFirewallOptions() );
        RuleSet *p;
        p = root->createPolicy();
        p->setTop(true);
        add(p);
        p = root->createNAT();
        p->setTop(true);
        add(p);
        p = root->createRouting();
        p->setTop(true);
        add(p);
    }
}

Firewall::~Firewall()  {}

void Firewall::fromXML(xmlNodePtr root)
{
    const char *n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("platform")));
    assert(n!=nullptr);
    setStr("platform", n);
    XMLTools::FreeXmlBuff(n);

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("version")));
    if (n!=nullptr)
    {
        setStr("version", n);
        XMLTools::FreeXmlBuff(n);
    }

    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("host_OS")));
    if (n!=nullptr)
    {
        setStr("host_OS", n);
        XMLTools::FreeXmlBuff(n);
    }
    
    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("lastModified")));
    if(n!=nullptr)
    {
        setStr("lastModified", n);
        XMLTools::FreeXmlBuff(n);
    }
    
    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("lastInstalled")));
    if(n!=nullptr)
    {
        setStr("lastInstalled", n);
        XMLTools::FreeXmlBuff(n);
    }
    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("lastCompiled")));
    if(n!=nullptr)
    {
        setStr("lastCompiled", n);
        XMLTools::FreeXmlBuff(n);
    }
    n=XMLTools::FromXmlCast(xmlGetProp(root,XMLTools::ToXmlCast("inactive")));
    if(n!=nullptr)
    {
        setStr("inactive", n);
        XMLTools::FreeXmlBuff(n);
    }
    
    Host::fromXML(root);

}

xmlNodePtr Firewall::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    FWObject *o;

    for (FWObjectTypedChildIterator it = findByType(NAT::TYPENAME);
         it != it.end(); ++it)
    {
        o = *it;
        if (o) o->toXML(me);
    }
    for (FWObjectTypedChildIterator it = findByType(Policy::TYPENAME);
         it != it.end(); ++it)
    {
        o = *it;
        if (o) o->toXML(me);
    }
    for (FWObjectTypedChildIterator it = findByType(Routing::TYPENAME);
         it != it.end(); ++it)
    {
        o = *it;
        if (o) o->toXML(me);
    }

    for(FWObjectTypedChildIterator it = findByType(Interface::TYPENAME);
        it != it.end(); ++it)
    {
 	o = *it;
        if (o) o->toXML(me);
    }

    o=getFirstByType( Management::TYPENAME );
    if(o) o->toXML(me);

    o=getFirstByType( FirewallOptions::TYPENAME );
    if (o) o->toXML(me);

    return me;
}

FWOptions* Firewall::getOptionsObject()
{
    return FWOptions::cast( getFirstByType(FirewallOptions::TYPENAME) );
}


Policy* Firewall::getPolicy() 
{ 
    return(Policy::cast(findObjectByName(Policy::TYPENAME, "Policy")));
}

NAT* Firewall::getNAT()    
{ 
    return(NAT::cast(findObjectByName(NAT::TYPENAME, "NAT")));
}


Routing* Firewall::getRouting()    
{
    return(Routing::cast(findObjectByName(Routing::TYPENAME, "Routing")));
}


bool  Firewall::validateChild(FWObject *o)
{ 
    string otype=o->getTypeName();
    if (Routing::isA(o))
    {
        // there can be only one
        list<FWObject*> routing_objects = getByType(Routing::TYPENAME);
        if (routing_objects.size() > 0) return false;
    }
    return (FWObject::validateChild(o) && 
	    (otype==Interface::TYPENAME  ||
	     otype==RuleSet::TYPENAME    ||
	     otype==Policy::TYPENAME     ||
	     otype==NAT::TYPENAME        ||
	     otype==Routing::TYPENAME    ||
	     otype==Management::TYPENAME ||
	     otype==FirewallOptions::TYPENAME ));
}

void Firewall::duplicateInterfaces(FWObject *target, const FWObject *source,
                                   map<int,int> &id_mapping, bool preserve_id)
{
    for (FWObjectTypedChildIterator m = source->findByType(Interface::TYPENAME);
         m!=m.end(); ++m ) 
    {
        FWObject *src_interface = *m;
        FWObject *dst_interface_copy =
            target->addCopyOf(src_interface, preserve_id);

        id_mapping[src_interface->getId()] = dst_interface_copy->getId();

        Interface::cast(dst_interface_copy)->duplicateWithIdMapping(
            src_interface, id_mapping, preserve_id);

#if 0
        if (Firewall::cast(target))
        {
            /*
             * duplicate FailoverClusterGroup object of all interfaces
             * this is actually used for Cluster object only atm.
             *
             * Do this only if @target is Firewall or Cluster, do not do
             * this if @target is an interface because in the latter case
             * this function copies subinterfaces and those don't have
             * failover group child objects.
             */
            for (FWObjectTypedChildIterator k = src_interface->findByType(FailoverClusterGroup::TYPENAME);
                 k!=k.end(); ++k )
            {
                FWObject *src_subinterface  = *k;
                FWObject *dst_subinterface_copy =
                    dst_interface_copy->addCopyOf(src_subinterface, preserve_id);

                if (src_subinterface!=nullptr && dst_subinterface_copy!=nullptr)
                    id_mapping[src_subinterface->getId()] = dst_subinterface_copy->getId();
            }

            duplicateInterfaces(dst_interface_copy, src_interface,
                                id_mapping, preserve_id);
        }
#endif

    }
}

FWObject& Firewall::duplicate(const FWObject *obj,
                              bool preserve_id)
{
    string err="Error creating object with type: ";

    checkReadOnly();
    bool xro = obj->getRO();

    //shallowDuplicate(obj, preserve_id);
    FWObject::shallowDuplicate(obj, preserve_id);

    setReadOnly(false);

    destroyChildren();

    id_mapping_for_duplicate.clear();

    duplicateInterfaces(this, obj, id_mapping_for_duplicate, preserve_id);

    for (FWObjectTypedChildIterator it = obj->findByType(Policy::TYPENAME);
         it != it.end(); ++it)
    {
        FWObject *new_ruleset = addCopyOf(*it, preserve_id);
        id_mapping_for_duplicate[(*it)->getId()] = new_ruleset->getId();
    }
    for (FWObjectTypedChildIterator it = obj->findByType(NAT::TYPENAME);
         it != it.end(); ++it)
    {
        FWObject *new_ruleset = addCopyOf(*it, preserve_id);
        id_mapping_for_duplicate[(*it)->getId()] = new_ruleset->getId();
    }
    for (FWObjectTypedChildIterator it = obj->findByType(Routing::TYPENAME);
         it != it.end(); ++it)
    {
        FWObject *new_ruleset = addCopyOf(*it, preserve_id);
        id_mapping_for_duplicate[(*it)->getId()] = new_ruleset->getId();
    }

    // replace references to old fw (obj) with references to this fw
    id_mapping_for_duplicate[obj->getId()] = getId();

    FWObject *o=obj->getFirstByType( Management::TYPENAME );
    addCopyOf(o,preserve_id);

    o=obj->getFirstByType( FirewallOptions::TYPENAME );
    addCopyOf(o,preserve_id);

    // replace references to old objects in rules
    map<int, int>::iterator it;
    for (it=id_mapping_for_duplicate.begin(); it!=id_mapping_for_duplicate.end(); ++it)
    {
        int old_id = it->first;
        int new_id = it->second;
        replaceRef(old_id,  new_id);
    }

    setDirty(true);
    if (xro)  setReadOnly(true);

    return *this;
}

FWObject& Firewall::duplicateForUndo(const FWObject *obj)
{
    setRO(false);
    FWObject *their_mgmt = obj->getFirstByType(Management::TYPENAME);
    if (their_mgmt)
    {
        FWObject *my_mgmt = getManagementObject();
        if (my_mgmt) my_mgmt->duplicate(their_mgmt);
    }
    FWObject::duplicateForUndo(obj);
    return *this;
}

void  Firewall::updateLastInstalledTimestamp()
{
    setInt("lastInstalled",time(nullptr));
}

void Firewall::updateLastModifiedTimestamp()
{
    setInt("lastModified",time(nullptr));
}

bool Firewall::needsInstall()
{
    if (getLastInstalled()==0 || getLastCompiled()==0) return true;
    return !(getLastModified()<=getLastCompiled() && 
            getLastCompiled()<=getLastInstalled());
}

bool Firewall::needsCompile()
{
    return getLastModified()>getLastCompiled() || getLastCompiled()==0;
}

time_t Firewall::getLastModified()
{
    return getInt("lastModified");
    
}

time_t Firewall::getLastInstalled()
{
    return getInt("lastInstalled");
}

time_t Firewall::getLastCompiled()
{
    return getInt("lastCompiled");
    
}
void   Firewall::updateLastCompiledTimestamp()
{
    setInt("lastCompiled",time(nullptr));
}

bool   Firewall::getInactive()
{
    return getBool("inactive");
}

void   Firewall::setInactive(bool b)
{
    setBool("inactive",b);
}

/*
 * There are only two levels of interfaces, i.e. a top-level interface can only
 * have a subinterface. Subinterfaces can not have further subinterfaces.
 */
list<Interface*> Firewall::getInterfacesByType(const string &iface_type)
{
    list<Interface*> res;

    for (FWObjectTypedChildIterator it = findByType(Interface::TYPENAME);
         it != it.end(); ++it)
    {
        Interface *iface = Interface::cast(*it);
        if (iface->getOptionsObject()->getStr("type") == iface_type)
            res.push_back(iface);

        for (FWObjectTypedChildIterator subit = iface->findByType(Interface::TYPENAME);
             subit != subit.end(); ++subit)
        {
            Interface *subiface = Interface::cast(*subit);
            if (subiface->getOptionsObject()->getStr("type") == iface_type)
                res.push_back(subiface);
        }
    }
    return res;
}

void Firewall::assignUniqueRuleIds()
{
    std::for_each(begin(), end(), RuleSet::UniqueRuleIdsSetter());
}

