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


#ifndef __FWOBJECTDATABASE_HH_FLAG__
#define __FWOBJECTDATABASE_HH_FLAG__

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/XMLTools.h"

#ifdef _WIN32
#  include <sys/timeb.h>
#  include <process.h>
#else
#  include <sys/types.h>
#  include <unistd.h>
#endif

#include <time.h>   // for time_t

#define DECLARE_CREATE_OBJ_METHOD(classname) \
    FWObject* create_##classname(int id=-1);

#define DECLARE_CREATE_OBJ_CLASS_METHOD(classname) \
    classname * create##classname(int id=-1);
        

namespace libfwbuilder
{
    class Group;

    // forward declarations for specialized create() methods
    class AddressRange;
    class AddressTable;
    class AttachedNetworks;
    class Cluster;
    class StateSyncClusterGroup;
    class FailoverClusterGroup;
    class ClusterGroupOptions;
    class CustomService;
    class DNSName;
    class DynamicGroup;
    class FWBDManagement;
    class FWIntervalReference;
    class FWObjectReference;
    class FWServiceReference;
    class Firewall;
    class FirewallOptions;
    class Host;
    class HostOptions;
    class ICMP6Service;
    class ICMPService;
    class IPService;
    class IPv4;
    class IPv6;
    class Interface;
    class InterfaceOptions;
    class Interval;
    class IntervalGroup;
    class Library;
    class Management;
    class NAT;
    class NATRule;
    class NATRuleOptions;
    class Network;
    class NetworkIPv6;
    class ObjectGroup;
    class Policy;
    class PolicyInstallScript;
    class PolicyRule;
    class PolicyRuleOptions;
    class Routing;
    class RoutingRule;
    class RoutingRuleOptions;
    class RuleElementDst;
    class RuleElementInterval;
    class RuleElementItf;
    class RuleElementItfInb;
    class RuleElementItfOutb;
    class RuleElementODst;
    class RuleElementOSrc;
    class RuleElementOSrv;
    class RuleElementRDst;
    class RuleElementRGtw;
    class RuleElementRItf;
    class RuleElementSrc;
    class RuleElementSrv;
    class RuleElementTDst;
    class RuleElementTSrc;
    class RuleElementTSrv;
    class RuleSetOptions;
    class SNMPManagement;
    class ServiceGroup;
    class TCPService;
    class TagService;
    class UDPService;
    class UserService;
    class physAddress;


    
    DECLARE_CREATE_OBJ_METHOD(AddressRange);
    DECLARE_CREATE_OBJ_METHOD(AddressTable);
    DECLARE_CREATE_OBJ_METHOD(AttachedNetworks);
    DECLARE_CREATE_OBJ_METHOD(Cluster);
    DECLARE_CREATE_OBJ_METHOD(StateSyncClusterGroup);
    DECLARE_CREATE_OBJ_METHOD(FailoverClusterGroup);
    DECLARE_CREATE_OBJ_METHOD(ClusterGroupOptions);
    DECLARE_CREATE_OBJ_METHOD(CustomService);
    DECLARE_CREATE_OBJ_METHOD(DNSName);
    DECLARE_CREATE_OBJ_METHOD(DynamicGroup);
    DECLARE_CREATE_OBJ_METHOD(FWBDManagement);
    DECLARE_CREATE_OBJ_METHOD(FWIntervalReference);
    DECLARE_CREATE_OBJ_METHOD(FWObjectReference);
    DECLARE_CREATE_OBJ_METHOD(FWServiceReference);
    DECLARE_CREATE_OBJ_METHOD(Firewall);
    DECLARE_CREATE_OBJ_METHOD(FirewallOptions);
    DECLARE_CREATE_OBJ_METHOD(Host);
    DECLARE_CREATE_OBJ_METHOD(HostOptions);
    DECLARE_CREATE_OBJ_METHOD(ICMP6Service);
    DECLARE_CREATE_OBJ_METHOD(ICMPService);
    DECLARE_CREATE_OBJ_METHOD(IPService);
    DECLARE_CREATE_OBJ_METHOD(IPv4);
    DECLARE_CREATE_OBJ_METHOD(IPv6);
    DECLARE_CREATE_OBJ_METHOD(Interface);
    DECLARE_CREATE_OBJ_METHOD(InterfaceOptions);
    DECLARE_CREATE_OBJ_METHOD(Interval);
    DECLARE_CREATE_OBJ_METHOD(IntervalGroup);
    DECLARE_CREATE_OBJ_METHOD(Library);
    DECLARE_CREATE_OBJ_METHOD(Management);
    DECLARE_CREATE_OBJ_METHOD(NAT);
    DECLARE_CREATE_OBJ_METHOD(NATRule);
    DECLARE_CREATE_OBJ_METHOD(NATRuleOptions);
    DECLARE_CREATE_OBJ_METHOD(Network);
    DECLARE_CREATE_OBJ_METHOD(NetworkIPv6);
    DECLARE_CREATE_OBJ_METHOD(ObjectGroup);
    DECLARE_CREATE_OBJ_METHOD(Policy);
    DECLARE_CREATE_OBJ_METHOD(PolicyInstallScript);
    DECLARE_CREATE_OBJ_METHOD(PolicyRule);
    DECLARE_CREATE_OBJ_METHOD(PolicyRuleOptions);
    DECLARE_CREATE_OBJ_METHOD(Routing);
    DECLARE_CREATE_OBJ_METHOD(RoutingRule);
    DECLARE_CREATE_OBJ_METHOD(RoutingRuleOptions);
    DECLARE_CREATE_OBJ_METHOD(RuleElementDst);
    DECLARE_CREATE_OBJ_METHOD(RuleElementInterval);
    DECLARE_CREATE_OBJ_METHOD(RuleElementItf);
    DECLARE_CREATE_OBJ_METHOD(RuleElementItfInb);
    DECLARE_CREATE_OBJ_METHOD(RuleElementItfOutb);
    DECLARE_CREATE_OBJ_METHOD(RuleElementODst);
    DECLARE_CREATE_OBJ_METHOD(RuleElementOSrc);
    DECLARE_CREATE_OBJ_METHOD(RuleElementOSrv);
    DECLARE_CREATE_OBJ_METHOD(RuleElementRDst);
    DECLARE_CREATE_OBJ_METHOD(RuleElementRGtw);
    DECLARE_CREATE_OBJ_METHOD(RuleElementRItf);
    DECLARE_CREATE_OBJ_METHOD(RuleElementSrc);
    DECLARE_CREATE_OBJ_METHOD(RuleElementSrv);
    DECLARE_CREATE_OBJ_METHOD(RuleElementTDst);
    DECLARE_CREATE_OBJ_METHOD(RuleElementTSrc);
    DECLARE_CREATE_OBJ_METHOD(RuleElementTSrv);
    DECLARE_CREATE_OBJ_METHOD(RuleSetOptions);
    DECLARE_CREATE_OBJ_METHOD(SNMPManagement);
    DECLARE_CREATE_OBJ_METHOD(ServiceGroup);
    DECLARE_CREATE_OBJ_METHOD(TCPService);
    DECLARE_CREATE_OBJ_METHOD(TagService);
    DECLARE_CREATE_OBJ_METHOD(UDPService);
    DECLARE_CREATE_OBJ_METHOD(UserService);
    DECLARE_CREATE_OBJ_METHOD(physAddress);
    DECLARE_CREATE_OBJ_METHOD(Group);

    
    class IDcounter {

protected:
        long cntr;
    
public:
        IDcounter();
        long get() { ++cntr; return cntr; }
    };

