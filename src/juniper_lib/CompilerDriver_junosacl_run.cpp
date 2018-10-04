
#include "CompilerDriver_junosacl.h"
#include "OSConfigurator_junos.h"

#include "cisco_lib/NamedObjectsManager.h"
#include "cisco_lib/NamedObjectsAndGroupsSupport.h"
#include "PolicyCompiler_junosacl.h"

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

#include <QStringList>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


QString CompilerDriver_junosacl::assembleManifest(Cluster*, Firewall*, bool)
{
    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);

    script << "/* " << manifestMarker()
           << " * " << this->escapeFileName(file_names[FW_FILE]) << " */" << endl;
    return script_buffer;
}

QString CompilerDriver_junosacl::printActivationCommands(Firewall *)
{
    return QString();
}

QString CompilerDriver_junosacl::assembleFwScript(Cluster *cluster,
                                                  Firewall *fw,
                                                  bool cluster_member,
                                                  OSConfigurator *oscnf)
{
    Configlet script_skeleton(fw, "junos", "script_skeleton");
    Configlet top_comment(fw, "junos", "top_comment");

    script_skeleton.setVariable("system_configuration_script",
                                QString::fromUtf8(system_configuration_script.c_str()));
    script_skeleton.setVariable("policy_script",
                                QString::fromUtf8(policy_script.c_str()));

    FWOptions* options = fw->getOptionsObject();
    options->setStr("prolog_script", options->getStr("junosacl_prolog_script"));
    options->setStr("epilog_script", options->getStr("junosacl_epilog_script"));

    // we do not offer user a choice of the place where to put prolog
    // lines, therefore we can reset this attribute to make sure it
    // does not interfere
    options->setStr("prolog_place", "");

    assembleFwScriptInternal(cluster, fw, cluster_member,
                             oscnf, &script_skeleton, &top_comment,
                             QString::fromStdString(comment_symbol), true);
    return script_skeleton.expand();
}

QString CompilerDriver_junosacl::run(const string &cluster_id,
                                     const string &firewall_id,
                                     const string &single_rule_id)
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

        // Note that fwobjectname may be different from the name of the
        // firewall fw This happens when we compile a member of a cluster
        current_firewall_name = fw->getName().c_str();

        determineOutputFileNames(cluster, fw, !cluster_id.empty(),
                                 QStringList(""), QStringList("fw"),
                                 QStringList(""));

        /* Now that all checks are done, we can drop copies of cluster
         * interfaces that were added to the firewall by
         * CompilerDriver::populateClusterElements()
         */
        list<FWObject*> all_interfaces = fw->getByTypeDeep(Interface::TYPENAME);
        list<FWObject*> copies_of_cluster_interfaces;
        for (std::list<FWObject*>::iterator i=all_interfaces.begin(); i!=all_interfaces.end(); ++i)
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);

            if (iface->getOptionsObject()->getBool("cluster_interface"))
                copies_of_cluster_interfaces.push_back(iface);
        }
        while (copies_of_cluster_interfaces.size())
        {
            fw->remove(copies_of_cluster_interfaces.front());
            copies_of_cluster_interfaces.pop_front();
        }

        FWOptions* options = fw->getOptionsObject();

        string fwvers = fw->getStr("version");
        if (fwvers == "") fw->setStr("version", "11.2");
        if (fwvers == "11.x") fw->setStr("version", "11.2");

        string platform = fw->getStr("platform");

        std::unique_ptr<OSConfigurator_junos> oscnf(new OSConfigurator_junos(objdb, fw, false));

        oscnf->prolog();
        oscnf->processFirewallOptions();

        list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);

        // assign unique rule ids that later will be used to generate
        // chain names.  This should be done after calls to
        // findImportedRuleSets()
        // NB: these ids are not used by this compiler

        assignUniqueRuleIds(all_policies);

        vector<int> ipv4_6_runs;

        // // // // //NamedObjectsManager named_objects_manager(persistent_objects, fw);

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

        string object_groups_definitions;

        for (vector<int>::iterator i=ipv4_6_runs.begin();
             i!=ipv4_6_runs.end(); ++i)
        {
            int policy_af = *i;
            bool ipv6_policy = (policy_af == AF_INET6);

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
                std::unique_ptr<Preprocessor> prep(new Preprocessor(objdb, fw, false));
                if (inTestMode()) prep->setTestMode();
                if (inEmbeddedMode()) prep->setEmbeddedMode();
                prep->compile();
            }

            for (list<FWObject*>::iterator p=all_policies.begin();
                 p!=all_policies.end(); ++p)
            {
                Policy *policy = Policy::cast(*p);

                if (!policy->matchingAddressFamily(policy_af)) continue;

                PolicyCompiler_junosacl c(objdb, fw, ipv6_policy, oscnf.get());

                // // // // //c.setNamedObjectsManager(&named_objects_manager);
                c.setSourceRuleSet( policy );
                c.setRuleSetName(policy->getName());
                c.setPersistentObjects(persistent_objects);

                c.setSingleRuleCompileMode(single_rule_id);
                if (inTestMode()) c.setTestMode();
                if (inEmbeddedMode()) c.setEmbeddedMode();
                c.setDebugLevel( dl );
                if (rule_debug_on) c.setDebugRule( drp );
                c.setVerbose( verbose );

                if ( c.prolog() > 0)
                {
                    c.compile();
                    c.epilog();

                    if (!single_rule_compile_on)
                    {
                        if (ipv6_policy)
                        {
                            policy_script += "\n\n";
                            policy_script += "# ================ IPv6\n";
                            policy_script += "\n\n";
                        } else {
                            policy_script += "\n\n";
                            policy_script += "# ================ IPv4\n";
                            policy_script += "\n\n";
                        }
                    }

                    if (c.haveErrorsAndWarnings())
                    {
                        all_errors.push_back(c.getErrors("").c_str());
                    }
                    policy_script += c.getCompiledScript();
                } else {
                    info(" Nothing to compile in Policy");
                }
            }
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

        // // // // //object_groups_definitions +=
        // // // // //        named_objects_manager.getNamedObjectsDefinitions();

        if (single_rule_compile_on)
        {
            return formSingleRuleCompileOutput(
                        QString::fromUtf8(
                            (object_groups_definitions +
                             policy_script).c_str()));
        }

        system_configuration_script += object_groups_definitions;

        QString script_buffer = assembleFwScript(
                    cluster, fw, !cluster_id.empty(), oscnf.get());

        QString ofname = getAbsOutputFileName(file_names[FW_FILE]);

        info("Output file name: " + ofname.toStdString());
        QFile fw_file(ofname);
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
        } else {
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
        return QString::fromUtf8((ex.toString().c_str()));
    }

    return "";
}

































