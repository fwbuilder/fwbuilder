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
#include "fwbuilder/Group.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/ClusterGroup.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/FWIntervalReference.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace libfwbuilder;


class FWObjectTreeScanner {

    FWObjectDatabase         *treeRoot;
    map<int,FWObject*>        srcMap;
    map<int,FWObject*>        dstMap;
    FWObjectDatabase::ConflictResolutionPredicate *crp;
    bool                         defaultCrp;
    int                          reference_object_id_offset;
    void walkTree(map<int,FWObject*> &m,FWObject *root);
    void addRecursively(FWObject *src);

    public:

    FWObjectTreeScanner(FWObject *r,
                        FWObjectDatabase::ConflictResolutionPredicate *_crp=nullptr)
    {
        reference_object_id_offset = 1000000;
        treeRoot=FWObjectDatabase::cast(r);
        defaultCrp=false;
        if (_crp==nullptr)
        {
            crp=new FWObjectDatabase::ConflictResolutionPredicate();
            defaultCrp=true;
        }
        else            crp=_crp;
    }
    ~FWObjectTreeScanner() { if (defaultCrp) delete crp; }

    void scanAndAdd(FWObject *dst,FWObject *source);
    void merge(     FWObject *dst,FWObject *source);
};

/*
 * why does FWReference not have an 'id' attribute ? Marginal savings
 * in the size of the data file turns into a major headache in coding
 * things such as tree merge.
 *
 * Here, in effect, I am artifically adding IDs to references.
 */
void FWObjectTreeScanner::walkTree(map<int,FWObject*> &m,
                                   FWObject *root)
{
    if (root->haveId())  m[root->getId()]=root;

    if (FWReference::cast(root)!=nullptr)
    {
        FWReference *r=FWReference::cast(root);
        // need to add reference to the map, but references do not have
        // their own Id. Create new id using id of the object reference
        // points to, plus some offset.
        // I can not just generate new uniq id because I need to be able
        // to find this object later, and for that its id must be predictable.
        m[reference_object_id_offset+r->getPointerId()]=root;
    }

    for (FWObject::iterator i=root->begin(); i!=root->end(); i++)
    {
        walkTree(m, *i);
    }
}

void FWObjectTreeScanner::addRecursively(FWObject *src)
{
    if (src==nullptr) return;

    if (treeRoot->getId()==src->getId()) return ;

    addRecursively(src->getParent());

    if (dstMap[src->getId()]==nullptr)
    {
        // last arg.==false : do not call method init() of the new object to
        // make sure it doesn't create its children
        FWObject *o1 = treeRoot->create(src->getTypeName(), -1, false);
        FWObject *pdst = dstMap[src->getParent()->getId()];
        assert(pdst!=nullptr);

        // no validation is necessary - this copies existing tree
        pdst->add(o1, false);

        if (src->size()==0) o1->shallowDuplicate(src, false);
        else 
        {
            if (Firewall::isA(src) ||
                Host::isA(src) ||
                Interface::isA(src) )  o1->duplicate(src, false);
            else
            {
               /* copy system groups (folders) partially, but user's
                * groups should be copied as a whole. There is no
                * definite and simple * way to tell them apart, except
                * that user groups contain references * while system
                * groups contain objects.
                */
                if (Group::cast(src)!=nullptr && FWReference::cast(src->front())!=nullptr)
                    o1->duplicate(src, false);
                else
                    o1->shallowDuplicate(src, false);
            }
        }

        walkTree( dstMap , o1 );  // there are children objects if we did a deep copy
    }
}

/*
 * scans tree treeRoot and finds references to missing objects. Tries
 * to add an object from 'source' by adding all missing objects
 * between it and a tree root. Method addRecusrively climbs up from
 * the objects that it needs to add to the root by doing recursive
 * calls to itself, then while it exist invokations it adds objects to
 * the tree. Since added objects are appended to the end of each level
 * of the tree, method scanAndAdd finds them later. For example, if
 * 'Standard' library was added because some standard object was
 * referenced but missing, this library will be found in the loop in
 * scanAndAdd even if it wasn't there when the loop started. This
 * ensures that this method will pull in any objects referenced from
 * objects it included, recursively.
 *
 */
