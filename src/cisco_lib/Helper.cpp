/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

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


#include "Helper.h"

#include <fwbuilder/Interface.h>
#include <fwbuilder/ObjectGroup.h>
#include <fwbuilder/InetAddr.h>
#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/RuleElement.h>
#include <fwbuilder/Rule.h>
#include "fwbuilder/Resources.h"
#include <fwbuilder/IPv6.h>
#include <fwbuilder/NetworkIPv6.h>
#include <fwbuilder/Network.h>
#include <fwbuilder/InetAddrMask.h>

#include <assert.h>
#include <limits.h>
#include <iostream>

#include <QObject>
#include <QString>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


// #define DEBUG_NETZONE_OPS 1

static unsigned long calculateDimension(FWObject* obj)
{
    if (Group::cast(obj)!=nullptr)
    {
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
        if (a!=nullptr) 
        {
            if (a->isAny()) return LONG_MAX;
            return a->dimension();
        }
    }
    return 0;
}

void Helper::expand_group_recursive(FWObject *o,list<FWObject*> &ol)
{
    if (Group::cast( o )!=nullptr)
    {
        for (FWObject::iterator i2=o->begin(); i2!=o->end(); ++i2) 
        {
            FWObject *o1= *i2;
            if (FWReference::cast(o1)!=nullptr) o1=FWReference::cast(o1)->getPointer();
            assert(o1);

            expand_group_recursive(o1,ol);
        }
    } else {
        ol.push_back( o );
    }
}

int  Helper::findInterfaceByAddress(Address *obj)
{
    return findInterfaceByAddress(obj->getAddressPtr(), obj->getNetmaskPtr());
}

/*
 * ticket #1293
 * Weird corner case: user made a mistake setting netmask of an
 * intrface to 0.0.0.0, which made this interface match any address.
 * At the same time, this interface was marked as "unprotected".  So,
 * if we get an interface from helper.findInterfaceByNetzoneOrAll()
 * but this interface is unprotected, issue a warning and use all
 * interfaces instead.
 */