    class FWObjectDatabase;
    typedef FWObject*(*create_function_ptr)(int);

    /**
     * Database of objects.
     */
    class FWObjectDatabase : public FWObject
    {

private:
        void _clearReferenceCounters(FWObject *o);
        void _fixReferenceCounters(FWObject *o);
        bool _isInIgnoreList(FWObject *o);

        /* bool _findWhereUsed( */
        /*         libfwbuilder::FWObject *o, */
        /*         libfwbuilder::FWObject *p, */
        /*         std::set<libfwbuilder::FWObject *> &resset); */

        bool _findWhereObjectIsUsed(libfwbuilder::FWObject *o,
                                    libfwbuilder::FWObject *p,
                                    std::set<libfwbuilder::FWObject *> &resset,
                                    int search_id);
    
        void _findObjectsInGroup(
            libfwbuilder::Group *g,
            std::set<libfwbuilder::FWObject *> &res);
        Firewall* _findFirewallByNameRecursive(
            FWObject* db, const std::string &name);
        FWObject* _recursively_copy_subtree(FWObject *target,
                                            FWObject *source,
                                            std::map<int,int> &id_map,
                                            const std::string &dedup_attribute);
        void _copy_foreign_obj_aux(FWObject *target, FWObject *source,
                                   std::map<int,int> &id_map,
                                   const std::string &dedup_attribute);

