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

#include "global.h"
#include "utils.h"

#include "DNSNameDialog.h"
#include "ProjectPanel.h"
#include "FWBSettings.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/DNSName.h"
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

#include "FWWindow.h"
using namespace std;
using namespace libfwbuilder;

DNSNameDialog::DNSNameDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::DNSNameDialog_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

DNSNameDialog::~DNSNameDialog()
{
    delete m_dialog;
}

void DNSNameDialog::loadFWObject(FWObject *o)
{
    obj=o;
    DNSName *s = dynamic_cast<DNSName*>(obj);
    assert(s!=nullptr);


    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->commentKeywords->loadFWObject(o);

    m_dialog->dnsrec->setText( s->getSourceName().c_str() );
    m_dialog->r_compiletime->setChecked(s->isCompileTime() );
    m_dialog->r_runtime->setChecked(s->isRunTime() );

    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->dnsrec->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->dnsrec);

    if (st->getBool("Objects/DNSName/useNameForDNSRecord"))
        m_dialog->dnsrec->setEnabled(false);

    init=false;
}

void DNSNameDialog::validate(bool *res)
{
    *res=true;
#ifndef NDEBUG
    DNSName *s = dynamic_cast<DNSName*>(obj);
    assert(s!=nullptr);
#endif
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}



void DNSNameDialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    DNSName *s = dynamic_cast<DNSName*>(new_state);
    assert(s!=nullptr);

    string oldname = obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    s->setRunTime(m_dialog->r_runtime->isChecked() );

    if (st->getBool("Objects/DNSName/useNameForDNSRecord") &&
        m_dialog->obj_name->text() != m_dialog->dnsrec->text())
        m_dialog->dnsrec->setText(m_dialog->obj_name->text().trimmed());

    s->setSourceName( m_dialog->dnsrec->text().trimmed().toLatin1().constData() );

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
    
}

