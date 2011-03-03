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
#include "FWBSettings.h"

#include "ND_SetupPage.h"

#include "fwbuilder/InetAddr.h"

#include <QHostInfo>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;

#define DISCOVERY_DRUID_PREFIX "DiscoveryDruid/"
#define DISCOVERY_DRUID_SEEDHOST "SeedHost"
#define DISCOVERY_DRUID_SNMPINADDR "SNMPInAddr"
#define DISCOVERY_DRUID_SNMPINMASK "SNMPInMask"

ND_SetupPage::ND_SetupPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::ND_SetupPage_q;
    m_dialog->setupUi(this);

    hideProgressBar();
    displayStatusNeutral("");

    dns_op_id = -1;
    seedHostOK = false;
    limitScanConfigurationOK = true;

    m_dialog->seedHostName->setText(st->getStr(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SEEDHOST));
    m_dialog->snmpInAddr->setText(st->getStr(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINADDR));
    m_dialog->snmpInMask->setText(st->getStr(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINMASK));

    registerField("seedHostName*", m_dialog->seedHostName);
    registerField("snmpInAddr", m_dialog->snmpInAddr);
    registerField("snmpInMask", m_dialog->snmpInMask);
}

ND_SetupPage::~ND_SetupPage() {}

void ND_SetupPage::initializePage()
{
    if (fwbdebug)
        qDebug() << "ND_SetupPage::initializePage()";
}

bool ND_SetupPage::validatePage()
{
    st->setStr(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SEEDHOST,
            m_dialog->seedHostName->text());
    st->setStr(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINADDR,
            m_dialog->snmpInAddr->text());
    st->setStr(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINMASK,
            m_dialog->snmpInMask->text());

    return true;
}

bool ND_SetupPage::isComplete() const
{
    return seedHostOK && limitScanConfigurationOK;
}

bool ND_SetupPage::isSeedHostOK(const QString &hostName)
{
    if (hostName.isEmpty()) return false;

    QRegExp r = QRegExp("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$",
                        Qt::CaseInsensitive); //non wildcard

    last_error = "";

    if (r.exactMatch(hostName))
    {
        try
        {
            InetAddr(hostName.toLatin1().constData());
            return true;
        } catch(const FWException &ex)
        {
            last_error = ex.toString().c_str();
            return false;
        }
    }
    else
    {
        last_error = tr("Wrong IPv4 format");
        return false;
    }
}

bool ND_SetupPage::looksLikeIpAddress(const QString &s)
{
    QRegExp r=QRegExp("^(\\d|\\.)+$",Qt::CaseInsensitive); //non wildcard
    return r.exactMatch(s);
}

void ND_SetupPage::displayStatusError(const QString &err)
{
    QPalette palette = m_dialog->seedHostErrorMessage->palette();
    palette.setColor(
        m_dialog->seedHostErrorMessage->foregroundRole(), Qt::darkRed);
    m_dialog->seedHostErrorMessage->setPalette(palette);
    m_dialog->seedHostErrorMessage->setText(err);
}

void ND_SetupPage::displayStatusSuccess(const QString &err)
{
    QPalette palette = m_dialog->seedHostErrorMessage->palette();
    palette.setColor(
        m_dialog->seedHostErrorMessage->foregroundRole(), Qt::darkGreen);
    m_dialog->seedHostErrorMessage->setPalette(palette);
    m_dialog->seedHostErrorMessage->setText(err);
}

void ND_SetupPage::displayStatusNeutral(const QString &err)
{
    QPalette palette = m_dialog->seedHostErrorMessage->palette();
    palette.setColor(
        m_dialog->seedHostErrorMessage->foregroundRole(), Qt::black);
    m_dialog->seedHostErrorMessage->setPalette(palette);
    m_dialog->seedHostErrorMessage->setText(err);
}

void ND_SetupPage::showProgressBar()
{
    m_dialog->DNSProgressBar->show();
}

void ND_SetupPage::hideProgressBar()
{
    m_dialog->DNSProgressBar->hide();
}

void ND_SetupPage::changedSeedHost()
{
    m_dialog->seedHostErrorMessage->setText(" ");

    hideProgressBar();

    QString hostName = m_dialog->seedHostName->text();

    if (dns_op_id > -1)
    {
        QHostInfo::abortHostLookup(dns_op_id);
        dns_op_id = -1;
    }

    if (hostName.isEmpty())
    {
        displayStatusError(tr("Enter a valid host name or address."));
    }
    else
    {
        if (looksLikeIpAddress(hostName))
        { 
            // seems to be an IP Address
            seedHostOK = isSeedHostOK(hostName);
            if (seedHostOK)
                displayStatusSuccess(tr("Address successfully verified"));
            else
                displayStatusError(last_error);
        }
        else
        {
            // it looks like a DNS name
            displayStatusNeutral(tr("DNS resolution in progress..."));
            showProgressBar();
            dns_op_id = QHostInfo::lookupHost(hostName,
                                              this, SLOT(dnsFinish(QHostInfo)));
        }
    }

    emit completeChanged();
}

void ND_SetupPage::dnsFinish(const QHostInfo &host)
{
    dns_op_id = -1;

    QList<QHostAddress> list = host.addresses();

    hideProgressBar();

    //get the test result
    if (list.isEmpty())
    {
        displayStatusError(tr( "host name not found"));
        seedHostOK = false;
    }
    else
    {
        displayStatusSuccess(tr("host name verified"));
        seedHostOK = true;
    }

    emit completeChanged();
}

void ND_SetupPage::changedLimitScanConfiguraton()
{
    m_dialog->limitScanError->setText(" ");

    if (m_dialog->snmpInAddr->text().isEmpty() &&
        m_dialog->snmpInMask->text().isEmpty())
    {
        m_dialog->limitScanError->setText(" ");
        limitScanConfigurationOK = true;
        emit completeChanged();
        return;
    }

    if ( ! m_dialog->snmpInAddr->text().isEmpty() &&
         ! m_dialog->snmpInMask->text().isEmpty())
    {
        try
        {
            InetAddr a(m_dialog->snmpInAddr->text().toLatin1().constData());
            InetAddr n(m_dialog->snmpInMask->text().toLatin1().constData());
            InetAddrMask(a, n);

            m_dialog->limitScanError->setText(" ");
            limitScanConfigurationOK = true;

        } catch (const FWException &ex)
        {
            m_dialog->limitScanError->setText(ex.toString().c_str());
            limitScanConfigurationOK = false;
        }
    }
    else
    {
        m_dialog->limitScanError->setText(tr("Incomplete network address / netmask"));
        limitScanConfigurationOK = false;
    }

    emit completeChanged();
}

