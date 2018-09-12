/*
 * secuwallIfaceOptsDialog.cpp - secunet wall Interface options implementation
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#include "secuwallIfaceOptsDialog.h"
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

secuwallIfaceOptsDialog::secuwallIfaceOptsDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::secuwallIfaceOptsDialog_q;
    m_dialog->setupUi(this);
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

    data.registerOption(m_dialog->iface_mtu,
                        ifopt,
                        "iface_mtu");

    data.registerOption(m_dialog->iface_disablearp,
                        ifopt,
                        "iface_disablearp");

    data.registerOption(m_dialog->iface_disableboot,
                        ifopt,
                        "iface_disableboot");

    data.registerOption(m_dialog->iface_options,
                        ifopt,
                        "iface_options");

    data.registerOption(m_dialog->vlan_id,
                        ifopt,
                        "vlan_id");

    data.loadAll();

    // perform special actions for different iface types
    typeChanged("");
}

secuwallIfaceOptsDialog::~secuwallIfaceOptsDialog()
{
    delete m_dialog;
}

/*
 * store all data in the object
 */
void secuwallIfaceOptsDialog::accept()
{
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

void secuwallIfaceOptsDialog::reject()
{
    QDialog::reject();
}

void secuwallIfaceOptsDialog::help()
{
    QString tab_title = m_dialog->tabWidget->tabText(
                            m_dialog->tabWidget->currentIndex());
    QString anchor = tab_title.replace('/', '-').replace(' ', '-').toLower();
    Help *h = Help::getHelpWindow(this);
    h->setName("Interface secunet wall");
    h->setSource(QUrl("secuwallIfaceOptsDialog.html#" + anchor));
    h->raise();
    h->show();
}

void secuwallIfaceOptsDialog::typeChanged(const QString&)
{
    QString new_type = m_dialog->iface_type->itemData(
        m_dialog->iface_type->currentIndex()).toString();

    // enable VLAN page for type VLAN
    if (new_type == "8021q")
    {
        m_dialog->options_stack->setCurrentIndex(1);
        return;
    }

    // page 0 is empty
    m_dialog->options_stack->setCurrentIndex(0);
}

