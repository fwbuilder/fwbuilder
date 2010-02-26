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

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/Tools.h>
#include <fwbuilder/XMLTools.h>
#include <fwbuilder/FWObject.h>
#include <fwbuilder/FWObjectDatabase.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/Library.h>

#include <fwbuilder/FWOptions.h>
#include <fwbuilder/RuleSet.h>
#include <fwbuilder/RuleElement.h>
#include <fwbuilder/Rule.h>
#include <fwbuilder/Host.h>

#include <stdlib.h>
#include <stdio.h>
// #include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <stack>

#include <assert.h>

using namespace std;
using namespace libfwbuilder;

const char *FWObject::TYPENAME={"UNDEF"};
string FWObject::NOT_FOUND="";


void FWObject::fromXML(xmlNodePtr root) throw(FWException)
{
    assert(root!=NULL);
    const char *n;

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("name")));
    if(n)
    {
        setName(n);
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("id")));
    if(n)
    {
        setId(FWObjectDatabase::registerStringId(n));
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("comment")));
    if(n)
    {
        setComment(XMLTools::unquote_linefeeds(n));
        FREEXMLBUFF(n);
    }

    n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("ro")));
    if(n)
    {
        ro = (cxx_strcasecmp(n, "1")==0 || cxx_strcasecmp(n , "true")==0);
        FREEXMLBUFF(n);
    }

    ref_counter = 0;
    FWObjectDatabase *dbr = getRoot();

    for (xmlNodePtr cur=root->xmlChildrenNode; cur; cur=cur->next)
    {
        if (cur && !xmlIsBlankNode(cur))
        {
            FWObject *o = dbr->createFromXML(cur);
            if (o!=NULL) 
            {
                /* Add w/o validation. Trust XML to do that */
		add(o, false);
                try
                {
                    o->fromXML(cur);
                } catch(FWException &ex)
                {
                    map<string, string> &properties = ex.getProperties();
                    if (properties.find("failed_element")==properties.end())
                        properties["failed_element"]=o->getTypeName();
                    throw;
                }
	    }
        }
    }
    setDirty(false);
}

xmlNodePtr FWObject::toXML(xmlNodePtr xml_parent_node) throw(FWException)
{
    return toXML(xml_parent_node, true);
}

xmlNodePtr FWObject::toXML(xmlNodePtr parent, bool process_children)
    throw(FWException)
{
    string s_id = FWObjectDatabase::getStringId(getId());

    xmlNodePtr me = xmlNewChild(
        parent,
        NULL,
        xml_name.empty() ? STRTOXMLCAST(getTypeName()) : STRTOXMLCAST(xml_name),
        NULL);

    if (id!=-1)
    {
        xmlNewProp(
            me, 
            TOXMLCAST("id"),
            STRTOXMLCAST(s_id));
    }

    for(map<string, string>::const_iterator i=data.begin(); i!=data.end(); ++i) 
    {
        const string &name  = (*i).first;
        const string &value = (*i).second;

        if (name[0]=='.') continue;

        xmlNewProp(me, STRTOXMLCAST(name), STRTOXMLCAST(value));
    }

    if (process_children)
    {
        for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
            (*j)->toXML(me);
    }

    return me;
}


FWObject::FWObject()
{
    init        = false;
    ref_counter = 0;
    parent      = NULL;
    dbroot      = NULL;
    name        = "";
    comment     = "";
    id          = -1;
    ro          = false;

    // When object is created we assign it unique Id
    setId(FWObjectDatabase::generateUniqueId());

    setDirty(false);
    storeCreationTime();
}

FWObject::FWObject(bool new_id)
{
    init        = false;
    ref_counter = 0;
    parent      = NULL;
    dbroot      = NULL;
    name        = "";
    comment     = "";
    id          = -1;
    ro          = false;

    // When object created we assign it unique Id
    if (new_id)
        setId(FWObjectDatabase::generateUniqueId());

    setDirty(false);
    storeCreationTime();
}

FWObject::FWObject(const FWObject &c) : list<FWObject*>(c)
{
    init = false;
    *this = c;
    storeCreationTime();
}

