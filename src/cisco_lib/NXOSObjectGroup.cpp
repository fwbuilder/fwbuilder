/* 

                          Firewall Builder

                 Copyright (C) 2002 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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


#include "NXOSObjectGroup.h"

#include "fwbuilder/Address.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"

#include <iostream>
#include <sstream>

using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


const char *NXOSObjectGroup::TYPENAME={"NXOSObjectGroup"};

QString NXOSObjectGroup::groupMemberToString(FWObject *obj,
                                           NamedObjectsManager*)
{
    ostringstream ostr;

    switch (getObjectGroupType()) 
    {
    case NETWORK:
    {
        Address *a = Address::cast(obj);
        assert(a!=nullptr);
        if (AddressRange::cast(a))
        {
            const InetAddr &start = AddressRange::cast(a)->getRangeStart();
            const InetAddr &end = AddressRange::cast(a)->getRangeEnd();
            ostr << "range " << start.toString() << " " << end.toString();
        } else
        {
            const InetAddr *addr = a->getAddressPtr();
            if (Network::cast(obj)!=nullptr)
            {
                const InetAddr *mask = a->getNetmaskPtr();
                // Note: the syntax is "A.B.C.D /NN" (there must be space before /)
                ostr << addr->toString() << " /" << mask->getLength();
            } else {
                ostr << "host " << addr->toString();
            }
        }
        break;
    }

    case PROTO:
    {
        Service *s = Service::cast(obj);
        assert(s!=nullptr);
        ostr << s->getProtocolNumber();
        break;
    }

    case ICMP_TYPE:
    {
        ostr << "icmp ";
        ICMPService *s = ICMPService::cast(obj);
        assert(s!=nullptr);
        if ( s->getInt("type")== -1) ostr << "";
        else ostr << s->getInt("type");
        break;
    }

    case TCP_SERVICE:
    case UDP_SERVICE:
    {
        if (getObjectGroupType()==TCP_SERVICE) ostr << "tcp ";
        else ostr << "udp ";

        TCPUDPService *s = TCPUDPService::cast(obj);
        assert(s!=nullptr);

        int rs = s->getDstRangeStart();
        int re = s->getDstRangeEnd();

        if (rs<0) rs = 0;
        if (re<0) re = 0;

        if (rs>0 || re>0) {
            if (rs==re)  ostr << "eq " << rs;
            else         ostr << "range " << rs << " " << re;
        }
        else ostr << "range 0 65535";
        break;
    }
    default:
        throw FWException("Unknown object group type");
    }
    return ostr.str().c_str();
}

string NXOSObjectGroup::getObjectGroupClass()
{
    switch (this->getObjectGroupType()) 
    {
    case NETWORK:     return "network";
    case PROTO:
    case ICMP_TYPE:
    case TCP_SERVICE:
    case UDP_SERVICE: return "service";
    default: throw FWException("Unknown object group type");
    }
}

string NXOSObjectGroup::getObjectGroupHeader()
{
    ostringstream ostr;
    ostr << "object-group " << getObjectGroupClass() << " " << this->getName();
    return ostr.str();
}

string NXOSObjectGroup::getObjectGroupFooter()
{
    return "exit";
}

