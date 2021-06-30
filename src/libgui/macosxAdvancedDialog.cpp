/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id$

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
#include "platforms.h"

#include "macosxAdvancedDialog.h"
#include "FWWindow.h"
#include "FWCmdChange.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Management.h"

#include <memory>

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qregexp.h>


using namespace std;
using namespace libfwbuilder;

macosxAdvancedDialog::~macosxAdvancedDialog()
{
    delete m_dialog;
}

macosxAdvancedDialog::macosxAdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::macosxAdvancedDialog_q;
    m_dialog->setupUi(this);

    obj=o;

    FWOptions *fwopt=(Firewall::cast(obj))->getOptionsObject();
    assert(fwopt!=nullptr);

#ifndef NDEBUG
    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=nullptr);
#endif

    QStringList threeStateMapping;

    threeStateMapping.push_back(QObject::tr("No change"));
    threeStateMapping.push_back("");

    threeStateMapping.push_back(QObject::tr("On"));
    threeStateMapping.push_back("1");

    threeStateMapping.push_back(QObject::tr("Off"));
    threeStateMapping.push_back("0");

    data.registerOption( m_dialog->macosx_ip_forward,
                          fwopt,
                          "macosx_ip_forward", threeStateMapping);
    data.registerOption( m_dialog->macosx_ip_redirect,
                          fwopt,
                          "macosx_ip_redirect", threeStateMapping);
    data.registerOption( m_dialog->macosx_ip_sourceroute,
                          fwopt,
                          "macosx_ip_sourceroute", threeStateMapping);
    data.registerOption( m_dialog->macosx_path_ipfw,
                          fwopt,
                          "macosx_path_ipfw");
    data.registerOption( m_dialog->macosx_path_sysctl,
                          fwopt,
                          "macosx_path_sysctl");
    data.loadAll();

    m_dialog->tabWidget->setCurrentIndex(0);
}

/*
 * store all data in the object
 */
void macosxAdvancedDialog::accept()
{
    ProjectPanel *project = mw->activeProject();
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(project, obj));

    // new_state  is a copy of the fw object
    FWObject* new_state = cmd->getNewState();
    FWOptions* fwoptions = Firewall::cast(new_state)->getOptionsObject();
    assert(fwoptions!=nullptr);

    data.saveAll(fwoptions);

    if (!cmd->getOldState()->cmp(new_state, true))
        project->undoStack->push(cmd.release());
    
    QDialog::accept();
}

void macosxAdvancedDialog::reject()
{
    QDialog::reject();
}