FWObject::FWObject(const FWObjectDatabase *root, bool )
{
    init        = false;
    ref_counter = 0    ;
    parent      = NULL ;
    dbroot      = (FWObjectDatabase*)root;
    name        = "";
    comment     = "";
    id          = -1;
    ro          = false;
    
    // When object created we assign it unique Id
    setId(FWObjectDatabase::generateUniqueId());

    setDirty(false);
    storeCreationTime();
}

FWObject::~FWObject() 
{
    init = true;  // ignore read-only
    destroyChildren();
    data.clear();
    private_data.clear();
}

void FWObject::setPrivateData(const string &key, void *data)
{
    private_data[key] = data;
}

void* FWObject::getPrivateData(const string &key) const
{
    map<string, void*>::const_iterator it = private_data.find(key);
    if(it == private_data.end())
        return NULL;
    else
        return it->second;
}

FWObject* FWObject::getParent() const
{
    return parent;
}

void FWObject::setParent(FWObject *p)
{
    parent=p;
}

void FWObject::setXMLName(const string &n)
{
    xml_name = n;
}

FWObject* FWObject::_find(const string& name) const
{
    const_iterator i = std::find_if(begin(),end(), FWObjectNameEQPredicate(name));
    return i==end()?NULL:(*i);
}

list<FWObject*> FWObject::findIf(FWObjectFindPredicate *pred)
{
    list<FWObject*> res_list;
    list<FWObject*>::iterator i1;
    for(i1=begin(); i1!=end(); ++i1)
    {
        if ((*pred)(*i1)) res_list.push_back(*i1);
        list<FWObject*> res1 = (*i1)->findIf(pred);
        res_list.splice(res_list.begin(), res1);
    }
    return res_list;
}

FWObject* FWObject::findObjectByName(const string &type,
                                     const string &name) throw(FWException)
{
    if (getTypeName()==type && getName()==name) return this;

    list<FWObject*>::iterator j;
    for(j=begin(); j!=end(); ++j)     
    {
        FWObject *o=*j;

        o=o->findObjectByName(type,name);
        if(o) return o;
    }

    return NULL; // not found
}

FWObject* FWObject::findObjectByAttribute(const std::string &attr,
                                          const std::string &val)
    throw(FWException)
{
    if (getStr(attr)==val) return this;

    list<FWObject*>::iterator j;
    for(j=begin(); j!=end(); ++j)     
    {
        FWObject *o=*j;

        o=o->findObjectByAttribute(attr, val);
        if(o) return o;
    }

    return NULL; // not found
}


bool FWObject::cmp(const FWObject *obj, bool recursive) throw(FWException)
{
    if (name != obj->name || comment != obj->comment || ro != obj->ro)
        return false;

    if (data.size() != obj->data.size()) return false;

    for(map<string, string>::const_iterator i=data.begin(); i!=data.end(); ++i) 
    {
        const string &name  = (*i).first;
        const string &value = (*i).second;
// 10/21/2008 --vk
        map<string,string>::const_iterator j=obj->data.find(name);
        if (j==obj->data.end()) return false;
        if (j->second!=value) return false;
    }

    if (recursive)
    {
        if (size()!=obj->size())  return false;

/* chidren are not necessarily in the same order in two groups */
        FWObject::const_iterator i1=begin();
        for ( ; i1!=end(); ++i1)
        {
            bool found=false;
            FWObject::const_iterator j1=obj->begin();
            for ( ; j1!=obj->end(); ++j1)
            {
                if ((*i1)->cmp(*j1, recursive))
                {
                    found=true;
                    break;
                }
            }
            if (!found) return false;
        }
    }
    return true;
}

FWObject& FWObject::operator=(const FWObject &x)  throw(FWException)
{
    return duplicate(&x, false);
}

FWObject& FWObject::duplicate(const FWObject *x, bool preserve_id)
    throw(FWException)
{
    checkReadOnly();
    bool xro = x->ro;

    shallowDuplicate(x, preserve_id);

    if (xro) setReadOnly(false);

    destroyChildren();   // does it erase index entries?

    for(list<FWObject*>::const_iterator m=x->begin(); m!=x->end(); ++m) 
    {
        FWObject *o = *m;
        addCopyOf( o, preserve_id);
    }

    setDirty(true);
    if (xro) setReadOnly(true);

    return *this;
}

