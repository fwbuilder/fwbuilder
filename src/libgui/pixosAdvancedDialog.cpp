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
#include "utils.h"

#include "pixosAdvancedDialog.h"
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
#include <qstackedwidget.h>
#include <qregexp.h>


using namespace std;
using namespace libfwbuilder;

pixosAdvancedDialog::~pixosAdvancedDialog()
{
    delete m_dialog;
}

pixosAdvancedDialog::pixosAdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::pixosAdvancedDialog_q;
    m_dialog->setupUi(this);

    obj=o;

    FWOptions *fwoptions=(Firewall::cast(obj))->getOptionsObject();
    assert(fwoptions!=nullptr);

#ifndef NDEBUG
    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=nullptr);
#endif

/* Page "General" */
    data.registerOption(
        m_dialog->pix_set_host_name, fwoptions, "pix_set_host_name");
    data.registerOption(
        m_dialog->pix_ip_address, fwoptions, "pix_ip_address");

/* Page NTP */

    string host_os = o->getStr("host_OS");

    if (host_os == "pix_os")
    {
        m_dialog->fwsm_ntp_warning->hide();
        m_dialog->ntp_servers_group->show();
        data.registerOption( m_dialog->ntp1,       fwoptions,  "pix_ntp1"        );
        data.registerOption( m_dialog->ntp1_pref,  fwoptions,  "pix_ntp1_pref"   );
        data.registerOption( m_dialog->ntp2,       fwoptions,  "pix_ntp2"        );
        data.registerOption( m_dialog->ntp2_pref,  fwoptions,  "pix_ntp2_pref"   );
        data.registerOption( m_dialog->ntp3,       fwoptions,  "pix_ntp3"        );
        data.registerOption( m_dialog->ntp3_pref,  fwoptions,  "pix_ntp3_pref"   );
    }

    if (host_os == "fwsm_os")
    {
        m_dialog->fwsm_ntp_warning->show();
        m_dialog->ntp_servers_group->hide();
    }

/* Page SNMP */

    data.registerOption( m_dialog->disable_snmp_agent, fwoptions,  "pix_disable_snmp_agent");

    data.registerOption( m_dialog->set_communities,    fwoptions,  "pix_set_communities_from_object_data" );
    data.registerOption( m_dialog->enable_traps,       fwoptions,  "pix_enable_snmp_traps"                );

    data.registerOption( m_dialog->snmp_server1,       fwoptions,  "pix_snmp_server1"                     );
    data.registerOption( m_dialog->snmp_server2,       fwoptions,  "pix_snmp_server2"                     );

    data.registerOption( m_dialog->snmp_poll_traps_1,  fwoptions,  "pix_snmp_poll_traps_1"                );
    data.registerOption( m_dialog->snmp_poll_traps_2,  fwoptions,  "pix_snmp_poll_traps_2"                );

/* Page Options */

    data.registerOption( m_dialog->tcpmss,             fwoptions,  "pix_tcpmss");
    data.registerOption( m_dialog->tcpmss_value,       fwoptions,  "pix_tcpmss_value");

    data.loadAll();

    m_dialog->tabWidget->setCurrentIndex(0);
}

/*
 * store all data in the object
 */
void pixosAdvancedDialog::accept()
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

void pixosAdvancedDialog::reject()
{
    QDialog::reject();
}


