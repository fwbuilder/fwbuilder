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

#include "config.h"
#include "global.h"
#include "platforms.h"

#include "freebsdAdvancedDialog.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Management.h"

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qregexp.h>

#include "FWWindow.h"

using namespace std;
using namespace libfwbuilder;

freebsdAdvancedDialog::~freebsdAdvancedDialog()
{
    delete m_dialog;
}

freebsdAdvancedDialog::freebsdAdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::freebsdAdvancedDialog_q;
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


    data.registerOption(  m_dialog->freebsd_ip_sourceroute,
                          fwopt, "freebsd_ip_sourceroute",  threeStateMapping);
    data.registerOption(  m_dialog->freebsd_ip_redirect,
                          fwopt, "freebsd_ip_redirect",  threeStateMapping);
    data.registerOption(  m_dialog->freebsd_ip_forward,
                          fwopt, "freebsd_ip_forward",  threeStateMapping);

    data.registerOption(  m_dialog->freebsd_path_ipnat,
                          fwopt, "freebsd_path_ipnat");
    data.registerOption(  m_dialog->freebsd_path_sysctl,
                          fwopt,"freebsd_path_sysctl");
    data.registerOption(  m_dialog->freebsd_path_ipf,
                          fwopt, "freebsd_path_ipf");
    data.registerOption(  m_dialog->freebsd_path_ipfw,
                          fwopt, "freebsd_path_ipfw");


    data.loadAll();
}

/*
 * store all data in the object
 */
void freebsdAdvancedDialog::accept()
{
    FWOptions *fwopt=(Firewall::cast(obj))->getOptionsObject();
    assert(fwopt!=NULL);

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=NULL);

    data.saveAll();
    mw->updateLastModifiedTimestampForAllFirewalls(obj);

    QDialog::accept();
}

void freebsdAdvancedDialog::reject()
{
    QDialog::reject();
}


