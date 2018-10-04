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




#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/Service.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interval.h"

#include "fwbuilder/Host.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/RuleSet.h"

using namespace std;
using namespace libfwbuilder;

const char *ObjectGroup::TYPENAME={"ObjectGroup"};

ObjectGroup::ObjectGroup() : Group() {}

ObjectGroup::~ObjectGroup() {}

bool ObjectGroup::validateChild(FWObject *o)
{ 
    if (FWObjectReference::cast(o)!=nullptr) return true;

    return (FWObject::validateChild(o) && 
            Service::cast(o)==nullptr &&
            ServiceGroup::cast(o)==nullptr &&
            Interval::cast(o)==nullptr &&
            FWServiceReference::cast(o)==nullptr &&
            RuleSet::cast(o)==nullptr);
}

xmlNodePtr ObjectGroup::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j)
        (*j)->toXML(me);

    return me;
}

void ObjectGroup::getAllowedTypesOfChildren(std::list<std::string> &types_list)
{
    types_list.clear();
    types_list.push_back(Host::TYPENAME);
    types_list.push_back(Firewall::TYPENAME);
    types_list.push_back(Cluster::TYPENAME);
    types_list.push_back(Network::TYPENAME);
    types_list.push_back(NetworkIPv6::TYPENAME);
    types_list.push_back(IPv4::TYPENAME);
    types_list.push_back(IPv6::TYPENAME);
    types_list.push_back(DNSName::TYPENAME);
    types_list.push_back(AddressTable::TYPENAME);
    types_list.push_back(AddressRange::TYPENAME);
    types_list.push_back(FWObjectReference::TYPENAME);
}


