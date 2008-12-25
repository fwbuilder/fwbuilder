/* 

                          Firewall Builder

                 Copyright (C) 2001 NetCitadel, LLC

  Author:  Vadim Zaliva lord@crocodile.org

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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <cstring>
#include <signal.h>

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/FWObject.h>
#include <fwbuilder/FWObjectDatabase.h>

#include <fwbuilder/Library.h>
#include <fwbuilder/Interval.h>
#include <fwbuilder/ICMPService.h>
#include <fwbuilder/ICMP6Service.h>
#include <fwbuilder/IPService.h>
#include <fwbuilder/TCPService.h>
#include <fwbuilder/UDPService.h>
#include <fwbuilder/CustomService.h>
#include <fwbuilder/FWReference.h>
#include <fwbuilder/FWObjectReference.h>
#include <fwbuilder/FWServiceReference.h>
#include <fwbuilder/FWIntervalReference.h>
#include <fwbuilder/Host.h>
#include <fwbuilder/Interface.h>
#include <fwbuilder/IPv4.h>
#include <fwbuilder/IPv6.h>
#include <fwbuilder/physAddress.h>
#include <fwbuilder/DNSName.h>
#include <fwbuilder/AddressTable.h>
#include <fwbuilder/Group.h>
#include <fwbuilder/Rule.h>
#include <fwbuilder/RuleElement.h>
#include <fwbuilder/RuleSet.h>
#include <fwbuilder/FWOptions.h>
#include <fwbuilder/Firewall.h>
#include <fwbuilder/NAT.h>
#include <fwbuilder/Policy.h>
#include <fwbuilder/Routing.h>
#include <fwbuilder/ObjectGroup.h>
#include <fwbuilder/ServiceGroup.h>
#include <fwbuilder/IntervalGroup.h>
#include <fwbuilder/Network.h>
#include <fwbuilder/NetworkIPv6.h>
#include <fwbuilder/AddressRange.h>
#include <fwbuilder/Management.h>
#include <fwbuilder/XMLTools.h>
#include <fwbuilder/TagService.h>
#include <fwbuilder/UserService.h>

#include <iostream>
#include <sstream>

using namespace std;
using namespace libfwbuilder;

// each program invocation tracks its own set of object ids (as int)
// we just increment this counter to get new id. We also keep a dictionary
// that maps integer ids to strings. This dictionary is populated when
// objects are loaded from xml file and then used to write string ids
// when objects are written back to file. Internally we operate with
// integer ids all the time, string ids are only used in xml file.
// "System" objects use ids < 1000.

int id_seed = 1000;
#ifdef _WIN32
static int cached_pid = _getpid();
#else
static int cached_pid = getpid();
#endif

// these two dictionaries must be static to ensure uniqueness of integer
// ids across multiple FWObjectDatabase objects

map<int, string> id_dict;
map<string, int> id_dict_reverse;

const char*  FWObjectDatabase::TYPENAME  = {"FWObjectDatabase"};
const string FWObjectDatabase::DTD_FILE_NAME  = "fwbuilder.dtd"    ;


FWObjectDatabase::FWObjectDatabase() : FWObject(false), data_file(), obj_index()
{
    init_create_methods_table();

    setRoot(this);
    index_hits = index_misses = 0;
    init_id_dict();

    searchId =0;
    lastModified = 0;

    setName(TYPENAME);

    setId( ROOT_ID );
    setDirty(false);
}

FWObjectDatabase::FWObjectDatabase(FWObjectDatabase& d) :
    FWObject(false), data_file(), obj_index()
{
    init_create_methods_table();

    setRoot(this);
    index_hits = index_misses = 0;
    init_id_dict();

    data_file = d.data_file;

    setName(TYPENAME);

    searchId =0;

    init = true;
    *this = d;  // copies entire tree
    setId( ROOT_ID );

// I do not understand why do I need to reindex the whole database
// after operator=. It calls FWobject::duplicate, which in turn
// uses FWObject::shallowDuplicate, which calls addToIndex
// This is necessary however.
//
    addToIndexRecursive( this );  // reindex

    setDirty(false);
    init = false;
}

FWObjectDatabase::~FWObjectDatabase()
{
    init = true;
}

void FWObjectDatabase::init_id_dict()
{
    if (id_dict.size()==0)
    {
        id_dict[ROOT_ID] = "root";
        id_dict[ANY_ADDRESS_ID] = "sysid0";
        id_dict[ANY_SERVICE_ID] = "sysid1";
        id_dict[ANY_INTERVAL_ID] = "sysid2";
        id_dict[STANDARD_LIB_ID] = "syslib000";
        id_dict[TEMPLATE_LIB_ID] = "syslib100";
        id_dict[DELETED_OBJECTS_ID] = "sysid99";

        for (map<int,string>::iterator i=id_dict.begin(); i!=id_dict.end(); ++i)
            id_dict_reverse[i->second] = i->first;
    }
}

int FWObjectDatabase::registerStringId(const std::string &s_id)
{
    int i_id = -1;

    if (id_dict_reverse.count(s_id) > 0)
        return id_dict_reverse[s_id];

    i_id = ++id_seed;
    id_dict[i_id] = s_id;
    id_dict_reverse[s_id] = i_id;
    return i_id;
}

int FWObjectDatabase::getIntId(const std::string &s_id)
{
    if (id_dict_reverse.count(s_id) > 0) return id_dict_reverse[s_id];
    return -1;
}

string FWObjectDatabase::getStringId(int i_id)
{
    if (id_dict.count(i_id) > 0) return id_dict[i_id];

    // TODO: Use proper GUID algorithm here
    char id_buf[64];
    sprintf(id_buf, "id%dX%d", i_id, cached_pid);
    id_dict[i_id] = string(id_buf);
    id_dict_reverse[string(id_buf)] = i_id;
    return id_dict[i_id];
}

int FWObjectDatabase::generateUniqueId()
{
    return ++id_seed;
}

void FWObjectDatabase::setFileName(const string &filename)
{
    data_file = filename;
}

const string &FWObjectDatabase::getFileName() 
{ 
    return data_file; 
}

const string FWObjectDatabase::getFileDir() 
{ 
  char dir_delimiter='/';
#ifdef __MINGW32__
  dir_delimiter = '\\';
#endif

    string::size_type i = data_file.rfind(dir_delimiter);
    if (i==string::npos || i==0) 
	return "";
    else 
	return data_file.substr(0,i);
}

void FWObjectDatabase::load(const string &f,
                            XMLTools::UpgradePredicate *upgrade,
                            const std::string &template_dir) throw(FWException)
{
    if(f=="") 
        return;
    
    xmlDocPtr doc = XMLTools::loadFile(f, FWObjectDatabase::TYPENAME,
                                       FWObjectDatabase::DTD_FILE_NAME,
                                       upgrade, template_dir);
    
    xmlNodePtr root = xmlDocGetRootElement(doc);
    
    if(!root || !root->name || strcmp(FROMXMLCAST(root->name),
                                      FWObjectDatabase::TYPENAME)!=SAME)
    {
	xmlFreeDoc(doc);
        throw FWException("Data file has invalid structure: "+f);
    }
    
    try
    {
        init = true;

//        clearChildren();
        deleteChildren();
        clearIndex();

        fromXML(root);
        setDirty(false);
        setFileName(f);
    } catch (FWException &ex)
    {
        init = false;
        throw(ex);
    }
    init = false;
}

void FWObjectDatabase::saveXML(xmlDocPtr doc) throw(FWException)
{
    doc->children = xmlNewDocNode(doc, NULL, STRTOXMLCAST(getName()), NULL);
    xmlNewNs(doc->children, TOXMLCAST("http://www.fwbuilder.org/1.0/"), NULL);

    toXML(xmlDocGetRootElement(doc));

    XMLTools::setDTD(doc, FWObjectDatabase::TYPENAME, FWObjectDatabase::DTD_FILE_NAME);
}

void FWObjectDatabase::saveFile(const string &filename) throw(FWException)
{
/* need to set flag 'init' so we ignore read-only status. Some objects
 * modify themselves in toXML (e.g. Management) so if they belong to a
 * read-only library, we can't save them to a file. It should be safe
 * to ignore read-only flag but save it though.
 */
    init=true;

    xmlDocPtr doc = xmlNewDoc(TOXMLCAST("1.0"));
  
    doc->children = xmlNewDocNode(doc, NULL, STRTOXMLCAST(getName()), NULL);
    xmlNewNs(doc->children, TOXMLCAST("http://www.fwbuilder.org/1.0/"), NULL);

    toXML(xmlDocGetRootElement(doc));

    XMLTools::saveFile(doc, filename, FWObjectDatabase::TYPENAME, FWObjectDatabase::DTD_FILE_NAME);

    xmlFreeDoc(doc);  
    setDirty(false);

    init=false;
}