FWObject* FWObject::addCopyOf(const FWObject *x, bool preserve_id)
    throw(FWException)
{
    if (x==NULL) return NULL;
    FWObject *o1;
    FWObjectDatabase *root = getRoot();
    if (root==NULL) root = x->getRoot();
    // do not prepopulte children for objects that do that automatically
    // in their constructor
    o1 = root->create(x->getTypeName(), -1, false);
    if(!o1)
        throw FWException(string("Error creating object with type: ")+
                          x->getTypeName());
    // This adds with validation
    add(o1);

    o1->duplicate(x, preserve_id);

    return o1;
}

/*
 * copying an object does not copy its read-only status; thus we can
 * take read-only object and make a read-write copy of it to work
 * with.
 *
 * 07/06/2007 -- changed that. Now copy has the same value of "ro"
 * attribute.  Clear it in the caller if neccessary.
 */
FWObject& FWObject::shallowDuplicate(const FWObject *x, bool preserve_id)
    throw(FWException)
{
    checkReadOnly();

    int old_id = getId();

    id = x->id;
    name = x->name;
    comment = x->comment;
    ro = x->ro;
    data = x->data;

    setReadOnly(false);

    //if (getBool("ro")) setReadOnly(false);
    if (!preserve_id)
    {
        ref_counter = 0           ;
        xml_name    = x->xml_name ;
    } else
    {
        // some objects do not have ID per DTD (e.g. Src, Dst, etc.)
        // Those will return -1 from getId()
        if (id > -1)
            setId(old_id);
    }

    if (dbroot==NULL) setRoot(x->getRoot());
    if (dbroot!=NULL) dbroot->addToIndex(this);

    setReadOnly(x->ro);
    setDirty(true);
    return *this;
}

class InheritsFWOptions: public std::unary_function<FWObject*, bool>
{
    public:
    InheritsFWOptions() {}
    bool operator()(const FWObject *o) const 
    {
        return FWOptions::constcast(o)!=NULL;
    }
};

FWObject& FWObject::duplicateForUndo(const FWObject *obj) throw(FWException)
{
    setRO(false);
    InheritsFWOptions pred;
    FWObject::const_iterator mine_opts_iter = std::find_if(begin(), end(), pred);
    FWObject::const_iterator their_opts_iter = std::find_if(obj->begin(), obj->end(), pred);
    if (their_opts_iter != obj->end())
    {
        if (mine_opts_iter != end()) (*mine_opts_iter)->duplicate(*their_opts_iter);
        else addCopyOf(*their_opts_iter);
    }
    shallowDuplicate(obj);
    return *this;
}

const string &FWObject::getName() const 
{ 
    return name;
}

void FWObject::setName(const string &n)   
{
    name = n;
    setDirty(true);
}

const string& FWObject::getLibraryName() const
{
    return getLibrary()->getName();
}

FWObject*  FWObject::getLibrary() const
{
    const FWObject *p=this;
    while (p!=NULL && !Library::isA(p) ) p=p->getParent();
    return (FWObject*)p;
}

FWObjectDatabase* FWObject::getRoot() const
{
    return dbroot;
}

string FWObject::getPath(bool relative) const
{
    string res;
    const FWObject *p=this;
    bool  first=true;
    while (p!=NULL)
    {
        if (relative && Library::isA(p)) return res;
        if (!first) res="/"+res;
        res=p->getName()+res;
        p=p->getParent();
        first=false;
    }
    res="/"+res;
    return res;
}

const string& FWObject::getComment() const
{ 
    return comment;
}

void FWObject::setComment(const string &c)
{
    comment = c;
    setDirty(true);
}

int FWObject::getId() const
{ 
    return id;
}

/*
 * need to update index because ID of the object changes
 */
void FWObject::setId(int c)
{
    id = c;
    setDirty(true);
    if (dbroot!=NULL) dbroot->addToIndex(this);
}

bool FWObject::exists(const string &name) const 
{
    return data.count(name)!=0; 
}

const string &FWObject::getStr(const string &name) const
{
    map<string,string>::const_iterator i=data.find(name);
    if (i==data.end())
        return NOT_FOUND;
    else
        return (*i).second;
}

void FWObject::remStr(const string &name)
{
    checkReadOnly();
    map<string, string>::iterator m=data.find(name);
    if(m != data.end()) 
    {
        data.erase(m);
        setDirty(true);
    }
}

