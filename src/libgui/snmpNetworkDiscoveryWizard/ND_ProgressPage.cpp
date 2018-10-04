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

#include <QFileDialog>
#include <QFile>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QtDebug>

#include "ND_ProgressPage.h"
#include "SNMPCrawlerThread.h"
#include "SNMPNetworkDiscoveryWizard.h"

// #include snmp.h only after all Qt headers; see #2185
#include "fwbuilder/snmp.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"


using namespace std;
using namespace libfwbuilder;

bool fwbdebug_nd = false;


ND_ProgressPage::ND_ProgressPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::ND_ProgressPage_q;
    m_dialog->setupUi(this);

#ifdef HAVE_LIBSNMP
    crawler = nullptr;

    connect(m_dialog->discoveryStopButton, SIGNAL(clicked()), this, SLOT(stop()));
    connect(m_dialog->logSaveButton, SIGNAL(clicked()), this, SLOT(saveLog()));

#endif

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
    disconnect(this, SLOT(crawlerFinished()));

#ifdef HAVE_LIBSNMP

    if (crawler != nullptr && crawler->isRunning())
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
#endif
}

#ifdef HAVE_LIBSNMP

bool ND_ProgressPage::validatePage()
{
    ObjectDescriptorList *objects = 
        dynamic_cast<SNMPNetworkDiscoveryWizard*>(wizard())->getObjects();

    if (fwbdebug_nd)
        qDebug() << "ND_ProgressPage::validatePage()"
                 << "crawler=" << crawler
                 << "isRunning=" << ((crawler) ? crawler->isRunning() : 0)
                 << "objects->size()=" << objects->size();

    if (crawler != nullptr && crawler->isRunning()) return false;
    return (objects->size() > 0);
}

bool ND_ProgressPage::isComplete() const
{
    if (crawler != nullptr && crawler->isRunning()) return false;
    return true;
}

void ND_ProgressPage::crawlerDestroyed(QObject *obj)
{
    if (fwbdebug_nd) qDebug() << "ND_ProgressPage::crawlerDestroyed() obj=" << obj;
    if (obj == crawler) crawler = nullptr;
}

void ND_ProgressPage::initializePage()
{
    if (fwbdebug_nd) qDebug() << "ND_ProgressPage::initializePage()";

    ObjectDescriptorList *objects = 
        dynamic_cast<SNMPNetworkDiscoveryWizard*>(wizard())->getObjects();

    ObjectDescriptorList *networks = 
        dynamic_cast<SNMPNetworkDiscoveryWizard*>(wizard())->getNetworks();


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

    include_networks.clear();

    bool limit_scan = false;
    Q_UNUSED(limit_scan);
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

    if (crawler != nullptr && crawler->isRunning())
    {
        if (fwbdebug_nd)
            qDebug() << "ND_ProgressPage::initializePage()"
                     << "crawler is still runnig; stopping";
        crawler->stop();
        crawler->wait();
        delete crawler;
    }

    objects->clear();
    networks->clear();

    emit completeChanged();

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
    connect(crawler, SIGNAL(finished()),
            this, SLOT(crawlerFinished()));

    crawler->start();
}

void ND_ProgressPage::cleanupPage()
{
    if (fwbdebug_nd) qDebug() << "ND_ProgressPage::cleanupPage()";
    disconnect(this, SLOT(logLine(QString)));
    disconnect(this, SLOT(crawlerFinished()));
    if (crawler != nullptr && crawler->isRunning()) crawler->stop();
    include_networks.clear();
}

void ND_ProgressPage::stop()
{
    if (crawler != nullptr && crawler->isRunning())
    {
        logLine(tr("Stopping network crawler process..."));
        crawler->stop();
    }
}

/*
 * SNMPCrawlerThread emits signal finished() that should be connected
 * to this slot. We collect all the data here.
 */
void ND_ProgressPage::crawlerFinished()
{
    if (fwbdebug_nd) qDebug() << "ND_ProgressPage::crawlerFinished()";

    ObjectDescriptorList *networks = 
        dynamic_cast<SNMPNetworkDiscoveryWizard*>(wizard())->getNetworks();

    ObjectDescriptorList *objects = 
        dynamic_cast<SNMPNetworkDiscoveryWizard*>(wizard())->getObjects();


    logLine("\n");
    logLine(tr("Network crawler stopped"));

    bool snmpDoDNS = field("snmpDoDNS").toBool();

    if (crawler==nullptr) return;

    set<InetAddrMask>::iterator m;
    set<InetAddrMask> discovered_networks = crawler->getNetworks();
    map<InetAddr, CrawlerFind> discovered_addresses = crawler->getAllIPs();

    logLine(tr("Discovered %1 networks").arg(discovered_networks.size()));

    for (m=discovered_networks.begin(); m!=discovered_networks.end(); ++m)
    {
        ObjectDescriptor od;
        InetAddrMask net = *m;

        logLine(QString("network %1").arg(net.toString().c_str()));

        // if address in *m is ipv6, recreate it as Inet6AddrMask and
        // use type NetworkIPv6
        if (net.getAddressPtr()->isV6())
        {
            Inet6AddrMask in6am(*(net.getAddressPtr()),
                                *(net.getNetmaskPtr()));
            od.sysname = in6am.toString(); // different from ipv6
            od.type = NetworkIPv6::TYPENAME;
        } else
        {
            od.sysname = net.toString();
            od.type = Network::TYPENAME;
        }
        od.addr = *(net.getAddressPtr());
        od.netmask = *(net.getNetmaskPtr());
        od.isSelected = false;

        networks->push_back(od);
    }

    logLine(tr("Discovered %1 addresses").arg(discovered_addresses.size()));

    int cntr = 0;
    map<InetAddr, CrawlerFind>::iterator j;
    for(j = discovered_addresses.begin(); j!=discovered_addresses.end(); ++j,++cntr)
    {
        ObjectDescriptor od( &(j->second) );
        od.addr = j->first;
        od.type = (od.interfaces.size()>1) ? (Host::TYPENAME) : (IPv4::TYPENAME);

        od.isSelected = false;

        if (od.sysname.empty())
        {
            od.sysname = string("h-") + od.addr.toString();

            if (snmpDoDNS)
            {
                QString hostName = getNameByAddr( od.addr.toString().c_str() );
                if (!hostName.isEmpty())
                    od.sysname = hostName.toUtf8().constData();
            }

            logLine(
                QString(od.addr.toString().c_str()) + " : " + od.sysname.c_str());
        }

        if (snmpDoDNS && od.dns_info.aliases.size() > 0)
        {
            set<string>::iterator si;
            for(si=od.dns_info.aliases.begin(); si!=od.dns_info.aliases.end(); ++si)
            {
                od.sysname = (*si);
                objects->push_back(od);;
            }
        } else
            objects->push_back(od);

    }

    emit completeChanged();
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
                qDebug("%s",txt.toLatin1().constData());
                qDebug("--------------------------------");
            }
            f.close();
        }
    }
}

#endif
