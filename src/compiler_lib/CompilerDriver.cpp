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
#include <set>
#include <algorithm>
#include <functional>
#include <memory>

// for chdir
#ifndef _WIN32
#  include <unistd.h>
#else
#  include <direct.h>
#  include <stdlib.h>
#  include <io.h>
#endif

#include "CompilerDriver.h"
#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include "fwbuilder/Cluster.h"
#include "fwbuilder/ClusterGroup.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FailoverClusterGroup.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/StateSyncClusterGroup.h"

#include "fwcompiler/Compiler.h"

#include <QStringList>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


CompilerDriver::CompilerDriver(FWObjectDatabase *db) : BaseCompiler()
{
    fwbdebug = 0;
    filename = "";
    wdir = "";
    fwobjectname = "";
    single_rule_compile_on = false;
    prepend_cluster_name_to_output_file = false;
    dl = 0;
    rule_debug_on = false;
    drp = -1;
    drn = -1;
    drr = -1;
    verbose = 0;
    have_dynamic_interfaces = false;
    ipv4_run = true;
    ipv6_run = true;
    fw_by_id = false;

    objdb = new FWObjectDatabase(*db);
    objdb->setIgnoreReadOnlyFlag(true);

    //objdb = db;

    persistent_objects = new Library();
    persistent_objects->setName("Persistent Objects");
    objdb->add(persistent_objects);

    workspace = new Library();
    workspace->setName("Workspace");
    objdb->add(workspace);

    prolog_done = false;
    epilog_done = false;
    have_filter = false;
    have_nat = false;
    start_current_dir = QDir::current();
}

CompilerDriver::~CompilerDriver()
{
    if (persistent_objects->getParent() == nullptr)
        delete persistent_objects;
    else
    {
        if (persistent_objects->getParent() == objdb)
        {
            objdb->remove(persistent_objects, false);
            delete persistent_objects;
        }
    }
    
    if (workspace->getParent() == nullptr)
        delete workspace;
    else
    {
        if (workspace->getParent() == objdb)
        {
            objdb->remove(workspace, false);
            delete workspace;
        }
    }

    delete objdb;
}

// create a copy of itself, including objdb
CompilerDriver* CompilerDriver::clone()
{
    CompilerDriver* new_cd = new CompilerDriver(objdb);
    if (inEmbeddedMode()) new_cd->setEmbeddedMode();
    return new_cd;
}

bool CompilerDriver::configure(const QStringList &args)
{
    QString last_arg;
    for (int idx=0; idx < args.size(); idx++)
    {
        QString arg = args.at(idx);

        last_arg = arg;
        if (arg == "-i")
        {
            fw_by_id = true;
            continue;
        }
        if (arg == "-v")
        {
            verbose++;
            continue;
        }

        if (arg == "-4")
        {
            ipv4_run = true;
            ipv6_run = false;
            continue;
        }
        if (arg == "-6")
        {
            ipv4_run = false;
            ipv6_run = true;
            continue;
        }
        if (arg == "-d")
        {
            // TODO: deal with UTF-8 in directory name
            idx++;
            wdir = string(args.at(idx).toLatin1().constData());
            continue;
        }
        if (arg == "-D")
        {
            idx++;
            FWObject::setDataDir(args.at(idx).toUtf8().constData());
            continue;
        }
        if (arg == "-f")
        {
            idx++;
            filename = string(args.at(idx).toLatin1().constData());
            continue;
        }
        if (arg == "-o")
        {
            idx++;
            file_name_setting_from_command_line = args.at(idx);
            continue;
        }
        if (arg == "-O")
        {
            // parameter is ',' separated list of <member fw object ID>,
            // <corresponding output file name>
            // All separated by commands, the id and file name just
            // follow one after another.
            idx++;
            QString member_files = args.at(idx);
            QStringList mf_list = member_files.split(",");
            QStringListIterator it(mf_list);
            while (it.hasNext())
            {
                QString fw_id = it.next();
                if (it.hasNext())
                {
                    QString file_name = it.next();
                    member_file_names[fw_id] = file_name;
                } else
                {
                    QString err("Misconfigured -O option, missing file "
                                "name component for ID %1");
                    abort(err.arg(fw_id).toStdString());
                }
            }
            continue;
        }

        if (arg == "-xc")
        {
            prepend_cluster_name_to_output_file = true;
            continue;
        }

        if (arg == "-xt")
        {
            setTestMode();
            info("*** Running in test mode, fatal errors are treated as warnings");
            continue;
        }

        if (arg == "-xp")
        {
            idx++;
            bool ok = false;
            drp = args.at(idx).toInt(&ok);
            if (!ok) return false;
            rule_debug_on = true;
            continue;
        }

        if (arg == "-xn")
        {
            idx++;
            bool ok = false;
            drn = args.at(idx).toInt(&ok);
            if (!ok) return false;
            rule_debug_on = true;
            continue;
        }

        if (arg == "-xr")
        {
            idx++;
            bool ok = false;
            drr = args.at(idx).toInt(&ok);
            if (!ok) return false;
            rule_debug_on = true;
            continue;
        }

        if (arg == "-s")
        {
            idx++;
            single_rule_id = args.at(idx).toStdString();
            single_rule_compile_on = true;
            continue;
        }
    }

    fwobjectname = last_arg;

    if (wdir.empty()) wdir="./";

    return true;
}

