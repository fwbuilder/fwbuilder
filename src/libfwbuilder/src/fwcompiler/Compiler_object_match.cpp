/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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


#include "Compiler.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/MultiAddress.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/ObjectMatcher.h"

#include <iostream>
#include <string>

 
using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


bool Compiler::complexMatch(Address *obj1, Address *obj2,
                            bool recognize_broadcasts,
                            bool recognize_multicasts)
{
    if (obj1==nullptr || obj2==nullptr) return false;
    ObjectMatcher om;
    om.setRecognizeBroadcasts(recognize_broadcasts);
    om.setRecognizeMulticasts(recognize_multicasts);
    om.setIPV6(ipv6);
    return om.complexMatch(obj1, obj2);
}

/**
 * This method finds interface of obj2 (which is usually
 * firewall object, but not necessarily so) which is connected
 * to the subnet on which obj1 is located.
 */
Interface* Compiler::findInterfaceFor(const Address *obj1, const Address *obj2)
{
    list<FWObject*> interfaces = obj2->getByTypeDeep(Interface::TYPENAME);
    list<FWObject*>::iterator j;
    for (j=interfaces.begin(); j!=interfaces.end(); ++j )
    {
	Interface *iface=Interface::cast(*j);
	assert(iface);

        if (iface->getId() == obj1->getId()) return iface;

        if ( iface->isRegular() && obj1->getAddressPtr() != nullptr)
        {
            if (obj1->getAddressPtr()->isV4())
            {
                FWObjectTypedChildIterator k= iface->findByType(IPv4::TYPENAME);
                for ( ; k!=k.end(); ++k ) 
                {
                    Address *addr = Address::cast(*k);
                    assert(addr);
                    if (checkIfAddressesMatch(addr, obj1)) return iface;
                }
            }
            if (obj1->getAddressPtr()->isV6())
            {
                FWObjectTypedChildIterator k= iface->findByType(IPv6::TYPENAME);
                for ( ; k!=k.end(); ++k ) 
                {
                    Address *addr = Address::cast(*k);
                    assert(addr);
                    if (checkIfAddressesMatch(addr, obj1)) return iface;
                }
            }
        }
    }
    return nullptr;
}

FWObject* Compiler::findAddressFor(const Address *obj1, const Address *obj2)
{
    list<FWObject*> interfaces = obj2->getByTypeDeep(Interface::TYPENAME);
    list<FWObject*>::iterator j;
    for (j=interfaces.begin(); j!=interfaces.end(); ++j )
    {
	Interface *iface=Interface::cast(*j);
	assert(iface);

        if (iface->getId() == obj1->getId() )      return iface;

        if ( iface->isRegular() && obj1->getAddressPtr())
        {
            if (obj1->getAddressPtr()->isV4())
            {
                FWObjectTypedChildIterator k= iface->findByType(IPv4::TYPENAME);
                for ( ; k!=k.end(); ++k ) 
                {
                    Address *addr = Address::cast(*k);
                    assert(addr);
                    if (checkIfAddressesMatch(addr, obj1)) return (*k);
                }
            }
            if (obj1->getAddressPtr()->isV6())
            {
                FWObjectTypedChildIterator k= iface->findByType(IPv6::TYPENAME);
                for ( ; k!=k.end(); ++k ) 
                {
                    Address *addr = Address::cast(*k);
                    assert(addr);
                    if (checkIfAddressesMatch(addr, obj1)) return (*k);
                }
            }
        }
    }
    return nullptr;
}
        
bool Compiler::checkIfAddressesMatch(const Address *a1, const Address *a2)
{
    if (a1->getId() == a2->getId()) return true;
    if (*(a1->getAddressPtr()) == *(a2->getAddressPtr()) ) return true;
    if ((Network::constcast(a2)!=nullptr ||
         NetworkIPv6::constcast(a2)!=nullptr ||
         Interface::constcast(a2->getParent())) &&
        a2->belongs(*(a1->getAddressPtr()))) return true;
    if ((Network::constcast(a1)!=nullptr ||
         NetworkIPv6::constcast(a1)!=nullptr ||
         Interface::constcast(a1->getParent())) &&
        a1->belongs(*(a2->getAddressPtr()))) return true;
    return false;
}

bool Compiler::MatchesAddressFamily(FWObject *o)
{
    if (Address::cast(o))
    {
        const  InetAddr *inet_addr = Address::cast(o)->getAddressPtr();
        if (inet_addr)
        {
            if (ipv6)
            {
                if (inet_addr->isV6()) return true;
            } else
            {
                if (inet_addr->isV4()) return true;
            }
        } else
        {
            // Address object with no ip address (e.g. dynamic interface
            // or run-time address table)
            return true;
        }
        return false;
    }
    // not an address object at all
    return true;
}


