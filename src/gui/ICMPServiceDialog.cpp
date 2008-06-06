/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: ICMPServiceDialog.cpp,v 1.26 2007/04/14 00:18:43 vkurland Exp $

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

#include "ProjectPanel.h"
#include "FWBTree.h"
#include "ICMPServiceDialog.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/ICMPService.h"

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include "FWBSettings.h"

#include <iostream>

#include "FWWindow.h"
using namespace std;
using namespace libfwbuilder;

ICMPServiceDialog::ICMPServiceDialog(ProjectPanel *project, QWidget *parent) :
        QWidget(parent), m_project(project)
{
    m_dialog = new Ui::ICMPServiceDialog_q;
    m_dialog->setupUi(this);
    setFont(st->getUiFont());
    obj=NULL;
}

ICMPServiceDialog::~ICMPServiceDialog()
{
    delete m_dialog;
}

void ICMPServiceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    ICMPService *s = dynamic_cast<ICMPService*>(obj);
    assert(s!=NULL);

    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->icmpType->setValue( s->getInt("type") );
    m_dialog->icmpCode->setValue( s->getInt("code") );
    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->icmpType->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->icmpType);

    m_dialog->icmpCode->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->icmpCode);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);

    init=false;
}

void ICMPServiceDialog::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void ICMPServiceDialog::validate(bool *res)
{
    *res=true;
    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}

void ICMPServiceDialog::isChanged(bool*)
{
    //*res=(!init && apply->isEnabled());
}

void ICMPServiceDialog::libChanged()
{
    changed();
}

void ICMPServiceDialog::applyChanges()
{
    string oldname=obj->getName();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );

    obj->setInt("type", m_dialog->icmpType->value() );
    obj->setInt("code", m_dialog->icmpCode->value() );

    mw->updateObjName(obj,QString::fromUtf8(oldname.c_str()));

    //apply->setEnabled( false );
    mw->updateLastModifiedTimestampForAllFirewalls(obj);
}

void ICMPServiceDialog::discardChanges()
{
    loadFWObject(obj);
}


/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void ICMPServiceDialog::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}


