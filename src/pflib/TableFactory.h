/* 

                          Firewall Builder

                 Copyright (C) 2005 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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

#ifndef __TABLEFACTORY_HH
#define __TABLEFACTORY_HH

#include <fwbuilder/Firewall.h>
#include <fwbuilder/FWException.h>
#include <fwbuilder/RuleElement.h>

#include <fwcompiler/BaseCompiler.h>
#include <fwcompiler/GroupRegistry.h>

#include <list>
#include <map>
#include <set>


namespace libfwbuilder
{
    class FWObject;
    class FWObjectDatabase;
};    

namespace fwcompiler
{

    class TableFactory
    {
        BaseCompiler *compiler;
        libfwbuilder::Firewall *firewall;
        libfwbuilder::FWObjectDatabase *dbroot;
        libfwbuilder::FWObject *persistent_tables;

        GroupRegistry *group_registry;
        std::map<std::string,libfwbuilder::FWObject*> tables;
        std::map<std::string,std::string> tblnames;
        std::string ruleSetName;
        std::map<std::string, std::set<int> > table_deduplicator;
        
        std::string generateTblID(libfwbuilder::RuleElement *re);
        libfwbuilder::FWObject* createTableObject(const std::string &tblname,
                                                  const std::string &tblid);

public:
        TableFactory(BaseCompiler *comp, libfwbuilder::Firewall *firewall,
                     libfwbuilder::Library *persistent_objects,
                     GroupRegistry *group_registry);

        void init(libfwbuilder::FWObjectDatabase *_dbroot);
        void detach();

        void setRuleSetName(const std::string &rsn="") { ruleSetName=rsn; }

        void registerTable(const std::string& tblname,
                           const std::string& tblid,
                           libfwbuilder::FWObject *tbl);
        void createTablesForRE(libfwbuilder::RuleElement *re,
                               libfwbuilder::Rule        *rule);
        void addObjectToTable(libfwbuilder::FWObject *tblgrp,
                              libfwbuilder::FWObject *obj);
        std::string PrintTables();

    };
};


#endif
