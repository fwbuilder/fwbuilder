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

#include "CompilerDriver_pix.h"
#include "PolicyCompiler_pix.h"
#include "NATCompiler_pix.h"
#include "RoutingCompiler_pix.h"
#include "OSConfigurator_pix_os.h"

#include "Helper.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/NAT.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"

#include "fwcompiler/Preprocessor.h"

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

QString CompilerDriver_pix::assembleManifest(Firewall* fw, bool cluster_member)
{
    QString script_buffer;
    QTextStream script(&script_buffer, QIODevice::WriteOnly);
    QString ofname = determineOutputFileName(fw, cluster_member, ".fw");
    script << "!" << MANIFEST_MARKER << "* " << ofname << endl;
    return script_buffer;
}

QString CompilerDriver_pix::printActivationCommands(Firewall*)
{
    return "";
}

QString CompilerDriver_pix::assembleFwScript(Firewall* fw,
                                             bool cluster_member,
                                             OSConfigurator *oscnf)
{
    Configlet script_skeleton(fw, "cisco", "script_skeleton");
    Configlet top_comment(fw, "cisco", "top_comment");

    FWOptions* options = fw->getOptionsObject();
    options->setStr("prolog_script", options->getStr("pix_prolog_script"));
    options->setStr("epilog_script", options->getStr("pix_epilog_script"));

    string vers = fw->getStr("version");
    string platform = fw->getStr("platform");
    bool outbound_acl_supported = Resources::platform_res[platform]->getResourceBool(
        string("/FWBuilderResources/Target/options/")+
        "version_"+vers+
        "/pix_outbound_acl_supported");
    bool afpa = options->getBool("pix_assume_fw_part_of_any");
    bool emulate_outb_acls = options->getBool("pix_emulate_out_acl");
    bool generate_outb_acls = options->getBool("pix_generate_out_acl");

    top_comment.setVariable("outbound_acl_supported", QString((outbound_acl_supported)?"supported":"not supported"));
    top_comment.setVariable("emulate_outb_acls", QString((emulate_outb_acls)?"yes":"no"));
    top_comment.setVariable("generate_outb_acls", QString((generate_outb_acls)?"yes":"no"));
    top_comment.setVariable("afpa", QString((afpa)?"yes":"no"));

    script_skeleton.setVariable("system_configuration_script", system_configuration_script.c_str());
    script_skeleton.setVariable("policy_script", policy_script.c_str());
    script_skeleton.setVariable("nat_script", nat_script.c_str());
    script_skeleton.setVariable("routing_script", routing_script.c_str());

    assembleFwScriptInternal(fw, cluster_member, oscnf, &script_skeleton, &top_comment, "!");
    return script_skeleton.expand();
}

