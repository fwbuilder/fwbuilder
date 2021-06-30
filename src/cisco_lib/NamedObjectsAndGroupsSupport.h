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


#include "BaseObjectGroup.h"
#include "NamedObject.h"

#include "fwbuilder/Group.h"
#include "fwbuilder/RuleElement.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"

#include "fwcompiler/RuleProcessor.h"


namespace fwcompiler
{

    class CreateObjectGroups : public BasicRuleProcessor
    {
protected:

        std::string re_type;
        std::string name_suffix;
        NamedObjectsManager *named_objects_manager;

        BaseObjectGroup* findObjectGroup(libfwbuilder::RuleElement *re);

        virtual void packObjects(libfwbuilder::RuleElement *re,
                                 BaseObjectGroup *obj_group);

public:

        CreateObjectGroups(const std::string &name,
                           const std::string &_ns,
                           const std::string &_type,
                           NamedObjectsManager *m) :
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
CreateObjectGroupsForSrc(const std::string &n, NamedObjectsManager *m) : 
        CreateObjectGroups(n,"src",libfwbuilder::RuleElementSrc::TYPENAME, m) {}
    };

    class CreateObjectGroupsForDst : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForDst(const std::string &n, NamedObjectsManager *m) : 
        CreateObjectGroups(n,"dst",libfwbuilder::RuleElementDst::TYPENAME, m) {}
    };

    class CreateObjectGroupsForSrv : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForSrv(const std::string &n, NamedObjectsManager *m) : 
        CreateObjectGroups(n,"srv",libfwbuilder::RuleElementSrv::TYPENAME, m) {}
    };
    

    // ################################################################
    // OSrc, ODst, OSrv, TSrc
    
    class CreateObjectGroupsForOSrc : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForOSrc(const std::string &n, NamedObjectsManager *m) : 
        CreateObjectGroups(n,"osrc",libfwbuilder::RuleElementOSrc::TYPENAME, m){}
    };

    class CreateObjectGroupsForODst : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForODst(const std::string &n, NamedObjectsManager *m) : 
        CreateObjectGroups(n,"odst",libfwbuilder::RuleElementODst::TYPENAME, m){}
    };

    class CreateObjectGroupsForOSrv : public CreateObjectGroups
    {
public:
        CreateObjectGroupsForOSrv(const std::string &n, NamedObjectsManager *m) : 
        CreateObjectGroups(n,"osrv",libfwbuilder::RuleElementOSrv::TYPENAME, m){}
    };
    
    class CreateObjectGroupsForTSrc : public CreateObjectGroups
    {
protected:

        virtual void packObjects(libfwbuilder::RuleElement *re,
                                 BaseObjectGroup *obj_group);

public:
        CreateObjectGroupsForTSrc(const std::string &n, NamedObjectsManager *m) : 
        CreateObjectGroups(n,"tsrc",libfwbuilder::RuleElementTSrc::TYPENAME, m){}
    };



    

    class createNamedObjectsCommon :  public BasicRuleProcessor
    {
protected:
        virtual void printObjectsForRE(libfwbuilder::FWObject *re);
        NamedObjectsManager *named_objects_manager;
public:
        createNamedObjectsCommon(const std::string &n,
            NamedObjectsManager *_m) : BasicRuleProcessor(n)
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
            NamedObjectsManager *m) : createNamedObjectsCommon(n, m) {}
        virtual bool processNext();
    };
    
    class createNamedObjectsForNAT :  public createNamedObjectsCommon
    {
public:
        createNamedObjectsForNAT(const std::string &n,
            NamedObjectsManager *m) : createNamedObjectsCommon(n, m) {}
        virtual bool processNext();
    };
    
}

#endif
