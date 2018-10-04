/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef  __FWOBJECT_HH_FLAG__
#define  __FWOBJECT_HH_FLAG__


#include <time.h>
#include <string>
#include <list>
#include <map>
#include <set>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "fwbuilder/FWException.h"
#include "fwbuilder/ObjectMatcher.h"
#include "fwbuilder/Dispatch.h"


namespace libfwbuilder
{

class FWObject;
class FWObjectDatabase;
class FWReference;
class FWObjectTypedChildIterator;
class FWObjectFindPredicate;

#define DECLARE_FWOBJECT_SUBTYPE(name) \
   static    const char *TYPENAME; \
   virtual   std::string getTypeName() const { return TYPENAME; } \
   static bool isA(const FWObject *o) { return o && o->getTypeName()==TYPENAME; }  \
   static name* cast(FWObject *o) { return dynamic_cast<name*>(o); } \
   static const name* constcast(const FWObject *o) { return dynamic_cast<const name*>(o); }

#define DECLARE_DISPATCH_METHODS(classname) \
    virtual void* dispatch(Dispatch* _d, void* _a1) \
        {return _d->dispatch(this, _a1); } \
    virtual void* dispatch(Dispatch* _d, const void* _a1) \
        {return _d->dispatch(this, _a1); } \
    virtual void* dispatch(Dispatch* _d, void* _a1) const \
        {return _d->dispatch(this, _a1); } \
    virtual void* dispatch(Dispatch* _d, const void* _a1) const \
        {return _d->dispatch(this, _a1); } \
    virtual const void* const_dispatch(Dispatch* _d, void* _a1) const \
        {return _d->const_dispatch(this, _a1); } \
    virtual const void* const_dispatch(Dispatch* _d, const void* _a1) const \
        {return _d->const_dispatch(this, _a1); }


/**
 * This class represents base class for all
 * objects we are dealing with.
 *
 * FWObject can have children, that is other objects of the same class
 * or derived classes which are included in this one
 */
class FWObject : public std::list<FWObject*>
{
    friend class libfwbuilder::FWObjectDatabase;

private:

    int            ref_counter;

    FWObject      *parent;

    /**
     * dbroot is a pointer to the database root. Method getRoot
     * returns this.  We need this pointer because most of the time we
     * create an object first, make some manipulations with it (such
     * as create its children and/or add copies of children objects)
     * and only then put it in its place in the tree. This means the
     * object does not have valid parent pointer for a while, yet we
     * must be able to work with it. Many methods rely on being able
     * to reach the root of the tree (FWObjectDatabase object) for
     * things like setting 'dirty' flag etc. Therefore we need to
     * store the pointer to the root as soon as possible after we
     * create the object. The 'parent' pointer, on the other hand, is
     * stored once the object is added to the tree.
     *
     * dbroot is assigned by method FWObjectDatabase::create 
     */
    FWObjectDatabase *dbroot;
    int id;
    bool ro;
    std::string name;
    std::string comment;

    static std::string NOT_FOUND;

    time_t creation_time;
    std::set<std::string> keywords;

    static std::string dataDir;

protected:

    std::string xml_name;
    bool busy;
    bool dirty;
    
    std::map<std::string, std::string> data;
    std::map<std::string, void*> private_data;

    void clearRefCounter() { ref_counter=0; }

    void _removeAll(FWObject *rm);
    void _removeAllRef(FWObject *rm);
    void _adopt(FWObject *obj);   // increments reference
    void _findDependencies_internal(FWObject *obj,
                                    std::list<FWObject*> &deps,
                                    int anti_loop_id);

    void setRO(bool f) { ro = f; }

    virtual void replaceReferenceInternal(int oldfw_id, int newfw_id, int &counter);

    /**
     * Finds direct child of this object with given name.
     */
    FWObject* _find(const std::string& name) const;

