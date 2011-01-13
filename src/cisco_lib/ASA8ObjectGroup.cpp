/* 

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

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

#include "config.h"

#include "ASA8ObjectGroup.h"

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
using namespace fwcompiler;

const char *ASA8ObjectGroup::TYPENAME={"ASA8ObjectGroup"};

string ASA8ObjectGroup::toString(std::map<int, ASA8Object*> &named_objects_registry)
    throw(FWException)
{
    ostringstream ostr;

    if (this->size()==0) return "";

    ostr << getObjectGroupHeader();

    for (FWObject::iterator i1=this->begin(); i1!=this->end(); ++i1)
    {
        FWObject *o   = *i1;
        FWObject *obj = o;
        if (FWReference::cast(o)!=NULL) obj=FWReference::cast(o)->getPointer();

        ASA8Object *named_object = named_objects_registry[obj->getId()];
        if (named_object)
        {
            ostr << " "
                 << named_object->getCommandWhenObjectGroupMember().toStdString();
            ostr << endl;
            continue;
        }

        if (this->getObjectGroupType() == NETWORK)
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
            continue;

        } else
        {

            if (IPService::isA(obj))
            {
                ostr << " service-object ";
                Service *s = Service::cast(obj);
                assert(s!=NULL);
                ostr << s->getProtocolName();
                ostr << endl;
                continue;
            }

            if (ICMPService::isA(obj))
            {
                ostr << " service-object icmp ";
                ICMPService *s = ICMPService::cast(obj);
                assert(s!=NULL);
                if ( s->getInt("type")== -1)
                    ostr << "";  // no keyword "any" anymore
                else
                    ostr << s->getInt("type");
                ostr << endl;
                continue;
            }

            if (TCPService::isA(obj) || UDPService::isA(obj))
            {
                ostr << " service-object ";
                ostr << ((TCPService::isA(obj))? "tcp " : "udp ");

                Service *s = Service::cast(obj);
                assert(s!=NULL);

                int rs = TCPUDPService::cast(s)->getDstRangeStart();
                int re = TCPUDPService::cast(s)->getDstRangeEnd();

                if (rs<0) rs = 0;
                if (re<0) re = 0;

                if (rs>0 || re>0) {
                    if (rs==re)  ostr << "eq " << rs;
                    else         ostr << "range " << rs << " " << re;
                }
                else ostr << "range 0 65535";
                ostr << endl;
                continue;
            }

            throw FWException("ASA8ObjectGroup: Unknown object group type");
        }
    }
    ostr << " exit" << endl << endl;
    return ostr.str();
}

string ASA8ObjectGroup::getObjectGroupClass()
{
    switch (this->getObjectGroupType()) 
    {
    case NETWORK:        return "network";
    case MIXED_SERVICE:  return "service";;
    default:
        throw FWException("ASA8ObjectGroup: Unknown object group type");
    }
}

string ASA8ObjectGroup::getObjectGroupHeader()
{
    ostringstream ostr;
    ostr << "object-group " << getObjectGroupClass() << " " << this->getName();
    ostr << endl;
    return ostr.str();
}

