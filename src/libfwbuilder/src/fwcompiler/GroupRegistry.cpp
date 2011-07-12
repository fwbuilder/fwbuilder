/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "GroupRegistry.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWReference.h"

using namespace libfwbuilder;
using namespace std;


GroupRegistry::GroupRegistry()
{}

void GroupRegistry::registerGroup(FWObject *grp, const list<FWObject*> &objects)
{
    for (list<FWObject*>::const_iterator it=objects.begin();
         it!=objects.end(); ++it)
    {
        FWObject *o = FWReference::getObject(*it);
        string str_id = FWObjectDatabase::getStringId(o->getId());
        group_registry[str_id].insert(grp->getName());
        setGroupRegistryKey(o, str_id);
    }
}

set<string> GroupRegistry::getGroupsForObject(FWObject *obj)
{
    return group_registry[getGroupRegistryKey(obj)];
}

string GroupRegistry::getGroupRegistryKey(FWObject *obj)
{
    return obj->getStr(".group_registry_key");
}

void GroupRegistry::setGroupRegistryKey(FWObject *obj, const std::string &key)
{
    return obj->setStr(".group_registry_key", key);
}


