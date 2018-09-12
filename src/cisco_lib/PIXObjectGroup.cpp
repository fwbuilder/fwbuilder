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


#include "PIXObjectGroup.h"

#include "fwbuilder/Address.h"
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

const char *PIXObjectGroup::TYPENAME={"PIXObjectGroup"};


QString PIXObjectGroup::groupMemberToString(FWObject *obj,
                                           NamedObjectsManager*)
{
    ostringstream ostr;

    if (this->getObjectGroupType() == NETWORK)
    {
        Address *a = Address::cast(obj);
        assert(a!=nullptr);
        const InetAddr *addr = a->getAddressPtr();
        ostr << "network-object ";
        if (Network::cast(obj)!=nullptr)
        {
            const InetAddr *mask = a->getNetmaskPtr();
            ostr << addr->toString() << " ";
            ostr << mask->toString() << " ";
        } else {
            ostr << "host ";
            ostr << addr->toString() << " ";
        }
        return ostr.str().c_str();

    } else
    {

        if (IPService::isA(obj))
        {
            ostr << "protocol-object ";
            Service *s=Service::cast(obj);
            assert(s!=nullptr);
            ostr << s->getProtocolName();
            return ostr.str().c_str();
        }

        if (ICMPService::isA(obj))
        {
            ostr << "icmp-object ";
            ICMPService *s=ICMPService::cast(obj);
            assert(s!=nullptr);
            if ( s->getInt("type")== -1)
                ostr << "any";
            else
                ostr << s->getInt("type");
            return ostr.str().c_str();
        }

        if (TCPService::isA(obj) || UDPService::isA(obj))
        {
            ostr << "port-object ";
            Service *s=Service::cast(obj);
            assert(s!=nullptr);

            int rs=TCPUDPService::cast(s)->getDstRangeStart();
            int re=TCPUDPService::cast(s)->getDstRangeEnd();

            if (rs<0) rs=0;
            if (re<0) re=0;

            if (rs>0 || re>0) {
                if (rs==re)  ostr << "eq " << rs;
                else         ostr << "range " << rs << " " << re;
            }
            else ostr << "range 0 65535";
            return ostr.str().c_str();
        }

        QString err("PIXObjectGroup: Unsupported object '%1' found in "
                    "object group");
        throw FWException(err.arg(obj->getName().c_str()).toStdString());
    }

    return ostr.str().c_str();
}

string PIXObjectGroup::getObjectGroupClass()
{
    switch (this->getObjectGroupType()) 
    {
    case NETWORK:         return "network";
    case PROTO:           return "protocol";
    case ICMP_TYPE:       return "icmp-type";
    case TCP_SERVICE:     return "service";
    case UDP_SERVICE:     return "service";
    case TCP_UDP_SERVICE: return "service";
    case MIXED_SERVICE:   return "service";;
    default:
    {
        QString err("PIXObjectGroup::getObjectGroupClass(): Unknown object "
                    "group type '%1'");
        throw FWException(err.arg(this->getObjectGroupType()).toStdString());
    }
    }
}

string PIXObjectGroup::getObjectGroupHeader()
{
    ostringstream ostr;
    ostr << "object-group " << getObjectGroupClass() << " " << this->getName();
    switch (this->getObjectGroupType()) 
    {
    case TCP_SERVICE:     ostr << " tcp"; break;
    case UDP_SERVICE:     ostr << " udp"; break;
    case TCP_UDP_SERVICE: ostr << " tcp-udp"; break;
    default: break;
    }
    return ostr.str();
}

string PIXObjectGroup::getObjectGroupFooter()
{
    return "exit";
}

