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

#include "../../config.h"

// among other things, utils.h defines list<QStringPair> which we need here
#include "utils.h"

#include <qstring.h>
#include <qstringlist.h>

#include <list>
#include <utility>

#include <fwbuilder/Rule.h>

namespace libfwbuilder {
    class FWOptions;
    class Firewall;
    class PolicyRule;
};

void init_platforms();

bool isUsingNetZone(libfwbuilder::Firewall *fw);

bool isDefaultPolicyRuleOptions(libfwbuilder::FWOptions *opt);
bool isDefaultNATRuleOptions(libfwbuilder::FWOptions *opt);
bool isDefaultRoutingRuleOptions(libfwbuilder::FWOptions *opt);


// using list of pairs instead of a map or QMap because maps are dictionaries
// and do not preserve order of elements
std::list<QStringPair> getVersionsForPlatform(const QString &platform);

QString getVersionString(const QString &platform,const QString &version);

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

/**
 * returns a list of Prolog places (mapping list)
 */
const QStringList& getPrologPlaces(const QString &platform);

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

QString getActionNameForPlatform(libfwbuilder::PolicyRule::Action action,
                                 const QString &platform);

bool getStatelessFlagForAction(libfwbuilder::PolicyRule *rule);

QString getReadableRuleElementName(const std::string &rule_element_type_name);

/*
 * convenience method that calls Resourcess::getPlatforms() and
 * converts the result to QMap<QString,QString>
 */
QMap<QString,QString> getAllPlatforms();

QMap<QString,QString> getAllOS();

QString readPlatform(QComboBox *platform);
QString readHostOS(QComboBox *hostOS);

void setPlatform(QComboBox *platform, const QString &pl);
void setHostOS(QComboBox *hostOS, const QString &platform, const QString &os);

#endif