    /**
     * Finds all references to the object 'obj' in object
     * tree starting from 'this' and address them to resulting
     * set.
     */
    virtual void findAllReferences(const FWObject *obj, std::set<FWReference*> &res);

    /**
     * internal method: update references to other objects this object
     * might keep in attributes. Example of this is reference to
     * branch rule set that is stored as its string ID in RuleOptions
     * object.  Eventually we must fix these and make these references
     * either use FWReference child object or be normal C++
     * pointers. Meanwhile, this virtual method is called from
     * FWObjectDatabase::_set_predictable_str_ids_recursively() when string
     * ids are replaced (and possibly from other places for similar reasons).
     */
    virtual void updateNonStandardObjectReferences();

    FWObject();

    // special constructor used to create FWObjectDatabase objects
    FWObject(bool new_id);
    
public:

    DECLARE_FWOBJECT_SUBTYPE(FWObject);

    DECLARE_DISPATCH_METHODS(FWObject);

    class tree_iterator
    {
        friend class libfwbuilder::FWObject;

        FWObject *node;

      public:
        tree_iterator()                        { node=nullptr;    }
        tree_iterator(FWObject *_n)            { node=_n;      }
        tree_iterator(const tree_iterator &ti) { node=ti.node; }
        FWObject* operator*() { return node; }
        tree_iterator& operator++();
        tree_iterator  operator++(int);
        tree_iterator& operator=(const tree_iterator &ti)
            { node=ti.node; return *this; }
        bool operator==(const tree_iterator& i) const;
        bool operator!=(const tree_iterator& i) const;
    };
    
 
    tree_iterator tree_begin();
    tree_iterator tree_end();


    FWObject(const FWObject &copy);

    /**
     * This method should create any standard mandatory child objects
     * the object might need. The function should not require that the
     * object be already added to the object tree but can use provided
     * pointer to FWObjectDatabase to create other objects it might
     * need to add as children.
     */
    virtual void init(FWObjectDatabase *root);
    
    int  getId() const;
    void setId(int i);
    bool haveId() { return (id != -1); }

    /**
     * getRO() simply returns value of private member ro. Note the
     * difference between _getRO and isReadOnly() - the latter checks
     * if any parent of this object is read-only in addition to
     * checking if this object is read-only itself.
     */
    bool getRO() const { return ro; }
    
    virtual void fromXML(xmlNodePtr xml_parent_node);
    virtual xmlNodePtr toXML(xmlNodePtr xml_parent_node);
    xmlNodePtr toXML(xmlNodePtr xml_parent_node, bool process_children);

    /**
     *  Rarely used feature: we can change the name of XML element
     *  represented by the class derived from FWObject if we want to. For
     *  example, used in all "Any"  objects so we did not have to create extra
     *  classes
     * 
     */
    void setXMLName (const std::string &);

    virtual ~FWObject();

    int ref()   { ++ref_counter; return(ref_counter); }
    int unref() { --ref_counter; return(ref_counter); }
    int getRefCounter() { return(ref_counter); }

    /**
     * It is same as calling duplicate(x, FALSE);
     */
    virtual FWObject& operator=(const FWObject &);

    /**
     * This method copies content of object 'x' in the object 'this'.
     * depending on 'preserve_id' flag, Id are either copied or new
     * ones are issued. All children of 'this' are destroyed and new ones
     * are created recursively as copies of corresponding children of obj.
     */
    virtual FWObject& duplicate(const FWObject *obj,
                                bool preserve_id = true);
    
    /**
     * This method works just like  duplicate, except it does not destroy
     * or change children of 'this'.
     */
    virtual FWObject& shallowDuplicate(const FWObject *obj, bool preserve_id = true);

    /**
     * This method copies all attributes of obj into this, plus
     * FWOptions child object if it exists but no other
     * children. Combination of attributes of the object plus its
     * option child object describe all parameters of the object and
     * should be sufficient to properly roll back or redo any changes.
     * Changes done to its children should be undone or redone using
     * corresponding objects.
     */
    virtual FWObject& duplicateForUndo(const FWObject *obj);

