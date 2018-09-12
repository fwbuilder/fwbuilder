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

#ifndef __ND_PROGRESSPAGE_H_
#define __ND_PROGRESSPAGE_H_

// for HAVE_LIBSNMP
#include "fwbuilder/InetAddrMask.h"

#include "ui_nd_progresspage_q.h"

#include <QTextCharFormat>

class SNMPCrawlerThread;


class ND_ProgressPage : public QWizardPage
{
    Q_OBJECT;

    Ui::ND_ProgressPage_q *m_dialog;
    QTextCharFormat normal_format;
    QTextCharFormat error_format;
    QTextCharFormat warning_format;

    std::vector<libfwbuilder::InetAddrMask>  include_networks;
        
public:
    ND_ProgressPage(QWidget *parent);
    virtual ~ND_ProgressPage();

#ifdef HAVE_LIBSNMP

private:
    SNMPCrawlerThread *crawler;

    virtual void initializePage();
    virtual void cleanupPage();
    virtual bool validatePage();
    virtual bool isComplete() const;

public slots:

    void stop();
    void saveLog();
    void logLine(const QString &line);
    void crawlerDestroyed(QObject*);
    void crawlerFinished();
#endif
};


#endif
