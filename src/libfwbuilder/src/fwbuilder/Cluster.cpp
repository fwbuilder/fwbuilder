/*
 * Cluster.cpp - Cluster class implementation
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#include <time.h>
#include <assert.h>



#include "fwbuilder/Cluster.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"

#include "fwbuilder/Routing.h"

#include "fwbuilder/RuleElement.h"

#include "fwbuilder/XMLTools.h"
#include "fwbuilder/Resources.h"

using namespace std;
using namespace libfwbuilder;

const char *Cluster::TYPENAME = {"Cluster"};

Cluster::Cluster()
{
    setStr("platform", "unknown");
    setStr("host_OS", "unknown");
    setInt("lastModified", 0);
    setInt("lastInstalled", 0);
    setInt("lastCompiled", 0);
}

void Cluster::init(FWObjectDatabase *root)
{
    Firewall::init(root);
    // create one conntrack member group
    FWObject *state_sync_members = getFirstByType(StateSyncClusterGroup::TYPENAME);
    if (state_sync_members == nullptr)
    {
        state_sync_members = root->create(StateSyncClusterGroup::TYPENAME);
        state_sync_members->setName("State Sync Group");
        state_sync_members->setStr("type", "conntrack");
        add(state_sync_members);
    }
}

void Cluster::fromXML(xmlNodePtr root)
{
    Firewall::fromXML(root);
}

xmlNodePtr Cluster::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = Firewall::toXML(parent);
    FWObject *o;
    for (FWObjectTypedChildIterator it = findByType(StateSyncClusterGroup::TYPENAME);
        it != it.end(); ++it)
    {
        o = *it;
        if (o)
        {
            o->toXML(me);
        }
    }
    return me;
}

FWOptions* Cluster::getOptionsObject()
{
    return FWOptions::cast(getFirstByType(FirewallOptions::TYPENAME));
}

Policy* Cluster::getPolicy()
{
    return (Policy::cast(findObjectByName(Policy::TYPENAME, "Policy")));
}

NAT* Cluster::getNAT()
{
    return (NAT::cast(findObjectByName(NAT::TYPENAME, "NAT")));
}

Routing* Cluster::getRouting()
{
    return(Routing::cast(findObjectByName(Routing::TYPENAME, "Routing")));
}

ClusterGroup* Cluster::getStateSyncGroupObject()
{
    StateSyncClusterGroup *group = StateSyncClusterGroup::cast(
        getFirstByType(StateSyncClusterGroup::TYPENAME));

    if (group == nullptr)
    {
        // create a new ClusterGroup object
        group = StateSyncClusterGroup::cast(getRoot()->create(
                                                StateSyncClusterGroup::TYPENAME));
        add(group);
    }
    return group;
}

bool Cluster::validateChild(FWObject *o)
{
    string otype = o->getTypeName();
    return (FWObject::validateChild(o) &&
            (otype == Interface::TYPENAME    ||
             otype == RuleSet::TYPENAME      ||
             otype == Policy::TYPENAME       ||
             otype == NAT::TYPENAME          ||
             otype == Routing::TYPENAME      ||
             otype == Management::TYPENAME   ||
             otype == StateSyncClusterGroup::TYPENAME ||
             otype == FirewallOptions::TYPENAME));
}

FWObject& Cluster::duplicate(const FWObject *obj,
                             bool preserve_id)
{
    Firewall::duplicate(obj, preserve_id);

    FWObject *o;
    for (FWObjectTypedChildIterator it =
        obj->findByType(StateSyncClusterGroup::TYPENAME); it != it.end(); ++it)
    {
        o = *it;
        if (o)
        {
            addCopyOf(o, preserve_id);
        }
    }
    return *this;
}

void Cluster::updateLastInstalledTimestamp()
{
    setInt("lastInstalled", time(nullptr));
}

void Cluster::updateLastModifiedTimestamp()
{
    setInt("lastModified", time(nullptr));
}

bool Cluster::needsInstall()
{
    if (getLastInstalled() == 0 || getLastCompiled() == 0)
    {
        return true;
    }
    return !(getLastModified() <= getLastCompiled() &&
             getLastCompiled() <= getLastInstalled());
}

bool Cluster::needsCompile()
{
    return getLastModified() > getLastCompiled() || getLastCompiled() == 0;
}

time_t Cluster::getLastModified()
{
    return getInt("lastModified");
}

time_t Cluster::getLastInstalled()
{
    return getInt("lastInstalled");
}

time_t Cluster::getLastCompiled()
{
    return getInt("lastCompiled");
}

void Cluster::updateLastCompiledTimestamp()
{
    setInt("lastCompiled", time(nullptr));
}

bool Cluster::getInactive()
{
    return getBool("inactive");
}

void Cluster::setInactive(bool b)
{
    setBool("inactive", b);
}

bool Cluster::validateMember(Firewall *fw)
{
    string my_host_os = getStr("host_OS");
    string their_host_os = fw->getStr("host_OS");
    string my_platform = getStr("platform");
    string their_platform = fw->getStr("platform");
    if (my_host_os != their_host_os) return false;
    if (my_platform != their_platform) return false;
    if (! Resources::getTargetCapabilityBool(my_host_os, "supports_cluster"))
        return false;

    // Any other checks we should do ?

    return true;
}

/*
 * List members should contain each member firewall only once, but the
 * same member firewall can be present in several failover groups and
 * possibly in state synchronization group. Using set<int> to make the
 * list unique.
 */
