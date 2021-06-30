/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland vadim@fwbuilder.org

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

#include <stdlib.h>



#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/FWIntervalReference.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleSet.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace libfwbuilder;



/*
 * find all objects used by the group 'gr'. Resolves references
 * recursively (that is, if a group member is another group, this
 * method descends into it and scans all objects that group uses)
 */
void FWObjectDatabase::findObjectsInGroup(Group *g, set<FWObject *> &res)
{
    searchId++;
    _findObjectsInGroup(g, res);
}

void FWObjectDatabase::_findObjectsInGroup(Group *g, set<FWObject *> &res)
{
    if (g->size()==0 || g->getInt(".searchId")==searchId) return;
    
    g->setInt(".searchId",searchId);
    FWObjectReference *ref;
    Group *sg;
    FWObject *obj;
    FWObject::iterator i=g->begin();

    for (;i!=g->end();++i)
    {
        
       ref=FWObjectReference::cast(*i);
       if (ref==nullptr) 
       {
           res.insert(*i);
           continue;
       }
       
       obj=ref->getPointer();
       sg=Group::cast(obj);
       
       if (sg==nullptr)
       {
           res.insert(obj);
           continue;
       }
       
       _findObjectsInGroup(sg,res);
    }    
}

/*
 ***********************************************************************
 */

/**
 * Find firewall object by name. Finds Firewall and Cluster objects.
 */
Firewall* FWObjectDatabase::_findFirewallByNameRecursive(FWObject* db,
                                         const string &name)
{
    // use Firewall::cast so that both Firewall and Cluster objects match
    if (Firewall::cast(db) &&
        db->getName()==name &&
        db->getParent()->getId()!=FWObjectDatabase::DELETED_OBJECTS_ID)
        return static_cast<Firewall*>(db);

    list<FWObject*>::iterator j;
    for(j=db->begin(); j!=db->end(); ++j)     
    {
        FWObject *o=*j;

        o = _findFirewallByNameRecursive(o, name);
        if(o) return static_cast<Firewall*>(o);
    }
    if (db==this)
        throw FWException("Firewall object '"+name+"' not found");
    return nullptr; // not found
}

Firewall* FWObjectDatabase::findFirewallByName(const string &name)
{
    return _findFirewallByNameRecursive(this, name);
}

//#define DEBUG_WHERE_USED 1

bool FWObjectDatabase::_isInIgnoreList(FWObject *o)
{
    if (FWOptions::cast(o)!=nullptr) return true;
    if (o->getId()==FWObjectDatabase::DELETED_OBJECTS_ID)  return true;
    return false;
}


/*
 ***********************************************************************
 */

/**
 *  Finds references to object 'o' in a subtree rooted at object
 *  'p'. Skips 'deleted objects' library. Avoids circular group
 *  references using temporary flags set in objects this method
 *  visits. Results are placed in the set resset. This function
 *  returns the following objects as the results:
 *
 *  - references that point to the object (FWObjectReference,
 * FWServiceReference objects). Note that the reference itself is
 * returned rather than the parent object the reference is a child of.
 * This includes cases where object <o> is a member of a user-defined
 * group and when it is used in a rule element. In both cases reference
 * is returned as part of the resset.
 *
 * - group objects which are parents of the object <o>.
 */
void FWObjectDatabase::findWhereObjectIsUsed(FWObject *o,
                                             FWObject *p,
                                             std::set<FWObject *> &resset)
{
    searchId++;
    _findWhereObjectIsUsed(o, p, resset, searchId);
}

bool FWObjectDatabase::_findWhereObjectIsUsed(FWObject *o,
                                              FWObject *p,
                                              std::set<FWObject *> &resset,
                                              int search_id)
{
    bool res = false;
    if ( _isInIgnoreList(p)) return res;
    if (p->size()==0) return res;
    if (p->getInt(".search_id")==search_id) return p->getBool(".searchResult");

// set flags to break indefinite recursion in case we encounter circular groups
    p->setInt(".search_id", search_id);
    p->setBool(".searchResult", false);

    Interface *intf = Interface::cast(p);
    if (intf)
    {
        string netzone_id = intf->getStr("network_zone");
        FWObject *netzone = findInIndex(FWObjectDatabase::getIntId(netzone_id));
        if (netzone == o)
        {
            resset.insert(p);
            res = true;
        }
    }

    PolicyRule *rule = PolicyRule::cast(p);
    if (rule)
    {
        if (rule->getAction() == PolicyRule::Branch)
        {
            FWObject *ruleset = rule->getBranch();
            if (o==ruleset)
            {
                resset.insert(p);
                res = true;
            }
        }

        if (rule->getTagging())
        {
            FWObject *tagobj = rule->getTagObject();
            if (o==tagobj)
            {
                resset.insert(p);
                res = true;
            }
        }
    }

    NATRule *nat_rule = NATRule::cast(p);
    if (nat_rule && nat_rule->getAction() == NATRule::Branch)
    {
        FWObject *ruleset = nat_rule->getBranch();
        if (o==ruleset)
        {
            resset.insert(p);
            res = true;
        }
    }

    if (Firewall::isA(o) && Cluster::isA(p))
    {
        if (Cluster::cast(p)->hasMember(Firewall::cast(o)))
        {
            resset.insert(p);
            res = true;
        }
    }
   
    FWObject::iterator i1 = p->begin();
    for ( ; i1!=p->end(); ++i1)
    {
        FWReference  *ref = FWReference::cast(*i1);
        if (ref!=nullptr)
        {  // child is a reference
            FWObject *g = ref->getPointer();
            if (o->getId() == g->getId())
            {
                resset.insert(*i1);
                res = true;
            }
        }
        else    // child is a regular object, not a reference
        {
            if (o->getId() == (*i1)->getId())
            {
                resset.insert(p);
                res = true;
                // still run search recursively, the same object could be
                // used in rules if it is a firewall
            }
            _findWhereObjectIsUsed(o, *i1, resset, search_id);
        }
    }

    p->setBool(".searchResult", res);
    return res;
}

