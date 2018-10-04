/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  alek@codeminders.com

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
#include "UserDialog.h"
#include "FWCmdChange.h"

#include "ProjectPanel.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/UserService.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWException.h"

#include <memory>

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <QUndoStack>


using namespace std;
using namespace libfwbuilder;


UserDialog::UserDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::UserDialog_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

UserDialog::~UserDialog() { delete m_dialog; }

void UserDialog::loadFWObject(FWObject *o)
{
    obj=o;
    UserService *s = dynamic_cast<UserService*>(obj);
    assert(s!=nullptr);
    
    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->userid->setText( s->getUserId().c_str() );

    m_dialog->commentKeywords->loadFWObject(o);

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->userid->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->userid);

    init=false;
}

void UserDialog::validate(bool *res)
{
    *res=true;

    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }

#ifndef NDEBUG
    UserService *s = dynamic_cast<UserService*>(obj);
    assert(s!=nullptr);
#endif
}



void UserDialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    UserService *s = dynamic_cast<UserService*>(new_state);
    assert(s!=nullptr);

    string oldname = obj->getName();
    s->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    s->setUserId( string(m_dialog->userid->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
}

