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




#include "fwbuilder/Service.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/Interval.h"

#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/RuleSet.h"

#include <iostream>

using namespace std;
using namespace libfwbuilder;

const char *ServiceGroup::TYPENAME={"ServiceGroup"};

ServiceGroup::ServiceGroup() : Group() {}
ServiceGroup::~ServiceGroup() {}

bool  ServiceGroup::validateChild(FWObject *o)
{ 
    if (FWServiceReference::cast(o)!=nullptr) return true;

    return (FWObject::validateChild(o) && 
            Address::cast(o)==nullptr &&
            ObjectGroup::cast(o)==nullptr &&
            Interval::cast(o)==nullptr &&
            FWObjectReference::cast(o)==nullptr &&
            RuleSet::cast(o)==nullptr);
}

FWReference* ServiceGroup::createRef()
{
//    FWServiceReference *ref=new FWServiceReference();
    FWServiceReference *ref = getRoot()->createFWServiceReference();
    ref->setPointer(this);
    return ref;
}

xmlNodePtr ServiceGroup::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j)
        (*j)->toXML(me);

    return me;
}

void ServiceGroup::getAllowedTypesOfChildren(std::list<std::string> &types_list)
{
    types_list.clear();
    types_list.push_back(IPService::TYPENAME);
    types_list.push_back(ICMPService::TYPENAME);
    types_list.push_back(ICMP6Service::TYPENAME);
    types_list.push_back(TCPService::TYPENAME);
    types_list.push_back(UDPService::TYPENAME);
    types_list.push_back(CustomService::TYPENAME);
    types_list.push_back(TagService::TYPENAME);
    types_list.push_back(UserService::TYPENAME);
    types_list.push_back(FWServiceReference::TYPENAME);
}


