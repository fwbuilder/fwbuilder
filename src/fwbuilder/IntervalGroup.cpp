/*

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/IntervalGroup.h>
#include <fwbuilder/FWIntervalReference.h>
#include <fwbuilder/FWObjectDatabase.h>

using namespace std;
using namespace libfwbuilder;

const char *IntervalGroup::TYPENAME={"IntervalGroup"};

IntervalGroup::IntervalGroup() : Group() {}
IntervalGroup::IntervalGroup(const FWObject *root,bool prepopulate) :
    FWObject(root,prepopulate) {}

IntervalGroup::~IntervalGroup() {}

bool  IntervalGroup::validateChild(FWObject *o)
{ 
    string otype=o->getTypeName();
    return (FWObject::validateChild(o) && 
	    (otype=="Interval" ||
	     otype=="IntervalGroup" ||
	     otype=="IntervalRef" ));
}

FWReference* IntervalGroup::createRef()
{
//    FWIntervalReference *ref=new FWIntervalReference();
    FWIntervalReference *ref=FWIntervalReference::cast(getRoot()->create(FWIntervalReference::TYPENAME));
    ref->setPointer(this);
    return ref;
}

xmlNodePtr IntervalGroup::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));
    xmlNewProp(me, TOXMLCAST("ro"), TOXMLCAST(((getRO()) ? "True" : "False")));

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j)
        (*j)->toXML(me);

    return me;
}