void CompilerDriver::chDir()
{
    if (
#ifdef _WIN32
        _chdir(wdir.c_str())
#else
        chdir(wdir.c_str())
#endif
    ) {
	cerr << "Can't change to: " << wdir << endl;
	exit(1);
    }
}

/*
 * See #1994. We need to reset read-only flag on the firewall and up
 * the tree all the way to the root in order to let compilers make any
 * modifications they need. Note that this resets read-only flags in
 * the copy of the database this class works with.
 */
void CompilerDriver::clearReadOnly(Firewall *fw)
{
    if (fw->isReadOnly())
    {
        FWObject *p = fw;
        while (p)
        {
            p->setReadOnly(false);
            p = p->getParent();
        }
    }
}
          

QString CompilerDriver::getAbsOutputFileName(const QString &output_file_name)
{
    QFileInfo finfo(output_file_name);
    if (finfo.isRelative())
    {
        // if fw_file_name is relative, it is relative to the
        // directory the program started in, or if wdir was defined
        // via "-d" command line switch, then it is relative to that.
        if (wdir.empty())
        {
            QFileInfo new_finfo(start_current_dir, output_file_name);
            return new_finfo.absoluteFilePath();
        } else
        {
            QFileInfo new_finfo(QDir(wdir.c_str()), output_file_name);
            return new_finfo.absoluteFilePath();
        }
    }
    return output_file_name;
}

void CompilerDriver::commonChecks(Firewall *fw)
{
    if (Cluster::isA(fw))
    {
        Cluster *cluster = Cluster::cast(fw);

        // Check #1 : make sure output file names are different in member
        // firewalls
        set<string> output_file_names;
        list<Firewall*> members;
        cluster->getMembersList(members);
        for (list<Firewall*>::iterator it=members.begin(); it!=members.end(); ++it)
        {
            FWOptions *fwopt = (*it)->getOptionsObject();
            string ofname = fwopt->getStr("output_file");
            if (ofname.empty()) continue;
            if (output_file_names.count(ofname) > 0)
            {
                QString err("Member firewalls use the same output file name %1");
                error(cluster, nullptr, nullptr, err.arg(ofname.c_str()).toStdString());
            }
            output_file_names.insert(ofname);
        }
    }
}

/*
 * This method performs series of checks for the configuration
 * consitency of clusters and cluster members as well as common
 * problems with interfaces, addresses and their combinations. There
 * are several possible levels of errors:
 *
 *  - errors that can be worked around. Compiler makes minor changes
 * to objects and continues. These are not warnings though, the user
 * should fix these problems. Using Compiler::error() to report.
 *
 * - serious errors that should stop processing because generated file
 * will be incorrect or inconsistent. However it is possible to
 * continue in single rule compile mode because the error may not
 * affect the rule being compiled. Using Compiler::abort() to
 * report. Normally this method throws FWException() but in single
 * rule compile mode or in testing mode it records the error and
 * continues.
 *
 * - fatal errors that make it impossible to continue even in test or
 * single rule compile modes. To report call Compiler::abort() and
 * then throw FatalErrorInSingleRuleCompileMode exception. This
 * exception should be caught in CompilerDriver::run() (virtual
 * method) where recorded error can be shown to the user in the GUI.
 *
 */
