/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: Helper.cpp,v 1.1 2008/03/06 06:48:56 vkurland Exp $

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


#include "Helper.h"

#include <fwbuilder/Interface.h>
#include <fwbuilder/ObjectGroup.h>
#include <fwbuilder/InetAddr.h>
#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/RuleElement.h>
#include <fwbuilder/Rule.h>
#include "fwbuilder/Resources.h"

#include <assert.h>
#include <limits.h>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;

static unsigned long calculateDimension(FWObject* obj)
{
    if (Group::cast(obj)!=NULL) {
        unsigned long res=0;
	for (FWObject::iterator i1=obj->begin(); i1!=obj->end(); ++i1) 
        {
            unsigned long n=calculateDimension( *i1 );
            if (n==LONG_MAX)    return n;
            if (LONG_MAX-res<n) return LONG_MAX; // prevent overflow
	    res+=n;
	}
        return res;
    } else
    {
        Address *a=Address::cast(obj);
        if (a!=NULL) 
        {
            if (a->isAny()) return LONG_MAX;
            return a->dimension();
        }
    }
    return 0;
}

void Helper::expand_group_recursive_no_cache(FWObject *o,list<FWObject*> &ol)
{
    if (Group::cast( o )!=NULL) {
        for (FWObject::iterator i2=o->begin(); i2!=o->end(); ++i2) 
        {
            FWObject *o1= *i2;
            if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
            assert(o1);

            expand_group_recursive_no_cache(o1,ol);
        }
    } else {
        ol.push_back( o );
    }
}


void Helper::expand_group_recursive(FWObject *o,list<FWObject*> &ol)
{
    if (Group::cast( o )!=NULL) {
        for (FWObject::iterator i2=o->begin(); i2!=o->end(); ++i2) 
        {
            FWObject *o1= *i2;
            if (FWReference::cast(o1)!=NULL) o1=FWReference::cast(o1)->getPointer();
            assert(o1);

            expand_group_recursive(o1,ol);
        }
    } else {
        ol.push_back( o );
    }
}

string  Helper::findInterfaceByAddress(libfwbuilder::Address *obj)
{
    return findInterfaceByAddress(obj->getAddress());
}

string  Helper::findInterfaceByAddress(const libfwbuilder::InetAddr &addr)
{
    Firewall *fw=compiler->fw;
    list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i) {
	Interface *iface=Interface::cast(*i);
	if ( iface->belongs( addr ) ) return iface->getId();
    }
    return "";
}

string  Helper::findInterfaceByNetzone(Address *obj)
{
    return findInterfaceByNetzone(obj->getAddress());
}

string  Helper::findInterfaceByNetzone(const InetAddr &addr) throw(string)
{
    Firewall *fw=compiler->fw;
    map<string,FWObject*> zones;
    FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
    for ( ; i!=i.end(); ++i)
    {
        string netzone_id = (*i)->getStr("network_zone");
        if (netzone_id != "")
        {
            FWObject *netzone=fw->getRoot()->findInIndex(netzone_id);
            for (list<FWObject*>::iterator j=netzone->begin();
                 j!=netzone->end(); ++j)
            {
                assert(Address::cast(*j)!=NULL);
                if (Address::cast(*j)->belongs(addr))
                    zones[(*i)->getId()]=netzone;
            }
        }
    }

/*
 * now compare dimensions of all netzones that contain address obj and
 * pick the one with smallest dimension
 */    
    string  res_id;
    unsigned long res_dim=LONG_MAX;
    for (map<string,FWObject*>::iterator i=zones.begin(); i!=zones.end(); ++i) 
    {
        string    iface_id=(*i).first;
        FWObject *netzone=(*i).second;
        unsigned long dim=calculateDimension(netzone);

        if (dim<=res_dim) 
        {
            res_id=iface_id;
            res_dim=dim;
        }
    }

/*
 * Subnets defined by addresses of interfaces are automatically part
 * of the corresponding network zones
 */
    if (res_id.empty())
        res_id=findInterfaceByAddress( addr );

    if (res_id.empty())
        throw( string("Can not find interface with network zone that includes address ") + addr.toString());
    return res_id;
}

list<string>  Helper::getAllInterfaceIDs()
{
    Firewall *fw=compiler->fw;
    list<string> intf_id_list;
    FWObjectTypedChildIterator i=fw->findByType(Interface::TYPENAME);
    for ( ; i!=i.end(); ++i)
    {
        Interface *ifs = Interface::cast(*i);
        assert(ifs);
        if (ifs->isUnprotected()) continue;   // skip!
        intf_id_list.push_back( (*i)->getId() );
    }
    return intf_id_list;
}

list<string>  Helper::findInterfaceByNetzoneOrAll(RuleElement *re)
{
    Firewall *fw=compiler->fw;
    list<string> intf_id_list;
    if (re->isAny())
    {
        return getAllInterfaceIDs();
    } else
    {
        FWObject *fo = re->front();
        if (FWReference::cast(fo)!=NULL) fo=FWReference::cast(fo)->getPointer();
        Address  *a = Address::cast(fo);
        if (a==NULL)
        {
            Rule *rule = Rule::cast(re->getParent());
            compiler->abort(string("findInterfaceByNetzoneOrAll failed to retrieve first object from the rule element; is argument not of the type RuleElementSrc or RuleElementDst ? Rule ") + rule->getLabel());
        }
        try
        {
            intf_id_list.push_back( findInterfaceByNetzone( a ) );
        } catch(string err)
        {
            // could not find interface with netzone to match address 'a'
            // will assign rule to all interfaces. Act as if all interfaces
            // had network zone 'any' and each matches this address.

            // issue warning only if platform uses netwrk zones.

            bool supports_network_zones =
                Resources::getTargetCapabilityBool(
                    compiler->fw->getStr("platform"), "network_zones");

            if (supports_network_zones) compiler->warning(err);

            FWObjectTypedChildIterator i = compiler->fw->findByType(Interface::TYPENAME);
            for ( ; i!=i.end(); ++i)
            {
                Interface  *ifs = Interface::cast(*i);
                intf_id_list.push_back( ifs->getId() );
            }
        }
    }
    return intf_id_list;
}

string triplet::hash()
{
    return src->getAddress().toString() + "." +
        dst->getAddress().toString() + "." +
        srv->getId();
}
