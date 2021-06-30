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

#include "AutomaticRules_ipt.h"

#include "fwbuilder/Address.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/IPService.h"

#include <QString>


using namespace fwcompiler;
using namespace libfwbuilder;
using namespace std;



PolicyRule* AutomaticRules_ipt::addMgmtRule(
    Address* src, Address* dst, Service* service, Interface* iface,
    const PolicyRule::Direction direction,
    const PolicyRule::Action action,
    const string &label,
    bool related)
{
    PolicyRule *rule = AutomaticRules::addMgmtRule(src, dst, service,
                                                   iface, direction, action,
                                                   label);

    FWOptions *ruleopt = rule->getOptionsObject(); assert(ruleopt!=nullptr);
    if (related)
    {
        ruleopt->setBool("stateless", false);
        ruleopt->setBool("accept_established", true);
    } else
    {
        ruleopt->setBool("stateless", true);
    }
    ruleopt->setBool("firewall_is_part_of_any_and_networks", true);

    return rule;
}

void AutomaticRules_ipt::addConntrackRule()
{
    if (ruleset == nullptr) return;

    FWOptions* options = fw->getOptionsObject();
    string conntrack_iface_name = options->getStr("state_sync_interface");
    if (conntrack_iface_name.empty())
    {
        /* CONNTRACK not active, nothing left to do */
        return;
    }

    string conntrack_group_id = options->getStr("state_sync_group_id");
    StateSyncClusterGroup *state_sync_group = 
        StateSyncClusterGroup::cast(
            ruleset->getRoot()->findInIndex(
                FWObjectDatabase::getIntId(conntrack_group_id)));

    Resources *os_res = Resources::os_res[fw->getStr("host_OS")];
    assert(os_res != nullptr);

    string default_address =
        os_res->getResourceStr("/FWBuilderResources/Target/protocols/conntrack/default_address");
    string default_port =
        os_res->getResourceStr("/FWBuilderResources/Target/protocols/conntrack/default_port");

    bool ucast = state_sync_group->getOptionsObject()->getBool("conntrack_unicast");
    string addr = state_sync_group->getOptionsObject()->getStr("conntrack_address");
    if (addr.empty()) addr = default_address;

    try
    {
        (void) InetAddr(addr);
    } catch (FWException &ex)
    {
        try
        {
            InetAddr(AF_INET6, addr);
        } catch (FWException &ex)
        {
            throw FWException(string("Invalid IP address for conntrack: ") + addr);
        }
    }

    string port = state_sync_group->getOptionsObject()->getStr("conntrack_port");
    if (port.empty()) port = default_port;

    /* Add CONNTRACK-Address to database */
    Address *conntrack_dst = Address::cast(ruleset->getRoot()->create(IPv4::TYPENAME));
    conntrack_dst->setName("CONNTRACK-Address");
    conntrack_dst->setAddress(InetAddr(addr));
    // Why the whole multicast adress range ?
    //conntrack_dst->setNetmask(InetAddr("240.0.0.0"));
    conntrack_dst->setComment("CONNTRACK Multicast Address");
    persistent_objects->add(conntrack_dst);

    UDPService *conntrack_srv = UDPService::cast(ruleset->getRoot()->create(UDPService::TYPENAME));
    conntrack_srv->setName("CONNTRACK-UDP");
    conntrack_srv->setDstRangeStart(atoi(port.c_str()));
    conntrack_srv->setDstRangeEnd(atoi(port.c_str()));
    conntrack_srv->setComment("CONNTRACK UDP port");
    persistent_objects->add(conntrack_srv);

    /* Find conntrack interface */
    Interface* conntrack_iface = Interface::cast(fw->findObjectByName(Interface::TYPENAME, conntrack_iface_name));

    if (conntrack_iface == nullptr)
    {
        throw FWException(
            "Unable to get CONNTRACK interface ("+ conntrack_iface_name +")");
    }

    /* Add automatic rules for CONNTRACK */
    if (ucast)
    {
        Interface *fw_iface = nullptr;
        list<Interface*> other_interfaces;
        for (FWObjectTypedChildIterator it =
                 state_sync_group->findByType(FWObjectReference::TYPENAME);
             it != it.end(); ++it)
        {
            Interface *iface =
                Interface::cast(FWObjectReference::getObject(*it));
            assert(iface);
            if (iface->isChildOf(fw))
            {
                fw_iface = iface;
            } else
            {
                other_interfaces.push_back(iface);
            }
        }

        for (list<Interface*>::iterator it=other_interfaces.begin(); it!=other_interfaces.end(); ++it)
        {
            Interface *other_iface = *it;

            addMgmtRule(other_iface,
                        fw,
                        conntrack_srv,
                        fw_iface,
                        PolicyRule::Inbound,
                        PolicyRule::Accept,
                        "CONNTRACK");
            addMgmtRule(fw,
                        other_iface,
                        conntrack_srv,
                        fw_iface,
                        PolicyRule::Outbound,
                        PolicyRule::Accept,
                        "CONNTRACK");
        }
    } else
    {
        addMgmtRule(nullptr,
                    conntrack_dst,
                    conntrack_srv,
                    conntrack_iface,
                    PolicyRule::Inbound,
                    PolicyRule::Accept,
                    "CONNTRACK");

        addMgmtRule(fw,
                    conntrack_dst,
                    conntrack_srv,
                    conntrack_iface,
                    PolicyRule::Outbound,
                    PolicyRule::Accept,
                    "CONNTRACK");
    }
}

