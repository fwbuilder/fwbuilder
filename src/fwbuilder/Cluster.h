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

#include <fwbuilder/Firewall.h>

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
        Cluster(const FWObjectDatabase *root, bool prepopulate);
        virtual ~Cluster() {};

        virtual void fromXML(xmlNodePtr parent) throw(FWException);
        virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);

        DECLARE_FWOBJECT_SUBTYPE(Cluster);

        /**
         * verify whether given object type is approppriate as a child
         */
        virtual bool validateChild(FWObject *o);

        virtual FWOptions* getOptionsObject();

        virtual FWObject& duplicate(const FWObject *obj,
                                    bool preserve_id = true) throw(FWException);

        Policy*  getPolicy();
        NAT*     getNAT();
        Routing* getRouting();

        /**
         * return attached cluster group members object.
         *
         * if the object is not found, a new one is created.
         *
         * @return assigned cluster group members object
         */
        virtual ClusterGroup* getMembersObject();

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

