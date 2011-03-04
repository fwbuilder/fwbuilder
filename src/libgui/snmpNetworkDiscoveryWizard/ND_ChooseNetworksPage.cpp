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

#include "ND_ChooseNetworksPage.h"
#include "SNMPNetworkDiscoveryWizard.h"

#include <QtDebug>


using namespace std;
using namespace libfwbuilder;


ND_ChooseNetworksPage::ND_ChooseNetworksPage(QWidget *parent) : QWizardPage(parent)
{
    m_dialog = new Ui::ND_ChooseNetworksPage_q;
    m_dialog->setupUi(this);

    registerField("networksToUse*", m_dialog->objectSelector,
                  "objectsToUse", SIGNAL(selectionChanged()));
}

void ND_ChooseNetworksPage::initializePage()
{
    if (fwbdebug)
        qDebug() << "ND_ChooseNetworksPage::initializePage()";

    ObjectDescriptorList *networks = 
        dynamic_cast<SNMPNetworkDiscoveryWizard*>(wizard())->getNetworks();

    m_dialog->objectSelector->init(*networks);

    /*
      list<ObjectDescriptor> objects;

      fill objects with data and call

      m_dialog->objectSelector->init(objects);

     */
}

bool ND_ChooseNetworksPage::validatePage()
{
    if (fwbdebug) qDebug() << "ND_ChooseNetworksPage::validatePage()";
    return true;
}

bool ND_ChooseNetworksPage::isComplete() const
{
    if (fwbdebug) qDebug() << "ND_ChooseNetworksPage::isComplete()";
    return (m_dialog->objectSelector->count() > 0);
}

