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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/Firewall.h>

#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWOptions.h>
#include <fwbuilder/Interface.h>
#include <fwbuilder/ClusterGroup.h>
#include <fwbuilder/Management.h>
#include <fwbuilder/IPv4.h>
#include <fwbuilder/IPv6.h>

#include <fwbuilder/Policy.h>
#include <fwbuilder/NAT.h>

#include <fwbuilder/Routing.h>
#include <iostream>

#include <fwbuilder/RuleElement.h>

#include <fwbuilder/XMLTools.h>

#include <iostream>

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

Firewall::Firewall(const FWObjectDatabase *root,bool prepopulate) : Host(root,prepopulate)
{
    setStr("platform","unknown");
    setStr("host_OS" ,"unknown");
    setInt("lastModified" ,0);
    setInt("lastInstalled" ,0);
    setInt("lastCompiled" ,0);

    if (prepopulate)
    {
        add( getRoot()->createFirewallOptions() );
        RuleSet *p;
        p = getRoot()->createPolicy();
        p->setTop(true);
        add(p);
        p = getRoot()->createNAT();
        p->setTop(true);
        add(p);
        p = getRoot()->createRouting();
        p->setTop(true);
        add(p);
    }
}


Firewall::~Firewall()  {}

void Firewall::fromXML(xmlNodePtr root) throw(FWException)
{
    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("platform")));
    assert(n!=NULL);
    setStr("platform", n);
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("version")));
    if (n!=NULL)
    {
        setStr("version", n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("host_OS")));
    if (n!=NULL)
    {
        setStr("host_OS", n);
        FREEXMLBUFF(n);
    }
    
    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("lastModified")));
    if(n!=NULL)
    {
        setStr("lastModified", n);
        FREEXMLBUFF(n);
    }
    
    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("lastInstalled")));
    if(n!=NULL)
    {
        setStr("lastInstalled", n);
        FREEXMLBUFF(n);
    }
    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("lastCompiled")));
    if(n!=NULL)
    {
        setStr("lastCompiled", n);
        FREEXMLBUFF(n);
    }
    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("inactive")));
    if(n!=NULL)
    {
        setStr("inactive", n);
        FREEXMLBUFF(n);
    }
    
    Host::fromXML(root);

}

xmlNodePtr Firewall::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));

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

