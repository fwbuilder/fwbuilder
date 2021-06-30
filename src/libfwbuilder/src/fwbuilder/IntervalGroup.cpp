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



#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/FWIntervalReference.h"
#include "fwbuilder/FWObjectDatabase.h"

using namespace std;
using namespace libfwbuilder;

const char *IntervalGroup::TYPENAME={"IntervalGroup"};

IntervalGroup::IntervalGroup() : Group() {}

IntervalGroup::~IntervalGroup() {}

bool  IntervalGroup::validateChild(FWObject *o)
{ 
    FWObject *oo = o;
    if (FWObjectReference::cast(o)!=nullptr) 
        oo = FWObjectReference::cast(o)->getPointer();

    string otype = oo->getTypeName();
    return (FWObject::validateChild(o) && 
	    (otype=="Interval" ||
	     otype=="IntervalGroup" ||
	     otype=="IntervalRef" ));
}

FWReference* IntervalGroup::createRef()
{
//    FWIntervalReference *ref=new FWIntervalReference();
    FWIntervalReference *ref = getRoot()->createFWIntervalReference();
    ref->setPointer(this);
    return ref;
}

xmlNodePtr IntervalGroup::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));
    xmlNewProp(me, XMLTools::ToXmlCast("ro"), XMLTools::ToXmlCast(((getRO()) ? "True" : "False")));

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j)
        (*j)->toXML(me);

    return me;
}

void IntervalGroup::getAllowedTypesOfChildren(std::list<std::string> &types_list)
{
    types_list.clear();
    types_list.push_back(Interval::TYPENAME);
    types_list.push_back(FWIntervalReference::TYPENAME);
}

