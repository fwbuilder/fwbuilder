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

#include "FWBTree.h"
#include "TCPServiceDialog.h"

#include "ProjectPanel.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/TCPService.h"

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtextedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include "FWBSettings.h"

#include "FWWindow.h"
using namespace std;
using namespace libfwbuilder;

TCPServiceDialog::TCPServiceDialog(ProjectPanel *project, QWidget *parent) : QWidget(parent), m_project(project)
{
    m_dialog = new Ui::TCPServiceDialog_q;
    m_dialog->setupUi(this);

    obj=NULL;
}

TCPServiceDialog::~TCPServiceDialog()
{
    delete m_dialog;
}

void TCPServiceDialog::loadFWObject(FWObject *o)
{
    obj=o;
    TCPService *s = dynamic_cast<TCPService*>(obj);
    assert(s!=NULL);

    init=true;

    m_dialog->obj_name->setText( QString::fromUtf8(s->getName().c_str()) );
    m_dialog->ss->setValue( TCPUDPService::cast(s)->getSrcRangeStart() );
    m_dialog->se->setValue( TCPUDPService::cast(s)->getSrcRangeEnd() );
    m_dialog->ds->setValue( TCPUDPService::cast(s)->getDstRangeStart() );
    m_dialog->de->setValue( TCPUDPService::cast(s)->getDstRangeEnd() );

    m_dialog->urg_m->setChecked( s->getBool("urg_flag_mask") );
    m_dialog->ack_m->setChecked( s->getBool("ack_flag_mask") );
    m_dialog->psh_m->setChecked( s->getBool("psh_flag_mask") );
    m_dialog->rst_m->setChecked( s->getBool("rst_flag_mask") );
    m_dialog->syn_m->setChecked( s->getBool("syn_flag_mask") );
    m_dialog->fin_m->setChecked( s->getBool("fin_flag_mask") );

    m_dialog->urg_s->setChecked( s->getBool("urg_flag") );
    m_dialog->ack_s->setChecked( s->getBool("ack_flag") );
    m_dialog->psh_s->setChecked( s->getBool("psh_flag") );
    m_dialog->rst_s->setChecked( s->getBool("rst_flag") );
    m_dialog->syn_s->setChecked( s->getBool("syn_flag") );
    m_dialog->fin_s->setChecked( s->getBool("fin_flag") );

    m_dialog->established->setChecked( s->getBool("established") );

    m_dialog->comment->setText( QString::fromUtf8(s->getComment().c_str()) );

    toggleEstablished();

    //apply->setEnabled( false );

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

    m_dialog->urg_m->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->urg_m);

    m_dialog->ack_m->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->ack_m);

    m_dialog->psh_m->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->psh_m);

    m_dialog->rst_m->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->rst_m);

    m_dialog->syn_m->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->syn_m);

    m_dialog->fin_m->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->fin_m);

    m_dialog->urg_s->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->urg_s);

    m_dialog->ack_s->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->ack_s);

    m_dialog->psh_s->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->psh_s);

    m_dialog->rst_s->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->rst_s);

    m_dialog->syn_s->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->syn_s);

    m_dialog->fin_s->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->fin_s);

    m_dialog->established->setEnabled(!o->isReadOnly());
    setDisabledPalette(m_dialog->established);

    m_dialog->comment->setReadOnly(o->isReadOnly());
    setDisabledPalette(m_dialog->comment);


    init=false;
}

void TCPServiceDialog::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void TCPServiceDialog::validate(bool *res)
{
    if (fwbdebug) qDebug("TCPServiceDialog::validate");

    *res=true;

    if (!isTreeReadWrite(this,obj)) { *res=false; return; }
    if (!validateName(this,obj,m_dialog->obj_name->text())) { *res=false; return; }

    // check port ranges (bug #1695481, range start must be <= range end)
    int sps = m_dialog->ss->value();
    int spe = m_dialog->se->value();
    int dps = m_dialog->ds->value();
    int dpe = m_dialog->de->value();

    if (sps > spe)
    {
        QMessageBox::warning(this, "Firewall Builder",
          QObject::tr("Invalid range defined for the source port."),
          QObject::tr("&Continue editing"), NULL, NULL, 0, 2 );
        *res = false;
        return;
    }
    if (dps > dpe)
    {
        QMessageBox::warning(this, "Firewall Builder",
          QObject::tr("Invalid range defined for the destination port."),
          QObject::tr("&Continue editing"), NULL, NULL, 0, 2 );
        *res = false;
        return;
    }
}