    /**
     * This method creates a copy of object 'x' and adds it to 'this'.
     * Depending on 'preserve_id' flag, Id are either copied or new
     * ones are issued.
     */
    virtual FWObject* addCopyOf(const FWObject *obj, bool preserve_id = true);
    
    /**
     * compares objects. Ignores ID and always looks at
     * attributes. Returns true if objects are equal.
     */
    virtual bool cmp(const FWObject *obj, bool recursive=false);
    
    void Show();
    void Hide();

    FWObject* getParent() const;
    void setParent(FWObject *p);

    bool exists(const std::string &name) const;

    void remStr(const std::string &name);

    const std::string &getStr(const std::string& name) const;
    void setStr(const std::string &name, const std::string &val);

    int getInt(const std::string &name) const;
    void setInt(const std::string &name, int    val);

    bool getBool(const std::string &name) const;
    void setBool(const std::string &name, bool val);
    void setBool(const std::string &name, const std::string &val);

    const std::string &getName() const;
    void setName(const std::string& n);
    
    const std::string &getComment() const;
    void setComment(const std::string& c);

    void storeCreationTime() { creation_time = time(nullptr); }
    time_t getCreationTime() { return creation_time; }

    void setPrivateData(const std::string &key, void *data);
    void* getPrivateData(const std::string &key) const;
    std::map<std::string, void*> &getAllPrivateData();

    
    /**
     * convenience method: returns the name of the library this object belongs to.
     * In fwbuilder2 library is an element directly under FWObjectDatabase; all
     * other objects are children of a Library object
     */
    const std::string &getLibraryName() const;

    /**
     * returns a pointer to the library object 'this' is a part of.
     */
    FWObject* getLibrary() const;

    /**
     *   Returns root of the objects tree this object is part of
     */
    FWObjectDatabase* getRoot() const;
    int getDistanceFromRoot() const;    /* How many parents does this have? */

    /**
     * Sets pointer to the database root
     */
    void setRoot(const FWObjectDatabase *_dbroot)
    { dbroot = (FWObjectDatabase*)_dbroot; }

    /**
     *   Returns a string that represents a path to the object
     *   'this'. Path is built using names of objects above 'this',
     *   all the way to the root of the tree. If parameter 'relative'
     *   is true, the path is built relative to the library 'this' is
     *   a part of (name of the library is not included).
     */
    std::string getPath(bool relative=false, bool detailed=false) const;

    void addAt(int where_id, FWObject *obj);
    virtual void add(FWObject *obj,bool validate=true);
    virtual void insert_before(FWObject *o1,FWObject *obj);
    virtual void insert_after(FWObject *o1,FWObject *obj);

    /**
     * call add(), but first remove() object from its old parent
     */
    virtual void reparent(FWObject *obj,bool validate=true);

    /**
     * In direct children of 'this' swaps all references
     * to o1 with o2 and vice versa.
     */
    virtual void swapObjects(FWObject *o1,FWObject *o2);
      
    /**
     * Finds first instance of 'obj' in children of this
     * object and removes if. If 'obj' reference counter
     * reaches 0 it is also deleted.
     */
    virtual void remove(FWObject *obj, bool delete_if_last=true);

    /**
     * Finds and removes instances of 'obj' recursively in the
     * whole tree under this object
     */
    virtual void removeAllInstances(FWObject *obj);

    /**
     * Adds reference object pointing to 'obj' as a child of 'this'
     */
    virtual void addRef(FWObject *obj);
    
    /**
     * Removes reference to given object among
     * children of 'this'.
     */
    virtual void removeRef(FWObject *obj);

    /**
     * check if 'this' already has reference to obj
     */
    virtual bool hasRef(FWObject *obj);

    /**
     * Removes all references to 'obj' recursively in the whole tree
     * under 'this'
     */
    virtual void removeAllReferences(FWObject *obj);

