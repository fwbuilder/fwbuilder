/* 

                          Firewall Builder

                 Copyright (C) 2010-2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef _OBJECT_GROUPS_SUPPORT_HH
#define _OBJECT_GROUPS_SUPPORT_HH

#include "config.h"

#include "BaseObjectGroup.h"
#include "NamedObject.h"

#include "fwbuilder/Group.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"

#include "fwcompiler/RuleProcessor.h"


namespace fwcompiler
{

    class NamedObjectManager
    {

public:
        std::map<int, NamedObject*> named_objects;
        std::string platform;
        std::string version;
// storage for object groups created to be used with PIX command object-group
        std::string object_groups_group_id;
        libfwbuilder::FWObjectDatabase *object_groups_tree;

        //const libfwbuilder::Firewall *fw;
        libfwbuilder::FWObjectDatabase *work_db;
        
        NamedObjectManager(const libfwbuilder::Firewall *_fw);
        virtual ~NamedObjectManager();
        void addNamedObject(const libfwbuilder::FWObject *obj);
        NamedObject* getNamedObject(const libfwbuilder::FWObject *obj);

        std::string getNamedObjectsDefinitions();

        bool haveNamedObjects();
        bool haveObjectGroups();

        BaseObjectGroup* createObjectGroup();
        libfwbuilder::Group* getObjectGroupsGroup();

        void setWorkingObjectTree(libfwbuilder::FWObjectDatabase *dbcopy);

        /*
         * saveObjectGroups() moves group that holds all newly created
         * object groups from the object database used by the compiler
         * (referenced by work_db) to object_groups_tree. Note that we
         * just simply re-parent group object which breaks all
         * references to it from rules in work_db. Call this from the
         * run() function only at the point where compiler's copy of
         * the object tree is not needed anymore. Good moment is right
         * after the call to epilog().
         *
         * Again, THIS METHOD BREAKS OBJECT TREE inside policy
         * compiler this instance of NamedObjectManager works with
         * (they get associated by the call to method setNamedObjectManager()
         * of the compiler)
         */
        void saveObjectGroups();

    };
    
    class CreateObjectGroups : public BasicRuleProcessor
    {
protected:

        std::string re_type;
        std::string name_suffix;
        NamedObjectManager *named_objects_manager;

        BaseObjectGroup* findObjectGroup(libfwbuilder::RuleElement *re);

        virtual void packObjects(libfwbuilder::RuleElement *re,
                                 BaseObjectGroup *obj_group);

public:

        CreateObjectGroups(const std::string &name,
                           const std::string &_ns,
                           const std::string &_type,
                           NamedObjectManager *m) :
            BasicRuleProcessor(name)
            {
                re_type=_type;
                name_suffix=_ns;
                named_objects_manager = m;
            }

        virtual ~CreateObjectGroups();
        virtual bool processNext();

    };

    class CreateObjectGroupsForSrc : public CreateObjectGroups
    {
public:
CreateObjectGroupsForSrc(const std::string &n, NamedObjectManager *m) : 
        CreateObjectGroups(n,"src",libfwbuilder::RuleElementSrc::TYPENAME, m) {}
    };

    class CreateObjectGroupsForDst : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForDst(const std::string &n, NamedObjectManager *m) : 
        CreateObjectGroups(n,"dst",libfwbuilder::RuleElementDst::TYPENAME, m) {}
    };

    class CreateObjectGroupsForSrv : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForSrv(const std::string &n, NamedObjectManager *m) : 
        CreateObjectGroups(n,"srv",libfwbuilder::RuleElementSrv::TYPENAME, m) {}
    };
    

    // ################################################################
    // OSrc, ODst, OSrv, TSrc
    
    class CreateObjectGroupsForOSrc : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForOSrc(const std::string &n, NamedObjectManager *m) : 
        CreateObjectGroups(n,"osrc",libfwbuilder::RuleElementOSrc::TYPENAME, m){}
    };

    class CreateObjectGroupsForODst : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForODst(const std::string &n, NamedObjectManager *m) : 
        CreateObjectGroups(n,"odst",libfwbuilder::RuleElementODst::TYPENAME, m){}
    };

    class CreateObjectGroupsForOSrv : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForOSrv(const std::string &n, NamedObjectManager *m) : 
        CreateObjectGroups(n,"osrv",libfwbuilder::RuleElementOSrv::TYPENAME, m){}
    };
    
    class CreateObjectGroupsForTSrc : public CreateObjectGroups
    {
protected:

        virtual void packObjects(libfwbuilder::RuleElement *re,
                                 BaseObjectGroup *obj_group);

public:
        CreateObjectGroupsForTSrc(const std::string &n, NamedObjectManager *m) : 
        CreateObjectGroups(n,"tsrc",libfwbuilder::RuleElementTSrc::TYPENAME, m){}
    };



    

    class createNamedObjectsCommon :  public BasicRuleProcessor
    {
protected:
        virtual void printObjectsForRE(libfwbuilder::FWObject *re);
        NamedObjectManager *named_objects_manager;
public:
        createNamedObjectsCommon(const std::string &n,
            NamedObjectManager *_m) : BasicRuleProcessor(n)
        {
            named_objects_manager = _m;
        }
    };

    class createNamedObjectsForPolicy :  public createNamedObjectsCommon
    {
protected:
        virtual void printObjectsForRE(libfwbuilder::FWObject *re);
public:
        createNamedObjectsForPolicy(const std::string &n,
            NamedObjectManager *m) : createNamedObjectsCommon(n, m) {}
        virtual bool processNext();
    };
    
    class createNamedObjectsForNAT :  public createNamedObjectsCommon
    {
public:
        createNamedObjectsForNAT(const std::string &n,
            NamedObjectManager *m) : createNamedObjectsCommon(n, m) {}
        virtual bool processNext();
    };
    
}

#endif
