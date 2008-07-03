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
#include "ProjectPanel.h"

#include "FWBTree.h"
#include "LibraryDialog.h"

#include "fwbuilder/Library.h"

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcolordialog.h>
#include <qpixmapcache.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include "FWBSettings.h"

#include "FWWindow.h"
using namespace std;
using namespace libfwbuilder;

LibraryDialog::LibraryDialog(ProjectPanel *project, QWidget *parent) : QWidget(parent), m_project(project)
{
    m_dialog = new Ui::LibraryDialog_q;
    m_dialog->setupUi(this);

    obj=NULL;
    //layout()->setSizeConstraint(QLayout::SetFixedSize);

    Qt::WindowFlags flags = windowFlags();
    flags &= ~Qt::WindowMaximizeButtonHint;
    flags &= ~Qt::WindowMinimizeButtonHint;
    setWindowFlags(flags);
}

LibraryDialog::~LibraryDialog()
{
    delete m_dialog;
}

void LibraryDialog::loadFWObject(FWObject *o)
{
    obj=o;
    Library *s = dynamic_cast<Library*>(obj);
    assert(s!=NULL);

    init=true;
    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

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

    m_dialog->comment->setReadOnly(o->isReadOnly());

    init=false;
}

void LibraryDialog::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void LibraryDialog::changeIds(FWObject *root)
{
    if (m_project->isStandardId(root))
        root->setId(FWObjectDatabase::generateUniqueId());

    for (FWObject::iterator i=root->begin(); i!=root->end(); i++)
        changeIds( *i );
}

void LibraryDialog::applyChanges()
{
    string oldname=obj->getName();
    QString oldcolor=obj->getStr("color").c_str();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );
    obj->setStr("color", color.toLatin1().constData());

/* just for fool-proof'ness, do not allow changing name of the 'Standard' lib */
    if (oldname!=obj->getName() &&
        obj->getId()==FWObjectDatabase::STANDARD_LIB_ID)
    {
        obj->setName( oldname );
    }

    mw->updateObjName(obj,QString::fromUtf8(oldname.c_str()));
    mw->updateLibName(obj);
    if (color!=oldcolor) mw->updateLibColor(obj);

    //apply->setEnabled( false );
}

void LibraryDialog::discardChanges()
{
    loadFWObject(obj);
}

void LibraryDialog::validate(bool *res)
{
    *res=true;
    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}

void LibraryDialog::isChanged(bool*)
{
    //*res=(!init && apply->isEnabled());
}

void LibraryDialog::changeColor()
{
    if (!isTreeReadWrite(this,obj)) return;

    QColor clr = QColorDialog::getColor(
        QColor(color), this );
    if (!clr.isValid()) return;

    color = clr.name();
    fillColor();

    changed();
}

void LibraryDialog::fillColor()
{
    QPixmap pm(40,14);
    pm.fill( QColor(color) );
    QPainter p( &pm );
    p.drawRect( pm.rect() );
    m_dialog->colorButton->setIcon(QIcon(pm));
}


/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void LibraryDialog::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}

