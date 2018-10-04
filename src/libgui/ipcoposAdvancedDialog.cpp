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

#include "ipcoposAdvancedDialog.h"
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
#include <qstackedwidget.h>
#include <qregexp.h>

#include "FWWindow.h"
#include "Help.h"

using namespace std;
using namespace libfwbuilder;

ipcoposAdvancedDialog::~ipcoposAdvancedDialog()
{
    delete m_dialog;
}

ipcoposAdvancedDialog::ipcoposAdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::ipcoposAdvancedDialog_q;
    m_dialog->setupUi(this);
    obj=o;

    string host_os = obj->getStr("host_OS");
    string description = Resources::os_res[host_os]->
        getResourceStr("/FWBuilderResources/Target/description");
    setWindowTitle(QObject::tr("%1 advanced settings").arg(description.c_str()));

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

    data.registerOption( m_dialog->linux24_log_martians,
                         fwopt,
                         "linux24_log_martians", threeStateMapping);
    data.registerOption( m_dialog->linux24_accept_redirects,
                         fwopt,
                         "linux24_accept_redirects", threeStateMapping);
    data.registerOption( m_dialog->linux24_icmp_echo_ignore_all,
                         fwopt,
                         "linux24_icmp_echo_ignore_all", threeStateMapping);
    data.registerOption( m_dialog->linux24_icmp_echo_ignore_broadcasts,
                         fwopt,
                         "linux24_icmp_echo_ignore_broadcasts",
                         threeStateMapping);
    data.registerOption( m_dialog->linux24_icmp_ignore_bogus_error_responses,
                         fwopt,
                         "linux24_icmp_ignore_bogus_error_responses",
                         threeStateMapping);
    data.registerOption( m_dialog->linux24_ip_dynaddr,
                         fwopt,
                         "linux24_ip_dynaddr", threeStateMapping);
    data.registerOption( m_dialog->linux24_rp_filter,
                         fwopt,
                         "linux24_rp_filter", threeStateMapping);
    data.registerOption( m_dialog->linux24_accept_source_route,
                         fwopt,
                         "linux24_accept_source_route", threeStateMapping);
    data.registerOption( m_dialog->linux24_ip_forward,
                         fwopt,
                         "linux24_ip_forward", threeStateMapping);
    data.registerOption( m_dialog->linux24_ipv6_forward,
                         fwopt,
                         "linux24_ipv6_forward", threeStateMapping);
    data.registerOption( m_dialog->linux24_tcp_fin_timeout,
                         fwopt,
                         "linux24_tcp_fin_timeout" );
    data.registerOption( m_dialog->linux24_tcp_keepalive_interval,
                         fwopt,
                         "linux24_tcp_keepalive_interval");
    data.registerOption( m_dialog->linux24_tcp_window_scaling,
                         fwopt,
                         "linux24_tcp_window_scaling", threeStateMapping);
    data.registerOption( m_dialog->linux24_tcp_sack,
                         fwopt,
                         "linux24_tcp_sack", threeStateMapping);
    data.registerOption( m_dialog->linux24_tcp_fack,
                         fwopt,
                         "linux24_tcp_fack", threeStateMapping);
    data.registerOption( m_dialog->linux24_tcp_ecn,
                         fwopt,
                         "linux24_tcp_ecn", threeStateMapping);
    data.registerOption( m_dialog->linux24_tcp_syncookies,
                         fwopt,
                         "linux24_tcp_syncookies", threeStateMapping);
    data.registerOption( m_dialog->linux24_tcp_timestamps,
                         fwopt,
                         "linux24_tcp_timestamps", threeStateMapping);
    data.registerOption( m_dialog->linux24_path_iptables,
                         fwopt,
                         "linux24_path_iptables" );
    data.registerOption( m_dialog->linux24_path_ip6tables,
                         fwopt,
                         "linux24_path_ip6tables" );
    data.registerOption( m_dialog->linux24_path_ip,
                         fwopt,
                         "linux24_path_ip");
    data.registerOption( m_dialog->linux24_path_lsmod,
                         fwopt,
                         "linux24_path_lsmod");
    data.registerOption( m_dialog->linux24_path_logger,
                         fwopt,
                         "linux24_path_logger");
    data.registerOption( m_dialog->linux24_path_vconfig,
                         fwopt,
                         "linux24_path_vconfig");
    data.registerOption( m_dialog->linux24_path_brctl,
                         fwopt,
                         "linux24_path_brctl");
    data.registerOption( m_dialog->linux24_path_ifenslave,
                         fwopt,
                         "linux24_path_ifenslave");
    data.registerOption( m_dialog->linux24_path_modprobe,
                         fwopt,
                         "linux24_path_modprobe");
    data.registerOption( m_dialog->linux24_path_iptables_restore,
                         fwopt,
                         "linux24_path_iptables_restore");
    data.registerOption( m_dialog->linux24_path_ip6tables_restore,
                         fwopt,
                         "linux24_path_ip6tables_restore");


    data.loadAll();

    m_dialog->tabWidget->setCurrentIndex(0);
}

/*
 * store all data in the object
 */
void ipcoposAdvancedDialog::accept()
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

void ipcoposAdvancedDialog::reject()
{
    QDialog::reject();
}

void ipcoposAdvancedDialog::help()
{
    QString tab_title = m_dialog->tabWidget->tabText(
        m_dialog->tabWidget->currentIndex());
    QString anchor = tab_title.replace('/', '-').replace(' ', '-').toLower();
    Help *h = Help::getHelpWindow(this);
    h->setName("Host type IPCOP");
    h->setSource(QUrl("ipcoposAdvancedDialog.html#" + anchor));
    h->raise();
    h->show();
}



