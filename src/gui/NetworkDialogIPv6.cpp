/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: NetworkDialogIPv6.cpp,v 1.22 2007/04/14 00:18:43 vkurland Exp $

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

#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"
#include "utils.h"

#include "FWBTree.h"
#include "NetworkDialogIPv6.h"
#include "ProjectPanel.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWException.h"

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include "FWBSettings.h"

#include "FWWindow.h"
using namespace std;
using namespace libfwbuilder;

NetworkDialogIPv6::NetworkDialogIPv6(ProjectPanel *project, QWidget *parent) : QWidget(parent), m_project(project)
{
    m_dialog = new Ui::NetworkDialogIPv6_q;
    m_dialog->setupUi(this);
    obj=NULL;
}

NetworkDialogIPv6::~NetworkDialogIPv6() { delete m_dialog; }

void NetworkDialogIPv6::loadFWObject(FWObject *o)
{
    obj=o;
    NetworkIPv6 *s = dynamic_cast<NetworkIPv6*>(obj);
    assert(s!=NULL);

    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->address->setText( s->getAddressPtr()->toString().c_str() );
    m_dialog->netmask->setText( QString("%1").arg(
                                    s->getNetmaskPtr()->getLength()) );
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

void NetworkDialogIPv6::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void NetworkDialogIPv6::validate(bool *res)
{
    *res=true;

    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }

    NetworkIPv6 *s = dynamic_cast<NetworkIPv6*>(obj);
    assert(s!=NULL);
    try
    {
        InetAddr(AF_INET6, m_dialog->address->text().toLatin1().constData() );
    } catch (FWException &ex)
    {
        *res=false;
        QMessageBox::critical(this, "Firewall Builder",
                              tr("Illegal IP address '%1'").arg(
                                  m_dialog->address->text()),
                              tr("&Continue"), 0, 0,
                              0 );
    }
    
    bool ok = false;
    int range =m_dialog->netmask->text().toInt(&ok); 
    if (ok && range >= 0 && range < 128)
    {
    }
    else
    {
        *res=false;
        QMessageBox::critical(this, "Firewall Builder",
                              tr("Illegal netmask '%1'").arg(
                                  m_dialog->netmask->text() ),
                              tr("&Continue"), 0, 0,
                              0 );
    }
}

void NetworkDialogIPv6::isChanged(bool*)
{
    //*res=(!init && apply->isEnabled());
}

void NetworkDialogIPv6::libChanged()
{
    changed();
}

void NetworkDialogIPv6::applyChanges()
{
    NetworkIPv6 *s = dynamic_cast<NetworkIPv6*>(obj);
    assert(s!=NULL);

    string oldname=obj->getName();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );
    try
    {
        s->setAddress(
            InetAddr(AF_INET6, m_dialog->address->text().toLatin1().constData()) );
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
    mw->updateObjName(obj,QString::fromUtf8(oldname.c_str()));

    //apply->setEnabled( false );
    mw->updateLastModifiedTimestampForAllFirewalls(obj);
}

void NetworkDialogIPv6::discardChanges()
{
    loadFWObject(obj);
}


/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void NetworkDialogIPv6::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}

