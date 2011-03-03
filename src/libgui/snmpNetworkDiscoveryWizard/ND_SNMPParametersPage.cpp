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

#include "ND_SNMPParametersPage.h"

#include <QtDebug>


using namespace std;
//using namespace libfwbuilder;


#define DISCOVERY_DRUID_PREFIX "DiscoveryDruid/"
#define DISCOVERY_DRUID_SNMPCOMMUNITY "SNMPCommunity"
#define DISCOVERY_DRUID_SNMPRETRIES "SNMPRetries"
#define DISCOVERY_DRUID_SNMPTIMEOUT "SNMPTimeout"


ND_SNMPParametersPage::ND_SNMPParametersPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::ND_SNMPParametersPage_q;
    m_dialog->setupUi(this);

    QString s = st->getStr(
        QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPCOMMUNITY);
    m_dialog->snmpCommunity->setText((s.isEmpty())?"public":s);

    int i = st->getInt(
        QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPRETRIES);
    m_dialog->snmpRetries->setValue((i)?i:1);

    i = st->getInt(
        QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPTIMEOUT);
    m_dialog->snmpTimeout->setValue((i)?i:2);

    registerField("snmpCommunity", m_dialog->snmpCommunity);
    registerField("snmpRetries", m_dialog->snmpRetries);
    registerField("snmpTimeout", m_dialog->snmpTimeout);
}

void ND_SNMPParametersPage::initializePage()
{
    if (fwbdebug)
        qDebug() << "ND_SNMPParametersPage::initializePage()";
}

bool ND_SNMPParametersPage::validatePage()
{
    st->setStr(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPCOMMUNITY,
            m_dialog->snmpCommunity->text());
    st->setInt(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPRETRIES,
            m_dialog->snmpRetries->value());
    st->setInt(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPTIMEOUT,
            m_dialog->snmpTimeout->value());

    return true;
}

