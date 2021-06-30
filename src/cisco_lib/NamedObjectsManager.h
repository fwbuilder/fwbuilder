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


#include "BaseObjectGroup.h"

namespace libfwbuilder
{
    class Group;
    class Firewall;
    class Library;
};

namespace fwcompiler
{

    class NamedObjectsManager
    {
protected:
        libfwbuilder::Firewall *fw;
        std::string platform;
        std::string version;
        // storage for object groups created to be used with PIX
        // command object-group
        std::string object_groups_group_id;

        libfwbuilder::Library *persistent_objects;
        
public:

        std::map<int, NamedObject*> named_objects;
        
        NamedObjectsManager(libfwbuilder::Library *persistent_objects,
                            libfwbuilder::Firewall *_fw);
        virtual ~NamedObjectsManager();
        void addNamedObject(const libfwbuilder::FWObject *obj);
        NamedObject* getNamedObject(const libfwbuilder::FWObject *obj);

        virtual std::string getNamedObjectsDefinitions();
        virtual std::string getClearCommands();
        
        bool haveNamedObjects();
        bool haveObjectGroups();

        BaseObjectGroup* createObjectGroup();
        libfwbuilder::Group* getObjectGroupsGroup();

    };
    
}

#endif