void CompilerDriver::commonChecks2(Cluster *cluster, Firewall *fw)
{
    QString current_firewall_name = fw->getName().c_str();
    string host_os = fw->getStr("host_OS");

    if (cluster)
    {
        // firewall is a member of a cluster.
        // Rely on the caller to make sure this firewall is really a member
        // of this cluster. Do not perform redundant check here.

        processStateSyncGroups(cluster, fw);

        // some initial sanity checks
        validateClusterGroups(cluster);
    }


    list<FWObject*> all_policies = fw->getByType(Policy::TYPENAME);
    list<FWObject*> all_nat = fw->getByType(NAT::TYPENAME);

    bool have_top = false;
    for (list<FWObject*>::iterator p=all_nat.begin(); p!=all_nat.end(); ++p)
    {
        if (RuleSet::cast(*p)->isTop())
        {
            have_top = true;
            break;
        }
    }
    if ( ! have_top )
        warning(fw, nullptr, nullptr,"Missing top level NAT ruleset");

    have_top = false;
    for (list<FWObject*>::iterator p=all_policies.begin(); p!=all_policies.end(); ++p)
    {
        if (RuleSet::cast(*p)->isTop())
        {
            have_top = true;
            break;
        }
    }
    if ( ! have_top )
        warning(fw, nullptr, nullptr,"Missing top level Policy ruleset");


    list<FWObject*> interfaces = fw->getByTypeDeep(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=interfaces.begin(); i!=interfaces.end(); ++i)
    {
        Interface *iface = Interface::cast(*i);
        assert(iface);

        string::size_type n;
        if ( (n=iface->getName().find("*"))!=string::npos) 
        {
/* this is a special 'wildcard' interface. Its name must end with '*',
 * it must be dynamic and should not have a child IPv4 or
 * physAddress object
 */
            if (n!=iface->getName().length()-1)
            {
                QString err("'*' must be the last character in "
                            "the wildcard's interface name: '%1'.");
                abort(fw, nullptr, nullptr,
                      err.arg(iface->getName().c_str()).toStdString());
                throw FatalErrorInSingleRuleCompileMode();
            }
/*
  removed test to implement RFE #837238: "unnummbered wildcard interfaces"

  if (!iface->isDyn())
  {
  QString errstr;
  errstr.sprintf(_("Wildcard interface '%s' must be dynamic."),
  iface->getName().c_str() );
  throw FWException(errstr);
  }
*/
            list<FWObject*> l3=iface->getByType(physAddress::TYPENAME);
            if (l3.size()>0)
            {
                QString err("Wildcard interface '%1' should not have "
                            "physcal address object attached to it. "
                            "The physical address object will be ignored.");
                error(fw, nullptr, nullptr,
                      err.arg(iface->getName().c_str()).toStdString());
                for (list<FWObject*>::iterator j=l3.begin(); j!=l3.end(); ++j) 
                    iface->remove(*j);
            }
        }

        if (iface->isDyn())  
        {
            have_dynamic_interfaces=true;

            iface->setBool("use_var_address",true);

            list<FWObject*> l3=iface->getByType(IPv4::TYPENAME);
            if (l3.size()>0)
            {
                for (list<FWObject*>::iterator j=l3.begin(); j!=l3.end(); ++j) 
                    if ( objdb->findAllReferences(*j).size()!=0 )
                    {
                        QString err("Dynamic interface %1 has IP address "
                                    "that is used in the firewall policy rule.");
                        abort(fw, nullptr, nullptr,
                              err.arg(iface->getName().c_str()).toStdString());
                        throw FatalErrorInSingleRuleCompileMode();
                    }

                QString err("Dynamic interface %1 should not have an "
                            "IP address object attached to it. "
                            "This IP address object will be ignored.");
                error(fw, nullptr, nullptr,
                      err.arg(iface->getName().c_str()).toStdString());
                for (list<FWObject*>::iterator j=l3.begin(); j!=l3.end(); ++j) 
                    iface->remove(*j);
            }
        } 

        if (iface->isRegular())
        {
            // Regular interface (should have an ip address)
            bool no_addr_ok = false;
            if (iface->getOptionsObject()->getBool("cluster_interface"))
            {
                // cluster interface with failover type heartbeat or
                // openais may have no ip address. Other failover
                // types require an address.
                FWObject *failover_group =
                    iface->getFirstByType(FailoverClusterGroup::TYPENAME);
                if (failover_group)
                {
                    string failover_type = failover_group->getStr("type");
                    no_addr_ok = Resources::os_res[host_os]->getResourceBool(
                    "/FWBuilderResources/Target/protocols/" + failover_type +
                    "/no_ip_ok");
                }
            }

            list<FWObject*> all_addr = iface->getByType(IPv4::TYPENAME);
            list<FWObject*> all_ipv6 = iface->getByType(IPv6::TYPENAME);
            all_addr.insert(all_addr.begin(), all_ipv6.begin(), all_ipv6.end());

            if (iface->isRegular() &&
                !no_addr_ok &&
                all_addr.empty() &&
                all_ipv6.empty())
            {
                QString err("Missing IP address for interface %1");
                abort(fw, nullptr, nullptr,
                      err.arg(iface->getName().c_str()).toStdString());
                throw FatalErrorInSingleRuleCompileMode();
            }

            for (list<FWObject*>::iterator j = all_addr.begin();
                 j != all_addr.end(); ++j) 
            {
                const InetAddr *ip_addr = Address::cast(*j)->getAddressPtr();
                const InetAddr *netmask = Address::cast(*j)->getNetmaskPtr();

                if (ip_addr && ip_addr->isAny())
                {
                    QString err("Interface %1 (id=%2) has IP address %3.");
                    abort(fw, nullptr, nullptr,
                          err.arg(iface->getName().c_str())
                          .arg(FWObjectDatabase::getStringId(
                                   iface->getId()).c_str())
                          .arg(ip_addr->toString().c_str()).toStdString());
                    throw FatalErrorInSingleRuleCompileMode();
                }

                if (ip_addr && netmask && netmask->isAny())
                {
                    QString err("Interface %1 (id=%2) has invalid netmask %3.");
                    abort(fw, nullptr, nullptr,
                          err.arg(iface->getName().c_str())
                          .arg(FWObjectDatabase::getStringId(
                                   iface->getId()).c_str())
                          .arg(netmask->toString().c_str()).toStdString());
                    throw FatalErrorInSingleRuleCompileMode();
                }
            }
        }

        FWObject *parent = iface->getParent();

        if (Interface::isA(parent))
        {
            Resources* os_res = Resources::os_res[fw->getStr("host_OS")];
            string os_family = fw->getStr("host_OS");
            if (os_res!=nullptr)
                os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

            std::unique_ptr<interfaceProperties> int_prop(
                interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                    os_family));

#if 0
            // See #2103. All interface name validation checks should
            // be done in the GUI.
            QString err;
            if (!int_prop->validateInterface(parent, iface, true, err))
            {
                abort(fw, nullptr, nullptr, err.toStdString());
                throw FatalErrorInSingleRuleCompileMode();
            }
#endif


            string interface_type = iface->getOptionsObject()->getStr("type");
            if (interface_type.empty()) interface_type = "ethernet";

            string parent_interface_type =
                Interface::cast(parent)->getOptionsObject()->getStr("type");

            if (parent_interface_type == "bridge" &&
                interface_type == "ethernet" &&
                int_prop->looksLikeVlanInterface(iface->getName().c_str()))
            {
                // if vlan interface is used as a bridge port, it
                // should be a copy of the top-level interface object
                // with the same name
                bool have_top_level_copy = false;
                for (list<FWObject*>::iterator i2=interfaces.begin();
                     i2!=interfaces.end(); ++i2)
                {
                    Interface *in = Interface::cast(*i2);
                    assert(in);
                    if (in == iface) continue;
                    if (in->getName() == iface->getName())
                    {
                        have_top_level_copy = true;
                        break;
                    }
                }
                if (!have_top_level_copy)
                {
                    QString err("Interface %1 looks like Vlan interface and is "
                                "used as a bridge port. This configuration "
                                "is only allowed if this object is a copy of another "
                                "top-level interface with the same name"
                    );
                    abort(fw, nullptr, nullptr,
                          err.arg(iface->getName().c_str()).toStdString());
                    throw FatalErrorInSingleRuleCompileMode();
                }
            }
        }
    }
}

