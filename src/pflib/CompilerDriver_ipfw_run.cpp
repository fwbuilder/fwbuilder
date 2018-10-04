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


#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <memory>

#include <assert.h>
#include <cstring>
#include <iomanip>

#include "CompilerDriver_ipfw.h"
#include "PolicyCompiler_ipfw.h"
#include "AutomaticRules_pf.h"

#include "OSConfigurator_freebsd.h"
#include "OSConfigurator_macosx.h"

#include "fwbuilder/Cluster.h"
#include "fwbuilder/ClusterGroup.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/XMLTools.h"

#include "fwcompiler/Preprocessor.h"

#include <QStringList>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


QString CompilerDriver_ipfw::assembleManifest(Cluster*, Firewall* , bool )
{
    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);
    script << manifestMarker()
           << "* "
           << this->escapeFileName(file_names[FW_FILE]);

    if (!remote_file_names[FW_FILE].isEmpty())
        script << " " << this->escapeFileName(remote_file_names[FW_FILE]);
    script << "\n";
    script << "#" << endl;
    script << "#" << endl;
    return script_buffer;
}

QString CompilerDriver_ipfw::printActivationCommands(Firewall*)
{
    return activation_commands.join("\n");
}

QString CompilerDriver_ipfw::assembleFwScript(Cluster *cluster,
                                              Firewall* fw,
                                              bool cluster_member,
                                              OSConfigurator *oscnf)
{
    Configlet script_skeleton(fw, "ipfw", "script_skeleton");
    Configlet top_comment(fw, "ipfw", "top_comment");

    assembleFwScriptInternal(
        cluster, fw, cluster_member, oscnf,
        &script_skeleton, &top_comment, "#", true);
    return script_skeleton.expand();
}

