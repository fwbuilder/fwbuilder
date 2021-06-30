/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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

#ifndef  __OBJECTMATCHER_HH_FLAG__
#define  __OBJECTMATCHER_HH_FLAG__

#include <string>
#include <list>

#include "fwbuilder/Dispatch.h"


namespace libfwbuilder
{

    class ObjectMatcher : public Dispatch
    {

public:
        typedef enum {EXACT, PARTIAL} address_range_match;

private:
        bool recognize_broadcasts;
        bool recognize_multicasts;
        bool ipv6;
        bool match_subnets;
        address_range_match address_range_match_mode;

        bool checkComplexMatchForSingleAddress(const InetAddr *addr1,
                                               FWObject *obj2);
        int matchRHS(const InetAddr *addr1, Address *obj2);
        int matchInetAddrRHS(const InetAddr *addr1,
                             const InetAddr *rhs_obj_addr);
        int matchSubnetRHS(const InetAddr *addr1,
                           const InetAddr *rhs_obj_addr,
                           const InetAddr *rhs_obj_netm);

public:
        ObjectMatcher() : Dispatch()
        {
            recognize_broadcasts = false;
            recognize_multicasts = false;
            ipv6 = false;
            match_subnets = false;
            address_range_match_mode = PARTIAL;
        }

        virtual ~ObjectMatcher() {}

        void setMatchSubnets(bool f) { match_subnets = f; }
        void setAddressRangeMatchMode(address_range_match f) { address_range_match_mode = f; }
        void setRecognizeBroadcasts(bool f) { recognize_broadcasts = f; }
        void setRecognizeMulticasts(bool f) { recognize_multicasts = f; }
        void setIPV6(bool f) { ipv6 = f; }

        /**
         *  check if address obj1 matches any of the addresses of obj2. Obj1
         * has to be a single address.
         */
        bool checkComplexMatchForSingleAddress(Address *obj1, FWObject *obj2);

        /**
         * check if any of the addresses of obj2 belongs to the subnet of obj1
         */
        bool checkComplexMatchForSubnet(Address *obj1, FWObject *obj2);
                
        /**
         * This method returns true if one of the following conditions is met:
         *
         * 1. obj1 is the same as obj2 (compares ID of both objects), or 
         * 2. obj1 is a child of obj2 on any depth, or
         * 3. address of obj1 matches that of any obj2's interfaces, or 
         * 4. address of obj1 is a broadcast address of one of 
         *    the interfaces of obj2
         * 5. address of obj1 is a broadcast (255.255.255.255)
         */
        bool complexMatch(Address *obj1, Address *obj2);


        virtual void* dispatch(Interface*, void*);
        virtual void* dispatch(Network*, void*);
        virtual void* dispatch(NetworkIPv6*, void*);
        virtual void* dispatch(IPv4*, void*);
        virtual void* dispatch(IPv6*, void*);
        virtual void* dispatch(Host*, void*);
        virtual void* dispatch(Firewall*, void*);
        virtual void* dispatch(Cluster*, void*);
        virtual void* dispatch(AddressRange*, void*);
        virtual void* dispatch(physAddress*, void*);
        virtual void* dispatch(MultiAddressRunTime*, void*);

    };
};

#endif
