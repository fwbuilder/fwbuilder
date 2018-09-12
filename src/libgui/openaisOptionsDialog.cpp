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

#include "openaisOptionsDialog.h"
#include "FWWindow.h"
#include "FWCmdChange.h"

#include "fwbuilder/Interface.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Resources.h"

#include <memory>

#include <qmessagebox.h>
#include <QUndoStack>


using namespace std;
using namespace libfwbuilder;


openaisOptionsDialog::openaisOptionsDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::openaisOptionsDialog_q;
    m_dialog->setupUi(this);
    obj = o;

    FWOptions *gropt = FWOptions::cast(obj);
    assert(gropt != nullptr);
    FWObject *p = obj;
    while (p && Cluster::cast(p)==nullptr) p = p->getParent();
    assert(p != nullptr);
    Cluster *cluster = Cluster::cast(p);
    Resources *os_res = Resources::os_res[cluster->getStr("host_OS")];
    assert(os_res != nullptr);

    string default_address =
        os_res->getResourceStr(
            "/FWBuilderResources/Target/protocols/openais/default_address");
    string default_port =
        os_res->getResourceStr(
            "/FWBuilderResources/Target/protocols/openais/default_port");

    string addr = gropt->getStr("openais_address");
    if (addr.empty()) gropt->setStr("openais_address", default_address);

    string port = gropt->getStr("openais_port");
    if (port.empty()) gropt->setStr("openais_port", default_port);

    data.registerOption(m_dialog->openais_address,
                        gropt,
                        "openais_address");
    data.registerOption(m_dialog->openais_port,
                        gropt,
                        "openais_port");
    data.loadAll();
}

openaisOptionsDialog::~openaisOptionsDialog()
{
    delete m_dialog;
}

void openaisOptionsDialog::accept()
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

void openaisOptionsDialog::reject()
{
    QDialog::reject();
}

bool openaisOptionsDialog::validate()
{
    try
    {
        InetAddr(m_dialog->openais_address->text().toLatin1().constData());
    } catch (FWException &ex)
    {
        try
        {
            InetAddr(AF_INET6, m_dialog->openais_address->text().toLatin1().constData() );
        } catch (FWException &ex)
        {
            QMessageBox::critical(
                this, "Firewall Builder",
                tr("Invalid IP address '%1'").arg(m_dialog->openais_address->text()),
                tr("&Continue"), nullptr, nullptr,
                0 );
            return false;
        }
    }

    return true;
}

