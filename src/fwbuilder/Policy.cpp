/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: Policy.cpp 975 2006-09-10 22:40:37Z vkurland $


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


#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/Policy.h>
#include <fwbuilder/Rule.h>
#include <fwbuilder/FWOptions.h>
#include <fwbuilder/XMLTools.h>
 
using namespace libfwbuilder;

const char *Policy::TYPENAME={"Policy"};


Policy::Policy() : RuleSet()
{
    remStr("name");
    remStr("comment");
}

Policy::Policy(const FWObject *root,bool prepopulate) : RuleSet(root,prepopulate)
{
    remStr("name");
    remStr("comment");
}

Policy::~Policy() {}

Rule*   Policy::createRule()
{
    FWObjectDatabase* db=getRoot();
    assert(db!=NULL);
    return Rule::cast( db->create(PolicyRule::TYPENAME) );
}