    /**
     * Finds all references to the object 'obj' in object
     * tree starting from 'this'.
     */
    virtual std::set<FWReference*> findAllReferences(const FWObject *obj);

    /*
     *  Create appropriate reference object for itself.
     *  Different object require different class of reference. e.g.:
     *  for Host reference should be of class FWObjectReference
     *  for Service reference should be of class FWServiceReference
     */
    virtual FWReference* createRef();

    /*
     * verify whether given object type is approppriate as a child
     */
    virtual bool validateChild(FWObject *o);

    /**
     * forcefully destroys all objects in the subtree under this,
     * except this. This method ignores reference counters and simply
     * deletes everything.
     */
    void destroyChildren();

    void clearChildren(bool recursive=true);

    void sortChildrenByName(bool follow_references=false);
    
    /**
     * Walks the tree, looking for objects that are referenced by two parents
     * or those with this->parent == nullptr. Prints report to stderr and
     * returns true if such objects have been found.
     */
    bool verifyTree();

    /**
     * sometimes we need to move object subtree from one object
     * database to another. For example, this can be a useful
     * mechanism to maintain persistent objects between compiler
     * passes. However when the object and its children are added to
     * the new tree, "parent" and "root" pointers in obejcts still
     * point to the old object tree and need to be fixed.
     */
    void fixTree();
    
    int getChildrenCount() const;

    virtual void dump(bool recursive,bool brief,int offset=0) const;
    virtual void dump(std::ostream &f,bool recursive,bool brief,int offset=0) const;

    /**
     * returns true if this is either direct child of obj, or a child
     * of one of its children and so on (this method works recursively).
     */
    bool isChildOf(FWObject *obj);

    bool hasChild(FWObject *obj);
    
    /**
     * Returns object with given ID. If recursive is 'false'
     * search is restricted to direct children of given object
     * and itslef. If it is true, method proceeds to all chidren of all
     * objects starting from current one.
     */
    virtual FWObject* getById(int id, bool recursive=false);


    /**
     * Returns list of direct children of current object whose
     * getTypeName() is equal type_name.  If not found, empty list is
     * returned.
     */
    virtual std::list<FWObject*> getByType(const std::string &type_name) const;

    /**
     * Returns list of children of current object whose getTypeName()
     * is equal type_name.  If not found, empty list is
     * returned. Search recursively including all direct children of this.
     */
    virtual std::list<FWObject*> getByTypeDeep(const std::string &type_name) const;

    /**
     * Returns list of direct children of current object
     * whose getTypeName() same as given.
     * If not found, end() is returned.
     */
    virtual FWObjectTypedChildIterator findByType(
        const std::string &type_name) const;

    /**
     * Returns first of direct children of current object
     * whose getTypeName() same as given or nullptr if not found.
     */
    virtual FWObject* getFirstByType(const std::string &type_name) const;

    /**
     * finds a child object of a given type with a given name
     */
    FWObject* findObjectByName(const std::string &type,
                               const std::string &name);

    /**
     * finds a child object of a given type with an attribute attr
     */
    FWObject* findObjectByAttribute(const std::string &attr,
                                    const std::string &val);

    /**
     * Generic find function, finds all objects in the tree rooted at
     * <this> that match given predicate. Fills @res_list with
     * pointers to the objects it finds.
     */
    std::list<FWObject*> findIf(FWObjectFindPredicate *pred);

    /**
     * if this object has any references as its children, replace IDs
     * these references point to.
     */
    virtual int replaceRef(int oldfw_id, int newfw_id);

    /**
     * recursively find all FWReference objects that are children of
     * this and generate list of pointers to the objects these
     * references point to.
     */
    void findDependencies(std::list<FWObject*> &deps);

    
    virtual void setDirty(bool f);
    virtual bool isDirty();

