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


#include "BaseObjectGroup.h"
#include "NamedObjectsAndGroupsSupport.h"
#include "NamedObjectsManager.h"

#include "fwbuilder/Address.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/FWException.h"

#include <sstream>

#include <QStringList>
#include <QtDebug>


using namespace libfwbuilder;
using namespace fwcompiler;
using namespace std;


map<QString,int>  BaseObjectGroup::name_disambiguation;

const char *BaseObjectGroup::TYPENAME={"BaseObjectGroup"};

FWObject& BaseObjectGroup::shallowDuplicate(const FWObject *other,
                                            bool preserve_id)
{
    gt = BaseObjectGroup::constcast(other)->gt;
    return FWObject::shallowDuplicate(other, preserve_id);
}


QString BaseObjectGroup::registerGroupName(const QString &prefix,
                                           object_group_type gt)
{
    QString type_suffix;

    switch (gt)
    {
    case UNKNOWN:         type_suffix = "unknown"; break;
    case NETWORK:         type_suffix = "net";     break;
    case PROTO:           type_suffix = "proto";   break;
    case ICMP_TYPE:       type_suffix = "icmp";    break;
    case TCP_SERVICE:     type_suffix = "tcp";     break;
    case UDP_SERVICE:     type_suffix = "udp";     break;
    case TCP_UDP_SERVICE: type_suffix = "tcpudp";  break;
    case MIXED_SERVICE:   type_suffix = "mixed";   break;
    default:              type_suffix = "unknown"; break;
    }

    int n = 0;
    while (true)
    {
        QString full_name =
            QString("%1.%2.%3").arg(prefix).arg(type_suffix).arg(n);
        if (name_disambiguation.count(full_name) == 0)
        {
            name_disambiguation[full_name] = 0;
            return full_name;
        }
        n++;
    }
    return "";
}

BaseObjectGroup::object_group_type BaseObjectGroup::getObjectGroupTypeFromFWObject(
    const FWObject *obj)
{
    if (Address::constcast(obj)!=nullptr)     return NETWORK;
    if (IPService::constcast(obj)!=nullptr)   return PROTO;
    if (ICMPService::constcast(obj)!=nullptr) return ICMP_TYPE;
    if (TCPService::constcast(obj)!=nullptr)  return TCP_SERVICE;
    if (UDPService::constcast(obj)!=nullptr)  return UDP_SERVICE;
    return UNKNOWN;
}

void BaseObjectGroup::setObjectGroupTypeFromFWObject(const FWObject *obj)
{
    setObjectGroupType(getObjectGroupTypeFromFWObject(obj));
}

void BaseObjectGroup::setObjectGroupTypeFromMembers(
    NamedObjectsManager *named_objects_manager)
{
    object_group_type my_type = UNKNOWN;
    std::map<int, int> type_counters;
    for (FWObject::iterator i1=this->begin(); i1!=this->end(); ++i1)
    {
        const FWObject *obj = FWReference::getObject(*i1);

        NamedObject *named_object =
            named_objects_manager->named_objects[obj->getId()];

        if (named_object)
            obj = named_object->getObject();

        object_group_type t = getObjectGroupTypeFromFWObject(obj);
        if (type_counters.count(t) == 0) type_counters[t] = 1;
        else type_counters[t]++;
    }

    if (type_counters[NETWORK]!=0 &&
        (type_counters[PROTO]!=0 ||
         type_counters[ICMP_TYPE]!=0 ||
         type_counters[TCP_SERVICE]!=0 ||
         type_counters[UDP_SERVICE]!=0 ||
         type_counters[MIXED_SERVICE]!=0))
        throw FWException("Object group should not contain both "
                          "network and service objects");

    if (type_counters[NETWORK]!=0) my_type = NETWORK;
    
    if (type_counters[PROTO]==0 &&
        type_counters[ICMP_TYPE]==0 &&
        (type_counters[TCP_SERVICE]!=0 ||
         type_counters[UDP_SERVICE]!=0) &&
        type_counters[MIXED_SERVICE]==0)
    {
        if (type_counters[TCP_SERVICE]!=0 && type_counters[UDP_SERVICE]!=0)
            my_type = TCP_UDP_SERVICE;
        if (type_counters[TCP_SERVICE]!=0 && type_counters[UDP_SERVICE]==0)
            my_type = TCP_SERVICE;
        if (type_counters[TCP_SERVICE]==0 && type_counters[UDP_SERVICE]!=0)
            my_type = UDP_SERVICE;
    }

    if (type_counters[PROTO]!=0 &&
        type_counters[ICMP_TYPE]==0 &&
        type_counters[MIXED_SERVICE]==0) my_type = PROTO;

    if (type_counters[PROTO]==0 &&
        type_counters[ICMP_TYPE]!=0 &&
        type_counters[MIXED_SERVICE]==0) my_type = ICMP_TYPE;

    if (my_type==UNKNOWN) my_type = MIXED_SERVICE;

    setObjectGroupType(my_type);
}

bool BaseObjectGroup::isServiceGroup()
{
    switch (getObjectGroupType())
    {
    case PROTO:           return true;
    case ICMP_TYPE:       return true;
    case TCP_SERVICE:     return true;
    case UDP_SERVICE:     return true;
    case TCP_UDP_SERVICE: return true;
    case MIXED_SERVICE:   return true;
    default:              return false;
    }
    return false;
}

bool BaseObjectGroup::isObjectGroup()
{
    switch (getObjectGroupType())
    {
    case UNKNOWN:      return true;
    case NETWORK:      return true;
    default:           return false;
    }
    return false;
}

string BaseObjectGroup::getSrvTypeName()
{
    switch (getObjectGroupType())
    {
    case ICMP_TYPE:    return "icmp";
    case TCP_SERVICE:  return "tcp";
    case UDP_SERVICE:  return "udp";
    case TCP_UDP_SERVICE:  return "tcp-udp";
    default:           break;
    }
    return "";
}

string BaseObjectGroup::getObjectGroupClass()
{
    switch (getObjectGroupType())
    {
    case PROTO:
    case ICMP_TYPE:
    case TCP_SERVICE:
    case UDP_SERVICE:
    case TCP_UDP_SERVICE:
    case MIXED_SERVICE:  return "service";
    default:             return "network";
    }
    return "";
}

QString BaseObjectGroup::groupMemberToString(FWObject*, NamedObjectsManager*)
{
    return "";
}

QString BaseObjectGroup::toString(NamedObjectsManager *nm)
{
    QStringList res;
    if (this->size()==0) return "";
    res << getObjectGroupHeader().c_str();
    for (FWObject::iterator i1=this->begin(); i1!=this->end(); ++i1)
    {
        res << QString("  %1").arg(
            groupMemberToString(FWReference::getObject(*i1), nm));
    }
    res << getObjectGroupFooter().c_str();
    res << "";
    return res.join("\n");
}

string BaseObjectGroup::getObjectGroupHeader()
{
    return "";
}

string BaseObjectGroup::getObjectGroupFooter()
{
    return "";
}

