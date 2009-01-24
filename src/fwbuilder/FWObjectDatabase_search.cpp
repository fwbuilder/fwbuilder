/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland vadim@fwbuilder.org

  $Id: FWObjectDatabase_tree_ops.cpp 233 2008-12-23 05:27:43Z vadim $

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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/FWObject.h>
#include <fwbuilder/FWObjectDatabase.h>

#include <fwbuilder/Library.h>
#include <fwbuilder/Host.h>
#include <fwbuilder/Firewall.h>
#include <fwbuilder/Group.h>
#include <fwbuilder/Interface.h>
#include <fwbuilder/FWReference.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWServiceReference.h>
#include <fwbuilder/FWIntervalReference.h>
#include <fwbuilder/Rule.h>
#include <fwbuilder/RuleSet.h>

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
       if (ref==NULL) 
       {
           res.insert(*i);
           continue;
       }
       
       obj=ref->getPointer();
       sg=Group::cast(obj);
       
       if (sg==NULL)
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
Firewall* FWObjectDatabase::_findFirewallByNameRecursive(FWObject* db,
                                         const string &name) throw(FWException)
{
    if (Firewall::isA(db) &&
        db->getName()==name &&
        db->getParent()->getId()!=FWObjectDatabase::DELETED_OBJECTS_ID)
        return static_cast<Firewall*>(db);

    list<FWObject*>::iterator j;
    for(j=db->begin(); j!=db->end(); ++j)     
    {
        FWObject *o=*j;

        o = _findFirewallByNameRecursive(o,name);
        if(o) return static_cast<Firewall*>(o);
    }
    if (db==this)
        throw FWException("Firewall object '"+name+"' not found");
    return NULL; // not found
}

Firewall* FWObjectDatabase::findFirewallByName(const string &name) throw(FWException)
{
    return _findFirewallByNameRecursive(this,name);
}

//#define DEBUG_WHERE_USED 1

bool FWObjectDatabase::_isInIgnoreList(FWObject *o)
{
    if (FWOptions::cast(o)!=NULL) return true;
    if (o->getId()==FWObjectDatabase::DELETED_OBJECTS_ID)  return true;
    return false;
}

#if 0

/**
 *  Finds which firewalls and groups use object 'o' in a subtree
 *  rooted at object 'p'. Skips 'deleted objects' library. Avoids
 *  circular group references using temporary flags set in objects
 *  this method visits.
 */
void FWObjectDatabase::findWhereUsed(FWObject *o,
                                     FWObject *p,
                                     set<FWObject *> &resset)
{
    set<FWObject*> results;
    searchId++;
    _findWhereUsed(o, p, results);
    for (set<FWObject*>::iterator it=results.begin(); it!=results.end(); ++it)
    {
        FWObject *obj = *it;
        if (RuleSet::cast(obj))
        {
            resset.insert(obj->getParent());
            continue;
        }
        resset.insert(obj);
    }
}

bool FWObjectDatabase::_findWhereUsed(FWObject *o,
                                      FWObject *p,
                                      set<FWObject *> &resset)
{
    bool res=false;
    if ( _isInIgnoreList(p)) return res;

#if DEBUG_WHERE_USED
    cerr << "_findWhereUsed"
         << "  o=" << o
         << "  " << o->getName()
         << "  p=" << p
         << "  " << p->getName()
         << " (" << p->getTypeName() << ")"
         << "  resset.size()=" << resset.size()
         << "  p->getInt(\".searchId\")=" << p->getInt(".searchId")
         << "  searchId=" << searchId
         << endl;
#endif        

    if (o==p)
    {
        resset.insert(p);
        res = true;
    }

    if (p->size()==0) return res;

    if (p->getInt(".searchId")==searchId)
    {
        res=p->getBool(".searchResult");
        return res;
    }

// set flags to break indefinite recursion in case we encounter circular groups
    p->setInt(".searchId", searchId);
    p->setBool(".searchResult", res);   // res==false at this time
 
    PolicyRule *rule = PolicyRule::cast(p);
    if (rule)
    {
        switch (rule->getAction()) {
        case PolicyRule::Tag:
        {
            FWObject *tagobj = rule->getTagObject();
            if (o==tagobj)
            {
                resset.insert(p);
                res = true;
                break;
            }
            break;
        }
        case PolicyRule::Branch:
        {
            FWObject *ruleset = rule->getBranch();
            if (o==ruleset)
            {
                resset.insert(p);
                res = true;
                break;
            }
            break;
        }
        default: ;
        }
    }
   
    FWObject::iterator i1 = p->begin();
    for ( ; i1!=p->end(); ++i1)
    {
        if ((*i1)->getId()==FWObjectDatabase::DELETED_OBJECTS_ID) continue;

        FWReference  *ref = FWReference::cast(*i1);
        if (ref!=NULL)
        {  // child is a reference
            FWObject *g = ref->getPointer();
            if (_findWhereUsed(o, g, resset))
            {
                resset.insert(p);
                res = true;
            }
        }
        else    // child is a regular object, not a reference
        {
            if (_findWhereUsed(o, *i1, resset))
            {
                // do not add rule to the results if rule element
                // matches because rule element should already be there.
                if (Rule::cast(p) && RuleElement::cast(*i1)==NULL)
                    resset.insert(p);
                res = true;
            }
        }
    }

    p->setBool(".searchResult",res);
    //if (res) resset.insert(p);
    return res;
}
#endif



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
 
    PolicyRule *rule = PolicyRule::cast(p);
    if (rule)
    {
        switch (rule->getAction()) {
        case PolicyRule::Tag:
        {
            FWObject *tagobj = rule->getTagObject();
            if (o==tagobj)
            {
                resset.insert(p);
                res = true;
            }
            break;
        }
        case PolicyRule::Branch:
        {
            FWObject *ruleset = rule->getBranch();
            if (o==ruleset)
            {
                resset.insert(p);
                res = true;
            }
            break;
        }
        default: ;
        }
    }
   
    FWObject::iterator i1 = p->begin();
    for ( ; i1!=p->end(); ++i1)
    {
        FWReference  *ref = FWReference::cast(*i1);
        if (ref!=NULL)
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
            } else
            {
                _findWhereObjectIsUsed(o, *i1, resset, search_id);
            }
        }
    }

    p->setBool(".searchResult", res);
    return res;
}

