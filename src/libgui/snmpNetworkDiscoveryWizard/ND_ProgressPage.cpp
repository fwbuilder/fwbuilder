/*

                          Firewall Builder

                 Copyright (C) 2011 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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
#include "utils.h"
#include "FWWindow.h"
#include "utils.h"
#include "FWBSettings.h"

#include "ND_ProgressPage.h"
#include "SNMPCrawlerThread.h"

#include "fwbuilder/snmp.h"

#include <QFileDialog>
#include <QFile>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;

bool fwbdebug_nd = fwbdebug;


ND_ProgressPage::ND_ProgressPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::ND_ProgressPage_q;
    m_dialog->setupUi(this);

    crawler = NULL;

    QTextCursor cursor(m_dialog->discoveryLog->textCursor());
    normal_format = cursor.charFormat();

    error_format = normal_format;
    error_format.setForeground(QBrush(Qt::red));
    error_format.setAnchorHref("http://somewhere.com");
    error_format.setAnchor(true);
    // weight must be between 0 and 99. Qt 4.4.1 does not seem to mind if
    // it is >99 (just caps it) but older versions assert
    error_format.setProperty(QTextFormat::FontWeight, 99);

    warning_format = normal_format;
    warning_format.setForeground(QBrush(Qt::blue));
    warning_format.setProperty(QTextFormat::FontWeight, 99);
    warning_format.setAnchor(true);
    warning_format.setAnchorHref("http://somewhere.com");
}

ND_ProgressPage::~ND_ProgressPage()
{
    if (fwbdebug_nd) qDebug() << "ND_ProgressPage::~ND_ProgressPage()";

    disconnect(this, SLOT(logLine(QString)));

    if (crawler != NULL && crawler->isRunning())
    {
        if (fwbdebug_nd)
            qDebug() << "ND_ProgressPage::initializePage()"
                     << "crawler is still runnig; stopping";
        crawler->stop();
        // crawler->wait();
        // do not delete crawler thread object, we call deleteLater()
        // in SNMPCrawlerThread::run() to make sure crawler thread
        // object is only deleted after snmp crawler has finished and
        // thread terminated
    }
}

void ND_ProgressPage::crawlerDestroyed(QObject *obj)
{
    if (fwbdebug_nd)
        qDebug() << "ND_ProgressPage::crawlerDestroyed() obj=" << obj;
    if (obj == crawler) crawler = NULL;
}

void ND_ProgressPage::initializePage()
{
    if (fwbdebug_nd) qDebug() << "ND_ProgressPage::initializePage()";

#ifdef HAVE_LIBSNMP
    QString seedHostName = field("seedHostName").toString();
    QString snmpInclAddr = field("snmpInAddr").toString();
    QString snmpInclMask = field("snmpInMask").toString();

    bool snmpRecursive = field("snmpRecursive").toBool();
    bool snmpFollowP2P = field("snmpFollowP2P").toBool();
//    bool snmpIncludeUnnumbered = field("snmpIncludeUnnumbered").toBool();

    QString snmpCommunity = field("snmpCommunity").toString();
    int snmpRetries = field("snmpRetries").toInt();
    int snmpTimeoutSec = field("snmpTimeout").toInt();

    QString seedHostAddress = getAddrByName(seedHostName, AF_INET);
    InetAddr seedHostInetAddr = InetAddr( seedHostAddress.toLatin1().constData() );

    vector<libfwbuilder::InetAddrMask>  include_networks;
    
    bool limit_scan = false;
    if ( ! snmpInclAddr.isEmpty() && ! snmpInclMask.isEmpty())
    {
        try
        {
            InetAddrMask in(
                 InetAddr(snmpInclAddr.toStdString()),
                 InetAddr(snmpInclMask.toStdString())
                 );
            include_networks.push_back(in);
            limit_scan = true;
        }
        catch (const FWException &ex)
        {
            //TODO: do something usefull
        }
    }

    if (crawler != NULL && crawler->isRunning())
    {
        if (fwbdebug_nd)
            qDebug() << "ND_ProgressPage::initializePage()"
                     << "crawler is still runnig; stopping";
        crawler->stop();
        crawler->wait();
        delete crawler;
    }

    // note that crawler deletes itself using call to deleteLater() after
    // underlying SNMPCrawler finishes its work.
    crawler = new SNMPCrawlerThread(this,
                                    seedHostName,
                                    snmpCommunity,
                                    snmpRecursive,
                                    snmpFollowP2P,
                                    snmpRetries,
                                    snmpTimeoutSec,
                                    &include_networks);
    connect(crawler, SIGNAL(destroyed(QObject*)),
            this, SLOT(crawlerDestroyed(QObject*)));
    crawler->start();

#endif

}

void ND_ProgressPage::cleanupPage()
{
    if (fwbdebug_nd) qDebug() << "ND_ProgressPage::cleanupPage()";
    disconnect(this, SLOT(logLine(QString)));
    if (crawler != NULL && crawler->isRunning()) crawler->stop();
}

void ND_ProgressPage::stop()
{
    if (crawler != NULL && crawler->isRunning())
    {
        logLine(tr("Stopping network crawler process..."));
        crawler->stop();
    }
}


void ND_ProgressPage::logLine(const QString &buf)
{
    if (buf.isEmpty()) return;

    foreach(QString line, buf.trimmed().split("\n"))
    {
        QTextCharFormat format = normal_format;

        if (line.contains("Parser error"))
            format = error_format;

        if (line.contains("Parser warning"))
            format = warning_format;

        if (line.contains("SNMP error, status 2 Timeout"))
            format = warning_format;

        QString txt = line;
        while (!txt.isEmpty() && (txt.endsWith("\n") || txt.endsWith("\r")))
            txt.chop(1);

        if (format == error_format || format == warning_format)
            format.setAnchorHref(txt);

        QTextCursor cursor = m_dialog->discoveryLog->textCursor();
        cursor.insertBlock();
        cursor.insertText(txt, format);
    }

    m_dialog->discoveryLog->ensureCursorVisible();

}

void ND_ProgressPage::saveLog()
{
    QString dir;
    dir = st->getWDir();
    if (dir.isEmpty())  dir = st->getOpenFileDir();
    if (dir.isEmpty())  dir = "~";

    QString s = QFileDialog::getSaveFileName(
        this,
        "Choose a file",
        dir,
        "Text file (*.txt)");

    if (!s.isEmpty())
    {
        if (s.endsWith(".txt"))
        {
            s += ".txt";
        }
        QFile f(s);
        if (f.open(QIODevice::WriteOnly))
        {
            if (fwbdebug)
            {
                qDebug("Saving crawler log to file: %d chars",
                       m_dialog->discoveryLog->toPlainText().length());
                qDebug("--------------------------------");
            }
            QTextStream strm(&f);
            QString txt = m_dialog->discoveryLog->toPlainText();
            strm << txt << endl;
            if (fwbdebug)
            {
                qDebug("%s",txt.toAscii().constData());
                qDebug("--------------------------------");
            }
            f.close();
        }
    }
}

