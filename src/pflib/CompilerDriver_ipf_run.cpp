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

#include <assert.h>
#include <cstring>
#include <iomanip>
#include <memory>

#include "CompilerDriver_ipf.h"

#include "PolicyCompiler_ipf.h"
#include "NATCompiler_ipf.h"
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
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/XMLTools.h"

#include "fwcompiler/Preprocessor.h"

#include <QStringList>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


QString CompilerDriver_ipf::composeActivationCommand(libfwbuilder::Firewall *fw,
                                                     bool filter,
                                                     const std::string &debug,
                                                     const std::string &,
                                                     const std::string &remote_file)
{
    Configlet act(fw, "ipf", "activation");
    act.removeComments();
    act.collapseEmptyStrings(true);
    act.setVariable("dyn_addr", fw->getOptionsObject()->getBool("dynAddr"));
    act.setVariable("not_dyn_addr", !fw->getOptionsObject()->getBool("dynAddr"));
    act.setVariable("filter", filter);
    act.setVariable("nat", !filter);
    act.setVariable("ipf_debug", debug.c_str());
    act.setVariable("remote_file", remote_file.c_str());
    act.setVariable("interface_name_substitution_commands",
                    printActivationCommandWithSubstitution(fw));
    return act.expand();
}

QString CompilerDriver_ipf::assembleManifest(Cluster*, Firewall* fw, bool )
{
    (void) fw; // Unused
    QString remote_name = remote_file_names[FW_FILE];
    QString remote_ipf_name = remote_file_names[CONF1_FILE];
    QString remote_nat_name = remote_file_names[CONF2_FILE];

    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);

    script << manifestMarker()
           << "* "
           << this->escapeFileName(file_names[FW_FILE]);

    if (remote_name != file_names[FW_FILE])
        script << " " << this->escapeFileName(remote_name);
    script << endl;

    if (have_filter) 
    {
        script << manifestMarker()
               << "  "
               << this->escapeFileName(file_names[CONF1_FILE]);

        if (remote_ipf_name != file_names[CONF1_FILE])
            script << " " << this->escapeFileName(remote_ipf_name);
        script << endl;
    }

    if (have_nat) 
    {
        script << manifestMarker()
               << "  "
               << this->escapeFileName(file_names[CONF2_FILE]);

        if (remote_nat_name != file_names[CONF2_FILE])
            script << " " << this->escapeFileName(remote_nat_name);
        script << endl;
    }

    return script_buffer;
}

QString CompilerDriver_ipf::assembleFwScript(Cluster *cluster,
                                             Firewall* fw,
                                             bool cluster_member,
                                             OSConfigurator *oscnf)
{
    Configlet script_skeleton(fw, "ipf", "script_skeleton");
    Configlet top_comment(fw, "ipf", "top_comment");

    assembleFwScriptInternal(
        cluster, fw, cluster_member, oscnf,
        &script_skeleton, &top_comment, "#", true);
    return script_skeleton.expand();
}

QString CompilerDriver_ipf::printActivationCommands(libfwbuilder::Firewall*)
{
    return activation_commands.join("\n");
}

