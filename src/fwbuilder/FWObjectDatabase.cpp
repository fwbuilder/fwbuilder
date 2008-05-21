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
#include <signal.h>

#include <fwbuilder/libfwbuilder-config.h>

#include <fwbuilder/FWObject.h>
#include <fwbuilder/FWObjectDatabase.h>

#include <fwbuilder/FWObject.h>
#include <fwbuilder/Library.h>
#include <fwbuilder/Interval.h>
#include <fwbuilder/ICMPService.h>
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
#include <fwbuilder/InterfacePolicy.h>
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

using namespace std;
using namespace libfwbuilder;

IDcounter::IDcounter()
{
    cntr=time(NULL);
}

IDcounter          IDc;

const char*        FWObjectDatabase::TYPENAME  = {"FWObjectDatabase"};
//long               FWObjectDatabase::IDcounter = 0;
//Mutex FWObjectDatabase::IDmutex;
const string FWObjectDatabase::DTD_FILE_NAME  = "fwbuilder.dtd"    ;


FWObjectDatabase::FWObjectDatabase():FWObject(), data_file()
{
    setRoot(this);
    index_hits = index_misses = 0;

//    if(db==NULL) db=this;

    searchId =0;
    lastModified = 0;

    setName(TYPENAME);

    setId( getRootId() );
    setDirty(false);
}

FWObjectDatabase::FWObjectDatabase(FWObjectDatabase& d):FWObject()
{
    setRoot(this);
    index_hits = index_misses = 0;

    data_file = d.data_file;

    setName(TYPENAME);

    searchId =0;

    init=true;
    *this = d;  // copies entire tree
    setId( getRootId() );

// I do not understand why do I need to reindex the whole database
// after operator=. It calls FWobject::duplicate, which in turn
// uses FWObject::shallowDuplicate, which calls addToIndex
// This is necessary however.
//
    addToIndexRecursive( this );  // reindex

    setDirty(false);
    init=false;
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
    
    xmlDocPtr doc=XMLTools::loadFile(f, FWObjectDatabase::TYPENAME,
                                     FWObjectDatabase::DTD_FILE_NAME,
                                     upgrade, template_dir);
    
    xmlNodePtr root = xmlDocGetRootElement(doc);
    
    if(!root || !root->name || strcmp(FROMXMLCAST(root->name), FWObjectDatabase::TYPENAME)!=SAME)
    {
	xmlFreeDoc(doc);
        throw FWException("Data file have invalid structure: "+f);
    }
    
    try {
        init=true;

//        clearChildren();
        deleteChildren();
        clearIndex();

        fromXML(root);
        setDirty(false);
        setFileName(f);
    } catch (FWException &ex)
    {
        init=false;
        throw(ex);
    }
    init=false;
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
        sscanf(n,"%d",&i);
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
        char lmbuf[32];
        sprintf(lmbuf,"%ld",lastModified);
        xmlNewProp(parent, 
                   TOXMLCAST("lastModified"),
                   TOXMLCAST(lmbuf));
    }

    string rootid = getId();

    xmlAttrPtr pr = xmlNewProp(parent, 
			       TOXMLCAST("id") , 
			       STRTOXMLCAST(rootid));

    xmlAddID(NULL, parent->doc, STRTOXMLCAST(rootid), pr);


    for(list<FWObject*>::const_iterator j=begin(); j!=end(); ++j) 
        if((o=(*j))!=NULL)   
            o->toXML(parent);
    
    return parent;
}

string FWObjectDatabase::generateUniqueId()
{
    char res[20];

    //TODO: mutex could not be used since threads
    // library is not initialized.

//    IDmutex.lock();
    int pid=0;
#ifdef _WIN32
    pid = _getpid();
#else
    pid = getpid();
#endif
    

    sprintf(res,"id%lX%d",IDc.get(),pid);
//    IDc++;
//    IDmutex.unlock();
    
    return res;
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
        if (!o->getId().empty() ) obj_index[o->getId()] = o;
    }
}

void FWObjectDatabase::removeFromIndex(const string &id)
{
    obj_index.erase(id);
}