    /**
     * sets read-only flag for the object. FWObject checks all objects
     * up in the tree before making any change to see if the subtree
     * is read-only. Marking an object read-only automatically makes
     * the whole tree branch under it read-only.
     */
    virtual void setReadOnly(bool f);
    virtual bool isReadOnly();
    virtual void checkReadOnly();

    /**
     * return true if this object can be copied around and put in the
     * tree in one of the system folders. False if it must be a child
     * of some other primary obejct. The latter category includes
     * physAddress, interface, IPv4 and IPv6 if they are children of
     * interface, rulesets, rule elements and rules.
     */
    virtual bool isPrimaryObject() const;

    // Attributes iterator
    std::map<std::string, std::string>::const_iterator dataBegin()
    { return data.begin(); }
    std::map<std::string, std::string>::const_iterator dataEnd()
    { return data.end();   }

    const std::set<std::string> &getKeywords() { return keywords; }
    const std::set<std::string> &getAllKeywords();
    void addKeyword(const std::string &keyword);
    void removeKeyword(const std::string &keyword);
    void clearKeywords();

    static std::string getDataDir() { return dataDir; }
    static void setDataDir(const std::string &dir) { dataDir = dir; }
};

class FWObjectTypedChildIterator
{
    public:

    FWObjectTypedChildIterator();
    FWObjectTypedChildIterator(const FWObjectTypedChildIterator &o);   
    FWObjectTypedChildIterator(const FWObject *o, const std::string &_type_name);
    
    bool operator==(const FWObject::const_iterator& __x) const;
    bool operator!=(const FWObject::const_iterator& __x) const;
    FWObject *operator*() const;
    FWObjectTypedChildIterator& operator++();
    FWObjectTypedChildIterator& operator--();
    
    const FWObject::const_iterator &begin() { return _begin; }
    const FWObject::const_iterator &  end() { return _end  ; }

    protected:
    
    std::string type_name;
    FWObject::const_iterator real_iterator;
    FWObject::const_iterator _begin;
    FWObject::const_iterator _end;

    void init(const FWObject *o, const std::string &_type_name);
};
 
/**
 * Predicate class testing object pointers for name eqivalence.
 * name is obtained by calling getName() method.
 * This class could be used in STL Algoriths find_if, and others.
 */
class FWObjectNameEQPredicate: public std::unary_function<FWObject*, bool>
{
    std::string n;
    
    public:
    
    FWObjectNameEQPredicate(const std::string &name):n(name) {}
    
    bool operator()(const FWObject *o) const 
    {
        return o->getName()==n;
    }
};

struct FWObjectNameCmpPredicate :
    public std::binary_function<FWObject*, FWObject*, bool>
{
    bool follow_references;
    FWObjectNameCmpPredicate(bool follow_refs=false);
    bool operator()(FWObject *a,FWObject *b);
};

class findFWObjectIDPredicate : public std::unary_function<FWObject*, bool>
{
    int _id;
    public:
    findFWObjectIDPredicate(int id):_id(id) {}
    bool operator()(const FWObject *o) const
    {return o->getId()==_id;}
};

/**
 * Predicate class testing object pointers for type name eqivalence.
 * name is obtained by calling getTypeName() method.
 * This class could be used in STL Algoriths find_if, and others.
 */
class FWObjectTypeNameEQPredicate: public std::unary_function<FWObject*, bool>
{
    std::string n;
    
    public:
    
    FWObjectTypeNameEQPredicate(const std::string &name):n(name) {}
    
    bool operator()(const FWObject *o) const 
    {
        return o->getTypeName()==n;
    }
};


/**
 * Generic predicate class testing any object parameters, used for findIf.
 */
class FWObjectFindPredicate
{
    public:
    FWObjectFindPredicate() {}
    virtual ~FWObjectFindPredicate() {}
    virtual bool operator()(FWObject*) const { std::cerr << "#"; return false; }
};



}

#endif // _FWOBJECT_HH
