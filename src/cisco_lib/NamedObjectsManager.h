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

#ifndef _NAMED_OBJECTS_MANAGER_HH
#define _NAMED_OBJECTS_MANAGER_HH

#include "config.h"

#include "BaseObjectGroup.h"

#include "fwbuilder/Group.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"


namespace fwcompiler
{

    class NamedObjectsManager
    {
protected:
        std::string platform;
        std::string version;
        // storage for object groups created to be used with PIX
        // command object-group
        std::string object_groups_group_id;

        /*
         * This is a storage object tree. Method saveObjectGroups()
         * copies object groups objects created during compiler pass
         * in the working tree work_db to this tree. There should be
         * no access to the storage tree from outside, it should only
         * be used by methods of this class that generate commands for
         * object groups definitions or "clear" commands.
         */
        libfwbuilder::FWObjectDatabase *object_groups_tree;

        /*
         * This is a working object tree. When compilers need to
         * interact with named object manager, they should use this
         * object tree. Access to the group that holds created object
         * groups is provided by method
         * getObjectGroupsGroupInWorkTree() that finds it in the
         * working tree
         */
        libfwbuilder::FWObjectDatabase *work_db;
        
public:
        std::map<int, NamedObject*> named_objects;

        
        NamedObjectsManager(const libfwbuilder::Firewall *_fw);
        virtual ~NamedObjectsManager();
        void addNamedObject(const libfwbuilder::FWObject *obj);
        NamedObject* getNamedObject(const libfwbuilder::FWObject *obj);

        virtual std::string getNamedObjectsDefinitions();
        virtual std::string getClearCommands();
        
        bool haveNamedObjects();
        bool haveObjectGroups();

        BaseObjectGroup* createObjectGroup();
        libfwbuilder::Group* getObjectGroupsGroupInWorkTree();

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
         * compiler this instance of NamedObjectsManager works with
         * (they get associated by the call to method setNamedObjectsManager()
         * of the compiler)
         */
        void saveObjectGroups();

    };
    
}

#endif
