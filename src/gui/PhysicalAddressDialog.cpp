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
#include "PhysicalAddressDialog.h"
#include "ProjectPanel.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/physAddress.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWException.h"

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <QUndoStack>

#include <iostream>

using namespace std;
using namespace libfwbuilder;

PhysicalAddressDialog::PhysicalAddressDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::PhysAddressDialog_q;
    m_dialog->setupUi(this);
    obj=NULL;
}

PhysicalAddressDialog::~PhysicalAddressDialog()
{
    delete m_dialog;
}

void PhysicalAddressDialog::getHelpName(QString *str)
{
    *str = "PhysicalAddressDialog";
}

void PhysicalAddressDialog::loadFWObject(FWObject *o)
{
    obj=o;
    physAddress *s = dynamic_cast<physAddress*>(obj);
    assert(s!=NULL);

    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->pAddress->setText( s->getPhysAddress().c_str() );
    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->pAddress->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->pAddress);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);


    init=false;
}

void PhysicalAddressDialog::validate(bool *res)
{
    *res=true;

    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}



void PhysicalAddressDialog::applyChanges()
{
    FWCmdChange* cmd = new FWCmdChange(m_project, obj);
    FWObject* new_state = cmd->getNewState();

    physAddress *s = dynamic_cast<physAddress*>(new_state);
    assert(s!=NULL);

    string oldname=obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    new_state->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );
    s->setPhysAddress( m_dialog->pAddress->text().toLatin1().constData() );

    m_project->undoStack->push(cmd);
    
    BaseObjectDialog::applyChanges();
}