QString CompilerDriver_ipf::run(const std::string &cluster_id,
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
        string fw_version = fw->getStr("version");

        // Note that fwobjectname may be different from the name of the
        // firewall fw This happens when we compile a member of a cluster
        current_firewall_name = fw->getName().c_str();

        string s;

        bool debug = options->getBool("debug");
        string ipf_dbg = (debug)?"-v":"";

        std::unique_ptr<Preprocessor> prep(new Preprocessor(objdb , fw, false));
        prep->compile();

/*
 * Process firewall options, build OS network configuration script
 */
        std::unique_ptr<OSConfigurator_bsd> oscnf;
        string host_os = fw->getStr("host_OS");
        string family=Resources::os_res[host_os]->Resources::getResourceStr("/FWBuilderResources/Target/family");
        if ( host_os == "solaris" )
            oscnf = std::unique_ptr<OSConfigurator_bsd>(new OSConfigurator_solaris(objdb , fw, false));

        if ( host_os == "openbsd")
            oscnf = std::unique_ptr<OSConfigurator_bsd>(new OSConfigurator_openbsd(objdb , fw, false));

        if ( host_os == "freebsd")
            oscnf = std::unique_ptr<OSConfigurator_bsd>(new OSConfigurator_freebsd(objdb , fw, false));

        if (oscnf.get()==nullptr)
        {
            abort("Unrecognized host OS " + host_os + "  (family " + family + ")");
            return "";
        }

        oscnf->prolog();

        list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);
        list<FWObject*> all_nat = fw->getByType(NAT::TYPENAME);

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

        PolicyCompiler_ipf c(objdb , fw, false , oscnf.get() );

        FWObject *policy = all_policies.front();

        c.setSourceRuleSet(Policy::cast(policy));
        c.setRuleSetName(policy->getName());
        c.setPersistentObjects(persistent_objects);

        c.setSingleRuleCompileMode(single_rule_id);
        c.setDebugLevel( dl );
        if (rule_debug_on) c.setDebugRule(  drp );
        c.setVerbose( verbose );
        if (inTestMode()) c.setTestMode();
        if (inEmbeddedMode()) c.setEmbeddedMode();

        if ( c.prolog() > 0 )
        {
            have_filter = true;
            c.compile();
            c.epilog();
        }

        NATCompiler_ipf n( objdb , fw, false , oscnf.get() );

        FWObject *nat = all_nat.front();

        n.setSourceRuleSet(NAT::cast(nat));
        n.setRuleSetName(nat->getName());
        n.setPersistentObjects(persistent_objects);

        n.setSingleRuleCompileMode(single_rule_id);
        n.setDebugLevel( dl );
        if (rule_debug_on) n.setDebugRule(  drn );
        n.setVerbose( verbose );
        if (inTestMode()) n.setTestMode();
        if (inEmbeddedMode()) n.setEmbeddedMode();

        if ( n.prolog() > 0 )
        {
            have_nat = true;
            n.compile();
            n.epilog();
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
            ostringstream ostr;

            if (have_filter) 
            {
                if (c.haveErrorsAndWarnings())
                {
                    all_errors.push_back(c.getErrors("").c_str());
                }
                ostr << c.getCompiledScript();
            }

            if (have_nat) 
            {
                if (n.haveErrorsAndWarnings())
                {
                    all_errors.push_back(n.getErrors("").c_str());
                }
                ostr << n.getCompiledScript();
            }

            return formSingleRuleCompileOutput(
                QString::fromUtf8(ostr.str().c_str()));
        }

        determineOutputFileNames(cluster, fw, !cluster_id.empty(),
                                 QStringList() << "" << "ipf" << "nat",
                                 QStringList() << "fw" << "conf" << "conf",
                                 QStringList() << "script_name_on_firewall"
                                 << "ipf_conf_file_name_on_firewall"
                                 << "nat_conf_file_name_on_firewall");

        QString remote_ipf_name = remote_file_names[CONF1_FILE];
        QString remote_nat_name = remote_file_names[CONF2_FILE];

        if (have_filter) 
        {
            QString output_file = getAbsOutputFileName(file_names[CONF1_FILE]);

            info("Output file name: " + output_file.toStdString());
            QFile ipf_file(output_file);
            if (ipf_file.open(QIODevice::WriteOnly))
            {
                QTextStream ipf_str(&ipf_file);
                if (c.haveErrorsAndWarnings())
                {
                    all_errors.push_back(c.getErrors("").c_str());
                    ipf_str << "# Policy compiler errors and warnings:"
                            << endl;
                    ipf_str << QString::fromUtf8(c.getErrors("# ").c_str());
                }
                ipf_str << QString::fromUtf8(c.getCompiledScript().c_str());
                ipf_file.close();
                ipf_file.setPermissions(QFile::ReadOwner | QFile::WriteOwner |
                                        QFile::ReadGroup | QFile::ReadOther |
                                        QFile::ExeOwner | 
                                        QFile::ExeGroup |
                                        QFile::ExeOther );
            } else
            {
                QString err(" Failed to open file %1 for writing: %2; "
                            "Current dir: %3");
                abort(err.arg(ipf_file.fileName())
                      .arg(ipf_file.error())
                      .arg(QDir::current().path()).toStdString());
            }

            QString remote_file_name = escapeFileName(remote_ipf_name);
            activation_commands.push_back(
                composeActivationCommand(
                    fw, true, ipf_dbg, fw_version,
                    remote_file_name.toUtf8().constData()));
        }

        if (have_nat) 
        {
            QString output_file = getAbsOutputFileName(file_names[CONF2_FILE]);

            info("Output file name: " + output_file.toStdString());
            QFile nat_file(output_file);
            if (nat_file.open(QIODevice::WriteOnly))
            {
                QTextStream nat_str(&nat_file);
                if (n.haveErrorsAndWarnings())
                {
                    all_errors.push_back(n.getErrors("").c_str());
                    nat_str << "# NAT compiler errors and warnings:"
                            << endl;
                    nat_str << QString::fromUtf8(n.getErrors("# ").c_str());
                }
                nat_str << QString::fromUtf8(n.getCompiledScript().c_str());
                nat_file.close();
                nat_file.setPermissions(QFile::ReadOwner | QFile::WriteOwner |
                                        QFile::ReadGroup | QFile::ReadOther |
                                        QFile::ExeOwner | 
                                        QFile::ExeGroup |
                                        QFile::ExeOther );
            } else
            {
                QString err(" Failed to open file %1 for writing: %2; "
                            "Current dir: %3");
                abort(err.arg(nat_file.fileName())
                      .arg(nat_file.error())
                      .arg(QDir::current().path()).toStdString());
            }

            QString remote_file_name = escapeFileName(remote_nat_name);
            activation_commands.push_back(
                composeActivationCommand(
                    fw, false, ipf_dbg, fw_version,
                    remote_file_name.toUtf8().constData()));
        }
/*
 * assemble the script and then perhaps post-process it if needed
 */
        QString script_buffer = assembleFwScript(
            cluster, fw, !cluster_id.empty(), oscnf.get());


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


