/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: IPServiceDialog.cpp,v 1.24 2007/04/14 00:18:43 vkurland Exp $

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
#include "IPServiceDialog.h"
#include "ProjectPanel.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ServiceGroup.h"

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>

#include <iostream>
#include "FWBSettings.h"

#include "FWWindow.h"
using namespace libfwbuilder;
using namespace std;

IPServiceDialog::IPServiceDialog(ProjectPanel *project, QWidget *parent) : QWidget(parent), m_project(project)
{
    m_dialog = new Ui::IPServiceDialog_q;
    m_dialog->setupUi(this);
    setFont(st->getUiFont());
    obj=NULL;
}

IPServiceDialog::~IPServiceDialog()
{
    delete m_dialog;
}

void IPServiceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    IPService *s = dynamic_cast<IPService*>(obj);
    assert(s!=NULL);

    init=true;

    fillLibraries(m_dialog->libs,obj);

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->protocolNum->setValue( s->getProtocolNumber() );
    m_dialog->lsrr->setChecked( s->getBool("m_dialog->lsrr") );
    m_dialog->ssrr->setChecked( s->getBool("m_dialog->ssrr") );
    m_dialog->rr->setChecked( s->getBool("m_dialog->rr") );
    m_dialog->timestamp->setChecked( s->getBool("ts") );
    m_dialog->all_fragments->setChecked( s->getBool("fragm") );
    m_dialog->short_fragments->setChecked( s->getBool("short_fragm") );

    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->libs->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->libs);

    m_dialog->protocolNum->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->protocolNum);

    m_dialog->lsrr->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->lsrr);

    m_dialog->ssrr->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->ssrr);

    m_dialog->rr->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->rr);

    m_dialog->timestamp->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->timestamp);

    m_dialog->all_fragments->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->all_fragments);

    m_dialog->short_fragments->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->short_fragments);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);


    init=false;
}

void IPServiceDialog::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void IPServiceDialog::validate(bool *res)
{
    *res=true;
    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}

void IPServiceDialog::isChanged(bool*)
{
    //*res=(!init && apply->isEnabled());
}

void IPServiceDialog::libChanged()
{
    changed();
}

void IPServiceDialog::applyChanges()
{
    string oldname=obj->getName();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );

    obj->setInt("protocol_num", m_dialog->protocolNum->value() );
    obj->setBool("m_dialog->lsrr", m_dialog->lsrr->isChecked() );
    obj->setBool("m_dialog->ssrr", m_dialog->ssrr->isChecked() );
    obj->setBool("m_dialog->rr", m_dialog->rr->isChecked() );
    obj->setBool("ts", m_dialog->timestamp->isChecked() );
    obj->setBool("fragm", m_dialog->all_fragments->isChecked() );
    obj->setBool("short_fragm", m_dialog->short_fragments->isChecked() );

    mw->updateObjName(obj,QString::fromUtf8(oldname.c_str()));

    init=true;

/* move to another lib if we have to */
    if (! m_project->isSystem(obj) && m_dialog->libs->currentText() != QString(obj->getLibrary()->getName().c_str()))
        mw->moveObject(m_dialog->libs->currentText(), obj);

    init=false;

    //apply->setEnabled( false );
    mw->updateLastModifiedTimestampForAllFirewalls(obj);
}

void IPServiceDialog::discardChanges()
{
    loadFWObject(obj);
}


/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void IPServiceDialog::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}

