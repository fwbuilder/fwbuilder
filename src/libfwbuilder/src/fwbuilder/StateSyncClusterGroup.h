/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id$


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

#ifndef __STATESYNCCLUSTERGROUP_HH_
#define __STATESYNCCLUSTERGROUP_HH_

#include "fwbuilder/ClusterGroup.h"

namespace libfwbuilder
{

    class StateSyncClusterGroup : public ClusterGroup
    {
    public:
        StateSyncClusterGroup();
        virtual ~StateSyncClusterGroup() {};

        DECLARE_FWOBJECT_SUBTYPE(StateSyncClusterGroup);

        DECLARE_DISPATCH_METHODS(StateSyncClusterGroup);
        
        virtual void fromXML(xmlNodePtr parent);
        virtual xmlNodePtr toXML(xmlNodePtr parent);
    };
}

#endif /* __CLUSTERGROUP_HH_ */

