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

#ifndef __COMPILER_DRIVER_HH__
#define __COMPILER_DRIVER_HH__

#include "fwcompiler/BaseCompiler.h"

#include "Configlet.h"

#include <string>
#include <sstream>
#include <memory>

#include <QDir>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QMap>


namespace libfwbuilder {
    class FWObject;
    class FWObjectDatabase;
    class Cluster;
    class ClusterGroup;
    class FailoverClusterGroup;
    class Firewall;
    class RuleSet;
    class Interface;
    class Address;
    class InetAddr;
};

#define FW_FILE 0
#define CONF1_FILE 1
#define CONF2_FILE 2


namespace fwcompiler
{

    class OSConfigurator;
    
    class CompilerDriver : public BaseCompiler
    {

        QString getOutputFileNameInternal(libfwbuilder::Firewall *current_fw,
                                          const QString &from_cli,
                                          const QString &option_name,
                                          const QString &fw_name,
                                          const QString &ext);
        
protected:

        QStringList all_errors;

        QStringList args;
        int fwbdebug;
        std::string filename;
        std::string wdir;
        QDir start_current_dir;
        QString fwobjectname;
        QString current_firewall_name;

        // list of file names we should generate. Items in the list are
        // as follows: [0] - the name of the initialization script (normally
        // the .fw file); [1] - the name of the confguration file (for
        // pf, ipfilter this is the main .conf file); [2] - the name of the
        // next conf file, if any; and so on.
        //
        // function determineOutputFileNames() fills this list
        
        QStringList file_names;

        // file names on the firewall with full path. Items should correspond
        // to items in the list file_name
        //
        // function determineOutputFileNames() fills this list
        
        QStringList remote_file_names;

        // I store file name provided via -o command line option here
        QString file_name_setting_from_command_line;
        
        // member_file_names is the mapping between member firewall
        // object ID and corresponding output file name. Can be
        // enfirced via -O command line option or determined
        // automatically using member firewall name. Used only when
        // compiling Cluster
        QMap<QString,QString> member_file_names;

        int dl;
        int drp;
        int drn;
        int drr;
        bool rule_debug_on;
        bool single_rule_compile_on;
        bool prepend_cluster_name_to_output_file;
        std::string single_rule_id;
        int verbose;
        bool have_dynamic_interfaces;
        bool ipv4_run;
        bool ipv6_run;
        bool fw_by_id;
        bool prolog_done;
        bool epilog_done;
        bool have_filter;
        bool have_nat;
        
        std::map<std::string,libfwbuilder::RuleSet*> branches;

        libfwbuilder::FWObjectDatabase *objdb;
        libfwbuilder::Library *persistent_objects;
        libfwbuilder::Library *workspace;

        void determineOutputFileNames(libfwbuilder::Cluster *cluster,
                                      libfwbuilder::Firewall *current_fw,
                                      bool cluster_member,
                                      const QStringList &suffixes,
                                      const QStringList &extensions,
                                      const QStringList &remote_file_name_fw_options);

        bool isSupported(std::list<std::string> *protocols,
                         const std::string &cluster_group_type);

        virtual int checkCluster(libfwbuilder::Cluster* cluster);

        void mergeRuleSets(libfwbuilder::Cluster *cluster,
                           libfwbuilder::Firewall *fw, const std::string &type);
    
        static bool isReachable(const libfwbuilder::Address* const subnet,
                                const libfwbuilder::InetAddr* const addr);

        void clearReadOnly(libfwbuilder::Firewall *fw);
            
        /* Virtual methods used to compose generated script */
        virtual QString printPathForAllTools(libfwbuilder::Firewall* fw,
                                             const std::string &os);

        virtual QString printActivationCommands(libfwbuilder::Firewall *fw);

        virtual QString assembleManifest(libfwbuilder::Cluster *cluster,
                                         libfwbuilder::Firewall* fw,
                                         bool cluster_member);

        virtual void assembleFwScriptInternal(libfwbuilder::Cluster *cluster,
                                              libfwbuilder::Firewall* fw,
                                              bool cluster_member,
                                              OSConfigurator *ocsnf,
                                              Configlet *script_skeleton,
                                              Configlet *top_comment,
                                              const QString &comment_char,
                                              bool indent);

