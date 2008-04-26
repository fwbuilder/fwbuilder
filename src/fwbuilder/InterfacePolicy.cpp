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

#include <assert.h>

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/InterfacePolicy.h>
#include <fwbuilder/XMLTools.h>
#include <fwbuilder/FWOptions.h>
#include <fwbuilder/FWObjectDatabase.h>

using namespace libfwbuilder;
 
const char *InterfacePolicy::TYPENAME={"InterfacePolicy"};

InterfacePolicy::InterfacePolicy()
{
    remStr("name");
    remStr("comment");
}

InterfacePolicy::InterfacePolicy(const FWObject*,bool )
{
    remStr("name");
    remStr("comment");
}

InterfacePolicy::~InterfacePolicy() {}
 
Rule*   InterfacePolicy::createRule()
{
    FWObjectDatabase* db=getRoot();
    assert(db!=NULL);
    return Rule::cast( db->create(PolicyRule::TYPENAME) );
}









