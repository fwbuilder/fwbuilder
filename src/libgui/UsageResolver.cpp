/*

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland <vadim@fwbuilder.org>
  Refactoring: Roman Bovsunivskiy     <a2k0001@gmail.com>

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
#include <QtDebug>

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/RuleSet.h"


#include <algorithm>

using namespace std;
using namespace libfwbuilder;

int UsageResolver::search_id_seed = 0;


UsageResolver::UsageResolver()
{
    search_id = search_id_seed;
    search_id_seed++;
}


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
        qDebug() << "UsageResolver::findWhereUsedRecursively obj="
                 << obj->getName().c_str()
                 << "(" << obj->getTypeName().c_str() << ")";

    set<FWObject*> new_obj_set;
    set<FWObject*> resset_tmp_2;

/*
 * findWhereObjectIsUsed() finds references to object 'obj' in a subtree
 * rooted at object 'top'.
 */
    db->findWhereObjectIsUsed(obj, top, resset_tmp_2);

/*
 * skip objects that are already in resset because they were found on previous
 * passes (recursive calls)
 */
    set<FWObject *>::iterator i = resset.begin();
    for ( ; i!=resset.end(); ++i)
        if (resset_tmp_2.count(*i)) resset_tmp_2.erase(*i);
/*
 * skip objects we have already seen to break recursive loops in rule branches
 */
    for (i=resset_tmp_2.begin(); i!=resset_tmp_2.end(); ++i)
    {
        if ((*i)->getInt(".usage_resolver_seach_id") != search_id)
        {
            new_obj_set.insert(*i);
            (*i)->setInt(".usage_resolver_seach_id", search_id);
        }
    }

    resset.insert(new_obj_set.begin(), new_obj_set.end());

    for (i = new_obj_set.begin(); i!=new_obj_set.end(); ++i)
    {
        FWObject *parent_obj = *i;
        FWReference  *ref = FWReference::cast(parent_obj);
        if (ref && RuleElement::cast(ref->getParent()) == nullptr)
        {
            // NB! We need parent of this ref for regular groups There
            // is no need to repeat search for the parent if it is
            // RuleElement because rule elements can not be members of
            // groups
            parent_obj = ref->getParent();
        }

        if (fwbdebug)
            qDebug() << "UsageResolver::findWhereUsedRecursively"
                     << "parent_obj=" << parent_obj->getName().c_str()
                     << "(" << parent_obj->getTypeName().c_str() << ")";

        // add new results to a separate set to avoid modifying the new_obj_set
        // in the middle of iteration
        if (Group::cast(parent_obj) && !RuleElement::cast(parent_obj))
            findWhereUsedRecursively(parent_obj, top, resset, db);
    }
}

