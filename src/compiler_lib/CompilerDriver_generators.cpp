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
#include <iomanip>

#include "CompilerDriver.h"
#include "Configlet.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Resources.h"

#include "fwcompiler/OSConfigurator.h"

#include <QStringList>
#include <QFileInfo>

#ifdef _WIN64
#define tzname _tzname
#endif

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;

extern QString user_name;


QString CompilerDriver::printPathForAllTools(Firewall*, const std::string &)
{
    return "";
}

QString CompilerDriver::printActivationCommands(Firewall*)
{
    return "";
}

QString CompilerDriver::assembleManifest(Cluster*, Firewall*, bool)
{
    return "";
}

void CompilerDriver::assembleFwScriptInternal(Cluster *cluster,
                                              Firewall* fw,
                                              bool cluster_member,
                                              OSConfigurator *oscnf,
                                              Configlet *script_skeleton,
                                              Configlet *top_comment,
                                              const QString &comment_char,
                                              bool indent)
{
    FWOptions* options = fw->getOptionsObject();
    string platform = fw->getStr("platform");
    string fw_version = fw->getStr("version");
    string host_os = fw->getStr("host_OS");
    string family = Resources::os_res[host_os]->Resources::getResourceStr(
        "/FWBuilderResources/Target/family");
    bool debug = options->getBool("debug");
    string shell_dbg = (debug)?"set -x":"" ;
    string cmd_dbg = (debug)?"-v ":"";
    string prolog_place = options->getStr("prolog_place");
    if (prolog_place.empty()) prolog_place = "fw_file";  // old default
    string pre_hook = fw->getOptionsObject()->getStr("prolog_script");
    string firewall_dir = options->getStr("firewall_dir");
    if (firewall_dir=="") firewall_dir = "/etc/fw";

    char *timestr;
    time_t tm;
    struct tm *stm;

    tm = time(nullptr);
    stm = localtime(&tm);
    timestr = strdup(ctime(&tm));
    timestr[strlen(timestr)-1] = '\0';

    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);

    script_skeleton->removeComments();
    script_skeleton->setVariable("shell_debug", shell_dbg.c_str());
    script_skeleton->setVariable("firewall_dir", firewall_dir.c_str());

    top_comment->setVariable("version", VERSION);
 
    top_comment->setVariable("timestamp", timestr);
    top_comment->setVariable("tz", tzname[stm->tm_isdst]);
    top_comment->setVariable("user", user_name);

    //QFileInfo fw_file_info(fw_file_name);

    top_comment->setVariable("manifest", assembleManifest(cluster, fw,
                                                          cluster_member));
    top_comment->setVariable("platform", platform.c_str());
    top_comment->setVariable("fw_version", fw_version.c_str());
    top_comment->setVariable(
        "comment", prepend(comment_char +  " ", fw->getComment().c_str()));

    script_skeleton->setVariable("have_nat", have_nat);
    script_skeleton->setVariable("have_filter", have_filter);

    script_skeleton->setVariable("top_comment", top_comment->expand());
    script_skeleton->setVariable(
        "errors_and_warnings", prepend(comment_char + " ", all_errors.join("\n")));

    script_skeleton->setVariable("tools", printPathForAllTools(fw, family));

    script_skeleton->setVariable("timestamp", timestr);
    script_skeleton->setVariable("user", user_name);
    if (prolog_place == "fw_file")
        script_skeleton->setVariable("prolog_script", pre_hook.c_str());
    else
        script_skeleton->setVariable("prolog_script", "");

    script_buffer = "";

    script_skeleton->setVariable("shell_functions",
                                 oscnf->printFunctions().c_str());
    script_skeleton->setVariable("kernel_vars_commands",
                                 prepend((indent) ? "    " : "",
                                        oscnf->printKernelVarsCommands().c_str()));
    script_skeleton->setVariable("configure_interfaces",
                                 prepend((indent) ? "    " : "",
                                        oscnf->configureInterfaces().c_str()));

    // this really adds nothing for the most of the systems
    script_skeleton->setVariable("other_os_configuration_commands",
                                 oscnf->getCompiledScript().c_str());

    script_skeleton->setVariable("activation_commands",
                                 printActivationCommands(fw));

    script_skeleton->setVariable("verify_interfaces", "");

    script_skeleton->setVariable("epilog_script",
                                fw->getOptionsObject()->getStr("epilog_script").c_str());
}

