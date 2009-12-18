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

namespace fwcompiler {

    class OSConfigurator;
    
    class CompilerDriver : public BaseCompiler
    {

protected:

        QStringList all_errors;

        QStringList args;
        int fwbdebug;
        std::string filename;
        std::string wdir;
        QDir start_current_dir;
        QString fwobjectname;
        QString current_firewall_name;
        // fw_file_name is the name of the output file. Can be enforced via -o command
        // line option or automatically determined using firewall object name
        QString fw_file_name;
        // member_file_names is the mapping between member firewall object ID and
        // corresponding output file name. Can be enfirced via -O command line option
        // or determined automatically using member firewall name. Used only when
        // compiling Cluster
        QMap<QString,QString> member_file_names;
        int dl;
        int drp;
        bool rule_debug_on;
        bool single_rule_compile_on;
        bool prepend_cluster_name_to_output_file;
        std::string single_rule_id;
        int drn;
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

        QString determineOutputFileName(libfwbuilder::Cluster *cluster,
                                        libfwbuilder::Firewall *current_fw,
                                        bool cluster_member,
                                        const QString &ext);
        bool isSupported(std::list<std::string> *protocols,
                         const std::string &cluster_group_type);

        virtual int checkCluster(libfwbuilder::Cluster* cluster);

        void mergeRuleSets(libfwbuilder::Cluster *cluster,
                           libfwbuilder::Firewall *fw, const std::string &type);
    
        // checks if address @addr belongs to the subnet defined by @subnet
        static bool isReachable(const libfwbuilder::Address* const subnet,
                                const libfwbuilder::InetAddr* const addr);

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
                                              const QString &comment_char);

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
        virtual std::string run(const std::string &cluster_id,
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

        void findImportedRuleSets(libfwbuilder::Firewall *fw,
                                  std::list<libfwbuilder::FWObject*> &all_policies);

        virtual bool prepare(const QStringList &args);
        virtual void compile();
        virtual QMap<QString,QString> compileSingleRule(const std::string &rule_id);
    };

};

QTextStream& operator<< (QTextStream &text_stream, const std::string &str);


#endif
