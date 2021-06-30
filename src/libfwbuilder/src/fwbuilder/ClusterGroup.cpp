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



#include "fwbuilder/ClusterGroup.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWOptions.h"

using namespace std;
using namespace libfwbuilder;

const char *ClusterGroup::TYPENAME = {"ClusterGroup"};

ClusterGroup::ClusterGroup() : ObjectGroup()
{
    setStr("type", "");
}

void ClusterGroup::init(FWObjectDatabase *root)
{
    FWObject *gopt = getFirstByType(ClusterGroupOptions::TYPENAME);
    if (gopt == nullptr)
    {
        gopt = root->create(ClusterGroupOptions::TYPENAME);
        add(gopt);
    }
}

bool ClusterGroup::validateChild(FWObject *o)
{
    string otype = o->getTypeName();

    return (FWObject::validateChild(o) &&
            (otype == Interface::TYPENAME ||
             otype == ClusterGroupOptions::TYPENAME ||
             otype == FWObjectReference::TYPENAME));
}

void ClusterGroup::replaceReferenceInternal(int old_id, int new_id, int &counter)
{
    if (old_id == new_id) return;

    FWObject::replaceReferenceInternal(old_id, new_id, counter);

    string master_iface_id = getStr("master_iface");
    if (!master_iface_id.empty())
    {
        int master_iface_id_int = FWObjectDatabase::getIntId(master_iface_id);
        if (master_iface_id_int == old_id)
        {
            setStr("master_iface", FWObjectDatabase::getStringId(new_id));
            counter++;
        }
    }
}

void ClusterGroup::fromXML(xmlNodePtr parent)
{
    FWObject::fromXML(parent);

    const char *n;
    n = XMLTools::FromXmlCast(xmlGetProp(parent, XMLTools::ToXmlCast("type")));
    if (n != nullptr)
    {
        setStr("type", n);
        XMLTools::FreeXmlBuff(n);
    }
    n = XMLTools::FromXmlCast(xmlGetProp(parent, XMLTools::ToXmlCast("master_iface")));
    if (n != nullptr)
    {
        setStr("master_iface", n);
        XMLTools::FreeXmlBuff(n);
    }
}

xmlNodePtr ClusterGroup::toXML(xmlNodePtr parent)
{
    xmlNodePtr me = FWObject::toXML(parent, false);
    xmlNewProp(me, XMLTools::ToXmlCast("name"), XMLTools::StrToXmlCast(getName()));
    xmlNewProp(me, XMLTools::ToXmlCast("comment"), XMLTools::StrToXmlCast(getComment()));

    FWObject *o;
    for (FWObjectTypedChildIterator it = findByType(FWObjectReference::TYPENAME);
        it != it.end(); ++it)
    {
        o = *it; if (o) o->toXML(me);
    }

    for (FWObjectTypedChildIterator it = findByType(ClusterGroupOptions::TYPENAME);
        it != it.end(); ++it)
    {
        o = *it; if (o) o->toXML(me);
    }

    return me;
}

ClusterGroupOptions* ClusterGroup::getOptionsObject()
{
    ClusterGroupOptions *gopt = ClusterGroupOptions::cast(
        getFirstByType(ClusterGroupOptions::TYPENAME));

    if (gopt == nullptr)
    {
        gopt = ClusterGroupOptions::cast(
            getRoot()->create(ClusterGroupOptions::TYPENAME));
        add(gopt);
    }
    return gopt;
}

FWObject& ClusterGroup::duplicateForUndo(const FWObject *obj)
{
    if (ClusterGroup::constcast(obj)==nullptr) return *this;

    setRO(false);

    ClusterGroupOptions *their_opts = ClusterGroupOptions::cast(
        obj->getFirstByType(ClusterGroupOptions::TYPENAME));
    ClusterGroupOptions *mine_opts = ClusterGroupOptions::cast(
        getFirstByType(ClusterGroupOptions::TYPENAME));

    list<FWObject*> all_refs = getByType(FWObjectReference::TYPENAME);
    while (all_refs.size())
    {
        remove(all_refs.front(), false);
        all_refs.pop_front();
    }

    for(list<FWObject*>::const_iterator m=obj->begin(); m!=obj->end(); ++m)
    {
        if (FWReference::cast(*m))
        {
            FWObject *object = FWReference::getObject(*m);
            addRef(object);
        }
    }
    if (their_opts && mine_opts) mine_opts->duplicate(their_opts);
    if (their_opts && mine_opts==nullptr) addCopyOf(their_opts);

    shallowDuplicate(obj);
    return *this;
}

Interface* ClusterGroup::getInterfaceForMemberFirewall(Firewall *fw)
{
#ifdef DEBUG_FOR_DMZ
    cerr << "ClusterGroup::getInterfaceForMemberFirewall " << fw << endl;
    if (fw)
    {
        cerr << "  fw: " << fw->getName() << endl;
    }
#endif

    for (FWObjectTypedChildIterator it = findByType(FWObjectReference::TYPENAME);
         it != it.end(); ++it)
    {
        Interface *other_iface = Interface::cast(FWObjectReference::getObject(*it));
        assert(other_iface);

#ifdef DEBUG_FOR_DMZ
        cerr << "  other_iface: " << other_iface->getName()
             << "  isChildOf(fw): " << other_iface->isChildOf(fw)
             << endl;
#endif

        if (other_iface->isChildOf(fw)) return other_iface;
    }
    return nullptr;
}

