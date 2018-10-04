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

#include "linksysAdvancedDialog.h"
#include "FWWindow.h"
#include "FWCmdChange.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"

#include <memory>

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qtabwidget.h>

using namespace std;
using namespace libfwbuilder;

linksysAdvancedDialog::~linksysAdvancedDialog()
{
    delete m_dialog;
}

linksysAdvancedDialog::linksysAdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::linksysAdvancedDialog_q;
    m_dialog->setupUi(this);

    obj=o;

    FWOptions *fwopt=(Firewall::cast(obj))->getOptionsObject();
    assert(fwopt!=nullptr);

#ifndef NDEBUG
    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=nullptr);
#endif

/*
 * since v2.0.3 we do not need to know shell prompt on linksys. Will
 * remove the page completely when code becomes stable.
 */
    m_dialog->tabWidget->removeTab( 1 );

    QStringList threeStateMapping;

    threeStateMapping.push_back(QObject::tr("No change"));
    threeStateMapping.push_back("");

    threeStateMapping.push_back(QObject::tr("On"));
    threeStateMapping.push_back("1");

    threeStateMapping.push_back(QObject::tr("Off"));
    threeStateMapping.push_back("0");

/* set default prompts */
    if (fwopt->getStr("prompt1").empty())
        Resources::os_res["sveasoft"]->Resources::setDefaultOption(fwopt,
                           "/FWBuilderResources/Target/options/default/prompt1");

    if (fwopt->getStr("prompt2").empty())
        Resources::os_res["sveasoft"]->Resources::setDefaultOption(fwopt,
                           "/FWBuilderResources/Target/options/default/prompt2");

    data.registerOption( m_dialog->linksys_prompt1,
                         fwopt,
                         "prompt1" );
    data.registerOption( m_dialog->linksys_prompt2,
                         fwopt,
                         "prompt2" );

    data.registerOption( m_dialog->linksys_path_iptables,
                         fwopt,
                         "linux24_path_iptables" );
    data.registerOption( m_dialog->linksys_path_ip,
                         fwopt,
                         "linux24_path_ip"       );
    data.registerOption( m_dialog->linksys_path_lsmod,
                         fwopt,
                         "linux24_path_lsmod"    );
    data.registerOption( m_dialog->linksys_path_logger,
                         fwopt,
                         "linux24_path_logger"   );
    data.registerOption( m_dialog->linksys_path_modprobe,
                         fwopt,
                         "linux24_path_modprobe" );
    data.registerOption( m_dialog->linksys_path_vconfig,
                         fwopt,
                         "linksys_path_vconfig");
    data.registerOption( m_dialog->linksys_path_brctl,
                         fwopt,
                         "linksys_path_brctl");
    data.registerOption( m_dialog->linksys_path_ifenslave,
                         fwopt,
                         "linksys_path_ifenslave");

    data.loadAll();

    m_dialog->tabWidget->setCurrentIndex(0);
}

/*
 * store all data in the object
 */
void linksysAdvancedDialog::accept()
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

void linksysAdvancedDialog::reject()
{
    QDialog::reject();
}

void linksysAdvancedDialog::setDefaultPrompts()
{
#ifndef NDEBUG
    FWOptions *fwopt=(Firewall::cast(obj))->getOptionsObject();
    assert(fwopt!=nullptr);
#endif
    m_dialog->linksys_prompt1->setText(
        Resources::getTargetOptionStr("sveasoft","default/prompt1").c_str() );
    m_dialog->linksys_prompt2->setText(
        Resources::getTargetOptionStr("sveasoft","default/prompt2").c_str() );
}