        void _findImportedRuleSetsRecursively(libfwbuilder::Firewall *fw,
                                              libfwbuilder::RuleSet *ruleset,
                                              std::map<libfwbuilder::FWObject*, int> &branch_rulesets);
        

        QString getAbsOutputFileName(const QString &output_file_name);
        
public:

        CompilerDriver(libfwbuilder::FWObjectDatabase *db);
        virtual ~CompilerDriver();

        // create a copy of itself, including objdb
        virtual CompilerDriver* clone();

        /**
         * Process command line arguments
         */
        virtual bool configure(const QStringList &args);

        /**
         * Assign target object by its id
         */
        void setTargetId(const std::string &id);

        /**
         * create right compiler objects and compile policy, nat and
         * routing rules for given firewall which can be a member of a
         * cluster. If firewall is standalone, @cluster_id is an empty
         * string. Cluster and firewall are defined by their string IDs.
         * In single compile mode rule ID is provided in @single_rule_id
         * and generated script is returned. For compilers that create
         * several files it is up to the actual cmopiler class to decide
         * what should be returned in the single rule compile mode. In
         * normal (not single rule) compile mode returned string is
         * undefined and should not be used.
         */
        virtual QString run(const std::string &cluster_id,
                            const std::string &firewall_id,
                            const std::string &single_rule_id);

        virtual void commonChecks(libfwbuilder::Firewall *fw);
        virtual void commonChecks2(libfwbuilder::Cluster *cluster,
                                   libfwbuilder::Firewall *fw);

        void copyFailoverInterface(libfwbuilder::Cluster *cluster,
                                   libfwbuilder::Firewall *fw,
                                   libfwbuilder::FailoverClusterGroup *cluster_group,
                                   libfwbuilder::Interface *iface);

        virtual void populateClusterElements(libfwbuilder::Cluster *cluster,
                                             libfwbuilder::Firewall *fw);

        virtual void processStateSyncGroups(libfwbuilder::Cluster*,
                                            libfwbuilder::Firewall*);

        std::string indent(int n_spaces, const std::string &txt);
        QString indent(int n_spaces, const QString &txt);
        QString prepend(const QString &prep, const QString &txt);
   
        /*
         * Verifies that state sync and failover group types configured in
         * the GUI are supported for the given host OS. List of supported
         * protocols is taken from the os resource file. If unsupported
         * protocol is found, calls compiler->abort to abort immediately.
         */
        virtual void validateClusterGroups(libfwbuilder::Cluster *cluster);

        /*
         * Use chdir to change working directory. In case of failure, exit(1)
         */
        void chDir();

        /*
         * Find firewall or cluster object we should process.
         */
        virtual libfwbuilder::Firewall* locateObject();

        void getFirewallAndClusterObjects(const std::string &cluster_id,
                                          const std::string &fw_id,
                                          libfwbuilder::Cluster **cl,
                                          libfwbuilder::Firewall **fw);
        
        void findImportedRuleSets(libfwbuilder::Firewall *fw,
                                  std::list<libfwbuilder::FWObject*> &all_policies);

        void assignUniqueRuleIds(std::list<libfwbuilder::FWObject*> &all_policies);
        
        virtual bool prepare(const QStringList &args);
        virtual void compile();
        virtual QMap<QString,QString> compileSingleRule(const std::string &rule_id);

        /*
         * if compilers produced empty string for the generated code,
         * this method checks if there were any errors and returns
         * them. If compilers generated output, errors should be
         * included in it because warning and error messages are
         * usually attached to rules. Errors are taken from the
         * all_errors member variable.
         */
        QString formSingleRuleCompileOutput(const QString &generated_code);
        
        static QString escapeFileName(QString fileName);
        static QString unescapeFileName(QString fileName);

        static QString getConfFileNameFromFwFileName(const QString &file_name,
                                                     const QString &ext);
        
	void setDebugRule(int dr)  { drp = drn = dr; rule_debug_on = true; }

    };

};

QTextStream& operator<< (QTextStream &text_stream, const std::string &str);


#endif