void TCPServiceDialog::isChanged(bool*)
{
    //*res=(!init && apply->isEnabled());
}

void TCPServiceDialog::libChanged()
{
    changed();
}

void TCPServiceDialog::applyChanges()
{
    string oldname=obj->getName();
    obj->setName( string(m_dialog->obj_name->text().toUtf8().constData()) );
    obj->setComment( string(m_dialog->comment->toPlainText().toUtf8().constData()) );

    if (m_dialog->ss->value()!=0 && m_dialog->se->value()==0) m_dialog->se->setValue( m_dialog->ss->value() );
    if (m_dialog->ds->value()!=0 && m_dialog->de->value()==0) m_dialog->de->setValue( m_dialog->ds->value() );


    TCPUDPService::cast(obj)->setSrcRangeStart(m_dialog->ss->value());
    TCPUDPService::cast(obj)->setSrcRangeEnd(m_dialog->se->value());
    TCPUDPService::cast(obj)->setDstRangeStart(m_dialog->ds->value());
    TCPUDPService::cast(obj)->setDstRangeEnd(m_dialog->de->value());

    obj->setBool("urg_flag_mask", m_dialog->urg_m->isChecked() );
    obj->setBool("ack_flag_mask", m_dialog->ack_m->isChecked() );
    obj->setBool("psh_flag_mask", m_dialog->psh_m->isChecked() );
    obj->setBool("rst_flag_mask", m_dialog->rst_m->isChecked() );
    obj->setBool("syn_flag_mask", m_dialog->syn_m->isChecked() );
    obj->setBool("fin_flag_mask", m_dialog->fin_m->isChecked() );

    obj->setBool("urg_flag",      m_dialog->urg_s->isChecked() );
    obj->setBool("ack_flag",      m_dialog->ack_s->isChecked() );
    obj->setBool("psh_flag",      m_dialog->psh_s->isChecked() );
    obj->setBool("rst_flag",      m_dialog->rst_s->isChecked() );
    obj->setBool("syn_flag",      m_dialog->syn_s->isChecked() );
    obj->setBool("fin_flag",      m_dialog->fin_s->isChecked() );

    obj->setBool("established",  m_dialog->established->isChecked());

    mw->updateObjName(obj,QString::fromUtf8(oldname.c_str()));

    //apply->setEnabled( false );
    mw->updateLastModifiedTimestampForAllFirewalls(obj);
}

void TCPServiceDialog::discardChanges()
{
    loadFWObject(obj);
}


/* ObjectEditor class connects its slot to this signal and does all
 * the verification for us, then accepts (or not) the event. So we do
 * nothing here and defer all the processing to ObjectEditor
 */
void TCPServiceDialog::closeEvent(QCloseEvent *e)
{
    emit close_sign(e);

}

void TCPServiceDialog::toggleEstablished()
{
    bool using_established = m_dialog->established->isChecked();

    m_dialog->urg_m->setEnabled( !using_established );
    m_dialog->ack_m->setEnabled( !using_established );
    m_dialog->psh_m->setEnabled( !using_established );
    m_dialog->rst_m->setEnabled( !using_established );
    m_dialog->syn_m->setEnabled( !using_established );
    m_dialog->fin_m->setEnabled( !using_established );

    m_dialog->urg_s->setEnabled( !using_established );
    m_dialog->ack_s->setEnabled( !using_established );
    m_dialog->psh_s->setEnabled( !using_established );
    m_dialog->rst_s->setEnabled( !using_established );
    m_dialog->syn_s->setEnabled( !using_established );
    m_dialog->fin_s->setEnabled( !using_established );

    m_dialog->flags_lbl_1->setEnabled( !using_established );
    m_dialog->flags_lbl_2->setEnabled( !using_established );
    m_dialog->flags_lbl_3->setEnabled( !using_established );
    m_dialog->flags_lbl_u->setEnabled( !using_established );
    m_dialog->flags_lbl_a->setEnabled( !using_established );
    m_dialog->flags_lbl_p->setEnabled( !using_established );
    m_dialog->flags_lbl_r->setEnabled( !using_established );
    m_dialog->flags_lbl_s->setEnabled( !using_established );
    m_dialog->flags_lbl_f->setEnabled( !using_established );

}

