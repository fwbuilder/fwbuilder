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

#include "FWBTree.h"
#include "IPServiceDialog.h"
#include "ProjectPanel.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ServiceGroup.h"

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>

#include <iostream>
#include "FWBSettings.h"

#include "FWWindow.h"
using namespace libfwbuilder;
using namespace std;

IPServiceDialog::IPServiceDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::IPServiceDialog_q;
    m_dialog->setupUi(this);
    obj=NULL;
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

void IPServiceDialog::getHelpName(QString *str)
{
    *str = "IPServiceDialog";
}

void IPServiceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    IPService *s = dynamic_cast<IPService*>(obj);
    assert(s!=NULL);

    init=true;

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

    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

    //apply->setEnabled( false );

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

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);

    anyOptionsStateChanged();

    init=false;
}

void IPServiceDialog::changed()
{
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
    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }
}

void IPServiceDialog::applyChanges()
{
    string oldname=obj->getName();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );

    obj->setInt("protocol_num", m_dialog->protocolNum->value() );
    obj->setBool("any_opt", m_dialog->any_opt->isChecked() );
    obj->setBool("lsrr", m_dialog->lsrr->isChecked() );
    obj->setBool("ssrr", m_dialog->ssrr->isChecked() );
    obj->setBool("rr", m_dialog->rr->isChecked() );
    obj->setBool("ts", m_dialog->timestamp->isChecked() );
    obj->setBool("fragm", m_dialog->all_fragments->isChecked() );
    obj->setBool("short_fragm", m_dialog->short_fragments->isChecked() );

    // router-alert IP option has only one defined value - "0". All other
    // values are reserved atm. RFC 2113
    obj->setBool("rtralt", m_dialog->router_alert->isChecked() );
    if (m_dialog->router_alert->isChecked()) obj->setInt("rtralt_value", 0);

    IPService *ip = IPService::cast(obj);
    if (m_dialog->use_dscp->isChecked())
    {
        ip->setDSCPCode(m_dialog->code->text().toUtf8().constData());
        ip->setTOSCode("");
    } else
    {
        ip->setTOSCode(m_dialog->code->text().toUtf8().constData());
        ip->setDSCPCode("");
    }

    m_project->updateObjName(obj,QString::fromUtf8(oldname.c_str()));

    BaseObjectDialog::applyChanges();
}

void IPServiceDialog::discardChanges()
{
    loadFWObject(obj);
}