void CompilerDriver::setTargetId(const string &id)
{
    fw_by_id = true;
    fwobjectname = id.c_str();
}

Firewall* CompilerDriver::locateObject()
{
    Firewall* obj;
    if (fw_by_id)
    {
        // fwobjectname is actually object id
        obj = Firewall::cast(
            objdb->findInIndex(
                objdb->getIntId(fwobjectname.toLatin1().constData())));
        //fwobjectname = obj->getName().c_str();
    }
    else
        obj = objdb->findFirewallByName(fwobjectname.toUtf8().constData());

    return obj;
}

/* Find rulesets that belong to other firewall objects but are
 * referenced by rules of this firewall using action Branch.
 *
 * Important: rulesets that belong to other firewalls may be marked as
 * "top rulesets", which means they should be translated into the
 * built-in chains INPUT/OUTPUT/FORWARD rather then into named chain
 * with the name the same as the name of the ruleset. However this
 * does not make sense if we want to jump to that ruleset from a rule
 * from a ruleset that belongs to the firewall we are compiling. If we
 * compile such "foreighn" ruleset as "top ruleset", then we do not
 * create chain we would jump to. To avoid this will reset "top
 * ruleset" flag of rulesets of other firewalls referenced by
 * branching rules of the firewall being compiled.
 */
void CompilerDriver::findImportedRuleSets(Firewall *fw,
                                          list<FWObject*> &all_policies)
{
    bool cluster_member = fw->getOptionsObject()->getBool("cluster_member");
    int cluster_id = fw->getInt("parent_cluster_id");

    list<FWObject*> imported_policies;
    for (list<FWObject*>::iterator i=all_policies.begin(); i!=all_policies.end(); ++i)
    {
        RuleSet *ruleset = RuleSet::cast(*i);
        if (ruleset == nullptr) continue; // should not happen

        for (list<FWObject*>::iterator r=ruleset->begin(); r!=ruleset->end(); ++r)
        {
            Rule *rule = Rule::cast(*r);
            if (rule == nullptr) continue; // skip RuleSetOptions object

            RuleSet *branch_ruleset = rule->getBranch();
            if (branch_ruleset!=nullptr)
            {
                // qDebug() << "ruleset=" << ruleset->getName().c_str()
                //          << "branch=" << branch_ruleset->getName().c_str();

                map<FWObject*, int> referenced_branch_rulesets;

                _findImportedRuleSetsRecursively(
                    fw, branch_ruleset, referenced_branch_rulesets);

                map<FWObject*, int>::iterator it;
                for (it=referenced_branch_rulesets.begin();
                     it!=referenced_branch_rulesets.end(); ++it)
                {
                    RuleSet *branch_ruleset = RuleSet::cast(it->first);
                    int counter = it->second;

                    // qDebug() << "    " 
                    //          << "branch=" << branch_ruleset->getName().c_str()
                    //          << "counter=" << counter;

                    if (counter > 1)
                    {
                        QString err(
                            "Rule branches to rule set %1 which branches "
                            "back to it, creating a loop");
                        warning(ruleset->getParent(), ruleset, rule,
                                err.arg(branch_ruleset->getName().c_str())
                                .toStdString());
                    }

                    if (branch_ruleset->isChildOf(fw)) continue;

                    list<FWObject*>::iterator it =
                        std::find(
                            imported_policies.begin(),
                            imported_policies.end(),
                            branch_ruleset);

                    if (it != imported_policies.end()) continue;

                    // Additional check: the rule set may be child of a
                    // cluster this firewall is member of. If it is, it
                    // has been taken care of in CompilerDriver::mergeRuleSets()
                    FWObject *ruleset_parent = branch_ruleset->getParent();
                    if (cluster_member && Cluster::isA(ruleset_parent) &&
                        ruleset_parent->getId() == cluster_id) continue;

                    branch_ruleset->setTop(false);
                    imported_policies.push_back(branch_ruleset);
                }
            }
        }
    }

    if (imported_policies.size() > 0)
        all_policies.insert(all_policies.end(),
                            imported_policies.begin(), imported_policies.end());
}

