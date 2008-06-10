/* 

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#include <string>
#include <list>
#include <map>
#include <set>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <fstream>
#include <cstdlib>

#include <fwbuilder/FWException.h>

namespace libfwbuilder
{

class FWObject;
class FWObjectDatabase;
class FWReference;
class FWObjectTypedChildIterator;

#define DECLARE_FWOBJECT_SUBTYPE(name) \
   static    const char *TYPENAME; \
   virtual   std::string getTypeName() const { return TYPENAME; } \
   static bool isA(const FWObject *o) { return o && o->getTypeName()==TYPENAME; }  \
   static name* cast(FWObject *o) { return dynamic_cast<name*>(o); } \
   static const name* constcast(const FWObject *o) { return dynamic_cast<const name*>(o); }



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
    FWObject *dbroot;
    int id;
    std::string name;
    std::string comment;

    static std::string NOT_FOUND;

protected:

    std::string         xml_name;
    bool                init;
    bool                dirty;
    
    std::map<std::string, std::string> data;

    void clearRefCounter() { ref_counter=0; }
    
    void      _removeAll(FWObject *rm);
    void      _removeAllRef(FWObject *rm);
    void      _adopt(FWObject *obj);   // increments reference
    void      _moveToDeletedObjects(FWObject *obj);
    
    /**
     * deletes all objects in the tree regardless of their usage counter
     */
    void deleteChildren();
    
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
    
    FWObject();

    // special constructor used to create FWObjectDatabase objects
    FWObject(bool new_id);
    
    /**
     *  Parameter 'root' is a pointer at the root object of the tree,
     *  we are going to add this newly created object to.
     */
    FWObject(const FWObject *root,bool prepopulate);

