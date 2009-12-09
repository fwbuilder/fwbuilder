/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@fwbuilder.org>
  Refactoring: Roman Bovsunivkiy     <a2k0001@gmail.com>

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

#ifndef USAGERESOLVER_H
#define USAGERESOLVER_H

#include "global.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"

#include <map>
#include <set>

class UsageResolver
{
public:
    //UsageResolver();
    static void findWhereUsedRecursively(
        libfwbuilder::FWObject *, libfwbuilder::FWObject *,
        std::set<libfwbuilder::FWObject*> &, libfwbuilder::FWObjectDatabase*);

    static std::list<libfwbuilder::Firewall*> findFirewallsForObject(
        libfwbuilder::FWObject*, libfwbuilder::FWObjectDatabase*);

    static void findAllReferenceHolders(
        libfwbuilder::FWObject *obj, libfwbuilder::FWObject *root,
        std::map<int, std::set<libfwbuilder::FWObject*> > &res);
};

#endif // USAGERESOLVER_H