void CompilerDriver::_findImportedRuleSetsRecursively(
    Firewall *fw, RuleSet *branch_ruleset, map<FWObject*, int> &branch_rulesets)
{
    // multiple rules in the rule set may branch to the same branch rule set
    map<FWObject*, int> local_branch_ruleset_counters;

    int c = branch_rulesets[branch_ruleset];
    branch_rulesets[branch_ruleset] = ++c;
    if (c > 1) return;  // we have seen this one already

    for (list<FWObject*>::iterator r=branch_ruleset->begin(); r!=branch_ruleset->end(); ++r)
    {
        Rule *rule = Rule::cast(*r);
        if (rule == nullptr) continue; // skip RuleSetOptions object

        RuleSet *next_branch_ruleset = rule->getBranch();
        if (next_branch_ruleset!=nullptr &&
            local_branch_ruleset_counters.count(next_branch_ruleset)==0)
        {
            local_branch_ruleset_counters[next_branch_ruleset] = 1;
            _findImportedRuleSetsRecursively(
                fw, next_branch_ruleset, branch_rulesets);
        }
    }
}

void CompilerDriver::assignUniqueRuleIds(list<FWObject*> &all_rulesets)
{
    for_each(all_rulesets.begin(), all_rulesets.end(),
             RuleSet::UniqueRuleIdsSetter());
}

QString CompilerDriver::run(const std::string&, const std::string&, const std::string&)
{
    return "";
}

void CompilerDriver::validateClusterGroups(Cluster *cluster)
{
    string host_os = cluster->getStr("host_OS");
    Resources* os_res = Resources::os_res[host_os];
    if (os_res==nullptr) return;

    // check if state sync groups are of supported type
    list<string> state_sync_protocols;
    os_res->getResourceStrList("/FWBuilderResources/Target/protocols/state_sync",
                               state_sync_protocols);

    for (FWObjectTypedChildIterator it = cluster->findByType(StateSyncClusterGroup::TYPENAME);
         it != it.end(); ++it)
    {
        string state_sync_type = (*it)->getStr("type");
        if (!isSupported(&state_sync_protocols, state_sync_type))
        {
            QString err("State sync group type '%1' is not supported");
            abort(cluster, nullptr, nullptr, err.arg(state_sync_type.c_str()).toStdString());
            throw FatalErrorInSingleRuleCompileMode();
        }
    }

    // same for failover groups
    list<string> failover_protocols;
    os_res->getResourceStrList("/FWBuilderResources/Target/protocols/failover",
                               failover_protocols);

    list<FWObject*> failover_groups = cluster->getByTypeDeep(FailoverClusterGroup::TYPENAME);
    for (list<FWObject*>::iterator it = failover_groups.begin(); it != failover_groups.end(); ++it)
    {
        FWObject *failover_group = *it;
        FWObject *parent = failover_group->getParent();
        string failover_type = failover_group->getStr("type");
        if (!isSupported(&failover_protocols, failover_type))
        {
            QString err("Failover group type '%1' is not supported");
            abort(cluster, nullptr, nullptr, err.arg(failover_type.c_str()).toStdString());
            throw FatalErrorInSingleRuleCompileMode();
        }

        list<FWObject*> l2 = failover_group->getByTypeDeep(FWObjectReference::TYPENAME);
        if (l2.size() == 0)
        {
            QString err("Failover group of cluster interface '%1' is empty");
            abort(cluster, nullptr, nullptr,
                  err.arg(parent->getName().c_str()).toStdString());
            throw FatalErrorInSingleRuleCompileMode();
        }
    }
}

bool CompilerDriver::isSupported(list<string> *protocols, const string &cluster_group_type)
{
    bool supported = false;
    for (list<string>::iterator supported_types=protocols->begin();
         supported_types!=protocols->end(); ++supported_types)
    {
        QString str = QString(supported_types->c_str());
        QStringList pl = str.split(",");
        if (cluster_group_type.c_str() == pl[0])
        {
            supported = true;
            break;
        }
    }
    return supported;
}


QTextStream& operator<< (QTextStream &text_stream, const string &str)
{
    text_stream << str.c_str();
    return text_stream;
}

/*
 * Add indentation to each line in txt
 */
string CompilerDriver::indent(int n_spaces, const string &txt)
{
    QString res = indent(n_spaces, QString(txt.c_str()));
    return res.toStdString();
}

QString CompilerDriver::indent(int n_spaces, const QString &txt)
{
    QString fill = QString("%1").arg("", n_spaces, ' ');
    return prepend(fill, txt);
}

/*
 * prepend each line in @txt with @prep, however there is no need to
 * prepend empty lines
 */
