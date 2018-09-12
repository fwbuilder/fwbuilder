/*

                          Firewall Builder

                 Copyright (C) 2000 NetCitadel, LLC

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

#include "global.h"

#include "platforms.h"
#include "FWBSettings.h"
#include "interfaceProperties.h"
#include "interfacePropertiesObjectFactory.h"

#include <QObject>
#include <QStringList>
#include <QComboBox>
#include <QtDebug>

#include "fwbuilder/Cluster.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Policy.h"
#include <fwbuilder/FailoverClusterGroup.h>
#include <fwbuilder/StateSyncClusterGroup.h>

#include <algorithm>
#include <iostream>
#include <memory>


using namespace std;
using namespace libfwbuilder;


QStringList emptyList;
QStringList logLevels;
QStringList logFacilities;
QStringList actionsOnReject;
QStringList routeOptions_pf_ipf;
QStringList routeLoadOptions_pf;
QStringList limitSuffixes;
QStringList classifyOptions_ipfw;

void init_platforms()
{
    logLevels.push_back("");
    logLevels.push_back("");
    logLevels.push_back(QObject::tr( "alert"  ));
    logLevels.push_back(             "alert"  );
    logLevels.push_back(QObject::tr( "crit"   ));
    logLevels.push_back(             "crit"   );
    logLevels.push_back(QObject::tr( "error"  ));
    logLevels.push_back(             "error"  );
    logLevels.push_back(QObject::tr( "warning"));
    logLevels.push_back(             "warning");
    logLevels.push_back(QObject::tr( "notice" ));
    logLevels.push_back(             "notice" );
    logLevels.push_back(QObject::tr( "info"   ));
    logLevels.push_back(             "info"   );
    logLevels.push_back(QObject::tr( "debug"  ));
    logLevels.push_back(             "debug"  );


    logFacilities.push_back("");
    logFacilities.push_back("");
    logFacilities.push_back(QObject::tr( "kern"   ));
    logFacilities.push_back(             "kern"   );
    logFacilities.push_back(QObject::tr( "user"   ));
    logFacilities.push_back(             "user"   );
    logFacilities.push_back(QObject::tr( "mail"   ));
    logFacilities.push_back(             "mail"   );
    logFacilities.push_back(QObject::tr( "daemon" ));
    logFacilities.push_back(             "daemon" );
    logFacilities.push_back(QObject::tr( "auth"   ));
    logFacilities.push_back(             "auth"   );
    logFacilities.push_back(QObject::tr( "syslog" ));
    logFacilities.push_back(             "syslog" );
    logFacilities.push_back(QObject::tr( "lpr"    ));
    logFacilities.push_back(             "lpr"    );
    logFacilities.push_back(QObject::tr( "news"   ));
    logFacilities.push_back(             "news"   );
    logFacilities.push_back(QObject::tr( "uucp"   ));
    logFacilities.push_back(             "uucp"   );
    logFacilities.push_back(QObject::tr( "cron"   ));
    logFacilities.push_back(             "cron"   );
    logFacilities.push_back(QObject::tr( "authpriv" ));
    logFacilities.push_back(             "authpriv");
    logFacilities.push_back(QObject::tr( "ftp"    ));
    logFacilities.push_back(             "ftp"    );
    logFacilities.push_back(QObject::tr( "local0" ));
    logFacilities.push_back(             "local0" );
    logFacilities.push_back(QObject::tr( "local1" ));
    logFacilities.push_back(             "local1" );
    logFacilities.push_back(QObject::tr( "local2" ));
    logFacilities.push_back(             "local2" );
    logFacilities.push_back(QObject::tr( "local3" ));
    logFacilities.push_back(             "local3" );
    logFacilities.push_back(QObject::tr( "local4" ));
    logFacilities.push_back(             "local4" );
    logFacilities.push_back(QObject::tr( "local5" ));
    logFacilities.push_back(             "local5" );
    logFacilities.push_back(QObject::tr( "local6" ));
    logFacilities.push_back(             "local6" );
    logFacilities.push_back(QObject::tr( "local7" ));
    logFacilities.push_back(             "local7" );

    actionsOnReject.push_back("");
    actionsOnReject.push_back("");
    actionsOnReject.push_back(QObject::tr("ICMP admin prohibited"));
    actionsOnReject.push_back("ICMP admin prohibited");
    actionsOnReject.push_back(QObject::tr("ICMP host prohibited"));
    actionsOnReject.push_back("ICMP host prohibited");
    actionsOnReject.push_back(QObject::tr("ICMP host unreachable"));
    actionsOnReject.push_back("ICMP host unreachable");
    actionsOnReject.push_back(QObject::tr("ICMP net prohibited"));
    actionsOnReject.push_back("ICMP net prohibited");
    actionsOnReject.push_back(QObject::tr("ICMP net unreachable"));
    actionsOnReject.push_back("ICMP net unreachable");
    actionsOnReject.push_back(QObject::tr("ICMP port unreachable"));
    actionsOnReject.push_back("ICMP port unreachable");
    actionsOnReject.push_back(QObject::tr("ICMP protocol unreachable"));
    actionsOnReject.push_back("ICMP protocol unreachable");
    actionsOnReject.push_back(QObject::tr("TCP RST"));
    actionsOnReject.push_back("TCP RST");

    routeOptions_pf_ipf.push_back(QObject::tr("None"));
    routeOptions_pf_ipf.push_back("none");
    routeOptions_pf_ipf.push_back(QObject::tr("Route through"));
    routeOptions_pf_ipf.push_back("route_through");
    routeOptions_pf_ipf.push_back(QObject::tr("Route reply through"));
    routeOptions_pf_ipf.push_back("route_reply_through");
    routeOptions_pf_ipf.push_back(QObject::tr("Route a copy through"));
    routeOptions_pf_ipf.push_back("route_copy_through");

    routeLoadOptions_pf.push_back(QObject::tr("None"));
    routeLoadOptions_pf.push_back("none");
    routeLoadOptions_pf.push_back(QObject::tr("Bitmask"));
    routeLoadOptions_pf.push_back("bitmask");
    routeLoadOptions_pf.push_back(QObject::tr("Random"));
    routeLoadOptions_pf.push_back("random");
    routeLoadOptions_pf.push_back(QObject::tr("Source Hash"));
    routeLoadOptions_pf.push_back("source_hash");
    routeLoadOptions_pf.push_back(QObject::tr("Round Robin"));
    routeLoadOptions_pf.push_back("round_robin");

    classifyOptions_ipfw.push_back(QObject::tr("None"));
    classifyOptions_ipfw.push_back("-1");
    classifyOptions_ipfw.push_back(QObject::tr("dummynet(4) 'pipe'"));
    classifyOptions_ipfw.push_back("1");
    classifyOptions_ipfw.push_back(QObject::tr("dummynet(4) 'queue'"));
    classifyOptions_ipfw.push_back("2");

    limitSuffixes.push_back("");
    limitSuffixes.push_back("");
    limitSuffixes.push_back(QObject::tr("/day"));
    limitSuffixes.push_back("/day");
    limitSuffixes.push_back(QObject::tr("/hour"));
    limitSuffixes.push_back("/hour");
    limitSuffixes.push_back(QObject::tr("/minute"));
    limitSuffixes.push_back("/minute");
    limitSuffixes.push_back(QObject::tr("/second"));
    limitSuffixes.push_back("/second");
}


bool isUsingNetZone(Firewall *fw)
{
    string platform=fw->getStr("platform");
    return (platform=="pix" || platform=="fwsm");
}

bool isDefaultPolicyRuleOptions(FWOptions *opt)
{
    bool res = true;
    FWObject *p;
    PolicyRule *rule = nullptr;

    p = opt;
    do {
        p = p->getParent();
        if (PolicyRule::cast(p)!=nullptr) rule = PolicyRule::cast(p);
    } while ( p!=nullptr && Firewall::cast(p)==nullptr );

    if (p==nullptr)
    {
        qDebug() << "isDefaultPolicyRuleOptions()"
                 << "Can not locate parent Firewall object for the options object";
        opt->dump(false, true);
        return true;
    }

    QString platform = p->getStr("platform").c_str();

//    if (fwbdebug)
//        qDebug(QString("Options object type: %1").arg(opt->getTypeName()));

    if (PolicyRuleOptions::isA(opt))
    {

	if (platform=="iptables")
        {
	    res= ( opt->getStr("log_prefix").empty() &&
                   opt->getStr("log_level").empty()      &&
		   opt->getInt("limit_value")<=0         &&
		   ! opt->getBool("limit_value_not")     &&
		   opt->getInt("limit_burst")<=0         &&
		   opt->getInt("connlimit_value")<=0     &&
		   ! opt->getBool("connlimit_above_not") &&
		   opt->getInt("connlimit_masklen")<=0   &&

		   opt->getStr("hashlimit_name").empty() &&
		   opt->getInt("hashlimit_value")<=0     &&
		   opt->getInt("hashlimit_burst")<=0     &&
		   opt->getInt("hashlimit_size")<=0      &&
		   opt->getInt("hashlimit_max")<=0       &&
		   opt->getInt("hashlimit_expire")<=0    &&
		   opt->getInt("hashlimit_gcinterval")<=0  &&

                   opt->getInt("ulog_nlgroup")<=1        &&
                   opt->getStr("limit_suffix").empty()   &&
                   opt->getStr("firewall_is_part_of_any_and_networks") == "");
	}

	if (platform=="pix" || platform=="fwsm")
        {
            string vers="version_"+p->getStr("version");
            if ( Resources::platform_res[platform.toLatin1().constData()]->getResourceBool(
                  "/FWBuilderResources/Target/options/"+vers+"/pix_rule_syslog_settings"))
            {
                res= ( opt->getStr("log_level").empty()        &&
                   opt->getInt("log_interval")<=0          &&
                   ! opt->getBool("disable_logging_for_this_rule") );
            }
            else
            {
                res=true;
            }
	}

	if (platform=="pf")
        {
            string version = p->getStr("version");
            bool ge_4_0 = XMLTools::version_compare(version, "4.0")>=0;
            bool ge_4_5 = XMLTools::version_compare(version, "4.5")>=0;
            if (ge_4_5)
            {
                res = (!opt->getBool("pf_no_sync") && !opt->getBool("pf_pflow"));
            }

            if (ge_4_0)
            {
                res = res &&
                    ( opt->getStr("log_prefix").empty()       &&
                      opt->getInt("pf_rule_max_state")<=0     &&
                      ! opt->getBool("pf_source_tracking")    &&
                      opt->getInt("pf_max_src_conn")<=0       &&
                      opt->getInt("pf_max_src_conn_rate_num")<=0       &&
                      opt->getInt("pf_max_src_conn_rate_seconds")<=0 &&
                      ! opt->getBool("pf_keep_state") &&
                      ! opt->getBool("pf_sloppy_tracker") &&
                      ! opt->getBool("pf_synproxy")    &&
                      ! opt->getBool("pf_modulate_state")
                );
            }else
            {
                res = res &&
                    ( opt->getStr("log_prefix").empty()       &&
                      opt->getInt("pf_rule_max_state")<=0     &&
                      ! opt->getBool("pf_source_tracking")    &&
                      opt->getInt("pf_max_src_conn")<=0       &&
                      opt->getInt("pf_max_src_conn_rate_num")<=0       &&
                        opt->getInt("pf_max_src_conn_rate_seconds")<=0 &&
                      ! opt->getBool("pf_keep_state") &&
                      ! opt->getBool("pf_sloppy_tracker") &&
                      ! opt->getBool("pf_synproxy")    &&
                      ! opt->getBool("pf_modulate_state")
                );
            }
	}

	if (platform=="ipf")
        {
	    res= ( opt->getStr("ipf_log_facility").empty()       &&
                   opt->getStr("log_level").empty()      &&
		   ! opt->getBool("ipf_keep_frags") &&
                   ! opt->getBool("ipf_return_icmp_as_dest") );
	}

	if (platform=="ipfw")
        {
	    //res= ( ! opt->getBool("stateless") );
            res = true;
	}

        if (rule!=nullptr)
        {
            PolicyRule::Action act = rule->getAction();

            if (act==PolicyRule::Accept)
            {
                // by default, these actions are not stateless
                res = res && (!opt->getBool("stateless"));
            } else
            {
                // other actions are stateless by default
                res = res && opt->getBool("stateless");
            }

        }

        // all rules are stateless for IOS ACL
	if (platform=="iosacl" || platform=="procurve_acl")
        {
            res = !opt->getBool("iosacl_add_mirror_rule");
        }

    }
    return res;
}

bool isDefaultNATRuleOptions(FWOptions *opt)
{
    bool res=true;
    FWObject *p;

    p=opt;
    do {  p=p->getParent();
    } while ( p!=nullptr && Firewall::cast(p)==nullptr );

    assert(p!=nullptr);

    QString platform = p->getStr("platform").c_str();

    if (NATRuleOptions::isA(opt))
    {
	if (platform=="iptables")
        {
            res = !opt->getBool("ipt_use_snat_instead_of_masq") &&
                !opt->getBool("ipt_use_masq") &&
                !opt->getBool("ipt_nat_random") &&
                !opt->getBool("ipt_nat_persistent");
        }

	if (platform=="pf")
        {
            // if "pf_pool_type_none" is undefined, then all others
            // should not be defined too because they all are set by
            // the same dialog
            // In this case consider options default.
            res = (opt->getStr("pf_pool_type_none") == "" ||
                   ( opt->getBool("pf_pool_type_none") &&
                     ! opt->getBool("pf_bitmask")      &&
                     ! opt->getBool("pf_random")       &&
                     ! opt->getBool("pf_source_hash")  &&
                     ! opt->getBool("pf_round_robin")  &&
                     ! opt->getBool("pf_static_port") ) );
	}

	if (platform=="pix" || platform=="fwsm")
        {
            res = (! opt->getBool("asa8_nat_dns") &&
                   ! opt->getBool("asa8_nat_static") &&
                   ! opt->getBool("asa8_nat_dynamic"));

        }
    }
    return res;
}

bool isDefaultRoutingRuleOptions(FWOptions *opt)
{
    bool res=true;

//    if (fwbdebug)
//        qDebug(QString("Options object type: %1").arg(opt->getTypeName()));

    if (RoutingRuleOptions::isA(opt))
    {
        res= ( ! opt->getBool("no_fail") );
    }
    return res;
}

QString getVersionString(const QString &platform, const QString &version)
{
    list<QStringPair> vl;
    getVersionsForPlatform(platform, vl);
    list<QStringPair>::iterator li =
        std::find_if(vl.begin(),vl.end(),findFirstInQStringPair(version));
    QString readableVersion = (li!=vl.end())?li->second:"";
    return readableVersion;
}

void getVersionsForPlatform(const QString &platform, std::list<QStringPair> &res)
{
/* versions are defined here instead of the resource files so that
 * strings could be localized. We use strings that can be localized
 * only for iptables but define versions for all platforms here for
 * uniformity
 */

    if (platform=="iptables")
    {
        res.push_back(QStringPair("", QObject::tr("- any -")));
        res.push_back(QStringPair("lt_1.2.6", QObject::tr("1.2.5 or earlier")));
        res.push_back(QStringPair("ge_1.2.6", QObject::tr("1.2.6 to 1.2.8")));
        res.push_back(QStringPair("1.2.9", QObject::tr("1.2.9 to 1.2.11")));
        res.push_back(QStringPair("1.3.0", QObject::tr("1.3.x")));
        res.push_back(QStringPair("1.4.0", QObject::tr("1.4.0 or later")));
        res.push_back(QStringPair("1.4.1.1", QObject::tr("1.4.1.1 or later")));
        res.push_back(QStringPair("1.4.3", QObject::tr("1.4.3")));
        res.push_back(QStringPair("1.4.4", QObject::tr("1.4.4 or later")));
        res.push_back(QStringPair("1.4.11", QObject::tr("1.4.11 or later")));
        res.push_back(QStringPair("1.4.20", QObject::tr("1.4.20 or later")));
    } else
    {
        // we list supported versions for the following platforms in
        // corresponding resource .xml file
        if (platform=="pix" ||
            platform=="fwsm" ||
            platform=="nxosacl" ||
            platform=="iosacl" ||
            platform=="procurve_acl")
        {
            QString lst = Resources::platform_res[
                platform.toLatin1().constData()]->getResourceStr(
                    "/FWBuilderResources/Target/versions").c_str();

            QStringList ll=lst.split(',');

            for (QStringList::iterator i=ll.begin(); i!=ll.end(); ++i)
                res.push_back(QStringPair(*i,*i));
        } else
        {
            if (platform=="pf")
            {
                res.push_back(QStringPair("","- any -"));
                res.push_back(QStringPair("3.x", QObject::tr("3.x")));
		res.push_back(QStringPair("ge_3.7", QObject::tr("3.7 to 3.9")));
                res.push_back(QStringPair("4.0", QObject::tr("4.0 to 4.2")));
                res.push_back(QStringPair("4.3", QObject::tr("4.3")));
                res.push_back(QStringPair("4.5", QObject::tr("4.5")));
                res.push_back(QStringPair("4.6", QObject::tr("4.6")));
                res.push_back(QStringPair("4.7", QObject::tr("4.7 and later")));
/* add pf versions here */
            } else
            {
                if (platform=="ipf")
                {
                    res.push_back(QStringPair("","- any -"));
/* add ipf versions here */
                } else
                {
                    if (platform=="ipfw")
                    {
                        res.push_back(QStringPair("","- any -"));
/* add ipfw versions here */
                    } else
                        res.push_back(QStringPair("","- any -"));
                }
            }
        }
    }
}

