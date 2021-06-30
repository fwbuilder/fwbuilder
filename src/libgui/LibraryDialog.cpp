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
#include "LibraryDialog.h"
#include "FWBSettings.h"
#include "FWWindow.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"

#include <memory>

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcolordialog.h>
#include <qpixmapcache.h>
#include <qpushbutton.h>

#include <QPainter>
#include <QtDebug>
#include <QUndoStack>

#include <iostream>


using namespace std;
using namespace libfwbuilder;


LibraryDialog::LibraryDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::LibraryDialog_q;
    m_dialog->setupUi(this);

    obj=nullptr;
    //layout()->setSizeConstraint(QLayout::SetFixedSize);

    Qt::WindowFlags flags = windowFlags();
    flags &= ~Qt::WindowMaximizeButtonHint;
    flags &= ~Qt::WindowMinimizeButtonHint;
    setWindowFlags(flags);

    connectSignalsOfAllWidgetsToSlotChange();
}

LibraryDialog::~LibraryDialog()
{
    delete m_dialog;
}

void LibraryDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Library *s = dynamic_cast<Library*>(obj);
    assert(s!=nullptr);

    init=true;
    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->commentKeywords->loadFWObject(o);

    m_dialog->obj_name->setEnabled(
        obj->getId() != FWObjectDatabase::STANDARD_LIB_ID);
//    apply->setEnabled( obj->getId() != "syslib000" );
//    comment->setEnabled(  !m_project->isSystem(obj) );

    color=obj->getStr("color").c_str();
    if (color=="") color="#FFFFFF";   // white is the default

    fillColor();

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());

    m_dialog->colorButton->setEnabled(!o->isReadOnly());

    init=false;
}

void LibraryDialog::changeIds(FWObject *root)
{
    if (FWBTree().isStandardId(root))
        root->setId(FWObjectDatabase::generateUniqueId());

    for (FWObject::iterator i=root->begin(); i!=root->end(); i++)
        changeIds( *i );
}

void LibraryDialog::applyChanges()
{
    if (FWBTree().isSystem(obj)) return;

    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    QString oldcolor = new_state->getStr("color").c_str();

    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);
    new_state->setStr("color", color.toLatin1().constData());

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
}

void LibraryDialog::validate(bool *res)
{
    *res=true;
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}


void LibraryDialog::changeColor()
{
    if (!isTreeReadWrite(this,obj)) return;

    QColor clr = QColorDialog::getColor(
        QColor(color), this );
    if (!clr.isValid()) return;

    color = clr.name();
    fillColor();

    emit changed();
}

void LibraryDialog::fillColor()
{
    QPixmap pm(40,14);
    pm.fill( QColor(color) );
    QPainter p( &pm );
    p.drawRect( pm.rect() );
    m_dialog->colorButton->setIcon(QIcon(pm));
}

