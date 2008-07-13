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
#include "ProjectPanel.h"

#include "FWBTree.h"
#include "AddressRangeDialog.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/AddressRange.h"
#include "fwbuilder/FWException.h"

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include "FWBSettings.h"

#include "FWWindow.h"
using namespace std;
using namespace libfwbuilder;

AddressRangeDialog::AddressRangeDialog(ProjectPanel *project, QWidget *parent):
        QWidget(parent), m_project(project)
{
    m_dialog = new Ui::AddressRangeDialog_q;
    m_dialog->setupUi(this);
    obj=NULL;

}

AddressRangeDialog::~AddressRangeDialog()
{
    delete m_dialog;
}

void AddressRangeDialog::loadFWObject(FWObject *o)
{
    obj=o;
    AddressRange *s = dynamic_cast<AddressRange*>(obj);
    assert(s!=NULL);

    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->rangeStart->setText( s->getRangeStart().toString().c_str() );
    m_dialog->rangeEnd->setText( s->getRangeEnd().toString().c_str() );
    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );
    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->rangeStart->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->rangeStart);

    m_dialog->rangeEnd->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->rangeEnd);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);


    init=false;
}

void AddressRangeDialog::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void AddressRangeDialog::validate(bool *res)
{
    *res=true;

    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }

    AddressRange *s = dynamic_cast<AddressRange*>(obj);
    assert(s!=NULL);
    try
    {
        InetAddr(m_dialog->rangeStart->text().toLatin1().constData());
    } catch (FWException &ex)
    {
        *res=false;
        QMessageBox::critical(this, "Firewall Builder",
                              tr("Illegal IP address '%1'").arg(m_dialog->rangeStart->text()),
                              tr("&Continue"), 0, 0,
                              0 );
    }
    try
    {
        InetAddr(m_dialog->rangeEnd->text().toLatin1().constData());
    } catch (FWException &ex)
    {
        *res=false;
        QMessageBox::critical(this, "Firewall Builder",
                              tr("Illegal IP address '%1'").arg(m_dialog->rangeEnd->text()),
                              tr("&Continue"), 0, 0,
                              0 );
    }
}

void AddressRangeDialog::isChanged(bool *)
{
    //*res=(!init && apply->isEnabled());
}

void AddressRangeDialog::libChanged()
{
    changed();
}


void AddressRangeDialog::applyChanges()
{
    AddressRange *s = dynamic_cast<AddressRange*>(obj);
    assert(s!=NULL);

    string oldname=obj->getName();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );
    try
    {
        s->setRangeStart( InetAddr(m_dialog->rangeStart->text().toLatin1().constData()) );
        s->setRangeEnd( InetAddr(m_dialog->rangeEnd->text().toLatin1().constData()) );
    } catch (FWException &ex)
    {

    }
    mw->updateObjName(obj,QString::fromUtf8(oldname.c_str()));

    //apply->setEnabled( false );
    mw->updateLastModifiedTimestampForAllFirewalls(obj);
}

void AddressRangeDialog::discardChanges()
{
    loadFWObject(obj);
}



/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void AddressRangeDialog::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}

