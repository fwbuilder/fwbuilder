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

#include "global.h"

#include "UserWorkflow.h"
#include "FWBSettings.h"
#include "HttpGet.h"

#include <QtDebug>
#include <QTimer>

/*
 * Create object UserWorkflow only after FWBSettings object has been
 * created and initialized.
 */
UserWorkflow::UserWorkflow()
{
    assert(st != NULL);
    start_timestamp = QDateTime::currentDateTime();
    report_query = NULL;
    int flags = st->getUserWorkflowFlags();
    int f = 1;
    for (int i=0; i<32; ++i)
    {
        if (flags & f) events.insert((enum workflowEvents)(f));
        f = f << 1;
    }
}

UserWorkflow::~UserWorkflow()
{
    if (report_query != NULL) delete report_query;
}

int UserWorkflow::eventsToInt()
{
    int flags = 0;
    foreach(int f, events) flags |= f;
    return flags;
}

bool UserWorkflow::checkEvent(enum workflowEvents e)
{
    return events.contains(e);
}

void UserWorkflow::registerEvent(enum workflowEvents e)
{
    if (fwbdebug)
        qDebug() << "UserWorkflow::registerEvent():" << e;
    events.insert(e);
    st->setUserWorkflowFlags(eventsToInt());
}

void UserWorkflow::clearEvent(enum workflowEvents e)
{
    if (fwbdebug)
        qDebug() << "UserWorkflow::clearEvent():" << e;
    events.remove(e);
    st->setUserWorkflowFlags(eventsToInt());
}

void UserWorkflow::registerTutorialViewing(const QString &tutorial_name)
{
    if (tutorial_name == "getting_started")
        registerEvent(UserWorkflow::GETTING_STARTED_TUTOTIAL);
}

void UserWorkflow::report()
{
    uint elapsed_time = QDateTime::currentDateTime().toTime_t() -
        start_timestamp.toTime_t();

    // Note that QTime::elapsed() wraps to zero after ~24hr. If
    // program stayed open for over 24 hr, it would return incorrect
    // session duration.


    if (fwbdebug)
    {
        QString s("%1");
        qDebug() << "UserWorkflow::report():" << s.arg(eventsToInt(), 0, 16);
        qDebug() << "Session:" << elapsed_time << "sec";
    }

    report_query = new HttpGet();
    connect(report_query, SIGNAL(done(const QString&)),
            this, SLOT(reportDone(const QString&)));

    QString report_url = CLOSING_REPORT_URL;

    // Use env variable FWBUILDER_CLOSING_REPORT_URL to override url to test
    // e.g. export FWBUILDER_CLOSING_REPORT_URL="file://$(pwd)/report_%1"
    //
    char* report_override_url = getenv("FWBUILDER_CLOSING_REPORT_URL");
    if (report_override_url != NULL)
        report_url = QString(report_override_url);

    // start http query to get latest version from the web site
    QString url = QString(report_url)
        .arg(VERSION).arg(st->getAppGUID()).arg(eventsToInt());
    if (!report_query->get(url) && fwbdebug)
    {
        qDebug() << "HttpGet error: " << report_query->getLastError();
        qDebug() << "Url: " << url;
    }

    if (fwbdebug) qDebug() << "Request launched";
}

void UserWorkflow::reportDone(const QString& resp)
{
    if (fwbdebug) qDebug() << "UserWorkflow::reportDone" << resp;

    disconnect(report_query, SIGNAL(done(const QString&)),
               this, SLOT(reportDone(const QString&)));
    // we ignore server response for the closing reports.
    if (report_query != NULL) delete report_query;
    report_query = NULL;
}
