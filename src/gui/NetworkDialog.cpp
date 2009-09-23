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

#include "../../config.h"
#include "global.h"
#include "utils.h"

#include "FWBTree.h"
#include "NetworkDialog.h"
#include "ProjectPanel.h"
#include "FWBSettings.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Inet6AddrMask.h"

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

#include "FWWindow.h"
using namespace std;
using namespace libfwbuilder;

NetworkDialog::NetworkDialog(ProjectPanel *project, QWidget *parent) : QWidget(parent), m_project(project)
{
    m_dialog = new Ui::NetworkDialog_q;
    m_dialog->setupUi(this);
    obj=NULL;
}

NetworkDialog::~NetworkDialog() { delete m_dialog; }

void NetworkDialog::getHelpName(QString *str)
{
    *str = "NetworkDialog";
}

void NetworkDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Network *s = dynamic_cast<Network*>(obj);
    assert(s!=NULL);
    
    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->address->setText( s->getAddressPtr()->toString().c_str() );
    m_dialog->netmask->setText( s->getNetmaskPtr()->toString().c_str() );
    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->address->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->address);

    m_dialog->netmask->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->netmask);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);


    init=false;
}

void NetworkDialog::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void NetworkDialog::validate(bool *result)
{
    *result=true;

    if (!isTreeReadWrite(this,obj)) { *result=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text()))
    {
        *result=false;
        return;
    }

    Network *s = dynamic_cast<Network*>(obj);
    assert(s!=NULL);
    try
    {
        InetAddr( m_dialog->address->text().toStdString() );
    } catch (FWException &ex)
    {
        *result=false;
        QMessageBox::critical(
            this, "Firewall Builder",
            tr("Illegal IP address '%1'").arg(m_dialog->address->text()),
            tr("&Continue"), 0, 0,
            0 );
    }
    try
    {
        QString len = m_dialog->netmask->text() ;
        bool ok = false ;
        int ilen = len.toInt (&ok);
        if (ok)
        {
            if (ilen>0 && ilen < 32)
            {
                return ;
            }
            else
            {
                *result=false;
                QMessageBox::critical(
                    this, "Firewall Builder",
                    tr("Illegal netmask '%1'").arg( m_dialog->netmask->text() ),
                    tr("&Continue"), 0, 0,
                    0 );
            }
        }
        InetAddr nm( m_dialog->netmask->text().toStdString() );
        // Do not allow netmask of 0 bits See #251
        if (nm.isAny())
            throw FWException("Network object can not have netmask 0.0.0.0");

    } catch (FWException &ex)
    {

        *result=false;
        QMessageBox::critical(
            this, "Firewall Builder",
            tr("Illegal netmask '%1'").arg( m_dialog->netmask->text() ),
            tr("&Continue"), 0, 0,
            0 );
    }
}

void NetworkDialog::isChanged(bool*)
{
    //*result=(!init && apply->isEnabled());
}

void NetworkDialog::libChanged()
{
    changed();
}

void NetworkDialog::applyChanges()
{
    Network *s = dynamic_cast<Network*>(obj);
    assert(s!=NULL);

    string oldname = obj->getName();
    obj->setName(string(m_dialog->obj_name->text().toUtf8().constData()));
    obj->setComment(string(
                        m_dialog->comment->toPlainText().toUtf8().constData()));
    try
    {
        s->setAddress(InetAddr(m_dialog->address->text().toStdString()));
    } catch (FWException &ex)
    {
// exception thrown if user types illegal m_dialog->address or
// m_dialog->netmask
    }

    try
    {
        QString len = m_dialog->netmask->text() ;
        bool ok = false ;
        int ilen = len.toInt (&ok);
        if (ok)
        {
            s->setNetmask(InetAddr(ilen));
        }
        else
        {
            s->setNetmask(InetAddr(m_dialog->netmask->text().toStdString()));
        }
    } catch (FWException &ex)
    {
// exception thrown if user types illegal m_dialog->address or
// m_dialog->netmask
//        bool ok = false ;
    }

    m_project->updateObjName(obj,QString::fromUtf8(oldname.c_str()));

    emit notify_changes_applied_sign();

}

void NetworkDialog::discardChanges()
{
    loadFWObject(obj);
}


/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void NetworkDialog::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}

void NetworkDialog::addressEntered()
{
    try
    {
        QString addr = m_dialog->address->text();
        InetAddrMask address_and_mask(string(addr.toStdString()));
        if (addr.contains('/'))
        {
            m_dialog->address->setText(
                address_and_mask.getAddressPtr()->toString().c_str());
            m_dialog->netmask->setText(
                address_and_mask.getNetmaskPtr()->toString().c_str());
        }
    } catch (FWException &ex)
    {
        // exception thrown if user types illegal m_dialog->address do
        // not show error dialog. This method is called by
        // editingFinished signal and therefore is invoked when user
        // switches focus from the address input widget to some other
        // widget or even when user switches to another application to
        // look up the address. Error dialog interrupts the workflow
        // in the latter case which is annoying.
    }

}

