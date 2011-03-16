/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#include "objectMaker.h"

#include "fwbuilder/AddressRange.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UDPService.h"

#include "QStringListOperators.h"

// TODO: FWBTree needs to be refactored into an independent module
#include "../libgui/FWBTree.h"

#include <QStringList>

extern int fwbdebug;

using namespace libfwbuilder;
using namespace std;


QString ObjectSignature::toString() const
{
    QStringList sig;

    sig << type_name;

    if (type_name == IPv4::TYPENAME || type_name == IPv6::TYPENAME ||
        type_name == Network::TYPENAME || type_name == NetworkIPv6::TYPENAME)
        sig << address << netmask;

    if (type_name == AddressRange::TYPENAME)
        sig << address_range_start << address_range_end;

    if (type_name == DNSName::TYPENAME)
        sig << dns_name;

    if (type_name == AddressTable::TYPENAME)
        sig << address_table_name;

    if (type_name == CustomService::TYPENAME)
        sig << platform << code << protocol_name;

    if (type_name == ICMPService::TYPENAME)
        sig << icmp_type << icmp_code;

    if (type_name == IPService::TYPENAME)
        sig << protocol << fragments;

    if (type_name == TCPService::TYPENAME)
        sig << src_port_range_start << src_port_range_end
            << dst_port_range_start << dst_port_range_end
            << established
            << flags_mask << flags_comp;

    if (type_name == UDPService::TYPENAME)
        sig << src_port_range_start << src_port_range_end
            << dst_port_range_start << dst_port_range_end;

    if (type_name == TagService::TYPENAME)
        sig << tag;

    return sig.join("|-|");
}

//****************************************************************

void ObjectMaker::clear()
{
    last_created = NULL;
    object_registry.clear(); 
}

FWObject* ObjectMaker::findMatchingObject(const ObjectSignature &sig)
{
    QString signature = sig.toString();
    if (object_registry.count(signature) > 0)
        return library->getRoot()->findInIndex(object_registry[signature]);
    return NULL;
}

void ObjectMaker::registerObject(const ObjectSignature &sig, FWObject* obj)
{
    object_registry[sig.toString()] = (obj!=NULL) ? obj->getId() : -1;
}

//****************************************************************

FWObject* ObjectMaker::createObject(const std::string &objType,
                                    const std::string &objName)
{
    assert(library!=NULL);
    FWBTree tree ;
    FWObject *slot = tree.getStandardSlotForObject(library,objType.c_str());
    return createObject(slot, objType, objName);
}

FWObject* ObjectMaker::createObject(FWObject *parent,
                                    const std::string &objType,
                                    const std::string &objName)
{
    assert(library!=NULL);
    FWObject*  o = library->getRoot()->create(objType);
    if (parent != NULL)
    {
        parent->add(o);
    }
    o->setName(objName);
    return o;
}
