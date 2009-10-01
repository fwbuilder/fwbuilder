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

#include "CompilerDriver_iosacl.h"

#include "PolicyCompiler_iosacl.h"
#include "RoutingCompiler_iosacl.h"
#include "OSConfigurator_ios.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"

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

string CompilerDriver_iosacl::run(const std::string &cluster_id,
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

    // Note that fwobjectname may be different from the name of the
    // firewall fw This happens when we compile a member of a cluster
    current_firewall_name = fw->getName().c_str();

    QString ofname = determineOutputFileName(fw, !cluster_id.empty(), ".fw");

    FWOptions* options = fw->getOptionsObject();

    string fwvers = fw->getStr("version");
    if (fwvers == "") fw->setStr("version", "12.x");

    string platform = fw->getStr("platform");
    string clearACLCmd = Resources::platform_res[platform]->getResourceStr(
        string("/FWBuilderResources/Target/options/") +
        "version_" + fwvers + "/iosacl_commands/clear_ip_acl");
    if (clearACLCmd.empty())
    {
        // incorrect version. This could have happened if user converted
        // firewall platform. See bug #2662290
        fw->setStr("version", "12.x");
    }

    bool ios_acl_basic = options->getBool("ios_acl_basic");
    bool ios_acl_no_clear = options->getBool("ios_acl_no_clear");
    bool ios_acl_substitution = options->getBool("ios_acl_substitution");
    bool ios_add_clear_statements = options->getBool("ios_add_clear_statements");

    if ( !ios_acl_basic &&
         !ios_acl_no_clear &&
         !ios_acl_substitution )
    {
        if ( ios_add_clear_statements ) options->setBool("ios_acl_basic",true);
        else options->setBool("ios_acl_no_clear",true);
    }

    Helper helper(NULL);

    char           timestr[256];
    time_t         tm;

    tm=time(NULL);
    strcpy(timestr,ctime(&tm));
    timestr[ strlen(timestr)-1 ]='\0';
    
#ifdef _WIN32
    char* user_name=getenv("USERNAME");
#else
    char* user_name=getenv("USER");
#endif
    if (user_name==NULL) 
        throw FWException("Can't figure out your user name, aborting");



    std::auto_ptr<OSConfigurator_ios> oscnf(new OSConfigurator_ios(objdb, fw, false));

    oscnf->prolog();
    oscnf->processFirewallOptions();





    list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);

    int policy_rules_count  = 0;

    vector<int> ipv4_6_runs;
    string generated_script;

    if (!single_rule_compile_on)
        generated_script = safetyNetInstall(fw);


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
            std::auto_ptr<Preprocessor> prep(new Preprocessor(objdb, fw, false));
            prep->compile();
        }

        for (list<FWObject*>::iterator p=all_policies.begin();
             p!=all_policies.end(); ++p )
        {
            Policy *policy = Policy::cast(*p);

            if (!policy->matchingAddressFamily(policy_af)) continue;

            PolicyCompiler_iosacl c(objdb, fw, ipv6_policy, oscnf.get());

            c.setSourceRuleSet( policy );
            c.setRuleSetName(policy->getName());

            c.setSingleRuleCompileMode(single_rule_id);
            if (inTestMode()) c.setTestMode();
            if (inEmbeddedMode()) c.setEmbeddedMode();
            c.setDebugLevel( dl );
            if (rule_debug_on) c.setDebugRule(  drp );
            c.setVerbose( verbose );

            if ( c.prolog() > 0 )
            {
                c.compile();
                c.epilog();

                if (!single_rule_compile_on)
                {
                    if (ipv6_policy)
                    {
                        generated_script += "\n\n";
                        generated_script += "! ================ IPv6\n";
                        generated_script += "\n\n";
                    } else
                    {
                        generated_script += "\n\n";
                        generated_script += "! ================ IPv4\n";
                        generated_script += "\n\n";
                    }
                }

                if (c.haveErrorsAndWarnings())
                {
                    all_errors.push_back(c.getErrors("").c_str());
                    // generated_script +=
                    //     "! Policy compiler errors and warnings:";
                    // generated_script += "\n";
                    // generated_script +=  c.getErrors("! ");
                }
                generated_script +=  c.getCompiledScript();

            } else
                info(" Nothing to compile in Policy");
        }

        if (!ipv6_policy)
        {
            list<FWObject*> all_routing = fw->getByType(Routing::TYPENAME);
            RuleSet *routing = RuleSet::cast(all_routing.front());

            // currently routing is supported only for ipv4
            RoutingCompiler_iosacl r(objdb, fw, false, oscnf.get());

            r.setSourceRuleSet(routing);
            r.setRuleSetName(routing->getName());

            r.setSingleRuleCompileMode(single_rule_id);
            if (inTestMode()) r.setTestMode();
            if (inEmbeddedMode()) r.setEmbeddedMode();
            r.setDebugLevel( dl );
            if (rule_debug_on) r.setDebugRule(  drp );
            r.setVerbose( verbose );
                
            if ( r.prolog() > 0 )
            {
                r.compile();
                r.epilog();

                if (r.haveErrorsAndWarnings())
                {
                    all_errors.push_back(r.getErrors("").c_str());
                    // generated_script +=
                    //     "! Routing compiler errors and warnings:";
                    // generated_script += "\n";
                    // generated_script += r.getErrors("! ");
                }

                generated_script += r.getCompiledScript();
            } else
                info(" Nothing to compile in Routing");
        }
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

    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);

    script << "!\n\
!  This is automatically generated file. DO NOT MODIFY !\n\
!\n\
!  Firewall Builder  fwb_iosacl v" << VERSION << "-" << BUILD_NUM << " \n\
!\n\
!  Generated " << timestr
          << " "
          << tzname[0]
          << " by " 
          << user_name;

    script << endl;

    script << "!" << endl;
    script << "!" << " Compiled for " << platform << " " << fwvers << endl;

    script << "!" << endl;
    script << "!" << MANIFEST_MARKER << "* " << ofname << endl;
    script << "!" << endl;

    script << prepend("! ", all_errors.join("\n")) << endl;

    script << endl;
    script << "!" << endl;
    script << "! Prolog script:" << endl;
    script << "!" << endl;

    string pre_hook= fw->getOptionsObject()->getStr("iosacl_prolog_script");
    script << pre_hook << endl;

    script << "!" << endl;
    script << "! End of prolog script:" << endl;
    script << "!" << endl;


    script << oscnf->getCompiledScript();
    script << endl;

    script << generated_script;

    script << endl;
        
    script << endl;
    script << "!" << endl;
    script << "! Epilog script:" << endl;
    script << "!" << endl;

    string post_hook= fw->getOptionsObject()->getStr("iosacl_epilog_script");
    script << post_hook << endl;

    script << endl;
    script << "! End of epilog script:" << endl;
    script << "!" << endl;



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
    } else
    {
        throw FWException(string(" Failed to open file ") +
                          fw_file_name.toStdString() +
                          " for writing");
    }

    return "";
}


