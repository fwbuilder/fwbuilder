/* 

  $Id$
 
  Copyright (c) 2008 secunet Security Networks AG
  Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
  Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 
  This work is dual-licensed under:
 
  o The terms of the GNU General Public License as published by the Free
    Software Foundation, either version 2 of the License, or (at your option)
    any later version.
 
  o The terms of NetCitadel End User License Agreement

*/

#include "Compiler.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"


#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <stdexcept>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

/*
 * Verify that there is at least one Cluster interface and that all
 * have unique names and IP addresses.
 */
int Compiler::checkCluster(Cluster* cluster)
{
    assert(cluster != NULL);

    bool incorrect_cluster = false;
    FWObjectTypedChildIterator cluster_ifaces = cluster->findByType(Interface::TYPENAME);
    if (cluster_ifaces == cluster_ifaces.end())
    {
        /* No configured cluster interface present */
        cerr << "No configured cluster interface present." << endl;
        incorrect_cluster = true;
    }

    for (; cluster_ifaces != cluster_ifaces.end(); ++cluster_ifaces)
    {
        string iface_name = Interface::cast(*cluster_ifaces)->getName();
        const InetAddr* iface_address = Interface::cast(*cluster_ifaces)->getAddressPtr();
        FWObjectTypedChildIterator other_ifaces = cluster_ifaces;
        for (++other_ifaces; other_ifaces != cluster_ifaces.end(); ++other_ifaces)
        {
            if (iface_name == Interface::cast(*other_ifaces)->getName())
            {
                cout << " Warning: found duplicate cluster interface name " << iface_name << "." << endl;
                incorrect_cluster = true;
            }
            if (*iface_address == *(Interface::cast(*other_ifaces)->getAddressPtr()))
            {
                cout << " Warning: found duplicate cluster interface address ";
                cout << iface_address->toString() << "." << endl;
                incorrect_cluster = true;
            }
        }
    }
    if (incorrect_cluster)
    {
        cerr << "Incorrect cluster configuration found." << endl;
        exit(1);
    }
    return 0;
}

bool Compiler::isReachable(const Address* const client,
                           const InetAddr* const server)
{
    const InetAddr *addr = client->getAddressPtr();
    const InetAddr *netm = client->getNetmaskPtr();
    if (addr)
    {
        InetAddrMask fw_net(*addr, *netm);
        if (fw_net.belongs(*server))
            return true;
    }
    return false;
}

/*
 * Perform checks for fialover interfaces and their addresses, add a
 * copy of failover interface form the cluster to the firewall object.
 *
 * This method assumes the following:
 *
 * - Failover interface owns its ip address which is different from
 *   addresses of either firewall
 *
 * - address of the failover interface must be on the same subnet as
 *   addresses of the firewalls (perhaps this restriction can be
 *   lifted? Was originally implemented by Secunet folks like this)
 */
void Compiler::processFailoverGroup(Cluster *cluster,
                                    Firewall *fw,
                                    FailoverClusterGroup *cluster_group,
                                    Interface *iface)
{
    Interface* cluster_if = Interface::cast(cluster_group->getParent());
    assert(cluster_if != NULL);
    string cluster_if_name = cluster_if->getName();

    /* Check that VRRP interface and fw interface are in same subnet.
     * Exception: if interface is dynamic and does not have an ip address in
     * fwbuilder configuration, assume it is ok.
     */
    if (iface->isRegular())
    {
        const Address *iface_addr = iface->getAddressObject();
        // even regular interface may have no address if user forgot to add one
        if (iface_addr && !isReachable(cluster_if->getAddressObject(),
                                       iface_addr->getAddressPtr()))
        {
            cerr << " Warning: "
                 << cluster_if_name
                 << " and "
                 << iface->getName() 
                 << " are not in the same subnet." << endl;
        }
    }

    assert(fw->getOptionsObject() != NULL);

    /* Verify uniqueness of cluster interface name.  Firewall object
     * should not have interface with the same name as cluster.
     */
    FWObjectTypedChildIterator current_ifaces = fw->findByType(Interface::TYPENAME);
    for (; current_ifaces != current_ifaces.end(); ++current_ifaces)
    {
        if (cluster_if_name ==  (*current_ifaces)->getName())
        {
            /* Fatal missconfiguration */
            /* TODO: Find more elegant way to bailout */
            cerr << " Error: multiple cluster interfaces with same name ("
                 << cluster_if_name;
            cerr << ") found." << endl;
            exit(1);
        }
    }

    /* Add copy of VRRP interface from the cluster to the firewall object */
    Interface* new_cl_if = Interface::cast(fw->addCopyOf(cluster_if, false));
    assert(new_cl_if != NULL);
    new_cl_if->getOptionsObject()->setStr("base_device", iface->getName());

    /* Set master property if interface is referenced
     * as master_iface
     */
    string master_id = cluster_group->getStr("master_iface");
    string iface_str_id = FWObjectDatabase::getStringId(iface->getId());

    new_cl_if->getOptionsObject()->setBool("failover_master",
                                           master_id == iface_str_id);
    
    fw->getOptionsObject()->setBool("cluster_member", true);

    /* Add copy of firewall's real interface to the cluster to make sure
     * compiler recognizes it when it encounters cluster object in rules.
     * This fixes #15 (makes compiler choose correct chains)
     */
    cluster->addCopyOf(iface, false);
}

