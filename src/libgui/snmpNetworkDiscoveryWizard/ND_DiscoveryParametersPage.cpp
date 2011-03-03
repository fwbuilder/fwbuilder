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

#include "ND_DiscoveryParametersPage.h"

#include <QtDebug>


using namespace std;
//using namespace libfwbuilder;


#define DISCOVERY_DRUID_PREFIX "DiscoveryDruid/"
#define DISCOVERY_DRUID_SNMPRECURSIVE "SNMPRecursive"
#define DISCOVERY_DRUID_SNMPFOLLOWP2P "SNMPFollowP2P"
#define DISCOVERY_DRUID_SNMPINCLUDEUNNUMBERED "SnmpIncludeUnnumbered"
#define DISCOVERY_DRUID_SNMPDODNS "SNMPDoDNS"

ND_DiscoveryParametersPage::ND_DiscoveryParametersPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::ND_DiscoveryParametersPage_q;
    m_dialog->setupUi(this);

    m_dialog->snmpRecursive->setChecked(st->getBool(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPRECURSIVE));
    m_dialog->snmpFollowP2P->setChecked(st->getBool(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPFOLLOWP2P));
    m_dialog->snmpIncludeUnnumbered->setChecked(st->getBool(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINCLUDEUNNUMBERED));
    m_dialog->snmpDoDNS->setChecked(st->getBool(
                QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPDODNS));

    registerField("snmpRecursive", m_dialog->snmpRecursive);
    registerField("snmpFollowP2P", m_dialog->snmpFollowP2P);
    registerField("snmpIncludeUnnumbered", m_dialog->snmpIncludeUnnumbered);
    registerField("snmpDoDNS", m_dialog->snmpDoDNS);

}

void ND_DiscoveryParametersPage::initializePage()
{
    if (fwbdebug)
        qDebug() << "ND_DiscoveryParametersPage::initializePage()";

}

bool ND_DiscoveryParametersPage::validatePage()
{
    st->setBool(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPRECURSIVE,
            m_dialog->snmpRecursive->isChecked());
    st->setBool(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPFOLLOWP2P,
            m_dialog->snmpFollowP2P->isChecked());
    st->setBool(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPINCLUDEUNNUMBERED,
            m_dialog->snmpIncludeUnnumbered->isChecked());
    st->setBool(
            QString(DISCOVERY_DRUID_PREFIX) + DISCOVERY_DRUID_SNMPDODNS,
            m_dialog->snmpDoDNS->isChecked());

    return true;
}