void FWObjectTreeScanner::scanAndAdd(FWObject *dst,FWObject *source)
{
    if (dst==nullptr)
    {
        dst=treeRoot;
        walkTree(dstMap,treeRoot);
        walkTree(srcMap, source);
    }

    for (FWObject::iterator i=dst->begin(); i!=dst->end(); i++)
    {
        FWObject *o1=*i;
        if (FWReference::cast(o1)!=nullptr)
        {
            int pid   = FWReference::cast(o1)->getPointerId();
            FWObject *o2   = dstMap[pid];

            if (o2==nullptr)
            {
                FWObject *osrc = srcMap[ pid ];
                if (osrc==nullptr)
                    cerr << "Object with ID=" << pid
                         << " (" << FWObjectDatabase::getStringId(pid) << ") "
                         << " disappeared" << endl;
                else
                    addRecursively( osrc);
            }
        } else
            scanAndAdd( o1 , source );
    }
    // TODO: do the same for the objects referenced by
    // rule actions Branch and Tag - find those objects and add.
    // Wrap operations with network_zone in methods of class Interface,
    // setNetworkZone(FWObject*) getNetworkZone()
    // (Just like Rule::getBranch Rule::setBranch)
    //
    if (Interface::isA(dst))
    {
        string sid = dst->getStr("network_zone");
        if ( !sid.empty() )
        {
            int pid = FWObjectDatabase::getIntId(sid);
            FWObject *o2 = dstMap[pid];
            if (o2==nullptr)
            {
                FWObject *osrc = srcMap[ pid ];
                addRecursively( osrc);
            }
        }
    }

}

// #define DEBUG_MERGE 1

void FWObjectTreeScanner::merge(FWObject *dst, FWObject *src)
{
    int dobjId = FWObjectDatabase::DELETED_OBJECTS_ID;

    if (dst==nullptr)
    {
        /* dst == nullptr on the first call to this function */

        dst = treeRoot;

        walkTree(dstMap,treeRoot);
        walkTree(srcMap, src);
        

        /**
         * find deleted objects library in src and check if any object
         * from it is present in dst
         */
        FWObjectDatabase *dstroot = dst->getRoot();

        /*
         * find deleted objects library in dst and delete objects from
         * it if they are present and not deleted in src
         */
        list<FWObject*> deletedObjects;
        FWObject *dstdobj = dstroot->findInIndex( dobjId );
        if (dstdobj)
        {
            for (FWObject::iterator i=dstdobj->begin(); i!=dstdobj->end(); i++)
            {
                FWObject *sobj = srcMap[ (*i)->getId() ];
                if(sobj!=nullptr && sobj->getParent()->getId()!=dobjId)
                    deletedObjects.push_back(*i);
            }
            for (FWObject::iterator i=deletedObjects.begin(); i!=deletedObjects.end(); i++)
            {
                dstroot->recursivelyRemoveObjFromTree( *i );
                dstMap[ (*i)->getId() ] = nullptr;
            }
        }
    }

    for (FWObject::iterator i=src->begin(); i!=src->end(); i++)
    {
        /*
         * commented 12/04/04. We now delete "deleted objects"
         * from libraries we are merging in before calling
         * FWObjectDatabase::merge.  Ignoring "Deleted objects" here
         * caused problems; in particular, deleted objects disappeared
         * from a data file whenever it was opened. This happened
         * because we merged user's data file into standard objects
         * tree, so user's file was _source_ here, and deleted objects
         * in it were ignored.
         */
//        if ((*i)->getId()==dobjId) continue;

        FWObject *dobj;
        FWObject *sobj;
        if (FWReference::cast( *i ))
        {
            FWReference *r=FWReference::cast(*i);
            dobj= dstMap[reference_object_id_offset + r->getPointerId()];
        } else dobj= dstMap[ (*i)->getId() ];

        if (dobj==nullptr)
        {
            sobj = *i;
            FWObject *o1 = treeRoot->create( sobj->getTypeName());

            FWObject *pdst = dstMap[ src->getId() ];
            assert(pdst!=nullptr);

            // no validation is necessary - this copies existing tree
            pdst->add(o1, false);

#ifdef DEBUG_MERGE
            cerr << "--------------------------------" << endl;
            cerr << "merge: duplicate #1 " << endl;
            cerr << "dobj: " << endl;
            o1->dump(true,true);
            cerr << endl;
            cerr << "sobj: " << endl;
            sobj->dump(true,true);
#endif

            o1->duplicate( sobj, false); // copy IDs as well


#ifdef DEBUG_MERGE
            cerr << "duplicate #1 done" << endl;
#endif

/* there may be children objects since we did a deep copy         */
            walkTree( dstMap , o1 );

        } else
        {
/* need to compare objects, looking into attributes. This is different
 * from Compiler::operator== operators because fwcompiler assumes that
 * objects with the same ID are equal. Here we specifically look for a
 * case when objects with the same ID have different attributes.
 */
            if (dobj->cmp(*i, true)) continue; // compare recursively

/* such object exists in destination tree but is different Since we
 * traverse the tree from the root towards leaves, it won't help much
 * if we ask the user if they want to overwrite the old library or a
 * high-level system group only because a single object somewhere deep
 * down the tree is different. Need to traverse the tree further until
 * the actual object that differs is found.
 *
 * There still is a problem because we do want to ask the user if the
 * group we are looking at is user-defined as opposed to our standard
 * top level one. There is no reliable way to distinguish them though
 * because both are represented by the same class. We use simple hack:
 * all children of our system groups are regular objects, while
 * children of user-defined groups are always references.
 */


            if (Group::cast(dobj)!=nullptr)
            {
                // at one point I've got bunch of data files where
                // DeletedObjects library contained references for
                // some reason. This should not happen, but at the
                // same time this is valid file structure so the code
                // should be able to handle it.
                if (dobj->getId()==FWObjectDatabase::DELETED_OBJECTS_ID)
                    merge( dobj , *i );
                else
                {

                    FWObject *firstChild=nullptr;
                    if (dobj->size()>0)         firstChild= dobj->front();
                    else
                    {
                        if ( (*i)->size()>0 )   firstChild= (*i)->front();
                    }
                    if (firstChild==nullptr || FWReference::cast(firstChild)!=nullptr)
                    {
                        if (crp!=nullptr && crp->askUser( dobj, *i ))
                        {
#ifdef DEBUG_MERGE
                            cerr << "--------------------------------" << endl;
                            cerr << "merge: duplicate #2 " << endl;
                            dobj->dump(true,true);
                            cerr << endl;
#endif
                            dobj->duplicate( (*i), false );
                        }
                    } else merge( dobj , *i );
                }
            }
            else
            {
                if (crp!=nullptr && crp->askUser( dobj, *i ))
                {
#ifdef DEBUG_MERGE
                    cerr << "--------------------------------" << endl;
                    cerr << "merge: duplicate #3 " << endl;
                    dobj->dump(true,true);
                    cerr << endl;
#endif
                    dobj->duplicate( (*i), false );
                }
            }
        }
    }
}


