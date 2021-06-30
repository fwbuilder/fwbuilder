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

#include "PrototypeDialog.h"
#include "ObjectManipulator.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Prototype.h"   //  should be an include file for the object type

#include <memory>

#include <qlineedit.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <QUndoStack>

#include <iostream>

using namespace libfwbuilder;
using namespace std;

void PrototypeDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Prototype *s = dynamic_cast<Prototype*>(obj);
    assert(s!=nullptr);

    init=true;

    fillLibraries(libs,obj);

    obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    comment->setText( QString::fromUtf8(s->getComment().c_str()) );



    apply->setEnabled( false );
    init=false;
}

void PrototypeDialog::changed()
{
    apply->setEnabled( true );
}

void PrototypeDialog::validate(bool *res)
{
    *res=true;
}

void PrototypeDialog::applyChanges()
{

    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    new_state->setName( string(obj_name->text().utf8()) );
    new_state->setComment( string(comment->text().utf8()) );

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
    
}

