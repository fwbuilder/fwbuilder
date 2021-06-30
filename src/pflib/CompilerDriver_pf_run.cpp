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

#include "Configlet.h"
#include "CompilerDriver_pf.h"

#include "PolicyCompiler_pf.h"
#include "NATCompiler_pf.h"
#include "TableFactory.h"
#include "Preprocessor_pf.h"
#include "RoutingCompiler_openbsd.h"
#include "RoutingCompiler_freebsd.h"
#include "AutomaticRules_pf.h"

#include "OSConfigurator_openbsd.h"
#include "OSConfigurator_freebsd.h"
#include "OSConfigurator_solaris.h"

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
#include "fwbuilder/Routing.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/XMLTools.h"

#include "fwcompiler/Preprocessor.h"
#include "fwcompiler/GroupRegistry.h"
#include "fwcompiler/exceptions.h"

#include <QStringList>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

// #define DEBUG_FILE_NAMES 1


QString CompilerDriver_pf::composeActivationCommand(Firewall *fw,
                                                    const string &pfctl_debug,
                                                    const string &anchor_name,
                                                    const string &pf_version,
                                                    const string &remote_file_name)
{
    FWOptions* options = fw->getOptionsObject();
    Configlet act(fw, "pf",
                  options->getBool("generate_rc_conf_file") ?
                  "rc_conf_activation" : "activation");
    
    act.removeComments();
    act.setVariable("pfctl_debug", pfctl_debug.c_str());
    act.setVariable("anchor", !anchor_name.empty());
    act.setVariable("anchor_name", anchor_name.c_str());
    if (pf_version == "obsd_lt_3.2")
    {
        act.setVariable("pf_version_lt_3_2", 1);
        act.setVariable("pf_version_ge_3_2", 0);
    } else
    {
        act.setVariable("pf_version_lt_3_2", 0);
        act.setVariable("pf_version_ge_3_2", 1);
    }
    act.setVariable("remote_file", remote_file_name.c_str());
    return act.expand();
}

QString CompilerDriver_pf::printActivationCommands(Firewall *fw)
{
    FWOptions* options = fw->getOptionsObject();
    bool debug = options->getBool("debug");
    string pfctl_dbg = (debug)?"-v ":"";

    QString remote_file_name = escapeFileName(remote_file_names[CONF1_FILE]);

    return composeActivationCommand(
        fw, pfctl_dbg, "",
        fw->getStr("version"), remote_file_name.toUtf8().constData());

#if 0
    QStringList activation_commands;

    // skip first item in the list since it is .fw script
    for(int idx=1; idx<file_names.size(); idx++)
    {
        QString remote_file_name = escapeFileName(remote_file_names[idx]);

        activation_commands.push_back(
            composeActivationCommand(
                fw, pfctl_dbg, anchor_names[idx].toUtf8().constData(),
                fw->getStr("version"), remote_file_name.toUtf8().constData()));
    }

    return activation_commands.join("\n");
#endif
}

QString CompilerDriver_pf::assembleManifest(Cluster*, Firewall* , bool )
{
    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);

    for(int idx=0; idx<file_names.size(); idx++)
    {
        QString master_file_marker = (idx==0) ? "* " : "  ";
        QString local_file_name = file_names[idx];
        QString remote_file_name = remote_file_names[idx];
        script << manifestMarker() << master_file_marker
               << escapeFileName(local_file_name);
        if (!remote_file_name.isEmpty() && remote_file_name != local_file_name)
            script << " " << escapeFileName(remote_file_name);
        script << "\n";
    }

    return script_buffer;
}

