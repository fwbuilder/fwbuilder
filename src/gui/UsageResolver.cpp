/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@fwbuilder.org>
  Refactoring: Roman Bovsunivkiy     <a2k0001@gmail.com>

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

#include "UsageResolver.h"

#include <QTime>
#include <QDebug>

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Management.h"

#include <algorithm>

using namespace std;
using namespace libfwbuilder;

/*
UsageResolver::UsageResolver()
{
}
*/

/*
 * per bug #2412334, FWObjectDatabase::findWhereObjectIsUsed finds
 * only "direct" uses of @obj (i.e. it finds group @obj is member of,
 * but not other groups or rules the group is member of). However here
 * we need to find all objects that use @obj, including nested groups
 * and so on. This method is recursive wrapper around
 * FWObjectDatabase::findWhereObjectIsUsed() that does this.
 *
 */
void UsageResolver::findWhereUsedRecursively(
    FWObject *obj, FWObject *top, set<FWObject*> &resset, FWObjectDatabase* db)
{
    if (fwbdebug)
        qDebug() << "ObjectManipulator::findWhereUsedRecursively obj="
                 << obj->getName().c_str()
                 << "(" << obj->getTypeName().c_str() << ")";

    set<FWObject*> resset_tmp;

/*
 * findWhereObjectIsUsed() finds references to object 'obj' in a subtree
 *  rooted at object 'top'.
 */
    db->findWhereObjectIsUsed(obj, top, resset_tmp);
    set<FWObject *>::iterator i = resset.begin();
    for ( ; i!=resset.end(); ++i)
        if (resset_tmp.count(*i)) resset_tmp.erase(*i);

    resset.insert(resset_tmp.begin(), resset_tmp.end());

    i = resset_tmp.begin();
    for ( ; i!=resset_tmp.end(); ++i)
    {
        FWObject *parent_obj = *i;
        FWReference  *ref = FWReference::cast(parent_obj);
        if (ref && RuleElement::cast(ref->getParent()) == NULL)
        {
            // NB! We need parent of this ref for regular groups There
            // is no need to repeat search for the parent if it is
            // RuleElement because rule elements can not be members of
            // groups
            parent_obj = ref->getParent();
        }

        if (fwbdebug)
            qDebug() << "ObjectManipulator::findWhereUsedRecursively"
                     << "paerent_obj=" << parent_obj->getName().c_str()
                     << "(" << parent_obj->getTypeName().c_str() << ")";

        // add new results to a separate set to avoid modifying the resset_tmp
        // in the middle of iteration
        if (Group::cast(parent_obj) && !RuleElement::cast(parent_obj))
            findWhereUsedRecursively(parent_obj, top, resset, db);
    }
}

list<Firewall*> UsageResolver::findFirewallsForObject(FWObject *o, FWObjectDatabase *db)
{
    if (fwbdebug)
        qDebug("ObjectManipulator::findFirewallsForObject");

    list<Firewall *> fws;

    set<FWObject *> resset;
    QTime tt;
    tt.start();
    FWObject *f=o;
    while (f!=NULL && !Firewall::cast(f)) f=f->getParent();
    if (f) fws.push_back(Firewall::cast(f));

    findWhereUsedRecursively(o, db, resset, db);

    //FindWhereUsedWidget::humanizeSearchResults(resset);

    set<FWObject *>::iterator i = resset.begin();
    for ( ;i!=resset.end(); ++i)
    {
        FWObject *obj = *i;
        FWReference  *ref = FWReference::cast(*i);
        if (ref && RuleElement::cast(ref->getParent()) != NULL)
            obj = ref->getParent();
        else
            continue;

        Rule *r = Rule::cast(obj->getParent());
        if (r && !r->isDisabled())
        {
            f = r;
            while (f!=NULL && Firewall::cast(f) == NULL) f = f->getParent();
            if (f && std::find(fws.begin(), fws.end(), f) == fws.end())
            {
                fws.push_back(Firewall::cast(f));
            }
        }
    }

    if (fwbdebug)
        qDebug(QString("Program spent %1 ms searching for firewalls.")
               .arg(tt.elapsed()).toAscii().constData());

    return fws;
}

/*
 * another wrapper around FWObjectDatabase::findWhereObjectIsUsed This
 * method finds all rule elements and groups that have references to
 * @obj and any of its children. The key in the map @res is object ID
 * and the value is a set of all Groups or RE that hold references to
 * it.
 */
void UsageResolver::findAllReferenceHolders(
    FWObject *obj, FWObject *root, std::map<int, std::set<FWObject*> > &res)
{
    set<FWObject*> reference_holders;
    set<FWObject*> res_tmp;
    root->getRoot()->findWhereObjectIsUsed(obj, root->getRoot(), res_tmp);
    foreach(FWObject* o, res_tmp)
    {
        if (FWReference::cast(o))
        {
            FWObject *holder = o->getParent();
            reference_holders.insert(holder);
        }
        if (Rule::cast(o))
            reference_holders.insert(o);
    }
    
    int obj_id = obj->getId();
    foreach(FWObject *o, reference_holders)
    {
        res[obj_id].insert(o);
    }

    for (FWObject::iterator i=obj->begin(); i!=obj->end(); ++i)
    {
        if ((*i)->getId() == -1) continue;
        if (FWReference::cast(*i)) continue;
        if (RuleElement::cast(*i)) continue;
        UsageResolver::findAllReferenceHolders(*i, root, res);
    }
}
