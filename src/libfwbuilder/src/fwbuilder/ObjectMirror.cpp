/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: ObjectMirror.cpp 534 2010-02-07 23:25:06Z vadim $

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


  ====================================================================

*/

#include <assert.h>


#include "fwbuilder/ObjectMirror.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"

#include <iostream>
#include <string>

 
using namespace libfwbuilder;
using namespace std;


Service* ObjectMirror::getMirroredService(Service *obj)
{
    void* res = obj->dispatch(this, (void*)nullptr);
    return Service::cast((FWObject*)(res));
}

void* ObjectMirror::dispatch(IPService* obj, void*)
{
    return obj;
}

void* ObjectMirror::dispatch(ICMPService* obj, void*)
{
    ICMPService *new_obj = obj->getRoot()->createICMPService();
    new_obj->setName(obj->getName() + "-mirror");
    if (obj->getInt("type") == 8)
    {
        new_obj->setInt("type", 0);
        new_obj->setInt("code", 0);
    } else
    {
        new_obj->setInt("type", obj->getInt("type"));
        new_obj->setInt("code", obj->getInt("code"));
    }
    return new_obj;
}

void* ObjectMirror::dispatch(ICMP6Service* obj, void*)
{
    ICMP6Service *new_obj = obj->getRoot()->createICMP6Service();
    new_obj->setName(obj->getName() + "-mirror");
    if (obj->getInt("type") == 128)
    {
        new_obj->setInt("type", 129);
        new_obj->setInt("code", 0);
    } else
    {
        new_obj->setInt("type", obj->getInt("type"));
        new_obj->setInt("code", obj->getInt("code"));
    }
    return new_obj;
}

void* ObjectMirror::dispatch(TCPService* obj, void*)
{
    TCPService *new_obj = obj->getRoot()->createTCPService();
    new_obj->duplicate(obj); // mostly to copy tcp flags
    new_obj->setName(obj->getName() + "-mirror");
    new_obj->setSrcRangeStart(obj->getDstRangeStart());
    new_obj->setSrcRangeEnd(obj->getDstRangeEnd());
    new_obj->setDstRangeStart(obj->getSrcRangeStart());
    new_obj->setDstRangeEnd(obj->getSrcRangeEnd());
    // if original tcp service does something with flags, we can't simply
    // invert it by adding flag "established". Just leave as-is
    if (!obj->inspectFlags())
        new_obj->setEstablished( ! obj->getEstablished());
    return new_obj;
}

void* ObjectMirror::dispatch(UDPService* obj, void*)
{
    UDPService *new_obj = obj->getRoot()->createUDPService();
    new_obj->setName(obj->getName() + "-mirror");
    new_obj->setSrcRangeStart(obj->getDstRangeStart());
    new_obj->setSrcRangeEnd(obj->getDstRangeEnd());
    new_obj->setDstRangeStart(obj->getSrcRangeStart());
    new_obj->setDstRangeEnd(obj->getSrcRangeEnd());
    return new_obj;
}

