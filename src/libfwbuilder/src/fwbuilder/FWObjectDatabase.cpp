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

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"

#include "fwbuilder/AttachedNetworks.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Interval.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/UDPService.h"
#include "fwbuilder/CustomService.h"
#include "fwbuilder/FWReference.h"
#include "fwbuilder/FWObjectReference.h"
#include "fwbuilder/FWServiceReference.h"
#include "fwbuilder/FWIntervalReference.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/physAddress.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/AddressTable.h"
#include "fwbuilder/Group.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/ObjectGroup.h"
#include "fwbuilder/ServiceGroup.h"
#include "fwbuilder/IntervalGroup.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/UserService.h"

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
#  ifndef __GNUC__
#define snprintf sprintf_s
#  endif
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
    predictable_id_tracker = 0;
    ignore_read_only = false;

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
    predictable_id_tracker = 0;
    ignore_read_only = false;

    data_file = d.data_file;

    setName(TYPENAME);

    searchId =0;

    busy = true;
    *this = d;  // copies entire tree
    setId( ROOT_ID );

// I do not understand why do I need to reindex the whole database
// after operator=. It calls FWobject::duplicate, which in turn
// uses FWObject::shallowDuplicate, which calls addToIndex
// This is necessary however.
//
    addToIndexRecursive( this );  // reindex

    setDirty(false);
    busy = false;
}

FWObjectDatabase::~FWObjectDatabase()
{
    busy = true;
    //verifyTree(); // debugging
    destroyChildren();
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
        id_dict[DUMMY_ADDRESS_ID] = "dummyaddressid0";
        id_dict[DUMMY_SERVICE_ID] = "dummyserviceid0";
        id_dict[DUMMY_INTERFACE_ID] = "dummyinterfaceid0";

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
    snprintf(id_buf, sizeof(id_buf), "id%dX%d", i_id, cached_pid);
    id_dict[i_id] = string(id_buf);
    id_dict_reverse[string(id_buf)] = i_id;
    return id_dict[i_id];
}

string FWObjectDatabase::getPredictableId(const string &prefix)
{
    ostringstream str;
    str << prefix << predictable_id_tracker;
    string new_id = str.str();
    predictable_id_tracker++;
    return new_id;
}

void FWObjectDatabase::_setPredictableStrIdsRecursively(FWObject *obj)
{
   if (obj->getBool(".seen_this")) return;

   if (!obj->isReadOnly() && !FWObjectDatabase::isA(obj) &&
       obj->getLibrary()->getId() != FWObjectDatabase::STANDARD_LIB_ID &&
       obj->getLibrary()->getId() != FWObjectDatabase::DELETED_OBJECTS_ID &&
       obj->getId() != -1)
   {
       string new_id = getPredictableId("id");
       int int_id = obj->getId();

       id_dict[int_id] = new_id;
       id_dict_reverse[new_id] = int_id;

       obj->setBool(".seen_this", true);
   }

   for (list<FWObject*>::iterator it=obj->begin(); it!=obj->end(); ++it)
   {
       _setPredictableStrIdsRecursively(*it);
   }
}

void FWObjectDatabase::_updateNonStandardObjectReferencesRecursively(
    FWObject *obj)
{
   for (list<FWObject*>::iterator it=obj->begin(); it!=obj->end(); ++it)
   {
       (*it)->updateNonStandardObjectReferences();
       _updateNonStandardObjectReferencesRecursively(*it);
   }
}

void FWObjectDatabase::setPredictableIds()
{
    _setPredictableStrIdsRecursively(this);
    _updateNonStandardObjectReferencesRecursively(this);
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
                            const std::string &template_dir)
{
    if(f=="") return;

    xmlDocPtr doc = XMLTools::loadFile(f, FWObjectDatabase::TYPENAME,
                                       FWObjectDatabase::DTD_FILE_NAME,
                                       upgrade, template_dir);
    
    xmlNodePtr root = xmlDocGetRootElement(doc);
    
    if(!root || !root->name || strcmp(XMLTools::FromXmlCast(root->name),
                                      FWObjectDatabase::TYPENAME)!=SAME)
    {
	xmlFreeDoc(doc);
        throw FWException("Data file has invalid structure: "+f);
    }
    
    try
    {
        busy = true;

        destroyChildren();
        clearIndex();

        fromXML(root);

        setDirty(false);
        setFileName(f);
    } catch (FWException &ex)
    {
        busy = false;
        throw(ex);
    }

    xmlFreeDoc(doc);

    busy = false;
}

