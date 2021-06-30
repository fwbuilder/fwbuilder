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

#include "AutomaticRules_pf.h"

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


void AutomaticRules_pf::addSshAccessRule()
{
    if (ruleset == nullptr) return;

    FWOptions *fwopt = fw->getOptionsObject();

    if (fwopt->getBool("mgmt_ssh") && ! fwopt->getStr("mgmt_addr").empty())
    {
        PolicyRule *r;
        TCPService *ssh = ruleset->getRoot()->createTCPService();
        ssh->setDstRangeStart(22);
        ssh->setDstRangeEnd(22);

        ssh->setName("mgmt_ssh");
        persistent_objects->add(ssh,false);

        string mgmt_addr = fwopt->getStr("mgmt_addr");
        InetAddr  addr;
        InetAddr netmask(InetAddr::getAllOnes());
        try
        {
            addr = InetAddr(mgmt_addr);
            string::size_type sep = mgmt_addr.find("/");
            if (sep != string::npos)
            {
                addr = InetAddr(mgmt_addr.substr(0,sep));
                string nm = mgmt_addr.substr(sep+1);
                int o1,o2,o3,o4;
                if (sscanf(nm.c_str(),
                           "%3u.%3u.%3u.%3u", &o1, &o2, &o3, &o4)==4)
                {
                    netmask = InetAddr(nm);
                } else
                {
                    sscanf(nm.c_str(),"%u",&o1);
                    netmask = InetAddr(o1);
                }
            }
        } catch(FWException &ex)
        {
            QString err("Invalid address for the backup ssh access: '%1'");
            throw FWException(err.arg(mgmt_addr.c_str()).toStdString());
        }

        Network *mgmt_workstation = ruleset->getRoot()->createNetwork();
        mgmt_workstation->setName("mgmt_addr");
        mgmt_workstation->setAddress(addr);
        mgmt_workstation->setNetmask(netmask);
        persistent_objects->add(mgmt_workstation,false);

        // r = ruleset->getRoot()->createPolicyRule();
        // ruleset->push_front(r);

        r = PolicyRule::cast(ruleset->insertRuleAtTop(true));

        r->setAction(PolicyRule::Accept);
        r->setLogging(false);
        r->setDirection(PolicyRule::Inbound);
        r->setPosition(9998);
        r->setComment("   backup ssh access rule ");
        r->setHidden(true);
        r->setFallback(false);
        r->setLabel("backup ssh access rule");
        r->setBool("needs_established",true);  // supported in ipfw

        RuleElement *src = r->getSrc();
        assert(src!=nullptr);
        src->addRef(mgmt_workstation);

        RuleElement *dst = r->getDst();
        assert(dst!=nullptr);
        dst->addRef(fw);

        RuleElement *srv = r->getSrv();
        assert(srv!=nullptr);
        srv->addRef(ssh);
    }
}

void AutomaticRules_pf::addCarpRules()
{
    if (ruleset == nullptr) return;

    /* Add CARP-Service to database */
    IPService* carp_service =
        IPService::cast(ruleset->getRoot()->create(IPService::TYPENAME));

    carp_service->setComment("CARP service");
    carp_service->setProtocolNumber(112);
    persistent_objects->add(carp_service);

    FWObjectTypedChildIterator interfaces = fw->findByType(Interface::TYPENAME);
    for (; interfaces != interfaces.end(); ++interfaces) 
    {
        Interface *iface = Interface::cast(*interfaces);

        if (iface->isFailoverInterface())
        {
            FWObject *failover_group =
                iface->getFirstByType(FailoverClusterGroup::TYPENAME);
            if (failover_group->getStr("type") == "carp")
            {
                /* Add automatic rules for CARP 
                 * Rule should be associated with physical interface
                 */
                string phys_iface_name =
                    iface->getOptionsObject()->getStr("base_device");

                Interface *phys_iface = Interface::cast(
                    fw->findObjectByName(Interface::TYPENAME, phys_iface_name));
                if (phys_iface)
                {
                    PolicyRule *rule = 
                        addMgmtRule(nullptr, nullptr, carp_service, phys_iface,
                                    PolicyRule::Both, PolicyRule::Accept, "CARP");
                    FWOptions *ruleopt = rule->getOptionsObject();
                    assert(ruleopt!=nullptr);
                    ruleopt->setBool("firewall_is_part_of_any_and_networks", false);
                } else
                {
                    throw FWException(
                        "Can not find interface " + phys_iface_name + 
                        " for the CARP interface " + iface->getName() +
                        " of the cluster");
                }
            }
        }
    }
}

void AutomaticRules_pf::addPfsyncRules()
{
    if (ruleset == nullptr) return;

    /* Add pfsync service to database */
    IPService* pfsync_service = IPService::cast(ruleset->getRoot()->create(IPService::TYPENAME));
    pfsync_service->setComment("pfsync service");
    pfsync_service->setProtocolNumber(240);
    persistent_objects->add(pfsync_service);

    FWObjectTypedChildIterator interfaces = fw->findByType(Interface::TYPENAME);
    for (; interfaces != interfaces.end(); ++interfaces) 
    {
        Interface *iface = Interface::cast(*interfaces);
        if (iface->getOptionsObject()->getBool("state_sync_group_member"))
        {
            FWObject *state_sync_group = ruleset->getRoot()->findInIndex(
                ruleset->getRoot()->getIntId(
                    iface->getOptionsObject()->getStr("state_sync_group_id")));
            assert(state_sync_group!=nullptr);
            if (state_sync_group && state_sync_group->getStr("type") == "pfsync")
            {
                PolicyRule *rule = addMgmtRule(nullptr, nullptr, pfsync_service, iface,
                                               PolicyRule::Both,
                                               PolicyRule::Accept, "pfsync");
                FWOptions *ruleopt = rule->getOptionsObject();
                assert(ruleopt!=nullptr);
                ruleopt->setBool("firewall_is_part_of_any_and_networks", false);
            }
        }
    }
}

void AutomaticRules_pf::addFallbackRule()
{
    if (ruleset == nullptr) return;

    FWOptions *fwopt = fw->getOptionsObject();

    PolicyRule *r = PolicyRule::cast(ruleset->appendRuleAtBottom(true));

    r->setUniqueId(
        ruleset->getRoot()->getPredictableId(
            FWObjectDatabase::getStringId(fw->getId()) + "."
        ));

    FWOptions *ruleopt;
    r->setAction(PolicyRule::Deny);
    r->setLogging(fwopt->getBool("fallback_log"));
    r->setDirection(PolicyRule::Both);
    r->setPosition(10000);
    r->setComment("   fallback rule ");
    r->setHidden(true);
    r->setFallback(true);
    r->setLabel("fallback rule");
    ruleopt = r->getOptionsObject();
    ruleopt->setBool("stateless", true);
}


