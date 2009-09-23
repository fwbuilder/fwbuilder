/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#include "../../config.h"
#include "../../build_num"

#include <assert.h>

#include "CompilerDriver_ipt.h"

#include "fwbuilder/Cluster.h"
#include "fwbuilder/ClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/StateSyncClusterGroup.h"

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;



/**
 * Do something with state sync cluster groups. Currently we only know
 * about conntrack groups. For these, we find interfaces that were
 * placed in the group and store the name in the variable
 * "conntrack_interface" which is used later in insertConntrackRule
 * to associate this rule with right interface
 */
void CompilerDriver_ipt::processStateSyncGroups(Cluster *cluster,
                                                Firewall *member_fw)
{
    for (FWObjectTypedChildIterator it = cluster->findByType(StateSyncClusterGroup::TYPENAME);
         it != it.end(); ++it)
    {
        FWObject *state_sync_group = *it;
        // there is only one supported state sync procol for Linux,
        // conntrack. So if the groups exits but the type is empty
        // (as happens when the group has just been created), assume it is
        // conntrack.
        if (state_sync_group->getStr("type") == "conntrack" ||
            state_sync_group->getStr("type").empty())
        {
            for (FWObjectTypedChildIterator grp_it =
                     state_sync_group->findByType(FWObjectReference::TYPENAME);
                 grp_it != grp_it.end(); ++grp_it)
            {
                FWObject *iface = FWObjectReference::getObject(*grp_it);
                if (iface->isChildOf(member_fw))
                {
                    member_fw->getOptionsObject()->setStr(
                        "conntrack_group_id",
                        FWObjectDatabase::getStringId(state_sync_group->getId()));

                    member_fw->getOptionsObject()->setStr(
                        "conntrack_interface",
                        iface->getName());
                    break;
                }
            }
        }
    }
}