/*
 * ticket #58: move state sync types and failover types to resource files.
 *
 * Note: this function fills in list of QString pairs, each pair is 
 * <protocol_name>,<user_readable_protocol_name>
 * The second element in the pair is for QComboBox and is visible to the user.
 */
void getStateSyncTypesForOS(const QString &host_os, std::list<QStringPair> &res)
{
    Resources* os_res = Resources::os_res[host_os.toStdString()];
    if (os_res==nullptr) return;
    list<string> protocols;
    os_res->getResourceStrList("/FWBuilderResources/Target/protocols/state_sync",
                               protocols);
    _repackStringList(protocols, res);
}

void getFailoverTypesForOS(const QString &host_os, std::list<QStringPair> &res)
{
    Resources* os_res = Resources::os_res[host_os.toStdString()];
    if (os_res==nullptr) return;
    list<string> protocols;
    os_res->getResourceStrList("/FWBuilderResources/Target/protocols/failover",
                               protocols);
    _repackStringList(protocols, res);
}

void getInterfaceTypes(Interface *iface, list<QStringPair> &res)
{
    FWObject *fw = iface->getParent();
    string host_os = fw->getStr("host_OS");
    Resources* os_res = Resources::os_res[host_os];
    if (os_res==nullptr) return;
    list<string> interface_types;

    if (Cluster::isA(fw))
    {
        os_res->getResourceStrList("/FWBuilderResources/Target/interfaces/cluster",
                                   interface_types);
    } else
    {
        os_res->getResourceStrList("/FWBuilderResources/Target/interfaces/firewall",
                                   interface_types);
    }
    _repackStringList(interface_types, res);
}

