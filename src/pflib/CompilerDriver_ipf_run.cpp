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



using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

string CompilerDriver_ipf::run(const std::string &cluster_id,
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

    QFileInfo finfo(fw_file_name);
    QString ipf_file_name = finfo.completeBaseName() + "-ipf.conf";
    QString nat_file_name = finfo.completeBaseName() + "-nat.conf";
    if (finfo.path() != ".")
    {
        ipf_file_name = finfo.path() + "/" + ipf_file_name;
        nat_file_name = finfo.path() + "/" + nat_file_name;
    }

    QString remote_ipf_name = options->getStr("ipf_conf_file_name_on_firewall").c_str();
    if (remote_ipf_name.isEmpty()) remote_ipf_name = ipf_file_name;

    QString remote_nat_name = options->getStr("nat_conf_file_name_on_firewall").c_str();
    if (remote_nat_name.isEmpty()) remote_nat_name = nat_file_name;

    string s;

    string firewall_dir = options->getStr("firewall_dir");
    if (firewall_dir=="") firewall_dir = "/etc/fw";

    bool debug = options->getBool("debug");
    QString shell_dbg = (debug)?"-x":"" ;
    QString ipf_dbg = (debug)?"-v":"";

    std::auto_ptr<Preprocessor> prep(new Preprocessor(objdb , fw, false));
    prep->compile();

/*
 * Process firewall options, build OS network configuration script
 */
    std::auto_ptr<OSConfigurator> oscnf;
    string family=Resources::os_res[fw->getStr("host_OS")]->Resources::getResourceStr("/FWBuilderResources/Target/family");
    if ( family=="solaris" )
        oscnf = std::auto_ptr<OSConfigurator>(new OSConfigurator_solaris(objdb , fw, false));

    if ( family=="openbsd")
        oscnf = std::auto_ptr<OSConfigurator>(new OSConfigurator_openbsd(objdb , fw, false));

    if ( family=="freebsd")
        oscnf = std::auto_ptr<OSConfigurator>(new OSConfigurator_freebsd(objdb , fw, false));

    if (oscnf.get()==NULL)
        throw FWException("Unrecognized host OS " + 
                          fw->getStr("host_OS")+"  (family "+family+")");

    oscnf->prolog();

    list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);
    list<FWObject*> all_nat = fw->getByType(NAT::TYPENAME);

    PolicyCompiler_ipf c( objdb , fw, false , oscnf.get() );

    FWObject *policy = all_policies.front();

    c.setSourceRuleSet(Policy::cast(policy));
    c.setRuleSetName(policy->getName());

    c.setSingleRuleCompileMode(single_rule_id);
    c.setDebugLevel( dl );
    if (rule_debug_on) c.setDebugRule(  drp );
    c.setVerbose( verbose );
    if (inTestMode()) c.setTestMode();
    if (inEmbeddedMode()) c.setEmbeddedMode();

    bool have_ipf=false;
    if ( c.prolog() > 0 )
    {
        have_ipf = true;
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

    bool have_nat=false;
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

        if (have_ipf) 
        {
            if (c.haveErrorsAndWarnings())
            {
                all_errors.push_back(c.getErrors("").c_str());
                // ostr << "# Policy compiler errors and warnings:"
                //      << endl;
                // ostr << c.getErrors("# ");
            }
            ostr << c.getCompiledScript();
        }

        if (have_nat) 
        {
            if (n.haveErrorsAndWarnings())
            {
                all_errors.push_back(n.getErrors("").c_str());
                // ostr << "# NAT compiler errors and warnings:"
                //      << endl;
                // ostr << n.getErrors("# ");
            }
            ostr << n.getCompiledScript();
        }

        return
            all_errors.join("\n").toStdString() + 
            ostr.str();
    }


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

    QString activation_commands_buffer;
    QTextStream activation_commands(&activation_commands_buffer, QIODevice::WriteOnly);


    if (have_ipf) 
    {
        QFile ipf_file(ipf_file_name);
        if (ipf_file.open(QIODevice::WriteOnly))
        {
            QTextStream ipf_str(&ipf_file);
            if (c.haveErrorsAndWarnings())
            {
                all_errors.push_back(c.getErrors("").c_str());
                ipf_str << "# Policy compiler errors and warnings:"
                        << endl;
                ipf_str << c.getErrors("# ");
            }
            ipf_str << c.getCompiledScript();
            ipf_file.close();
            ipf_file.setPermissions(QFile::ReadOwner | QFile::WriteOwner |
                                    QFile::ReadGroup | QFile::ReadOther |
                                    QFile::ExeOwner | 
                                    QFile::ExeGroup |
                                    QFile::ExeOther );
        } else
        {
            throw FWException(string(" Failed to open file ") +
                              ipf_file_name.toStdString() +
                              " for writing");
        }

        QString cmd = QString("$IPF ") + ipf_dbg + " -I -f ";
        QString filePath;
        if (remote_ipf_name[0] == '/') filePath = remote_ipf_name;
        else filePath = QString("${FWDIR}/") + remote_ipf_name;

        if (fw->getOptionsObject()->getBool("dynAddr"))
        {
            cmd += "-";
            activation_commands << printActivationCommandWithSubstitution(fw, filePath, cmd);
        } else
        {
            activation_commands << cmd << filePath << endl;
        }
    }

    if (have_nat) 
    {
        QFile nat_file(nat_file_name);
        if (nat_file.open(QIODevice::WriteOnly))
        {
            QTextStream nat_str(&nat_file);
            if (n.haveErrorsAndWarnings())
            {
                all_errors.push_back(n.getErrors("").c_str());
                nat_str << "# NAT compiler errors and warnings:"
                        << endl;
                nat_str << n.getErrors("# ");
            }
            nat_str << n.getCompiledScript();
            nat_file.close();
            nat_file.setPermissions(QFile::ReadOwner | QFile::WriteOwner |
                                   QFile::ReadGroup | QFile::ReadOther |
                                   QFile::ExeOwner | 
                                   QFile::ExeGroup |
                                   QFile::ExeOther );
        } else
        {
            throw FWException(string(" Failed to open file ") +
                              nat_file_name.toStdString() +
                              " for writing");
        }

        QString cmd = QString("$IPNAT ") + ipf_dbg + " -f ";
        QString filePath;
        if (remote_nat_name[0] == '/') filePath = remote_nat_name;
        else filePath = QString("${FWDIR}/") + remote_nat_name;

        if (fw->getOptionsObject()->getBool("dynAddr"))
        {
            cmd += "-";
            activation_commands << printActivationCommandWithSubstitution(fw, filePath, cmd);
        } else
        {
            activation_commands << cmd << filePath << endl;
        }
    }

    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);

    script << "#!/bin/sh " << shell_dbg << endl << endl;
    script << "#\n\
#  This is automatically generated file. DO NOT MODIFY !\n\
#\n\
#  Firewall Builder  fwb_ipf v" << VERSION << "-" << BUILD_NUM << " \n\
#\n\
#  Generated " << timestr << " " << tzname[stm->tm_isdst] << " by "
            << user_name << "\n#\n#\n";

    info("Output file name: " + fw_file_name.toStdString());

    QFileInfo fw_file_info(fw_file_name);

    script << MANIFEST_MARKER << "* " << fw_file_info.fileName();
    string remote_name = fw->getOptionsObject()->getStr("script_name_on_firewall");
    if (!remote_name.empty()) script << " " << remote_name;
    script << endl;

    if (have_ipf) 
    {
        script << MANIFEST_MARKER << "  " << QFileInfo(ipf_file_name).fileName();
        if (remote_ipf_name != ipf_file_name) script << " " << remote_ipf_name;
        script << endl;
    }

    if (have_nat) 
    {
        script << MANIFEST_MARKER << "  " << QFileInfo(nat_file_name).fileName();
        if (remote_nat_name != nat_file_name) script << " " << remote_nat_name;
        script << endl;
    }

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

    script << "FWDIR=`dirname $0`" << endl << endl;

    script << oscnf->getCompiledScript();

    script << endl;

    script << "log '";
    script << "Activating firewall script generated "
            << timestr << " " << " by "
            << user_name;
    script << "'" << endl;

    script << endl;

    script << endl 
            << "$IPF -Fa" << endl
            << "$IPNAT -C" << endl;

