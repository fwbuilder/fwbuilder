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


#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/Service.h>
#include <fwbuilder/ServiceGroup.h>
#include <fwbuilder/FWServiceReference.h>
#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/ObjectGroup.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/Address.h>
#include <fwbuilder/Interval.h>

using namespace std;
using namespace libfwbuilder;

const char *ServiceGroup::TYPENAME={"ServiceGroup"};

ServiceGroup::ServiceGroup() : Group() {}
ServiceGroup::ServiceGroup(const FWObjectDatabase *root,bool prepopulate) : Group(root,prepopulate) {}

ServiceGroup::~ServiceGroup() {}

bool  ServiceGroup::validateChild(FWObject *o)
{ 
    FWObject *oo = FWReference::getObject(o);

    return (FWObject::validateChild(oo) && 
            Address::cast(oo)==NULL &&
            ObjectGroup::cast(oo)==NULL &&
            Interval::cast(oo)==NULL &&
            FWObjectReference::cast(oo)==NULL);
}

FWReference* ServiceGroup::createRef()
{
//    FWServiceReference *ref=new FWServiceReference();
    FWServiceReference *ref = getRoot()->createFWServiceReference();
    ref->setPointer(this);
    return ref;
}

xmlNodePtr ServiceGroup::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j)
        (*j)->toXML(me);

    return me;
}


