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

#include "../../config.h"
#include "global.h"
#include "platforms.h"

#include "solarisAdvancedDialog.h"
#include "FWWindow.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Management.h"

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qstackedwidget.h>
#include <qregexp.h>


using namespace std;
using namespace libfwbuilder;

solarisAdvancedDialog::~solarisAdvancedDialog()
{
    delete m_dialog;
}

solarisAdvancedDialog::solarisAdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::solarisAdvancedDialog_q;
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

    data.registerOption(  m_dialog->solaris_ip_forward,
                          fwopt,
                          "solaris_ip_forward", threeStateMapping);
    data.registerOption(  m_dialog->solaris_ip_forward_src_routed,
                          fwopt,
                          "solaris_ip_forward_src_routed", threeStateMapping);
    data.registerOption(  m_dialog->solaris_ip_forward_directed_broadcasts,
                          fwopt,
                          "solaris_ip_forward_directed_broadcasts", threeStateMapping);
    data.registerOption(  m_dialog->solaris_ip_respond_to_echo_broadcast,
                          fwopt,
                          "solaris_ip_respond_to_echo_broadcast", threeStateMapping);
    data.registerOption(  m_dialog->solaris_ip_forward_directed_broadcasts,
                          fwopt,
                          "solaris_ip_forward_directed_broadcasts", threeStateMapping);
    data.registerOption(  m_dialog->solaris_ip_ignore_redirect,
                          fwopt,
                          "solaris_ip_ignore_redirect", threeStateMapping);
    data.registerOption(  m_dialog->solaris_ip_forward_src_routed,
                          fwopt,
                          "solaris_ip_forward_src_routed", threeStateMapping);
    data.registerOption(  m_dialog->solaris_path_ipf  ,
                          fwopt,
                          "solaris_path_ipf");
    data.registerOption(  m_dialog->solaris_path_ipnat,
                          fwopt,
                          "solaris_path_ipnat");


    data.loadAll();
}

/*
 * store all data in the object
 */
void solarisAdvancedDialog::accept()
{
    FWOptions *fwopt=(Firewall::cast(obj))->getOptionsObject();
    assert(fwopt!=NULL);

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=NULL);

    data.saveAll();

    mw->updateLastModifiedTimestampForAllFirewalls(obj);
    QDialog::accept();
}

void solarisAdvancedDialog::reject()
{
    QDialog::reject();
}


