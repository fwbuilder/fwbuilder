/*

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

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

#include "../../config.h"
#include "global.h"
#include "utils.h"

#include "DNSNameDialog.h"
#include "ProjectPanel.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/DNSName.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWException.h"

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

#include "FWWindow.h"
using namespace std;
using namespace libfwbuilder;

DNSNameDialog::DNSNameDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::DNSNameDialog_q;
    m_dialog->setupUi(this);
    obj=NULL;
}

DNSNameDialog::~DNSNameDialog()
{
    delete m_dialog;
}

void DNSNameDialog::getHelpName(QString *str)
{
    *str = "DNSNameDialog";
}

void DNSNameDialog::loadFWObject(FWObject *o)
{
    obj=o;
    DNSName *s = dynamic_cast<DNSName*>(obj);
    assert(s!=NULL);


    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

    m_dialog->dnsrec->setText( s->getSourceName().c_str() );
    m_dialog->r_compiletime->setChecked(s->isCompileTime() );
    m_dialog->r_runtime->setChecked(s->isRunTime() );

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->dnsrec->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->dnsrec);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);



    init=false;
}

void DNSNameDialog::validate(bool *res)
{
    *res=true;
    DNSName *s = dynamic_cast<DNSName*>(obj);
    assert(s!=NULL);

    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}



void DNSNameDialog::applyChanges()
{
    FWCmdChange* cmd = new FWCmdChange(m_project, obj);
    FWObject* new_state = cmd->getNewState();

    DNSName *s = dynamic_cast<DNSName*>(new_state);
    assert(s!=NULL);

    string oldname = obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    new_state->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );

    s->setSourceName( m_dialog->dnsrec->text().toLatin1().constData() );
    s->setRunTime(m_dialog->r_runtime->isChecked() );

    if (!cmd->getOldState()->cmp(new_state, true)) m_project->undoStack->push(cmd);
    
}

