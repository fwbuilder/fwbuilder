/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@fwbuilder.org>

  $Id$

  This program is free software which we release under the GNU General Public
  License. You may redistribute and/or modify this program under the terms
  of that license as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed with the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  To get a copy of the GNU General Public License, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/


#include <assert.h>
#include <iostream>


#include "fwbuilder/Address.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWObjectDatabase.h"

using namespace libfwbuilder;
using namespace std;

const char *Address::TYPENAME={"Address"};

Address::Address() : FWObject()
{
    inet_addr_mask = new InetAddrMask();
    setName("address");
}

Address::Address(const Address& other) : FWObject(other)
{
    inet_addr_mask = new InetAddrMask(*(other.inet_addr_mask));
}

Address::~Address()
{
    delete inet_addr_mask;
}

FWObject& Address::shallowDuplicate(const FWObject *other,
                                    bool preserve_id)
{
    const Address* a_other = Address::constcast(other);
    delete inet_addr_mask;
    inet_addr_mask = new InetAddrMask(*(a_other->inet_addr_mask));
    return FWObject::shallowDuplicate(other, preserve_id);
}

FWReference* Address::createRef()
{
    FWObjectReference *ref = getRoot()->createFWObjectReference();
    ref->setPointer(this);
    return ref;
}

bool Address::isAny() const
{ 
    return getId()==FWObjectDatabase::ANY_ADDRESS_ID;
}

const Address* Address::getAddressObject() const
{
    return nullptr;
}

const InetAddrMask* Address::getInetAddrMaskObjectPtr() const
{
    const Address *addr_obj = getAddressObject();
    if (addr_obj) return addr_obj->inet_addr_mask;
    return nullptr;
}

bool Address::hasInetAddress() const
{
    return false;
}

int Address::countInetAddresses(bool ) const
{
    return 0;
}

const InetAddr* Address::getAddressPtr() const
{
    const InetAddrMask *inet_addr_mask = getInetAddrMaskObjectPtr();
    if (inet_addr_mask) return inet_addr_mask->getAddressPtr();
    return nullptr;
}

const InetAddr* Address::getNetmaskPtr() const
{
    const InetAddrMask *inet_addr_mask = getInetAddrMaskObjectPtr();
    if (inet_addr_mask) return inet_addr_mask->getNetmaskPtr();
    return nullptr;
}

const InetAddr* Address::getNetworkAddressPtr() const
{
    const InetAddrMask *inet_addr_mask = getInetAddrMaskObjectPtr();
    if (inet_addr_mask) return inet_addr_mask->getNetworkAddressPtr();
    return nullptr;
}

const InetAddr* Address::getBroadcastAddressPtr() const
{
    const InetAddrMask *inet_addr_mask = getInetAddrMaskObjectPtr();
    if (inet_addr_mask) return inet_addr_mask->getBroadcastAddressPtr();
    return nullptr;
}

void Address::setAddress(const InetAddr& a)
{
    inet_addr_mask->setAddress(a);
}

void Address::setNetmask(const InetAddr& nm)
{
    inet_addr_mask->setNetmask(nm);
}

void Address::setAddressNetmask(const std::string&)
{
}

/* By default dimension is 1. Compilers may rely on this behavior
 * assuming that every Address object represents single address unless
 * specific netmask is given.
 */
unsigned int Address::dimension()  const
{
    const InetAddrMask *addr_obj = getInetAddrMaskObjectPtr();
    if (addr_obj!=nullptr) return addr_obj->dimension();
    return 1;
}

bool Address::belongs(const InetAddr &other) const
{
    const InetAddrMask *addr_obj = getInetAddrMaskObjectPtr();
    if (addr_obj!=nullptr) return addr_obj->belongs(other);
    return false;
}

bool Address::cmp(const FWObject *obj, bool recursive)
{
    if (Address::constcast(obj)==nullptr) return false;
    if (!FWObject::cmp(obj, recursive)) return false;
    if (hasInetAddress()!=Address::constcast(obj)->hasInetAddress()) return false;
    if (!hasInetAddress()) return true;
    // both this and obj are objects that own ip addresses
    return (*inet_addr_mask == *(Address::constcast(obj)->inet_addr_mask));
}

