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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/Cluster.h>
#include <fwbuilder/ClusterGroup.h>

#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWOptions.h>
#include <fwbuilder/Interface.h>
#include <fwbuilder/Management.h>
#include <fwbuilder/IPv4.h>
#include <fwbuilder/IPv6.h>

#include <fwbuilder/Policy.h>
#include <fwbuilder/NAT.h>

#include <fwbuilder/Routing.h>

#include <fwbuilder/RuleElement.h>

#include <fwbuilder/XMLTools.h>

using namespace std;
using namespace libfwbuilder;

const char *Cluster::TYPENAME = {"Cluster"};

Cluster::Cluster()
{
    setStr("platform", "unknown");
    setStr("host_OS", "unknown");
    setInt("lastModified" ,0);
    setInt("lastInstalled" ,0);
    setInt("lastCompiled" ,0);
}

Cluster::Cluster(const FWObjectDatabase *root, bool prepopulate)
    : Firewall(root, prepopulate)
{
    if (prepopulate)
    {
        // create one conntrack member group
        FWObject *members = getRoot()->create(ClusterGroup::TYPENAME);
        members->setName("Members (conntrack)");
        members->setStr("type", "conntrack");
        add(members);
    }
}

void Cluster::fromXML(xmlNodePtr root) throw(FWException)
{
    Firewall::fromXML(root);
}

xmlNodePtr Cluster::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = Firewall::toXML(parent);
    FWObject *o;
    for (FWObjectTypedChildIterator it = findByType(ClusterGroup::TYPENAME);
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

ClusterGroup* Cluster::getMembersObject()
{
    ClusterGroup *members = NULL;
    members = ClusterGroup::cast(getFirstByType(ClusterGroup::TYPENAME));

    if (members == NULL)
    {
        // create a new ClusterGroup object
        members = ClusterGroup::cast(getRoot()->create(ClusterGroup::TYPENAME));
        add(members);
    }
    return members;
}

bool Cluster::validateChild(FWObject *o)
{
    string otype=o->getTypeName();
    return (FWObject::validateChild(o) &&
            (otype==Interface::TYPENAME    ||
             otype==RuleSet::TYPENAME      ||
             otype==Policy::TYPENAME       ||
             otype==NAT::TYPENAME          ||
             otype==Routing::TYPENAME      ||
             otype==Management::TYPENAME   ||
             otype==ClusterGroup::TYPENAME ||
             otype==FirewallOptions::TYPENAME ));
}

FWObject& Cluster::duplicate(const FWObject *obj,
                             bool preserve_id) throw(FWException)
{
    Firewall::duplicate(obj, preserve_id);

    FWObject *o;
    for (FWObjectTypedChildIterator it =
             obj->findByType(ClusterGroup::TYPENAME); it != it.end(); ++it)
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
    setInt("lastInstalled", time(NULL));
}

void Cluster::updateLastModifiedTimestamp()
{
    setInt("lastModified", time(NULL));
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
    setInt("lastCompiled",time(NULL));
}

bool Cluster::getInactive()
{
    return getBool("inactive");
}

void Cluster::setInactive(bool b)
{
    setBool("inactive", b);
}

