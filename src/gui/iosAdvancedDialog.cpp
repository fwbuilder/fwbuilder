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
#include "utils.h"

#include "iosAdvancedDialog.h"
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

iosAdvancedDialog::~iosAdvancedDialog()
{
    delete m_dialog;
}

iosAdvancedDialog::iosAdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::iosAdvancedDialog_q;
    m_dialog->setupUi(this);
    obj=o;

    FWOptions *fwoptions=(Firewall::cast(obj))->getOptionsObject();
    assert(fwoptions!=NULL);

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=NULL);

/* Page "General" */
    data.registerOption( m_dialog->ios_set_host_name  , fwoptions,  "ios_set_host_name" );
    data.registerOption( m_dialog->ios_ip_address  ,    fwoptions,  "ios_ip_address"    );

    data.loadAll();

    m_dialog->tabWidget->setCurrentIndex(0);
}

/*
 * store all data in the object
 */
void iosAdvancedDialog::accept()
{
    data.saveAll();
//    mw->updateLastModifiedTimestampForAllFirewalls(obj);
    QDialog::accept();
}

void iosAdvancedDialog::reject()
{
    QDialog::reject();
}


