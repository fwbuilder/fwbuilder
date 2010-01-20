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

#include "config.h"

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
using namespace std;

const char *PIXObjectGroup::TYPENAME={"PIXObjectGroup"};

string PIXObjectGroup::toString()  throw(FWException)
{
    ostringstream ostr;

    if (this->size()==0) return "";

    switch (this->getObjectGroupType()) 
    {
    case NETWORK:     
        ostr << "object-group network "
             << this->getName() << endl;
        break;
    case PROTO:       
        ostr << "object-group protocol "
             << this->getName() << endl;
        break;
    case ICMP_TYPE:   
        ostr << "object-group icmp-type "
             << this->getName() << endl;
        break;
    case TCP_SERVICE: 
        ostr << "object-group service "
             << this->getName() << " tcp" << endl;
        break;
    case UDP_SERVICE: 
        ostr << "object-group service "
             << this->getName() << " udp" << endl;
        break;
    default:
        throw FWException("Unknown object group type");
    }

    for (FWObject::iterator i1=this->begin(); i1!=this->end(); ++i1)
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();

        switch (this->getObjectGroupType()) 
        {
        case NETWORK:
        {
            Address *a = Address::cast(obj);
            assert(a!=NULL);
            const InetAddr *addr = a->getAddressPtr();
            ostr << " network-object ";
            if (Network::cast(obj)!=NULL)
            {
                const InetAddr *mask = a->getNetmaskPtr();
                ostr << addr->toString() << " ";
                ostr << mask->toString() << " ";
            } else {
                ostr << " host ";
                ostr << addr->toString() << " ";
            }
            ostr << endl;
            break;
        }
        case PROTO:
        {
            ostr << " protocol-object ";
            Service *s=Service::cast(obj);
            assert(s!=NULL);
            ostr << s->getProtocolName();
            ostr << endl;
            break;
        }
        case ICMP_TYPE:
        {
            ostr << " icmp-object ";
            ICMPService *s=ICMPService::cast(obj);
            assert(s!=NULL);
            if ( s->getInt("type")== -1)
                ostr << "any";
            else
                ostr << s->getInt("type");
            ostr << endl;
            break;
        }
        case TCP_SERVICE:
        case UDP_SERVICE:
        {
            ostr << " port-object ";
            Service *s=Service::cast(obj);
            assert(s!=NULL);

            int rs=TCPUDPService::cast(s)->getDstRangeStart();
            int re=TCPUDPService::cast(s)->getDstRangeEnd();

            if (rs<0) rs=0;
            if (re<0) re=0;

            if (rs>0 || re>0) {
                if (rs==re)  ostr << "eq " << rs;
                else         ostr << "range " << rs << " " << re;
            }
            else ostr << "range 0 65535";
            ostr << endl;
            break;
        }
        default:
            throw FWException("Unknown object group type");
        }
    }
    ostr << " exit" << endl << endl;
    return ostr.str();
}