FWObject& Firewall::duplicate(const FWObject *obj,
                              bool preserve_id) throw(FWException)
{
    string err="Error creating object with type: ";

    checkReadOnly();
    bool xro=obj->getBool("ro"); 

    //shallowDuplicate(obj, preserve_id);
    FWObject::shallowDuplicate(obj, preserve_id);

    setReadOnly(false);

    destroyChildren();

    for (FWObjectTypedChildIterator it = obj->findByType(Policy::TYPENAME);
         it != it.end(); ++it)
    {
        addCopyOf(*it, preserve_id);
    }
    for (FWObjectTypedChildIterator it = obj->findByType(NAT::TYPENAME);
         it != it.end(); ++it)
    {
        addCopyOf(*it, preserve_id);
    }
    for (FWObjectTypedChildIterator it = obj->findByType(Routing::TYPENAME);
         it != it.end(); ++it)
    {
        addCopyOf(*it, preserve_id);
    }

    replaceRef(obj->getId(), getId() );

    for (FWObjectTypedChildIterator m = obj->findByType(Interface::TYPENAME);
         m!=m.end(); ++m ) 
    {
        FWObject *o   = *m;
        FWObject *o1  = addCopyOf(o,preserve_id);

        replaceRef(o->getId(),   o1->getId()   );

        o1->destroyChildren();

        for (FWObjectTypedChildIterator k=o->findByType(IPv4::TYPENAME);
             k!=k.end(); ++k ) 
        {
            FWObject *oa = *k;
            FWObject *oa1= o1->addCopyOf(oa,preserve_id);

            if (oa!=NULL && oa1!=NULL)
                replaceRef(oa->getId(),  oa1->getId() );
        }

        for (FWObjectTypedChildIterator k=o->findByType(IPv6::TYPENAME);
             k!=k.end(); ++k ) 
        {
            FWObject *oa = *k;
            FWObject *oa1= o1->addCopyOf(oa,preserve_id);

            if (oa!=NULL && oa1!=NULL)
                replaceRef(oa->getId(),  oa1->getId() );
        }

        for (FWObjectTypedChildIterator k = o->findByType(physAddress::TYPENAME);
             k!=k.end(); ++k ) 
        {
            FWObject *opa = *k;
            FWObject *opa1= o1->addCopyOf(opa,preserve_id);

            if (opa!=NULL && opa1!=NULL)
                replaceRef(opa->getId(),  opa1->getId() );
        }

        for (FWObjectTypedChildIterator k = o->findByType(InterfaceOptions::TYPENAME);
             k!=k.end(); ++k )
        {
            FWObject *opa = *k;
            FWObject *opa1= o1->addCopyOf(opa,preserve_id);

            if (opa!=NULL && opa1!=NULL)
                replaceRef(opa->getId(),  opa1->getId() );
        }

        /*
         * duplicate ClusterGroup object of all interfaces
         * this is actually used for Cluster object only atm
         */
        for (FWObjectTypedChildIterator k = o->findByType(ClusterGroup::TYPENAME);
             k!=k.end(); ++k )
        {
            FWObject *opa = *k;
            FWObject *opa1= o1->addCopyOf(opa,preserve_id);

            if (opa!=NULL && opa1!=NULL)
                replaceRef(opa->getId(),  opa1->getId() );
        }

        /*
         * duplicate sub-interfaces (only for interfaces with advanced
         * interface config mode enabled)
         */
        for (FWObjectTypedChildIterator s = o->findByType(Interface::TYPENAME);
             s!=s.end(); ++s )
        {
            FWObject *os   = *s;
            FWObject *os1  = o1->addCopyOf(os,preserve_id);

            replaceRef(os->getId(),   os1->getId()   );
            os1->destroyChildren();

            for (FWObjectTypedChildIterator k=os->findByType(IPv4::TYPENAME);
                 k!=k.end(); ++k )
            {
                FWObject *oa = *k;
                FWObject *oa1= os1->addCopyOf(oa,preserve_id);

                if (oa!=NULL && oa1!=NULL)
                    replaceRef(oa->getId(),  oa1->getId() );
            }

            for (FWObjectTypedChildIterator k=os->findByType(IPv6::TYPENAME);
                 k!=k.end(); ++k )
            {
                FWObject *oa = *k;
                FWObject *oa1= os1->addCopyOf(oa,preserve_id);

                if (oa!=NULL && oa1!=NULL)
                    replaceRef(oa->getId(),  oa1->getId() );
            }

            for (FWObjectTypedChildIterator k = os->findByType(physAddress::TYPENAME);
                 k!=k.end(); ++k )
            {
                FWObject *opa = *k;
                FWObject *opa1= os1->addCopyOf(opa,preserve_id);

                if (opa!=NULL && opa1!=NULL)
                    replaceRef(opa->getId(),  opa1->getId() );
            }

            for (FWObjectTypedChildIterator k = os->findByType(InterfaceOptions::TYPENAME);
                 k!=k.end(); ++k )
            {
                FWObject *opa = *k;
                FWObject *opa1= os1->addCopyOf(opa,preserve_id);

                if (opa!=NULL && opa1!=NULL)
                    replaceRef(opa->getId(),  opa1->getId() );
            }
        }
    }

    FWObject *o=obj->getFirstByType( Management::TYPENAME );
    addCopyOf(o,preserve_id);

    o=obj->getFirstByType( FirewallOptions::TYPENAME );
    addCopyOf(o,preserve_id);

    setDirty(true);
    if (xro)  setReadOnly(true);

    return *this;
}

void   Firewall::updateLastInstalledTimestamp()
{
    setInt("lastInstalled",time(NULL));
}

void Firewall::updateLastModifiedTimestamp()
{
    setInt("lastModified",time(NULL));
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
    setInt("lastCompiled",time(NULL));
}

bool   Firewall::getInactive()
{
    return getBool("inactive");
}

void   Firewall::setInactive(bool b)
{
    setBool("inactive",b);
}

