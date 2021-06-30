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

#include "heartbeatOptionsDialog.h"
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


heartbeatOptionsDialog::heartbeatOptionsDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::heartbeatOptionsDialog_q;
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
            "/FWBuilderResources/Target/protocols/heartbeat/default_address");
    string default_port =
        os_res->getResourceStr(
            "/FWBuilderResources/Target/protocols/heartbeat/default_port");

    string addr = gropt->getStr("heartbeat_address");
    if (addr.empty()) gropt->setStr("heartbeat_address", default_address);

    string port = gropt->getStr("heartbeat_port");
    if (port.empty()) gropt->setStr("heartbeat_port", default_port);

    data.registerOption(m_dialog->use_unicast,
                        gropt,
                        "heartbeat_unicast");
    data.registerOption(m_dialog->heartbeat_address,
                        gropt,
                        "heartbeat_address");
    data.registerOption(m_dialog->heartbeat_port,
                        gropt,
                        "heartbeat_port");
    data.loadAll();

    toggleUseUnicast();
}

heartbeatOptionsDialog::~heartbeatOptionsDialog()
{
    delete m_dialog;
}

void heartbeatOptionsDialog::accept()
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

void heartbeatOptionsDialog::reject()
{
    QDialog::reject();
}

bool heartbeatOptionsDialog::validate()
{
    try
    {
        InetAddr(m_dialog->heartbeat_address->text().toLatin1().constData());
    } catch (FWException &ex)
    {
        try
        {
            InetAddr(AF_INET6, m_dialog->heartbeat_address->text().toLatin1().constData() );
        } catch (FWException &ex)
        {
            QMessageBox::critical(
                this, "Firewall Builder",
                tr("Invalid IP address '%1'").arg(m_dialog->heartbeat_address->text()),
                tr("&Continue"), nullptr, nullptr,
                0 );
            return false;
        }
    }
    return true;
}

void heartbeatOptionsDialog::toggleUseUnicast()
{
    bool onoff = m_dialog->use_unicast->isChecked();
    m_dialog->heartbeat_address->setEnabled( ! onoff );
    m_dialog->heartbeat_address_label->setEnabled( ! onoff );
}

