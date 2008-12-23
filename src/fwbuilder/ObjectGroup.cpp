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


#include <fwbuilder/ObjectGroup.h>
#include <fwbuilder/Service.h>
#include <fwbuilder/ServiceGroup.h>
#include <fwbuilder/FWServiceReference.h>
#include <fwbuilder/FWObjectDatabase.h>

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

xmlNodePtr ObjectGroup::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j)
        (*j)->toXML(me);

    return me;
}



