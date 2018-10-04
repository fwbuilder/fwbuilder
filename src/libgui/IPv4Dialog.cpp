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

#include "FWBSettings.h"
#include "FWCmdChange.h"
#include "IPv4Dialog.h"

#include "ProjectPanel.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWException.h"

#include <memory>

#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <QtDebug>
#include <QApplication>

#include <iostream>


using namespace std;
using namespace libfwbuilder;

IPv4Dialog::IPv4Dialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::IPv4Dialog_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

IPv4Dialog::~IPv4Dialog()
{
    delete m_dialog;
}

void IPv4Dialog::loadFWObject(FWObject *o)
{
    obj=o;
    IPv4 *s = dynamic_cast<IPv4*>(obj);
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
        m_dialog->address->setText( s->getAddressPtr()->toString().c_str() );
    } catch (FWException &ex) {}

    try
    {
        if ( Interface::isA( obj->getParent() ) )
            m_dialog->netmask->setText( s->getNetmaskPtr()->toString().c_str() );
    } catch (FWException &ex) {}

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->address->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->address);

    m_dialog->netmask->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->netmask);

    m_dialog->dnsLookup->setEnabled(!o->isReadOnly());

    init=false;
}

void IPv4Dialog::validate(bool *result)
{
    if (fwbdebug) qDebug() << "IPv4Dialog::validate()";

    *result=true;

    if (!validateName(this,obj,m_dialog->obj_name->text()))
    {
        *result=false;
        return;
    }

#ifndef NDEBUG
    IPv4 *s = dynamic_cast<IPv4*>(obj);
    assert(s!=nullptr);
#endif

    try
    {
        InetAddr( m_dialog->address->text().trimmed().toLatin1().constData() );
    } catch (FWException &ex)
    {
        *result = false;
        if (QApplication::focusWidget() != nullptr)
        {
            blockSignals(true);
            QMessageBox::critical(
                this, "Firewall Builder",
                tr("Illegal IP address '%1'").arg(m_dialog->address->text()),
                tr("&Continue"), 0, 0,
                0 );
            blockSignals(false);
        }
    }

    if (showNetmask)
    {
        try
        {
            InetAddr nm( m_dialog->netmask->text().trimmed().toLatin1().constData() );

            if (!nm.isValidV4Netmask())
            {
                *result = false;
                if (QApplication::focusWidget() != nullptr)
                {
                    blockSignals(true);
                    // Do not allow netmask with zeroes inside.
                    QMessageBox::critical(
                        this, "Firewall Builder",
                        tr("Netmasks with zeroes in the middle are not supported"),
                        tr("&Continue"), 0, 0,
                        0 );
                    blockSignals(false);
                }
                return;
            }

        } catch (FWException &ex)
        {
            *result = false;
            if (QApplication::focusWidget() != nullptr)
            {
                blockSignals(true);
                QMessageBox::critical(
                    this, "Firewall Builder",
                    tr("Illegal netmask '%1'").arg(m_dialog->netmask->text()),
                    tr("&Continue"), 0, 0,
                    0 );
                blockSignals(false);
            }
        }
    }
}


void IPv4Dialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    IPv4 *s = dynamic_cast<IPv4*>(new_state);
    assert(s!=nullptr);

    new_state->setName(m_dialog->obj_name->text().toUtf8().constData());
    m_dialog->commentKeywords->applyChanges(new_state);

    try
    {
        s->setAddress(
            InetAddr(m_dialog->address->text().trimmed().toLatin1().constData()) );
    } catch (FWException &ex) { }

    if ( showNetmask )
    {
        try
        {
            s->setNetmask(
                InetAddr(m_dialog->netmask->text().trimmed().toLatin1().constData()) );
        } catch (FWException &ex) { }
    } else
        s->setNetmask(InetAddr());

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }

}

void IPv4Dialog::DNSlookup()
{
    if (fwbdebug)
        qDebug("IPv4Dialog::DNSlookup()  dnsBusy=%d", dnsBusy);

    if (!dnsBusy)
    {
        QString name = m_dialog->obj_name->text().trimmed();
        if (fwbdebug) qDebug("IPv4Dialog::DNSlookup()  name=%s", name.toLatin1().constData());
        dnsBusy=true;
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
        QString addr = getAddrByName(name, AF_INET);
        QApplication::restoreOverrideCursor();
        dnsBusy=false;
        if (fwbdebug) qDebug("IPv4Dialog::DNSlookup()  done");

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

            if (fwbdebug) qDebug("IPv4Dialog::DNSlookup()  name=%s",
                name.toLatin1().constData());
            dnsBusy=true;
            QApplication::setOverrideCursor( QCursor( Qt::WaitCursor) );
            QString addr = getAddrByName(name, AF_INET);
            QApplication::restoreOverrideCursor();
            dnsBusy=false;
            if (fwbdebug) qDebug("IPv4Dialog::DNSlookup()  done");
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
