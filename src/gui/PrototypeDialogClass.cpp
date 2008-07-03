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


#include "fwbuilder_ph.h"

#include "config.h"
#include "global.h"
#include "utils.h"

#include "PrototypeDialog.h"
#include "ObjectManipulator.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Prototype.h"   //  should be an include file for the object type

#include <qlineedit.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>

#include <iostream>

using namespace libfwbuilder;
using namespace std;

void PrototypeDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Prototype *s = dynamic_cast<Prototype*>(obj);
    assert(s!=NULL);

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

void PrototypeDialog::isChanged(bool *res)
{
    *res=(!init && apply->isEnabled());
}

void PrototypeDialog::libChanged()
{
    changed();
}

void PrototypeDialog::applyChanges()
{
    if (!isTreeReadWrite(this,obj)) return;

    string oldname=obj->getName();
    obj->setName( string(obj_name->text().utf8()) );
    obj->setComment( string(comment->text().utf8()) );


    mw->updateObjName(obj,QString::fromUtf8(oldname.c_str()));

    init=true;

/* move to another lib if we have to */
    if (! m_project->isSystem(obj) && libs->currentText() != QString(obj->getLibrary()->getName().c_str()))
        mw->moveObject(libs->currentText(), obj);

    init=false;

    apply->setEnabled( false );
}

