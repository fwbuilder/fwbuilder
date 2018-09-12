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

#ifndef _PLATFORMS_HH
#define _PLATFORMS_HH


// among other things, utils.h defines list<QStringPair> which we need here
#include "utils.h"

#include <qstring.h>
#include <qstringlist.h>

#include <list>
#include <utility>

#include "fwbuilder/Rule.h"
#include "fwbuilder/InterfaceData.h"


namespace libfwbuilder
{
    class FWOptions;
    class Firewall;
    class PolicyRule;
    class FailoverClusterGroup;
    class StateSyncClusterGroup;
};

void init_platforms();

bool isUsingNetZone(libfwbuilder::Firewall *fw);

bool isDefaultPolicyRuleOptions(libfwbuilder::FWOptions *opt);
bool isDefaultNATRuleOptions(libfwbuilder::FWOptions *opt);
bool isDefaultRoutingRuleOptions(libfwbuilder::FWOptions *opt);

void setDefaultFailoverGroupAttributes(libfwbuilder::FailoverClusterGroup *grp);
void setDefaultStateSyncGroupAttributes(libfwbuilder::StateSyncClusterGroup *grp);

// using list of pairs instead of a map or QMap because maps are dictionaries
// and do not preserve order of elements
void getVersionsForPlatform(const QString &platform, std::list<QStringPair> &list);

QString getVersionString(const QString &platform, const QString &version);

/*
 * Get list of supported state synchronization protocols for given
 * cluster host OS. This is used for the "type" in StateSyncClusterGroup
 */
void getStateSyncTypesForOS(const QString &host_os, std::list<QStringPair> &list);

/*
 * Get list of supported failover protocols for given cluster
 * host OS. This is used for the "type" in FailoverClusterGroup
 */
void getFailoverTypesForOS(const QString &host_os, std::list<QStringPair> &list);

/*
 * Get list of supported interface types for the "advanced" interface
 * options dialog. Returns list of string pairs, <name, description>
 */
void getInterfaceTypes(libfwbuilder::Interface *iface, std::list<QStringPair> &list);

/*
 * Get list of allowed subinterface types for the given interface
 */
void getSubInterfaceTypes(libfwbuilder::Interface *iface, std::list<QStringPair> &list);

/*
 * Fill QComboBox widget with interface types
 */
void setInterfaceTypes(QComboBox *iface_type,
                       libfwbuilder::Interface *iface,
                       const QString &current_type);

/**
 *  this method is a collection of heuristics that allow us to assign
 *  a reasonable label to the interface based on firewall platform,
 *  name of the interface, its label and other parameters.
 */
void guessInterfaceLabel(libfwbuilder::InterfaceData *idata);

/**
 *  this method is a collection of heuristics that allow us to make an
 *  educated guess about interface's security level based on the
 *  firewall platform, name of the interface, its label and other
 *  parameters.
 */
void guessSecurityLevel(const std::string &platform,
                        libfwbuilder::InterfaceData *idata);

/**
 * !!! returns a list of log levels that can be used to populate qcombobox
 * !!! widget.  I do not see how log levels can be different for various
 * !!! fw platforms, but who knows.
 */
const QStringList& getLogLevels(const QString &platform);

/**
 * like the above, except returns a list of log facilities.
 */
const QStringList& getLogFacilities(const QString &platform);

/**
 * returns a list of Actions on reject (mapping list) 
 * 
 */
const QStringList& getActionsOnReject(const QString &platform);

/**
 * returns a list of options for Route action
 * 
 */
const QStringList& getRouteOptions_pf_ipf(const QString &platform);

const QStringList& getRouteLoadOptions_pf(const QString &platform);

const QStringList& getClassifyOptions_ipfw(const QString &platform);

/**
 * returns a list of Limit Suffixes (mapping list)
 */
const QStringList& getLimitSuffixes(const QString &platform);

/**
 * returns a list of screen names from the mapping list that can be 
 * used to populate qcombobox.
 */
QStringList getScreenNames(const QStringList &sl);

/**
 * finds screen name (i.e. string that can be localized) for the
 * internal item name s in the mapping list sl
 */
QString getScreenName(QString s,const QStringList &sl);

QString getRuleAction(libfwbuilder::Rule *rule);

QString getActionNameForPlatform(libfwbuilder::Firewall *fw,
                                 libfwbuilder::Rule *rule);

QString getActionNameForPlatform(libfwbuilder::Firewall *fw,
                                 const std::string &action);

bool getStatelessFlagForAction(libfwbuilder::PolicyRule *rule);

QString getReadableRuleElementName(const std::string &platform,
                                   const std::string &rule_element_type_name);

/*
 * convenience method that calls Resourcess::getPlatforms() and
 * converts the result to QMap<QString,QString>. If @filter is true,
 * platforms marked as disabled in global FWBSettings are dropped.
 */
QMap<QString,QString> getAllPlatforms(bool filter=true);

QMap<QString,QString> getAllOS(bool filter=true);

QString readPlatform(QComboBox *platform);
QString readHostOS(QComboBox *hostOS);

void setPlatform(QComboBox *platform, const QString &pl);
void setHostOS(QComboBox *hostOS, const QString &platform, const QString &os);

void guessOSAndPlatformFromSysDescr(const QString &sysDescr,
                                    QString &platform, QString &hostOS, QString &version);

QString findBestVersionMatch(const QString &platform,
                             const QString &discovered_version);

/*
 * Internal: Auxiliary function that copies elements from the list returned by
 * Resources::getResourceStrList()  to the list of string pairs
 */
void _repackStringList(std::list<std::string> &list1,
                       std::list<QStringPair> &list2);

#endif