void FWObjectDatabase::saveFile(const string &filename)
{
/* need to set flag 'busy' so we ignore read-only status. Some objects
 * modify themselves in toXML() (e.g. Management) so if they belong to
 * a read-only library, we can't save them to a file. It should be
 * safe to ignore read-only flag but save it though.
 */
    busy = true;

    xmlDocPtr doc = xmlNewDoc(XMLTools::ToXmlCast("1.0"));
    xmlNodePtr node = xmlNewNode(nullptr, XMLTools::StrToXmlCast(getName()));
    xmlDocSetRootElement(doc, node);
    xmlNewNs(node, XMLTools::ToXmlCast("http://www.fwbuilder.org/1.0/"), nullptr);

    toXML(xmlDocGetRootElement(doc));
    
    XMLTools::saveFile(doc,
                       filename,
                       FWObjectDatabase::TYPENAME,
                       FWObjectDatabase::DTD_FILE_NAME);

    xmlFreeDoc(doc);

    setDirty(false);
    busy = false;
}

void FWObjectDatabase::saveToBuffer(xmlChar **buffer, int *size)
{
/* need to set flag 'busy' so we ignore read-only status. Some objects
 * modify themselves in toXML() (e.g. Management) so if they belong to a
 * read-only library, we can't save them to a file. It should be safe
 * to ignore read-only flag but save it though.
 */
    busy = true;

    xmlDocPtr doc = xmlNewDoc(XMLTools::ToXmlCast("1.0"));
    xmlNodePtr node = xmlNewDocNode(doc, nullptr, XMLTools::StrToXmlCast(getName()), nullptr);
    xmlDocSetRootElement(doc, node);
    xmlNewNs(node, XMLTools::ToXmlCast("http://www.fwbuilder.org/1.0/"), nullptr);

    toXML(xmlDocGetRootElement(doc));

    XMLTools::dumpToMemory(doc,
                           buffer,
                           size,
                           FWObjectDatabase::TYPENAME,
                           FWObjectDatabase::DTD_FILE_NAME);
    xmlFreeDoc(doc);  
    //xmlCleanupParser();
//    setDirty(false);

    busy = false;
}

void FWObjectDatabase::fromXML(xmlNodePtr root)
{
    FWObject::fromXML(root);
    
    const char *n = XMLTools::FromXmlCast(xmlGetProp(root, XMLTools::ToXmlCast("lastModified")));
    if (n!=nullptr)
    {        
        int i = 0;
        istringstream str(n);
        str >> i;
        lastModified = i;
        XMLTools::FreeXmlBuff(n);
        //xmlFree((void*)n);
    }
}

xmlNodePtr FWObjectDatabase::toXML(xmlNodePtr parent)
{
    FWObject *o;

    //xmlNewProp(parent, nullptr, nullptr);

    xmlNewProp(parent, 
               XMLTools::ToXmlCast("version") ,
               XMLTools::ToXmlCast(FWBUILDER_XML_VERSION));

    if (lastModified!=0)
    {
        ostringstream str;
        str << lastModified;
        xmlNewProp(parent, 
                   XMLTools::ToXmlCast("lastModified"),
                   XMLTools::ToXmlCast(str.str().c_str()));
    }

    int rootid = getId();

    //NOTUSED xmlAttrPtr pr =
    xmlNewProp(parent,
               XMLTools::ToXmlCast("id") ,
               XMLTools::StrToXmlCast(id_dict[rootid]));

    //xmlAddID(nullptr, parent->doc, XMLTools::StrToXmlCast(id_dict[rootid]), pr);

    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
    {
        if ((o=(*j))!=nullptr) o->toXML(parent);
    }
    return parent;
}

void  FWObjectDatabase::setDirty(bool f)
{
    dirty=f;
    if(!busy && f) lastModified=time(nullptr);
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
    if (obj_index.count(id)==0) return nullptr;
    return obj_index[id];
}

FWObject* FWObjectDatabase::findInIndex(int id)
{
    if (id < 0) return nullptr;
    FWObject *o = checkIndex(id);
    if (o!=nullptr) index_hits++;
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

void FWObjectDatabase::validateIndex()
{
    std::map<int, FWObject*>::iterator it;
    for (it=obj_index.begin(); it!=obj_index.end(); ++it)
    {
        if (it->second->getRoot() != this)
        {
            cerr << "Object '" << it->second->getName() << "'"
                 << " ( "
                 << it->second 
                 << " type " << it->second->getTypeName() << ")"
                 << " in index of db " << this
                 << " has incorrect db root ptr " 
                 << it->second->getRoot()
                 << endl;
        }
    }
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
    if (FWReference::cast(o)!=nullptr)
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
        if (FWReference::cast(*i)!=nullptr || RuleSet::cast(*i)!=nullptr) continue;
        recursivelyRemoveObjFromTree( *i , true);
    }

    if (remove_ref)  removeAllReferences(obj);
    else             removeAllInstances(obj);
}


