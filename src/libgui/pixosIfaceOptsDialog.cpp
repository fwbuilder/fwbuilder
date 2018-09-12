/* 

                          Firewall Builder

                 Copyright (C) 2009 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@vk.crocodile.org

  $Id: pixosIfaceOptsDialog.cpp 1586 2009-10-13 05:05:03Z vadim $

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


#include "pixosIfaceOptsDialog.h"
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

pixosIfaceOptsDialog::pixosIfaceOptsDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::pixosIfaceOptsDialog_q;
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

    data.registerOption(m_dialog->vlan_id, ifopt, "vlan_id");

    data.loadAll();

    // special actions for different iface types
    // VLAN (8021q)
    typeChanged("");
}

pixosIfaceOptsDialog::~pixosIfaceOptsDialog()
{
    delete m_dialog;
}

/*
 * store all data in the object
 */
void pixosIfaceOptsDialog::accept()
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

void pixosIfaceOptsDialog::reject()
{
    QDialog::reject();
}

void pixosIfaceOptsDialog::help()
{
    QString tab_title = m_dialog->tabWidget->tabText(
                            m_dialog->tabWidget->currentIndex());
    QString anchor = tab_title.replace('/', '-').replace(' ', '-').toLower();
    Help *h = Help::getHelpWindow(this);
    h->setName("PIX Interface");
    h->setSource(QUrl("pixosIfaceOptsDialog.html#" + anchor));
    h->raise();
    h->show();
}

void pixosIfaceOptsDialog::typeChanged(const QString&)
{
    QString new_type = m_dialog->iface_type->itemData(
        m_dialog->iface_type->currentIndex()).toString();

    // enable VLAN ID line edit for type VLAN
    if (new_type == "8021q")
    {
        m_dialog->options_stack->setCurrentIndex(1);
        return;
    }

    // page 0 is empty
    m_dialog->options_stack->setCurrentIndex(0);
}

bool pixosIfaceOptsDialog::validate()
{
    bool valid = true;
    QString combobox = m_dialog->iface_type->currentText();
    QString type = m_dialog->iface_type->itemData(
        m_dialog->iface_type->currentIndex()).toString();
    QWidget *focus = nullptr;
    QString message;

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

