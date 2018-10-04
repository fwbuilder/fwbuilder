/*

                          Firewall Builder

                 Copyright (C) 201 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

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
#include "AttachedNetworksDialog.h"
#include "ProjectPanel.h"
#include "FWBSettings.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/AttachedNetworks.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Inet6AddrMask.h"

#include <memory>

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <QUndoStack>
#include <QtDebug>
#include <QApplication>


using namespace std;
using namespace libfwbuilder;

AttachedNetworksDialog::AttachedNetworksDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::AttachedNetworksDialog_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

AttachedNetworksDialog::~AttachedNetworksDialog() { delete m_dialog; }

void AttachedNetworksDialog::loadFWObject(FWObject *o)
{
    obj=o;
    AttachedNetworks *s = dynamic_cast<AttachedNetworks*>(obj);
    assert(s!=nullptr);
    
    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->commentKeywords->loadFWObject(o);

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->addresses->setEnabled(false);  // always read-only
    setDisabledPalette(m_dialog->addresses);

    Interface *parent_intf = Interface::cast(obj->getParent());
    assert(parent_intf);

    m_dialog->addresses->clear();

    FWObjectTypedChildIterator k = parent_intf->findByType(IPv4::TYPENAME);
    for ( ; k!=k.end(); ++k)
    {
        Address *addr = Address::cast(*k);
        addAddressToList(addr->getNetworkAddressPtr(), addr->getNetmaskPtr());
    }

    k = parent_intf->findByType(IPv6::TYPENAME);
    for ( ; k!=k.end(); ++k)
    {
        Address *addr = Address::cast(*k);
        addAddressToList(addr->getNetworkAddressPtr(), addr->getNetmaskPtr());
    }

    init=false;
}

void AttachedNetworksDialog::addAddressToList(const InetAddr *ip_addr,
                                              const InetAddr *ip_netm)
{
    QString name("%1/%2");
    QString itm;
    if (ip_addr->isV6())
    {
        itm = name.arg(ip_addr->toString().c_str()).arg(ip_netm->getLength());
    } else
    {
        itm = name.arg(ip_addr->toString().c_str()).arg(ip_netm->toString().c_str());
    }

    QList<QListWidgetItem*> items = m_dialog->addresses->findItems(
        itm, Qt::MatchExactly);
    if (items.size() == 0) m_dialog->addresses->addItem(itm);
}

void AttachedNetworksDialog::validate(bool *result)
{
    if (fwbdebug) qDebug() << "AttachedNetworksDialog::validate";

    *result = true;

#ifndef NDEBUG
    AttachedNetworks *s = dynamic_cast<AttachedNetworks*>(obj);
    assert(s!=nullptr);
#endif

    if (!validateName(this, obj, m_dialog->obj_name->text()))
    {
        *result = false;
    }
}


void AttachedNetworksDialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

#ifndef NDEBUG
    AttachedNetworks *s = dynamic_cast<AttachedNetworks*>(new_state);
    assert(s!=nullptr);
#endif

    string oldname = obj->getName();
    new_state->setName(string(m_dialog->obj_name->text().toUtf8().constData()));
    m_dialog->commentKeywords->applyChanges(new_state);

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (fwbdebug)
            qDebug() << "Pushing FWCmdChange to undo stack";

        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
}
