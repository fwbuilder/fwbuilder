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

#include "CompilerDriver_ipfw.h"
#include "PolicyCompiler_ipfw.h"

#include "OSConfigurator_freebsd.h"
#include "OSConfigurator_macosx.h"

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


QString CompilerDriver_ipfw::assembleManifest(Firewall* fw)
{
    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);
    script << MANIFEST_MARKER << "* " << QFileInfo(fw_file_name).fileName();
    string remote_name = fw->getOptionsObject()->getStr("script_name_on_firewall");
    if (!remote_name.empty()) script << " " << remote_name;
    script << "\n";
    script << "#" << endl;
    script << "#" << endl;
    return script_buffer;
}

QString CompilerDriver_ipfw::printActivationCommands(Firewall *fw)
{
    return activation_commands.join("\n");
}

QString CompilerDriver_ipfw::assembleFwScript(Firewall* fw, OSConfigurator *oscnf)
{
    Configlet script_skeleton(fw, "bsd", "ipfw_script_skeleton");
    Configlet top_comment(fw, "bsd", "top_comment");

    assembleFwScriptInternal(fw, oscnf, &script_skeleton, &top_comment);
    return script_skeleton.expand();
}

string CompilerDriver_ipfw::run(const std::string &cluster_id,
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

    string s;

    string firewall_dir=options->getStr("firewall_dir");
    if (firewall_dir=="") firewall_dir="/etc/fw";

    bool debug=options->getBool("debug");
    string shell_dbg=(debug)?"-x":"" ;

/*
 * Process firewall options, build OS network configuration script
 */
    std::auto_ptr<OSConfigurator_bsd> oscnf;
    string host_os = fw->getStr("host_OS");
    string family = Resources::os_res[host_os]->Resources::getResourceStr("/FWBuilderResources/Target/family");
    if ( host_os == "macosx")
        oscnf = std::auto_ptr<OSConfigurator_bsd>(new OSConfigurator_macosx(objdb , fw, false));

    if ( host_os == "freebsd")
        oscnf = std::auto_ptr<OSConfigurator_bsd>(new OSConfigurator_freebsd(objdb , fw, false));

    if (oscnf.get()==NULL)
        throw FWException("Unrecognized host OS " + host_os + "  (family " + family + ")");

    oscnf->prolog();


    list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);
    vector<int> ipv4_6_runs;
    string generated_script;
    int policy_rules_count  = 0;
    int ipfw_rule_number = 0;

    findImportedRuleSets(fw, all_policies);

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
            std::auto_ptr<Preprocessor> prep(new Preprocessor(objdb , fw, ipv6_policy));
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
                    if (c.haveErrorsAndWarnings())
                    {
                        all_errors.push_back(c.getErrors("").c_str());
                        // c_str << "# Policy compiler errors and warnings:"
                        //       << endl;
                        // c_str << c.getErrors("# ");
                    }
                    c_str << c.getCompiledScript();
                    c_str << endl;
                    empty_output = false;
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

    if (haveErrorsAndWarnings())
    {
        all_errors.push_front(getErrors("").c_str());
    }

    if (single_rule_compile_on)
    {
        return
            all_errors.join("\n").toStdString() + 
            generated_script;
    }

    PolicyCompiler_ipfw c(objdb, fw, false, oscnf.get());
    activation_commands.push_back(c.defaultRules().c_str());
    activation_commands.push_back(generated_script.c_str());

/*
 * assemble the script and then perhaps post-process it if needed
 */
    QString script_buffer = assembleFwScript(fw, oscnf.get());


/*********************************************************************/
#if OLD_SCHOOL
/*
 * now write generated scripts to files
 */


    char          *timestr;
    time_t         tm;
    struct tm     *stm;

    tm=time(NULL);
    stm=localtime(&tm);
    timestr=strdup(ctime(&tm));
    timestr[ strlen(timestr)-1 ]='\0';
    
#ifdef _WIN32
    char* user_name=getenv("USERNAME");
#else
    struct passwd *pwd=getpwuid(getuid());
    assert(pwd);
    char *user_name=pwd->pw_name;
#endif
    if (user_name==NULL)
    {
        user_name=getenv("LOGNAME");
        if (user_name==NULL)
            abort("Can't figure out your user name");

    }

    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);

    script << "#!/bin/sh " << shell_dbg << endl << endl;

    script << "#\n\
#  This is automatically generated file. DO NOT MODIFY !\n\
#\n\
#  Firewall Builder  fwb_ipfw v" << VERSION << "-" << BUILD_NUM << " \n\
#\n\
#  Generated " << timestr << " " << tzname[stm->tm_isdst] << " by " 
            << user_name << "\n#\n";

    script << MANIFEST_MARKER << "* " << QFileInfo(fw_file_name).fileName();
    string remote_name = fw->getOptionsObject()->getStr("script_name_on_firewall");
    if (!remote_name.empty()) script << " " << remote_name;
    script << "\n";
    script << "#" << endl;
    script << "#" << endl;

    string fwcomment=fw->getComment();
    string::size_type n1,n2;
    n1=n2=0;
    while ( (n2=fwcomment.find("\n",n1))!=string::npos )
    {
        script << "#  " << fwcomment.substr(n1,n2-n1) << endl;
        n1=n2+1;
    }
    script << "#  " << fwcomment.substr(n1) << endl;
    script << "#\n#\n#\n";

    script << prepend("# ", all_errors.join("\n")).toStdString() << endl;

    script << "cd " << firewall_dir << " || exit 1" << endl << endl;

    script << endl;
    script << "#" << endl;
    script << "# Prolog script" << endl;
    script << "#" << endl;

    string pre_hook= fw->getOptionsObject()->getStr("prolog_script");
    script << pre_hook << endl;

    script << "#" << endl;
    script << "# End of prolog script" << endl;
    script << "#" << endl;

    script << oscnf->getCompiledScript();

    script << endl;

    script << "log '";
    script << "Activating firewall script generated "
            << timestr << " " << " by "
            << user_name;
    script << "'" << endl;

    script << endl;



/* commented out since we now use sets
   script << "\"$IPFW\" -f -q flush" << endl;
*/
    script << endl;

#if NO_IPV6
    if (have_ipfw)
    {
        if (c.haveErrorsAndWarnings())
        {
            script << "# Policy compiler errors and warnings:"
                    << endl;
            script << c.getErrors("# ");
        }

        script << c.getCompiledScript();
    }
#else
    PolicyCompiler_ipfw c(objdb, fw, false, oscnf.get());
    script << c.defaultRules();
    script << generated_script;
#endif

    script << endl;
    script << "#" << endl;
    script << "# Epilog script" << endl;
    script << "#" << endl;

    string post_hook= fw->getOptionsObject()->getStr("epilog_script");
    script << post_hook << endl;

    script << endl;
    script << "# End of epilog script" << endl;
    script << "#" << endl;

    script << "\"$IPFW\" set swap 0 1 || exit 1" << endl;
    script << "\"$IPFW\" delete set 1" << endl;

    script << endl;

#endif


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

