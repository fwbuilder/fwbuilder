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

#include <assert.h>
#include <iostream>



#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"

#include "fwbuilder/XMLTools.h"

using namespace std;
using namespace libfwbuilder;

const char *Host::TYPENAME={"Host"};

Host::Host()  
{
}

void Host::init(FWObjectDatabase *root)
{
    FWObject *opt = getFirstByType(HostOptions::TYPENAME);
    if (opt == nullptr)
        add( root->createHostOptions() );
}

Host::~Host()  {}

void Host::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);
}

xmlNodePtr Host::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    FWObject *o;

    for(FWObjectTypedChildIterator j=findByType(Interface::TYPENAME); j!=j.end(); ++j)
 	if((o=(*j))!=nullptr )
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

Management *Host::getManagementObject()
{
    Management *res = dynamic_cast<Management *>(
        getFirstByType(Management::TYPENAME));
    if(!res)
        add( res = getRoot()->createManagement() );
//        add(res = new Management());
    return res;
}

/**
 * returns address from management interface. If there is no
 * management interface or no address to be found, returns nullptr.
 * May throw exception if interface has invalid address.
 */
const InetAddr* Host::getManagementAddress()
{
    list<FWObject*> interfaces = getByTypeDeep(Interface::TYPENAME);
    list<FWObject*>::iterator i;
    for (i=interfaces.begin(); i!=interfaces.end(); ++i )
    {
        Interface *iface = Interface::cast(*i);
        if (iface->isManagement())
        {
            FWObjectTypedChildIterator k = iface->findByType(IPv4::TYPENAME);
            if (k != k.end())
                return Address::cast(*k)->getAddressPtr();

        }
    }    
    return nullptr;
}

const Address* Host::getAddressObject() const
{
    FWObjectTypedChildIterator j = findByType(Interface::TYPENAME);
    if (j == j.end()) return nullptr;
    return Interface::cast(*j)->getAddressObject();
}

int Host::countInetAddresses(bool skip_loopback) const
{
    int res = 0;
    FWObjectTypedChildIterator j = findByType(Interface::TYPENAME);
    for( ; j!=j.end(); ++j)
    {
        Interface *iface = Interface::cast(*j);
        res += iface->countInetAddresses(skip_loopback);
    }
    return res;
}

/*
 * This function will find parent host, firewall or cluster object of
 * a given object. If object is not a child of host, firewall or
 * cluster, it returns nullptr
 */
FWObject* Host::getParentHost(FWObject *obj)
{
    FWObject *parent_h = obj;
    while (parent_h != nullptr && Host::cast(parent_h) == nullptr)
        parent_h = parent_h->getParent();
    return parent_h;
}