QString CompilerDriver_pf::assembleFwScript(Cluster *cluster,
                                            Firewall* fw,
                                            bool cluster_member,
                                            OSConfigurator *oscnf)
{
    FWOptions* options = fw->getOptionsObject();

    Configlet script_skeleton(
        fw, "pf",
        options->getBool("generate_rc_conf_file") ?
            "rc_conf_skeleton" : "script_skeleton");

    Configlet top_comment(fw, "pf",
                          options->getBool("generate_rc_conf_file") ?
                          "rc_conf_top_comment" : "top_comment");

    script_skeleton.setVariable("routing_script", 
                                QString::fromUtf8(routing_script.c_str()));

    assembleFwScriptInternal(
        cluster, fw, cluster_member, oscnf,
        &script_skeleton, &top_comment, "#",
        !options->getBool("generate_rc_conf_file"));

    if (fw->getStr("platform") == "pf")
    {
        script_skeleton.setVariable(
            "pf_flush_states", options->getBool("pf_flush_states"));
        script_skeleton.setVariable(
            "pf_version_ge_4_x", // fw->getStr("version")=="4.x");
            XMLTools::version_compare(fw->getStr("version"), "4.0")>=0);

    } else
    {
        script_skeleton.setVariable("pf_flush_states", 0);
        script_skeleton.setVariable("pf_version_ge_4_x", 0);
    }

    return script_skeleton.expand();
}

QString CompilerDriver_pf::run(const std::string &cluster_id,
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
        current_firewall_name = QString::fromUtf8(fw->getName().c_str());

        string firewall_dir = options->getStr("firewall_dir");
        if (firewall_dir=="") firewall_dir="/etc/fw";

        string prolog_place = options->getStr("prolog_place");
        if (prolog_place.empty()) prolog_place = "fw_file";  // old default
        string pre_hook = fw->getOptionsObject()->getStr("prolog_script");

        bool debug = options->getBool("debug");
        string shell_dbg = (debug)?"set -x":"" ;
        string pfctl_dbg = (debug)?"-v ":"";

        
/*
 * Process firewall options, build OS network configuration script
 */
        std::unique_ptr<OSConfigurator_bsd> oscnf;
        string platform = fw->getStr("platform");
        string fw_version = fw->getStr("version");
        string host_os = fw->getStr("host_OS");
        string family = Resources::os_res[host_os]->
            Resources::getResourceStr("/FWBuilderResources/Target/family");

        if (host_os == "solaris")
            oscnf = std::unique_ptr<OSConfigurator_bsd>(new OSConfigurator_solaris(
                                                          objdb , fw, false));

        if (host_os == "openbsd")
            oscnf = std::unique_ptr<OSConfigurator_bsd>(new OSConfigurator_openbsd(
                                                          objdb , fw, false));
    
        if (host_os == "freebsd")
            oscnf = std::unique_ptr<OSConfigurator_bsd>(new OSConfigurator_freebsd(
                                                          objdb , fw, false));

        if (oscnf.get()==nullptr)
        {
            abort("Unrecognized host OS " + host_os + "  (family " + family + ")");
            return "";
        }

        oscnf->prolog();

        QString remote_fw_name = QString::fromUtf8(
            options->getStr("script_name_on_firewall").c_str());
        QString remote_conf_name = QString::fromUtf8(
            options->getStr("conf_file_name_on_firewall").c_str());

        list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);
        list<FWObject*> all_nat = fw->getByType(NAT::TYPENAME);

        findImportedRuleSets(fw, all_policies);
        findImportedRuleSets(fw, all_nat);

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
        // NB: these ids are not really used by compiler for PF

        assignUniqueRuleIds(all_policies);
        assignUniqueRuleIds(all_nat);

        list<FWObject*> all_rulesets;
        all_rulesets.insert(
            all_rulesets.begin(), all_policies.begin(), all_policies.end());
        all_rulesets.insert(
            all_rulesets.begin(), all_nat.begin(), all_nat.end());

        // establish mapping of rule sets to file names so it can be used
        // for "load anchor" commands
        
        QMap<QString, QString> rulesets_to_file_names;
        QMap<QString, QString> rulesets_to_remote_file_names;
        QMap<QString, int> rulesets_to_indexes;
        QStringList file_extensions;
        QStringList remote_file_options;

        anchor_names.clear();

        anchor_names << ""; // for fw_file
        anchor_names << ""; // for main .conf file (both policy and nat top rule sets)

        // Can not make extension .conf when generating rc.conf file
        // because the second file also has extension .conf and this
        // causes conflict if both names are generated using default
        // algorithm from the fw name
        //

        file_extensions << "fw";
        file_extensions << "conf";

        remote_file_options << "script_name_on_firewall";
        remote_file_options << "conf_file_name_on_firewall";

        rulesets_to_indexes["__main__"] = CONF1_FILE;

        int idx = CONF2_FILE;
        for (list<FWObject*>::iterator p=all_rulesets.begin();
             p!=all_rulesets.end(); ++p)
        {
            RuleSet *rs = RuleSet::cast(*p);
            QString ruleset_name = QString::fromUtf8(rs->getName().c_str());

            if (ruleset_name.endsWith("/*"))
            {
                QString err("The name of the %1 ruleset %2"
                            " ends with '/*', assuming it is externally"
                            " controlled and skipping it.");
                warning(fw, rs, nullptr,
                        err.arg(rs->getTypeName().c_str())
                        .arg(ruleset_name).toStdString());
                rs->setBool(".skip_ruleset", true);
                continue;
            }

            if (rs->isTop()) continue;

            // record index of this ruleset in file_names and remote_file_names
            if (rulesets_to_indexes.count(ruleset_name) == 0)
            {
                anchor_names << ruleset_name;
                file_extensions << "conf";
                remote_file_options << ""; // to make sure it has right number of items
                rulesets_to_indexes[ruleset_name] = idx;
                idx++;
            }
        }