/*
 * Return list of types of subinterfaces that given interface can have
 *
 * @iface an Interface object. This is not a subinterface, this is a
 * regular interface. This function returns list of subinterface types
 * this interface can have.
 *
 * @res a list of pairs of QString, each pair is <type>,<description>
 */
void getSubInterfaceTypes(Interface *iface, list<QStringPair> &res)
{
    FWObject *p = Host::getParentHost(iface);
    //FWObject *p = iface->getParentHost();
    assert(p!=nullptr);

    QString host_os = p->getStr("host_OS").c_str();
    Resources* os_res = Resources::os_res[host_os.toStdString()];
    if (os_res==nullptr) return;

    FWOptions *ifopt;
    ifopt = Interface::cast(iface)->getOptionsObject();
    string parent_type = ifopt->getStr("type");

    // empty parent type is equivalent to "ethernet" for backwards
    // compatibility
    if (parent_type.empty()) parent_type = "ethernet";

    QString obj_name = iface->getName().c_str();

    list<string> interface_types;
    os_res->getResourceStrList(
        "/FWBuilderResources/Target/subinterfaces/" + parent_type,
        interface_types);
    _repackStringList(interface_types, res);
}
 
void setInterfaceTypes(QComboBox *iface_type,
                       Interface *iface,
                       const QString &current_type)
{
    bool this_is_subinterface = Interface::isA(iface->getParent());
    list<QStringPair> mapping;
    if (this_is_subinterface)
        getSubInterfaceTypes(Interface::cast(iface->getParent()), mapping);
    else getInterfaceTypes(iface, mapping);

    if (st->getBool("Objects/Interface/autoconfigureInterfaces"))
    {
        // #335 : if interface name matches naming convention for vlan
        // interfaces and vlan type is in the list that came from the
        // resource file, then leave only vlan in the list we return.
        // Note that if resource file says this subint can not be vlan, we
        // dan't return vlan type on the list even if its name looks like
        // it could be one.
        FWObject *p = Host::getParentHost(iface);
        //FWObject *p = iface->getParentHost();
        assert(p!=nullptr);
        QString host_os = p->getStr("host_OS").c_str();
        QString obj_name = iface->getName().c_str();

        Resources* os_res = Resources::os_res[p->getStr("host_OS")];
        string os_family = p->getStr("host_OS");
        if (os_res!=nullptr)
            os_family = os_res->getResourceStr("/FWBuilderResources/Target/family");

        std::unique_ptr<interfaceProperties> int_prop(
            interfacePropertiesObjectFactory::getInterfacePropertiesObject(
                os_family));
        if (int_prop->looksLikeVlanInterface(obj_name))
        {
            QString parent_name = iface->getParent()->getName().c_str();
            QString err;
            if (int_prop->isValidVlanInterfaceName(obj_name, parent_name, err))
            {
                // iface can be valid vlan interface. Leave only vlan type
                // in the list if it was there to begin with. 
                for (list<QStringPair>::iterator it=mapping.begin();
                     it!=mapping.end(); ++it)
                {
                    QString itype = it->first;
                    QString rtype = it->second;
                    if (itype == "8021q")
                    {
                        mapping.clear();
                        mapping.push_back(QStringPair(itype, rtype));
                        mapping.push_back(QStringPair("unknown", "Unknown"));
                        break;
                    }
                }
            }
        }
    }

    list<QStringPair>::iterator it;
    int idx = 0;
    int unknown_idx = 0;
    int current_idx = -1;
    for (it = mapping.begin(); it != mapping.end(); it++)
    {
        if (it->first == "unknown") unknown_idx = idx;
        iface_type->addItem(it->second);
        iface_type->setItemData(idx,  QVariant(it->first));
        if (current_type == it->first)
            current_idx = idx;
        idx++;
    }
    if (current_idx >= 0)
        iface_type->setCurrentIndex(current_idx);
    else
        iface_type->setCurrentIndex(unknown_idx);
}

