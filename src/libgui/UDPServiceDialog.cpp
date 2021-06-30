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

#include "FWBTree.h"
#include "UDPServiceDialog.h"
#include "ProjectPanel.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/UDPService.h"

#include <memory>

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <QUndoStack>


using namespace std;
using namespace libfwbuilder;

UDPServiceDialog::UDPServiceDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::UDPServiceDialog_q;
    m_dialog->setupUi(this);

    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

UDPServiceDialog::~UDPServiceDialog()
{
    delete m_dialog;
}

void UDPServiceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    UDPService *s = dynamic_cast<UDPService*>(obj);
    assert(s!=nullptr);

    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->ss->setValue( TCPUDPService::cast(s)->getSrcRangeStart() );
    m_dialog->se->setValue( TCPUDPService::cast(s)->getSrcRangeEnd() );
    m_dialog->ds->setValue( TCPUDPService::cast(s)->getDstRangeStart() );
    m_dialog->de->setValue( TCPUDPService::cast(s)->getDstRangeEnd() );

    m_dialog->commentKeywords->loadFWObject(o);

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->ss->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->ss);

    m_dialog->se->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->se);

    m_dialog->ds->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->ds);

    m_dialog->de->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->de);

    init=false;
}

void UDPServiceDialog::validate(bool *res)
{
    *res = true;
    if (!validateName(this,obj,m_dialog->obj_name->text()))
    {
        *res = false;
        return;
    }
}

void UDPServiceDialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    string oldname = obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    // check port ranges (bug #1695481, range start must be <= range end)
    int sps = m_dialog->ss->value();
    int spe = m_dialog->se->value();
    int dps = m_dialog->ds->value();
    int dpe = m_dialog->de->value();

    if (sps > spe) m_dialog->se->setValue( m_dialog->ss->value() );
    if (dps > dpe) m_dialog->de->setValue( m_dialog->ds->value() );

    spe = m_dialog->se->value();
    dpe = m_dialog->de->value();

    TCPUDPService::cast(new_state)->setSrcRangeStart(m_dialog->ss->value());
    TCPUDPService::cast(new_state)->setSrcRangeEnd(m_dialog->se->value());
    TCPUDPService::cast(new_state)->setDstRangeStart(m_dialog->ds->value());
    TCPUDPService::cast(new_state)->setDstRangeEnd(m_dialog->de->value());

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
    
}

