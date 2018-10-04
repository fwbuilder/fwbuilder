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
#include <string>
#include <cstring>
#include <iomanip>

#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"

#include "CompilerDriver_pf.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


CompilerDriver_pf::CompilerDriver_pf(FWObjectDatabase *db) :
    CompilerDriver(db)
{
    have_nat = false;
    have_filter = false;
}

// create a copy of itself, including objdb
CompilerDriver* CompilerDriver_pf::clone()
{
    CompilerDriver_pf* new_cd = new CompilerDriver_pf(objdb);
    if (inEmbeddedMode()) new_cd->setEmbeddedMode();
    return new_cd;
}

/*
 * Generate file name for the ruleset .conf file using general conf
 * file name as a prototype.
 */
QString CompilerDriver_pf::getConfFileNameForRuleset(const QString &ruleset_name,
                                                     const QString &conf_file_name,
                                                     const QString &ext)
{
    assert(!conf_file_name.isEmpty());

    if (ruleset_name == "__main__") return conf_file_name;

    QString suffix = QString("-") + ruleset_name;

    QFileInfo fi(conf_file_name);
    QString path = fi.path();

    // qDebug() << "getConfFileNameForRuleset:"
    //          << "conf_file_name=" << conf_file_name
    //          << "path=" << path;

    QString using_suffix = fi.completeSuffix();
    if (!ext.isEmpty()) using_suffix = ext;

    QString new_name = fi.completeBaseName() + suffix + "." + using_suffix;
    if (!path.isEmpty() && path != ".") new_name = path + "/" + new_name;
    return new_name;
}

QString CompilerDriver_pf::getRemoteConfFileName(const QString &ruleset_name,
                                                 const QString &local_conf_name,
                                                 const QString &remote_fw_name,
                                                 const QString &remote_conf_name)
{
    QString conf_file_name;
    QString suffix = QString("-") + ruleset_name;
    if (ruleset_name == "__main__") suffix = "";

    if (remote_conf_name.isEmpty() && remote_fw_name.isEmpty())
    {
        // local_conf_name may be a relative or absolute path. Return
        // just the file name
        QFileInfo fi(local_conf_name);
        return fi.fileName();
    }

    QFileInfo fi;

    if (!remote_conf_name.isEmpty()) fi = QFileInfo(remote_conf_name);
    else
        if (!remote_fw_name.isEmpty()) fi = QFileInfo(remote_fw_name);

    QString new_name = fi.completeBaseName() + suffix + ".conf";
    QString path = fi.path();

    if (path == ".") return new_name;
    else return path + "/" + new_name;
}

string CompilerDriver_pf::printTimeout(FWOptions* options,
                                       const string &OnOffOption,
                                       const string &ValOption,
                                       const string &pfCode)
{
    std::ostringstream res;
    if (options->getBool(OnOffOption) && options->getInt(ValOption)>0)
    {
        res << "set timeout "
            << pfCode << " " << options->getInt(ValOption) << endl;
    }
    return res.str();
}

void CompilerDriver_pf::printProlog(QTextStream &file, const string &prolog_code)
{
    file << endl;
    file << "#" << endl;
    file << "# Prolog script" << endl;
    file << "#" << endl;
    file << prolog_code << endl;
    file << "#" << endl;
    file << "# End of prolog script" << endl;
    file << "#" << endl;
}