/* currently we return the same list for all platforms */
const QStringList& getLogLevels(const QString&)
{
    return logLevels;
}

const QStringList& getLogFacilities(const QString&)
{
    return logFacilities;
}

const QStringList& getActionsOnReject(const QString&)
{
    return actionsOnReject;
}

/*
 *  need to return mapping list for the parameter 'route_option' of
 *  action 'Routing' regardless of the firewall platform even though
 *  it only makes sense and is needed for pf and ipf. This is because
 *  ActionsDialog is designed with widget stack and therefore must
 *  always initialize widgets for all platforms. Worse, it always
 *  saves all parameters into rule options object, regardless of the
 *  platform. So, if we return an empty mapping list from this method
 *  because platform is not pf or ipf while user is editing action
 *  parameters for iptables, parameters for pf and ipf get saved
 *  uninitizalized and unmapped. QComboBox::currentText() returns the
 *  first item which goes straight into rule options object. This is
 *  ok in English locale, but breaks XML if the item has been
 *  translated and the program runs under national locale. Sigh.
 */
const QStringList& getRouteOptions_pf_ipf(const QString&)
{
    return routeOptions_pf_ipf;
}

const QStringList& getRouteLoadOptions_pf(const QString&)
{
    return routeLoadOptions_pf;
}