void FWObject::setStr(const string &name, const string &val)
{
    if (name[0]!='.') checkReadOnly();
    data[name] = val;
    if (name[0]!='.') setDirty(true);
}

int FWObject::getInt(const string &name) const
{
    string val = getStr(name);
    size_t n = val.find_first_of(" \n\r\t");
    while (n != string::npos)
    {
        val = val.erase(n, 1);
        n = val.find_first_of(" \n\r\t");
    }
    if (val != "") return( atol(val.c_str()) );
    else return(-1);
}

void FWObject::setInt(const string &name, int val)
{
    ostringstream str;
    str << val;
    setStr(name, str.str());
    // attribute with name that starts with "." is considered "hidden"
    // or "internal". Such attribute is not saved to the data file and
    // should not trigger "dirty" flag.
    if (name[0]!='.') setDirty(true);
}

bool FWObject::getBool(const string &name) const
{
// 10/21/2008 --vk
    string val = getStr(name);
    size_t n = val.find_first_of(" \n\r\t");
    while (n != string::npos)
    {
        val = val.erase(n, 1);
        n = val.find_first_of(" \n\r\t");
    }
    return (val=="1" || cxx_strcasecmp(val.c_str() , "true")==0);
}

void FWObject::setBool(const string &name, bool val)
{
    setStr(name, (val)?"True":"False");
    if (name[0]!='.') setDirty(true);
}

void FWObject::setBool(const string &name, const string &val)
{
    if(!name.empty())
	setBool(name,
		(val=="1" || cxx_strcasecmp(val.c_str(),"true")==0)); 
}

void FWObject::Show()
{
    setBool("read",true);
}

void FWObject::Hide()
{
    setBool("read",false);
}



void FWObject::dump(bool recursive,bool brief,int offset) const
{
    dump(cerr,recursive,brief,offset);
}

void FWObject::dump(std::ostream &f,bool recursive,bool brief,int offset) const
{
    FWObject *o;
    string    n;

    if (brief)
    {
	f << string(offset,' ');
	f << " Obj=" << this;
	f << " ID="  << getId()
          << " (" << FWObjectDatabase::getStringId(getId()) << ")";
	f << " Name=" << getName();
	f << " Type=" << getTypeName();
        if (this!=getRoot())
            f << " Library=" << getLibrary();
	f << " Root=" << getRoot();
        f << " ref_counter=" << ref_counter;
	f << endl;

	if (recursive)
        {
	    list<FWObject*>::const_iterator m;
	    for (m=begin(); m!=end(); ++m) {
		if (  (o=(*m))!=NULL)  o->dump(f,recursive,brief,offset+2);
	    }
	}
    } else
    {
	f << string(offset,' ') << string(16,'-') << endl;
	f << string(offset,' ') << "Obj:    " << this << endl;
	f << string(offset,' ') << "ID:     " << getId()
          << " (" << FWObjectDatabase::getStringId(getId()) << ")"
          << endl;
	f << string(offset,' ') << "Name:   " << getName() << endl;
	f << string(offset,' ') << "Ref.ctr:" << ref_counter << endl;
	f << string(offset,' ') << "Type:   " << getTypeName() << endl;
	f << string(offset,' ') << "Library:" << getLibrary() << endl;
//    f << string(offset,' ') << "Path:   " << getPath() << endl;
	n=(getParent()!=NULL)?getParent()->getName():"";
	f << string(offset,' ') << "Parent: " << getParent()
	  << "  name=" << n << endl;
	f << string(offset,' ') << "Root:   " << getRoot() << endl;

	map<string, string>::const_iterator d;
	for (d=data.begin(); d!=data.end(); ++d)
        {
	    f << string(offset,' ');
	    f << (*d).first << ": " << (*d).second << endl;
	}
	if (recursive)
        {
	    list<FWObject*>::const_iterator m;
	    for (m=begin(); m!=end(); ++m)
            {
		if (  (o=(*m))!=NULL)  o->dump(f,recursive,brief,offset+2);
	    }
	}
    }

}

void FWObject::_adopt(FWObject *obj)
{
    obj->ref();
    obj->setParent(this);
/*
 * normally we always create objects using FWObjectDatabase::create
 * which sets dbroot in the object. However, if an object is created
 * just with 'new ...' and then added to the database using
 * FWObject::add, it doesn't have correct pointer to the database
 * root. We can easily repair this by setting dbroot here. Even if
 * this operation is redundant, its cost is very low and it adds
 * robustness.
 *
 */
    obj->setRoot(getRoot());
}