FWObjectDatabase* FWObjectDatabase::exportSubtree( const list<FWObject*> &libs )
{
    FWObjectDatabase *ndb = new FWObjectDatabase();

    ndb->busy = true;

    for (list<FWObject*>::const_iterator i=libs.begin(); i!=libs.end(); i++)
    {
        FWObject *lib = *i;
        FWObject *nlib = ndb->create(lib->getTypeName());
        // no validation is necessary - this copies existing tree
        ndb->add(nlib, false);
        *nlib = *lib;
    }

    FWObjectTreeScanner scanner(ndb);
    scanner.scanAndAdd(nullptr, this);

    ndb->busy = false;

    return ndb;
}

FWObjectDatabase* FWObjectDatabase::exportSubtree( FWObject *lib )
{
    FWObjectDatabase *ndb = new FWObjectDatabase();

    ndb->busy = true;

    FWObject *nlib = ndb->create(lib->getTypeName());
    // no validation is necessary - this copies existing tree
    ndb->add(nlib, false);
    *nlib = *lib;

    FWObjectTreeScanner scanner(ndb);
    scanner.scanAndAdd(nullptr, this);

    ndb->busy = false;

    return ndb;
}

/**
 * check if source and destination files have objects with the
 * same ID. To do this, compare keys in obj_index in this and
 * ndb. 
 */
void FWObjectDatabase::findDuplicateIds(FWObjectDatabase *ndb, set<int> &dupids)
{
    FWObjectDatabase *db1;
    FWObjectDatabase *db2;
    if (obj_index.size() > ndb->obj_index.size()) 
    {
        db1 = ndb;
        db2 = this;
    } else
    {
        db2 = ndb;
        db1 = this;
    }
    for (map<int, FWObject*>::iterator it=db1->obj_index.begin();
         it!=db1->obj_index.end(); ++it)
    {
        int id = it->first;
        if (db2->obj_index.count(id) != 0)
        {
            // skip standard IDs
            if (id <= DELETED_OBJECTS_ID) continue;
            FWObject *obj = db1->findInIndex(id);
            assert(obj);
            if (obj->getLibrary()->getId() == STANDARD_LIB_ID ||
                obj->getLibrary()->getId() == DELETED_OBJECTS_ID) continue;
            dupids.insert(id);
        }
    }
}