const QStringList& getClassifyOptions_ipfw(const QString&)
{
    return classifyOptions_ipfw;
}

const QStringList& getLimitSuffixes(const QString&)
{
    return limitSuffixes;
}


QStringList getScreenNames(const QStringList &sl)
{
    QStringList res;

    for( QStringList::const_iterator it = sl.begin();
         it!=sl.end();
         ++it,++it)
    {
        res.push_back(*it);
    }
    return res;
}

QString getScreenName(QString s, const QStringList &sl)
{
    QString res;
    for( QStringList::const_iterator it = sl.begin();
         it!=sl.end();
         ++it)
    {
        res=(*it);
        ++it;
        if ((*it)==s) break;
    }
    return res;
}

QString getRuleAction(Rule *rule)
{
    PolicyRule *policy_rule = PolicyRule::cast(rule);
    NATRule *nat_rule = NATRule::cast(rule);
    string act;
    if (policy_rule) act = policy_rule->getActionAsString();
    if (nat_rule) act = nat_rule->getActionAsString();
    return act.c_str();
}

/*
 * will remap names of some actions to make it clear what commands or
 * configuration language keywords they will be translated to for the
 * target firewall platform. This should help users who are familiar
 * with the platform. There are very few places where such mapping is
 * necessary, plus we need to provide for localization of the mapped
 * names. That is why action names are not stored in platform resource
 * files and are not pulled using Rule::getActionAsString.
 */

QString getActionNameForPlatform(Firewall *fw, Rule *rule)
{
    if (fw==nullptr) return "";
    PolicyRule *policy_rule = PolicyRule::cast(rule);
    NATRule *nat_rule = NATRule::cast(rule);
    string act;
    if (policy_rule) act = policy_rule->getActionAsString();
    if (nat_rule) act = nat_rule->getActionAsString();
    return getActionNameForPlatform(fw, act);
}

QString getActionNameForPlatform(Firewall *fw, const std::string &action)
{
    if (fw==nullptr) return "";
    string platform = fw->getStr("platform");
    string name;
    try  
    {
        name = Resources::getTargetCapabilityStr(
            platform, "actions/" + action + "/description");
    } catch (FWException &ex) { }
    return name.c_str();
}

/*
 * this function provides logic for the decision whether the rule
 * should be stateless by default. Currently it only depends on the
 * action, but may depend on the platform as well.
 *
 * actions Accept, Tag and Route by default assume the rule is
 * stateful.  Other actions by default assume it is stateless
 * and set rule option accordingly
 *
 * See bugs #1676635 and 1671910
 */
bool getStatelessFlagForAction(PolicyRule *rule)
{
    PolicyRule::Action act = rule->getAction();
    if (act==PolicyRule::Accept) return false;
    else
        return true;
}

/**
 * Returns translatable string - name of the corresponding rule element.
 */