void FWObject::addAt(int where_id, FWObject *obj)
{
    FWObject *p=getRoot()->findInIndex( where_id );
    assert (p!=NULL);
    p->add(obj);
}

void FWObject::add(FWObject *obj, bool validate)
{
    checkReadOnly();

    if (!validate || validateChild(obj)) 
    {
	push_back(obj);
	_adopt(obj);
	setDirty(true);
    }
}

FWReference* FWObject::createRef()
{
//    FWObjectReference *ref=new FWObjectReference();
    FWObjectReference *ref = getRoot()->createFWObjectReference();
    ref->setPointer(this);
    return ref;
}


void FWObject::addRef(FWObject *obj)
{
    checkReadOnly();

    if (validateChild(obj)) 
    {
	FWReference *oref = obj->createRef();
	obj->ref();

	push_back(oref);
	_adopt(oref);
	setDirty(true);
// see comment in FWObject::_adopt
        obj->setRoot(getRoot());
    }
}

void FWObject::insert_before(FWObject *o1, FWObject *obj)
{
    checkReadOnly();

    if(!obj) 
        return;
    
    list<FWObject*>::iterator m=find(begin(),end(),o1);
    if(m!=end())
    {
        insert(m, obj);
        _adopt(obj);
        setDirty(true);
    }
}

void FWObject::insert_after(FWObject *o1, FWObject *obj)
{
    checkReadOnly();

    if(!obj) 
        return;

    list<FWObject*>::iterator m=find(begin(),end(),o1);
    if(m!=end())
    {
        insert(++m,obj);
        _adopt(obj);
        setDirty(true);
    }
}

void FWObject::swapObjects(FWObject *o1, FWObject *o2)
{
    checkReadOnly();

    for(list<FWObject*>::iterator m=begin(); m!=end(); ++m) 
    {
        if(*m==o1) 
        {
            *m=o2;
        } else if(*m==o2)
        {
            *m=o1;
        }
    }
    setDirty(true);
}

void FWObject::remove(FWObject *obj, bool delete_if_last)
{
    FWObject::iterator fi=std::find(begin(), end(), obj);
    if(fi!=end())
    {
        checkReadOnly();

        erase(fi);
        setDirty(true);
        obj->unref();

        if (delete_if_last && obj->ref_counter==0)
        {
            getRoot()->removeFromIndex(obj->getId());
            delete obj;
        }
    }
}

void FWObject::_removeAll(FWObject *rm)
{
    for (FWObject::iterator i=begin(); i!=end(); i++)
        (*i)->_removeAll(rm);

    remove(rm,false);
}

void FWObject::removeAllInstances(FWObject *rm)
{
    checkReadOnly();
    removeAllReferences(rm);
    _removeAll(rm);
}

void FWObject::removeRef(FWObject *obj)
{
    int  obj_id=obj->getId();
    for(list<FWObject*>::iterator m=begin(); m!=end(); ++m) 
    {
        FWObject *o=*m;
        FWReference *oref = FWReference::cast(o);
        if (oref && oref->getPointerId()==obj_id)
        {
            // do not delete object even if this reference was the last one (?)
            obj->unref();  

            FWObject::remove(o, false);  // do not remove
            delete o;
            return;
        }
    }
}

void FWObject::_removeAllRef(FWObject *rm)
{
    // Do not delete references to the same object from its children
    // such as references to the firewall or cluster in its own rules
    if (this == rm || this->isChildOf(rm)) return;

    for (FWObject::iterator i=begin(); i!=end(); i++)
        (*i)->_removeAllRef(rm);
    
    removeRef(rm);
}

void FWObject::removeAllReferences(FWObject *rm)
{
    _removeAllRef(rm);
}

void FWObject::findAllReferences(const FWObject *obj, std::set<FWReference*> &res)
{
    int obj_id = obj->getId();
    for(list<FWObject*>::iterator m=begin(); m!=end(); ++m) 
    {
        FWObject *o=*m;
        FWReference *oref = FWReference::cast(o);
        if(oref)
        {
	    if(oref->getPointerId()==obj_id) 
                res.insert(oref);
	} else
        {
            o->findAllReferences(obj, res);
        }
    }
}

