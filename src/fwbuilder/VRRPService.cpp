/*
 * VRRPService.cpp - VRRPService class implementation
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#include <assert.h>

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/VRRPService.h>
#include <fwbuilder/XMLTools.h>

using namespace libfwbuilder;
using namespace std;

const char *VRRPService::TYPENAME={"VRRPService"};

VRRPService::VRRPService() {
    setStr("type", "-1");
    setStr("code", "-1");
}

VRRPService::VRRPService(const FWObjectDatabase *root,bool prepopulate) :
    Service(root,prepopulate)
{
    setStr("type", "-1");
    setStr("code", "-1");
}

VRRPService::~VRRPService() {}

string VRRPService::getProtocolName()    { return "vrrp"; }
int    VRRPService::getProtocolNumber()  { return 112; }

void VRRPService::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);

    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("type")));
    assert(n!=NULL);
    setStr("type", n);
    FREEXMLBUFF(n);

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("code")));
    if(n!=NULL)
    {
        setStr("code", n);
        FREEXMLBUFF(n);
    }
}

xmlNodePtr VRRPService::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);

    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));

    return me;
}