QString getReadableRuleElementName(const string &platform,
                                   const string &re_type_name)
{
    bool nat_intf_in = Resources::getTargetCapabilityBool(
            platform, "inbound_interface_in_nat");
    bool nat_intf_out = Resources::getTargetCapabilityBool(
            platform, "outbound_interface_in_nat");

    // The following map TYPENAME of RuleElement classes to readable
    // translatable names.
    if (re_type_name == "Src") return QObject::tr("Source");
    if (re_type_name == "Dst") return QObject::tr("Destination");
    if (re_type_name == "Srv") return QObject::tr("Service");
    if (re_type_name == "Itf") return QObject::tr("Interface");
    if (re_type_name == "When") return QObject::tr("Time");

    if (re_type_name == "OSrc") return QObject::tr("Original Src");
    if (re_type_name == "ODst") return QObject::tr("Original Dst");
    if (re_type_name == "OSrv") return QObject::tr("Original Srv");

    if (re_type_name == "TSrc") return QObject::tr("Translated Src");
    if (re_type_name == "TDst") return QObject::tr("Translated Dst");
    if (re_type_name == "TSrv") return QObject::tr("Translated Srv");

    if (nat_intf_in != nat_intf_out)
    {
        // For some platforms I only show one interface column in nat
        // rules, in this case nat_intf_in and nat_intf_out have
        // different values.  For example, for PF I hide inbound
        // interface and show outbound interface column. Columns title
        // should then be just "Interface"
        if (re_type_name == "ItfInb") return QObject::tr("Interface");
        if (re_type_name == "ItfOutb") return QObject::tr("Interface");
    } else
    {
        if (re_type_name == "ItfInb") return QObject::tr("Interface In");
        if (re_type_name == "ItfOutb") return QObject::tr("Interface Out");
    }

    if (re_type_name == "RDst") return QObject::tr("Destination");
    if (re_type_name == "RGtw") return QObject::tr("Gateway");
    if (re_type_name == "RItf") return QObject::tr("Interface");

    // as of v3.0.x the following are not real rule elements (not separate
    // classes with names) but just attributes of corresponding Rule class.
    if (re_type_name == "Direction") return QObject::tr("Direction");
    if (re_type_name == "Action") return QObject::tr("Action");
    if (re_type_name == "Options") return QObject::tr("Options");
    if (re_type_name == "Metric") return QObject::tr("Metric");
    if (re_type_name == "Comment") return QObject::tr("Comment");

    return QString();
}

QMap<QString,QString> getAllPlatforms(bool filter)
{
    QMap<QString,QString> res;
    map<string,string> platforms = Resources::getPlatforms();
    map<string,string>::iterator i;
    for (i=platforms.begin(); i!=platforms.end(); i++)
    {
        QString name = i->first.c_str();
        QString res_status = Resources::platform_res[name.toStdString()]->getResourceStr(
            "/FWBuilderResources/Target/status/").c_str();
        QString status = st->getTargetStatus(name, res_status);
        if (filter && status == "disabled") continue;
        res[name] = i->second.c_str();
    }
    return res;
}

QMap<QString,QString> getAllOS(bool filter)
{
    QMap<QString,QString> res;
    map<string,string> OSs = Resources::getOS();
    map<string,string>::iterator i;
    for (i=OSs.begin(); i!=OSs.end(); i++)
    {
        QString name = i->first.c_str();
        QString res_status = Resources::os_res[name.toStdString()]->getResourceStr(
            "/FWBuilderResources/Target/status/").c_str();
        QString status = st->getTargetStatus(name, res_status);
        if (filter && status == "disabled") continue;
        res[name] = i->second.c_str();
    }
    return res;
}

QString readPlatform(QComboBox *platform)
{
    return platform->itemData(platform->currentIndex()).toString();
}

QString readHostOS(QComboBox *hostOS)
{
    return hostOS->itemData(hostOS->currentIndex()).toString();
}

/*
 * Fill combobox widget <platform> with items that exist in resources.
 * If second argument is not an empty string, make corresponding item current.
 * If it is an empty string, add an empty item on top to the combo box and make
 * it current.
 */
void setPlatform(QComboBox *platform, const QString &pl)
{
    platform->clear();
    // platforms maps platform name (pix) to readable name (Cisco PIX)
    QMap<QString,QString> platforms = getAllPlatforms();

    QMap<QString,QString>::iterator i;

    // platform_mapping maps key (<group>.<platform name>) to pair
    // <group>, <platform name>
    QMap<QString, QPair<QString, QString> > platform_mapping;
    QStringList platform_keys;

    for (i=platforms.begin(); i!=platforms.end(); i++)
    {
        QString group =
            Resources::platform_res[i.key().toLatin1().constData()]->
                getResourceStr("/FWBuilderResources/Target/group").c_str();
        QString key = group + "." + i.key();
        platform_mapping[key] = QPair<QString,QString>(group, i.key());
        platform_keys.push_back(key);
    }

    qSort(platform_keys);

    QStringList::iterator iter;
    int ind = 0;
    int cp = 0;

    if (pl.isEmpty())
    {
        platform->addItem("", "");
        cp++;
    }

    QString current_group = "";
    for (iter=platform_keys.begin(); iter!=platform_keys.end(); iter++)
    {
        if (fwbdebug) qDebug() << *iter;

        QString group = platform_mapping[*iter].first;
        QString platform_name = platform_mapping[*iter].second;

        if (platforms.count(platform_name) == 0) continue;

        if (group != current_group)
        {
            current_group = group;
            platform->insertSeparator(cp);
            cp++;
        }

        platform->addItem(platforms[platform_name], platform_name);
        // note that if pl is "", then no real platform name will
        // match it and ind will remain 0, which makes the top item in
        // the combobox current.
        if ( pl == platform_name ) ind = cp;
        cp++;
    }
    platform->setCurrentIndex( ind );
}