set<FWReference*> FWObject::findAllReferences(const FWObject *obj)
{
    set<FWReference*> res;
    findAllReferences(obj, res);
    return res;
}

bool FWObject::validateChild(FWObject *obj)
{ 
    return true;

    /*
     *  Check if object "this" is a descendant of object "obj" to avoid loops
     *
     *  check disabled for now since we need to be able to add firewall to its
     *  own policy
     */
    FWObject *p;
    p=this;
    do {
	if (p==obj) return false;
    } while ((p=p->getParent())!=NULL);
    return true;
}

/*
 * this method deletes all children recursively regardless of their
 * usage counter. We call this method form destructor.
 */
void FWObject::destroyChildren()
{
    FWObjectDatabase *dbr = getRoot();
    for(list<FWObject*>::iterator m=begin(); m!=end(); ++m) 
    {
        FWObject *o = *m;
        if (o && o->size())
        {
            o->destroyChildren();
            if (dbr && !dbr->init) dbr->removeFromIndex( o->getId() );
            delete o;
        }
    }
    clear();
}

/*
 *  even if I run this method with flag recursive=true, it does not
 *  guarantee that there will be no objects lost in the memory. If
 *  some children of this are referenced from somewhere else, they
 *  won't be deleted because their reference counter is >1. This is
 *  bad because it leads to a situation when object is not a part of
 *  the tree anymore, but reference to it does exist.
 *
 *  If this method is called with recursive=false, then it deletes
 *  only immediate children of this, leaving their children hanging in
 *  the memory. TODO: research whether we ever need to call it with
 *  recursive=false
 *
 *  In other words, this method leaves tree in inconsistent state. At
 *  this time I am just using it carefully, only when I copy objects
 *  between main tree and scratch pad, and when I create copies of
 *  objects. In both cases children of this will be immediately
 *  restored after call to clearChildren.
 *
 *                                              05/08/02 vk
 */
void FWObject::clearChildren(bool recursive)
{
    FWObjectDatabase *dbr = getRoot();

    checkReadOnly();

    for(list<FWObject*>::iterator m=begin(); m!=end(); ++m) 
    {
        FWObject *o=*m;
        if (recursive) o->clearChildren(recursive);
        o->unref();
        if(o->ref_counter==0) 
        {
            if (dbr) dbr->removeFromIndex( o->getId() );
            delete o;
        }
    }
    clear();
    setDirty(true);
}

int FWObject::getChildrenCount() const
{
    return(size());
}

/*
 * returns true if this is either direct child of obj, or a child
 * of one of its children and so on.
 */
bool FWObject::isChildOf(FWObject *obj)
{
    if (this==obj) return false;
#if 0
    cerr << "FWObject::isChildOf" << endl;
    cerr << "this: " << endl;
    dump(true,true);
    cerr << endl;
    cerr << "obj: " << endl;
    obj->dump(true,true);
    cerr << endl;
#endif
    FWObject *p=this;
    while (p!=NULL && p!=obj) p=p->getParent();
    return (p==obj);
}

bool FWObject::hasChild(FWObject *obj)
{
    int o_id = obj->getId();
    for (list<FWObject*>::iterator it=begin(); it!=end(); ++it)
    {
        if ((*it)->getId() == o_id) return true;
    }
    return false;
}

FWObject* FWObject::getById  (int id, bool recursive)
{
    if(id==getId())  return this;
    
    list<FWObject*>::iterator j;
    for(j=begin(); j!=end(); ++j)     
    {
        FWObject *o=*j;
        int oid = o->getId();
        if(id==oid) return o;

        if(recursive && (o=o->getById(id, true))!=NULL ) return o;
    }
    return NULL; // not found
}

FWObject* FWObject::getFirstByType(const string &type_name) const
{
    const_iterator i=find_if(begin(),end(), FWObjectTypeNameEQPredicate(type_name));
    return i==end()?NULL:(*i);
}

list<FWObject*> FWObject::getByType(const string &type_name) const
{
    list<FWObject*> res;
    for(const_iterator i=begin(); i!=end(); ++i) 
    {
	i=find_if( i, end(), FWObjectTypeNameEQPredicate(type_name));
	if (i==end()) break;
	res.push_back(*i);
    } 
    return res; 
}

