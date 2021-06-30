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

#include "FWBTree.h"
#include "PhysicalAddressDialog.h"
#include "ProjectPanel.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/physAddress.h"
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
#include <QUndoStack>

#include <iostream>

using namespace std;
using namespace libfwbuilder;

PhysicalAddressDialog::PhysicalAddressDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::PhysAddressDialog_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

PhysicalAddressDialog::~PhysicalAddressDialog()
{
    delete m_dialog;
}

void PhysicalAddressDialog::loadFWObject(FWObject *o)
{
    obj=o;
    physAddress *s = dynamic_cast<physAddress*>(obj);
    assert(s!=nullptr);

    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->pAddress->setText( s->getPhysAddress().c_str() );
    m_dialog->commentKeywords->loadFWObject(o);

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->pAddress->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->pAddress);

    init=false;
}

void PhysicalAddressDialog::validate(bool *res)
{
    *res=true;

    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}



void PhysicalAddressDialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    physAddress *s = dynamic_cast<physAddress*>(new_state);
    assert(s!=nullptr);

    string oldname=obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    s->setPhysAddress( m_dialog->pAddress->text().toLatin1().constData() );

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
    
}

