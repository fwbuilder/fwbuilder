/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: linux24AdvancedDialog.cpp,v 1.4 2006/03/16 05:38:14 vkurland Exp $

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

#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"
#include "platforms.h"

#include "linux24AdvancedDialog.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Management.h"

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qstackedwidget.h>
#include <qregexp.h>

#include "FWWindow.h"

using namespace std;
using namespace libfwbuilder;

linux24AdvancedDialog::~linux24AdvancedDialog()
{
    delete m_dialog;
}

linux24AdvancedDialog::linux24AdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::linux24AdvancedDialog_q;
    m_dialog->setupUi(this);
    obj=o;

    FWOptions *fwopt=(Firewall::cast(obj))->getOptionsObject();
    assert(fwopt!=NULL);

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=NULL);

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
                         "linux24_icmp_echo_ignore_broadcasts", threeStateMapping);
    data.registerOption( m_dialog->linux24_icmp_ignore_bogus_error_responses,
                         fwopt,
                         "linux24_icmp_ignore_bogus_error_responses", threeStateMapping);
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
                         "linux24_path_ip"       );
    data.registerOption( m_dialog->linux24_path_lsmod,
                         fwopt,
                         "linux24_path_lsmod"    );
    data.registerOption( m_dialog->linux24_path_logger,
                         fwopt,
                         "linux24_path_logger"   );
    data.registerOption( m_dialog->linux24_path_modprobe,
                         fwopt,
                         "linux24_path_modprobe" );
    data.registerOption( m_dialog->linux24_path_iptables_restore,
                         fwopt,
                         "linux24_path_iptables_restore" );
    data.registerOption( m_dialog->linux24_path_ip6tables_restore,
                         fwopt,
                         "linux24_path_ip6tables_restore" );


    data.loadAll();
}

/*
 * store all data in the object
 */
void linux24AdvancedDialog::accept()
{
    FWOptions *fwopt=(Firewall::cast(obj))->getOptionsObject();
    assert(fwopt!=NULL);

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=NULL);

    data.saveAll();

    mw->updateLastModifiedTimestampForAllFirewalls(obj);
    QDialog::accept();
}

void linux24AdvancedDialog::reject()
{
    QDialog::reject();
}