QString CompilerDriver::prepend(const QString &prep, const QString &txt)
{
    QStringList str;
    foreach (QString line, txt.split("\n"))
    {
        if (line.isEmpty()) str.append(line);
        else str.append(line.prepend(prep));
    }
    return str.join("\n");
}

void CompilerDriver::mergeRuleSets(Cluster *cluster, Firewall *fw,
                                   const string &type)
{
    list<FWObject*> all_rulesets = cluster->getByType(type);
    for (list<FWObject*>::iterator p = all_rulesets.begin();
         p != all_rulesets.end(); ++p)
    {
        FWObject *ruleset = *p;

        FWObject::iterator i = std::find_if(
            fw->begin(), fw->end(),
            FWObjectNameEQPredicate(ruleset->getName()));

        if (i!=fw->end() && (*i)->getTypeName() == type)
        {
            FWObject *fw_ruleset = *i;

            /*
             * fw has rule set with the same name.  See ticket #372
             * for details.
             *
             * if member firewall has rule set of the same type and
             * with the same name as cluster and firewall's rule set
             * is not empty, cluster's rule set is ignored and warning
             * is ussued
             *
             * if matching firewall's rule set is empty, cluster's
             * rule set is used
             *
             * all rule sets from the cluster that do not have
             * matching ones in the member firewall are merged into
             * the firewall before compilation and used.
             *
             * Note that rule set object has two different kinds of
             * children: rules and RuleSetOption objects. Check if it
             * has any rules.
             */

            int rule_cntr = 0;
            list<FWObject*>::iterator it = fw_ruleset->begin();
            for ( ; it!=fw_ruleset->end(); ++it)
            {
                if (Rule::cast(*it)!=nullptr) rule_cntr++;
            }

            if (rule_cntr > 0)
            {
                QString err("ignoring cluster rule set \"%1\" "
                            "because member firewall \"%2\" "
                            "has rule set with the same name.");
                warning(fw, fw_ruleset, nullptr,
                        err.arg(fw_ruleset->getName().c_str())
                        .arg(fw->getName().c_str()).toStdString());
            } else
            {
                fw_ruleset->clear();
                fw_ruleset->duplicate(ruleset, false);
                fw_ruleset->setStr(".ruleset_owner", cluster->getName());
                fw_ruleset->setInt(".ruleset_owner_id", cluster->getId());
            }
        } else
        {
            // fw does not have rule set with this name
            FWObject *copy_ruleset = fw->addCopyOf(ruleset, false);
            copy_ruleset->setStr(".ruleset_owner", cluster->getName());
            copy_ruleset->setInt(".ruleset_owner_id", cluster->getId());
        }
    }
}

/*
 * 1. Iterate over all fw interfaces and check if they are referenced in a
 *    ClusterGroup.
 *    -> if yes then make copy of vrrp interface and set BASEDEV accordingly
 * 2. clear Policy, NAT & Routing rules of the firewall, then copy cluster
 *    policy, NAT and routing rules.
 */
