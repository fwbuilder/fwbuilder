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

#include "global.h"

#include "pixFailoverOptionsDialog.h"
#include "FWWindow.h"
#include "FWCmdChange.h"

#include "fwbuilder/Interface.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Firewall.h"

#include <memory>

#include <qmessagebox.h>

using namespace std;
using namespace libfwbuilder;


pixFailoverOptionsDialog::pixFailoverOptionsDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::pixFailoverOptionsDialog_q;
    m_dialog->setupUi(this);
    obj = o;

    FWOptions *gropt = FWOptions::cast(obj);
    assert(gropt != nullptr);
    
    data.registerOption(m_dialog->pix_failover_key,
                        gropt,
                        "pix_failover_key");
    data.loadAll();
}

pixFailoverOptionsDialog::~pixFailoverOptionsDialog()
{
    delete m_dialog;
}

/*
 * store all data in the object
 */
void pixFailoverOptionsDialog::accept()
{
    if (!validate()) return;
    // the parent of this dialog is InterfaceDialog, not ProjectPanel
    ProjectPanel *project = mw->activeProject();
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChangeOptionsObject(project, obj));
    FWObject* new_state = cmd->getNewState();

    data.saveAll(new_state);

    if (!cmd->getOldState()->cmp(new_state, true))
        project->undoStack->push(cmd.release());
    
    QDialog::accept();
}

void pixFailoverOptionsDialog::reject()
{
    QDialog::reject();
}

bool pixFailoverOptionsDialog::validate()
{
    bool valid = true;
    QWidget *focus = nullptr;
    QString message;

    // key must be set
    if (m_dialog->pix_failover_key->text().isEmpty())
    {
        message = "PIX failover key field can not be empty!";
        focus = m_dialog->pix_failover_key;
        valid = false;
    }

    if (!valid)
    {
        QMessageBox::warning(this, "Firewall Builder",
                             tr("Input not valid: %1").arg(message), "&Continue",
                             QString::null, QString::null, 0, 1);
        focus->setFocus();
    }
    return valid;
}

