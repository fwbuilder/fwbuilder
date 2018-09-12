/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

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


#include "bsdIfaceOptsDialog.h"
#include "platforms.h"
#include "FWCmdChange.h"

#include "fwbuilder/Interface.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Firewall.h"

#include "FWWindow.h"
#include "Help.h"

#include <memory>

#include <qmessagebox.h>

using namespace std;
using namespace libfwbuilder;

bsdIfaceOptsDialog::bsdIfaceOptsDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::bsdIfaceOptsDialog_q;
    m_dialog->setupUi(this);
    setWindowModality(Qt::WindowModal);
    obj = o;

    FWOptions *ifopt = (Interface::cast(obj))->getOptionsObject();
    cluster_interface = (Cluster::cast(obj->getParent()) != nullptr);

    setInterfaceTypes(m_dialog->iface_type, Interface::cast(obj),
                      ifopt->getStr("type").c_str());

    // Using "type" control only for subinterfaces
    // and main interfaces of the firewall objects
    if (cluster_interface)
    {
        m_dialog->iface_type->hide();
        m_dialog->iface_type_label->hide();
    } else
    {
        m_dialog->iface_type->show();
        m_dialog->iface_type_label->show();
    }

    int mtu = ifopt->getInt("iface_mtu");
    if (mtu <=0 )
    {
        mtu = 1500;
        ifopt->setInt("iface_mtu", mtu);
    }

    data.registerOption(m_dialog->vlan_id, ifopt, "vlan_id");
    data.registerOption(m_dialog->iface_configure_mtu, ifopt, "iface_configure_mtu");
    data.registerOption(m_dialog->iface_mtu, ifopt, "iface_mtu");
    data.registerOption(m_dialog->iface_options, ifopt, "iface_options");
    data.registerOption(m_dialog->enable_stp, ifopt, "enable_stp");

    data.loadAll();

    // special actions for different iface types
    // VLAN (8021q)
    typeChanged("");
}

bsdIfaceOptsDialog::~bsdIfaceOptsDialog()
{
    delete m_dialog;
}

/*
 * store all data in the object
 */
void bsdIfaceOptsDialog::accept()
{
    // validate user input before saving
    if (!validate())  return;

    ProjectPanel *project = mw->activeProject();
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(project, obj));

    // new_state  is a copy of the interface object
    FWObject* new_state = cmd->getNewState();
    FWOptions* ifopt = Interface::cast(new_state)->getOptionsObject();
    assert(ifopt!=nullptr);

    if (cluster_interface)
    {
        ifopt->setStr("type", "cluster_interface");
    } else
    {
        QString new_type = m_dialog->iface_type->itemData(
            m_dialog->iface_type->currentIndex()).toString();
        ifopt->setStr("type", new_type.toStdString());
    }

    data.saveAll(ifopt);

    if (!cmd->getOldState()->cmp(new_state, true))
        project->undoStack->push(cmd.release());
    
    QDialog::accept();
}

void bsdIfaceOptsDialog::reject()
{
    QDialog::reject();
}

void bsdIfaceOptsDialog::help()
{
    QString tab_title = m_dialog->tabWidget->tabText(
                            m_dialog->tabWidget->currentIndex());
    QString anchor = tab_title.replace('/', '-').replace(' ', '-').toLower();
    Help *h = Help::getHelpWindow(this);
    h->setName("Interface OpenBSD");
    h->setSource(QUrl("bsdIfaceOptsDialog.html#" + anchor));
    h->raise();
    h->show();
}

void bsdIfaceOptsDialog::typeChanged(const QString&)
{
    QString new_type = m_dialog->iface_type->itemData(
        m_dialog->iface_type->currentIndex()).toString();

    // enable VLAN ID line edit for type VLAN
    if (new_type.isEmpty() || new_type == "ethernet")
    {
        m_dialog->options_stack->setCurrentIndex(1);
        return;
    }

    if (new_type == "8021q")
    {
        m_dialog->options_stack->setCurrentIndex(2);
        return;
    }

    if (new_type == "bridge")
    {
        m_dialog->options_stack->setCurrentIndex(3);
        return;
    }

    // page 0 is empty
    m_dialog->options_stack->setCurrentIndex(0);
}

bool bsdIfaceOptsDialog::validate()
{
    return true;
}

