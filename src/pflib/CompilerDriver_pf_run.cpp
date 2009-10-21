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

#include "../../config.h"
#include "../../build_num"

#ifndef _WIN32
#  include <unistd.h>
#  include <pwd.h>
#else
#  include <direct.h>
#  include <stdlib.h>
#  include <io.h>
#endif

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

#include "OSConfigurator_openbsd.h"
#include "OSConfigurator_freebsd.h"
#include "OSConfigurator_solaris.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"

#include "fwcompiler/Preprocessor.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/ClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"

#include <QStringList>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


QString CompilerDriver_pf::composeActivationCommand(Firewall *fw,
                                                    const string &pfctl_debug,
                                                    const string &anchor_name,
                                                    const string &pf_version,
                                                    const string &remote_file_name)
{
    Configlet act(fw, "bsd", "pf_activation");
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

    QStringList activation_commands;
    string remote_file = remote_conf_files["__main__"];
    if (remote_file.empty()) remote_file = conf_files["__main__"];
    if (remote_file[0] != '/') remote_file = "${FWDIR}/" + remote_file;

    activation_commands.push_back(
        composeActivationCommand(
            fw, pfctl_dbg, "", fw->getStr("version"), remote_file));

    for (map<string,string>::iterator i=conf_files.begin();
         i!=conf_files.end(); ++i)
    {
        string remote_file = remote_conf_files[i->first];
        if (remote_file.empty()) remote_file = i->second;
        if (remote_file[0] != '/') remote_file = "${FWDIR}/" + remote_file;

        if (i->first != "__main__")
            activation_commands.push_back(
                composeActivationCommand(
                    fw, pfctl_dbg, i->first, fw->getStr("version"), remote_file));
    }
    return activation_commands.join("\n");
}

QString CompilerDriver_pf::assembleManifest(Firewall* fw, bool )
{
    QFileInfo fw_file_info(fw_file_name);
    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);

    script << MANIFEST_MARKER << "* " << fw_file_info.fileName();
    string remote_name = fw->getOptionsObject()->getStr("script_name_on_firewall");
    if (!remote_name.empty()) script << " " << remote_name;
    script << "\n";

    for (map<string,string>::iterator i=conf_files.begin();
         i!=conf_files.end(); ++i)
    {
        string ruleset_name = i->first;
        QString file_name = QFileInfo(i->second.c_str()).fileName();
        QString remote_file_name = remote_conf_files[ruleset_name].c_str();
        script << MANIFEST_MARKER << "  " << file_name;
        if (!remote_file_name.isEmpty() && remote_file_name != file_name)
            script << " " << remote_file_name;
        script << "\n";
    }
    return script_buffer;
}

QString CompilerDriver_pf::assembleFwScript(Firewall* fw, bool cluster_member, OSConfigurator *oscnf)
{
    FWOptions* options = fw->getOptionsObject();
    Configlet script_skeleton(fw, "bsd", "pf_script_skeleton");
    Configlet top_comment(fw, "bsd", "top_comment");

    assembleFwScriptInternal(
        fw, cluster_member, oscnf, &script_skeleton, &top_comment, "#");

    if (fw->getStr("platform") == "pf")
    {
        script_skeleton.setVariable("pf_flush_states", options->getBool("pf_flush_states"));
        script_skeleton.setVariable("pf_version_ge_4_x", // fw->getStr("version")=="4.x");
                                    XMLTools::version_compare(fw->getStr("version"), "4.0")>=0);

    } else
    {
        script_skeleton.setVariable("pf_flush_states", 0);
        script_skeleton.setVariable("pf_version_ge_4_x", 0);
    }

    return script_skeleton.expand();
}

