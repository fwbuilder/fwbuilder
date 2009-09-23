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
#include "UDPServiceDialog.h"
#include "ProjectPanel.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/UDPService.h"

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include "FWBSettings.h"

#include "FWWindow.h"
using namespace std;
using namespace libfwbuilder;

UDPServiceDialog::UDPServiceDialog(ProjectPanel *project, QWidget *parent) : QWidget(parent), m_project(project)
{
    m_dialog = new Ui::UDPServiceDialog_q;
    m_dialog->setupUi(this);

    obj=NULL;
}

UDPServiceDialog::~UDPServiceDialog()
{
    delete m_dialog;
}

void UDPServiceDialog::getHelpName(QString *str)
{
    *str = "UDPServiceDialog";
}

void UDPServiceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    UDPService *s = dynamic_cast<UDPService*>(obj);
    assert(s!=NULL);

    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->ss->setValue( TCPUDPService::cast(s)->getSrcRangeStart() );
    m_dialog->se->setValue( TCPUDPService::cast(s)->getSrcRangeEnd() );
    m_dialog->ds->setValue( TCPUDPService::cast(s)->getDstRangeStart() );
    m_dialog->de->setValue( TCPUDPService::cast(s)->getDstRangeEnd() );

    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->ss->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->ss);

    m_dialog->se->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->se);

    m_dialog->ds->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->ds);

    m_dialog->de->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->de);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);


    init=false;
}

void UDPServiceDialog::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void UDPServiceDialog::validate(bool *res)
{
    if (fwbdebug) qDebug("UDPServiceDialog::validate");

    *res=true;

    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }

    // check port ranges (bug #1695481, range start must be <= range end)
    int sps = m_dialog->ss->value();
    int spe = m_dialog->se->value();
    int dps = m_dialog->ds->value();
    int dpe = m_dialog->de->value();

    if (sps > spe)
    {
        QMessageBox::warning(this, "Firewall Builder",
          QObject::tr("Invalid range defined for the source port."),
          QObject::tr("&Continue editing"), NULL, NULL, 0, 2 );
        *res = false;
        return;
    }
    if (dps > dpe)
    {
        QMessageBox::warning(this, "Firewall Builder",
          QObject::tr("Invalid range defined for the destination port."),
          QObject::tr("&Continue editing"), NULL, NULL, 0, 2 );
        *res = false;
        return;
    }
}

void UDPServiceDialog::isChanged(bool*)
{
    //*res=(!init && apply->isEnabled());
}

void UDPServiceDialog::libChanged()
{
    changed();
}

void UDPServiceDialog::applyChanges()
{
    string oldname=obj->getName();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );

    if (m_dialog->ss->value()!=0 && m_dialog->se->value()==0) m_dialog->se->setValue( m_dialog->ss->value() );
    if (m_dialog->ds->value()!=0 && m_dialog->de->value()==0) m_dialog->de->setValue( m_dialog->ds->value() );

    TCPUDPService::cast(obj)->setSrcRangeStart(m_dialog->ss->value());
    TCPUDPService::cast(obj)->setSrcRangeEnd(m_dialog->se->value());
    TCPUDPService::cast(obj)->setDstRangeStart(m_dialog->ds->value());
    TCPUDPService::cast(obj)->setDstRangeEnd(m_dialog->de->value());

    m_project->updateObjName(obj,QString::fromUtf8(oldname.c_str()));

    emit notify_changes_applied_sign();

}

void UDPServiceDialog::discardChanges()
{
    loadFWObject(obj);
}


/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void UDPServiceDialog::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}

