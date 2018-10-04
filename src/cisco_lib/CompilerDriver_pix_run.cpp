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
#include <memory>

#include <assert.h>
#include <cstring>
#include <iomanip>

#include "CompilerDriver_pix.h"
#include "PolicyCompiler_pix.h"
#include "NATCompiler_pix.h"
#include "NATCompiler_asa8.h"
#include "RoutingCompiler_pix.h"
#include "OSConfigurator_pix_os.h"
#include "NamedObjectsAndGroupsSupport.h"
#include "NamedObjectsManagerPIX.h"
#include "NamedObjectsManagerASA8.h"
#include "AutomaticRules_cisco.h"

#include "Helper.h"

#include "fwbuilder/Cluster.h"
#include "fwbuilder/ClusterGroup.h"
#include "fwbuilder/FWException.h"
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

class sort_by_net_zone {
    string any_address_id;
    public:
    explicit sort_by_net_zone()
    {
        any_address_id = FWObjectDatabase::getStringId(
            FWObjectDatabase::ANY_ADDRESS_ID);
    }
    bool operator()(const FWObject *a, const FWObject *b)
    {
	if (Interface::constcast(a) && Interface::constcast(b))
        {
	    string netzone_a=a->getStr("network_zone");
	    string netzone_b=b->getStr("network_zone");
	    if ( netzone_a==any_address_id) return false;
	    if ( netzone_b==any_address_id) return true;
	}
	return false;
    }
};

QString CompilerDriver_pix::assembleManifest(Cluster*, Firewall*, bool)
{
    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);

    script << "!" << manifestMarker()
           << "* " << this->escapeFileName(file_names[FW_FILE]) << endl;

    return script_buffer;
}

QString CompilerDriver_pix::printActivationCommands(Firewall*)
{
    return "";
}

QString CompilerDriver_pix::assembleFwScript(Cluster *cluster,
                                             Firewall* fw,
                                             bool cluster_member,
                                             OSConfigurator *oscnf)
{
    Configlet script_skeleton(fw, "pix_os", "script_skeleton");
    Configlet top_comment(fw, "pix_os", "top_comment");

    FWOptions* options = fw->getOptionsObject();
    options->setStr("prolog_script", options->getStr("pix_prolog_script"));
    options->setStr("epilog_script", options->getStr("pix_epilog_script"));
    options->setStr("prolog_place", "");

    string vers = fw->getStr("version");
    string platform = fw->getStr("platform");

    bool outbound_acl_supported =
        Resources::platform_res[platform]->getResourceBool(
            string("/FWBuilderResources/Target/options/")+
            "version_"+vers+
            "/pix_outbound_acl_supported");

    bool afpa = options->getBool("pix_assume_fw_part_of_any");
    bool emulate_outb_acls = options->getBool("pix_emulate_out_acl");
    bool generate_outb_acls = options->getBool("pix_generate_out_acl");

    top_comment.setVariable(
        "outbound_acl_supported",
        QString((outbound_acl_supported) ? "supported" : "not supported"));

    top_comment.setVariable("emulate_outb_acls",
                            QString((emulate_outb_acls)?"yes":"no"));

    top_comment.setVariable("generate_outb_acls",
                            QString((generate_outb_acls)?"yes":"no"));

    top_comment.setVariable("afpa", QString((afpa)?"yes":"no"));

    script_skeleton.setVariable("short_script", options->getBool("short_script"));

    script_skeleton.setVariable("not_short_script",
                                ! options->getBool("short_script"));

    script_skeleton.setVariable("preamble_commands", 
                                QString::fromUtf8(
                                    preamble_commands.c_str()));

    script_skeleton.setVariable("clear_commands", 
                                QString::fromUtf8(
                                    clear_commands.c_str()));

    script_skeleton.setVariable("system_configuration_script", 
                                QString::fromUtf8(
                                    system_configuration_script.c_str()));

    script_skeleton.setVariable("named_objects_and_object_groups",
                                QString::fromUtf8(
                                    named_objects_and_groups.c_str()));

    script_skeleton.setVariable("policy_script",
                                QString::fromUtf8(policy_script.c_str()));
    script_skeleton.setVariable("nat_script",
                                QString::fromUtf8(nat_script.c_str()));
    script_skeleton.setVariable("routing_script",
                                QString::fromUtf8(routing_script.c_str()));

    assembleFwScriptInternal(cluster, fw, cluster_member, oscnf,
                             &script_skeleton, &top_comment, "!", true);

    return script_skeleton.expand();
}

