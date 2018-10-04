/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

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

#ifndef __COMPILER_DRIVER_PF_HH__
#define __COMPILER_DRIVER_PF_HH__

#include "CompilerDriver.h"
#include "OSConfigurator_bsd.h"
#include "TableFactory.h"
#include "OSData_pf.h"
#include "Configlet.h"

#include <string>
#include <sstream>
#include <memory>

#include <QMap>
#include <QTextStream>


namespace libfwbuilder {
    class FWObjectDatabase;
    class Cluster;
    class ClusterGroup;
    class Firewall;
    class RuleSet;
    class Interface;
};


class MapOstringStream : public std::map<QString, std::ostringstream*>
{
  public:
    MapOstringStream() {}
    ~MapOstringStream();
    void clear();
};

class MapTableFactory : public std::map<QString, fwcompiler::TableFactory*>
{
  public:
    MapTableFactory() {}
    ~MapTableFactory();
    void clear();
};



namespace fwcompiler
{

    class CompilerDriver_pf : public CompilerDriver
    {

        QStringList anchor_names;
        
//        QString conf_file_name;

// Note that in the following maps ruleset name will be 
// "__main__" for both main Policy and NAT rulesets.

// map ruleset_name -> conf file name
//        std::map<QString, QString> conf_files;

// map ruleset_name -> remote conf file name
//        std::map<QString, QString> remote_conf_files;

// map ruleset_name -> generated script
//    std::map<std::string, std::ostringstream*> generated_scripts;
        MapOstringStream generated_scripts;
    
// map ruleset_name -> TableFactory*
//    std::map<std::string, fwcompiler::TableFactory*> table_factories;
        MapTableFactory table_factories;

        void setToolPathVar(libfwbuilder::Firewall* fw,
                            const std::string &os,
                            const std::string &var_path_suffix,
                            OSData_pf::tools osdata_tool_type,
                            Configlet *configlet);

        QString composeActivationCommand(libfwbuilder::Firewall *fw,
                                         const std::string &pfctl_debug,
                                         const std::string &anchor_name,
                                         const std::string &pf_version,
                                         const std::string &remote_file_name);

protected:

        std::string routing_script;
        
        QString getConfFileNameForRuleset(const QString &ruleset_name,
                                          const QString &conf_file_name,
                                          const QString &ext="");
        
        QString getRemoteConfFileName(const QString &ruleset_name,
                                      const QString &local_file_name,
                                      const QString &remote_fw_file_name,
                                      const QString &remote_conf_file_name);
        
        std::string printTimeout(libfwbuilder::FWOptions* options,
                                 const std::string &OnOffOption,
                                 const std::string &ValOption,
                                 const std::string &pfCode);
        void printProlog(QTextStream &file, const std::string &prolog_code);
        void printStaticOptions(QTextStream &file, libfwbuilder::Firewall* fw);

        virtual QString printPathForAllTools(libfwbuilder::Firewall* fw,
                                             const std::string &os);

        virtual QString printActivationCommands(libfwbuilder::Firewall *fw);

        virtual QString assembleFwScript(libfwbuilder::Cluster *cluster,
                                         libfwbuilder::Firewall* fw,
                                         bool cluster_member,
                                         OSConfigurator *ocsnf);
        virtual QString assembleManifest(libfwbuilder::Cluster *cluster,
                                         libfwbuilder::Firewall* fw,
                                         bool cluster_member);

public:

        CompilerDriver_pf(libfwbuilder::FWObjectDatabase *db);

        // create a copy of itself, including objdb
        virtual CompilerDriver* clone();
    
        virtual QString run(const std::string &cluster_id,
                            const std::string &firewall_id,
                            const std::string &single_rule_id);

    };
};

#endif
