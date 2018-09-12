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
#include "IPServiceDialog.h"
#include "ProjectPanel.h"
#include "FWCmdChange.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ServiceGroup.h"

#include <memory>

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <QUndoStack>

#include <iostream>

#include "FWWindow.h"
using namespace libfwbuilder;
using namespace std;

IPServiceDialog::IPServiceDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::IPServiceDialog_q;
    m_dialog->setupUi(this);
    obj=nullptr;

    connectSignalsOfAllWidgetsToSlotChange();
}

IPServiceDialog::~IPServiceDialog()
{
    delete m_dialog;
}

void IPServiceDialog::setCodeLabel()
{
    if (m_dialog->use_dscp->isChecked())
    {
        m_dialog->code_label->setText(
            tr("DSCP code or class:"));
    }
    else
    {
        m_dialog->code_label->setText(
            tr("TOS code (numeric):"));
    }
}

void IPServiceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    IPService *s = dynamic_cast<IPService*>(obj);
    assert(s!=nullptr);

    init = true;

    //apply->setEnabled( false );

    // See #893 No need to show object attributes if the object is "Any"
    if (obj->getId() == FWObjectDatabase::ANY_SERVICE_ID)
    {
        m_dialog->object_attributes_1->hide();
        m_dialog->object_attributes_2->hide();
        m_dialog->object_attributes_3->hide();

        m_dialog->commentKeywords->setReadOnlyComment(
            QObject::tr(
                "When used in the Service field of a rule, "
                "the Any object will match all "
                "IP, ICMP, TCP or UDP services. To update your rule to "
                "match only specific "
                "service, drag-and-drop an object from "
                "the Object tree into the field in the rule."));

        //m_dialog->comment->setReadOnly(true);
        //setDisabledPalette(m_dialog->comment);
    } else
    {
        m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
        m_dialog->protocolNum->setValue( s->getProtocolNumber() );
        m_dialog->any_opt->setChecked( s->getBool("any_opt") );
        m_dialog->lsrr->setChecked( s->getBool("lsrr") );
        m_dialog->ssrr->setChecked( s->getBool("ssrr") );
        m_dialog->rr->setChecked( s->getBool("rr") );
        m_dialog->timestamp->setChecked( s->getBool("ts") );
        m_dialog->all_fragments->setChecked( s->getBool("fragm") );
        m_dialog->short_fragments->setChecked( s->getBool("short_fragm") );
        m_dialog->router_alert->setChecked( s->getBool("rtralt") );

        string tos = s->getTOSCode();
        string dscp = s->getDSCPCode();

        if (!dscp.empty())
        {
            m_dialog->use_dscp->setChecked(true);  
            m_dialog->code->setText(dscp.c_str());
        }
        else
        {
            m_dialog->use_tos->setChecked(true);    
            m_dialog->code->setText(tos.c_str());
        }
        setCodeLabel();

        anyOptionsStateChanged();

        m_dialog->commentKeywords->loadFWObject(o);

        m_dialog->object_attributes_1->show();
        m_dialog->object_attributes_2->show();
        m_dialog->object_attributes_3->show();

        m_dialog->obj_name->setEnabled(!o->isReadOnly());
        setDisabledPalette(m_dialog->obj_name);

        m_dialog->protocolNum->setEnabled(!o->isReadOnly());
        setDisabledPalette(m_dialog->protocolNum);

        m_dialog->any_opt->setEnabled(!o->isReadOnly());
        setDisabledPalette(m_dialog->any_opt);

        m_dialog->lsrr->setEnabled(!o->isReadOnly());
        setDisabledPalette(m_dialog->lsrr);

        m_dialog->ssrr->setEnabled(!o->isReadOnly());
        setDisabledPalette(m_dialog->ssrr);

        m_dialog->rr->setEnabled(!o->isReadOnly());
        setDisabledPalette(m_dialog->rr);

        m_dialog->timestamp->setEnabled(!o->isReadOnly());
        setDisabledPalette(m_dialog->timestamp);

        m_dialog->router_alert->setEnabled(!o->isReadOnly());
        setDisabledPalette(m_dialog->router_alert);

        m_dialog->all_fragments->setEnabled(!o->isReadOnly());
        setDisabledPalette(m_dialog->all_fragments);

        m_dialog->short_fragments->setEnabled(!o->isReadOnly());
        setDisabledPalette(m_dialog->short_fragments);

        m_dialog->use_tos->setEnabled(!o->isReadOnly());
        m_dialog->use_dscp->setEnabled(!o->isReadOnly());
        m_dialog->code->setEnabled(!o->isReadOnly());
        setDisabledPalette(m_dialog->code);

        //anyOptionsStateChanged();
    }

    init = false;
}

void IPServiceDialog::changed()
{
    if (fwbdebug) qDebug() << "IPServiceDialog::changed()";

    setCodeLabel();
    BaseObjectDialog::changed();
}

void IPServiceDialog::anyOptionsStateChanged()
{
    bool any_opt_state = m_dialog->any_opt->isChecked();

    if (any_opt_state)
    {
        m_dialog->lsrr->setChecked(false);
        m_dialog->ssrr->setChecked(false);
        m_dialog->rr->setChecked(false);
        m_dialog->timestamp->setChecked(false);
        m_dialog->router_alert->setChecked(false);
    }

    m_dialog->lsrr->setEnabled(!any_opt_state);
    m_dialog->ssrr->setEnabled(!any_opt_state);
    m_dialog->rr->setEnabled(!any_opt_state);
    m_dialog->timestamp->setEnabled(!any_opt_state);
    m_dialog->router_alert->setEnabled(!any_opt_state);

    changed();
}

void IPServiceDialog::validate(bool *res)
{
    *res=true;
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}

void IPServiceDialog::applyChanges()
{
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(m_project, obj));
    FWObject* new_state = cmd->getNewState();

    string oldname=obj->getName();
    new_state->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    m_dialog->commentKeywords->applyChanges(new_state);

    new_state->setInt("protocol_num", m_dialog->protocolNum->value() );
    new_state->setBool("any_opt", m_dialog->any_opt->isChecked() );
    new_state->setBool("lsrr", m_dialog->lsrr->isChecked() );
    new_state->setBool("ssrr", m_dialog->ssrr->isChecked() );
    new_state->setBool("rr", m_dialog->rr->isChecked() );
    new_state->setBool("ts", m_dialog->timestamp->isChecked() );
    new_state->setBool("fragm", m_dialog->all_fragments->isChecked() );
    new_state->setBool("short_fragm", m_dialog->short_fragments->isChecked() );

    // router-alert IP option has only one defined value - "0". All other
    // values are reserved atm. RFC 2113
    new_state->setBool("rtralt", m_dialog->router_alert->isChecked() );
    if (m_dialog->router_alert->isChecked()) new_state->setInt("rtralt_value", 0);

    IPService *ip = IPService::cast(new_state);
    if (m_dialog->use_dscp->isChecked())
    {
        ip->setDSCPCode(m_dialog->code->text().toUtf8().constData());
        ip->setTOSCode("");
    } else
    {
        ip->setTOSCode(m_dialog->code->text().toUtf8().constData());
        ip->setDSCPCode("");
    }

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }
}