int  Helper::findInterfaceByAddress(const InetAddr *addr,
                                    const InetAddr *nm)
{
    if (addr==nullptr) return -1;

#if DEBUG_NETZONE_OPS
    cerr << "Helper::findInterfaceByAddress";
    cerr << " addr=" << addr->toString();
    cerr << " nm=" << nm->toString();
    cerr << endl;
#endif

    Firewall *fw = compiler->fw;
    list<FWObject*> l2 = fw->getByTypeDeep(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
    {
	Interface *iface = Interface::cast(*i);
        if (iface->isDedicatedFailover()) continue;
        if (iface->isUnprotected()) continue;

#if DEBUG_NETZONE_OPS
        cerr << "Helper::findInterfaceByAddress";
        cerr << " intf=" << iface->getName();
        cerr << endl;
#endif

        FWObjectTypedChildIterator j =
            iface->findByType((addr->isV4())?IPv4::TYPENAME:IPv6::TYPENAME);
        for (; j!=j.end(); ++j)
        {
            const Address *i_addr = Address::constcast(*j);

#if DEBUG_NETZONE_OPS
            cerr << "Helper::findInterfaceByAddress";
            cerr << " i_addr=" << i_addr->getName();
            cerr << endl;
            cerr << "    " << i_addr->getAddressPtr()->toString();
            cerr << "    " << i_addr->getNetmaskPtr()->toString();
            cerr << endl;
#endif

            if (nm != nullptr)
            {
                InetAddrMask interface_subnet(*(i_addr->getAddressPtr()),
                                              *(i_addr->getNetmaskPtr()));
                InetAddrMask other_subnet(*addr, *nm);

#if DEBUG_NETZONE_OPS
                cerr << "Helper::findInterfaceByAddress";
                cerr << " addr=" << other_subnet.toString();
                cerr << " intf=" << iface->getName()
                     << "  " << interface_subnet.toString();
                cerr << endl;
#endif

                vector<InetAddrMask> ovr =
                    libfwbuilder::getOverlap(interface_subnet, other_subnet);

#if DEBUG_NETZONE_OPS
                cerr << "Helper::findInterfaceByAddress";
                cerr << " overlap:";
                cerr << " ovr.size()=" << ovr.size();
                if (ovr.size() > 0)
                    cerr << " ovr.front()=" << ovr.front().toString();
                cerr << endl;
#endif
                if (ovr.size()==0) continue;

                // if interface_subnet is equal or wider than other_subnet,
                // getOverlap() returns subnet object equal to other_subnet
                // If other_subnet is wider, returned object is equal
                // to interface_subnet. If they intersect but one does not fit
                // completely in the other, returned object is not equal
                // to either.
                if (ovr.front() == other_subnet)
                {
                    return iface->getId();
                }
            } else
            {
                if ( i_addr->belongs(*addr) ) return iface->getId();
            }
        }
    }
    return -1;
}

int  Helper::findInterfaceByNetzone(Address *obj)
{
    if (IPv4::isA(obj))
    {
        InetAddr host_netmask("255.255.255.255");
        return findInterfaceByNetzone(obj->getAddressPtr(), &host_netmask);
    } else
        return findInterfaceByNetzone(obj->getAddressPtr(), obj->getNetmaskPtr());
}

/**
 * finds interface of the firewall associated with the netzone
 * that object 'obj' belongs to.  Returns interface ID
 *
 */
int  Helper::findInterfaceByNetzone(const InetAddr *addr, const InetAddr *nm)
{
#if DEBUG_NETZONE_OPS
    cerr << "Helper::findInterfaceByNetzone";
    cerr << " matching to";
    cerr << " addr=" << addr;
    if (addr) cerr << " " << addr->toString();
    cerr << " nm=" << nm;
    if (nm) cerr << " " << nm->toString();
    cerr << endl;
#endif

    Firewall *fw = compiler->fw;
    map<int,FWObject*> zones;
    list<FWObject*> l2 = fw->getByTypeDeep(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
    {
        Interface *iface = Interface::cast(*i);
        if (iface->isDedicatedFailover()) continue;
        if (iface->isUnprotected()) continue;

        // NOTE: "network_zone" is globally unique string ID
        int netzone_id =
            FWObjectDatabase::getIntId(iface->getStr("network_zone"));

        if (netzone_id != -1)
        {
            FWObject *netzone = fw->getRoot()->findInIndex(netzone_id);
            list<FWObject*> nz;
            expand_group_recursive(netzone, nz);

#if DEBUG_NETZONE_OPS
            cerr << "Helper::findInterfaceByNetzone";
            cerr << "  netzone_id=" << netzone_id
                 << "  " << iface->getStr("network_zone")
                 << "  " << netzone->getName()
                 << endl;
#endif

            for (list<FWObject*>::iterator j=nz.begin(); j!=nz.end(); ++j)
            {
                Address *netzone_addr = Address::cast(*j);
                
                if (netzone_addr == nullptr) continue;

#if DEBUG_NETZONE_OPS
                cerr << "Helper::findInterfaceByNetzone";
                cerr << "    " << netzone_addr->getName()
                     << "  " << netzone_addr->getAddressPtr()->toString()
                     << endl;
#endif

                // if addr==nullptr, return id of the interfacce that has
                // net_zone=="any"
                if (addr==nullptr)
                {
                    if (netzone_addr->getId()==FWObjectDatabase::ANY_ADDRESS_ID)
                        return iface->getId(); // id of the interface
                } else
                {
                    // see SF bug 3213019
                    // skip ipv6 addresses in network zone group
                    if (netzone_addr->getAddressPtr()->addressFamily() !=
                        addr->addressFamily()) continue;

                    const InetAddr *nz_addr = netzone_addr->getAddressPtr();
                    const InetAddr *nz_netm = netzone_addr->getNetmaskPtr();
                    if (nm != nullptr && nz_netm != nullptr)
                    {
                        InetAddrMask nz_subnet(*nz_addr, *nz_netm);
                        InetAddrMask other_subnet(*addr, *nm);
                        vector<InetAddrMask> ovr =
                            libfwbuilder::getOverlap(nz_subnet,
                                                     other_subnet);
#if DEBUG_NETZONE_OPS
                        cerr << "Helper::findInterfaceByNetzone";
                        cerr << " addr=" << other_subnet.toString();
                        cerr << " nz=" << nz_subnet.toString();
                        cerr << " overlap:";
                        cerr << " ovr.size()=" << ovr.size();
                        if (ovr.size() > 0)
                            cerr << " ovr.front()=" << ovr.front().toString();
                        cerr << endl;
#endif
                        if (ovr.size()==0) continue;
                        // if nz_subnet is equal or wider than other_subnet,
                        // getOverlap() returns subnet object equal to other_subnet
                        // If other_subnet is wider, returned object is equal
                        // to nz_subnet. If they intersect but one does not fit
                        // completely in the other, returned object is not equal
                        // to either.
                        if (ovr.front() == other_subnet)
                        {
                            zones[iface->getId()] = netzone_addr;
#if DEBUG_NETZONE_OPS
                            cerr << "Helper::findInterfaceByNetzone";
                            cerr << "    match" << endl;
#endif
                        }
                    } else
                    {
                        if (netzone_addr->belongs(*addr))
                        {
                            zones[iface->getId()] = netzone_addr;

#if DEBUG_NETZONE_OPS
                            cerr << "Helper::findInterfaceByNetzone";
                            cerr << "    match" << endl;
#endif
                        }
                    }
                }
            }
        }
    }

/*
 * now compare dimensions of all netzones that contain address obj and
 * pick the one with smallest dimension
 */    
    int  res_id = -1;
    unsigned long res_dim = LONG_MAX;
    for (map<int,FWObject*>::iterator i=zones.begin(); i!=zones.end(); ++i) 
    {
        int iface_id = (*i).first;
        FWObject *netzone = (*i).second;
        unsigned long dim = calculateDimension(netzone);

#if DEBUG_NETZONE_OPS
        cerr << "Helper::findInterfaceByNetzone";
        cerr << "    netzone=" << netzone->getName()
             << "  dim=" << dim
             << "  res_dim=" << res_dim
             << endl;
#endif

        if (dim<=res_dim) 
        {
            res_id = iface_id;
            res_dim = dim;
        }
    }

#if DEBUG_NETZONE_OPS
    cerr << "Helper::findInterfaceByNetzone";
    cerr << " Result after scanning network zones: " << res_id << endl;
#endif

/*
 * Subnets defined by addresses of interfaces are automatically part
 * of the corresponding network zones
 */
    if (res_id == -1)
        res_id = findInterfaceByAddress(addr, nm);

    if (res_id == -1)
    {
        QString err = QObject::tr("Can not find interface with network zone "
                                  "that includes address '%1%2'");
        throw(FWException(err
                          .arg((addr)?addr->toString().c_str():"NULL")
                          .arg((nm)?QString("/%1").arg(nm->toString().c_str()):"")
                          .toStdString()));
    }

#if DEBUG_NETZONE_OPS
    cerr << "Helper::findInterfaceByNetzone";
    cerr << " returning " << res_id << endl;
#endif

    return res_id;
}

list<int> Helper::getAllInterfaceIDs()
{
    Firewall *fw = compiler->fw;
    list<int> intf_id_list;
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

list<int> Helper::findInterfaceByNetzoneOrAll(RuleElement *re)
{
    list<int> intf_id_list;
    if (re->isAny())
    {
        return getAllInterfaceIDs();
    } else
    {
        FWObject *fo = re->front();
        if (FWReference::cast(fo)!=nullptr) fo=FWReference::cast(fo)->getPointer();
        Address  *a = Address::cast(fo);
        if (a==nullptr)
        {
            Rule *rule = Rule::cast(re->getParent());
            Q_UNUSED(rule);
            compiler->abort(
                re->getParent(), 
                string("findInterfaceByNetzoneOrAll failed to retrieve first "
                       "object from the rule element; is argument not of "
                       "the type RuleElementSrc or RuleElementDst ?"));
            return intf_id_list;
        }

        try
        {
            int intf_id = findInterfaceByNetzone(a);
            intf_id_list.push_back(intf_id);
        } catch(FWException &ex)
        {
            // could not find interface with netzone to match address 'a'
            // will assign rule to all interfaces. Act as if all interfaces
            // had network zone 'any' and each matches this address.

            // issue warning only if platform uses netwrk zones.

            bool supports_network_zones =
                Resources::getTargetCapabilityBool(
                    compiler->fw->getStr("platform"), "network_zones");

            if (supports_network_zones)
                compiler->warning(ex.toString());

            FWObjectTypedChildIterator i = compiler->fw->findByType(
                Interface::TYPENAME);
            for ( ; i!=i.end(); ++i)
            {
                Interface  *ifs = Interface::cast(*i);
                intf_id_list.push_back( ifs->getId() );
            }
        }
    }
    return intf_id_list;
}