        void _setPredictableStrIdsRecursively(FWObject *obj);
        void _updateNonStandardObjectReferencesRecursively(FWObject *obj);
        
protected:

        static const std::string DTD_FILE_NAME ;

        time_t lastModified;
        int index_hits;
        int index_misses;
        std::string data_file;
        std::map<int, FWObject*> obj_index;
        int searchId;
        int predictable_id_tracker;
        bool ignore_read_only;
        
        void init_create_methods_table();
        void init_id_dict();

public:

        DECLARE_FWOBJECT_SUBTYPE(FWObjectDatabase);

        DECLARE_DISPATCH_METHODS(FWObjectDatabase);
        
        enum {ROOT_ID = 0,
              ANY_ADDRESS_ID = 1,
              ANY_SERVICE_ID = 2,
              ANY_INTERVAL_ID = 3,
              STANDARD_LIB_ID = 4,
              USER_LIB_ID = 5,
              TEMPLATE_LIB_ID = 6,
              DELETED_OBJECTS_ID = 7,
              DUMMY_ADDRESS_ID = 8,
              DUMMY_SERVICE_ID = 9,
              DUMMY_INTERFACE_ID = 10
        } standard_ids;

        static void registerObjectType(const std::string &type_name,
                                       create_function_ptr create_function);
        
        /**
         * this constructor initializes singleton db
         */
        FWObjectDatabase();
        /**
         * this constructor makes a copy of entire tree and does not
         * intitialize db
         */
        FWObjectDatabase(FWObjectDatabase& d);

        virtual ~FWObjectDatabase();

        // --- methods dealing with object index

        void addToIndexRecursive(FWObject *o);

        /**
         * add an object to the index
         */
        void addToIndex(FWObject* obj);

        /**
         * remove an object from the index
         */
        void removeFromIndex(int id);

        /**
         * check if an object is present in the index using its ID
         */
        FWObject* checkIndex(int id);

        /**
         * find an object in the index using its ID
         */
        FWObject* findInIndex(int id);

        /**
         * build index
         */
        void buildIndex();

        /**
         * clear the index
         */
        void clearIndex();

        /**
         * this is just like buildIndex, except it also fixes reference
         * counters in objects. Call this method after loading database
         * from XML file. This method uses private _fixReferenceCounters
         */
        void reIndex();
    
        /**
         * return index usage statistics
         */
        void getIndexStats(int &index_size, int &hit_counter, int &miss_counter);

        /**
         * this function is intended for debugging.
         */
        void validateIndex();
        
        /**
         * Some operations, such as object tree merging, should ignore
         * read-only flag on individual objects.
         */
        bool getIgnoreReadOnlyFlag() { return ignore_read_only; }
        void setIgnoreReadOnlyFlag(bool f) { ignore_read_only = f; }

        // --- XML import/export ---
    
        virtual void fromXML(xmlNodePtr xml_parent_node);
        virtual xmlNodePtr toXML(xmlNodePtr parent);
    
        time_t getTimeLastModified() { return lastModified; }
        void resetTimeLastModified(time_t t) { lastModified=t; }

        // --- Load/Save ---
    
        virtual void saveFile(const std::string &filename);
        virtual void saveToBuffer(xmlChar **buffer,int *size);
        virtual void load( const std::string &filename,
                           XMLTools::UpgradePredicate *upgrade,
                           const std::string &template_dir);
        virtual void setDirty(bool f);

        Firewall* findFirewallByName(const std::string &name);

        FWObjectDatabase* exportSubtree( FWObject *lib );
        FWObjectDatabase* exportSubtree( const std::list<FWObject*> &libs );

   
        /* void findWhereUsed( */
        /*         libfwbuilder::FWObject *o, */
        /*         libfwbuilder::FWObject *p, */
        /*         std::set<libfwbuilder::FWObject *> &resset); */

