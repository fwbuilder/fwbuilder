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

#ifndef __ND_SETUPPAGE_H_
#define __ND_SETUPPAGE_H_

#include "ui_nd_setuppage_q.h"

#include <QHostInfo>


class ND_SetupPage : public QWizardPage
{
    Q_OBJECT;

    Ui::ND_SetupPage_q *m_dialog;
    QString last_error;
    int dns_op_id;
    bool seedHostOK;
    bool limitScanConfigurationOK;
    
    bool isSeedHostOK(const QString &s);
    bool looksLikeIpAddress(const QString &s);
    void displayStatusError(const QString &err);
    void displayStatusSuccess(const QString &err);
    void displayStatusNeutral(const QString &err);
    void showProgressBar();
    void hideProgressBar();
    
public:
    ND_SetupPage(QWidget *parent);
    virtual ~ND_SetupPage();

    virtual void initializePage();
    virtual bool validatePage();
    virtual bool isComplete() const;

public slots:
    void changedSeedHost();
    void changedLimitScanConfiguraton();
    void dnsFinish(const QHostInfo &host);
};


#endif
