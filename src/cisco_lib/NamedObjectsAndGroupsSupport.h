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

#include "fwcompiler/RuleProcessor.h"


namespace fwcompiler
{

    class NamedObjectManager
    {
public:
        static std::string addNamedObject(const libfwbuilder::FWObject *obj);
        static NamedObject* getNamedObject(const libfwbuilder::FWObject *obj);
    };
    
    class CreateObjectGroups : public BasicRuleProcessor
    {
        static void clearNamedObjectsRegistry();
        
protected:

        std::string re_type;
        std::string name_suffix;

        BaseObjectGroup* findObjectGroup(libfwbuilder::RuleElement *re);

        virtual void packObjects(libfwbuilder::RuleElement *re,
                                 BaseObjectGroup *obj_group);

public:
// storage for object groups created to be used with PIX command object-group
        static libfwbuilder::Group *object_groups;
        static std::map<int, NamedObject*> named_objects;


        CreateObjectGroups(const std::string &name,
                   const std::string &_ns,
                   const std::string &_type) :
            BasicRuleProcessor(name) {re_type=_type; name_suffix=_ns; }

        virtual ~CreateObjectGroups();
        virtual bool processNext();

        static void init(libfwbuilder::FWObjectDatabase *db);
        
    };

    class CreateObjectGroupsForSrc : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForSrc(const std::string &n) : 
          CreateObjectGroups(n,"src",libfwbuilder::RuleElementSrc::TYPENAME) {}
    };

    class CreateObjectGroupsForDst : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForDst(const std::string &n) : 
          CreateObjectGroups(n,"dst",libfwbuilder::RuleElementDst::TYPENAME) {}
    };

    class CreateObjectGroupsForSrv : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForSrv(const std::string &n) : 
          CreateObjectGroups(n,"srv",libfwbuilder::RuleElementSrv::TYPENAME) {}
    };
    

    // ################################################################
    // OSrc, ODst, OSrv, TSrc
    
    class CreateObjectGroupsForOSrc : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForOSrc(const std::string &n) : 
          CreateObjectGroups(n,"osrc",libfwbuilder::RuleElementOSrc::TYPENAME) {}
    };

    class CreateObjectGroupsForODst : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForODst(const std::string &n) : 
          CreateObjectGroups(n,"odst",libfwbuilder::RuleElementODst::TYPENAME) {}
    };

    class CreateObjectGroupsForOSrv : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForOSrv(const std::string &n) : 
          CreateObjectGroups(n,"osrv",libfwbuilder::RuleElementOSrv::TYPENAME) {}
    };
    
    class CreateObjectGroupsForTSrc : public CreateObjectGroups
    {
protected:

        virtual void packObjects(libfwbuilder::RuleElement *re,
                                 BaseObjectGroup *obj_group);

public:
        CreateObjectGroupsForTSrc(const std::string &n) : 
          CreateObjectGroups(n,"tsrc",libfwbuilder::RuleElementTSrc::TYPENAME) {}
    };



    
    /**
     * this processor accumulates all rules fed to it by previous
     * processors, then prints all object groups and feeds all
     * rules to the next processor. Usually this processor is in
     * chain right before PrintRules.
     *
     */
    class printObjectGroups : public BasicRuleProcessor
    {
public:
        printObjectGroups(const std::string &n) : BasicRuleProcessor(n) {}
        virtual bool processNext();
    };

    class printNamedObjects :  public BasicRuleProcessor
    {
        void printObjectsForRE(libfwbuilder::RuleElement *re);

public:
        printNamedObjects(const std::string &n) : BasicRuleProcessor(n) {}
        virtual bool processNext();
    };

    
    
}

#endif
