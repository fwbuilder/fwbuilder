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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>
#include <cstring>
#include <iomanip>
#include <memory>

#include "CompilerDriver_ipt.h"
#include "PolicyCompiler_ipt.h"
#include "NATCompiler_ipt.h"
#include "RoutingCompiler_ipt.h"
#include "Preprocessor_ipt.h"
#include "OSConfigurator_linux24.h"
#include "OSConfigurator_secuwall.h"
#include "OSConfigurator_ipcop.h"
#include "combinedAddress.h"
#include "AutomaticRules_ipt.h"

#include "Configlet.h"


#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/ClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/StateSyncClusterGroup.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Constants.h"

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QtDebug>
#include <QTime>

#ifdef _WIN64
#define tzname _tzname
#endif

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

extern QString user_name;

FWObject* create_combinedAddress(int id)
{
    FWObject *nobj = new combinedAddress();
    if (id > -1) nobj->setId(id);
    return nobj;
}


/*
 * Go through paces to compile firewall which may be a member of a
 * cluster.  Note that both firewall and cluster are defined by their
 * unique string IDs.  This is necessary because CompilerDriver
 * operates with a copy of the object database which is not exposed
 * outside, so the caller can not provide pointers to these obejcts.
 */
QString CompilerDriver_ipt::run(const std::string &cluster_id,
                                const std::string &firewall_id,
                                const std::string &single_rule_id)
{

    FWObjectDatabase::registerObjectType(combinedAddress::TYPENAME,
                                         &create_combinedAddress);

    // see #2212 Create temporary copy of the firewall and cluster
    // objects and pass them to the compilers.

    Cluster *cluster = nullptr;
    Firewall *fw = nullptr;

    getFirewallAndClusterObjects(cluster_id, firewall_id, &cluster, &fw);

    string generated_script;

    try
    {
        clearReadOnly(fw);

        // Copy rules from the cluster object
        populateClusterElements(cluster, fw);

        commonChecks2(cluster, fw);

        string fw_version = fw->getStr("version");
        if (fw_version.empty()) fw_version = "(any version)";
        string platform = fw->getStr("platform");
        string host_os = fw->getStr("host_OS");

        FWOptions* options = fw->getOptionsObject();
        string s;

        // Note that fwobjectname may be different from the name of the
        // firewall fw This happens when we compile a member of a cluster
        current_firewall_name = fw->getName().c_str();

        if (fw->getOptionsObject()->getStr("prolog_place") == "after_flush" &&
            fw->getOptionsObject()->getBool("use_iptables_restore"))
        {
            abort("Prolog place \"after policy reset\" can not be used"
                  " when policy is activated with iptables-restore");
        }

        string firewall_dir = options->getStr("firewall_dir");
        if (firewall_dir=="") firewall_dir="/etc";

        bool debug=options->getBool("debug");
        QString shell_dbg = (debug)?"set -x":"" ;

        std::unique_ptr<OSConfigurator_linux24> oscnf;

        string platform_family = Resources::platform_res[platform]->
            getResourceStr("/FWBuilderResources/Target/family");
        string os_family = Resources::os_res[host_os]->
            getResourceStr("/FWBuilderResources/Target/family");

        bool supports_prolog_epilog = Resources::getTargetCapabilityBool(
            platform, "supports_prolog_epilog");

        if (!supports_prolog_epilog)
        {
            prolog_done = true;
            epilog_done = true;
        }

        string os_variant = Constants::getDistro();

/* minimal sanity checking */
        if (os_family == "ipcop")
        {
            os_variant = "ipcop";

            // can't use iptables-restore with ipcop
            fw->getOptionsObject()->setBool("use_iptables_restore", false);
            // ipcop has its own iptables commands that accept packets
            // in states ESTABLISHED,RELATED
            fw->getOptionsObject()->setBool("accept_established", false);

            oscnf = std::unique_ptr<OSConfigurator_linux24>(
                new OSConfigurator_ipcop(objdb , fw, false));
        }

        if (os_family == "linux24" ||
            os_family == "linux317" ||
            os_family == "openwrt" ||
            os_family == "dd-wrt-nvram" ||
            os_family == "dd-wrt-jffs" ||
            os_family == "sveasoft")
            oscnf = std::unique_ptr<OSConfigurator_linux24>(
                new OSConfigurator_linux24(objdb , fw, false));

        if (os_family == "secuwall")
            oscnf = std::unique_ptr<OSConfigurator_linux24>(
                new OSConfigurator_secuwall(objdb , fw, false));

        if (oscnf.get()==nullptr)
        {
            abort("Unrecognized host OS " + fw->getStr("host_OS") +
                  "  (family " + os_family+")");
            return "";
        }

        if (inTestMode()) oscnf->setTestMode();
        if (inEmbeddedMode()) oscnf->setEmbeddedMode();

        oscnf->validateInterfaces();
        oscnf->prolog();

        list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);
        list<FWObject*> all_nat = fw->getByType(NAT::TYPENAME);

        int routing_rules_count = 0;
        bool have_ipv4 = false;
        bool have_ipv6 = false;

        // track chains in each table separately. Can we have the same
        // chain in filter and mangle tables ? Would it be the same
        // chain, i.e. do we need to create it only once or do we create
        // it twice, in each table separately ? 
        // Using separate trackers we track and create chain in each
        // table separately.
        std::map<const std::string, bool> minus_n_commands_filter;
        std::map<const std::string, bool> minus_n_commands_mangle;
        std::map<const std::string, bool> minus_n_commands_nat;

        vector<int> ipv4_6_runs;

        findImportedRuleSets(fw, all_policies);
        findBranchesInMangleTable(fw, all_policies);
        findImportedRuleSets(fw, all_nat);

        try
        {
            AutomaticRules_ipt auto_rules(fw, persistent_objects);
            auto_rules.addConntrackRule();
            auto_rules.addFailoverRules();
        } catch (FWException &ex)
        {
            abort(ex.toString());
        }

        // assign unique rule ids that later will be used to generate
        // chain names.  This should be done after calls to
        // findImportedRuleSets()

        assignUniqueRuleIds(all_policies);
        assignUniqueRuleIds(all_nat);

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

        for (vector<int>::iterator i=ipv4_6_runs.begin(); i!=ipv4_6_runs.end(); ++i)
        {
            int policy_af = *i;
            bool ipv6_policy = (policy_af == AF_INET6);

            /*
              clear chain tracker map only between ipv4/ipv6 runs
              Don't clear it between compiler runs for different
              policy or nat objects for the same address family.
            */
            minus_n_commands_filter.clear();
            minus_n_commands_mangle.clear();
            minus_n_commands_nat.clear();

            /*
              We need to create and run preprocessor for this address
              family before nat and policy compilers, but if there are
              no nat / policy rules for this address family, we do not
              need preprocessor either.
            */

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
                Preprocessor_ipt* prep = new Preprocessor_ipt(
                    objdb , fw, ipv6_policy);
                prep->setSingleRuleCompileMode(single_rule_id);
                if (inTestMode()) prep->setTestMode();
                if (inEmbeddedMode()) prep->setEmbeddedMode();
                prep->compile();
                delete prep;
            }

            ostringstream automaitc_rules_stream;
            ostringstream automaitc_mangle_stream;
            ostringstream filter_rules_stream;
            ostringstream mangle_rules_stream;
            ostringstream nat_rules_stream;

            bool empty_output = true;

            // First, process branch NAT rulesets, then top NAT ruleset

            NAT *top_nat = nullptr;
            for (list<FWObject*>::iterator p=all_nat.begin();
                 p!=all_nat.end(); ++p)
            {
                NAT *nat = NAT::cast(*p);
                if (!nat->matchingAddressFamily(policy_af)) continue;
                if (nat->isTop())
                {
                    top_nat = nat;
                    continue;
                }
                if (! processNatRuleSet(
                        fw,
                        nat,
                        single_rule_id,
                        nat_rules_stream,
                        oscnf.get(),
                        policy_af,
                        minus_n_commands_nat)) empty_output = false;
            }

            if (top_nat &&
                ! processNatRuleSet(
                    fw,
                    top_nat,
                    single_rule_id,
                    nat_rules_stream,
                    oscnf.get(),
                    policy_af,
                    minus_n_commands_nat)) empty_output = false;

            // first process all non-top rule sets, then all top rule sets
            for (int all_top = 0; all_top < 2; ++all_top)
            {
                for (list<FWObject*>::iterator p=all_policies.begin();
                     p!=all_policies.end(); ++p )
                {
                    Policy *policy = Policy::cast(*p);
                    if (!policy->matchingAddressFamily(policy_af)) continue;

                    if (policy->isTop() && all_top == 0) continue;
                    if (!policy->isTop() && all_top == 1) continue;

                    if (! processPolicyRuleSet(
                            fw,
                            policy,
                            single_rule_id,
                            filter_rules_stream,
                            mangle_rules_stream,
                            automaitc_rules_stream,
                            automaitc_mangle_stream,
                            oscnf.get(),
                            policy_af,
                            minus_n_commands_filter,
                            minus_n_commands_mangle)) empty_output = false;
                }
            }

            if (!empty_output && !single_rule_compile_on)
            {
                if (ipv6_policy)
                {
                    have_ipv6 = true;
                    generated_script += "\n\n";
                    generated_script += "# ================ IPv6\n";
                    generated_script += "\n\n";
                } else
                {
                    have_ipv4 = true;
                    generated_script += "\n\n";
                    generated_script += "# ================ IPv4\n";
                    generated_script += "\n\n";
                }
            }

            generated_script += dumpScript(fw,
                                           automaitc_rules_stream.str(),
                                           automaitc_mangle_stream.str(),
                                           nat_rules_stream.str(),
                                           mangle_rules_stream.str(),
                                           filter_rules_stream.str(),
                                           ipv6_policy);
            if (single_rule_compile_on)
                generated_script += "\n\n";
        }

        std::unique_ptr<RoutingCompiler_ipt> routing_compiler(
            new RoutingCompiler_ipt(objdb, fw, false, oscnf.get()));

        RuleSet *routing = RuleSet::cast(fw->getFirstByType(Routing::TYPENAME));
        if (routing)
        {
            routing_compiler->setSourceRuleSet(routing);
            routing_compiler->setRuleSetName(routing->getName());
            routing_compiler->setPersistentObjects(persistent_objects);

            routing_compiler->setSingleRuleCompileMode(single_rule_id);
            routing_compiler->setDebugLevel( dl );
            if (rule_debug_on) routing_compiler->setDebugRule(drr);
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
                QString::fromUtf8(
                    (getErrors("") + 
                     generated_script +
                     routing_compiler->getCompiledScript()).c_str()));
        }


