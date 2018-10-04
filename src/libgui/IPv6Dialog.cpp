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

#include "IPv6Dialog.h"
#include "ProjectPanel.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/IPv6.h"
#include "fwbuilder/InetAddr.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWException.h"

#include <memory>

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qhostinfo.h>
#include <qapplication.h>
#include <qcursor.h>
#include <QUndoStack>
#include <QApplication>

#include <iostream>

#include "FWWindow.h"

using namespace std;
using namespace libfwbuilder;

IPv6Dialog::IPv6Dialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::IPv6Dialog_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

IPv6Dialog::~IPv6Dialog()
{
    delete m_dialog;
}

void IPv6Dialog::loadFWObject(FWObject *o)
{
    obj=o;
    IPv6 *s = dynamic_cast<IPv6*>(obj);
    assert(s!=nullptr);

    dnsBusy=false;
    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->commentKeywords->loadFWObject(o);

/*
 * if this is an address that belongs to an interface, we can't move
 * it from library to library just like that. Only IPv4 objects that
 * belong to the standard group "Addresses" can be moved.
 */
    if ( Interface::isA( obj->getParent() ) )
    {
        showNetmask=true;
        m_dialog->netmaskLabel->show();
        m_dialog->netmask->show();
    } else
    {
        showNetmask=false;
        m_dialog->netmaskLabel->hide();
        m_dialog->netmask->hide();
    }

/* catch exceptions separately so even if we have a bad address, we
 * still can show netmask */
    try
    {
        m_dialog->address->setText(InetAddr(AF_INET6, 0).toString().c_str() );
        const InetAddr *inet_addr = s->getAddressPtr();
        m_dialog->address->setText(
            inet_addr->toString().c_str());
    } catch (FWException &ex) {}

    try
    {
        if ( Interface::isA( obj->getParent() ) )
            m_dialog->netmask->setText( QString("%1").arg(
                                            s->getNetmaskPtr()->getLength()) );
    } catch (FWException &ex) {}

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->address->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->address);

    m_dialog->netmask->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->netmask);

    init=false;
}

void IPv6Dialog::validate(bool *res)
{
    *res=true;

    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }

#ifndef NDEBUG
    IPv6 *s = dynamic_cast<IPv6*>(obj);
    assert(s!=nullptr);
#endif

    try
    {
        InetAddr(AF_INET6, m_dialog->address->text().trimmed().toLatin1().constData() );
    } catch (FWException &ex)
    {
        *res = false;
        if (QApplication::focusWidget() != nullptr)
        {
            blockSignals(true);
            QMessageBox::critical(this, "Firewall Builder",
                                  tr("Illegal IP address '%1'").arg(m_dialog->address->text()),
                                  tr("&Continue"), 0, 0,
                                  0 );
            blockSignals(false);
        }
    }

    if ( showNetmask )
    {
        try
        {
            bool ok = false;
            InetAddr(AF_INET6, m_dialog->netmask->text().trimmed().toInt(&ok));
            if (!ok) throw FWException("");
        } catch (FWException &ex)
        {
            *res = false;
            if (QApplication::focusWidget() != nullptr)
            {
                blockSignals(true);
                QMessageBox::critical(this, "Firewall Builder",
                                      tr("Illegal netmask '%1'").arg(m_dialog->netmask->text()),
                                      tr("&Continue"), 0, 0,
                                      0 );
                blockSignals(false);
            }
        }
    }
}



void IPv6Dialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    IPv6 *s = dynamic_cast<IPv6*>(new_state);
    assert(s!=nullptr);

    string oldname=obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    try
    {
        s->setAddress(
            InetAddr(AF_INET6, m_dialog->address->text().trimmed().toLatin1().constData()) );
    } catch (FWException &ex) { }

    if ( showNetmask )
    {
        try
        {
            bool ok = false;
            s->setNetmask(
                InetAddr(AF_INET6, m_dialog->netmask->text().trimmed().toInt(&ok)) );
            if (!ok) throw FWException("");
        } catch (FWException &ex) { }
    } else
        s->setNetmask(InetAddr(AF_INET6, 0));

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
}

void IPv6Dialog::DNSlookup()
{
    if (fwbdebug)
        qDebug("IPv6Dialog::DNSlookup()  dnsBusy=%d", dnsBusy);

    if (!dnsBusy)
    {
        QString name = m_dialog->obj_name->text().trimmed();

        if (fwbdebug)
            qDebug("IPv6Dialog::DNSlookup()  name=%s",
                   name.toLatin1().constData());

        dnsBusy=true;
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );

        QString addr = getAddrByName(name, AF_INET6);

        QApplication::restoreOverrideCursor();
        dnsBusy=false;

        if (fwbdebug) qDebug("IPv6Dialog::DNSlookup()  done");

        if (! addr.isEmpty())
        {
            m_dialog->address->setText( addr );
            changed();
            return;
        }

        if ( Interface::isA(obj->getParent()) )
        {
            FWObject *host = obj->getParent()->getParent();
            assert(host!=nullptr);
            name = host->getName().c_str();

            if (fwbdebug) qDebug("IPv6Dialog::DNSlookup()  name=%s",
                name.toLatin1().constData());
            dnsBusy=true;
            QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );

            QString addr = getAddrByName(name, AF_INET6);

            QApplication::restoreOverrideCursor();
            dnsBusy=false;
            if (fwbdebug) qDebug("IPv6Dialog::DNSlookup()  done");
            if ( ! addr.isEmpty())
            {
                m_dialog->address->setText( addr );
                changed();
                return;
            }
            QMessageBox::warning(
                this,"Firewall Builder",
                tr("DNS lookup failed for both names of the address object '%1' and the name of the host '%2'.")
                .arg(m_dialog->obj_name->text()).arg(name),
                "&Continue", QString::null,QString::null, 0, 1 );
            return;
        }
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("DNS lookup failed for name of the address object '%1'.")
            .arg(name),
            "&Continue", QString::null,QString::null, 0, 1 );
        return;
    }
}
