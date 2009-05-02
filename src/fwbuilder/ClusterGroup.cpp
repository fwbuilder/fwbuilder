/*
 * ClusterGroup.cpp - ClusterGroup class implementation
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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/ClusterGroup.h>
#include <fwbuilder/Interface.h>
#include <fwbuilder/FWObjectDatabase.h>

using namespace std;
using namespace libfwbuilder;

const char *ClusterGroup::TYPENAME = {"ClusterGroup"};

ClusterGroup::ClusterGroup() : ObjectGroup()
{
    setStr("type", "vrrp");
}

ClusterGroup::ClusterGroup(const FWObjectDatabase *root, bool prepopulate)
    : ObjectGroup(root, prepopulate)
{
    setStr("type", "vrrp");
}

bool ClusterGroup::validateChild(FWObject *o)
{
    string otype = o->getTypeName();

    return (FWObject::validateChild(o) &&
            (otype == Interface::TYPENAME ||
             otype == FWObjectReference::TYPENAME));
}

void ClusterGroup::fromXML(xmlNodePtr parent) throw(FWException)
{
    FWObject::fromXML(parent);

    const char *n;
    n = FROMXMLCAST(xmlGetProp(parent, TOXMLCAST("type")));
    if (n != NULL)
    {
        setStr("type", n);
        FREEXMLBUFF(n);
    }
    n = FROMXMLCAST(xmlGetProp(parent, TOXMLCAST("master_iface")));
    if (n != NULL)
    {
        setStr("master_iface", n);
        FREEXMLBUFF(n);
    }
}

xmlNodePtr ClusterGroup::toXML(xmlNodePtr parent) throw(FWException)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, TOXMLCAST("name"), STRTOXMLCAST(getName()));
    xmlNewProp(me, TOXMLCAST("comment"), STRTOXMLCAST(getComment()));

    for (list<FWObject*>::const_iterator j = begin(); j != end(); ++j)
    {
        (*j)->toXML(me);
    }

    return me;
}

