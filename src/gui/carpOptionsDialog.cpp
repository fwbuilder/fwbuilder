/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

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

#include "carpOptionsDialog.h"
#include "FWWindow.h"

#include "fwbuilder/Interface.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Firewall.h"

#include <qmessagebox.h>

using namespace std;
using namespace libfwbuilder;


carpOptionsDialog::carpOptionsDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::carpOptionsDialog_q;
    m_dialog->setupUi(this);
    obj = o;

    FWOptions *gropt = FWOptions::cast(obj);
    assert(gropt != NULL);
    
    data.registerOption(m_dialog->carp_password,
                        gropt,
                        "carp_password");

    data.registerOption(m_dialog->carp_vhid,
                        gropt,
                        "carp_vhid");

    data.registerOption(m_dialog->advbase,
                        gropt,
                        "carp_advbase");

    data.registerOption(m_dialog->master_advskew,
                        gropt,
                        "carp_master_advskew");

    data.registerOption(m_dialog->default_advskew,
                        gropt,
                        "carp_default_advskew");

    data.loadAll();
}

carpOptionsDialog::~carpOptionsDialog()
{
    delete m_dialog;
}

/*
 * store all data in the object
 */
void carpOptionsDialog::accept()
{
    if (!validate()) return;
    data.saveAll();
//    mw->updateLastModifiedTimestampForAllFirewalls(obj);
    QDialog::accept();
}

void carpOptionsDialog::reject()
{
    QDialog::reject();
}

bool carpOptionsDialog::validate()
{
    bool valid = true;
    QWidget *focus = NULL;
    QString message;

    // carp secret must be set
    if (m_dialog->carp_password->text().isEmpty())
    {
        message = "CARP Password field can not be empty!";
        focus = m_dialog->carp_password;
        valid = false;
    }

    // if vrid is set, it must be an integer 1 <= vrid <= 255
    // Now QSpinBox widget enforses boundaries

    if (!valid)
    {
        QMessageBox::warning(this, "Firewall Builder",
                             tr("Input not valid: %1").arg(message), "&Continue",
                             QString::null, QString::null, 0, 1);
        focus->setFocus();
    }
    return valid;
}