        /**
         * Find reference to object <o> in the subtree rooted at <p>. Do
         * not search for indirect usage, i.e. when an object has a
         * reference to a group that in turn has reference to <o>. Search
         * also includes references to objects used in rule actions Tag
         * and Branch.
         */
        void findWhereObjectIsUsed(
            libfwbuilder::FWObject *o,
            libfwbuilder::FWObject *p,
            std::set<libfwbuilder::FWObject *> &resset);

        /**
         * find all objects used by the group 'gr'. Resolve references
         * recursively (that is, if a group member is another group, this
         * method descends into it and scans all objects that group uses)
         */
        void findObjectsInGroup(
            libfwbuilder::Group *g,
            std::set<libfwbuilder::FWObject *> &resset);

        /**
         * We ignore read-only flag on individual objects when whole object
         * tree is duplicated
         */
        virtual FWObject& duplicate(const FWObject *obj,
                                    bool preserve_id = true);

        
        void recursivelyRemoveObjFromTree(FWObject* obj, bool remove_ref=false);

        /**
         * Copy <source> object and all its children, recursively, into <this>
         * object tree starting from <target>. <target> is a parent of the copy
         * of <source> that will be created.
         * Store ID mapping in <id_map> (as a dictionary old_id -> new_id)
         */
        FWObject* recursivelyCopySubtree(FWObject *target,
                                         FWObject *source,
                                         std::map<int,int> &id_map);

        /**
         * Create groups to reproduce path inside given library. If groups
         * with required names exist, do nothing. Return pointer to the
         * last object created to copy the path. Do not copy <source> object.
         * This means returned object can be a parent for the copy of <source>.
         */
        FWObject* reproduceRelativePath(FWObject *lib, const FWObject *source);
    
        /**
         * fix references in children of obj according to the map_ids which
         * maps old IDs to the new ones. Return the number of fixed references.
         */
        int fixReferences(FWObject *obj, const std::map<int,int> &map_ids);
    
        /**
         * this predicate is used to hand control over to user in case
         * when a conflict is detected while merging trees. By default the
         * old object is overwritten with new one. 
         */
        class ConflictResolutionPredicate
        {
    public:
            virtual ~ConflictResolutionPredicate() {}
            virtual bool askUser(FWObject*,FWObject*) 
            { 
                return true;
            }
        };

        void merge(FWObjectDatabase *ndb, ConflictResolutionPredicate *mp=nullptr);
        void findDuplicateIds(FWObjectDatabase *ndb, std::set<int> &dupids);
        
        void setFileName(const std::string &filename);
        const std::string& getFileName ();
        const std::string  getFileDir ();

        static int generateUniqueId();
        static int registerStringId(const std::string &s_id);
        static int getIntId(const std::string &s_id);
        static std::string getStringId(int i_id);

        /**
         * generate predictable ID based on given prefix by adding sequential
         * suffix to it.
         */
        std::string getPredictableId(const std::string &prefix);
        
        /**
         * This method replaces random string object ids with
         * predictable ones. This does not change their int IDs, only
         * string IDs that appear in the XML file when objects are
         * saved change.
         *
         * Used in unit testing to create .fwb files that can be
         * compared.
         */
        virtual void setPredictableIds();
        
        /**
         * This is the main "Create" method:
         * it creates instance of FWObject of given type
         *
         * if parameter 'create_with_root' is true, this method will create
         * objects using constructor that uses pointer to this as a parameter,
         * otherwise empty constructor is used
         */
        FWObject *create(const std::string &type, int id=-1, bool init=true);

        /**
         * Creates instance of FWObject using its XML representation
         */
        virtual FWObject *createFromXML(xmlNodePtr data);

        /**
         * Specialized createClass() methods: class name is part of the method
         * name, e.g. createLibrary(), also these return a pointer to the
         * corresponding class. Note that each macro declares two methods:
         * Class* createClass(int,bool) and FWObject* createFWObjectClass(int,bool)
         */