void CompilerDriver_pf::printStaticOptions(QTextStream &file, Firewall* fw)
{
    FWOptions* options = fw->getOptionsObject();
    list<FWObject*> all_interfaces=fw->getByType(Interface::TYPENAME);
    string prolog_place = options->getStr("prolog_place");
    if (prolog_place.empty()) prolog_place = "fw_file";  // old default
    string pre_hook = options->getStr("prolog_script");

    if (prolog_place == "pf_file_top")
        printProlog(file, pre_hook);

    file << endl;

    string set_debug = options->getStr("pf_set_debug");
    if (!set_debug.empty())
    {
        file << "set debug " << set_debug << endl;
    }

    string state_policy = options->getStr("pf_state_policy");
    if (!state_policy.empty())
    {
        file << "set state-policy " << state_policy << endl;
    }

    string block_policy = options->getStr("pf_block_policy");
    if (!block_policy.empty())
    {
        file << "set block-policy " << block_policy << endl;
    }

    QStringList limits;
    if (options->getBool("pf_do_limit_frags") &&
        options->getInt("pf_limit_frags")>0 )
        limits.push_back(QString("frags ") +
                         options->getStr("pf_limit_frags").c_str());

    if (options->getBool("pf_do_limit_states") &&
        options->getInt("pf_limit_states")>0 )
        limits.push_back(QString("states ") +
                         options->getStr("pf_limit_states").c_str());

    if (options->getBool("pf_do_limit_src_nodes") &&
        options->getInt("pf_limit_src_nodes")>0 )
        limits.push_back(QString("src-nodes ") +
                         options->getStr("pf_limit_src_nodes").c_str());

    if (options->getBool("pf_do_limit_tables") &&
        options->getInt("pf_limit_tables")>0 )
        limits.push_back(QString("tables ") +
                         options->getStr("pf_limit_tables").c_str());

    if (options->getBool("pf_do_limit_table_entries") &&
        options->getInt("pf_limit_table_entries")>0 )
        limits.push_back(QString("table-entries ") +
                         options->getStr("pf_limit_table_entries").c_str());

    if (limits.size() > 0)
    {
        file << "set limit ";
        if (limits.size() > 1 ) file << "{ ";
        file << limits.join(", ");
        if (limits.size() > 1 ) file << " }";
        file << endl;
    }

    if ( ! options->getStr("pf_optimization").empty() )
        file << "set optimization "
             << options->getStr("pf_optimization") << endl;

    file << printTimeout(options,
                         "pf_do_timeout_interval","pf_timeout_interval",
                         "interval");
    file << printTimeout(options,
                         "pf_do_timeout_frag","pf_timeout_frag",
                         "frag");

    file << printTimeout(options,
                         "pf_set_tcp_first","pf_tcp_first",
                         "tcp.first" );
    file << printTimeout(options,
                         "pf_set_tcp_opening","pf_tcp_opening",
                         "tcp.opening" );
    file << printTimeout(options,
                         "pf_set_tcp_established","pf_tcp_established",
                         "tcp.established" );
    file << printTimeout(options,
                         "pf_set_tcp_closing","pf_tcp_closing",
                         "tcp.closing" );
    file << printTimeout(options,
                         "pf_set_tcp_finwait","pf_tcp_finwait",
                         "tcp.finwait" );
    file << printTimeout(options,
                         "pf_set_tcp_closed","pf_tcp_closed",
                         "tcp.closed" );
    file << printTimeout(options,
                         "pf_set_udp_first","pf_udp_first",
                         "udp.first" );
    file << printTimeout(options,
                         "pf_set_udp_single","pf_udp_single",
                         "udp.single" );
    file << printTimeout(options,
                         "pf_set_udp_multiple","pf_udp_multiple",
                         "udp.multiple" );
    file << printTimeout(options,
                         "pf_set_icmp_first","pf_icmp_first",
                         "icmp.first" );
    file << printTimeout(options,
                         "pf_set_icmp_error","pf_icmp_error",
                         "icmp.error" );
    file << printTimeout(options,
                         "pf_set_other_first","pf_other_first",
                         "other.first" );
    file << printTimeout(options,
                         "pf_set_other_single","pf_other_single",
                         "other.single" );
    file << printTimeout(options,
                         "pf_set_other_multiple","pf_other_multiple",
                         "other.multiple" );

    file << printTimeout(options,
                         "pf_set_adaptive","pf_adaptive_start",
                         "adaptive.start" );
    file << printTimeout(options,
                         "pf_set_adaptive","pf_adaptive_end",
                         "adaptive.end");

    // check if any interface is marked as 'unprotected'
    // and generate 'set skip on <ifspec>' commands

    if (fw->getStr("version")=="ge_3.7" ||
//        fw->getStr("version")=="4.x") 
        XMLTools::version_compare(fw->getStr("version"), "4.0")>=0)
    {
        for (list<FWObject*>::iterator i=all_interfaces.begin();
             i!=all_interfaces.end(); ++i) 
        {
            Interface *iface=dynamic_cast<Interface*>(*i);
            assert(iface);

            if ( iface->isUnprotected())  
                file << "set skip on " << iface->getName() << endl;
        }
    }

    file << endl;

    if (prolog_place == "pf_file_after_set")
        printProlog(file, pre_hook);

    QStringList scrub_options;

    string scrub_rule_direction = "in ";

    if (options->getBool("pf_do_scrub"))
    {
        if (XMLTools::version_compare(fw->getStr("version"), "4.6")<0)
        {
            if (options->getBool("pf_scrub_reassemble"))     
                scrub_options << "fragment reassemble";
            if (options->getBool("pf_scrub_fragm_crop"))
                scrub_options << "fragment crop";
            if (options->getBool("pf_scrub_fragm_drop_ovl"))
                scrub_options << "fragment drop-ovl";
        }
        if (options->getBool("pf_scrub_reassemble_tcp"))
        {
            // "scrub all reassemble tcp" - does not allow direction
            scrub_options << "reassemble tcp";
            scrub_rule_direction = "";
        }
    }

    if (options->getBool("pf_scrub_no_df"))  scrub_options << "no-df ";

    if (!scrub_options.empty())
    {
        file << "#" << endl;
        file << "# Scrub rules" << endl;
        file << "#" << endl;

        if (XMLTools::version_compare(fw->getStr("version"), "4.6")>=0)
        {
            file << "match "
                 << scrub_rule_direction
                 << "all scrub ("
                 << scrub_options.join(" ").toStdString() << ")"
                 << endl;
        } else
        {
            file << "scrub "
                 << scrub_rule_direction
                 << "all "
                 << scrub_options.join(" ").toStdString()
                 << endl;
        }
    }

    scrub_options.clear();

    if (options->getBool("pf_scrub_random_id"))
        scrub_options << "random-id";

    if (options->getBool("pf_scrub_use_minttl"))
        scrub_options << "min-ttl " << options->getStr("pf_scrub_minttl").c_str();

    if (options->getBool("pf_scrub_use_maxmss"))
        scrub_options << "max-mss " << options->getStr("pf_scrub_maxmss").c_str();

    if (!scrub_options.empty())
    {
        if (XMLTools::version_compare(fw->getStr("version"), "4.6")>=0)
        {
            file << "match out all scrub ("
                 << scrub_options.join(" ").toStdString() << ")" << endl;
        } else
        {
            file << "scrub out all "
                 << scrub_options.join(" ").toStdString() << endl;
        }
    }

    file << endl;

    if (prolog_place == "pf_file_after_scrub")
        printProlog(file, pre_hook);

    //file << table_factory->PrintTables();
    //file << endl;

    //if (prolog_place == "pf_file_after_tables")
    //    printProlog(file, pre_hook);

}

