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

#ifndef __GROUP_REGISTRY_HH__
#define __GROUP_REGISTRY_HH__

#include "fwbuilder/FWObject.h"

#include <list>
#include <set>
#include <map>


namespace libfwbuilder
{
    class RuleElement;
};


class GroupRegistry
{
    // key: object Id, value: a set of names of groups it belongs to
    std::map<std::string, std::set<std::string> > group_registry;

    // key: rule element id, value: a set of names of groups that belonged to it
    std::map<std::string, std::set<std::string> > rule_element_groups;

    std::string getREKey(libfwbuilder::RuleElement *re);
    
public:
    GroupRegistry();

    void registerGroup(libfwbuilder::FWObject *grp,
                       const std::list<libfwbuilder::FWObject*> &objects);

    void registerGroupInRE(libfwbuilder::RuleElement *re, libfwbuilder::FWObject *grp);

    std::set<std::string> getGroupsForRE(libfwbuilder::RuleElement *re);
    std::set<std::string> getGroupsForObject(libfwbuilder::FWObject *obj);

    std::string getGroupRegistryKey(libfwbuilder::FWObject *obj);
    void setGroupRegistryKey(libfwbuilder::FWObject *obj, const std::string &key);
};

#endif