void CompilerDriver::populateClusterElements(Cluster *cluster, Firewall *fw)
{
    if (cluster==nullptr) return;

#ifdef DEBUG_CLUSTER_INTERFACES
    cerr << "CompilerDriver::populateClusterElements " << endl;

    cerr << cluster->getPath(false, true) << endl;
    list<FWObject*> cl_interfaces = cluster->getByTypeDeep(Interface::TYPENAME);
    cerr << cl_interfaces.size() << " interface" << endl;
    cluster->dump(false, true);

    cerr << fw->getPath(false, true) << endl;
    list<FWObject*> fw_interfaces = fw->getByTypeDeep(Interface::TYPENAME);
    cerr << fw_interfaces.size() << " interface" << endl;
    fw->dump(false, true);
#endif

//    int addedPolicies = 0;
    set<string> state_sync_types;

    checkCluster(cluster);
    
    for (FWObjectTypedChildIterator it = cluster->findByType(StateSyncClusterGroup::TYPENAME);
        it != it.end(); ++it)
    {
        StateSyncClusterGroup *state_sync_group = StateSyncClusterGroup::cast(*it);
        /* For the state syncing cluster group, hierarchy looks like this:
         * Cluster->StateSyncClusterGroup->ObjectRef
         */
        string grp_type = state_sync_group->getStr("type");
        if (state_sync_types.count(grp_type) > 0)
            throw FWException("Several state synchronization groups of the same type in one cluster object.");

        state_sync_types.insert(grp_type);

        for (FWObjectTypedChildIterator it =
                 state_sync_group->findByType(FWObjectReference::TYPENAME);
             it != it.end(); ++it)
        {
            Interface *iface = Interface::cast(FWObjectReference::getObject(*it));
            assert(iface);
            //processStateSyncGroup(cluster, fw, state_sync_group, iface);

            iface->getOptionsObject()->setBool("state_sync_group_member", true);
            iface->getOptionsObject()->setStr(
                "state_sync_group_id",
                FWObjectDatabase::getStringId(state_sync_group->getId()));
            string master_id = state_sync_group->getStr("master_iface");
            string iface_str_id = FWObjectDatabase::getStringId(iface->getId());
            iface->getOptionsObject()->setBool("state_sync_master",
                                               master_id == iface_str_id);
            fw->getOptionsObject()->setBool("cluster_member", true);
        }
    }

    // For VRRP references the hierarchy is as follows:
    // Cluster->Interface->FailoverClusterGroup->ObjectRef

    // get a list of pointers to all cluster interfaces. Can't use findByType()
    // and iterator because we'll be adding interfaces in the middle of the loop
    list<FWObject*> cluster_interfaces = cluster->getByTypeDeep(Interface::TYPENAME);

    list<FWObject*>::iterator cl_iface = cluster_interfaces.begin();
    for (; cl_iface != cluster_interfaces.end(); ++cl_iface)
    {
        Interface *cluster_interface = Interface::cast(*cl_iface);
        FailoverClusterGroup *failover_group =
            FailoverClusterGroup::cast(
                cluster_interface->getFirstByType(FailoverClusterGroup::TYPENAME));
        if (failover_group)
        {
            Interface *member_iface =
                failover_group->getInterfaceForMemberFirewall(fw);
            if (member_iface == nullptr) continue;

            assert(fw->getOptionsObject() != nullptr);

            member_iface->getOptionsObject()->setStr(
                "failover_group_id",
                FWObjectDatabase::getStringId(failover_group->getId()));

            // per #971: cluster interface should inherit attributes
            // of the member interfaces: regular / dynamic / unnimbered
            cluster_interface->setDyn(member_iface->isDyn());
            cluster_interface->setUnnumbered(member_iface->isUnnumbered());
            cluster_interface->setUnprotected(member_iface->isUnprotected());
            cluster_interface->setSecurityLevel(member_iface->getSecurityLevel());

            copyFailoverInterface(cluster, fw, failover_group, member_iface);

        } else
        {
            // cluster interface without failover group
            // is this a loopback interface ?
            if (cluster_interface->isLoopback())
            {
                /* Add copy of the interface from the cluster to the
                 * firewall object so that when it is encountered in
                 * the "intrface" rule element of its rules, it
                 * belongs to the firewall and is therefore valid.
                 */
                Interface* new_cl_if = Interface::cast(fw->addCopyOf(cluster_interface, true));
                assert(new_cl_if != nullptr);
                new_cl_if->getOptionsObject()->setBool("cluster_interface", true);
            }
        }
    }
    
    mergeRuleSets(cluster, fw, Policy::TYPENAME);
    mergeRuleSets(cluster, fw, NAT::TYPENAME);
    mergeRuleSets(cluster, fw, Routing::TYPENAME);

    // finally need to remember cluster object ID so that compiler can later
    // associate it in rules with the firewall.
    //
    // The alternative is to find all references to the cluster object
    // in rules and replace them with refs to the firewall. That could
    // be done either in prolog or in a special rule processor. It is
    // _much_ cheaper to just remember cluster ID though.
    fw->setInt("parent_cluster_id", cluster->getId());

//    return addedPolicies;
}

/*
 * Perform checks for failover interfaces and their addresses, add a
 * copy of failover interface form the cluster to the firewall object.
 *
 * This method assumes the following:
 *
 * - Failover interface owns its ip address which is different from
 *   addresses of either firewall
 *
 * - address of the failover interface must be on the same subnet as
 *   addresses of the firewalls (perhaps this restriction can be
 *   lifted? Was originally implemented by Secunet folks like this)
 */
void CompilerDriver::copyFailoverInterface(Cluster * /*UNUSED cluster */,
                                           Firewall *fw,
                                           FailoverClusterGroup *cluster_group,
                                           Interface *iface)
{
    Interface* cluster_if = Interface::cast(cluster_group->getParent());
    assert(cluster_if != nullptr);

    /* Add copy of the cluster interface to the firewall object
     *
     * While adding a copy of cluster interface to the firewall, make
     * sure it has new unique ID instead of a copy of the ID of the
     * cluster's interface object. If the ID is the same,
     * RuleElementItf::validateChild() finds clusters' interface which
     * is not a child of the firewall object and therefore is
     * rejected.
     */
    Interface* new_cl_if = Interface::cast(fw->addCopyOf(cluster_if, true));
    assert(new_cl_if != nullptr);
    new_cl_if->getOptionsObject()->setBool("cluster_interface", true);
    new_cl_if->getOptionsObject()->setStr("base_device", iface->getName());
    new_cl_if->getOptionsObject()->setStr(
        "base_interface_id", FWObjectDatabase::getStringId(iface->getId()));

    /* Set master property if interface is referenced
     * as master_iface
     */
    string master_id = cluster_group->getStr("master_iface");
    string iface_str_id = FWObjectDatabase::getStringId(iface->getId());

    new_cl_if->getOptionsObject()->setBool("failover_master",
                                           master_id == iface_str_id);

    /*
     * cluster interface should "inherit" some of the attributes of
     * the member interfaces it represents. For example, if member
     * interfaces are marked "unprotected" or "dedicated failover",
     * should be the cluster interface.  What else?
     */
    new_cl_if->setDedicatedFailover(iface->isDedicatedFailover());
    new_cl_if->setUnprotected(iface->isUnprotected());
    
    fw->getOptionsObject()->setBool("cluster_member", true);
}