#ifdef DEBUG_FILE_NAMES
        qDebug() << "anchor_names=" << anchor_names;
        qDebug() << "file_extensions=" << file_extensions;
        qDebug() << "remote_file_options=" << remote_file_options;
#endif

        // The order of file names in file_names and remote_file_names
        // is the same as the order of rule sets in all_rulesets
        determineOutputFileNames(cluster, fw, !cluster_id.empty(),
                                 anchor_names, file_extensions,
                                 remote_file_options);


        for (list<FWObject*>::iterator p=all_rulesets.begin();
             p!=all_rulesets.end(); ++p)
        {
            RuleSet *rs = RuleSet::cast(*p);
            if (rs->getBool(".skip_ruleset")) continue;
            QString ruleset_name = QString::fromUtf8(rs->getName().c_str());
            if (rs->isTop()) ruleset_name = "__main__";
            int idx = rulesets_to_indexes[ruleset_name];
            rulesets_to_file_names[ruleset_name] = file_names[idx];
            rulesets_to_remote_file_names[ruleset_name] = remote_file_names[idx];
        }

#ifdef DEBUG_FILE_NAMES
        qDebug() << "file_names=" << file_names;
        qDebug() << "remote_file_names=" << remote_file_names;
        qDebug() << "rulesets_to_file_names=" << rulesets_to_file_names;
        qDebug() << "rulesets_to_remote_file_names=" << rulesets_to_remote_file_names;
