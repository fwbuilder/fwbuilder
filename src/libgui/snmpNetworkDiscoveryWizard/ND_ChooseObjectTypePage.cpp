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

#include "ND_ChooseObjectTypePage.h"
#include "SNMPNetworkDiscoveryWizard.h"

#include <QtDebug>


using namespace std;
using namespace libfwbuilder;


ND_ChooseObjectTypePage::ND_ChooseObjectTypePage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::ND_ChooseObjectTypePage_q;
    m_dialog->setupUi(this);

}

void ND_ChooseObjectTypePage::initializePage()
{
    if (fwbdebug)
        qDebug() << "ND_ChooseObjectTypePage::initializePage()";

    objects = dynamic_cast<SNMPNetworkDiscoveryWizard*>(wizard())->getObjects();
    objectsToUse = dynamic_cast<SNMPNetworkDiscoveryWizard*>(wizard())->getObjectsToUse();

    fillTypeChangingList();
}

void ND_ChooseObjectTypePage::fillTypeChangingList()
{
    m_dialog->typeChangingList->clear();

    qDebug() << objectsToUse;

    int idx = 0;
    foreach(ObjectDescriptor od, *objects)
    {
        if (objectsToUse->contains(QString::fromUtf8(od.sysname.c_str())))
        {
            QString ins;
            ins = (od.interfaces.size()) ?
                QString("%1").arg(od.interfaces.size()) : "";
            QStringList sl;
            sl << QString::fromUtf8(od.toString().c_str())
               << ins << od.type.c_str();
            QTreeWidgetItem *itm = new QTreeWidgetItem(
                m_dialog->typeChangingList, sl );
            itm->setData(0, Qt::UserRole, idx);
        }
        idx++;
    }

    m_dialog->typeChangingList->resizeColumnToContents(0);
    m_dialog->typeChangingList->resizeColumnToContents(1);
}

void ND_ChooseObjectTypePage::typeAddress()
{
    changeTargetObject(IPv4::TYPENAME);
}

void ND_ChooseObjectTypePage::typeHost()
{
    changeTargetObject(Host::TYPENAME);
}

void ND_ChooseObjectTypePage::typeFirewall()
{
    changeTargetObject(Firewall::TYPENAME);
}

void ND_ChooseObjectTypePage::changeTargetObject(const QString &buf)
{
    QTreeWidgetItem* item = m_dialog->typeChangingList->topLevelItem(0);

    while (item!=nullptr)
    {
        if (item->isSelected())
        {
            int idx = item->data(0, Qt::UserRole).toInt();
            (*objects)[idx].type = buf.toStdString();
            item->setText(2, buf);
        }
        item = m_dialog->typeChangingList->topLevelItem(
            m_dialog->typeChangingList->indexOfTopLevelItem(item)+1);
    }
}