/*
 * Fill in "host os" combo box with list of os supported for the given
 * platform and make current host os item current.
 *
 * If platform == "", then use all known host OS but also add
 *  empty item on top of the combobox and make that item current.
 * If os == "", make the first OS in the list current.
 */
void setHostOS(QComboBox *hostOS, const QString &platform, const QString &os)
{
    hostOS->clear();

    QStringList supported_os_list;

    if (!platform.isEmpty())
    {
        Resources *platform_res = Resources::platform_res[platform.toLatin1().constData()];
        if (!platform_res)
            platform_res = Resources::platform_res["unknown"];

        QString supported_os = platform_res->
            getResourceStr("/FWBuilderResources/Target/supported_os").c_str();

        if (fwbdebug)
            qDebug("supported_os %s", supported_os.toLatin1().constData());

        if (supported_os.isEmpty())
        {
            // something is broken, we have no supported host OS for
            // this platform.  Just add os to the combo box and return
            if (fwbdebug)
                qDebug("No supported host OS for platform %s",
                       platform.toLatin1().constData());

            hostOS->addItem(os, os);
            hostOS->setCurrentIndex(0);
            return;
        }
        supported_os_list = supported_os.split(",");

        int cp = 0;
        int ind = 0;
        QMap<QString,QString> OSs = getAllOS();
        QStringList::iterator os_iter;
        for (os_iter=supported_os_list.begin();
             os_iter!=supported_os_list.end(); ++os_iter)
        {
            QString os_code = *os_iter;
            if (OSs.count(os_code) > 0)
            {
                hostOS->addItem( OSs[os_code], os_code);
                if ( os == os_code ) ind = cp;
                cp++;
            }
        }
        hostOS->setCurrentIndex( ind );
        return;
    }

    // platform is empty

    int cp = 0;
    int ind = 0;

    hostOS->addItem("", "");
    cp++;

    QMap<QString,QString> OSs = getAllOS();
    QMap<QString,QString>::iterator i;
    for (i=OSs.begin(); i!=OSs.end(); i++)
    {
        hostOS->addItem( i.value(), i.key() );
        if ( os == i.key() ) ind = cp;
        cp++;
    }

    hostOS->setCurrentIndex( ind );
}

void _repackStringList(list<string> &list1, list<QStringPair> &list2)
{
    list2.clear();
    foreach(string p, list1)
    {
        QString str = QString(p.c_str());
        QStringList pl = str.split(",");
        if (pl.size() == 1)
            list2.push_back(QStringPair(str, str));
        else
            list2.push_back(QStringPair(pl[0], pl[1]));
    }
}

void setDefaultStateSyncGroupAttributes(StateSyncClusterGroup *grp)
{
    FWObject *p = grp;
    while (p && Cluster::cast(p)==nullptr) p = p->getParent();
    assert(p != nullptr);
    Cluster *cluster = Cluster::cast(p);
    Resources *os_res = Resources::os_res[cluster->getStr("host_OS")];
    assert(os_res != nullptr);

    list<string> protocols;
    os_res->getResourceStrList("/FWBuilderResources/Target/protocols/state_sync",
                               protocols);

    QStringList protocol_names = QString(protocols.front().c_str()).split(",");

    grp->setName(protocol_names[1].toStdString());
    grp->setStr("type", protocol_names[0].toStdString());
}

void setDefaultFailoverGroupAttributes(FailoverClusterGroup *grp)
{
    FWObject *p = grp;
    while (p && Cluster::cast(p)==nullptr) p = p->getParent();
    assert(p != nullptr);
    Cluster *cluster = Cluster::cast(p);
    Resources *os_res = Resources::os_res[cluster->getStr("host_OS")];
    assert(os_res != nullptr);

    FWOptions *gropt = grp-> getOptionsObject();
    assert(gropt != nullptr);
    
    string failover_protocol = grp->getStr("type");

    if (failover_protocol == "carp")
    {
        gropt->setStr("carp_password", "");
        gropt->setInt("carp_vhid", 1);
        gropt->setInt("carp_advbase", 1);
        gropt->setInt("carp_master_advskew", 10);
        gropt->setInt("carp_default_advskew", 20);
    }

    if (failover_protocol == "vrrp")
    {
        gropt->setStr("vrrp_secret", "");
        gropt->setInt("vrrp_vrid", 1);
        gropt->setBool("vrrp_over_ipsec_ah", false);
    }

    if (failover_protocol == "heartbeat")
    {
        string default_address =
            os_res->getResourceStr(
                "/FWBuilderResources/Target/protocols/heartbeat/default_address");
        string default_port =
            os_res->getResourceStr(
                "/FWBuilderResources/Target/protocols/heartbeat/default_port");

        gropt->setStr("heartbeat_address", default_address);
        gropt->setStr("heartbeat_port", default_port);
        gropt->setBool("heartbeat_unicast", false);
    }

    if (failover_protocol == "openais")
    {
        string default_address =
            os_res->getResourceStr(
                "/FWBuilderResources/Target/protocols/openais/default_address");
        string default_port =
            os_res->getResourceStr(
                "/FWBuilderResources/Target/protocols/openais/default_port");
        gropt->setStr("openais_address", default_address);
        gropt->setStr("openais_port", default_port);
    }

    if (failover_protocol == "pix_failover")
    {
        gropt->setStr("pix_failover_key", "");
    }
}