string CompilerDriver_pf::run(const std::string &cluster_id,
                              const std::string &firewall_id,
                              const std::string &single_rule_id)
{
    Cluster *cluster = NULL;
    if (!cluster_id.empty())
        cluster = Cluster::cast(
            objdb->findInIndex(objdb->getIntId(cluster_id)));

    Firewall *fw = Firewall::cast(
        objdb->findInIndex(objdb->getIntId(firewall_id)));
    assert(fw);

    // Copy rules from the cluster object
    populateClusterElements(cluster, fw);

    commonChecks2(cluster, fw);

    FWOptions* options = fw->getOptionsObject();

    // Note that fwobjectname may be different from the name of the
    // firewall fw This happens when we compile a member of a cluster
    current_firewall_name = fw->getName().c_str();

    fw_file_name = determineOutputFileName(fw, !cluster_id.empty(), ".fw");

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
    std::auto_ptr<OSConfigurator_bsd> oscnf;
    string platform = fw->getStr("platform");
    string fw_version = fw->getStr("version");
    string host_os = fw->getStr("host_OS");
    string family = Resources::os_res[host_os
      ]->Resources::getResourceStr("/FWBuilderResources/Target/family");

    if (host_os == "solaris")
        oscnf = std::auto_ptr<OSConfigurator_bsd>(new OSConfigurator_solaris(
                                                      objdb , fw, false));

    if (host_os == "openbsd")
        oscnf = std::auto_ptr<OSConfigurator_bsd>(new OSConfigurator_openbsd(
                                                      objdb , fw, false));
    
    if (host_os == "freebsd")
        oscnf = std::auto_ptr<OSConfigurator_bsd>(new OSConfigurator_freebsd(
                                                      objdb , fw, false));

    if (oscnf.get()==NULL)
        throw FWException("Unrecognized host OS " + 
                          host_os + "  (family " + family + ")");

    oscnf->prolog();

    string remote_fw_name = options->getStr("script_name_on_firewall");
    string remote_conf_name = options->getStr("conf_file_name_on_firewall");

    list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);
    list<FWObject*> all_nat = fw->getByType(NAT::TYPENAME);

    findImportedRuleSets(fw, all_policies);

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

    for (vector<int>::iterator i=ipv4_6_runs.begin();
         i!=ipv4_6_runs.end(); ++i)
    {
        int policy_af = *i;
        bool ipv6_policy = (policy_af == AF_INET6);

        // Count rules for each address family
        int nat_count = 0;
        int policy_count = 0;

        for (list<FWObject*>::iterator p=all_nat.begin();
             p!=all_nat.end(); ++p)
        {
            NAT *nat = NAT::cast(*p);
            if (nat->matchingAddressFamily(policy_af)) nat_count++;
        }

        for (list<FWObject*>::iterator p=all_policies.begin();
             p!=all_policies.end(); ++p)
        {
            Policy *policy = Policy::cast(*p);
            if (policy->matchingAddressFamily(policy_af)) policy_count++;
        }

        if (nat_count || policy_count)
        {
            Preprocessor_pf* prep = new Preprocessor_pf(
                objdb , fw, ipv6_policy);
            if (inTestMode()) prep->setTestMode();
            if (inEmbeddedMode()) prep->setEmbeddedMode();
            prep->compile();
            delete prep;
        }

        list<NATCompiler_pf::redirectRuleInfo> redirect_rules_info;

        for (list<FWObject*>::iterator p=all_nat.begin();
             p!=all_nat.end(); ++p )
        {
            NAT *nat = NAT::cast(*p);

            if (!nat->matchingAddressFamily(policy_af)) continue;

            string ruleset_name = nat->getName();

            if (ruleset_name.find("/*")!=string::npos)
            {
                QString err("The name of the policy ruleset %1"
                            " ends with '/*', assuming it is externally"
                            " controlled and skipping it.");
                warning(fw, nat, NULL,
                        err.arg(ruleset_name.c_str()).toStdString());
                continue;
            }

            if (nat->isTop())
                ruleset_name = "__main__";

            if (table_factories.count(ruleset_name) == 0)
            {
                table_factories[ruleset_name] = new fwcompiler::TableFactory();
            }

            NATCompiler_pf n( objdb, fw, ipv6_policy, oscnf.get(),
                              table_factories[ruleset_name] );

            n.setSourceRuleSet( nat );
            n.setRuleSetName(nat->getName());

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
                generated_scripts[ruleset_name] = main_str;
            } else
            {
                generated_scripts[ruleset_name] = new ostringstream();
            }

            if (n.getCompiledScriptLength() > 0)
            {
                if (n.haveErrorsAndWarnings())
                {
                    // store errors and warnings so they will appear on top
                    // of .fw file in addition to the .conf file
                    all_errors.push_back(n.getErrors("").c_str());
                    *(generated_scripts[ruleset_name])
                        << "# NAT compiler errors and warnings:"
                        << endl;
                    *(generated_scripts[ruleset_name]) << n.getErrors("# ");
                }
                *(generated_scripts[ruleset_name]) << n.getCompiledScript();
                *(generated_scripts[ruleset_name]) << endl;
            }

            conf_files[ruleset_name] = getConfFileName(
                ruleset_name,
                current_firewall_name.toUtf8().constData(),
                fw_file_name.toUtf8().constData());

            remote_conf_files[ruleset_name] = getRemoteConfFileName(
                ruleset_name,
                conf_files[ruleset_name],
                remote_fw_name,
                remote_conf_name);

            const list<NATCompiler_pf::redirectRuleInfo> lst = 
                n.getRedirRulesInfo();
            redirect_rules_info.insert(redirect_rules_info.begin(),
                                       lst.begin(), lst.end());
        }

        for (list<FWObject*>::iterator p=all_policies.begin();
             p!=all_policies.end(); ++p )
        {
            Policy *policy = Policy::cast(*p);
            string ruleset_name = policy->getName();

            if (ruleset_name.find("/*")!=string::npos)
            {
                QString err("The name of the policy ruleset %1"
                            " ends with '/*', assuming it is externally"
                            " controlled and skipping it.");
                warning(fw, policy, NULL,
                        err.arg(ruleset_name.c_str()).toStdString());
                continue;
            }

            if (!policy->matchingAddressFamily(policy_af)) continue;

            if (policy->isTop())
                ruleset_name = "__main__";

            if (table_factories.count(ruleset_name) == 0)
            {
                table_factories[ruleset_name] = new fwcompiler::TableFactory();
            }

            PolicyCompiler_pf c( objdb, fw, ipv6_policy, oscnf.get(),
                                 &redirect_rules_info,
                                 table_factories[ruleset_name] );

            c.setSourceRuleSet( policy );
            c.setRuleSetName(policy->getName());

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
                generated_scripts[ruleset_name] = main_str;
            } else
            {
                generated_scripts[ruleset_name] = new ostringstream();
            }

            if (c.getCompiledScriptLength() > 0)
            {
                if (c.haveErrorsAndWarnings())
                {
                    all_errors.push_back(c.getErrors("").c_str());
                    *(generated_scripts[ruleset_name])
                        << "# Policy compiler errors and warnings:"
                        << endl;
                    *(generated_scripts[ruleset_name]) << c.getErrors("# ");
                }
                *(generated_scripts[ruleset_name]) << c.getCompiledScript();
                *(generated_scripts[ruleset_name]) << endl;
            }

            conf_files[ruleset_name] = getConfFileName(
                ruleset_name,
                current_firewall_name.toUtf8().constData(),
                fw_file_name.toUtf8().constData());

            remote_conf_files[ruleset_name] = getRemoteConfFileName(
                ruleset_name,
                conf_files[ruleset_name],
                remote_fw_name,
                remote_conf_name);

        }
    }

    if (haveErrorsAndWarnings())
    {
        all_errors.push_front(getErrors("").c_str());
    }

    if (single_rule_compile_on)
    {
        // in single rule compile mode just return the result
        QString buffer;
        QTextStream pf_str(&buffer);

        for (map<string, ostringstream*>::iterator fi=generated_scripts.begin();
             fi!=generated_scripts.end(); fi++)
        {
            string ruleset_name = fi->first;
            ostringstream *strm = fi->second;
            pf_str << table_factories[ruleset_name]->PrintTables();
            pf_str << strm->str();
        }

        // clear() calls destructors of all elements in the container
        table_factories.clear();
        generated_scripts.clear();

        return
            all_errors.join("\n").toStdString() + 
            buffer.toStdString();
    }

