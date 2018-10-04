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

#include "AutomaticRules_cisco.h"

#include "fwbuilder/Address.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"

#include <QString>


using namespace fwcompiler;
using namespace libfwbuilder;
using namespace std;


void AutomaticRules_cisco::addSshAccessRule()
{
    if (ruleset == nullptr) return;

    FWOptions *fwopt = fw->getOptionsObject();

    if (fwopt->getBool("mgmt_ssh") && ! fwopt->getStr("mgmt_addr").empty())
    {
        TCPService *ssh = ruleset->getRoot()->createTCPService();
        ssh->setDstRangeStart(22);
        ssh->setDstRangeEnd(22);
        persistent_objects->add(ssh, false);

        TCPService *ssh_rev = ruleset->getRoot()->createTCPService();
        ssh_rev->setSrcRangeStart(22);
        ssh_rev->setSrcRangeEnd(22);
        persistent_objects->add(ssh_rev, false);

        Network *mgmt_workstation = ruleset->getRoot()->createNetwork();
        mgmt_workstation->setAddressNetmask(fwopt->getStr("mgmt_addr"));

        persistent_objects->add(mgmt_workstation, false);

        addMgmtRule(
            mgmt_workstation, fw, ssh,
            nullptr, PolicyRule::Inbound, PolicyRule::Accept,
            "backup ssh access rule");
    }
}