/*
 *  we add prolog and epilog to the activation shell script rather
 *  than to ipf and nat .conf files. This is more flexible since user
 *  can execute some shell commands, as well as add any policy and/or
 *  nat rules by putting them into their .conf file and loading them
 *  from prolog or epilog script. Because of this, prolog is added
 *  after all policy and nat rules are flushed.
 */ 
    script << endl;
    script << "#" << endl;
    script << "# Prolog script" << endl;
    script << "#" << endl;
        
    string pre_hook= fw->getOptionsObject()->getStr("prolog_script");
    script << pre_hook << endl;

    script << "#" << endl;
    script << "# End of prolog script" << endl;
    script << "#" << endl;

    script << activation_commands_buffer.toStdString();

    if (have_ipf)
        script << "$IPF " << ipf_dbg << " -s " << endl;

    script << endl;
    script << "#" << endl;
    script << "# Epilog script" << endl;
    script << "#" << endl;

    string post_hook= fw->getOptionsObject()->getStr("epilog_script");
    script << post_hook << endl;

    script << endl;
    script << "# End of epilog script" << endl;
    script << "#" << endl;

    script << endl;

    script << "/sbin/kldstat -n ipl.ko > /dev/null 2>&1 || $IPF -E" << endl;
    script << endl;

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


