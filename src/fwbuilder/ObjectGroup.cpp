/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: ObjectGroup.cpp 975 2006-09-10 22:40:37Z vkurland $


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

#include <fwbuilder/libfwbuilder-config.h>


#include <fwbuilder/ObjectGroup.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/Address.h>
#include <fwbuilder/AddressRange.h>
#include <fwbuilder/DNSName.h>
#include <fwbuilder/AddressTable.h>
#include <fwbuilder/Service.h>
#include <fwbuilder/ServiceGroup.h>
#include <fwbuilder/FWServiceReference.h>

using namespace std;
using namespace libfwbuilder;

const char *ObjectGroup::TYPENAME={"ObjectGroup"};

ObjectGroup::ObjectGroup() : Group() {}
ObjectGroup::ObjectGroup(const FWObject *root,bool prepopulate) :
    Group(root,prepopulate) {}

ObjectGroup::~ObjectGroup() {}

bool ObjectGroup::validateChild(FWObject *o)
{ 
    string otype=o->getTypeName();

    return (FWObject::validateChild(o) && 
            Service::cast(o)==NULL &&
            ServiceGroup::cast(o)==NULL &&
            FWServiceReference::cast(o)==NULL);
}



