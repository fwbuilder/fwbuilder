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
#include "utils.h"

#include "FWBTree.h"
#include "HostDialog.h"
#include "ProjectPanel.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Host.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/FWException.h"

#include <memory>

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <QUndoStack>

#include "FWWindow.h"

using namespace std;
using namespace libfwbuilder;

HostDialog::HostDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::HostDialog_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

HostDialog::~HostDialog()
{
    delete m_dialog;
}

void HostDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Host *s = dynamic_cast<Host*>(obj);
    assert(s!=nullptr);

    init = true;

#ifndef NDEBUG
    Management *mgmt=s->getManagementObject();
    assert(mgmt!=nullptr);
#endif

    FWOptions  *opt =s->getOptionsObject();

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
//    snmpCommunity->setText( mgmt->getSNMPManagement()->getReadCommunity().c_str() );
    m_dialog->MACmatching->setChecked( opt->getBool("use_mac_addr_filter") );

    m_dialog->commentKeywords->loadFWObject(o);

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

//    snmpCommunity->setEnabled(!o->isReadOnly());
//    setDisabledPalette(snmpCommunity);

    m_dialog->MACmatching->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->MACmatching);

    init=false;
}

void HostDialog::validate(bool *res)
{
    *res = true;
    if (!validateName(this,obj,m_dialog->obj_name->text()))
    {
        *res=false;
        return;
    }
}



void HostDialog::applyChanges()
{
    bool autorename_chidren = false;
    QString dialog_txt = tr(
        "The name of the object '%1' has changed. The program can also "
        "rename IP address objects that belong to this object, "
        "using standard naming scheme 'host_name:interface_name:ip'. "
        "This makes it easier to distinguish what host or a firewall "
        "given IP address object belongs to when it is used in "
        "the policy or NAT rule. The program also renames MAC address "
        "objects using scheme 'host_name:interface_name:mac'. "
        "Do you want to rename child IP and MAC address objects now? "
        "(If you click 'No', names of all address objects that belong to "
        "%2 will stay the same.)")
        .arg(QString::fromUtf8(obj->getName().c_str()))
        .arg(QString::fromUtf8(obj->getName().c_str()));

    if (obj->getName() != m_dialog->obj_name->text().toUtf8().constData())
    {
        /* see comment about this in FirewallDialog */
        blockSignals(true);
        autorename_chidren = (QMessageBox::warning(
                                  this,"Firewall Builder", dialog_txt,
                                  tr("&Yes"), tr("&No"), QString::null,
                                  0, 1 )==0 );
        blockSignals(false);
    }

    std::unique_ptr<FWCmdChange> cmd(
        new FWCmdChange(m_project, obj, "", autorename_chidren));
    FWObject* new_state = cmd->getNewState();

    Host *s = dynamic_cast<Host*>(new_state);
    assert(s!=nullptr);

#ifndef NDEBUG
    Management *mgmt = s->getManagementObject();
    assert(mgmt!=nullptr);
#endif

    FWOptions  *opt =s->getOptionsObject();

    string oldname=obj->getName();
    new_state->setName(string(m_dialog->obj_name->text().toUtf8().constData()));
    m_dialog->commentKeywords->applyChanges(new_state);
    opt->setBool("use_mac_addr_filter", m_dialog->MACmatching->isChecked());

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
}

