/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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
#include "NetworkDialogIPv6.h"
#include "ProjectPanel.h"
#include "FWBSettings.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/Interface.h"
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
#include <QApplication>


using namespace std;
using namespace libfwbuilder;

NetworkDialogIPv6::NetworkDialogIPv6(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::NetworkDialogIPv6_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

NetworkDialogIPv6::~NetworkDialogIPv6() { delete m_dialog; }

void NetworkDialogIPv6::loadFWObject(FWObject *o)
{
    obj=o;
    NetworkIPv6 *s = dynamic_cast<NetworkIPv6*>(obj);
    assert(s!=nullptr);

    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->address->setText( s->getAddressPtr()->toString().c_str() );
    m_dialog->netmask->setText( QString("%1").arg(
                                    s->getNetmaskPtr()->getLength()) );
    m_dialog->commentKeywords->loadFWObject(o);

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->address->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->address);

    m_dialog->netmask->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->netmask);

    init=false;
}

void NetworkDialogIPv6::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void NetworkDialogIPv6::validate(bool *res)
{
    *res = true;

    if (!validateName(this,obj,m_dialog->obj_name->text()))
    {
        *res = false;
        return;
    }

#ifndef NDEBUG
    NetworkIPv6 *s = dynamic_cast<NetworkIPv6*>(obj);
    assert(s != nullptr);
#endif

    try
    {
        InetAddr(AF_INET6, m_dialog->address->text().toStdString() );
    } catch (FWException &ex)
    {
        *res = false;
        if (QApplication::focusWidget() != nullptr)
        {
            blockSignals(true);
            QMessageBox::critical(this, "Firewall Builder",
                                  tr("Illegal IPv6 address '%1'").arg(
                                      m_dialog->address->text()),
                                  tr("&Continue"), nullptr, nullptr,
                                  0 );
            blockSignals(false);
        }
    }
    
    bool ok = false;
    int range = m_dialog->netmask->text().toInt(&ok); 
    // Do not allow netmask of 0 bits. See #251
    if (ok && range > 0 && range < 128)
    {
        return;
    }
    else
    {
        *res = false;
        if (QApplication::focusWidget() != nullptr)
        {
            blockSignals(true);
            QMessageBox::critical(this, "Firewall Builder",
                                  tr("Illegal netmask '%1'").arg(
                                      m_dialog->netmask->text() ),
                                  tr("&Continue"), nullptr, nullptr,
                                  0 );
            blockSignals(false);
        }
    }
}



void NetworkDialogIPv6::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    NetworkIPv6 *s = dynamic_cast<NetworkIPv6*>(new_state);
    assert(s!=nullptr);

    string oldname=obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    try
    {
        s->setAddress(
            InetAddr(AF_INET6,
                     m_dialog->address->text().toStdString()) );
        bool ok = false;
        s->setNetmask(
            InetAddr(AF_INET6, m_dialog->netmask->text().toInt(&ok)) );
        if (!ok) throw FWException("");
    } catch (FWException &ex)
    {
/* exception thrown if user types illegal m_dialog->address or
 * m_dialog->netmask
 */

    }

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
    

}

void NetworkDialogIPv6::addressEntered()
{
    try
    {
        QString addr = m_dialog->address->text();
        Inet6AddrMask address_and_mask(string(addr.toStdString()));
        if (addr.contains('/'))
        {
            m_dialog->address->setText(
                address_and_mask.getAddressPtr()->toString().c_str());
            m_dialog->netmask->setText(
                QString().setNum(
                    address_and_mask.getNetmaskPtr()->getLength()));
        }
    } catch (FWException &ex)
    {
// exception thrown if user types illegal m_dialog->address 

    }

}