void FWObjectDatabase::merge( FWObjectDatabase *ndb,
                              ConflictResolutionPredicate *crp)
{
    busy = true;
    setIgnoreReadOnlyFlag(true);

    FWObjectTreeScanner scanner(this, crp);
    scanner.merge(nullptr, ndb);

    setIgnoreReadOnlyFlag(false);
    busy = false;
}

/**
 * Copy <source> object and all its children, recursively, into <this>
 * object tree starting from <target>. <target> is a parent of the copy
 * of <source> that will be created.
 * Store ID mapping in <id_map> (as a dictionary old_id -> new_id)
 */
FWObject* FWObjectDatabase::recursivelyCopySubtree(FWObject *target,
                                                   FWObject *source,
                                                   std::map<int,int> &id_map)
{
    char s[64];
    snprintf(s, sizeof(s), ".copy_of_%p", source->getRoot());
    string dedup_attribute = s;

    FWObject *nobj = _recursively_copy_subtree(target, source, id_map,
                                             dedup_attribute);

    fixReferences(nobj, id_map);

    // one more pass to fix references in other firewalls and groups
    // we might have copied.
    for (map<int,int>::const_iterator i=id_map.begin(); i!=id_map.end(); ++i)
    {
        int new_id = i->second;   // new id
        FWObject *new_obj = findInIndex(new_id);
        if (Firewall::cast(new_obj) || Group::cast(new_obj))
            fixReferences(new_obj, id_map);

    }

    return nobj;
}

/*
 * fix references in children of obj according to the map_ids which
 * maps old IDs to the new ones. Return the number of fixed references.
 */
int FWObjectDatabase::fixReferences(FWObject *obj, const map<int,int> &map_ids)
{
    int total_counter = 0;
    for (map<int,int>::const_iterator it=map_ids.begin(); it!=map_ids.end(); ++it)
        total_counter += obj->replaceRef(it->first, it->second);
    return total_counter;
}

FWObject* FWObjectDatabase::_recursively_copy_subtree(
    FWObject *target, FWObject *source, std::map<int,int> &id_map,
    const string &dedup_attribute)
{
    target->checkReadOnly();

    // TODO: get rid of references in attributes, they suck. ticket #1004
    if (Interface::cast(source))
    {
        int nzid = FWObjectDatabase::getIntId(source->getStr("network_zone"));
        // check if we have seen old_ptr_obj already.
        if (nzid!= 0 && id_map.count(nzid) == 0 && findInIndex(nzid)==nullptr)
        {
            FWObject *netzone = source->getRoot()->findInIndex(nzid);
            if (netzone)
                _copy_foreign_obj_aux(target, netzone, id_map, dedup_attribute);
        }
    }

    if (ClusterGroup::cast(source))
    {
        int miface_id = FWObjectDatabase::getIntId(source->getStr("master_iface"));
        FWObject *miface = source->getRoot()->findInIndex(miface_id);
        if (miface) _copy_foreign_obj_aux(target, miface, id_map, dedup_attribute);
    }

    FWObject *nobj = create(source->getTypeName());
    nobj->clearChildren();
    nobj->shallowDuplicate(source, true);
    id_map[source->getId()] = nobj->getId();
    nobj->setInt(dedup_attribute, source->getId());
    // no validation is necessary - this copies existing tree
    target->add(nobj, false);

    // copy interfaces and options objects before rule sets
    for(list<FWObject*>::iterator m=source->begin(); m!=source->end(); ++m) 
    {
        FWObject *old_obj = *m;
        if (RuleSet::cast(old_obj)!=nullptr) continue;
        if (FWReference::cast(old_obj)!=nullptr) continue;
        _recursively_copy_subtree(nobj, old_obj, id_map, dedup_attribute);
    }

    for(list<FWObject*>::iterator m=source->begin(); m!=source->end(); ++m) 
    {
        FWObject *old_obj = *m;
        if (id_map.count(old_obj->getId()) > 0) continue;

        if (FWReference::cast(old_obj))
        {
            FWReference *old_ref_obj = FWReference::cast(old_obj);
            FWObject *old_ptr_obj = old_ref_obj->getPointer();

            FWObject *n_ptr_obj = nullptr;

            // check if we have seen old_ptr_obj already.
            if (id_map.count(old_ptr_obj->getId()) > 0)
            {
                n_ptr_obj = findInIndex(id_map[old_ptr_obj->getId()]);
                nobj->addRef(n_ptr_obj);
                continue;
            }

            // search for old_ptr_obj in the index. If it is found, we do not
            // need to copy it and its ID is valid (perhaps standard object?)
            n_ptr_obj = findInIndex(old_ptr_obj->getId());
            if (n_ptr_obj != nullptr)
            {
                nobj->addRef(n_ptr_obj);
                continue;
            }

            // Check if we have already copied the same object before
            char s[64];
            snprintf(s, sizeof(s), "%d", old_ptr_obj->getId());
            n_ptr_obj = findObjectByAttribute(dedup_attribute, s);
            if (n_ptr_obj)
            {
                nobj->addRef(n_ptr_obj);
                continue;
            }

            // Need to create a copy of old_ptr_obj and put it in the
            // same place in the tree.
            // Problem: what if old_ptr_obj is interface or an address of
            // interface or a rule etc ? Check isPrimaryObject().
            // 
            _copy_foreign_obj_aux(nobj, old_ptr_obj, id_map, dedup_attribute);

        } else
            _recursively_copy_subtree(nobj, old_obj, id_map, dedup_attribute);
    }

    return nobj;
}