string CompilerDriver_pix::run(const std::string &cluster_id,
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
            FWObject *this_member_interface = NULL;
            list<FWObject*> other_member_interfaces;
            for (FWObjectTypedChildIterator it =
                     failover_group->findByType(FWObjectReference::TYPENAME);
                 it != it.end(); ++it)
            {
                FWObject *intf = FWObjectReference::getObject(*it);
                assert(intf);
                if (intf->isChildOf(fw)) this_member_interface = intf;
                else other_member_interfaces.push_back(intf);
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

#if 0
    FWObjectTypedChildIterator iface = fw->findByType(Interface::TYPENAME);
    for (; iface != iface.end(); ++iface)
    {
        (*iface)->dump(true, true);
    }
#endif

    commonChecks2(cluster, fw);

    // Note that fwobjectname may be different from the name of the
    // firewall fw This happens when we compile a member of a cluster
    current_firewall_name = fw->getName().c_str();

    QString ofname = determineOutputFileName(fw, !cluster_id.empty(), ".fw");
    FWOptions* options = fw->getOptionsObject();

    bool pix_acl_basic = options->getBool("pix_acl_basic");
    bool pix_acl_no_clear = options->getBool("pix_acl_no_clear");
    bool pix_acl_substitution = options->getBool("pix_acl_substitution");
    bool pix_add_clear_statements = options->getBool("pix_add_clear_statements");

    if ( !pix_acl_basic &&
         !pix_acl_no_clear &&
         !pix_acl_substitution )
    {
        if ( pix_add_clear_statements ) options->setBool("pix_acl_basic",true);
        else options->setBool("pix_acl_no_clear",true);
    }

    Helper helper(NULL);

    multimap<string, FWObject*> netzone_objects;

    std::list<FWObject*> l2=fw->getByType(Interface::TYPENAME);
    for (std::list<FWObject*>::iterator i=l2.begin(); i!=l2.end(); ++i)
    {
        Interface *iface = dynamic_cast<Interface*>(*i);
        assert(iface);

        // dedicated failover interfaces are not used in ACLs or anywhere
        // else in configuration, except in "failover" commands.
        if (iface->isDedicatedFailover()) continue;

        // Tests for label, security level and network zone make sense
        // only for interfaces that can be used in ACLs or to bind
        // ACLs to.  Unnumbered interfaces can't, so we do not need to
        // run these checks.  One example of unnumbered interface is
        // parent interface for vlan subinterfaces.
        if (iface->isUnnumbered()) continue;

        if (iface->getOptionsObject()->getBool("cluster_interface")) continue;

/*
 * missing labels on interfaces
 */
        if (iface->getLabel()=="")
        {
            string lbl;
            if (iface->getSecurityLevel()==0)   lbl="outside";
            else
            {
                if (iface->getSecurityLevel()==100) lbl="inside";
                else
                {
                    char s[64];
                    sprintf(s,"dmz%d",iface->getSecurityLevel());
                    lbl=s;
                }
            }
            iface->setLabel(lbl);
        }

/*
 * there shouldn't be two interfaces with the same security level
 */
        for (std::list<FWObject*>::iterator j=l2.begin(); j!=l2.end(); ++j)
        {
            Interface *iface2 = dynamic_cast<Interface*>(*j);
            assert(iface2);
            if (iface2->isDedicatedFailover()) continue;
            if (iface2->isUnnumbered()) continue;
            if (iface->getId()==iface2->getId()) continue;
            if (iface->getOptionsObject()->getBool("cluster_interface") ||
                iface2->getOptionsObject()->getBool("cluster_interface")) continue;

            if (iface->getSecurityLevel()==iface2->getSecurityLevel())
            {
                QString err(
                    "Security level of each interface should be unique, "
                    "however interfaces %1 (%2) and %3 (%4)"
                    " have the same security level."
                );
                abort(fw, NULL, NULL,
                      err.arg(iface->getName().c_str())
                      .arg(iface->getLabel().c_str())
                      .arg(iface2->getName().c_str())
                      .arg(iface2->getLabel().c_str()).toStdString());
                return "";
            }
        }
/*
 * in PIX, we need network zones to be defined for all interfaces
 */
        string netzone_id=iface->getStr("network_zone");
        if (netzone_id=="")
        {
            QString err("Network zone definition is missing for interface %1 (%2)");
            abort(fw, NULL, NULL,
                  err.arg(iface->getName().c_str())
                  .arg(iface->getLabel().c_str()).toStdString());
            return "";
        }
        FWObject *netzone=objdb->findInIndex(
            FWObjectDatabase::getIntId(netzone_id));
        if (netzone==NULL) 
        {
            QString err("Network zone points at nonexisting object for interface %1 (%2)");
            abort(fw, NULL, NULL,
                  err.arg(iface->getName().c_str())
                  .arg(iface->getLabel().c_str()).toStdString());
            return "";
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
        helper.expand_group_recursive(netzone,ol);

        FWObject *nz = objdb->createObjectGroup();
        assert(nz!=NULL);
        nz->setName("netzone_"+iface->getLabel());
        objdb->add(nz);

        for (list<FWObject*>::iterator j=ol.begin(); j!=ol.end(); ++j)
        {
            netzone_objects.insert( pair<string,FWObject*>(iface->getLabel(),*j));
            nz->add(*j);
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
                    QString err("Object %1 is used more than once in network zone of interface %2");
                    abort(fw, NULL, NULL,
                          err.arg(l->second->getName().c_str())
                          .arg(k->first.c_str()).toStdString());
                    return "";
                } else
                {
                    QString err("Object %1 is used in network zones of "
                                "interfaces %2 and %3");
                    abort(fw, NULL, NULL,
                          err.arg(l->second->getName().c_str())
                          .arg(k->first.c_str())
                          .arg(l->first.c_str()).toStdString());
                    return "";
                }
            }
        }
    }

/*
 *  now sort interfaces by their network zone "width" (that is, more narrow 
 *  network zone should go first, interface with network zone "any" should be
 *  the last)
 *
 std::sort(fw->begin(), fw->end(), sort_by_net_zone() );
*/


    std::auto_ptr<Preprocessor> prep(new Preprocessor(objdb , fw, false));
    prep->compile();

/*
 * Process firewall options, build OS network configuration script
 */
    std::auto_ptr<OSConfigurator> oscnf(new OSConfigurator_pix_os(objdb , fw, false));

    oscnf->prolog();
    oscnf->processFirewallOptions();


/* create compilers and run the whole thing */

    std::auto_ptr<NATCompiler_pix> n(new NATCompiler_pix(objdb, fw, false, oscnf.get()));

    RuleSet *nat = RuleSet::cast(fw->getFirstByType(NAT::TYPENAME));
    if (nat)
    {
        n->setSourceRuleSet(nat);
        n->setRuleSetName(nat->getName());

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
        } else
            info(" Nothing to compile in NAT");
    }

    std::auto_ptr<PolicyCompiler_pix> c(
        new PolicyCompiler_pix(objdb, fw, false, oscnf.get() , n.get()));

    RuleSet *policy = RuleSet::cast(fw->getFirstByType(Policy::TYPENAME));
    if (policy)
    {
        c->setSourceRuleSet(policy);
        c->setRuleSetName(policy->getName());

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
        } else
            info(" Nothing to compile in Policy");
    }

    std::auto_ptr<RoutingCompiler_pix> r(new RoutingCompiler_pix(objdb, fw, false, oscnf.get()));

    RuleSet *routing = RuleSet::cast(fw->getFirstByType(Routing::TYPENAME));
    if (routing)
    {
        r->setSourceRuleSet(routing);
        r->setRuleSetName(routing->getName());

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

    if (haveErrorsAndWarnings())
    {
        all_errors.push_front(getErrors("").c_str());
    }

    system_configuration_script = oscnf->getCompiledScript();
    policy_script = c->getCompiledScript();
    nat_script = n->getCompiledScript();
    routing_script = r->getCompiledScript();

    if (c->haveErrorsAndWarnings())
        all_errors.push_back(c->getErrors("C ").c_str());
    if (n->haveErrorsAndWarnings())
        all_errors.push_back(n->getErrors("N ").c_str());
    if (r->haveErrorsAndWarnings())
        all_errors.push_back(r->getErrors("R ").c_str());

    if (single_rule_compile_on)
    {
        return all_errors.join("\n").toStdString() +
            policy_script + nat_script + routing_script;
    }

    QString script_buffer = assembleFwScript(fw, !cluster_id.empty(), oscnf.get());

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
        abort(string(" Failed to open file ") +
              fw_file_name.toStdString() +
              " for writing");
    }

    return "";
}


