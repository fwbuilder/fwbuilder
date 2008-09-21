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

#include <fwbuilder/FWObject.h>
#include <fwbuilder/FWException.h>
#include <fwbuilder/ThreadTools.h>
#include <fwbuilder/XMLTools.h>

#ifdef _WIN32
#  include <sys/timeb.h>
#  include <process.h>
#else
#  include <sys/types.h>
#  include <unistd.h>
#endif

#include <time.h>   // for time_t

namespace libfwbuilder
{
    class Group;
    class Firewall;

class IDcounter {

 protected:
    long cntr;
    
 public:
    IDcounter();
    long get() { ++cntr; return cntr; }
};
    
/**
 * Database of objects.
 */
class FWObjectDatabase : public FWObject
{
private:
    void _clearReferenceCounters(FWObject *o);
    void _fixReferenceCounters(FWObject *o);
    bool _isInIgnoreList(FWObject *o);
    
protected:

    static const std::string         DTD_FILE_NAME ;
    time_t                           lastModified;

    int                              index_hits;
    int                              index_misses;
    std::map<int, FWObject*>         obj_index;
    int                              searchId;

    std::string data_file;

    Firewall* _findFirewallByNameRecursive(FWObject* db,const std::string &name) throw(FWException);
    void init_id_dict();
    

public:

    DECLARE_FWOBJECT_SUBTYPE(FWObjectDatabase);

    enum {ROOT_ID = 0,
          ANY_ADDRESS_ID = 1,
          ANY_SERVICE_ID = 2,
          ANY_INTERVAL_ID = 3,
          STANDARD_LIB_ID = 4,
          USER_LIB_ID = 5,
          TEMPLATE_LIB_ID = 6,
          DELETED_OBJECTS_ID = 7
    } standard_ids;
    
    /**
     * this constructor initializes singleton db
     */
    FWObjectDatabase();
    /**
     * this constructor makes a copy of entire tree and does not
     * intitialize db
     */
    FWObjectDatabase(FWObjectDatabase& d);

    virtual ~FWObjectDatabase() {};

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

    // --- XML import/export ---
    
    /**
     * This is the main "Create" method:
     * it creates instance of FWObject of given type
     *
     * if parameter 'create_with_root' is true, this method will create
     * objects using constructor that uses pointer to this as a parameter,
     * otherwise empty constructor is used
     */
    FWObject *create(const std::string &type, int id=-1, bool prepopulate=true);

    /**
     * Creates instance of FWObject using its XML representation
     */
    virtual FWObject *createFromXML(xmlNodePtr data);

    virtual void       fromXML    (xmlNodePtr xml_parent_node) throw(FWException);
    virtual xmlNodePtr toXML(xmlNodePtr parent) throw(FWException);
    
    time_t getTimeLastModified()           { return lastModified; }
    void   resetTimeLastModified(time_t t) { lastModified=t; }

    // --- Load/Save ---
    
    virtual void saveFile ( const std::string &filename) throw(FWException); 
    virtual void saveXML  ( xmlDocPtr                  ) throw(FWException); 
    virtual void saveToBuffer(xmlChar **buffer,int *size) throw(FWException);
    virtual void load     ( const std::string &filename,
                            XMLTools::UpgradePredicate *upgrade,
                            const std::string &template_dir) throw(FWException);
    virtual void    setDirty(bool f);

    Firewall* findFirewallByName(const std::string &name) throw(FWException);

    FWObjectDatabase* exportSubtree( FWObject *lib );
    FWObjectDatabase* exportSubtree( const std::list<FWObject*> &libs );


    void findWhereUsed(
            libfwbuilder::FWObject *o,
            libfwbuilder::FWObject *p,
            std::set<libfwbuilder::FWObject *> &resset);

    bool _findWhereUsed(
            libfwbuilder::FWObject *o,
            libfwbuilder::FWObject *p,
            std::set<libfwbuilder::FWObject *> &resset);
    
    void findObjectsInGroup(
            libfwbuilder::Group *g,
            std::set<libfwbuilder::FWObject *> &res);
    void _findObjectsInGroup(
            libfwbuilder::Group *g,
            std::set<libfwbuilder::FWObject *> &res);
    
    void recursivelyRemoveObjFromTree(FWObject* obj, bool remove_ref=false);

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

    void merge( FWObjectDatabase *ndb , ConflictResolutionPredicate *mp=NULL);

    void setFileName (const std::string &filename);
    const std::string& getFileName ();
    const std::string  getFileDir ();

    static int generateUniqueId();
    static int registerStringId(const std::string &s_id);
    static int getIntId(const std::string &s_id);
    static std::string getStringId(int i_id);
       
};

}

#endif