/*
 * now write generated scripts to files
 */
    for (map<string, ostringstream*>::iterator fi=generated_scripts.begin();
         fi!=generated_scripts.end(); fi++)
    {
        string ruleset_name = fi->first;
        string file_name = conf_files[ruleset_name];
        ostringstream *strm = fi->second;

        if (ruleset_name.find("/*")!=string::npos) continue;

        QFile pf_file(file_name.c_str());
        if (pf_file.open(QIODevice::WriteOnly))
        {
            QTextStream pf_str(&pf_file);

            if (ruleset_name == "__main__")
            {
                printStaticOptions(pf_str, fw);
                pf_str << table_factories[ruleset_name]->PrintTables();
                if (prolog_place == "pf_file_after_tables")
                    printProlog(pf_str, pre_hook);
            } else 
            {
                pf_str << table_factories[ruleset_name]->PrintTables();
            }

            pf_str << strm->str();
            pf_file.close();
        } else
        {
            // clear() calls destructors of all elements in the container
            table_factories.clear();
            generated_scripts.clear();

            throw FWException(string(" Failed to open file ") +
                              file_name + " for writing");
        }

    }
/*
 * assemble the script and then perhaps post-process it if needed
 */
    QString script_buffer = assembleFwScript(fw, !cluster_id.empty(), oscnf.get());

    // clear() calls destructors of all elements in the container
    table_factories.clear();
    generated_scripts.clear();

    info("Output file name: " + fw_file_name.toStdString());

    QFile fw_file(fw_file_name);
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
        throw FWException(string(" Failed to open file ") +
                          fw_file_name.toStdString() +
                          " for writing");
    }

    return "";
}

MapOstringStream::~MapOstringStream()
{
    clear();
}

void MapOstringStream::clear()
{
    std::map<std::string, std::ostringstream*>::iterator it;
    for (it=begin(); it!=end(); ++it)
        delete it->second;
    std::map<std::string, std::ostringstream*>::clear();
}

MapTableFactory::~MapTableFactory()
{
    clear();
}

void MapTableFactory::clear()
{
    std::map<std::string, fwcompiler::TableFactory*>::iterator it;
    for (it=begin(); it!=end(); ++it)
        delete it->second;
    std::map<std::string, fwcompiler::TableFactory*>::clear();
}

