/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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

#ifndef __USERWORKFLOW_H_
#define __USERWORKFLOW_H_

#include <QObject>
#include <QDateTime>
#include <QHash>

#include "HttpGet.h"

/*
 * We track certain events in the new user workflow to optimize user
 * experience. This helps us understand things such as if "Getting
 * Started" tutorial helps convert users that just experiment with the
 * program into active users. We would like to know at what point
 * users abandon the program after they launch it for the first
 * time. We keep a boolean flag for each event in the settings (to
 * make it presistent) and report combination of the flags when the
 * GUI is closed.
 *
 * To report workflow statistics, the program connects to our web
 * server when user closes the program. If update checking is disabled
 * in the global preferences, this closing report is disabled as
 * well.
 *
 * Connection sends the following data to the server:
 *
 * version, uuid and a combination of bit flags of the following (each
 * flag is set once when corresponding even happens for the first
 * time; flag is not updated ever since)
 * 
 * - user disabled version update and closing reports checks (sent once when checks and reports are disabled)
 * - user looked at the "Getting Started" tutorial at least once
 * - user created their first firewall object using template
 * - user created their first firewall object without template
 * - user did their first rule modification of any kind
 * - user ran their first policy compile
 * - user ran their first policy install
 * - user ran their first policy import
 *
 */

#define CLOSING_REPORT_URL \
    "http://update.fwbuilder.org/reports/closing.cgi?v=%1&uuid=%2&%3"

class UserWorkflow : public QObject {

    Q_OBJECT;

public:

    enum workflowFlags
    {
         UPDATE_CHECKS_DISABLED,
         GETTING_STARTED_TUTOTIAL,
         NEW_FW_WITH_TEMPLATE,
         NEW_FW_NO_TEMPLATE,
         RULE_MOD,
         COMPILE,
         INSTALL,
         INSTALL_RAN,
         INSTALL_SUCCESS,
         IMPORT,
         TIP_OF_THE_DAY_DISABLED,
         USING_HTTP_PROXY,
         SSH_CONFIGURED,
         INTRO_TUTOTIAL,
    };

private:
    QDateTime start_timestamp;
    QHash<quint32, quint32> flags;
    HttpGet *report_query;

    QString flagsToQueryString(int session_duration);

public:
    UserWorkflow();
    virtual ~UserWorkflow();
    void registerFlag(enum workflowFlags e, quint32 f);
    void registerTutorialViewing(const QString &tutorial_name);
    void report();
    bool reportInProgress() { return report_query != NULL; }
    
public slots:
    void reportDone(const QString&);
};

#endif

