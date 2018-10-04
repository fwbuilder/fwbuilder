/*
 * ClusterGroup.h - Class to group cluster interfaces
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
 *
 *
 * Class ClusterGroup serves as a base class for StateSyncClusterGroup and
 * FailoverClusterGroup. Objects of the class ClusterGroup are never used
 * and can not be stored in XML file.
 */

#ifndef __CLUSTERGROUP_HH_
#define __CLUSTERGROUP_HH_

#include "fwbuilder/ObjectGroup.h"

namespace libfwbuilder
{
    class ClusterGroupOptions;

    class ClusterGroup : public ObjectGroup
    {
protected:

        virtual void replaceReferenceInternal(int oldfw_id, int newfw_id, int &counter);

public:
        ClusterGroup();
        virtual ~ClusterGroup() {};

        /**
         * This method should create any standard mandatory child objects
         * the object might need.
         */
        virtual void init(FWObjectDatabase *root);
    
        DECLARE_FWOBJECT_SUBTYPE(ClusterGroup);

        DECLARE_DISPATCH_METHODS(ClusterGroup);

        virtual void fromXML(xmlNodePtr parent);
        virtual xmlNodePtr toXML(xmlNodePtr parent);

        /*
         * verify whether given object type is approppriate as a child
         */
        virtual bool validateChild(FWObject *o);

        virtual ClusterGroupOptions* getOptionsObject();

        /**
         * This method copies all attributes of obj plus all reference
         * child objects and the options object to reproduce accurate
         * state of this.
         */
        virtual FWObject& duplicateForUndo(const FWObject *obj);

        /**
         * If @this is cluster interface and it is correctly
         * configured with member interfaces, this method returns
         * pointer to interface that belongs to the given member
         * firewall. Otherwise it returns nullptr.
         */
        Interface* getInterfaceForMemberFirewall(Firewall *fw);
    };
}

#endif /* __CLUSTERGROUP_HH_ */