void FWObjectDatabase::_copy_foreign_obj_aux(
    FWObject *target, FWObject *source,
    map<int,int> &id_map, const std::string &dedup_attribute)
{
    FWObject *parent_obj = source;
    while (parent_obj && !parent_obj->isPrimaryObject())
        parent_obj = parent_obj->getParent();

    // check if this parent (which is a primary object) is
    // unknown. If it is known or exist in our tree, no need
    // to create a copy.
    if (parent_obj && 
        id_map.count(parent_obj->getId()) == 0 &&
        !Library::isA(parent_obj))
    {
        FWObject *new_parent = reproduceRelativePath(
            target->getLibrary(), parent_obj);

        // (parent_obj at this point is either pointer
        // to the same obj or pointer to its parent
        // object that we can copy as a whole. The latter
        // happens if obj is an interface or address
        // of an interface.
        new_parent = _recursively_copy_subtree(new_parent,
                                               parent_obj,
                                               id_map,
                                               dedup_attribute);
        // we just copied old object to the target data tree.
        // Copy of obj is either new_parent, or one of its
        // children. In the process of making this copy,
        // its ID should have been added to id_map.
        assert(id_map.count(source->getId()) > 0);

        FWObject *n_ptr_obj = new_parent->getById(id_map[source->getId()], true);
        target->addRef(n_ptr_obj);
    }

}

/**
 * Create groups to reproduce path inside given library. If groups
 * with required names exist, do nothing. Return pointer to the
 * last object created to copy the path. Do not copy <source> object.
 * This means returned object can be a parent for the copy of <source>.
 */
FWObject* FWObjectDatabase::reproduceRelativePath(FWObject *lib,
                                                  const FWObject *source)
{
    list<FWObject*> path;
    
    FWObject *p = source->getParent();
    while (p && !Library::isA(p))
    {
        path.push_front(p);
        p = p->getParent();
    }

    FWObject *target = lib;
    FWObject *nobj;
    for (list<FWObject*>::iterator p=path.begin(); p!=path.end(); ++p)
    {
        FWObject *obj = *p;
        nobj = target->findObjectByName(obj->getTypeName(), obj->getName());
        if (nobj==nullptr)
        {
            nobj = create(obj->getTypeName());
            nobj->shallowDuplicate(obj, false);
            // no validation is necessary - this copies existing tree
            target->add(nobj, false);
        }
        target = nobj;
    }
    return target;
}

FWObject& FWObjectDatabase::duplicate(const FWObject *obj,
                                      bool preserve_id)
{
    setIgnoreReadOnlyFlag(true);
    FWObject &o = FWObject::duplicate(obj, preserve_id);
    setIgnoreReadOnlyFlag(false);
    return o;
}

