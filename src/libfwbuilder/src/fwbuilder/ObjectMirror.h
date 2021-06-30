/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: ObjectMirror.h 534 2010-02-07 23:25:06Z vadim $


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

#ifndef  __OBJECTMIRROR_HH_FLAG__
#define  __OBJECTMIRROR_HH_FLAG__

#include <string>
#include <list>

#include "fwbuilder/Dispatch.h"


namespace libfwbuilder
{

    class ObjectMirror : public Dispatch {

public:
        ObjectMirror() : Dispatch() { }

        /**
         * Creates "mirrored" object (currently only
         * service). Mirroring means swapping source and destination
         * port ranges for tcp and udp and picking appropriate ICMP
         * type for some known ICMP services. The object it returns is
         * not added to the tree, caller must do this in order to
         * avoid memory leaks and to be able to use the object later.
         *
         * Note: mirroring of actual rules requires knowledge of the
         * firewall platform specifics, such as what set of rule
         * elements is supported and their semantics (e.g. is time
         * supported ? If yes, then what does it mean to mirror the
         * rule with time match?). So it is not done here to avoid
         * dependency on the platform. This class focuses only on
         * mirroring of individual objects.
         */
        Service* getMirroredService(Service *obj);

        virtual void* dispatch(IPService*, void*);
        virtual void* dispatch(ICMPService*, void*);
        virtual void* dispatch(ICMP6Service*, void*);
        virtual void* dispatch(UDPService*, void*);
        virtual void* dispatch(TCPService*, void*);

    };
};

#endif
