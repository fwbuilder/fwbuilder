/*
 * Cluster.h - Cluster class header file
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

#ifndef __CLUSTER_HH__
#define __CLUSTER_HH__

#include "fwbuilder/Firewall.h"

namespace libfwbuilder
{
    class Policy;
    class NAT;
    class RuleSet;
    class Routing;

    class ClusterGroup;

    class Cluster : public Firewall
    {

    public:

        Cluster();
        virtual ~Cluster() {};

        /**
         * This method should create any standard mandatory child objects
         * the object might need.
         */
        virtual void init(FWObjectDatabase *root);
    
        virtual void fromXML(xmlNodePtr parent);
        virtual xmlNodePtr toXML(xmlNodePtr parent);

        DECLARE_FWOBJECT_SUBTYPE(Cluster);

        DECLARE_DISPATCH_METHODS(Cluster);

        /**
         * verify whether given object type is approppriate as a child
         */
        virtual bool validateChild(FWObject *o);

        virtual FWOptions* getOptionsObject();

        virtual FWObject& duplicate(const FWObject *obj,
                                    bool preserve_id = true);

        Policy*  getPolicy();
        NAT*     getNAT();
        Routing* getRouting();

        /**
         * return attached cluster group members object that
         * supports state synchronization (e.g. conntrackd).
         *
         * if the object is not found, a new one is created.
         *
         * @return assigned cluster group members object
         */
        virtual ClusterGroup* getStateSyncGroupObject();

        /**
         * Build a list of member firewalls. This method scans all
         * failover groups as well as state sync groups of the
         * cluster. Upon return, each firewall object appears in the
         * list exactly once.
         */
        virtual void getMembersList(std::list<libfwbuilder::Firewall*> &members);

        /**
         * Check validity of a given member. This is where we
         * implement logic to check if host OS of the cluster and
         * members is the same and possibly other criteria.
         */
        virtual bool validateMember(libfwbuilder::Firewall*);

        /**
         * check if @fw is a member of this cluster.
         */
        bool hasMember(libfwbuilder::Firewall *fw);

        time_t getLastModified();
        time_t getLastInstalled();
        time_t getLastCompiled();
        void   updateLastInstalledTimestamp();
        void   updateLastModifiedTimestamp();
        void   updateLastCompiledTimestamp();
        bool   needsInstall();
        bool   needsCompile();
        bool   getInactive();
        void   setInactive(bool b);
    };
}

#endif /* __CLUSTER_HH__ */

