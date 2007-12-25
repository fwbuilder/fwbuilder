/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: Host.cpp 1045 2007-08-30 04:47:19Z vk $


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
#include <fwbuilder/Host.h>
#include <fwbuilder/Interface.h>
#include <fwbuilder/Management.h>

#include <fwbuilder/XMLTools.h>

using namespace std;
using namespace libfwbuilder;

const char *Host::TYPENAME={"Host"};

Host::Host()  
{
}

Host::Host(const FWObject *root,bool prepopulate) : Address(root,prepopulate)
{
    if (prepopulate)
        add( getRoot()->create(HostOptions::TYPENAME) );
        //add(new HostOptions() );
}

Host::~Host()  {}

void Host::fromXML(xmlNodePtr root) throw(FWException)
{
//    const char *n;
//    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("address")));
//    assert(n!=NULL);
//    setStr("address", n);
//    FREEXMLBUFF(n);

    FWObject::fromXML(root);
}

xmlNodePtr Host::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    FWObject *o;

    for(FWObjectTypedChildIterator j=findByType(Interface::TYPENAME); j!=j.end(); ++j)
 	if((o=(*j))!=NULL )
 	    o->toXML(me);
    
    o=getFirstByType( Management::TYPENAME );
    if (o) o->toXML(me);

    o=getFirstByType( HostOptions::TYPENAME );
    if (o) o->toXML(me);

    return me;
}


bool  Host::validateChild(FWObject *o)
{ 
    string otype=o->getTypeName();
    return (FWObject::validateChild(o) && 
	    (otype==Interface::TYPENAME || 
	     otype==Management::TYPENAME ||
	     otype==HostOptions::TYPENAME) );
}


/*
 *  TODO:    Implement syntax checks for interfaces
 *
 */
bool Host::Appropriate(Interface*)
{
  return true;
}

void Host::addInterface(Interface *i)
{
    add(i);
}

void Host::removeInterface(Interface *i)
{
    remove(i);
}

FWOptions* Host::getOptionsObject()
{
    return FWOptions::cast( getFirstByType(HostOptions::TYPENAME) );
}

IPAddress Host::getAddress() const
{
    Interface *iface=NULL;
    for(FWObjectTypedChildIterator j=findByType(Interface::TYPENAME); j!=j.end(); ++j)
    {
        iface=Interface::cast(*j);
        if (iface->isLoopback()) continue;
        if (iface->isManagement()) return iface->getAddress();
    }
    if (iface!=NULL) return iface->getAddress();
    return IPAddress("0.0.0.0"); 
}

Netmask   Host::getNetmask() const
{
    Interface *iface=Interface::cast( getFirstByType(Interface::TYPENAME));
    if (iface!=NULL) return iface->getNetmask();
    return Netmask("0.0.0.0");
}

void Host::setAddress(const IPAddress &a)  { setAddress(a.toString()); }
void Host::setNetmask(const Netmask&) {}

void Host::setAddress(const std::string &a)  
{ 
    Interface *iface=Interface::cast( getFirstByType(Interface::TYPENAME));
    if (iface!=NULL) iface->setAddress(a); 
}

void Host::setNetmask(const std::string &nm)
{ 
    Interface *iface=Interface::cast( getFirstByType(Interface::TYPENAME));
    if (iface!=NULL) iface->setNetmask(nm); 
}

guint32   Host::dimension()  const
{
    return 1;
}

Management *Host::getManagementObject()
{
    Management *res = dynamic_cast<Management *>(getFirstByType(Management::TYPENAME));
    if(!res)
        add( res = Management::cast(getRoot()->create(Management::TYPENAME)) );
//        add(res = new Management());
    return res;
}

/*
 * takes address from management interface and copies it into
 * Management object. If there is no management interface or no
 * address to be found, returns "0.0.0.0". May throw exception if
 * interface has invalid address.
 */
IPAddress Host::getManagementAddress() throw(FWException)
{
    Management *mgmt=getManagementObject();

    for(FWObjectTypedChildIterator j=findByType(Interface::TYPENAME); j!=j.end(); ++j)
    {
        Interface *iface=Interface::cast(*j);
        if (iface->isManagement())
        {
            mgmt->setAddress( iface->getAddress() );
            return iface->getAddress();
        }
    }    
    return IPAddress("0.0.0.0");
}