void Cluster::getMembersList(list<libfwbuilder::Firewall*> &members)
{
    set<int> members_ids;
    list<FWObject*> all_groups = getByTypeDeep(StateSyncClusterGroup::TYPENAME);
    list<FWObject*> all_failover = getByTypeDeep(FailoverClusterGroup::TYPENAME);

    all_groups.merge(all_failover);

    for (list<FWObject*>::iterator it = all_groups.begin();
         it != all_groups.end(); ++it)
    {
        for (list<FWObject*>::iterator j = (*it)->begin();
             j != (*it)->end(); ++j)
        {
            FWObject *member = FWReference::getObject(*j);
            if (ClusterGroupOptions::isA(member)) continue;
            Firewall *fw = nullptr;
            // as of 05/04 members of StateSyncClusterGroup are interfaces. See
            // tickets #10 and #11
            if (Interface::cast(member))
                fw = Firewall::cast(Host::getParentHost(member));
            else
                fw = Firewall::cast(member);
            members_ids.insert(fw->getId());
        }
    }

    for (set<int>::iterator it = members_ids.begin();
         it != members_ids.end(); ++it)
    {
        Firewall *fw = Firewall::cast(getRoot()->findInIndex(*it));
        if (fw) members.push_back(fw);
    }
}

bool Cluster::hasMember(Firewall *fw)
{
    list<FWObject*> all_groups = getByTypeDeep(StateSyncClusterGroup::TYPENAME);
    list<FWObject*> all_failover = getByTypeDeep(FailoverClusterGroup::TYPENAME);

    all_groups.merge(all_failover);

    for (list<FWObject*>::iterator it = all_groups.begin();
         it != all_groups.end(); ++it)
    {
        for (list<FWObject*>::iterator j = (*it)->begin();
             j != (*it)->end(); ++j)
        {
            FWObject *member = FWReference::getObject(*j);
            if (ClusterGroupOptions::isA(member)) continue;
            Firewall *member_fw = nullptr;
            // as of 05/04/2009 members of StateSyncClusterGroup are
            // interfaces. See tickets #10 and #11
            if (Interface::cast(member))
                member_fw = Firewall::cast(Host::getParentHost(member));
            else
                member_fw = Firewall::cast(member);
            if (fw == member_fw) return true;
        }
    }
    return false;
}