void FWObjectDatabase::saveToBuffer(xmlChar **buffer,int *size) throw(FWException)
{
/* need to set flag 'init' so we ignore read-only status. Some objects
 * modify themselves in toXML (e.g. Management) so if they belong to a
 * read-only library, we can't save them to a file. It should be safe
 * to ignore read-only flag but save it though.
 */
    init=true;

    xmlDocPtr doc = xmlNewDoc(TOXMLCAST("1.0"));
  
    doc->children = xmlNewDocNode(doc, NULL, STRTOXMLCAST(getName()), NULL);
    xmlNewNs(doc->children, TOXMLCAST("http://www.fwbuilder.org/1.0/"), NULL);

    toXML(xmlDocGetRootElement(doc));

    XMLTools::dumpToMemory(doc, buffer, size, FWObjectDatabase::TYPENAME, FWObjectDatabase::DTD_FILE_NAME);

    xmlFreeDoc(doc);  
//    setDirty(false);

    init=false;
}

void FWObjectDatabase::fromXML(xmlNodePtr root) throw(FWException)
{
    FWObject::fromXML(root);
    
    const char *n=FROMXMLCAST(xmlGetProp(root,TOXMLCAST("lastModified")));
    if(n!=NULL)
    {        
        int i=0;
        istringstream str(n);
        str >> i;
        //sscanf(n,"%d",&i);
        lastModified=i;
        FREEXMLBUFF(n);
    }
}

