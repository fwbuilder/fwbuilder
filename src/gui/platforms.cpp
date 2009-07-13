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

#include "../../config.h"
#include "global.h"

#include "platforms.h"

#include <qobject.h>
#include <qstringlist.h>

#include "fwbuilder/Firewall.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Policy.h"

#include <algorithm>

using namespace std;
using namespace libfwbuilder;

QStringList emptyList;

QStringList logLevels;
QStringList logFacilities;
QStringList actionsOnReject;
QStringList routeOptions_pf_ipf;
QStringList routeLoadOptions_pf;
QStringList limitSuffixes;

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
    bool res=true;
    FWObject *p;
    PolicyRule *rule = NULL;

    p=opt;
    do {
        p=p->getParent();
        if (PolicyRule::cast(p)!=NULL) rule = PolicyRule::cast(p);
    } while ( p!=NULL && Firewall::cast(p)==NULL );

    assert(p!=NULL);

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
		   opt->getInt("limit_burst")<=0         &&
		   opt->getInt("connlimit_value")<=0     &&
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
                   ! opt->getBool("firewall_is_part_of_any_and_networks"));
	}

	if (platform=="pix" || platform=="fwsm")
        {
            string vers="version_"+p->getStr("version");
            if ( Resources::platform_res[platform.toAscii().constData()]->getResourceBool(
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
            string vers=p->getStr("version");
            if (vers=="4.x")
            {
                res= ( opt->getStr("log_prefix").empty()       &&
                       opt->getInt("pf_rule_max_state")<=0     &&
                       ! opt->getBool("pf_source_tracking")    &&
                       opt->getInt("pf_max_src_conn")<=0       &&
                       opt->getInt("pf_max_src_conn_rate_num")<=0       &&
                       opt->getInt("pf_max_src_conn_rate_seconds")<=0 &&
                       ! opt->getBool("pf_keep_state") &&
                       ! opt->getBool("pf_sloppy_tracker")
                );
            }else
            {
                res= ( opt->getStr("log_prefix").empty()       &&
                       opt->getInt("pf_rule_max_state")<=0     &&
                       ! opt->getBool("pf_source_tracking")    &&
                       opt->getInt("pf_max_src_conn")<=0       &&
                       opt->getInt("pf_max_src_conn_rate_num")<=0       &&
                       opt->getInt("pf_max_src_conn_rate_seconds")<=0 &&
                       ! opt->getBool("pf_sloppy_tracker")
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

        if (rule!=NULL)
        {
            PolicyRule::Action act=rule->getAction();
            if (act==PolicyRule::Accept || act==PolicyRule::Tag || act==PolicyRule::Route)
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
	if (platform=="iosacl")
        {
	    res = true; // ignore "stateless" option
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
    } while ( p!=NULL && Firewall::cast(p)==NULL );

    assert(p!=NULL);

    QString platform = p->getStr("platform").c_str();

//    if (fwbdebug)
//        qDebug(QString("Options object type: %1   platform: %2   pf_pool_type_none: '%3'").arg(opt->getTypeName()).arg(platform).arg(opt->getStr("pf_pool_type_none").c_str()));

    if (NATRuleOptions::isA(opt))
    {
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
    list<QStringPair> vl = getVersionsForPlatform(platform);
    list<QStringPair>::iterator li =
        std::find_if(vl.begin(),vl.end(),findFirstInQStringPair(version));
    QString readableVersion = (li!=vl.end())?li->second:"";
    return readableVersion;
}

list<QStringPair> getVersionsForPlatform(const QString &platform)
{
    list<QStringPair> res;

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
    } else
    {
        if (platform=="pix" || platform=="fwsm" || platform=="iosacl")
        {
            QString lst = Resources::platform_res[
                platform.toAscii().constData()]->getResourceStr(
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
                res.push_back(QStringPair("4.x", QObject::tr("4.x")));
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

    return res;
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

/*
 * will remap names of some actions to make it clear what commands or
 * configuration language keywords they will be translated to for the
 * target firewall platform. This should help users who are familiar
 * with the platform. There are very few places where such mapping is
 * necessary, plus we need to provide for localization of the mapped
 * names. That is why action names are not stored in platform resource
 * files and are not pulled using Rule::getActionAsString.
 */

QString getActionNameForPlatform(PolicyRule::Action action,
                                 const QString &platform)
{
    QString action_name = "";
    switch (action)
    {
    case PolicyRule::Accept:     action_name = QObject::tr("Accept");   break;

    case PolicyRule::Deny:       action_name = QObject::tr("Deny");     break;

    case PolicyRule::Reject:     action_name = QObject::tr("Reject");   break;

    case PolicyRule::Scrub:      action_name = QObject::tr("Scrub");    break;

    case PolicyRule::Return:     action_name = QObject::tr("Return");   break;

    case PolicyRule::Skip:       action_name = QObject::tr("Skip");     break;

    case PolicyRule::Continue:   action_name = QObject::tr("Continue"); break;

    case PolicyRule::Modify:     action_name = QObject::tr("Modify");   break;

    case PolicyRule::Classify:   action_name = QObject::tr("Classify"); break;

    case PolicyRule::Custom:     action_name = QObject::tr("Custom");   break;

    case PolicyRule::Branch:
        action_name = QObject::tr("Branch");
        if (platform=="iptables") action_name = QObject::tr("Chain");
        if (platform=="pf")       action_name = QObject::tr("Anchor");
        break;

    case PolicyRule::Accounting:
        action_name = QObject::tr("Accounting");
        if (platform=="ipf" || platform=="ipfw") action_name = QObject::tr("Count");
        break;

    case PolicyRule::Tag:
        action_name = QObject::tr("Tag");
        if (platform=="iptables") action_name = QObject::tr("Mark");
        break;

    case PolicyRule::Pipe:
        action_name = QObject::tr("Pipe");
        if (platform=="iptables") action_name = QObject::tr("Queue");
        break;
    case PolicyRule::Route:
        action_name = QObject::tr("Routing");
        break;
    default:
        ;
    }
    return action_name;
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
    if (act==PolicyRule::Accept ||
        act==PolicyRule::Tag    ||
        act==PolicyRule::Route) return false;
    else
        return true;
}

/**
 * Returns translatable string - name of the corresponding rule element.
 */
QString getReadableRuleElementName(const string &rule_element_type_name)
{
    // The following map TYPENAME of RuleElement classes to readable
    // translatable names.
    if (rule_element_type_name == "Src") return QObject::tr("Source");
    if (rule_element_type_name == "Dst") return QObject::tr("Destination");
    if (rule_element_type_name == "Srv") return QObject::tr("Service");
    if (rule_element_type_name == "Itf") return QObject::tr("Interface");
    if (rule_element_type_name == "When") return QObject::tr("Time");

    if (rule_element_type_name == "OSrc") return QObject::tr("Original Src");
    if (rule_element_type_name == "ODst") return QObject::tr("Original Dst");
    if (rule_element_type_name == "OSrv") return QObject::tr("Original Srv");

    if (rule_element_type_name == "TSrc") return QObject::tr("Translated Src");
    if (rule_element_type_name == "TDst") return QObject::tr("Translated Dst");
    if (rule_element_type_name == "TSrv") return QObject::tr("Translated Srv");

    if (rule_element_type_name == "RDst") return QObject::tr("Destination");
    if (rule_element_type_name == "RGtw") return QObject::tr("Gateway");
    if (rule_element_type_name == "RItf") return QObject::tr("Interface");

    // as of v3.0.x the following are not real rule elements (not separate
    // classes with names) but just attributes of corresponding Rule class.
    if (rule_element_type_name == "Direction") return QObject::tr("Direction");
    if (rule_element_type_name == "Action") return QObject::tr("Action");
    if (rule_element_type_name == "Options") return QObject::tr("Options");
    if (rule_element_type_name == "Metric") return QObject::tr("Metric");
    if (rule_element_type_name == "Comment") return QObject::tr("Comment");

    return QString();
}