list<Firewall*> UsageResolver::findFirewallsForObject(FWObject *o,
                                                      FWObjectDatabase *db)
{
    if (fwbdebug)
        qDebug("UsageResolver::findFirewallsForObject");

    list<Firewall *> fws;

    set<FWObject *> resset;
    QTime tt;
    tt.start();
    FWObject *f=o;
    while (f!=nullptr && !Firewall::cast(f)) f=f->getParent();
    if (f) fws.push_back(Firewall::cast(f));

    findWhereUsedRecursively(o, db, resset, db);

    //FindWhereUsedWidget::humanizeSearchResults(resset);

    if (fwbdebug)
    {
        qDebug() << "UsageResolver::findFirewallsForObject"
                 << "resset.size()=" << resset.size();
        set<FWObject *>::iterator i = resset.begin();
        for ( ;i!=resset.end(); ++i)
        {
            FWObject *obj = *i;
            qDebug() << "UsageResolver::findFirewallsForObject"
                     << obj->getName().c_str()
                     << "(" << obj->getTypeName().c_str() << ")";
        }
    }

    // whenever we find that a rule has been modified by the change in
    // the object @o, we record rule set it belongs to. We should scan
    // other rule sets to see if some rule somewhere might be using
    // one of the rule sets that changed as a branch. However in a
    // common situation where many rules of the same rule set are
    // affected, it won't make sense to search for the dependencies on
    // the same rule set many times.

    set<RuleSet*> modified_rule_sets;

    set<FWObject*>::iterator i = resset.begin();
    for ( ;i!=resset.end(); ++i)
    {
        FWObject *obj = *i;

        // We only want cluster (to pick up member changes) and rule elements
        if (Cluster::isA(obj)) fws.push_back(Firewall::cast(obj));

        FWReference  *ref = FWReference::cast(*i);
        if (ref && RuleElement::cast(ref->getParent()) != nullptr)
        {
            obj = ref->getParent();
        }

        Rule *r = Rule::cast(obj);
        if (r == nullptr) r = Rule::cast(obj->getParent());

        if (r && !r->isDisabled())
        {
            f = r;
            while (f!=nullptr && Firewall::cast(f) == nullptr) f = f->getParent();
            if (f && std::find(fws.begin(), fws.end(), f) == fws.end())
            {
                fws.push_back(Firewall::cast(f));
            }

            // check if some rule somewhere may use @ruleset as a
            // branch

            RuleSet *ruleset = RuleSet::cast(r->getParent());
            assert(ruleset != nullptr);
            modified_rule_sets.insert(ruleset);
        }
    }

    set<RuleSet*>::iterator i1 = modified_rule_sets.begin();
    for ( ;i1!=modified_rule_sets.end(); ++i1)
    {
        RuleSet *ruleset = *i1;

        list<Firewall*> other_fws =
            UsageResolver::findFirewallsForObject(ruleset, db);

        for (list<Firewall*>::iterator fit = other_fws.begin();
             fit != other_fws.end(); ++fit)
        {
            if (std::find(fws.begin(), fws.end(), *fit) == fws.end())
                fws.push_back(*fit);
        }
    }


    if (fwbdebug)
    {
        qDebug() << QString("Program spent %1 ms searching for firewalls.")
            .arg(tt.elapsed());
        qDebug() << "UsageResolver::findFirewallsForObject returns";

        for (list<Firewall*>::iterator i = fws.begin(); i!=fws.end(); ++i)
        {
            qDebug() << "    " << (*i)->getName().c_str();
        }
    }

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
        if (fwbdebug)
            qDebug() << "UsageResolver::findAllReferenceHolders"
                     << "obj=" << obj->getName().c_str()
                     << "(" << obj->getTypeName().c_str() << ")"
                     << "container=" << o->getName().c_str()
                     << "(" << o->getTypeName().c_str() << ")";

        if (FWReference::cast(o))
        {
            FWObject *holder = o->getParent();
            reference_holders.insert(holder);
        }
        if (Rule::cast(o) || Interface::cast(o))
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
        if (FWOptions::cast(*i)) continue;
        if (FWReference::cast(*i)) continue;
        if (RuleElement::cast(*i)) continue;
        if (Rule::cast(*i)) continue;
        UsageResolver::findAllReferenceHolders(*i, root, res);
    }
}

/*
 * This method post-processes the list of objects found by
 * findFirewallsForObject to make them suitable for presentation.
 * First, it does deduplication.  Event showObjectInRulesetEvent that
 * finds an object and highlights it in rules requires reference or
 * object itself as an argument. So, when parent is RuleElement, we
 * preserve the reference. But for regular groups we find and
 * highlight the group itself, so in that case replace reference to
 * the object with the group, which is its parent.
 *
 */
void UsageResolver::humanizeSearchResults(std::set<FWObject *> &resset)
{
    set<FWObject*> tmp_res;  // set deduplicates items automatically
    set<FWObject*>::iterator i = resset.begin();
    for (;i!=resset.end();++i)
    {
        FWObject *obj = *i;
        if (fwbdebug)
            qDebug() << "humanizeSearchResults:"
                     << obj->getName().c_str()
                     << " (" << obj->getTypeName().c_str() << ")";
        FWReference  *ref = FWReference::cast(*i);
        if (ref && RuleElement::cast(ref->getParent()) == nullptr)
        {
            obj = ref->getParent();  // NB! We need parent of this ref for groups
        } else
            obj = *i;
        tmp_res.insert(obj);
    }
    resset.clear();
    resset = tmp_res;
}
