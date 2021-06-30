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

#ifndef _SNMPCRAWLERTHREAD_H_
#define _SNMPCRAWLERTHREAD_H_


#include <QWidget>
#include <QThread>

#include <map>
#include <set>
#include <atomic>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/InetAddrMask.h"

#ifdef HAVE_LIBSNMP

// avoid #include "snmp.h" since it conflicts with Qt, see #2185
namespace libfwbuilder
{
    class SNMPCrawler;
    class CrawlerFind;
};


class SNMPCrawlerThread : public QThread
{
    Q_OBJECT;
    
    libfwbuilder::SNMPCrawler *q;
    std::atomic<bool> *stop_flag;

    QWidget *ui;
    
public:
    SNMPCrawlerThread(QWidget *ui,
                      const QString &seedHost,
                      const QString &community,
                      bool recursive,
                      bool followP2P,
                      int snmpRetries,
                      int snmpTimeout,
                      const std::vector<libfwbuilder::InetAddrMask> *include);
    virtual ~SNMPCrawlerThread();

    void run();
    void stop();

    std::map<libfwbuilder::InetAddr, libfwbuilder::CrawlerFind>  getAllIPs();
    std::set<libfwbuilder::InetAddrMask> getNetworks();
    
signals:
    void finished();
};

#endif

#endif