FWObject* FWObjectDatabase::checkIndex(const std::string &id)
{
    FWObject *o = obj_index[id];
    return o;
}

FWObject* FWObjectDatabase::findInIndex(const std::string &id)
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

void FWObjectDatabase::getIndexStats(int &index_size, int &hit_counter, int &miss_counter)
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
FWObject *FWObjectDatabase::createFromXML(xmlNodePtr data)
{
    const char *n;
    string typen, id;

    n=FROMXMLCAST(data->name);
    if(!n)
        return NULL;
    typen = n;

    n=FROMXMLCAST(xmlGetProp(data,TOXMLCAST("id")));
    if(n)
    {
        id = n;
        FREEXMLBUFF(id);
    }

// create new object but do not prepopulate objects that
// automatically create children in constructor
    return create(typen,id,false);   
}


#define CREATE_OBJ(classname,str,id,prepopulate)     \
if(strcmp(classname::TYPENAME,str)==SAME) \
{ \
  nobj=new classname(this,prepopulate); \
  if (!id.empty()) nobj->setId(id); \
  addToIndex(nobj); \
  return nobj; \
}


FWObject *FWObjectDatabase::create(const string &type_name,
                                   const std::string &id,
                                   bool prepopulate)
{
    const char *n=type_name.c_str();
    FWObject   *nobj;

    if (strcmp("comment",n)==SAME) return NULL;

    if(strcmp("AnyNetwork"  ,n)==SAME)
    {
        nobj = new Network(this,prepopulate);
        if (!id.empty()) nobj->setId(id);
        nobj->setXMLName("AnyNetwork");
        addToIndex(nobj);
        return nobj;
    }

    if(strcmp("AnyIPService",n)==SAME)
    {
        nobj = new IPService(this,prepopulate);
        if (!id.empty()) nobj->setId(id);
        nobj->setXMLName("AnyIPService");
        addToIndex(nobj);
        return nobj;
    }

    if(strcmp("AnyInterval" ,n)==SAME)
    {
        nobj = new Interval(this,prepopulate);
        if (!id.empty()) nobj->setId(id);
        nobj->setXMLName("AnyInterval");
        addToIndex(nobj);
        return nobj;
    }


    CREATE_OBJ(Library             ,n, id, prepopulate);
    CREATE_OBJ(Policy              ,n, id, prepopulate);
    CREATE_OBJ(InterfacePolicy     ,n, id, prepopulate);
    CREATE_OBJ(NAT                 ,n, id, prepopulate);
    CREATE_OBJ(Routing             ,n, id, prepopulate);
    CREATE_OBJ(PolicyRule          ,n, id, prepopulate);
    CREATE_OBJ(NATRule             ,n, id, prepopulate);
    CREATE_OBJ(RoutingRule         ,n, id, prepopulate);
    CREATE_OBJ(RuleElementSrc      ,n, id, prepopulate);
    CREATE_OBJ(RuleElementDst      ,n, id, prepopulate);
    CREATE_OBJ(RuleElementSrv      ,n, id, prepopulate);
    CREATE_OBJ(RuleElementItf      ,n, id, prepopulate);
    CREATE_OBJ(RuleElementOSrc     ,n, id, prepopulate);
    CREATE_OBJ(RuleElementODst     ,n, id, prepopulate);
    CREATE_OBJ(RuleElementOSrv     ,n, id, prepopulate);
    CREATE_OBJ(RuleElementTSrc     ,n, id, prepopulate);
    CREATE_OBJ(RuleElementTDst     ,n, id, prepopulate);
    CREATE_OBJ(RuleElementTSrv     ,n, id, prepopulate);
    CREATE_OBJ(RuleElementInterval ,n, id, prepopulate);
    CREATE_OBJ(RuleElementRDst     ,n, id, prepopulate);
    CREATE_OBJ(RuleElementRGtw     ,n, id, prepopulate);
    CREATE_OBJ(RuleElementRItf     ,n, id, prepopulate);
    CREATE_OBJ(HostOptions         ,n, id, prepopulate);
    CREATE_OBJ(FirewallOptions     ,n, id, prepopulate);
    CREATE_OBJ(PolicyRuleOptions   ,n, id, prepopulate);
    CREATE_OBJ(NATRuleOptions      ,n, id, prepopulate);
    CREATE_OBJ(RoutingRuleOptions  ,n, id, prepopulate);
    CREATE_OBJ(Host                ,n, id, prepopulate);
    CREATE_OBJ(AddressRange        ,n, id, prepopulate);
    CREATE_OBJ(Management          ,n, id, prepopulate);
    CREATE_OBJ(PolicyInstallScript ,n, id, prepopulate);
    CREATE_OBJ(SNMPManagement      ,n, id, prepopulate);
    CREATE_OBJ(FWBDManagement      ,n, id, prepopulate);
    CREATE_OBJ(Firewall            ,n, id, prepopulate);
    CREATE_OBJ(Network             ,n, id, prepopulate);
    CREATE_OBJ(NetworkIPv6         ,n, id, prepopulate);
    CREATE_OBJ(Interface           ,n, id, prepopulate);
    CREATE_OBJ(IPv4                ,n, id, prepopulate);
    CREATE_OBJ(IPv6                ,n, id, prepopulate);
    CREATE_OBJ(physAddress         ,n, id, prepopulate);
    CREATE_OBJ(DNSName             ,n, id, prepopulate);
    CREATE_OBJ(AddressTable        ,n, id, prepopulate);
    CREATE_OBJ(FWObjectReference   ,n, id, prepopulate);
    CREATE_OBJ(FWServiceReference  ,n, id, prepopulate);
    CREATE_OBJ(FWIntervalReference ,n, id, prepopulate);
    CREATE_OBJ(TCPService          ,n, id, prepopulate);
    CREATE_OBJ(UDPService          ,n, id, prepopulate);
    CREATE_OBJ(TagService          ,n, id, prepopulate);
    CREATE_OBJ(ICMPService         ,n, id, prepopulate);
    CREATE_OBJ(IPService           ,n, id, prepopulate);
    CREATE_OBJ(CustomService       ,n, id, prepopulate);
    CREATE_OBJ(UserService         ,n, id, prepopulate);
    CREATE_OBJ(Interval            ,n, id, prepopulate);
    CREATE_OBJ(ObjectGroup         ,n, id, prepopulate);
    CREATE_OBJ(ServiceGroup        ,n, id, prepopulate);
    CREATE_OBJ(IntervalGroup       ,n, id, prepopulate);

    cerr << "FWObjectDatabase::create: ERROR: attempt to create object of unknown type '" 
         << type_name << "'" << endl;

    //TODO: throw exception
    nobj = NULL;

    return nobj;
}