public:

    DECLARE_FWOBJECT_SUBTYPE(FWObject);

    class tree_iterator {
        friend class libfwbuilder::FWObject;

        FWObject                       *node;

      public:
        tree_iterator()                        { node=NULL;    }
        tree_iterator(FWObject *_n)            { node=_n;      }
        tree_iterator(const tree_iterator &ti) { node=ti.node; }
        FWObject* operator*() { return node; }
        tree_iterator& operator++();
        tree_iterator  operator++(int);
        tree_iterator& operator=(const tree_iterator &ti) { node=ti.node; return *this; }
        bool operator==(const tree_iterator& i) const;
        bool operator!=(const tree_iterator& i) const;
    };
    
 
    tree_iterator tree_begin();
    tree_iterator tree_end();


    FWObject(const FWObject &copy);

    int  getId() const;
    void setId(int i);
    bool haveId() { return (id != -1); }

    virtual void       fromXML    (xmlNodePtr xml_parent_node) throw(FWException);
    virtual xmlNodePtr toXML      (xmlNodePtr xml_parent_node) throw(FWException);
    xmlNodePtr toXML(xmlNodePtr xml_parent_node, bool process_children) throw(FWException);

    /**
     *  Rarely used feature: we can change the name of XML element
     *  represented by the class derived from FWObject if we want to. For
     *  example, used in all "Any"  objects so we did not have to create extra
     *  classes
     * 
     */
    void setXMLName (const std::string &);

    virtual  ~FWObject();

    int ref()   { ++ref_counter; return(ref_counter); }
    int unref() { --ref_counter; return(ref_counter); }

    /**
     * It is same as calling duplicate(x, FALSE);
     */
    virtual FWObject& operator=(const FWObject &) throw(FWException);

    /**
     * This method copies content of object 'x' in the object 'this'.
     * depending on 'preserve_id' flag, Id are either copied or new
     * ones are issued. All children of 'this' are destroyed and new ones
     * are created recursively as copies of corresponding children of obj.
     */
    virtual FWObject& duplicate(const FWObject *obj, bool preserve_id = true) throw(FWException);
    
    /**
     * This method works just like  duplicate, except it does not destroy
     * or change children of 'this'.
     */
    virtual FWObject& shallowDuplicate(const FWObject *obj, bool preserve_id = true) throw(FWException);

    /**
     * This method creates a copy of object 'x' and adds it to 'this'.
     * Depending on 'preserve_id' flag, Id are either copied or new
     * ones are issued.
     */
    virtual FWObject* addCopyOf(const FWObject *obj, bool preserve_id = true) throw(FWException);
    
    /**
     * compares objects. Ignores ID and always looks at
     * attributes. Returns true if objects are equal.
     */
    virtual bool cmp(const FWObject *obj) throw(FWException);
    
    void      Show();
    void      Hide();

    FWObject* getParent() const;
    void      setParent(FWObject *p);

    bool    exists(const std::string &name) const;

    void    remStr(const std::string &name);

    const std::string &getStr(const std::string& name) const;
    void    setStr(const std::string &name, const std::string &val);

    int     getInt(const std::string &name) const;
    void    setInt(const std::string &name, int    val);

    bool    getBool(const std::string &name) const;
    void    setBool(const std::string &name, bool val);
    void    setBool(const std::string &name, const std::string &val);

    const std::string &getName() const;
    void          setName(const std::string& n);
    
    const std::string &getComment() const;
    void          setComment(const std::string& c);

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

    /**
     * Sets pointer to the database root
     */
    void setRoot(const FWObject *_dbroot) { dbroot = (FWObject*)_dbroot; }

    /**
     *   Returns a string that represents a path to the object
     *   'this'. Path is built using names of objects above 'this',
     *   all the way to the root of the tree. If parameter 'relative'
     *   is true, the path is built relative to the library 'this' is
     *   a part of (name of the library is not included).
     */
    std::string getPath(bool relative=false) const;

    void addAt(int where_id, FWObject *obj);
    virtual void add(FWObject *obj,bool validate=true);
    virtual void insert_before(FWObject *o1,FWObject *obj);
    virtual void insert_after(FWObject *o1,FWObject *obj);

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
    virtual bool  validateChild(FWObject *o);

    /**
     * forcefully destroys all objects in the subtree under this,
     * except this. This method ignores reference counters and simply
     * deletes everything.
     */
    void destroyChildren();

    void clearChildren(bool recursive=true);

    int getChildrenCount();

    virtual void dump(bool recursive,bool brief,int offset=0);
    virtual void dump(std::ostream &f,bool recursive,bool brief,int offset=0);

    /**
     * returns true if this is either direct child of obj, or a child
     * of one of its children and so on (this method works recursively).
     */
    bool isChildOf(FWObject *obj);

    /**
     * Returns object with given ID. If recursive is 'false'
     * search is restricted to direct children of given object
     * and itslef. If it is true, method proceeds to all chidren of all
     * objects starting from current one.
     */
    virtual FWObject* getById  (int id, bool recursive=false);


    /**
     * Returns list of direct children of current object
     * whose getTypeName() same as given.
     * If not found, empty list is returned.
     */
    virtual std::list<FWObject*> getByType(const std::string &type_name) const;

    /**
     * Returns list of direct children of current object
     * whose getTypeName() same as given.
     * If not found, end() is returned.
     */
    virtual FWObjectTypedChildIterator findByType(const std::string &type_name) const;

    /**
     * Returns first of direct children of current object
     * whose getTypeName() same as given or NULL if not found.
     */
    virtual FWObject* getFirstByType(const std::string &type_name) const;

    /**
     * finds a child object of a given type with a given name
     */
    FWObject* findObjectByName(const std::string &type,
                               const std::string &name) throw(FWException);
    
    virtual void    setDirty(bool f);
    virtual bool    isDirty();

    /**
     * sets read-only flag for the object. FWObject checks all objects
     * up in the tree before making any change to see if the subtree
     * is read-only. Marking an object read-only automatically makes
     * the whole tree branch under it read-only.
     */
    virtual void    setReadOnly(bool f);
    virtual bool    isReadOnly();
    virtual void    checkReadOnly() throw(FWException);

    // Attributes iterator
    std::map<std::string, std::string>::const_iterator dataBegin() { return data.begin(); }
    std::map<std::string, std::string>::const_iterator dataEnd  () { return data.end();   }

};

class FWObjectTypedChildIterator
{
    public:

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

}

#endif // _FWOBJECT_HH







