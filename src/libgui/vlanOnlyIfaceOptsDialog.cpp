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


#include "vlanOnlyIfaceOptsDialog.h"
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

vlanOnlyIfaceOptsDialog::vlanOnlyIfaceOptsDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::vlanOnlyIfaceOptsDialog_q;
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

    m_dialog->vlan_id->setValue(ifopt->getInt("vlan_id"));

    // special actions for different iface types
    // VLAN (8021q)
    typeChanged("");
}

vlanOnlyIfaceOptsDialog::~vlanOnlyIfaceOptsDialog()
{
    delete m_dialog;
}

/*
 * store all data in the object
 */
void vlanOnlyIfaceOptsDialog::accept()
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

    ifopt->setInt("vlan_id", m_dialog->vlan_id->value());

    if (!cmd->getOldState()->cmp(new_state, true))
        project->undoStack->push(cmd.release());
    
    QDialog::accept();
}

void vlanOnlyIfaceOptsDialog::reject()
{
    QDialog::reject();
}

void vlanOnlyIfaceOptsDialog::help()
{
    QString tab_title = m_dialog->tabWidget->tabText(
                            m_dialog->tabWidget->currentIndex());
    QString anchor = tab_title.replace('/', '-').replace(' ', '-').toLower();
    Help *h = Help::getHelpWindow(this);
    h->setName("Interface Properties");
    h->setSource(QUrl("vlanOnlyIfaceOptsDialog.html#" + anchor));
    h->raise();
    h->show();
}

void vlanOnlyIfaceOptsDialog::typeChanged(const QString&)
{
    QString new_type = m_dialog->iface_type->itemData(
        m_dialog->iface_type->currentIndex()).toString();

    // enable VLAN ID line edit for type VLAN
    bool enable_vlan = (new_type == "8021q");

    m_dialog->vlan_id->setEnabled(enable_vlan);
    m_dialog->vlan_label->setEnabled(enable_vlan);
}

bool vlanOnlyIfaceOptsDialog::validate()
{
    bool valid = true;
    QString combobox = m_dialog->iface_type->currentText();
    QString type = m_dialog->iface_type->itemData(
        m_dialog->iface_type->currentIndex()).toString();
    QWidget *focus = nullptr;
    QString message;

    if (type == "vrrp")
    {
        // Both vvrp_secret and vrrp_id attributes moved to vrrpOptionsDialog
        ;
    }

    if (type == "8021q")
    {
        // VLAN ID must be set between 1 <= vid <= 4'094
        // QSpinBox widget enforces these limits
        ;
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

