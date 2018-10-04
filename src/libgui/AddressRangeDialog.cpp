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
#include "FWCmdChange.h"
#include "FWBSettings.h"
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
#include <QApplication>
#include <QUndoStack>

#include <memory>
#include <sstream>

using namespace std;
using namespace libfwbuilder;

AddressRangeDialog::AddressRangeDialog(QWidget *parent):
        BaseObjectDialog(parent)
{
    m_dialog = new Ui::AddressRangeDialog_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

AddressRangeDialog::~AddressRangeDialog()
{
    delete m_dialog;
}

void AddressRangeDialog::loadFWObject(FWObject *o)
{
    obj=o;
    AddressRange *s = dynamic_cast<AddressRange*>(obj);
    assert(s!=nullptr);

    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->rangeStart->setText( s->getRangeStart().toString().c_str() );
    m_dialog->rangeEnd->setText( s->getRangeEnd().toString().c_str() );
    m_dialog->commentKeywords->loadFWObject(o);
    //apply->setEnabled( false );

    m_dialog->obj_name->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->obj_name);

    m_dialog->rangeStart->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->rangeStart);

    m_dialog->rangeEnd->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->rangeEnd);

    init=false;
}

void AddressRangeDialog::validate(bool *res)
{
    *res=true;

    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }

#ifndef NDEBUG
    AddressRange *s = dynamic_cast<AddressRange*>(obj);
    assert(s!=nullptr);
#endif
    try
    {
        InetAddr range_start(AF_UNSPEC, m_dialog->rangeStart->text().toLatin1().constData());
        InetAddr range_end(AF_UNSPEC, m_dialog->rangeEnd->text().toLatin1().constData());

        if (range_start.addressFamily() != range_end.addressFamily()) {

            std::ostringstream s;
            s << "AddressRange start and end address must be of same IP address family: ";
            s << "start_address: " << m_dialog->rangeStart->text().toStdString() << ", ";
            s << "end_address: " << m_dialog->rangeEnd->text().toStdString();

            throw(FWException(s.str()));
        }
    } catch (FWException &ex)
    {
        *res = false;
        // show warning dialog only if app has focus
        if (QApplication::focusWidget() != nullptr)
        {
            blockSignals(true);
            QMessageBox::critical(
                this, "Firewall Builder",
                QString::fromUtf8(ex.toString().c_str()),
                tr("&Continue"), nullptr, nullptr,
                0 );
            blockSignals(false);
        }
    }
}


void AddressRangeDialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd(new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    AddressRange *s = dynamic_cast<AddressRange*>(new_state);
    assert(s!=nullptr);

    string oldname = obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    try
    {
        InetAddr addr_start(AF_UNSPEC, m_dialog->rangeStart->text().toStdString());
        InetAddr addr_end(AF_UNSPEC, m_dialog->rangeEnd->text().toStdString());
        if (addr_end < addr_start)
        {
            addr_end = addr_start;
            m_dialog->rangeEnd->setText(addr_end.toString().c_str());
        }

        s->setRangeStart(addr_start);
        s->setRangeEnd(addr_end);
    } catch (FWException &ex)
    {

    }

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
    
}




