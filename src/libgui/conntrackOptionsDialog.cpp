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

#include "conntrackOptionsDialog.h"
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


conntrackOptionsDialog::conntrackOptionsDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::conntrackOptionsDialog_q;
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
        os_res->getResourceStr("/FWBuilderResources/Target/protocols/conntrack/default_address");
    string default_port =
        os_res->getResourceStr("/FWBuilderResources/Target/protocols/conntrack/default_port");

    string addr = gropt->getStr("conntrack_address");
    if (addr.empty()) gropt->setStr("conntrack_address", default_address);

    string port = gropt->getStr("conntrack_port");
    if (port.empty()) gropt->setStr("conntrack_port", default_port);

    data.registerOption(m_dialog->use_unicast,
                        gropt,
                        "conntrack_unicast");
    data.registerOption(m_dialog->conntrack_address,
                        gropt,
                        "conntrack_address");

    data.registerOption(m_dialog->conntrack_port,
                        gropt,
                        "conntrack_port");
    data.loadAll();

    toggleUseUnicast();
}

conntrackOptionsDialog::~conntrackOptionsDialog()
{
    delete m_dialog;
}

void conntrackOptionsDialog::accept()
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

void conntrackOptionsDialog::reject()
{
    QDialog::reject();
}

bool conntrackOptionsDialog::validate()
{
    try
    {
        InetAddr( m_dialog->conntrack_address->text().toLatin1().constData() );
    } catch (FWException &ex)
    {
        try
        {
            InetAddr(AF_INET6, m_dialog->conntrack_address->text().toLatin1().constData() );
        } catch (FWException &ex)
        {
            QMessageBox::critical(
                this, "Firewall Builder",
                tr("Invalid IP address '%1'").arg(m_dialog->conntrack_address->text()),
                tr("&Continue"), nullptr, nullptr,
                0 );
            return false;
        }
    }

    return true;
}

void conntrackOptionsDialog::toggleUseUnicast()
{
    bool onoff = m_dialog->use_unicast->isChecked();
    m_dialog->conntrack_address->setEnabled( ! onoff );
    m_dialog->conntrack_address_label->setEnabled( ! onoff );
}

