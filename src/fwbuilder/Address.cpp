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

const char *Address::TYPENAME={"Address"};

Address::Address(const FWObject *root,bool prepopulate) : FWObject(root,prepopulate) {}

IPAddress Address::getAddress() const { return IPAddress(); }
Netmask   Address::getNetmask() const { return Netmask(); }
guint32   Address::dimension()  const { return 0; }

void Address::setAddress(const IPAddress &a)    {}
void Address::setNetmask(const Netmask   &nm)   {}
void Address::setAddress(const std::string &a)  {}
void Address::setNetmask(const std::string &nm) {}

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
