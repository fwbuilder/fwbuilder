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
#include "events.h"
#include "FWBSettings.h"
#include "networkZoneManager.h"

#include "IC_NetworkZonesPage.h"
#include "ImportFirewallConfigurationWizard.h"

#include "fwbuilder/Interface.h"
#include "fwbuilder/Firewall.h"

#include <QComboBox>
#include <QTableWidgetItem>
#include <QList>


using namespace std;
using namespace libfwbuilder;


IC_NetworkZonesPage::IC_NetworkZonesPage(QWidget *parent) :
    QWizardPage(parent),
    m_dialog(new Ui::IC_NetworkZonesPage_q)
{
    m_dialog->setupUi(this);
}

IC_NetworkZonesPage::~IC_NetworkZonesPage()
{
    delete m_dialog;
}

void IC_NetworkZonesPage::initializePage()
{
    Firewall *fw = 
        dynamic_cast<ImportFirewallConfigurationWizard*>(wizard())->getFirewall();

    m_dialog->iface_nz_list->clear();

    QStringList labels;
    labels << QObject::tr("Name") << QObject::tr("Label")
           << QObject::tr("Address") << QObject::tr("Network Zone");
    m_dialog->iface_nz_list->setHorizontalHeaderLabels(labels);

    NetworkZoneManager netzone_manager;
    netzone_manager.load(
        dynamic_cast<ImportFirewallConfigurationWizard*>(wizard())->db());

    list<FWObject*> all_interfaces = fw->getByTypeDeep(Interface::TYPENAME);
    list<FWObject*>::iterator it;
    int row = 0;
    for (it=all_interfaces.begin(); it!=all_interfaces.end(); ++it)
    {
        Interface *iface = Interface::cast(*it);

        m_dialog->iface_nz_list->insertRow(row);

        QTableWidgetItem* itm;

        itm = new QTableWidgetItem(iface->getName().c_str());
        itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);
        m_dialog->iface_nz_list->setItem(row, 0, itm);

        itm = new QTableWidgetItem(iface->getLabel().c_str());
        itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);
        m_dialog->iface_nz_list->setItem(row, 1, itm);

        QString addr_str;
        const InetAddr* addr = iface->getAddressPtr();
        if (addr) addr_str = addr->toString().c_str();

        itm = new QTableWidgetItem(addr_str);
        itm->setFlags(itm->flags() & ~Qt::ItemIsEditable);
        m_dialog->iface_nz_list->setItem(row, 2, itm);

        QComboBox *widget = new QComboBox();
        netzone_manager.packComboBox(widget, -1);
        m_dialog->iface_nz_list->setCellWidget(row, 3, widget);

        row++;
    }

    m_dialog->iface_nz_list->resizeColumnToContents(3);
}

void IC_NetworkZonesPage::setNetworkZones()
{
    Firewall *fw = 
        dynamic_cast<ImportFirewallConfigurationWizard*>(wizard())->getFirewall();

    if (fw == nullptr) return;

    // read and configure network zones
    list<FWObject*> all_interfaces = fw->getByTypeDeep(Interface::TYPENAME);
    list<FWObject*>::iterator it;
    for (it=all_interfaces.begin(); it!=all_interfaces.end(); ++it)
    {
        Interface *iface = Interface::cast(*it);

        string  network_zone_str_id = "";

        QList<QTableWidgetItem*> ltwi =
            m_dialog->iface_nz_list->findItems( iface->getName().c_str(),
                                                Qt::MatchExactly );
        if ( ! ltwi.empty())
        {
            QTableWidgetItem *itm2 = ltwi[0];
            assert(itm2!=nullptr);
            int row = itm2->row();
            QComboBox *cb = dynamic_cast<QComboBox*>(
                m_dialog->iface_nz_list->cellWidget(row, 3));
            assert(cb!=nullptr);
            int network_zone_int_id =
                cb->itemData(cb->currentIndex(), Qt::UserRole).toInt();
            if (network_zone_int_id != 0)
                network_zone_str_id = FWObjectDatabase::getStringId(
                    network_zone_int_id);
            else
                network_zone_str_id = "";
        }

        // only set network zone if it is supported and is not
        // empty. See #2014
        if (!network_zone_str_id.empty())
            iface->setStr("network_zone", network_zone_str_id);

    }
}
