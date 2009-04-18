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
 */

#ifndef __CLUSTERGROUP_HH_
#define __CLUSTERGROUP_HH_

#include <fwbuilder/ObjectGroup.h>

namespace libfwbuilder
{

    class ClusterGroup : public ObjectGroup
    {
public:
        ClusterGroup();
        ClusterGroup(const FWObjectDatabase *root, bool prepopulate);
        virtual ~ClusterGroup() {};

        DECLARE_FWOBJECT_SUBTYPE(ClusterGroup);

        virtual void fromXML(xmlNodePtr parent) throw(FWException);
        virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);

        /*
         * verify whether given object type is approppriate as a child
         */
        virtual bool validateChild(FWObject *o);
    };
}

#endif /* __CLUSTERGROUP_HH_ */