#endif

        GroupRegistry group_registry;

        int routing_rules_count = 0;

        vector<int> ipv4_6_runs;

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

        ostringstream* main_str = new ostringstream();
        list<NATCompiler_pf::redirectRuleInfo> redirect_rules_info;

        for (vector<int>::iterator i=ipv4_6_runs.begin();
             i!=ipv4_6_runs.end(); ++i)
        {
            int ruleset_address_family = *i;
            bool is_ipv6 = (ruleset_address_family == AF_INET6);
            Preprocessor_pf* prep = new Preprocessor_pf(objdb , fw, is_ipv6);
            prep->setSingleRuleCompileMode(single_rule_id);
            if (inTestMode()) prep->setTestMode();
            if (inEmbeddedMode()) prep->setEmbeddedMode();
            prep->compile();
            delete prep;
        }

        // ################################################################
        // First I process NAT rules, both ipv4 and ipv6, then process
        // ipv4 and ipv6 policy rules. See SF bug 3428992

        for (vector<int>::iterator i=ipv4_6_runs.begin();
             i!=ipv4_6_runs.end(); ++i)
        {
            int ruleset_address_family = *i;
            bool is_ipv6 = (ruleset_address_family == AF_INET6);

            for (list<FWObject*>::iterator p=all_nat.begin();
                 p!=all_nat.end(); ++p )
            {
                NAT *nat = NAT::cast(*p);

                if (!nat->matchingAddressFamily(ruleset_address_family)) continue;
                if (nat->getBool(".skip_ruleset")) continue;

                QString ruleset_name = QString::fromUtf8(nat->getName().c_str());
                if (nat->isTop()) ruleset_name = "__main__";

                if (table_factories.count(ruleset_name) == 0)
                {
                    table_factories[ruleset_name] =
                        new fwcompiler::TableFactory(this, fw, persistent_objects, &group_registry);
                }

                NATCompiler_pf n( objdb, fw, is_ipv6, oscnf.get(),
                                  table_factories[ruleset_name]
                );

                n.setSourceRuleSet( nat );
                n.setRuleSetName(nat->getName());
                n.setPersistentObjects(persistent_objects);
                n.setGroupRegistry(&group_registry);

                n.setSingleRuleCompileMode(single_rule_id);
                n.setDebugLevel( dl );
                if (rule_debug_on) n.setDebugRule(drn);
                n.setVerbose( verbose );
                if (inTestMode()) n.setTestMode();
                if (inEmbeddedMode()) n.setEmbeddedMode();

                int nat_rules_count = 0;
                if ( (nat_rules_count=n.prolog()) > 0 ) 
                {
                    n.compile();
                    n.epilog();
                }
                have_nat = (have_nat || (nat_rules_count > 0));

                if (nat->isTop())
                {
                    if (generated_scripts.count(ruleset_name) == 0)
                        generated_scripts[ruleset_name] = main_str;
                } else
                {
                    if (generated_scripts.count(ruleset_name) == 0)
                        generated_scripts[ruleset_name] = new ostringstream();
                }

                if (n.getCompiledScriptLength() > 0)
                {
                    if (n.haveErrorsAndWarnings())
                    {
                        // store errors and warnings so they will appear on top
                        // of .fw file in addition to the .conf file
                        if (!single_rule_compile_on)
                        {
                            *(generated_scripts[ruleset_name])
                                << "# NAT compiler errors and warnings:"
                                << endl;
                            *(generated_scripts[ruleset_name]) << n.getErrors("# ");
                        }
                    }
                    *(generated_scripts[ruleset_name]) << n.getCompiledScript();
                    *(generated_scripts[ruleset_name]) << endl;
                }

                all_errors.push_back(n.getErrors("").c_str());

                const list<NATCompiler_pf::redirectRuleInfo> lst = 
                    n.getRedirRulesInfo();
                redirect_rules_info.insert(redirect_rules_info.begin(),
                                           lst.begin(), lst.end());
            }
        }

        // ################################################################
        // Process policy rule sets

        for (vector<int>::iterator i=ipv4_6_runs.begin();
             i!=ipv4_6_runs.end(); ++i)
        {
            int ruleset_address_family = *i;
            bool is_ipv6 = (ruleset_address_family == AF_INET6);

            for (list<FWObject*>::iterator p=all_policies.begin();
                 p!=all_policies.end(); ++p )
            {
                Policy *policy = Policy::cast(*p);

                if (!policy->matchingAddressFamily(ruleset_address_family)) continue;
                if (policy->getBool(".skip_ruleset")) continue;

                QString ruleset_name = QString::fromUtf8(policy->getName().c_str());
                if (policy->isTop()) ruleset_name = "__main__";

                if (table_factories.count(ruleset_name) == 0)
                {
                    table_factories[ruleset_name] =
                        new fwcompiler::TableFactory(this, fw, persistent_objects, &group_registry);
                }

                PolicyCompiler_pf c( objdb, fw, is_ipv6, oscnf.get(),
                                     &redirect_rules_info,
                                     table_factories[ruleset_name]
                );

                c.setSourceRuleSet( policy );
                c.setRuleSetName(policy->getName());
                c.setPersistentObjects(persistent_objects);
                c.setGroupRegistry(&group_registry);

                c.setSingleRuleCompileMode(single_rule_id);
                c.setDebugLevel( dl );
                if (rule_debug_on) c.setDebugRule(drp);
                c.setVerbose( verbose );
                if (inTestMode()) c.setTestMode();
                if (inEmbeddedMode()) c.setEmbeddedMode();

                int pf_rules_count = 0;
                if ( (pf_rules_count=c.prolog()) > 0 ) 
                {
                    c.compile();
                    c.epilog();
                }
                have_filter = (have_filter || (pf_rules_count > 0));

                if (policy->isTop())
                {
                    if (generated_scripts.count("__main__") == 0)
                        generated_scripts["__main__"] = main_str;
                } else
                {
                    if (generated_scripts.count(ruleset_name) == 0)
                        generated_scripts[ruleset_name] = new ostringstream();
                }

                if (c.getCompiledScriptLength() > 0)
                {
                    if (c.haveErrorsAndWarnings())
                    {
                        if (!single_rule_compile_on)
                        {
                            *(generated_scripts[ruleset_name])
                                << "# Policy compiler errors and warnings:"
                                << endl;
                            *(generated_scripts[ruleset_name]) << c.getErrors("# ");
                        }
                    }
                    *(generated_scripts[ruleset_name]) << c.getCompiledScript();
                    *(generated_scripts[ruleset_name]) << endl;
                }

                all_errors.push_back(c.getErrors("").c_str());

            }

        }

        std::unique_ptr<RoutingCompiler> routing_compiler;

        if (host_os == "openbsd")
            routing_compiler = std::unique_ptr<RoutingCompiler>(
                new RoutingCompiler_openbsd(objdb, fw, false, oscnf.get()));

        if (host_os == "freebsd")
            routing_compiler = std::unique_ptr<RoutingCompiler>(
                new RoutingCompiler_freebsd(objdb, fw, false, oscnf.get()));

        if (routing_compiler.get() == nullptr)
        {
            abort("Unrecognized host OS " + host_os + "  (family " + family + ")");
            return "";
        }

        RuleSet *routing = RuleSet::cast(fw->getFirstByType(Routing::TYPENAME));
        if (routing)
        {
            routing_compiler->setSourceRuleSet(routing);
            routing_compiler->setRuleSetName(routing->getName());
            routing_compiler->setPersistentObjects(persistent_objects);

            routing_compiler->setSingleRuleCompileMode(single_rule_id);
            routing_compiler->setDebugLevel( dl );
            if (rule_debug_on) routing_compiler->setDebugRule(drp);
            routing_compiler->setVerbose( verbose );
            if (inTestMode()) routing_compiler->setTestMode();
            if (inEmbeddedMode()) routing_compiler->setEmbeddedMode();

            if ( (routing_rules_count=routing_compiler->prolog()) > 0 )
            {
                routing_compiler->compile();
                routing_compiler->epilog();
            }

            if (routing_compiler->haveErrorsAndWarnings())
                all_errors.push_back(routing_compiler->getErrors("").c_str());

            routing_script += routing_compiler->getCompiledScript();
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
            // in single rule compile mode just return the result
            QString buffer;
            QTextStream pf_str(&buffer);

            for (map<QString, ostringstream*>::iterator fi=generated_scripts.begin();
                 fi!=generated_scripts.end(); fi++)
            {
                QString ruleset_name = fi->first;
                ostringstream *strm = fi->second;
                pf_str << table_factories[ruleset_name]->PrintTables();
                pf_str << QString::fromUtf8(strm->str().c_str());
                pf_str << QString::fromUtf8(routing_script.c_str());
            }

            // clear() calls destructors of all elements in the container
            table_factories.clear();
            generated_scripts.clear();

            return formSingleRuleCompileOutput(buffer);
        }

        /* add commands to load anchors to the bottom of the main .conf file */
        QMap<QString, QString>::iterator it;
        for (it=rulesets_to_remote_file_names.begin();
             it!=rulesets_to_remote_file_names.end(); ++it)
        {
            QString ruleset_name = it.key();
            if (ruleset_name == "__main__") continue;
            QString remote_file_name = it.value();
            ostringstream *ostr = generated_scripts["__main__"];
            // note that ostr can be nullptr if the firewall we are
            // trying to compile has no top-level rule sets
            if (ostr == nullptr) continue;
            *ostr << QString("load anchor %1 from \"%2\"")
                .arg(ruleset_name).arg(remote_file_name).toUtf8().constData()
                                             << endl;
        }


        /*
         * now write generated scripts to files
         */

        idx = CONF1_FILE;
        for (map<QString, ostringstream*>::iterator fi=generated_scripts.begin();
             fi!=generated_scripts.end(); fi++)
        {
            QString ruleset_name = fi->first;
            QString file_name = rulesets_to_file_names[ruleset_name]; // file_names[idx];
            ostringstream *strm = fi->second;

            if (strm==nullptr) continue;

            if (ruleset_name.contains("/*")) continue;

            file_name = getAbsOutputFileName(file_name);

            info("Output file name: " + file_name.toStdString());
            QFile pf_file(file_name);
            if (pf_file.open(QIODevice::WriteOnly))
            {
                QTextStream pf_str(&pf_file);

                if (ruleset_name == "__main__")
                {
                    printStaticOptions(pf_str, fw);

                    // attach persistent_tables subtree inside TableFactory object
                    // to the object tree
                    table_factories[ruleset_name]->init(objdb);

                    pf_str << table_factories[ruleset_name]->PrintTables();

                    if (prolog_place == "pf_file_after_tables")
                        printProlog(pf_str, pre_hook);
                } else 
                {
                    pf_str << table_factories[ruleset_name]->PrintTables();
                }

                pf_str << QString::fromUtf8(strm->str().c_str());
                pf_file.close();
            } else
            {
                // clear() calls destructors of all elements in the container
                table_factories.clear();
                generated_scripts.clear();

                QString err("Failed to open file %1 for writing: %2; "
                            "Current dir: %3");
                abort(err.arg(pf_file.fileName())
                      .arg(pf_file.error())
                      .arg(QDir::current().path()).toStdString());
            }

            idx++;
        }
/*
 * assemble the script and then perhaps post-process it if needed
 */
        QString script_buffer = assembleFwScript(
            cluster, fw, !cluster_id.empty(), oscnf.get());

        // clear() calls destructors of all elements in the container
        table_factories.clear();
        generated_scripts.clear();


        file_names[FW_FILE] = getAbsOutputFileName(file_names[FW_FILE]);

        info("Output file name: " + file_names[FW_FILE].toStdString());
        QFile fw_file(file_names[FW_FILE]);
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

MapOstringStream::~MapOstringStream()
{
    clear();
}

void MapOstringStream::clear()
{
    std::map<QString, std::ostringstream*>::iterator it;
    for (it=begin(); it!=end(); ++it)
        delete it->second;
    std::map<QString, std::ostringstream*>::clear();
}

MapTableFactory::~MapTableFactory()
{
    clear();
}

void MapTableFactory::clear()
{
    std::map<QString, fwcompiler::TableFactory*>::iterator it;
    for (it=begin(); it!=end(); ++it)
        delete it->second;
    std::map<QString, fwcompiler::TableFactory*>::clear();
}