void CompilerDriver_pf::setToolPathVar(Firewall* fw,
                                       const string &os,
                                       const string &var_path_suffix,
                                       OSData_pf::tools osdata_tool_type,
                                       Configlet *configlet)
{
    OSData_pf os_data;
    FWOptions* options = fw->getOptionsObject();
    string s;
    string path;
    s = options->getStr(os + "_" + var_path_suffix);
    if (!s.empty()) path = s;
    else            path = os_data.getPathForTool(os, osdata_tool_type);
    configlet->setVariable(var_path_suffix.c_str(), path.c_str());
}

QString CompilerDriver_pf::printPathForAllTools(Firewall* fw, const string &os)
{
    Configlet tools = Configlet(fw, "bsd", "tools");
    tools.removeComments();

    setToolPathVar(fw, os, "path_ifconfig", OSData_pf::IFCONFIG, &tools);
    setToolPathVar(fw, os, "path_ipf", OSData_pf::IPF, &tools);
    setToolPathVar(fw, os, "path_ipnat", OSData_pf::IPNAT, &tools);
    setToolPathVar(fw, os, "path_ipfw", OSData_pf::IPFW, &tools);
    setToolPathVar(fw, os, "path_pfctl", OSData_pf::PFCTL, &tools);
    setToolPathVar(fw, os, "path_sysctl", OSData_pf::SYSCTL, &tools);
    setToolPathVar(fw, os, "path_logger", OSData_pf::LOGGER, &tools);
    return tools.expand();
}

