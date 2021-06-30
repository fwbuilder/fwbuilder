/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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

#include "TagServiceDialog.h"
#include "ProjectPanel.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/TagService.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWException.h"

#include <memory>

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qcursor.h>
#include <QUndoStack>

#include <iostream>

using namespace std;
using namespace libfwbuilder;

TagServiceDialog::~TagServiceDialog()
{
    delete m_dialog;
}

TagServiceDialog::TagServiceDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::TagServiceDialog_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

void TagServiceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    TagService *s = dynamic_cast<TagService*>(obj);
    assert(s!=nullptr);


    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->commentKeywords->loadFWObject(o);

    m_dialog->tagcode->setText( s->getCode().c_str() );

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->tagcode->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->tagcode);

    init=false;
}

void TagServiceDialog::validate(bool *res)
{
    *res=true;
#ifndef NDEBUG
    TagService *s = dynamic_cast<TagService*>(obj);
    assert(s!=nullptr);
#endif
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}



void TagServiceDialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    TagService *s = dynamic_cast<TagService*>(new_state);
    assert(s!=nullptr);

    string oldname = obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    s->setCode( m_dialog->tagcode->text().toLatin1().constData() );

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
    
}

