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

#include "ImportFirewallConfigurationWizard.h"

#include "IC_FileNamePage.h"
#include "IC_FirewallNamePage.h"
#include "IC_PlatformWarningPage.h"
#include "IC_ProgressPage.h"
#include "IC_NetworkZonesPage.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Firewall.h"

#include <QtDebug>

using namespace std;
using namespace libfwbuilder;


ImportFirewallConfigurationWizard::ImportFirewallConfigurationWizard(QWidget *parent) :
    QWizard(parent)
{
    fw = NULL;

    QPixmap pm;
    pm.load(":/Images/fwbuilder3-72x72.png");
    setPixmap(QWizard::LogoPixmap, pm);

    pm.load(":/Images/fwbuilder3-256x256-fade.png");
    setPixmap(QWizard::BackgroundPixmap, pm);

    setWindowTitle(tr("Import Firewall Configuration"));

    setPage(Page_FileName, new IC_FileNamePage(this));
    setPage(Page_Platform, new IC_PlatformWarningPage(this));
    setPage(Page_FirewallName, new IC_FirewallNamePage(this));
    setPage(Page_Progess, new IC_ProgressPage(this));
    setPage(Page_NetworkZones, new IC_NetworkZonesPage(this));

    resize(700, 600);
}

void ImportFirewallConfigurationWizard::accept()
{
    qDebug() << "ImportFirewallConfigurationWizard::accept()";

    if (platform == "pix" || platform == "fwsm")
        dynamic_cast<IC_NetworkZonesPage*>(
            page(Page_NetworkZones))->setNetworkZones();

    QWizard::accept();
}
