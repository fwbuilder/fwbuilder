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

#include "ProjectPanel.h"
#include "FWBTree.h"
#include "ICMPServiceDialog.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/ICMPService.h"
#include "fwbuilder/ICMP6Service.h"

#include <memory>

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <QUndoStack>

#include <iostream>

#include "FWWindow.h"
using namespace std;
using namespace libfwbuilder;

ICMPServiceDialog::ICMPServiceDialog(QWidget *parent) :
        BaseObjectDialog(parent)
{
    m_dialog = new Ui::ICMPServiceDialog_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

ICMPServiceDialog::~ICMPServiceDialog()
{
    delete m_dialog;
}

void ICMPServiceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    ICMPService *s = dynamic_cast<ICMPService*>(obj);
    assert(s!=nullptr);

    // if (ICMP6Service::isA(o))
    // {
    //     m_dialog->editorTitle->setText("ICMP6 Service");
    //     m_dialog->editorLabel->setPixmap(
    //         QPixmap(QString::fromUtf8(":/Icons/ICMP6Service/icon")));
    // } else
    // {
    //     m_dialog->editorTitle->setText("ICMP Service");
    //     m_dialog->editorLabel->setPixmap(
    //         QPixmap(QString::fromUtf8(":/Icons/ICMPService/icon")));
    // }

    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->icmpType->setValue( s->getInt("type") );
    m_dialog->icmpCode->setValue( s->getInt("code") );
    m_dialog->commentKeywords->loadFWObject(o);

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->icmpType->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->icmpType);

    m_dialog->icmpCode->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->icmpCode);

    init=false;
}

void ICMPServiceDialog::validate(bool *res)
{
    *res=true;
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}



void ICMPServiceDialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    string oldname = obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    new_state->setInt("type", m_dialog->icmpType->value() );
    new_state->setInt("code", m_dialog->icmpCode->value() );

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
}