/**
 * Do something with state sync cluster groups. Find interfaces that
 * were placed in the group and store the name in the variable
 * "state_sync_interface" which is used later to associate policy rule
 * that should be added to permit state sync protocol with right
 * interface. For iptables we add rule to permit conntrackd, for PIX
 * we generate "failover" commands, etc.
 */
void CompilerDriver::processStateSyncGroups(Cluster *cluster, Firewall *member_fw)
{
    for (FWObjectTypedChildIterator it = cluster->findByType(StateSyncClusterGroup::TYPENAME);
         it != it.end(); ++it)
    {
        FWObject *state_sync_group = *it;
        for (FWObjectTypedChildIterator grp_it =
                 state_sync_group->findByType(FWObjectReference::TYPENAME);
             grp_it != grp_it.end(); ++grp_it)
        {
            FWObject *iface = FWObjectReference::getObject(*grp_it);
            if (iface->isChildOf(member_fw))
            {
                member_fw->getOptionsObject()->setStr(
                    "state_sync_group_id",
                    FWObjectDatabase::getStringId(state_sync_group->getId()));

                member_fw->getOptionsObject()->setStr(
                    "state_sync_interface",
                    iface->getName());
                break;
            }
        }
    }
}

/*
 * Verify that there is at least one Cluster interface and that all
 * have unique names and IP addresses.
 */
int CompilerDriver::checkCluster(Cluster* cluster)
{
    assert(cluster != nullptr);
    FWObjectTypedChildIterator cluster_ifaces = cluster->findByType(Interface::TYPENAME);
    if (cluster_ifaces == cluster_ifaces.end())
    {
        /* No configured cluster interface found */
        abort(cluster, nullptr, nullptr, "The cluster has no interfaces.");
        throw FatalErrorInSingleRuleCompileMode();
    }

    for (; cluster_ifaces != cluster_ifaces.end(); ++cluster_ifaces)
    {
        string iface_name = Interface::cast(*cluster_ifaces)->getName();
        const InetAddr* iface_address = Interface::cast(*cluster_ifaces)->getAddressPtr();
        if (iface_address==nullptr) continue; // cluster interface with no address
        FWObjectTypedChildIterator other_ifaces = cluster_ifaces;
        for (++other_ifaces; other_ifaces != cluster_ifaces.end(); ++other_ifaces)
        {
            if (iface_name == Interface::cast(*other_ifaces)->getName())
            {
                QString err("Found duplicate cluster interface %1");
                abort(cluster, nullptr, nullptr, err.arg(iface_name.c_str()).toStdString());
                throw FatalErrorInSingleRuleCompileMode();
            }
            const InetAddr *other_iface_address = Interface::cast(*other_ifaces)->getAddressPtr();
            if (other_iface_address==nullptr) continue; // cluster interface with no address
            if (*iface_address == *other_iface_address)
            {
                QString err("Found duplicate cluster interface address %1");
                abort(cluster, nullptr, nullptr, err.arg(iface_address->toString().c_str()).toStdString());
                throw FatalErrorInSingleRuleCompileMode();
            }
        }
    }

    return 0;
}

QString CompilerDriver::formSingleRuleCompileOutput(const QString &generated_code)
{
    // in single rule compile mode just return the
    // result. Note that we do not return all_errors because
    // all compilers include errors and warnings with
    // generated code for each rule. Two exceptions: 1)
    // CompilerDriver errors need to be added on top, 2) if no
    // output has been produced by the compiler, we have to
    // show all_errors to the user because there could be an
    // error message explaining this. Combined output of all
    // compilers we assemble here may consist of a bunch of
    // empty lines separated by LF. Need to account for that.
    QString res = generated_code;
    QString res2 = res.split("\n", QString::SkipEmptyParts).join("").replace(" ", "");
    if (res2.isEmpty()) res = all_errors.join("\n");
    return res;
}

void CompilerDriver::getFirewallAndClusterObjects(const string &cluster_id,
                                                  const string &firewall_id,
                                                  Cluster **cl,
                                                  Firewall **fw)
{
    if (!cluster_id.empty())
    {
        Cluster *orig_cluster = Cluster::cast(
            objdb->findInIndex(objdb->getIntId(cluster_id)));

#ifdef WORK_ON_COPIES
        *cl = objdb->createCluster();
        workspace->add(*cl);
        (*cl)->duplicate(orig_cluster);
#else

        *cl = orig_cluster;

#endif

    }

    Firewall *orig_fw = Firewall::cast(
        objdb->findInIndex(objdb->getIntId(firewall_id)));
    assert(orig_fw);

#ifdef WORK_ON_COPIES

    *fw = objdb->createFirewall();
    workspace->add(*fw);
    (*fw)->duplicate(orig_fw);

    if (*cl != nullptr)
    {
        const map<int, int> &id_map = (*fw)->getIDMappingTable();
        map<int, int>::const_iterator it;
        for (it=id_map.begin(); it!=id_map.end(); ++it)
            (*cl)->replaceRef(it->first, it->second);
    }
#else

    *fw = orig_fw;

#endif

}

        
