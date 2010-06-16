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
#include <QDataStream>


/*
 * Create object UserWorkflow only after FWBSettings object has been
 * created and initialized.
 */
UserWorkflow::UserWorkflow()
{
    assert(st != NULL);
    start_timestamp = QDateTime::currentDateTime();
    report_query = NULL;

    QDataStream str(st->getUserWorkflowFlags());
    str.setVersion(QDataStream::Qt_4_0);
    str >> flags;

    if (fwbdebug)
        qDebug() << "UserWorkflow flags initialization:" << flagsToQueryString(0);

    // what if the user disabled tip of the day before they upgraded
    // to the version with UserWorkflow ? Or re-enabled version update
    // check ?

    registerFlag(TIP_OF_THE_DAY_DISABLED, st->getBool("UI/NoStartTip"));
    registerFlag(UPDATE_CHECKS_DISABLED, !st->getCheckUpdates());

    registerFlag(UserWorkflow::SSH_CONFIGURED, 
                 !st->getSCPPath().isEmpty() && !st->getSCPPath().isEmpty());
}

UserWorkflow::~UserWorkflow()
{
    if (report_query != NULL) delete report_query;
}

void UserWorkflow::registerFlag(enum workflowFlags e, quint32 f)
{
    if (fwbdebug)
        qDebug() << "UserWorkflow::registerFlag(" << e << "," << f << ")";
    flags.insert(e, f);

    QByteArray buffer;
    QDataStream str(&buffer, QIODevice::WriteOnly);
    str.setVersion(QDataStream::Qt_4_0);
    str << flags;

    st->setUserWorkflowFlags(buffer);
}

void UserWorkflow::registerTutorialViewing(const QString &tutorial_name)
{
    if (tutorial_name == "getting_started")
        registerFlag(UserWorkflow::GETTING_STARTED_TUTOTIAL, true);
}

QString UserWorkflow::flagsToQueryString(int session_duration)
{
    // query string of flags has the format uc=0&gs=1&ft=0 ...
    // each flag is a two-characters variable with a value of 0 or 1
    QStringList fl;
    fl << QString("co=%1").arg(flags.value(COMPILE));
    fl << QString("fn=%1").arg(flags.value(NEW_FW_NO_TEMPLATE));
    fl << QString("ft=%1").arg(flags.value(NEW_FW_WITH_TEMPLATE));
    fl << QString("gs=%1").arg(flags.value(GETTING_STARTED_TUTOTIAL));
    fl << QString("im=%1").arg(flags.value(IMPORT));
    fl << QString("in=%1").arg(flags.value(INSTALL));
    fl << QString("ir=%1").arg(flags.value(INSTALL_RAN));
    fl << QString("is=%1").arg(flags.value(INSTALL_SUCCESS));
    fl << QString("pr=%1").arg(flags.value(USING_HTTP_PROXY));
    fl << QString("rm=%1").arg(flags.value(RULE_MOD));
    fl << QString("sc=%1").arg(flags.value(SSH_CONFIGURED));
    fl << QString("sd=%1").arg(session_duration);
    fl << QString("ti=%1").arg(flags.value(TIP_OF_THE_DAY_DISABLED));
    fl << QString("uc=%1").arg(flags.value(UPDATE_CHECKS_DISABLED));
    return fl.join("&");
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
        QByteArray buffer;
        QDataStream str(&buffer, QIODevice::WriteOnly);
        str.setVersion(QDataStream::Qt_4_0);
        str << flags;

        qDebug() << "UserWorkflow::report():" << flagsToQueryString(elapsed_time);
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

    // closing report url has three parameters: 
    // %1 = version
    // %2 = uuid
    // %3 = a query string of flags in the format uc=0&gs=1&ft=0 ...
    // each flag is a two-characters variable with a value of 0 or 1

    QString url = QString(report_url)
        .arg(VERSION).arg(st->getAppGUID()).arg(flagsToQueryString(elapsed_time));

    // start http query
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
    if (report_query != NULL) report_query->deleteLater();
    report_query = NULL;
}