Firewall* FWObjectDatabase::_findFirewallByNameRecursive(FWObject* db,
                                         const string &name) throw(FWException)
{
    if (Firewall::isA(db) &&
        db->getName()==name &&
        db->getParent()->getId()!=getDeletedObjectsId())
        return Firewall::cast(db);

    list<FWObject*>::iterator j;
    for(j=db->begin(); j!=db->end(); ++j)     
    {
        FWObject *o=*j;

        o=_findFirewallByNameRecursive(o,name);
        if(o) return Firewall::cast(o);
    }
    if (db==this)
        throw FWException("Firewall object '"+name+"' not found");
    return NULL; // not found
}

Firewall* FWObjectDatabase::findFirewallByName(const string &name) throw(FWException)
{
    return _findFirewallByNameRecursive(this,name);
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
        if (FWReference::cast(*i)!=NULL ||
            RuleSet::cast(*i)!=NULL) continue;
        recursivelyRemoveObjFromTree( *i , true);
    }

    if (remove_ref)  removeAllReferences(obj);
    else             removeAllInstances(obj);
}




class FWObjectTreeScanner {

    FWObjectDatabase            *treeRoot;
    map<string,FWObject*>        srcMap;
    map<string,FWObject*>        dstMap;
    FWObjectDatabase::ConflictResolutionPredicate *crp;
    bool                         defaultCrp;

    void walkTree(map<string,FWObject*> &m,FWObject *root);
    void addRecursively(FWObject *src);

    public:

