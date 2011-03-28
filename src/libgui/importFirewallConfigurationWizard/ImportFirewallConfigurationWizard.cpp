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

#include "ImportFirewallConfigurationWizard.h"

#include "IC_FileNamePage.h"
#include "IC_FirewallNamePage.h"
#include "IC_PlatformWarningPage.h"
#include "IC_ProgressPage.h"
#include "IC_NetworkZonesPage.h"

#include "FWWindow.h"
#include "ProjectPanel.h"
#include "ObjConflictResolutionDialog.h"

#include "fwbuilder/FWObject.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/TCPService.h"
#include "fwbuilder/ServiceGroup.h"

#include <QDesktopWidget>
#include <QtDebug>

using namespace std;
using namespace libfwbuilder;


ImportFirewallConfigurationWizard::ImportFirewallConfigurationWizard(
    QWidget *parent, FWObjectDatabase *_db) : QWizard(parent)
{
    fw = NULL;
    db_orig = _db;
    db_copy = new FWObjectDatabase();
    db_copy->duplicate(_db, false);
    current_lib = Library::cast(db_copy->findInIndex(mw->getCurrentLib()->getId()));

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

    // always show cancel button
    setOption(QWizard::NoCancelButton, false);

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

ImportFirewallConfigurationWizard::~ImportFirewallConfigurationWizard()
{
    delete db_copy;
}

void ImportFirewallConfigurationWizard::accept()
{
    if (fwbdebug) qDebug() << "ImportFirewallConfigurationWizard::accept()"
                           << "fw=" << fw;

    if (fw != NULL && (platform == "pix" || platform == "fwsm"))
        dynamic_cast<IC_NetworkZonesPage*>(
            page(Page_NetworkZones))->setNetworkZones();

    if (fwbdebug) qDebug() << "ImportFirewallConfigurationWizard::accept()"
                           << "merging object trees";

    // merge dbcopy into db

    CompareObjectsDialog cod(this);
    db_orig->merge(db_copy, &cod);

    if (fwbdebug) qDebug() << "ImportFirewallConfigurationWizard::accept()"
                           << "merge done";

    ProjectPanel *pp = mw->activeProject();
    QString filename = pp->getFileName();

    QCoreApplication::postEvent(
        mw, new reloadObjectTreeImmediatelyEvent(filename));

    if (fw)
    {
        QCoreApplication::postEvent(
            pp, new showObjectInTreeEvent(filename, fw->getId()));

        QCoreApplication::postEvent(
            pp, new expandObjectInTreeEvent(
                mw->activeProject()->getFileName(), fw->getId()));

        QCoreApplication::postEvent(
            mw, new openObjectInEditorEvent(filename, fw->getId()));

        // Open first created Policy ruleset object
        FWObject *first_policy = fw->getFirstByType(Policy::TYPENAME);
        if (first_policy)
            QCoreApplication::postEvent(
                pp, new openRulesetEvent(filename, first_policy->getId()));
    }

    QWizard::accept();
}