void AutomaticRules_ipt::addFailoverRules()
{
    if (ruleset == nullptr) return;

    Resources *os_res = Resources::os_res[fw->getStr("host_OS")];
    assert(os_res != nullptr);

    string default_heartbeat_port =
        os_res->getResourceStr(
            "/FWBuilderResources/Target/protocols/heartbeat/default_port");
    string default_heartbeat_address =
        os_res->getResourceStr(
            "/FWBuilderResources/Target/protocols/heartbeat/default_address");
    string default_openais_port =
        os_res->getResourceStr(
            "/FWBuilderResources/Target/protocols/openais/default_port");
    string default_openais_address =
        os_res->getResourceStr(
            "/FWBuilderResources/Target/protocols/openais/default_address");

    list<FWObject*> interfaces = fw->getByTypeDeep(Interface::TYPENAME);
    interfaces.sort(FWObjectNameCmpPredicate());
    list<FWObject*>::iterator iface_i;
    for (iface_i=interfaces.begin(); iface_i != interfaces.end(); ++iface_i)
    {
        Interface *iface = Interface::cast(*iface_i);

        /*
          We add copies of cluster interface objects to fw objects
          so each interface appears twice, the original interface
          of the firewall, plus a copy of the cluster
          interface. To deduplicate will use only copies of
          cluster interfaces because these include VRRP interfaces.
        */

        if (iface->isFailoverInterface() &&
            iface->getOptionsObject()->getBool("cluster_interface"))
        {
            FWObject *failover_group =
                iface->getFirstByType(FailoverClusterGroup::TYPENAME);

            PolicyRule *rule = nullptr;

            string fw_iface_id = iface->getOptionsObject()->getStr("base_interface_id");
            Interface *fw_iface =
                Interface::cast(
                    ruleset->getRoot()->findInIndex(FWObjectDatabase::getIntId(fw_iface_id)));
            if (fw_iface == nullptr)
            {
                throw FWException(
                    QString("Can not find interface of the firewall "
                            "for the cluster failover group %1. ")
                    .arg(failover_group->getName().c_str()).toStdString());

            }

            if (failover_group->getStr("type") == "vrrp")
            {
                /* Add VRRP-Address to database */
                Address *vrrp_dst = Address::cast(
                    ruleset->getRoot()->create(IPv4::TYPENAME));

                vrrp_dst->setName("VRRP-Address");
                vrrp_dst->setAddress(InetAddr("224.0.0.18"));
                vrrp_dst->setNetmask(InetAddr(InetAddr::getAllOnes()));
                vrrp_dst->setComment("VRRP Multicast Address");
                persistent_objects->add(vrrp_dst);

                bool use_ipsec_ah = false;

                FWOptions *failover_opts =
                    FailoverClusterGroup::cast(failover_group)->getOptionsObject();
                if (failover_opts)
                {
                    use_ipsec_ah = failover_opts->getBool("vrrp_over_ipsec_ah");
                }

                /* Add VRRP-Service to database */
                IPService* vrrp_srv = IPService::cast(
                    ruleset->getRoot()->create(IPService::TYPENAME));
                vrrp_srv->setComment("VRRP service");
                vrrp_srv->setProtocolNumber(112);
                persistent_objects->add(vrrp_srv);

                /*
                 * Add AH-Service to database.
                 * According to RFC 2338 section 5.3.6.3, VRRP can use
                 * IPsec AH.
                 */
                IPService* ah_srv = IPService::cast(
                    ruleset->getRoot()->create(IPService::TYPENAME));
                ah_srv->setComment("IPSEC-AH");
                ah_srv->setProtocolNumber(51);
                persistent_objects->add(ah_srv);

                for (FWObjectTypedChildIterator it =
                         failover_group->findByType(FWObjectReference::TYPENAME);
                     it != it.end(); ++it)
                {
                    Interface *other_iface =
                        Interface::cast(FWObjectReference::getObject(*it));
                    assert(other_iface);
                    if (other_iface->getId() == fw_iface->getId()) continue;
                    // if interface is dynamic, we can't use it in the rule
                    // (because it belongs to another machine, not the fw
                    // we compile for so we can't use script). nullptr means "any"
                    // in the call to addMgmtRule()
                    if (other_iface->isDyn()) other_iface = nullptr;

                    if (!use_ipsec_ah)
                    {
                        addMgmtRule(other_iface, vrrp_dst, vrrp_srv, iface,
                                    PolicyRule::Inbound, PolicyRule::Accept,
                                    "VRRP");
                    } else
                    {
                        addMgmtRule(other_iface, vrrp_dst, ah_srv, iface,
                                    PolicyRule::Inbound, PolicyRule::Accept,
                                    "VRRP (with IPSEC-AH)");
                    }
                }
                // outbound rule does not use other_interface and
                // should be created outside the loop to avoid
                // duplicates. Duplicates happen when cluster has 3 or
                // more members.
                if (!use_ipsec_ah)
                {
                    addMgmtRule(fw, vrrp_dst, vrrp_srv, iface,
                                PolicyRule::Outbound, PolicyRule::Accept,
                                "VRRP");
                } else
                {
                    addMgmtRule(fw, vrrp_dst, ah_srv, iface,
                                PolicyRule::Outbound, PolicyRule::Accept,
                                "VRRP (with IPSEC-AH)");
                }
            }

            if (failover_group->getStr("type") == "heartbeat")
            {
                /*
                 * Note that iface is a copy of the cluster inetrface.
                 * Find interface of the member firewall fw that corresponds
                 * to the cluster interface iface
                 */

                bool ucast = FailoverClusterGroup::cast(failover_group)->
                    getOptionsObject()->getBool("heartbeat_unicast");

                string addr = FailoverClusterGroup::cast(failover_group)->
                    getOptionsObject()->getStr("heartbeat_address");
                if (addr.empty()) addr = default_heartbeat_address;

                string port = FailoverClusterGroup::cast(failover_group)->
                    getOptionsObject()->getStr("heartbeat_port");
                if (port.empty()) port = default_heartbeat_port;

                UDPService *heartbeat_srv = UDPService::cast(
                    ruleset->getRoot()->create(UDPService::TYPENAME));

                /* Add heartbeat-Address to database */
                Address *heartbeat_dst = Address::cast(ruleset->getRoot()->create(
                                                           IPv4::TYPENAME));
                heartbeat_dst->setName("HEARTBEAT-Address");
                heartbeat_dst->setAddress(InetAddr(addr));
                heartbeat_dst->setNetmask(InetAddr(InetAddr::getAllOnes()));
                heartbeat_dst->setComment("HEARTBEAT Multicast Address");
                persistent_objects->add(heartbeat_dst);

                heartbeat_srv->setName("HEARTBEAT-UDP");
                heartbeat_srv->setDstRangeStart(atoi(port.c_str()));
                heartbeat_srv->setDstRangeEnd(atoi(port.c_str()));
                heartbeat_srv->setComment("HEARTBEAT UDP port");
                persistent_objects->add(heartbeat_srv);

                // Heartbeat can use either multicast or unicast
                for (FWObjectTypedChildIterator it =
                         failover_group->findByType(FWObjectReference::TYPENAME);
                     it != it.end(); ++it)
                {
                    Interface *other_iface =
                        Interface::cast(FWObjectReference::getObject(*it));
                    assert(other_iface);
                    if (other_iface->getId() == fw_iface->getId()) continue;
                    // if interface is dynamic, we can't use it in the rule
                    // (because it belongs to another machine, not the fw
                    // we compile for so we can't use script). nullptr means "any"
                    // in the call to addMgmtRule()
                    if (other_iface->isDyn()) other_iface = nullptr;

                    if (ucast)
                    {
                        addMgmtRule(other_iface, fw, heartbeat_srv, fw_iface,
                                    PolicyRule::Inbound, PolicyRule::Accept,
                                    "heartbeat");
                        addMgmtRule(fw, other_iface, heartbeat_srv, fw_iface,
                                    PolicyRule::Outbound, PolicyRule::Accept,
                                    "heartbeat");
                    }
                    else
                    {
                        addMgmtRule(other_iface, heartbeat_dst, heartbeat_srv, fw_iface,
                                    PolicyRule::Inbound, PolicyRule::Accept,
                                    "heartbeat");
                        addMgmtRule(fw, heartbeat_dst, heartbeat_srv, fw_iface,
                                    PolicyRule::Outbound, PolicyRule::Accept,
                                    "heartbeat");
                    }
                }
            }

            if (failover_group->getStr("type") == "openais")
            {
                string addr = FailoverClusterGroup::cast(failover_group)->
                    getOptionsObject()->getStr("openais_address");
                if (addr.empty()) addr = default_openais_address;

                string port = FailoverClusterGroup::cast(failover_group)->
                    getOptionsObject()->getStr("openais_port");
                if (port.empty()) port = default_openais_port;

                /* Add OPENAIS-Address to database */
                Address *openais_dst = Address::cast(ruleset->getRoot()->create(
                                                         IPv4::TYPENAME));
                openais_dst->setName("OPENAIS-Address");
                openais_dst->setAddress(InetAddr(addr));
                openais_dst->setNetmask(InetAddr(InetAddr::getAllOnes()));
                openais_dst->setComment("OPENAIS Multicast Address");
                persistent_objects->add(openais_dst);

                UDPService *openais_srv = UDPService::cast(
                    ruleset->getRoot()->create(UDPService::TYPENAME));

                openais_srv->setName("OPENAIS-UDP");
                openais_srv->setDstRangeStart(atoi(port.c_str()));
                openais_srv->setDstRangeEnd(atoi(port.c_str()));
                openais_srv->setComment("OPENAIS UDP port");
                persistent_objects->add(openais_srv);

                for (FWObjectTypedChildIterator it =
                         failover_group->findByType(FWObjectReference::TYPENAME);
                     it != it.end(); ++it)
                {
                    Interface *other_iface =
                        Interface::cast(FWObjectReference::getObject(*it));
                    assert(other_iface);
                    if (other_iface->getId() == fw_iface->getId()) continue;
                    // if interface is dynamic, we can't use it in the rule
                    // (because it belongs to another machine, not the fw
                    // we compile for so we can't use script). nullptr means "any"
                    // in the call to addMgmtRule()
                    if (other_iface->isDyn()) other_iface = nullptr;

                    addMgmtRule(other_iface, openais_dst, openais_srv, iface,
                                PolicyRule::Inbound, PolicyRule::Accept,
                                "openais");
                    addMgmtRule(fw, openais_dst, openais_srv, iface,
                                PolicyRule::Outbound, PolicyRule::Accept,
                                "openais");
                }
            }

            if (rule)
            {
                FWOptions *ruleopt = rule->getOptionsObject();
                assert(ruleopt!=nullptr);
                ruleopt->setInt("firewall_is_part_of_any_and_networks", 1);
            }
        }
    }
}