    FWObjectTreeScanner(FWObject *r,
                        FWObjectDatabase::ConflictResolutionPredicate *_crp=NULL)
    {
        treeRoot=FWObjectDatabase::cast(r);
        defaultCrp=false;
        if (_crp==NULL)
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
void FWObjectTreeScanner::walkTree(map<string,FWObject*> &m,FWObject *root)
{
    if (root->exists("id"))  m[root->getId()]=root;

    if (FWReference::cast(root)!=NULL)
    {
        FWReference *r=FWReference::cast(root);
        m[string("ref_")+r->getPointerId()]=root;
    }

    for (FWObject::iterator i=root->begin(); i!=root->end(); i++)
    {
        walkTree(m, *i);
    }
}

void FWObjectTreeScanner::addRecursively(FWObject *src)
{
    if (treeRoot->getId()==src->getId()) return ;

    addRecursively(src->getParent());

    if (dstMap[src->getId()]==NULL)
    {
        FWObject *o1=treeRoot->create(src->getTypeName(),"",false);
        FWObject *pdst = dstMap[src->getParent()->getId()];
        assert(pdst!=NULL);

        pdst->add(o1);

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
                if (Group::cast(src)!=NULL && FWReference::cast(src->front())!=NULL)
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
    if (dst==NULL)
    {
        dst=treeRoot;
        walkTree(dstMap,treeRoot);
        walkTree(srcMap, source);
    }

    for (FWObject::iterator i=dst->begin(); i!=dst->end(); i++)
    {
        FWObject *o1=*i;
        if (FWReference::cast(o1)!=NULL)
        {
            string   pid   = FWReference::cast(o1)->getPointerId();
            FWObject *o2   = dstMap[pid];

            if (o2==NULL)
            {
                FWObject *osrc = srcMap[ pid ];
                addRecursively( osrc);
            }
        } else
            scanAndAdd( o1 , source );
    }
    if (Interface::isA(dst))
    {
        string   pid = dst->getStr("network_zone");
        if ( !pid.empty() )
        {
            FWObject *o2 = dstMap[pid];
            if (o2==NULL)
            {
                FWObject *osrc = srcMap[ pid ];
                addRecursively( osrc);
            }
        }
    }
}

//#define DEBUG_MERGE 1

void FWObjectTreeScanner::merge(FWObject *dst,FWObject *src)
{
    string dobjId = FWObjectDatabase::getDeletedObjectsId();

    if (dst==NULL)
    {
        dst=treeRoot;
        walkTree(dstMap,treeRoot);
        walkTree(srcMap, src);

        /**
         * find deleted objects library in src and check if any object
         * from it is present in dst
         */
        FWObjectDatabase *dstroot = dst->getRoot();

        /* commented out 11/12/04
         * If src has some deleted objects, we should just ignore them
         */

#if 0
        FWObjectDatabase *srcroot = src->getRoot();
        FWObject *srcdobj = srcroot->getById( dobjId );
        if (srcdobj)
        {
            for (FWObject::iterator i=srcdobj->begin(); i!=srcdobj->end(); i++)
            {
                FWObject *dobj = dstMap[ (*i)->getId() ];
                if(dobj!=NULL && dobj->getParent()->getId()!=dobjId)
                {
                    crp->askUser( dobj, *i );
                    dstroot->recursivelyRemoveObjFromTree(dobj);
                }
            }
        }
#endif

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
                if(sobj!=NULL && sobj->getParent()->getId()!=dobjId)
                    deletedObjects.push_back(*i);
            }
            for (FWObject::iterator i=deletedObjects.begin(); i!=deletedObjects.end(); i++)
            {
                dstroot->recursivelyRemoveObjFromTree( *i );
                dstMap[ (*i)->getId() ] = NULL;
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
            dobj= dstMap[ string("ref_")+r->getPointerId() ];
        } else dobj= dstMap[ (*i)->getId() ];

        if (dobj==NULL)
        {
            sobj = *i;
            FWObject *o1=treeRoot->create( sobj->getTypeName());

            FWObject *pdst = dstMap[ src->getId() ];
            assert(pdst!=NULL);

            pdst->add(o1);

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
            if (dobj->cmp( *i )) continue;

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


            if (Group::cast(dobj)!=NULL)
            {
                FWObject *firstChild=NULL;
                if (dobj->size()>0)         firstChild= dobj->front();
                else
                {
                    if ( (*i)->size()>0 )   firstChild= (*i)->front();
                }
                if (firstChild==NULL || FWReference::cast(firstChild)!=NULL)
                {
                    if (crp!=NULL && crp->askUser( dobj, *i ))
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
            else
            {
                if (crp!=NULL && crp->askUser( dobj, *i ))
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

    ndb->init = true;

    for (list<FWObject*>::const_iterator i=libs.begin(); i!=libs.end(); i++)
    {
        FWObject *lib = *i;
        FWObject *nlib = ndb->create(lib->getTypeName());
        ndb->add(nlib);
        *nlib = *lib;
    }

    FWObjectTreeScanner scanner(ndb);
    scanner.scanAndAdd(NULL, this);

    ndb->init = false;

    return ndb;
}

FWObjectDatabase* FWObjectDatabase::exportSubtree( FWObject *lib )
{
    FWObjectDatabase *ndb = new FWObjectDatabase();

    ndb->init = true;

    FWObject *nlib = ndb->create(lib->getTypeName());
    ndb->add(nlib);
    *nlib = *lib;

    FWObjectTreeScanner scanner(ndb);
    scanner.scanAndAdd(NULL, this);

    ndb->init = false;

    return ndb;
}

void FWObjectDatabase::merge( FWObjectDatabase *ndb,
                              ConflictResolutionPredicate *crp)
{
    init = true;

    FWObjectTreeScanner scanner(this, crp);
    scanner.merge(NULL, ndb);

    init = false;
}

/*
 * find all objects used by the group 'gr'. REsolves references
 * recursively (that is, if a group member is another group, this
 * method descends into it and scans all objects that group uses)
 */
void FWObjectDatabase::findObjectsInGroup(Group *g,set<FWObject *> &res)
{
    searchId++;
    _findObjectsInGroup(g, res);
}

void FWObjectDatabase::_findObjectsInGroup(Group *g,set<FWObject *> &res)
{
    if (g->size()==0 || g->getInt(".searchId")==searchId) return;
    
    g->setInt(".searchId",searchId);
    FWObjectReference *ref;
    Group *sg;
    FWObject *obj;
    FWObject::iterator i=g->begin();
    for(;i!=g->end();++i)
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

bool FWObjectDatabase::_isInIgnoreList(FWObject *o)
{
    return (FWOptions::cast(o)!=NULL);
}


/**
 *  find which firewalls and groups use object 'o' in a subtree rooted
 *  at object 'p' skip 'deleted objects' library, avoid circular group
 *  references using temporary flags set in objects this method
 *  visits.
 */
void FWObjectDatabase::findWhereUsed(FWObject *o,FWObject *p,set<FWObject *> &resset)
{
    searchId++;
    _findWhereUsed(o,p,resset);
}

bool FWObjectDatabase::_findWhereUsed(FWObject *o,FWObject *p,set<FWObject *> &resset)
{
    bool res=false;
        
    if ( _isInIgnoreList(p) || p->size()==0) return res;
    
    if (p->getInt(".searchId")==searchId)
    {
        res=p->getBool(".searchResult");
        return res;
    }

// set flags to break indefinite recursion in case we encounter circular groups
    p->setInt(".searchId",searchId);
    p->setBool(".searchResult",res);   // res==false at this time
   
    FWObject::iterator i1=p->begin();
    for (;i1!=p->end(); ++i1)
    {
        if ((*i1)->getId()==FWObjectDatabase::getDeletedObjectsId()) continue;

        FWReference  *ref=FWReference::cast(*i1);
        if (ref!=NULL)
        {  // child is a reference
            FWObject *g=ref->getPointer();
            if (o==g) res= true;
            else      res |= _findWhereUsed(o,g,resset);
        }
        else    // child is a regular object, not a reference
        {
            if (o==(*i1))  res=true;
            else           res |= _findWhereUsed(o,*i1,resset);
        }
    }

    p->setBool(".searchResult",res);
    if (res) resset.insert(p);
    return res;
}


