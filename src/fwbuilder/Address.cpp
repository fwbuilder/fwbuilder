/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@vk.crocodile.org>

  $Id: Address.cpp 975 2006-09-10 22:40:37Z vkurland $

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


#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/Address.h>
#include <fwbuilder/FWException.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWObjectDatabase.h>

using namespace libfwbuilder;
using namespace std;

const char *Address::TYPENAME={"Address"};

Address::Address() :
    FWObject(),
    InetAddrMask(InetAddr(), InetNetmask(InetAddr::getAllOnes()))
{
    setName("address");
}

Address::Address(const FWObject *root,bool prepopulate) :
    FWObject(root, prepopulate),
    InetAddrMask(InetAddr(), InetNetmask(InetAddr::getAllOnes()))
{
    setName("address");
}

Address::Address(const Address& other) :
    FWObject(other),
    InetAddrMask(other)
{
}

Address::Address(const string& a, const string& nm) : 
    FWObject(),
    InetAddrMask(InetAddr(a), InetNetmask(nm))
{
} 

Address::Address(const std::string &s) throw(FWException) :
    FWObject(),
    InetAddrMask(s)
{
}

FWObject& Address::shallowDuplicate(const FWObject *other,
                                    bool preserve_id) throw(FWException)
{
    const Address* a_other = Address::constcast(other);
    setAddress(a_other->getAddress());
    setNetmask(a_other->getNetmask());
    return FWObject::shallowDuplicate(other, preserve_id);
}

FWReference* Address::createRef()
{
//    FWObjectReference *ref=new FWObjectReference();
    FWObjectReference *ref =
        FWObjectReference::cast(getRoot()->create(FWObjectReference::TYPENAME));
    ref->setPointer(this);
    return ref;
}

bool Address::isAny() const
{ 
    return getId()==FWObjectDatabase::getAnyNetworkId();
}


