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
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

extern QString build_num;


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
    FWOptions* options = fw->getOptionsObject();

    //qDebug() << "CompilerDriver_ipf::assembleManifest fw_file_name=" << fw_file_name;

    QFileInfo fw_file_info(fw_file_name);
    QString ipf_file_name = fw_file_info.completeBaseName() + "-ipf.conf";
    QString nat_file_name = fw_file_info.completeBaseName() + "-nat.conf";
    if (fw_file_info.path() != ".")
    {
        ipf_file_name = fw_file_info.path() + "/" + ipf_file_name;
        nat_file_name = fw_file_info.path() + "/" + nat_file_name;
    }
    QString remote_ipf_name = options->getStr("ipf_conf_file_name_on_firewall").c_str();
    if (remote_ipf_name.isEmpty()) remote_ipf_name = ipf_file_name;

    QString remote_nat_name = options->getStr("nat_conf_file_name_on_firewall").c_str();
    if (remote_nat_name.isEmpty()) remote_nat_name = nat_file_name;

    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);

    script << MANIFEST_MARKER << "* " << this->escapeFileName(fw_file_info.fileName());
    string remote_name = fw->getOptionsObject()->getStr("script_name_on_firewall");
    if (!remote_name.empty()) script << " " << this->escapeFileName(remote_name.c_str());
    script << endl;

    if (have_filter) 
    {
        script << MANIFEST_MARKER << "  " << this->escapeFileName(QFileInfo(ipf_file_name).fileName());
        if (remote_ipf_name != ipf_file_name) script << " " << this->escapeFileName(remote_ipf_name);
        script << endl;
    }

    if (have_nat) 
    {
        script << MANIFEST_MARKER << "  " << this->escapeFileName(QFileInfo(nat_file_name).fileName());
        if (remote_nat_name != nat_file_name) script << " " << this->escapeFileName(remote_nat_name);
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
        cluster, fw, cluster_member, oscnf, &script_skeleton, &top_comment, "#");
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
    Cluster *cluster = NULL;
    if (!cluster_id.empty())
        cluster = Cluster::cast(
            objdb->findInIndex(objdb->getIntId(cluster_id)));

    Firewall *fw = Firewall::cast(
        objdb->findInIndex(objdb->getIntId(firewall_id)));
    assert(fw);

    try
    {
        // Copy rules from the cluster object
        populateClusterElements(cluster, fw);

        commonChecks2(cluster, fw);

        FWOptions* options = fw->getOptionsObject();
        string fw_version = fw->getStr("version");

        // Note that fwobjectname may be different from the name of the
        // firewall fw This happens when we compile a member of a cluster
        current_firewall_name = fw->getName().c_str();

        fw_file_name = determineOutputFileName(cluster, fw, !cluster_id.empty(), ".fw");

        QFileInfo finfo(fw_file_name);
        QString ipf_file_name = finfo.completeBaseName() + "-ipf.conf";
        QString nat_file_name = finfo.completeBaseName() + "-nat.conf";
        if (finfo.path() != ".")
        {
            ipf_file_name = finfo.path() + "/" + ipf_file_name;
            nat_file_name = finfo.path() + "/" + nat_file_name;
        }

        QString remote_ipf_name = options->getStr("ipf_conf_file_name_on_firewall").c_str();
        if (remote_ipf_name.isEmpty())
            remote_ipf_name = QFileInfo(ipf_file_name).fileName();
        remote_ipf_name = this->escapeFileName(remote_ipf_name);

        QString remote_nat_name = options->getStr("nat_conf_file_name_on_firewall").c_str();
        if (remote_nat_name.isEmpty())
            remote_nat_name = QFileInfo(nat_file_name).fileName();
        remote_nat_name = this->escapeFileName(remote_nat_name);

        string s;

        string firewall_dir = options->getStr("firewall_dir");
        if (firewall_dir=="") firewall_dir = "/etc/fw";

        bool debug = options->getBool("debug");
        string ipf_dbg = (debug)?"-v":"";

        std::auto_ptr<Preprocessor> prep(new Preprocessor(objdb , fw, false));
        prep->compile();

/*
 * Process firewall options, build OS network configuration script
 */
        std::auto_ptr<OSConfigurator_bsd> oscnf;
        string host_os = fw->getStr("host_OS");
        string family=Resources::os_res[host_os]->Resources::getResourceStr("/FWBuilderResources/Target/family");
        if ( host_os == "solaris" )
            oscnf = std::auto_ptr<OSConfigurator_bsd>(new OSConfigurator_solaris(objdb , fw, false));

        if ( host_os == "openbsd")
            oscnf = std::auto_ptr<OSConfigurator_bsd>(new OSConfigurator_openbsd(objdb , fw, false));

        if ( host_os == "freebsd")
            oscnf = std::auto_ptr<OSConfigurator_bsd>(new OSConfigurator_freebsd(objdb , fw, false));

        if (oscnf.get()==NULL)
        {
            abort("Unrecognized host OS " + host_os + "  (family " + family + ")");
            return "";
        }

        oscnf->prolog();

        list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);
        list<FWObject*> all_nat = fw->getByType(NAT::TYPENAME);

        PolicyCompiler_ipf c(objdb , fw, false , oscnf.get() );

        FWObject *policy = all_policies.front();

        c.setSourceRuleSet(Policy::cast(policy));
        c.setRuleSetName(policy->getName());

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


        if (have_filter) 
        {
            QFileInfo finfo(ipf_file_name);
            if (finfo.isRelative())
            {
                // if ipf_file_name is relative, it is relative to the
                // directory the program started in, which can be
                // different from wdir and different from the current dir
                // at this point because we do chdir to the directory
                // defined by the -d command line option
                QFileInfo new_finfo(start_current_dir, ipf_file_name);
                ipf_file_name = new_finfo.absoluteFilePath();
            }

            info("Output file name: " + ipf_file_name.toStdString());
            QFile ipf_file(ipf_file_name);
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
                QString err(" Failed to open file %1 for writing: %2; Current dir: %3");
                abort(err.arg(ipf_file.fileName()).arg(ipf_file.error()).arg(QDir::current().path()).toStdString());
            }

            QString filePath;
            if (remote_ipf_name[0] == '/') filePath = remote_ipf_name;
            else filePath = QString("${FWDIR}/") + remote_ipf_name;

            activation_commands.push_back(
                composeActivationCommand(
                    fw, true, ipf_dbg, fw_version, filePath.toStdString()));
        }

        if (have_nat) 
        {
            QFileInfo finfo(nat_file_name);
            if (finfo.isRelative())
            {
                // if nat_file_name is relative, it is relative to the
                // directory the program started in, which can be
                // different from wdir and different from the current dir
                // at this point because we do chdir to the directory
                // defined by the -d command line option
                QFileInfo new_finfo(start_current_dir, nat_file_name);
                nat_file_name = new_finfo.absoluteFilePath();
            }

            info("Output file name: " + nat_file_name.toStdString());
            QFile nat_file(nat_file_name);
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
                QString err(" Failed to open file %1 for writing: %2; Current dir: %3");
                abort(err.arg(nat_file.fileName()).arg(nat_file.error()).arg(QDir::current().path()).toStdString());
            }

            QString filePath;
            if (remote_nat_name[0] == '/') filePath = remote_nat_name;
            else filePath = QString("${FWDIR}/") + remote_nat_name;
            activation_commands.push_back(
                composeActivationCommand(
                    fw, false, ipf_dbg, fw_version, filePath.toStdString()));
        }
/*
 * assemble the script and then perhaps post-process it if needed
 */
        QString script_buffer = assembleFwScript(
            cluster, fw, !cluster_id.empty(), oscnf.get());


        QFileInfo fw_finfo(fw_file_name);
        if (fw_finfo.isRelative())
        {
            // if fw_file_name is relative, it is relative to the
            // directory the program started in, which can be
            // different from wdir and different from the current dir
            // at this point because we do chdir to the directory
            // defined by the -d command line option
            QFileInfo new_finfo(start_current_dir, fw_file_name);
            fw_file_name = new_finfo.absoluteFilePath();
        }

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
            QString err(" Failed to open file %1 for writing: %2; Current dir: %3");
            abort(err.arg(fw_file.fileName()).arg(fw_file.error()).arg(QDir::current().path()).toStdString());
        }
    }
    catch (FatalErrorInSingleRuleCompileMode &ex)
    {
        return QString::fromUtf8(getErrors("").c_str());
    }
    
        
    return "";
}


