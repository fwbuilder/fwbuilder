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

#include <QWidget>
#include <QtDebug>

#include "utils.h"
#include "QThreadLogger.h"
#include "SNMPCrawlerThread.h"

#ifdef HAVE_LIBSNMP

// #include snmp.h only after all Qt headers; see #2185
#include "fwbuilder/snmp.h"


using namespace std;
using namespace libfwbuilder;


SNMPCrawlerThread::SNMPCrawlerThread(QWidget *ui,
                                     const QString &seedHostName,
                                     const QString &community,
                                     bool recursive,
                                     bool followP2P,
                                     int snmpRetries,
                                     int snmpTimeout,
                                     const std::vector<InetAddrMask> *include_net)
{
    this->ui = ui;

    stop_flag = new std::atomic<bool>();

    QString seedHostAddress = getAddrByName(seedHostName, AF_INET);
    InetAddr seedHostInetAddr = InetAddr( seedHostAddress.toLatin1().constData());

    q = new SNMPCrawler();
    q->init(seedHostInetAddr,
            community.toLatin1().constData(),
            recursive,
            false,
            followP2P,
            0,
            snmpRetries,
            1000000L * snmpTimeout,
            0,
            0,
            (include_net->size() > 0) ? include_net : nullptr);

}

SNMPCrawlerThread::~SNMPCrawlerThread()
{
    if (fwbdebug) qDebug() << "SNMPCrawlerThread::~SNMPCrawlerThread()";
    delete q;
    delete stop_flag;
}

void SNMPCrawlerThread::run()
{
    QThreadLogger *logger = new QThreadLogger();
    connect(logger, SIGNAL(lineReady(QString)),
            this->ui, SLOT(logLine(QString)),
            Qt::QueuedConnection);
    q->run_impl(logger, stop_flag);

    emit finished();

    deleteLater(); // mark this object for destruction on the next run of event loop
}

void SNMPCrawlerThread::stop()
{
    stop_flag->store(true);
}

map<InetAddr, CrawlerFind>  SNMPCrawlerThread::getAllIPs()
{
    return q->getAllIPs();
}

set<InetAddrMask> SNMPCrawlerThread::getNetworks()
{
    return q->getNetworks();
}

#endif