    DECLARE_CREATE_OBJ_CLASS_METHOD(AddressRange);
    DECLARE_CREATE_OBJ_CLASS_METHOD(AddressTable);
    DECLARE_CREATE_OBJ_CLASS_METHOD(AttachedNetworks);
    DECLARE_CREATE_OBJ_CLASS_METHOD(Cluster);
    DECLARE_CREATE_OBJ_CLASS_METHOD(StateSyncClusterGroup);
    DECLARE_CREATE_OBJ_CLASS_METHOD(FailoverClusterGroup);
    DECLARE_CREATE_OBJ_CLASS_METHOD(ClusterGroupOptions);
    DECLARE_CREATE_OBJ_CLASS_METHOD(CustomService);
    DECLARE_CREATE_OBJ_CLASS_METHOD(DNSName);
    DECLARE_CREATE_OBJ_CLASS_METHOD(DynamicGroup);
    DECLARE_CREATE_OBJ_CLASS_METHOD(FWBDManagement);
    DECLARE_CREATE_OBJ_CLASS_METHOD(FWIntervalReference);
    DECLARE_CREATE_OBJ_CLASS_METHOD(FWObjectReference);
    DECLARE_CREATE_OBJ_CLASS_METHOD(FWServiceReference);
    DECLARE_CREATE_OBJ_CLASS_METHOD(Firewall);
    DECLARE_CREATE_OBJ_CLASS_METHOD(FirewallOptions);
    DECLARE_CREATE_OBJ_CLASS_METHOD(Host);
    DECLARE_CREATE_OBJ_CLASS_METHOD(HostOptions);
    DECLARE_CREATE_OBJ_CLASS_METHOD(ICMP6Service);
    DECLARE_CREATE_OBJ_CLASS_METHOD(ICMPService);
    DECLARE_CREATE_OBJ_CLASS_METHOD(IPService);
    DECLARE_CREATE_OBJ_CLASS_METHOD(IPv4);
    DECLARE_CREATE_OBJ_CLASS_METHOD(IPv6);
    DECLARE_CREATE_OBJ_CLASS_METHOD(Interface);
    DECLARE_CREATE_OBJ_CLASS_METHOD(InterfaceOptions);
    DECLARE_CREATE_OBJ_CLASS_METHOD(Interval);
    DECLARE_CREATE_OBJ_CLASS_METHOD(IntervalGroup);
    DECLARE_CREATE_OBJ_CLASS_METHOD(Library);
    DECLARE_CREATE_OBJ_CLASS_METHOD(Management);
    DECLARE_CREATE_OBJ_CLASS_METHOD(NAT);
    DECLARE_CREATE_OBJ_CLASS_METHOD(NATRule);
    DECLARE_CREATE_OBJ_CLASS_METHOD(NATRuleOptions);
    DECLARE_CREATE_OBJ_CLASS_METHOD(Network);
    DECLARE_CREATE_OBJ_CLASS_METHOD(NetworkIPv6);
    DECLARE_CREATE_OBJ_CLASS_METHOD(ObjectGroup);
    DECLARE_CREATE_OBJ_CLASS_METHOD(Policy);
    DECLARE_CREATE_OBJ_CLASS_METHOD(PolicyInstallScript);
    DECLARE_CREATE_OBJ_CLASS_METHOD(PolicyRule);
    DECLARE_CREATE_OBJ_CLASS_METHOD(PolicyRuleOptions);
    DECLARE_CREATE_OBJ_CLASS_METHOD(Routing);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RoutingRule);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RoutingRuleOptions);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementDst);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementInterval);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementItf);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementItfInb);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementItfOutb);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementODst);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementOSrc);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementOSrv);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementRDst);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementRGtw);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementRItf);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementSrc);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementSrv);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementTDst);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementTSrc);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleElementTSrv);
    DECLARE_CREATE_OBJ_CLASS_METHOD(RuleSetOptions);
    DECLARE_CREATE_OBJ_CLASS_METHOD(SNMPManagement);
    DECLARE_CREATE_OBJ_CLASS_METHOD(ServiceGroup);
    DECLARE_CREATE_OBJ_CLASS_METHOD(TCPService);
    DECLARE_CREATE_OBJ_CLASS_METHOD(TagService);
    DECLARE_CREATE_OBJ_CLASS_METHOD(UDPService);
    DECLARE_CREATE_OBJ_CLASS_METHOD(UserService);
    DECLARE_CREATE_OBJ_CLASS_METHOD(physAddress);
    DECLARE_CREATE_OBJ_CLASS_METHOD(Group);

        
    };

}

#endif