QString CompilerDriver_ipfw::run(const std::string &cluster_id,
                                 const std::string &firewall_id,
                                 const std::string &single_rule_id)
{
    Cluster *cluster = nullptr;
    Firewall *fw = nullptr;

    getFirewallAndClusterObjects(cluster_id, firewall_id, &cluster, &fw);

    try
    {
        clearReadOnly(fw);

        // Copy rules from the cluster object
        populateClusterElements(cluster, fw);

        commonChecks2(cluster, fw);

        FWOptions* options = fw->getOptionsObject();

        // Note that fwobjectname may be different from the name of the
        // firewall fw This happens when we compile a member of a cluster
        current_firewall_name = fw->getName().c_str();

        string s;

        string firewall_dir=options->getStr("firewall_dir");
        if (firewall_dir=="") firewall_dir="/etc/fw";

        bool debug=options->getBool("debug");
        string shell_dbg=(debug)?"-x":"" ;

/*
 * Process firewall options, build OS network configuration script
 */
        std::unique_ptr<OSConfigurator_bsd> oscnf;
        string host_os = fw->getStr("host_OS");
        string family = Resources::os_res[host_os]->Resources::getResourceStr("/FWBuilderResources/Target/family");
        if ( host_os == "macosx")
            oscnf = std::unique_ptr<OSConfigurator_bsd>(new OSConfigurator_macosx(objdb , fw, false));

        if ( host_os == "freebsd")
            oscnf = std::unique_ptr<OSConfigurator_bsd>(new OSConfigurator_freebsd(objdb , fw, false));

        if (oscnf.get()==nullptr)
        {
            abort("Unrecognized host OS " + host_os + "  (family " + family + ")");
            return "";
        }

        oscnf->prolog();


        list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);
        vector<int> ipv4_6_runs;
        string generated_script;
        int policy_rules_count  = 0;
        int ipfw_rule_number = 0;

        findImportedRuleSets(fw, all_policies);

        try
        {
            AutomaticRules_pf auto_rules(fw, persistent_objects);
            auto_rules.addSshAccessRule();
            auto_rules.addCarpRules();
            auto_rules.addPfsyncRules();
            auto_rules.addFallbackRule();
        } catch (FWException &ex)
        {
            abort(ex.toString());
        }

        // assign unique rule ids that later will be used to generate
        // chain names.  This should be done after calls to
        // findImportedRuleSets()
        // NB: these ids are not used by this compiler

        assignUniqueRuleIds(all_policies);

        // command line options -4 and -6 control address family for which
        // script will be generated. If "-4" is used, only ipv4 part will 
        // be generated. If "-6" is used, only ipv6 part will be generated.
        // If neither is used, both parts will be done.

        if (options->getStr("ipv4_6_order").empty() ||
            options->getStr("ipv4_6_order") == "ipv4_first")
        {
            if (ipv4_run) ipv4_6_runs.push_back(AF_INET);
            if (ipv6_run) ipv4_6_runs.push_back(AF_INET6);
        }

        if (options->getStr("ipv4_6_order") == "ipv6_first")
        {
            if (ipv6_run) ipv4_6_runs.push_back(AF_INET6);
            if (ipv4_run) ipv4_6_runs.push_back(AF_INET);
        }

        for (vector<int>::iterator i=ipv4_6_runs.begin(); 
             i!=ipv4_6_runs.end(); ++i)
        {
            int policy_af = *i;
            bool ipv6_policy = (policy_af == AF_INET6);

            /*
              We need to create and run preprocessor for this address
              family before nat and policy compilers, but if there are
              no nat / policy rules for this address family, we do not
              need preprocessor either.
            */

            // Count rules for each address family
            int policy_count = 0;

            for (list<FWObject*>::iterator p=all_policies.begin();
                 p!=all_policies.end(); ++p)
            {
                Policy *policy = Policy::cast(*p);
                if (policy->matchingAddressFamily(policy_af)) policy_count++;
            }

            if (policy_count)
            {
                std::unique_ptr<Preprocessor> prep(new Preprocessor(objdb , fw, ipv6_policy));
                if (inTestMode()) prep->setTestMode();
                if (inEmbeddedMode()) prep->setEmbeddedMode();
                prep->compile();
            }

            ostringstream c_str;
            bool empty_output = true;
 
            for (list<FWObject*>::iterator p=all_policies.begin();
                 p!=all_policies.end(); ++p )
            {
                Policy *policy = Policy::cast(*p);
                string branch_name = policy->getName();

                if (!policy->matchingAddressFamily(policy_af)) continue;

                PolicyCompiler_ipfw c(objdb, fw, ipv6_policy, oscnf.get());
                c.setIPFWNumber(ipfw_rule_number);
                c.setSourceRuleSet( policy );
                c.setRuleSetName(branch_name);
                c.setPersistentObjects(persistent_objects);

                c.setSingleRuleCompileMode(single_rule_id);
                c.setDebugLevel( dl );
                if (rule_debug_on) c.setDebugRule(  drp );
                c.setVerbose( (bool)(verbose) );
                if (inTestMode()) c.setTestMode();
                if (inEmbeddedMode()) c.setEmbeddedMode();

                if ( (policy_rules_count=c.prolog()) > 0 )
                {
                    c.compile();
                    c.epilog();

                    ipfw_rule_number = c.getIPFWNumber();

                    if (c.getCompiledScriptLength() > 0)
                    {
                        if (!single_rule_compile_on)
                            c_str << "# ================ Rule set "
                                  << branch_name << endl;
                        c_str << c.getCompiledScript();
                        c_str << endl;
                        empty_output = false;
                    }
                    if (c.haveErrorsAndWarnings())
                    {
                        all_errors.push_back(c.getErrors("").c_str());
                    }
                }
            }

            if (!empty_output && !single_rule_compile_on)
            {
                if (ipv6_policy)
                {
                    generated_script += "\n\n";
                    generated_script += "# ================ IPv6\n";
                    generated_script += "\n\n";
                } else
                {
                    generated_script += "\n\n";
                    generated_script += "# ================ IPv4\n";
                    generated_script += "\n\n";
                }
            }

            generated_script += c_str.str();
        }

        /*
         * compilers detach persistent objects when they finish, this
         * means at this point library persistent_objects is not part
         * of any object tree.
         */
        objdb->reparent(persistent_objects);

        if (haveErrorsAndWarnings())
        {
            all_errors.push_front(getErrors("").c_str());
        }

        if (single_rule_compile_on)
        {
            return formSingleRuleCompileOutput(
                QString::fromUtf8(generated_script.c_str()));
        }

        PolicyCompiler_ipfw c(objdb, fw, false, oscnf.get());
        activation_commands.push_back(c.defaultRules().c_str());
        activation_commands.push_back(
            QString::fromUtf8(generated_script.c_str()));

/*
 * assemble the script and then perhaps post-process it if needed
 */
        determineOutputFileNames(cluster, fw, !cluster_id.empty(),
                                 QStringList(""), QStringList("fw"),
                                 QStringList("script_name_on_firewall"));

        QString script_buffer = assembleFwScript(
            cluster, fw, !cluster_id.empty(), oscnf.get());


        QString output_file = getAbsOutputFileName(file_names[FW_FILE]);

        info("Output file name: " + output_file.toStdString());
        QFile fw_file(output_file);
        if (fw_file.open(QIODevice::WriteOnly))
        {
            QTextStream fw_str(&fw_file);
            fw_str << script_buffer;
            fw_file.close();
            fw_file.setPermissions(QFile::ReadOwner | QFile::WriteOwner |
                                   QFile::ReadGroup | QFile::ReadOther |
                                   QFile::ExeOwner | 
                                   QFile::ExeGroup |
                                   QFile::ExeOther );

            info(" Compiled successfully");
        } else
        {
            QString err(" Failed to open file %1 for writing: %2; Current dir: %3");
            abort(err.arg(fw_file.fileName())
                  .arg(fw_file.error()).arg(QDir::current().path()).toStdString());
        }

        if (!all_errors.isEmpty())
            status = BaseCompiler::FWCOMPILER_WARNING;

    }
    catch (FWException &ex)
    {
        status = BaseCompiler::FWCOMPILER_ERROR;
        return QString::fromUtf8(ex.toString().c_str());
    }

    return "";
}