/*
 * These store generated configuration internally, extract it later using
 * OSConfiguration::getGeneratedFiles();
 */
        oscnf->processFirewallOptions();

/*
 * now write generated scripts to files
 */

        char *timestr = nullptr;
        time_t tm;
        struct tm *stm;

        tm = time(nullptr);
        stm = localtime(&tm);
        timestr = strdup(ctime(&tm));
        timestr[strlen(timestr)-1] = '\0';

        /*
         * assemble the script and then perhaps post-process it if it
         * should run on Linksys device with sveasoft firmware
         */
        Configlet script_skeleton(fw, "linux24", "script_skeleton");
        script_skeleton.removeComments();

        QString script_buffer;
        QTextStream script(&script_buffer, QIODevice::WriteOnly);

        /*
         * text comes from the compiler in UTF-8 (because all comments
         * and object names are stored in UTF-8 in objects and
         * compilers do not decode). We have a choice: 1) apply
         * QString::fromUtf8() to all strings coming from the compiler
         * to convert to Unicode and rely on QTextStream to convert
         * back to UTF-8 in the generated file, or 2) leavle strings
         * coming from compilers as-is and tell the stream to not
         * covert.
         */

        script_buffer = "";

        script_skeleton.setVariable("shell_debug", shell_dbg);

        script << "PATH=\"/sbin:/usr/sbin:/bin:/usr/bin:${PATH}\"" << "\n";
        script << "export PATH" << "\n";

        script_skeleton.setVariable("path", script_buffer);
        script_buffer = "";

        Configlet script_constants(fw, "linux24", "constants");
        script_skeleton.setVariable("constants", script_constants.expand());

        /*
         * print definitions for variables IPTABLES, IP, LOGGER. Some
         * day we may add a choice of os_variant in the GUI. Right now
         * paths are either default for a given os_variant, or custom
         * strings entered by user in the GUI and stored in firewall
         * options.
         */
        script_skeleton.setVariable("tools",
                                    oscnf->printPathForAllTools(os_variant).c_str());
        script_skeleton.setVariable("shell_functions",
                                    oscnf->printShellFunctions(have_ipv6).c_str());
        script_skeleton.setVariable("run_time_address_tables",
                                    oscnf->printRunTimeAddressTablesCode().c_str());
        script_skeleton.setVariable("using_ipset", oscnf->usingIpSetModule());

        if (supports_prolog_epilog)
        {
            //script_skeleton.setVariable("prolog_epilog",
            //                         oscnf->printPrologEpilogFunctions().c_str());
            script_skeleton.setVariable(
                "prolog_script", 
                fw->getOptionsObject()->getStr("prolog_script").c_str());
            script_skeleton.setVariable(
                "epilog_script", 
                fw->getOptionsObject()->getStr("epilog_script").c_str());
        }

        ostringstream ostr;

        ostr << "# Configure interfaces" << endl;

        if ( options->getBool("configure_bonding_interfaces") ) 
            ostr << oscnf->printBondingInterfaceConfigurationCommands();

        if ( options->getBool("configure_vlan_interfaces")) 
            ostr << oscnf->printVlanInterfaceConfigurationCommands();

        if ( options->getBool("configure_bridge_interfaces") ) 
            ostr << oscnf->printBridgeInterfaceConfigurationCommands();


        if ( options->getBool("configure_interfaces") ||
             options->getBool("manage_virtual_addr"))
        {
            if ( options->getBool("configure_interfaces"))
                ostr << oscnf->printInterfaceConfigurationCommands();
            else
                ostr << oscnf->printVirtualAddressesForNatCommands();
        }

        ostr << oscnf->printCommandsToClearKnownInterfaces();

        ostr << oscnf->printDynamicAddressesConfigurationCommands();

        script_skeleton.setVariable(
            "configure_interfaces", indent(4, QString(ostr.str().c_str())));



        // verify_interfaces checks bridge interfaces so run it
        // after those have been created
        if (options->getBool("verify_interfaces")) 
        {
            list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
            if (l2.empty() )
                script_skeleton.setVariable("verify_interfaces", QString());
            else
                script_skeleton.setVariable("verify_interfaces",
                                            oscnf->printVerifyInterfacesCommands().c_str());
        } else
            script_skeleton.setVariable("verify_interfaces", QString());
    
        string prolog_place = fw->getOptionsObject()->getStr("prolog_place");
        if (prolog_place == "") prolog_place="top";

        /* there is no way to stick prolog commands between iptables
         * reset and iptables rules if we use iptables-restore to
         * activate policy. Therefore, if prolog needs to be ran after
         * iptables flush and we use iptables-restore, we run prolog
         * on top of the script.
         */
        if (!prolog_done &&
            (prolog_place == "top" ||
             (prolog_place == "after_flush" && 
              fw->getOptionsObject()->getBool("use_iptables_restore"))))
        {
            script_skeleton.setVariable("prolog_top", 1);
            script_skeleton.setVariable("prolog_after_interfaces", 0);
            script_skeleton.setVariable("prolog_after_flush", 0);
            prolog_done = true;
        }

        if (!prolog_done && prolog_place == "after_interfaces")
        {
            script_skeleton.setVariable("prolog_top", 0);
            script_skeleton.setVariable("prolog_after_interfaces", 1);
            script_skeleton.setVariable("prolog_after_flush", 0);
            prolog_done = true;
        }

        if (!prolog_done && prolog_place == "after_flush")
        {
            script_skeleton.setVariable("prolog_top", 0);
            script_skeleton.setVariable("prolog_after_interfaces", 0);
            script_skeleton.setVariable("prolog_after_flush", 1);
            prolog_done = true;
        }

        script_skeleton.setVariable("load_modules",
                                    oscnf->generateCodeForProtocolHandlers().c_str());
        script_skeleton.setVariable("load_modules_with_nat", (have_nat)?"nat":"");
        script_skeleton.setVariable("load_modules_with_ipv6", (have_ipv6)?"ipv6":"");

        script_skeleton.setVariable("ip_forward_commands",
                                    oscnf->printIPForwardingCommands().c_str());

        /*
         * script body begins here
         */
        script_buffer = "";

        if (oscnf->haveErrorsAndWarnings())
        {
            all_errors.push_back(oscnf->getErrors("").c_str());
        }

        // convert from UTF8 to make sure localized comments are shown correctly
        // script << oscnf->getCompiledScript().c_str();
        // script << generated_script.c_str();
        // script << routing_compiler->getCompiledScript().c_str();

        script << QString::fromUtf8(oscnf->getCompiledScript().c_str());
        script << QString::fromUtf8(generated_script.c_str());
        script << QString::fromUtf8(routing_compiler->getCompiledScript().c_str());

        script << endl;

        script_skeleton.setVariable("script_body", indent(4, script_buffer));

        script_skeleton.setVariable("timestamp", timestr);
        script_skeleton.setVariable("tz", tzname[stm->tm_isdst]);
        script_skeleton.setVariable("user", user_name);
        script_skeleton.setVariable("database", objdb->getFileName().c_str());

        /*
         * Call reset_all function to flush and reset iptables, but only
         * do this if we do not use iptables_restore. Reset is done as part
         * of iptables-restore script in the latter case and commands are
         * added in PolicyCompiler_ipt::flushAndSetDefaultPolicy()
         */
        script_skeleton.setVariable("not_using_iptables_restore",
                                    ! fw->getOptionsObject()->getBool("use_iptables_restore"));

        script_buffer = "";
        if (have_ipv4) script << "  reset_iptables_v4" << endl;
        if (have_ipv6) script << "  reset_iptables_v6" << endl;
        script_skeleton.setVariable("reset_all", script_buffer);

        script_buffer = "";

        Configlet block_action(fw, "linux24", "block_action");
        if (XMLTools::version_compare(fw_version, "1.4.20") >= 0)
            block_action.setVariable("opt_wait", "-w");
        else
            block_action.setVariable("opt_wait", "");

        block_action.collapseEmptyStrings(true);

        // the name of the option is historical (including the typo)
        if (fw->getOptionsObject()->getBool("add_mgmt_ssh_rule_when_stoped"))
        {
            std::unique_ptr<PolicyCompiler_ipt> policy_compiler =
                createPolicyCompiler(fw, false, nullptr,  nullptr);
            PolicyCompiler_ipt::PrintRule* print_rule =
                policy_compiler->createPrintRuleProcessor();
            print_rule->setContext(policy_compiler.get());
            print_rule->_printBackupSSHAccessRules(&block_action);
        } else
        {
            block_action.setVariable("mgmt_access", 0);
        }

        script_skeleton.setVariable("block_action", block_action.expand());


        Configlet stop_action(fw, "linux24", "stop_action");
        stop_action.collapseEmptyStrings(true);
        stop_action.setVariable("have_ipv4", have_ipv4);
        stop_action.setVariable("have_ipv6", have_ipv6);

        if (XMLTools::version_compare(fw_version, "1.4.20") >= 0)
            stop_action.setVariable("opt_wait", "-w");
        else
            stop_action.setVariable("opt_wait", "");

        script_skeleton.setVariable("stop_action", stop_action.expand());



        Configlet status_action(fw, "linux24", "status_action");
        status_action.collapseEmptyStrings(true);
        script_skeleton.setVariable("status_action", status_action.expand());

        Configlet top_comment(fw, "linux24", "top_comment");

        top_comment.setVariable("version", VERSION);
        top_comment.setVariable("timestamp", timestr);
        top_comment.setVariable("tz", tzname[stm->tm_isdst]);
        top_comment.setVariable("user", user_name);
        top_comment.setVariable("database", objdb->getFileName().c_str());

        determineOutputFileNames(cluster, fw, !cluster_id.empty(),
                                 QStringList(""), QStringList("fw"),
                                 QStringList("script_name_on_firewall"));

        script_buffer = "";
        script << manifestMarker()
               << "* "
               << this->escapeFileName(file_names[FW_FILE]);

        if (!remote_file_names[FW_FILE].isEmpty())
            script << " " << this->escapeFileName(remote_file_names[FW_FILE]);
        script << "\n";

        /* Add additional files to manifest if specified.  Currently there
         * are no GUI controls to let user provide alternative names for
         * these on the firewall. See description of manifest format in
         * comments in src/gui/FirewallInstaller.cpp
         */
        map<string, string> file_list = oscnf->getGeneratedFiles();
        if (!file_list.empty())
        {
            info(" Adding additional files to manifest");
            map<string, string>::const_iterator c_iter = file_list.begin();
            for (; c_iter != file_list.end(); ++c_iter)
            {
                string name = c_iter->first;
                string dest = c_iter->second;
                script << manifestMarker() << this->escapeFileName(name.c_str());
                if (!dest.empty()) script << " " << dest;
                script << "\n";
            }
        }

        top_comment.setVariable("manifest", script_buffer);
        top_comment.setVariable("platform", platform.c_str());
        top_comment.setVariable("fw_version", fw_version.c_str());
        top_comment.setVariable("comment", prepend("# ", fw->getComment().c_str()));

        script_skeleton.setVariable("top_comment", top_comment.expand());
        script_skeleton.setVariable("errors_and_warnings",
                                    prepend("# ", all_errors.join("\n")));

        info("Output file name: " + file_names[FW_FILE].toStdString());

        QFile fw_file(file_names[FW_FILE]);
        if (fw_file.open(QIODevice::WriteOnly))
        {
            QTextStream fw_str(&fw_file);
            fw_str << script_skeleton.expand();
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

        free(timestr);

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