list<FWObject*> FWObject::getByTypeDeep(const string &type_name) const
{
    list<FWObject*> res = getByType(type_name); // direct children
    for (const_iterator i=begin(); i!=end(); ++i)
    {
        list<FWObject*> c_res = (*i)->getByTypeDeep(type_name);
        res.insert(res.end(), c_res.begin(), c_res.end());
    }
    return res;
}

FWObjectTypedChildIterator FWObject::findByType(const std::string &type_name) const
{
    return FWObjectTypedChildIterator(this, type_name);
}

/**
 *  this method should change 'dirty' flag in the root, i.e. in
 *  FWObjectDatabase object
 */
void  FWObject::setDirty(bool f)
{
    FWObjectDatabase *dbr = getRoot();
    if (dbr==NULL) return;
    if (dbr==this) dirty = f;
    else dbr->dirty = f;
}

bool FWObject::isDirty()
{
    FWObjectDatabase *dbr = getRoot();
    if (dbr==NULL) return false;
    return (dbr->dirty);
}

/*
 * can't use normal method setBool because it checks for read-only
 * flag
 */
void FWObject::setReadOnly(bool f)
{
    ro = f;
    FWObjectDatabase *dbr = getRoot();
    if (dbr)
    {
        bool ri = dbr->init;
        dbr->init = true;
        setDirty(true);
        dbr->init = ri;
    }
}

/*
 * scan the tree up from 'this' to the root and see if any object
 * along the way is marked as read-only. If such object exists, it
 * makes the whole subtree under it read-only which blocks all
 * changes.
 *
 * If root of the tree has flag 'init' set to true, we do not check
 * for read-only. This is used to initialize the tree.
 */
bool FWObject::isReadOnly()
{
    FWObjectDatabase *dbr = getRoot();
    if (dbr==NULL || dbr->init) return false;
    FWObject *p=this;
    while (p)
    {
        if (p->ro) return true;
        p = p->getParent();
    }
    return false;
}

void FWObject::checkReadOnly() throw(FWException)
{
    if (isReadOnly()) throw FWException(
        string("Attempt to modify read-only object ")+getName());
}

FWObjectTypedChildIterator::FWObjectTypedChildIterator(
    const FWObjectTypedChildIterator &o) :
    type_name(o.type_name),
    real_iterator(o.real_iterator), _begin(o._begin), _end(o._end)
{
}

FWObjectTypedChildIterator::FWObjectTypedChildIterator(
    const FWObject *o, const std::string &_type_name)
{
    type_name     = _type_name ;
    _end          = o->end()   ;
    real_iterator = o->begin() ;
    // position to first element
    while(real_iterator!=_end && (*real_iterator)->getTypeName()!=type_name)
        real_iterator++;
    _begin = real_iterator;
}

bool FWObjectTypedChildIterator::operator==(
    const FWObject::const_iterator& __x) const 
{ 
    return real_iterator == __x; 
}

bool FWObjectTypedChildIterator::operator!=(
    const FWObject::const_iterator& __x) const 
{ 
    return real_iterator != __x; 
}
    
FWObject *FWObjectTypedChildIterator::operator*() const 
{ 
    return *real_iterator;
}

FWObjectTypedChildIterator& FWObjectTypedChildIterator::operator++() 
{
    if(real_iterator==_end)
        return *this;
    do
    {
        real_iterator++;
    } while(real_iterator!=_end && (*real_iterator)->getTypeName()!=type_name);
        return *this;
}

FWObjectTypedChildIterator& FWObjectTypedChildIterator::operator--() 
{ 
    if(real_iterator==_begin)
        return *this;
    do
    {
        real_iterator--;
    } while(real_iterator!=_begin &&
           (real_iterator==_end || (*real_iterator)->getTypeName()!=type_name));
    return *this;
}

/* this is a postfix operator */
FWObject::tree_iterator FWObject::tree_iterator::operator++(int )
{
    FWObject::tree_iterator __tmp = *this;
    ++*this;
    return __tmp;
}
 
//#define TI_DEBUG 1

