/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: AddressRange.cpp 975 2006-09-10 22:40:37Z vkurland $

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

#include <fwbuilder/AddressRange.h>
#include <fwbuilder/XMLTools.h>

using namespace libfwbuilder;

const char *AddressRange::TYPENAME={"AddressRange"};

AddressRange::AddressRange() : Address() , 
                               start_address("0.0.0.0") , 
                               end_address("0.0.0.0")
{
}

AddressRange::AddressRange(const FWObject *root,bool prepopulate) :
    Address(root,prepopulate) , start_address("0.0.0.0") , end_address("0.0.0.0")
{
}

AddressRange::AddressRange(AddressRange &o) : Address() , 
                                              start_address(o.getRangeStart()) , 
                                              end_address(o.getRangeEnd()) 
{
    FWObject::operator=(o);
}

IPAddress AddressRange::getAddress() const { return getRangeStart(); }
Netmask   AddressRange::getNetmask() const { return Netmask("255.255.255.255"); }
guint32   AddressRange::dimension()  const
{
    guint32 a1=ntohl( start_address.to32BitInt() );
    guint32 a2=ntohl( end_address.to32BitInt()   );
    return a2-a1+1;
}

void AddressRange::setAddress(const IPAddress &a)  { setRangeStart(a); setRangeEnd(a); }
void AddressRange::setNetmask(const Netmask   &nm) {}

void AddressRange::setAddress(const std::string &a)  { setRangeStart(IPAddress(a)); setRangeEnd(IPAddress(a)); }
void AddressRange::setNetmask(const std::string &nm) {}


FWObject& AddressRange::shallowDuplicate(const FWObject *o, bool preserve_id) throw(FWException)
{
    const AddressRange *n = dynamic_cast<const AddressRange *>(o);
    if (n==NULL) throw(FWException("Attempt to copy incompatible object to AddressRange: objectID="+o->getId()));

    start_address = n->getRangeStart();
    end_address   = n->getRangeEnd();

    return FWObject::shallowDuplicate(o, preserve_id);
}

bool AddressRange::cmp(const FWObject *obj) throw(FWException)
{
    if (AddressRange::constcast(obj)==NULL) return false;
    if (!FWObject::cmp(obj)) return false;

    IPAddress o1b;
    IPAddress o1e;
    IPAddress o2b;
    IPAddress o2e;

    o1b=getRangeStart();  
    o1e=getRangeEnd();

    o2b=AddressRange::constcast(obj)->getRangeStart();  
    o2e=AddressRange::constcast(obj)->getRangeEnd();

    return (o1b==o2b && o1e==o2e);
}

void AddressRange::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);
    
    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("start_address")));
    assert(n!=NULL);
    start_address = n;
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("end_address")));
    assert(n!=NULL);
    end_address = n;
    FREEXMLBUFF(n);
}

xmlNodePtr AddressRange::toXML(xmlNodePtr xml_parent_node) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(xml_parent_node);
    
    xmlNewProp(me, 
               TOXMLCAST("start_address"),
               STRTOXMLCAST(start_address.toString()));
    
    xmlNewProp(me, 
               TOXMLCAST("end_address"),
               STRTOXMLCAST(end_address.toString()));
    
    return me;
}