xmlNodePtr FWObjectDatabase::toXML(xmlNodePtr parent) throw(FWException)
{
    FWObject *o;

    xmlNewProp(parent,NULL,NULL);

    xmlNewProp(parent, 
               TOXMLCAST("version") , 
               TOXMLCAST(LIBFWBUILDER_FORMAT_VERSION));

    if (lastModified!=0)
    {
        ostringstream str;
        str << lastModified;
        //char lmbuf[32];
        //sprintf(lmbuf,"%ld",lastModified);
        xmlNewProp(parent, 
                   TOXMLCAST("lastModified"),
                   TOXMLCAST(str.str().c_str()));
    }

    int rootid = getId();

    xmlAttrPtr pr = xmlNewProp(parent, 
			       TOXMLCAST("id") , 
			       STRTOXMLCAST(id_dict[rootid]));

    xmlAddID(NULL, parent->doc, STRTOXMLCAST(id_dict[rootid]), pr);


    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        if((o=(*j))!=NULL)   
            o->toXML(parent);
    
    return parent;
}

void  FWObjectDatabase::setDirty(bool f)
{
    dirty=f;
    if(!init && f) lastModified=time(NULL);
}

void FWObjectDatabase::addToIndex(FWObject* o)
{
    if (o)
    {
        o->setRoot( this );
        if (o->getId() > -1 ) obj_index[o->getId()] = o;
    }
}

void FWObjectDatabase::removeFromIndex(int id)
{
    obj_index.erase(id);
}

FWObject* FWObjectDatabase::checkIndex(int id)
{
    if (obj_index.count(id)==0) return NULL;
    return obj_index[id];
}

FWObject* FWObjectDatabase::findInIndex(int id)
{
    FWObject *o = checkIndex(id);
    if (o!=NULL) index_hits++;
    else
    {
        index_misses++;
// if index is incomplete or empty, update it automatically using
// recursive search to find object
        o = getById( id , true );
        if (o) addToIndex(o);
    }
    return o;
}

void FWObjectDatabase::buildIndex()
{
    clearIndex();
    addToIndexRecursive(this);
}

void FWObjectDatabase::_clearReferenceCounters(FWObject *o)
{
    o->clearRefCounter();
    for (FWObject::iterator i=o->begin(); i!=o->end(); ++i)
    {
        _clearReferenceCounters(*i);
    }
}

void FWObjectDatabase::_fixReferenceCounters(FWObject *o)
{
    if (FWReference::cast(o)!=NULL)
    {
        FWObject *obj = findInIndex( FWReference::cast(o)->getPointerId() );
        assert(obj);
        obj->ref();
    }
    for (FWObject::iterator i=o->begin(); i!=o->end(); ++i)
    {
        (*i)->ref();
        _fixReferenceCounters(*i);
    }
}

void FWObjectDatabase::reIndex()
{
    buildIndex();
    _clearReferenceCounters(this);
    _fixReferenceCounters(this);
}

void FWObjectDatabase::clearIndex()
{
    index_hits = index_misses = 0;
    obj_index.clear();
}

void FWObjectDatabase::getIndexStats(int &index_size,
                                     int &hit_counter,
                                     int &miss_counter)
{
    index_size = obj_index.size();
    hit_counter = index_hits;
    miss_counter = index_misses;
}

void FWObjectDatabase::addToIndexRecursive(FWObject *o)
{
    addToIndex(o);
    for (FWObject::iterator i=o->begin(); i!=o->end(); ++i)
        addToIndexRecursive(*i);
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

/**
 *  find which firewalls and groups use object 'o' in a subtree rooted
 *  at object 'p' skip 'deleted objects' library, avoid circular group
 *  references using temporary flags set in objects this method
 *  visits.
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

//#define DEBUG_WHERE_USED 1

bool FWObjectDatabase::_isInIgnoreList(FWObject *o)
{
    return (FWOptions::cast(o)!=NULL);
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


/*
 * This method removes all references to child objects of obj, then
 * removes obj.  FWObject::remove moves obj to the "DeletedObjects"
 * library so we want to preserve subtree structure under obj. There
 * is no need to delete child objects, but we must remove all
 * references to them.
 *
 * Note: there is no need to search for references pointing at certain
 * types of objects, such as references and rules/rule sets. This
 * dramatically speeds up deleting firewalls with large policies and
 * groups with lots of objects
 */
void FWObjectDatabase::recursivelyRemoveObjFromTree(FWObject* obj,
                                                    bool remove_ref)
{
    obj->checkReadOnly();

    for (FWObject::iterator i=obj->begin(); i!=obj->end(); ++i)
    {
        if (FWReference::cast(*i)!=NULL || RuleSet::cast(*i)!=NULL) continue;
        recursivelyRemoveObjFromTree( *i , true);
    }

    if (remove_ref)  removeAllReferences(obj);
    else             removeAllInstances(obj);
}