/*
 * 1. Iterate over all fw interfaces and check if they are referenced in a
 *    ClusterGroup.
 *    -> if yes then make copy of vrrp interface and set BASEDEV accordingly
 * 2. clear Policy, NAT & Routing rules of the firewall, then copy cluster
 *    policy, NAT and routing rules.
 */
int Compiler::populateClusterElements(Cluster *cluster, Firewall *fw)
{
    int addedPolicies = 0;
    set<string> state_sync_types;

    checkCluster(cluster);
    
    for (FWObjectTypedChildIterator it = cluster->findByType(StateSyncClusterGroup::TYPENAME);
        it != it.end(); ++it)
    {
        FWObject *state_sync_group = *it;
        /* For the state syncing cluster group, hierarchy looks like this:
         * Cluster->StateSyncClusterGroup->ObjectRef
         */
        string grp_type = state_sync_group->getStr("type");
        if (state_sync_types.count(grp_type) > 0)
            abort("Several state synchronization groups of the same type in one cluster object.");

        state_sync_types.insert(grp_type);
    }

    /* For VRRP references the hierarchy is as follows:
     * Cluster->Interface->FailoverClusterGroup->ObjectRef
     */
    FWObjectTypedChildIterator cl_iface = cluster->findByType(Interface::TYPENAME);
    for (; cl_iface != cl_iface.end(); ++cl_iface)
    {
        FailoverClusterGroup *failover_group =
            FailoverClusterGroup::cast(
                (*cl_iface)->getFirstByType(FailoverClusterGroup::TYPENAME));
        if (failover_group)
        {
            for (FWObjectTypedChildIterator it =
                     failover_group->findByType(FWObjectReference::TYPENAME);
                 it != it.end(); ++it)
            {
                Interface *iface = Interface::cast(FWObjectReference::getObject(*it));
                assert(iface);
                if (iface->getParent() == fw)
                {
                    // We need to do some sanity checks of cluster
                    // interfaces for VRRP and then add them to the
                    // firewall object.
                    // These actions are very generic and have nothing specific
                    // to VRRP. Unless new protocol is added that requires
                    // something radically different, will always call this method
                    // for failover groups.
                    //if (failover_group->getStr("type") == "vrrp")
                    processFailoverGroup(cluster, fw, failover_group, iface);
                }
            }
        }
    }
    
    int fw_direct_rules_count = fw->getPolicy()->size() +
        fw->getNAT()->size() + fw->getRouting()->size();
    
    if (fw_direct_rules_count > 0)
    {
        cout << "Warning: ignoring firewall policy (" << fw->getName();
        cout << ") since firewall is a cluster member." << endl;
    }

    fw->getPolicy()->clear();
    fw->getNAT()->clear();
    fw->getRouting()->clear();

// Copy PolicyRules from the cluster.
    cout << endl;
    cout << " Firewall " << fw->getName()
         << " member of cluster " << cluster->getName() << endl;

    /* Policy rules */
    Policy* cluster_policy = cluster->getPolicy();
    Policy* fw_policy = fw->getPolicy();
    if (cluster_policy)
    {
        cluster_policy->setName("Cluster-Policy");
        for(int i = 0; i < cluster_policy->getRuleSetSize(); i++)
        {
            /* Add rule to firewall policy */
            PolicyRule* rule = PolicyRule::cast(cluster_policy->getRuleByNum(i));
            PolicyRule::cast(fw_policy->addCopyOf(rule, false));
            addedPolicies++;
        }
    }

    /* NAT rules */
    NAT* cluster_nat = cluster->getNAT();
    NAT* fw_nat = fw->getNAT();
    if (cluster_nat)
    {
        cluster_nat->setName("Cluster-NAT");
        for (int i = 0; i < cluster_nat->getRuleSetSize(); i++)
        {
            /* Add rule to firewall policy */
            NATRule* rule = NATRule::cast(cluster_nat->getRuleByNum(i));
            NATRule::cast(fw_nat->addCopyOf(rule, false));
            addedPolicies++;
        }
    }

    /* Routing rules */
    Routing* cluster_routes = cluster->getRouting();
    Routing* fw_routes = fw->getRouting();
    if (cluster_routes)
    {
        cluster_routes->setName("Cluster-Routing");
        for(int i = 0; i < cluster_routes->getRuleSetSize(); i++)
        {
            /* Add rule to firewall policy */
            RoutingRule* rule = RoutingRule::cast(cluster_routes->getRuleByNum(i));
            RoutingRule::cast(fw_routes->addCopyOf(rule, false));
            addedPolicies++;
        }
    }

    // finally need to remember cluster object ID so that compiler can later
    // associate it in rules with the firewall.
    //
    // The alternative is to find all references to the cluster object
    // in rules and replace them with refs to the firewall. That could
    // be done either in prolog or in a special rule processor. It is
    // _much_ cheaper to just remember cluster ID though.
    fw->setInt("parent_cluster_id", cluster->getId());

    return addedPolicies;
}

