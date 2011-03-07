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

#include "SNMPNetworkDiscoveryWizard.h"

#include "ND_ChooseNetworksPage.h"
#include "ND_ChooseObjectsPage.h"
#include "ND_ChooseObjectTypePage.h"
#include "ND_CreateObjectsPage.h"
#include "ND_DiscoveryParametersPage.h"
#include "ND_ProgressPage.h"
#include "ND_SelectLibraryPage.h"
#include "ND_SetupPage.h"
#include "ND_SNMPParametersPage.h"
#include "FWWindow.h"

#include <QDesktopWidget>
#include <QtDebug>

using namespace std;
//using namespace libfwbuilder;


SNMPNetworkDiscoveryWizard::SNMPNetworkDiscoveryWizard(QWidget *parent) : QWizard(parent)
{
    QPixmap pm;
    pm.load(":/Images/fwbuilder3-72x72.png");
    setPixmap(QWizard::LogoPixmap, pm);

    setWindowTitle(tr("Discover addresses and subnets using SNMP"));

    addPage(new ND_SetupPage(this));
    addPage(new ND_DiscoveryParametersPage(this));
    addPage(new ND_SNMPParametersPage(this));
    addPage(new ND_ProgressPage(this));
    addPage(new ND_ChooseNetworksPage(this));
    addPage(new ND_ChooseObjectsPage(this));
    addPage(new ND_ChooseObjectTypePage(this));
    addPage(new ND_SelectLibraryPage(this));
    addPage(new ND_CreateObjectsPage(this));

    QRect sg = QApplication::desktop()->screenGeometry(mw);
    QSize screen_size = sg.size();

#if defined(Q_OS_MACX)
    QSize desired_size(900, 700);
#else
    QSize desired_size(800, 700);
#endif

    if (desired_size.width() > screen_size.width())
        desired_size.setWidth(screen_size.width());
    if (desired_size.height() > screen_size.height())
        desired_size.setHeight(screen_size.height());

    resize(desired_size);
}

