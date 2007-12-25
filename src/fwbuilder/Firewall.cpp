/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: Firewall.cpp 1029 2007-07-07 05:36:22Z vkurland $


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
#include <fwbuilder/Management.h>

#include <fwbuilder/Policy.h>
#include <fwbuilder/InterfacePolicy.h>
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

Firewall::Firewall(const FWObject *root,bool prepopulate) : Host(root,prepopulate)
{
    setStr("platform","unknown");
    setStr("host_OS" ,"unknown");
    setInt("lastModified" ,0);
    setInt("lastInstalled" ,0);
    setInt("lastCompiled" ,0);

    if (prepopulate)
    {
        add( getRoot()->create(FirewallOptions::TYPENAME) );
        add( getRoot()->create(Policy::TYPENAME) );
        add( getRoot()->create(NAT::TYPENAME) );
        add( getRoot()->create(Routing::TYPENAME) );
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
    FWObject *o;
    o=getFirstByType( NAT::TYPENAME );
    if (o) o->toXML(me);

    o=getFirstByType( Policy::TYPENAME );
    if (o) o->toXML(me);

    o=getFirstByType( Routing::TYPENAME );
    if (o) o->toXML(me);

    for(FWObjectTypedChildIterator j=findByType(Interface::TYPENAME); j!=j.end(); ++j)
 	if((o=(*j))!=NULL )
 	    o->toXML(me);

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
    return(Policy::cast(getFirstByType(Policy::TYPENAME)));
}

NAT* Firewall::getNAT()    
{ 
    return(NAT::cast(getFirstByType(NAT::TYPENAME)));
}


Routing* Firewall::getRouting()    
{
    return(Routing::cast(getFirstByType(Routing::TYPENAME)));
}


bool  Firewall::validateChild(FWObject *o)
{ 
    string otype=o->getTypeName();
    return (FWObject::validateChild(o) && 
	    (otype==Interface::TYPENAME  ||
	     otype==RuleSet::TYPENAME    ||
	     otype==Policy::TYPENAME     ||
	     otype==NAT::TYPENAME        ||
	     otype==Routing::TYPENAME    ||
	     otype==Management::TYPENAME ||
	     otype==FirewallOptions::TYPENAME ));
}

void Firewall::replaceRef(FWObject *rs,
                          const string &old_id,const std::string &new_id)
{
    FWReference *ref=FWObjectReference::cast(rs);
    if (ref==NULL)
    {
        for (FWObject::iterator j1=rs->begin(); j1!=rs->end(); ++j1)
            replaceRef( *j1, old_id, new_id);
    } else
    { 
        if (ref->getPointerId()==old_id)
            ref->setPointerId(new_id);
    }
}

FWObject& Firewall::duplicate(const FWObject *obj, bool preserve_id) throw(FWException)
{
    string err="Error creating object with type: ";

    checkReadOnly();
    bool xro=obj->getBool("ro"); 

    shallowDuplicate(obj,preserve_id);

    setReadOnly(false);

    destroyChildren();

    Policy   *op = Policy::cast(obj->getFirstByType(Policy::TYPENAME));
    FWObject *pol = addCopyOf(op,preserve_id);
    replaceRef(pol,obj->getId(), getId() );

    NAT      *on = NAT::cast(obj->getFirstByType(NAT::TYPENAME));
    FWObject *nat = addCopyOf(on,preserve_id);
    replaceRef(nat,obj->getId(), getId() );

    Routing  *oroute = Routing::cast(obj->getFirstByType(Routing::TYPENAME));
    FWObject *r = addCopyOf(oroute,preserve_id);
    replaceRef(r,obj->getId(), getId() );  
    
    FWObjectTypedChildIterator m=obj->findByType(Interface::TYPENAME);
    for ( ; m!=m.end(); ++m ) 
    {
        FWObject *o   = *m;
        FWObject *o1  = addCopyOf(o,preserve_id);
        replaceRef(pol, o->getId(),   o1->getId()   );
        replaceRef(nat, o->getId(),   o1->getId()   );
        o1->destroyChildren();

#ifdef USING_INTERFACE_POLICY
	FWObject  *ipolicy  = o->getFirstByType(InterfacePolicy::TYPENAME);
        FWObject  *ipolicy1 = NULL;
        if (ipolicy)
        {
            ipolicy1=o1->addCopyOf(ipolicy,preserve_id);
            replaceRef(ipolicy1, obj->getId(), getId()       );
            replaceRef(ipolicy1, o->getId(),   o1->getId()   );
        }
#endif

        FWObjectTypedChildIterator k=o->findByType(IPv4::TYPENAME);
        for ( ; k!=k.end(); ++k ) 
        {
            FWObject *oa = *k;
            FWObject *oa1= o1->addCopyOf(oa,preserve_id);

            if (oa!=NULL && oa1!=NULL)
            {       
                replaceRef(pol, oa->getId(),  oa1->getId() );
                replaceRef(nat, oa->getId(),  oa1->getId() );
//                if (ipolicy1) replaceRef(ipolicy1, oa->getId(),  oa1->getId() );
            }
        }

        k=o->findByType(physAddress::TYPENAME);
        for ( ; k!=k.end(); ++k ) 
        {
            FWObject *opa = *k;
            FWObject *opa1= o1->addCopyOf(opa,preserve_id);

            if (opa!=NULL && opa1!=NULL)
            {       
                replaceRef(pol, opa->getId(),  opa1->getId() );
                replaceRef(nat, opa->getId(),  opa1->getId() );
//                if (ipolicy1) replaceRef(ipolicy1, opa->getId(),  opa1->getId() );
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