/* this is a prefix operator */
FWObject::tree_iterator& FWObject::tree_iterator::operator++()
{
    if (node == (FWObject*)(-1)) return *this;

#ifdef TI_DEBUG
    cerr << endl;
    cerr << "ENTRY  node=" << node << "("
         << node->getTypeName() << " " << node->getName() << ")"
         << endl;
    node->dump(false, false);
#endif

    if (node->size()!=0)
    {
        node = node->front();

#ifdef TI_DEBUG
        cerr << "#2     node=" << node << "("
             << node->getTypeName() << " " << node->getName() << ")" << endl;
#endif
        return *this;
    }

    FWObject *p = node;
    while (node->getParent()!=NULL)
    {
        p = node->getParent();

#ifdef TI_DEBUG
        cerr << "       p=" << p << "("
             << p->getTypeName() << " " << p->getName() << ")" << endl;
#endif

        for (FWObject::iterator i=p->begin(); i!=p->end(); ++i)
        {
#ifdef TI_DEBUG
            cerr << "       child=" << (*i) << "("
                 << (*i)->getTypeName() << " " << (*i)->getName() << ")" << endl;
#endif

            if ( node == (*i) )
            {
                ++i;
                if (i==p->end())
                {
#ifdef TI_DEBUG
                    cerr << "      end of list" << endl;
#endif
                    node = p;
                    break;
                }

                node = *i;

#ifdef TI_DEBUG
                cerr << "#3     node=" << node << "("
                     << node->getTypeName() << " " << node->getName() << ")"
                     << endl;
#endif
                return *this;
            }
        }
    }
    node=(FWObject*)(-1);

#ifdef TI_DEBUG
    cerr << "#4  END" << endl;
#endif

    return *this;
}

bool FWObject::tree_iterator::operator==(const FWObject::tree_iterator& i) const
{
    return (node==i.node);
}

bool FWObject::tree_iterator::operator!=(const FWObject::tree_iterator& i) const
{
    return (node!=i.node);
}

FWObject::tree_iterator FWObject::tree_begin()
{
    return FWObject::tree_iterator(this);
}

FWObject::tree_iterator FWObject::tree_end()
{
    return FWObject::tree_iterator( (FWObject*)(-1) );
}

/*
 * find all references to object with id "old_id" in objects in rs
 * (recursively) and replace them with references to object with id
 * "new_id" Use this to find all references to old firewall with
 * references to the new one when copying policy of the old one into
 * the new one.
 */
int FWObject::replaceRef(int old_id, int new_id)
{
    int ref_replacement_counter = 0;
    replaceReferenceInternal(old_id, new_id, ref_replacement_counter);
    return ref_replacement_counter;
}

void FWObject::replaceReferenceInternal(int old_id, int new_id, int &counter)
{
    if (old_id == new_id) return;

    FWReference *ref = FWReference::cast(this);
    if (ref==NULL)
    {
        for (FWObject::iterator j1=begin(); j1!=end(); ++j1)
            (*j1)->replaceReferenceInternal(old_id, new_id, counter);
    } else
    { 
        if (ref->getPointerId()==old_id)
        {
            ref->setPointerId(new_id);
            counter++;
        }
    }
}

void FWObject::findDependencies(list<FWObject*> &deps)
{
    int loop_id = time(NULL);
    _findDependencies_internal(this, deps, loop_id);
}

void FWObject::_findDependencies_internal(FWObject *obj,
                                          list<FWObject*> &deps,
                                          int anti_loop_id)
{
    if (obj==NULL) return;
    if (FWOptions::cast(obj)) return;
    if (FWReference::cast(obj)!=NULL)
    {
        _findDependencies_internal(FWReference::cast(obj)->getPointer(),
                                   deps, anti_loop_id);
    } else
    {
        if (obj->getInt(".anti_loop")==anti_loop_id) return;
        obj->setInt(".anti_loop", anti_loop_id);

        if (!RuleElement::cast(obj) && !Rule::cast(obj) && !RuleSet::cast(obj))
            deps.push_back(obj);

        if (Group::cast(obj) || RuleSet::cast(obj) || Host::cast(obj))
        {
            for (FWObject::iterator j1=obj->begin(); j1!=obj->end(); ++j1)
            {
                FWObject *o = *j1;
                _findDependencies_internal(o, deps, anti_loop_id);
            }
        }
    }
}

bool FWObject::isPrimaryObject() const { return false; }