QString CompilerDriver_pix::run(const std::string &cluster_id,
                                const std::string &firewall_id,
                                const std::string &single_rule_id)
{
    Cluster *cluster = nullptr;
    Firewall *fw = nullptr;

    getFirewallAndClusterObjects(cluster_id, firewall_id, &cluster, &fw);

    // Copy rules from the cluster object
    populateClusterElements(cluster, fw);

    if (cluster)
    {
        // PIX failover is dfferent from VRRP and other failover protocols
        // in that it does not create new virtual address. Instead, each
        // unit is configured with two ip addresses, one for the active
        // unit and another for standby one. When active unit fails, the
        // other one assumes its address.
        //
        // This matters because when we use cluster object or one of its
        // interfaces in rules, compiler should expand it to the set of
        // addresses that includes addresses of the corresponding
        // interface of both member firewalls. Method
        // CompilerDriver::copyFailoverInterface adds a copy of firewall
        // interface to the cluster object. This works for all firewalls,
        // but for PIX we need to add copies of interfaces from both
        // members.
        // 
        FWObjectTypedChildIterator cl_iface = cluster->findByType(Interface::TYPENAME);
        for (; cl_iface != cl_iface.end(); ++cl_iface)
        {
            FailoverClusterGroup *failover_group =
                FailoverClusterGroup::cast(
                    (*cl_iface)->getFirstByType(FailoverClusterGroup::TYPENAME));
            if (failover_group)
            {
                //FWObject *this_member_interface = nullptr; //UNUSED
                list<FWObject*> other_member_interfaces;
                for (FWObjectTypedChildIterator it =
                         failover_group->findByType(FWObjectReference::TYPENAME);
                     it != it.end(); ++it)
                {
                    FWObject *intf = FWObjectReference::getObject(*it);
                    assert(intf);
                    //if (intf->isChildOf(fw)) this_member_interface = intf; //UNUSED
                    //else other_member_interfaces.push_back(intf);
                    if (!intf->isChildOf(fw)) other_member_interfaces.push_back(intf);
                }

                if (!other_member_interfaces.empty())
                {
                    for (list<FWObject*>::iterator it=other_member_interfaces.begin();
                         it!=other_member_interfaces.end(); ++it)
                    {
                        cluster->addCopyOf(*it, true);
                    }
                }
            }
        }
    }

#if 0
    FWObjectTypedChildIterator iface = fw->findByType(Interface::TYPENAME);
    for (; iface != iface.end(); ++iface)
    {
        (*iface)->dump(true, true);
    }
#endif


    determineOutputFileNames(cluster, fw, !cluster_id.empty(),
                             QStringList(""), QStringList("fw"),
                             QStringList(""));

    FWOptions* options = fw->getOptionsObject();

    QString script_buffer;

    std::unique_ptr<NATCompiler_pix> n;
    std::unique_ptr<PolicyCompiler_pix> c;
    std::unique_ptr<RoutingCompiler_pix> r;


    try
    {
        clearReadOnly(fw);

        commonChecks2(cluster, fw);

        pixClusterConfigurationChecks(cluster, fw);

        // Note that fwobjectname may be different from the name of the
        // firewall fw This happens when we compile a member of a cluster
        current_firewall_name = fw->getName().c_str();

        bool pix_acl_basic = options->getBool("pix_acl_basic");
        bool pix_acl_no_clear = options->getBool("pix_acl_no_clear");
        bool pix_acl_substitution = options->getBool("pix_acl_substitution");
        bool pix_add_clear_statements = options->getBool("pix_add_clear_statements");

        if (!pix_acl_basic && !pix_acl_no_clear && !pix_acl_substitution)
        {
            if ( pix_add_clear_statements ) options->setBool("pix_acl_basic",true);
            else options->setBool("pix_acl_no_clear",true);
        }



        list<FWObject*> all_interfaces = fw->getByTypeDeep(Interface::TYPENAME);

        pixSecurityLevelChecks(fw, all_interfaces);
        pixNetworkZoneChecks(fw, all_interfaces);

        /* Now that all checks are done, we can drop copies of cluster
         * interfaces that were added to the firewall by
         * CompilerDriver::populateClusterElements()
         */
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

        NamedObjectsManagerPIX named_objects_manager(persistent_objects, fw);

        all_interfaces = fw->getByTypeDeep(Interface::TYPENAME);

        for (std::list<FWObject*>::iterator i=all_interfaces.begin();
             i!=all_interfaces.end(); ++i)
        {
            Interface *iface = Interface::cast(*i);
            assert(iface);
/*
 * missing labels on interfaces
 *
 */
            if (iface->getLabel()=="")
            {
                string lbl;
                if (iface->isDedicatedFailover()) 
                {
                    // dedicated failover interface misses label. This
                    // interface can be used in failover cluster group
                    // or state sync group. Assign label depending on
                    // the function.
                    FWObjectTypedChildIterator it =
                        cluster->findByType(StateSyncClusterGroup::TYPENAME);
                    StateSyncClusterGroup *state_sync_group =
                        StateSyncClusterGroup::cast(*it);
                    if (state_sync_group && state_sync_group->hasMember(iface))
                        lbl = "state";

                    if (!iface->getOptionsObject()->getStr("failover_group_id").empty())
                        lbl = "failover";
                }

                if (lbl.empty())
                {
                    if (iface->getSecurityLevel()==0)   lbl="outside";
                    else
                    {
                        if (iface->getSecurityLevel()==100) lbl="inside";
                        else
                        {
                            QString l("dmz%1");
                            lbl = l.arg(iface->getSecurityLevel()).toStdString();
                        }
                    }
                }
                iface->setLabel(lbl);
            }



        }
        /*
         *  now sort interfaces by their network zone "width" (that
         *  is, more narrow network zone should go first, interface
         *  with network zone "any" should be the last)
         *
         std::sort(fw->begin(), fw->end(), sort_by_net_zone() );
        */


        try
        {
            AutomaticRules_cisco auto_rules(fw, persistent_objects);
            auto_rules.addSshAccessRule();
        } catch (FWException &ex)
        {
            abort(ex.toString());
        }

        std::unique_ptr<Preprocessor> prep(
            new Preprocessor(objdb , fw, false));
        if (inTestMode()) prep->setTestMode();
        if (inEmbeddedMode()) prep->setEmbeddedMode();
        prep->compile();

        std::unique_ptr<OSConfigurator> oscnf(
            new OSConfigurator_pix_os(objdb , fw, false));
        if (inTestMode()) oscnf->setTestMode();
        if (inEmbeddedMode()) oscnf->setEmbeddedMode();

        oscnf->prolog();
        oscnf->processFirewallOptions();

        bool have_named_objects = false;
        bool have_object_groups = false;

/* create compilers and run the whole thing */
        string version = fw->getStr("version");

        if (XMLTools::version_compare(version, "8.3")>=0)
            n = std::unique_ptr<NATCompiler_pix>(
                new NATCompiler_asa8(objdb, fw, false, oscnf.get()));
        else
            n = std::unique_ptr<NATCompiler_pix>(
                new NATCompiler_pix(objdb, fw, false, oscnf.get()));

        RuleSet *nat = RuleSet::cast(fw->getFirstByType(NAT::TYPENAME));
        if (nat)
        {
            nat->assignUniqueRuleIds();

            n->setNamedObjectsManager(&named_objects_manager);
            n->setSourceRuleSet(nat);
            n->setRuleSetName(nat->getName());
            n->setPersistentObjects(persistent_objects);

            if (inTestMode()) n->setTestMode();
            if (inEmbeddedMode()) n->setEmbeddedMode();
            n->setSingleRuleCompileMode(single_rule_id);
            n->setDebugLevel( dl );
            if (rule_debug_on) n->setDebugRule(  drn );
            n->setVerbose( verbose );

            if ( n->prolog() > 0 )
            {
                n->compile();
                n->epilog();

                preamble_commands += n->printPreambleCommands();
                clear_commands += n->printClearCommands();
                have_named_objects = (have_named_objects ||
                                      named_objects_manager.haveNamedObjects());
                have_object_groups = (have_object_groups ||
                                      named_objects_manager.haveObjectGroups());
                //named_objects_manager.saveObjectGroups();
            } else
                info(" Nothing to compile in NAT");
        }

        c = std::unique_ptr<PolicyCompiler_pix>(
            new PolicyCompiler_pix(objdb, fw, false, oscnf.get() , n.get()));

        RuleSet *policy = RuleSet::cast(fw->getFirstByType(Policy::TYPENAME));
        if (policy)
        {
            policy->assignUniqueRuleIds();

            c->setNamedObjectsManager(&named_objects_manager);
            c->setSourceRuleSet(policy);
            c->setRuleSetName(policy->getName());
            c->setPersistentObjects(persistent_objects);

            if (inTestMode()) c->setTestMode();
            if (inEmbeddedMode()) c->setEmbeddedMode();
            c->setSingleRuleCompileMode(single_rule_id);
            c->setDebugLevel( dl );
            if (rule_debug_on) c->setDebugRule(  drp );
            c->setVerbose( verbose );

            if ( c->prolog() > 0 )
            {
                c->compile();
                c->epilog();

                preamble_commands += c->printPreambleCommands();
                clear_commands += c->printClearCommands();
                have_named_objects = (have_named_objects ||
                                      named_objects_manager.haveNamedObjects());
                have_object_groups = (have_object_groups ||
                                      named_objects_manager.haveObjectGroups());
                //named_objects_manager.saveObjectGroups();
            } else
                info(" Nothing to compile in Policy");
        }

        r = std::unique_ptr<RoutingCompiler_pix>(
            new RoutingCompiler_pix(objdb, fw, false, oscnf.get()));

        RuleSet *routing = RuleSet::cast(fw->getFirstByType(Routing::TYPENAME));
        if (routing)
        {
            routing->assignUniqueRuleIds();

            r->setNamedObjectsManager(&named_objects_manager);
            r->setSourceRuleSet(routing);
            r->setRuleSetName(routing->getName());
            r->setPersistentObjects(persistent_objects);
                
            if (inTestMode()) r->setTestMode();
            if (inEmbeddedMode()) r->setEmbeddedMode();
            r->setSingleRuleCompileMode(single_rule_id);
            r->setDebugLevel( dl );
            if (rule_debug_on) r->setDebugRule(  drp );
            r->setVerbose( verbose );

            if ( r->prolog() > 0 )
            {
                r->compile();
                r->epilog();
            } else
                info(" Nothing to compile in Routing");
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

        policy_script = c->getCompiledScript();
        nat_script = n->getCompiledScript();
        routing_script = r->getCompiledScript();

        named_objects_and_groups = named_objects_manager.getNamedObjectsDefinitions();

        if (c->haveErrorsAndWarnings())
            all_errors.push_back(c->getErrors("C ").c_str());
        if (n->haveErrorsAndWarnings())
            all_errors.push_back(n->getErrors("N ").c_str());
        if (r->haveErrorsAndWarnings())
            all_errors.push_back(r->getErrors("R ").c_str());

        if (single_rule_compile_on)
        {
            return formSingleRuleCompileOutput(
                QString::fromUtf8(
                    (named_objects_and_groups +
                     policy_script + nat_script + routing_script).c_str()));
        }

        system_configuration_script = oscnf->getCompiledScript();
        system_configuration_script += "\n";

        clear_commands += named_objects_manager.getClearCommands() + "\n";

        // system_configuration_script += preamble_commands;
        // system_configuration_script += clear_commands;


        script_buffer = assembleFwScript(
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


void CompilerDriver_pix::pixSecurityLevelChecks(Firewall *fw,
                                                list<FWObject*> &all_interfaces)
{
    for (std::list<FWObject*>::iterator i=all_interfaces.begin(); i!=all_interfaces.end(); ++i)
    {
        Interface *iface = dynamic_cast<Interface*>(*i);
        assert(iface);

        if (iface->getOptionsObject()->getBool("cluster_interface")) continue;

        if ((iface->getOptionsObject()->getStr("type") == "" ||
             iface->getOptionsObject()->getStr("type") == "ethernet") &&
            iface->getByType(Interface::TYPENAME).size() > 0)
        {
            // Parent vlan interface (i.e. trunk)
            if (!iface->isUnprotected())
            {
                QString err(
                    "Interface %1 has vlan subinterfaces, it can not "
                    "be used for ACL. Marking this interface \"unprotected\" "
                    "to exclude it."
                );
                warning(fw, nullptr, nullptr,
                        err.arg(iface->getName().c_str())
                        .toStdString());
                iface->setUnprotected(true);
            }
        }

        // Tests for label, security level and network zone make sense
        // only for interfaces that can be used in ACLs or to bind
        // ACLs to.  Unnumbered interfaces can't, so we do not need to
        // run these checks.  One example of unnumbered interface is
        // parent interface for vlan subinterfaces.
        if (iface->isUnnumbered()) continue;
        if (iface->isUnprotected()) continue;

/*
 * there shouldn't be two interfaces with the same security level and
 * same label
 * 
 */
        for (std::list<FWObject*>::iterator j=all_interfaces.begin(); j!=all_interfaces.end(); ++j)
        {
            Interface *iface2 = dynamic_cast<Interface*>(*j);
            assert(iface2);
            if (iface2->isUnnumbered()) continue;
            if (iface2->isUnprotected()) continue;
            if (iface->getId()==iface2->getId()) continue;
            if (iface->getOptionsObject()->getBool("cluster_interface") ||
                iface2->getOptionsObject()->getBool("cluster_interface"))
                continue;

            // see #2351. Security levels do not have to be unique
            // if (iface->getSecurityLevel()==iface2->getSecurityLevel())
            // {
            //     QString err(
            //         "Security level of each interface should be unique, "
            //         "however interfaces %1 (%2) and %3 (%4)"
            //         " have the same security level."
            //     );
            //     abort(fw, nullptr, nullptr,
            //           err.arg(iface->getName().c_str())
            //           .arg(iface->getLabel().c_str())
            //           .arg(iface2->getName().c_str())
            //           .arg(iface2->getLabel().c_str()).toStdString());
            //     throw FatalErrorInSingleRuleCompileMode();
            // }

            if (iface->getLabel()==iface2->getLabel())
            {
                QString err(
                    "Label of each interface should be unique, "
                    "however interfaces %1 (%2) and %3 (%4)"
                    " have the same."
                );
                abort(fw, nullptr, nullptr,
                      err.arg(iface->getName().c_str())
                      .arg(iface->getLabel().c_str())
                      .arg(iface2->getName().c_str())
                      .arg(iface2->getLabel().c_str()).toStdString());
                throw FatalErrorInSingleRuleCompileMode();
            }
        }

        // We only do limited checks for dedicated failover
        // interfaces because they are not used in ACLs or
        // anywhere else in configuration, except in "failover"
        // commands.
        if (iface->isDedicatedFailover()) continue;

    }
}


void CompilerDriver_pix::pixNetworkZoneChecks(Firewall *fw,
                                              list<FWObject*> &all_interfaces)
{
    multimap<string, FWObject*> netzone_objects;
    Helper helper(nullptr);

    for (std::list<FWObject*>::iterator i=all_interfaces.begin(); i!=all_interfaces.end(); ++i)
    {
        Interface *iface = dynamic_cast<Interface*>(*i);
        assert(iface);

        if (iface->getOptionsObject()->getBool("cluster_interface")) continue;
        if (iface->isDedicatedFailover()) continue;
        if (iface->isUnprotected()) continue;


        /*
         * in PIX, we need network zones to be defined for all
         * interfaces
         */
        string netzone_id = iface->getStr("network_zone");
        if (netzone_id=="")
        {
            QString err("Network zone definition is missing for interface '%1' (%2)");
            abort(fw, nullptr, nullptr,
                  err.arg(iface->getName().c_str())
                  .arg(iface->getLabel().c_str()).toStdString());
            throw FatalErrorInSingleRuleCompileMode();
        }

        FWObject *netzone = objdb->findInIndex(
            FWObjectDatabase::getIntId(netzone_id));
        if (netzone==nullptr) 
        {
            QString err("Network zone points at nonexisting object for "
                        "interface '%1' (%2)");
            abort(fw, nullptr, nullptr,
                  err.arg(iface->getName().c_str())
                  .arg(iface->getLabel().c_str()).toStdString());
            throw FatalErrorInSingleRuleCompileMode();
        }
/*
 * netzone may be a group, in which case we need to expand it
 * (recursively). 
 * 
 * 1. We create new temporary object (type Group).
 *
 * 2. put it in the database somewhere
 *
 * 3. add all objects that belong to the network zone to this
 * group. We add objects directly, not as a reference.
 *
 * 4. finally replace reference to the old network zone object in the
 * interface with reference to this new group.
 *
 * 5. we store ID of the original network zone object 
 *    using iface->setStr("orig_netzone_id")
 *
 * This ensures netzones do not contain other groups and do not
 * require any recursive expanding anymore. Since objects were added
 * to netzones directly, we do not need to bother with dereferencing,
 * too.
 */
        list<FWObject*> ol;
        helper.expand_group_recursive(netzone, ol);

        FWObject *nz = objdb->createObjectGroup();
        assert(nz!=nullptr);
        nz->setName("netzone_" + iface->getLabel());
        objdb->add(nz);

        for (list<FWObject*>::iterator j=ol.begin(); j!=ol.end(); ++j)
        {
            Address *addr = Address::cast(*j);
            if (addr == nullptr || addr->getAddressPtr() == nullptr)
            {
                QString err("Network zone of interface '%1' uses object '%2' "
                            "that is not an address");
                abort(fw, nullptr, nullptr,
                      err.arg(iface->getLabel().c_str())
                      .arg((*j)->getName().c_str()).toStdString());
                throw FatalErrorInSingleRuleCompileMode();
            }

/*
  Commented out for SF bug 3213019

  currently we do not support ipv6 with PIX/ASA and FWSM. If user
  creates a group to be used as network zone object and places ipv6
  address in it, this address should be ignored while compiling the
  policy but this should not be an error. Compiler uses network zone
  group to do various address matching operations when it tries to
  determine an interface for a rule where user did not specify
  one. Since we never (should) have ipv6 in policy and nat rules,
  compiler is not going to have anything to compare to ipv6 address in
  the network zone even if there is one and this ipv6 address is going
  to be ignored.


            if (addr->getAddressPtr()->isV6())
            {
                QString err("Network zone of interface '%1' uses object '%2' "
                            "that is IPv6 address");
                abort(fw, nullptr, nullptr,
                      err.arg(iface->getLabel().c_str())
                      .arg((*j)->getName().c_str()).toStdString());
                throw FatalErrorInSingleRuleCompileMode();
            }
*/
            netzone_objects.insert(
                pair<string,FWObject*>(iface->getLabel(),*j));
            nz->addRef(*j);
        }
        iface->setStr("orig_netzone_id", netzone_id );
        iface->setStr("network_zone",
                      FWObjectDatabase::getStringId(nz->getId()) );
    }


/*
 * the same object (network or host) can not belong to network zones
 * of two different interfaces. Map netzone_objects holds pairs
 * interface_id/object. We just make sure the same object does not
 * appear in two pairs with different interfaces.
 */
    multimap<string,FWObject*>::iterator k;
    for (k=netzone_objects.begin(); k!=netzone_objects.end(); ++k)
    {
        multimap<string,FWObject*>::iterator l;
        l=k;
        ++l;
        for ( ; l!=netzone_objects.end(); ++l)
        {
            if ( l->second->getId() == k->second->getId() )
            {
                if (k->first==l->first)
                {
                    QString err("Object %1 is used more than once in network "
                                "zone of interface '%2'");
                    abort(fw, nullptr, nullptr,
                          err.arg(l->second->getName().c_str())
                          .arg(k->first.c_str()).toStdString());
                    throw FatalErrorInSingleRuleCompileMode();
                } else
                {
                    QString err("Object %1 is used in network zones of "
                                "interfaces '%2' and '%3'");
                    abort(fw, nullptr, nullptr,
                          err.arg(l->second->getName().c_str())
                          .arg(k->first.c_str())
                          .arg(l->first.c_str()).toStdString());
                    throw FatalErrorInSingleRuleCompileMode();
                }
            }
        }
    }


}

/*
 * Sanity checks for the cluster configuration. Per ticket #606:
 *
 * - state sync group must have master
 *
 * - one interface must be marked as "dedicated failover" for failover
 *   group.
 *
 * - this interface must have failover group with members, one of
 *   which must be master
 *
 * - failover interfaces in member firewalls must have ip addresses
 *   which should be different but in the same subnet.
 *
 * - possibly another interface can be defined as "dedicated failover"
 *   and used in state sync group.
 *
 * - if second interface is used for state sync, it must have ip
 *   address in member firewalls (different)
 *
 * - addresses of the dedicated failover interfaces must belong to the
 *   same subnet in each pair of failover inetrfaces (failover and state sync)
 *
 * - failover interfaces of both members used in the failover cluster
 *   group of the cluster object must have the same name.
 *
 * - The same check should be performed in the state sync group. 
 *
 *
 */
void CompilerDriver_pix::pixClusterConfigurationChecks(Cluster *cluster,
                                                       Firewall*)
{
    if (cluster==nullptr) return;

    FWObjectTypedChildIterator it = cluster->findByType(StateSyncClusterGroup::TYPENAME);
    StateSyncClusterGroup *state_sync_group = StateSyncClusterGroup::cast(*it);

    if (state_sync_group->getStr("master_iface").empty())
    {
        QString err("One of the interfaces in the state synchronization group "
                    "must be marked as 'Master'");
        abort(cluster, nullptr, nullptr, err.toStdString());
        throw FatalErrorInSingleRuleCompileMode();
    }

    pixClusterGroupChecks(state_sync_group);

    //bool failover_group_inspected = false; //UNUSED
    list<FWObject*> l2 = cluster->getByTypeDeep(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
    {
	Interface *iface = dynamic_cast<Interface*>(*i);
	assert(iface);

        FailoverClusterGroup *failover_group =
            FailoverClusterGroup::cast(
                iface->getFirstByType(FailoverClusterGroup::TYPENAME));
        if (failover_group)
        {
            for (FWObjectTypedChildIterator it =
                     failover_group->findByType(FWObjectReference::TYPENAME);
                 it != it.end(); ++it)
            {
#ifndef NDEBUG
                Interface *member_iface = Interface::cast(FWObjectReference::getObject(*it));
                assert(member_iface);
#endif

                pixClusterGroupChecks(failover_group);

                //if (member_iface->isDedicatedFailover())
                //{
                //    failover_group_inspected = true; //UNUSED
                //}
            }
        }
    }
}

void CompilerDriver_pix::pixClusterGroupChecks(ClusterGroup *cluster_group)
{
    FWObject *cluster = cluster_group;
    while (cluster && !Cluster::isA(cluster)) cluster = cluster->getParent();

    FWObject *cluster_interface = nullptr;
    FWObject *p = cluster_group->getParent();
    if (Interface::isA(p)) cluster_interface = p;

    map<QString, const InetAddrMask*> addresses_and_masks;

    for (FWObjectTypedChildIterator it = cluster_group->findByType(FWObjectReference::TYPENAME);
         it != it.end(); ++it)
    {
        Interface *member_iface = Interface::cast(FWObjectReference::getObject(*it));
        assert(member_iface);
        FWObject *member = Host::getParentHost(member_iface);
        //FWObject *member = member_iface->getParentHost();

        if (cluster_interface)
        {
            // check consistency of the names.
            // In case of PIX the name of the cluster interface should match 
            // names of member interfaces
            if (cluster_interface->getName() != member_iface->getName())
            {
                QString err("Names of interfaces used in state synchronization "
                            "or failover group must match the name of the "
                            "cluster inetrface. Interface %1:%2 has the name "
                            "that is different from the cluster interface name %3");

                abort(cluster, nullptr, nullptr,
                      err.arg(member->getName().c_str())
                      .arg(member_iface->getName().c_str())
                      .arg(cluster_interface->getName().c_str()).toStdString());
                throw FatalErrorInSingleRuleCompileMode();
            }
        }

        if (StateSyncClusterGroup::isA(cluster_group) &&
            !member_iface->isDedicatedFailover())
        {
            QString err("Interface %1 is used in a state synchronization "
                        "but is not marked as 'Dedicated Failover' "
                        "interface. All interfaces used for the state "
                        "synchronization or failover must be marked "
                        "'Dedicated Failover'. ");

            abort(member, nullptr, nullptr,
                  err.arg(member_iface->getName().c_str()).toStdString());
            throw FatalErrorInSingleRuleCompileMode();
        }

        if (!member_iface->isRegular() || member_iface->countInetAddresses(true)==0)
        {
            QString err("Interface %1 which is used in state synchronization "
                        "or failover group does not have an IP address. "
                        "All interfaces used for the state "
                        "synchronization or failover must have ip addresses.");

            abort(member, nullptr, nullptr,
                  err.arg(member_iface->getName().c_str()).toStdString());
            throw FatalErrorInSingleRuleCompileMode();
        }
        QString key("%1:%2");

        FWObjectTypedChildIterator it_addr = member_iface->findByType(IPv4::TYPENAME);
        IPv4* addr = IPv4::cast(*it_addr);
        addresses_and_masks[key.arg(member->getName().c_str()).arg(member_iface->getName().c_str())] =
            addr->getInetAddrMaskObjectPtr();
    }
    
    if (addresses_and_masks.size() >= 2)
    {
        QString first_key;
        const InetAddr *first_network_addr = nullptr;
        map<QString, const InetAddrMask*>::iterator it;
        for (it=addresses_and_masks.begin(); it!=addresses_and_masks.end(); ++it)
        {
            QString key = it->first;
            const InetAddrMask *am = it->second;
            if (first_network_addr == nullptr)
            {
                first_key = key;
                first_network_addr = am->getNetworkAddressPtr();
            } else
            {
                const InetAddr *network_addr = am->getNetworkAddressPtr();
                if (*first_network_addr != *(network_addr))
                {
                    QString err("Interfaces used in state synchronization "
                                "or failover group must have IP addresses on "
                                "the same subnet. Interfaces %1 and %2 have "
                                "addresses on different subnets: %3 , %4");

                    abort(cluster, nullptr, nullptr,
                          err.arg(first_key).arg(key)
                          .arg(first_network_addr->toString().c_str())
                          .arg(network_addr->toString().c_str()).toStdString());
                    throw FatalErrorInSingleRuleCompileMode();
                }
            }
        }
    }
}