void guessInterfaceLabel(InterfaceData *idata)
{
/*
 *  some firewalls report fairly regular names for interfaces through
 *  their built-in SNMP agent. We can use this to assign labels
 *  automatically.
 *
 *  in PIX interfaces have names like "PIX Firewall 'inside' interface"
 *
 */
    QString qs_name = idata->name.c_str();
    QString qs_label;

    QRegExp pat1("Adaptive Security Appliance '(.*)' interface");
    QRegExp pat2("Cisco PIX Security Appliance '(.*)' interface");
    QRegExp pat3("PIX Firewall '(.*)' interface");

    if (pat1.indexIn(qs_name) > -1) qs_label = pat1.cap(1);
    if (pat2.indexIn(qs_name) > -1) qs_label = pat2.cap(1);
    if (pat3.indexIn(qs_name) > -1) qs_label = pat3.cap(1);

    idata->label = qs_label.toStdString();

    if ( ! idata->isDyn &&
         ! idata->isUnnumbered &&
         ! idata->isBridgePort &&
         idata->addr_mask.size()!=0 &&
         idata->addr_mask.front()->getAddressPtr()->toString() == InetAddr::getLoopbackAddr().toString())
        idata->label = "loopback";
}

void guessSecurityLevel(const string&, InterfaceData *idata)
{
    InetAddrMask n10(InetAddr("10.0.0.0"), InetAddr("255.0.0.0"));
    InetAddrMask n172(InetAddr("172.16.0.0"), InetAddr("255.240.0.0"));
    InetAddrMask n192(InetAddr("192.168.0.0"), InetAddr("255.255.0.0"));

    idata->securityLevel = -1;

    string llbl = idata->label;

    for (string::size_type i=0; i<llbl.length(); i++)
        llbl[i] = tolower( llbl[i] );

    if ( llbl=="out" ||
         llbl=="ext" ||
         llbl=="internet" ||
         llbl=="wan" ||
         llbl=="dsl" ||
         llbl=="cable" ||
         llbl.find("outside")!=string::npos ||
         llbl.find("external")!=string::npos) idata->securityLevel = 0;

    if ( llbl=="lan" ||
         llbl=="in" ||
         llbl.find("inside")!=string::npos ||
         llbl.find("internal")!=string::npos ) idata->securityLevel = 100;

    if ( llbl.find("dmz")!=string::npos ) idata->securityLevel = 50;

    if ((*(idata->addr_mask.front()->getAddressPtr()))==InetAddr::getLoopbackAddr())
        idata->securityLevel = 100; 

    if (idata->name=="Null0") idata->securityLevel = 100; 

    if (idata->securityLevel==-1 &&
        ! idata->isDyn && ! idata->isUnnumbered && ! idata->isBridgePort)
    {
        if (n10.belongs(InetAddr(*(idata->addr_mask.front()->getAddressPtr()))))
            idata->securityLevel = 100;
        
        if (n172.belongs(InetAddr(*(idata->addr_mask.front()->getAddressPtr()))))
            idata->securityLevel = 100;

        if (n192.belongs(InetAddr(*(idata->addr_mask.front()->getAddressPtr()))))
            idata->securityLevel = 100;
    }

    if (idata->isDyn || idata->isUnnumbered || idata->isBridgePort)
        idata->securityLevel = 0;

    if (idata->securityLevel==-1) idata->securityLevel = 0;
}

void guessOSAndPlatformFromSysDescr(
    const QString &sysDescr, QString &platform, QString &hostOS, QString &version)
{
    QList<QRegExp> pix_re;
    pix_re << QRegExp("Cisco PIX Firewall Version ([0-9\\.]+)")
           << QRegExp("Cisco PIX Security Appliance Version ([0-9\\.]+)")
           << QRegExp("Cisco Adaptive Security Appliance Version ([0-9\\.]+)");

    QList<QRegExp> ios_re;
    ios_re << QRegExp("Cisco Internetwork Operating System Software .* Version ([0-9\\.]+)");

    platform = "";
    hostOS = "";
    version = "";

    if (fwbdebug)
        qDebug() << "guessOSAndPlatformFromSysDescr:"
                 << "sysdescr=" << sysDescr;

    list<QStringPair> allowed_versions;
    QString version_from_sysdescr;

    foreach (QRegExp re, pix_re)
    {
        if (re.indexIn(sysDescr) > -1)
        {
            platform = "pix";
            hostOS = "pix_os";
            version_from_sysdescr = re.cap(1);
        }
    }


    foreach (QRegExp re, ios_re)
    {
        if (re.indexIn(sysDescr) > -1)
        {
            platform = "iosacl";
            hostOS = "ios";
            version_from_sysdescr = re.cap(1);
        }
    }

    if (fwbdebug)
        qDebug() << "guessOSAndPlatformFromSysDescr:"
                 << "platform=" << platform
                 << "hostOS=" << hostOS
                 << "version=" << version_from_sysdescr;

    if ( ! platform.isEmpty())
        version = findBestVersionMatch(platform, version_from_sysdescr);

}

QString findBestVersionMatch(const QString &platform,
                             const QString &discovered_version)
{
    list<QStringPair> allowed_versions;

    getVersionsForPlatform(platform, allowed_versions);

    if ( ! discovered_version.isEmpty())
    {
        QString version_fit;
        list<QStringPair>::iterator it;
        foreach (QStringPair p, allowed_versions)
        {
            QString vers = p.first;
            if (XMLTools::version_compare(vers.toStdString(),
                                          discovered_version.toStdString())>0)
                break;
            version_fit = vers;
        }
        return version_fit;
    }
    return "";
}



