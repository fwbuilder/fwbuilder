/* 

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

  $Id: ActionsDialog.cpp,v 1.29 2007/06/14 02:55:29 vkurland Exp $

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

#include "config.h"
#include "global.h"
#include "utils.h"
#include "platforms.h"
#include "definitions.h"

#include "ActionsDialog.h"
#include "ObjectManipulator.h"
#include "FWWindow.h"

#include "fwbuilder/Library.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Firewall.h"

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
#include <QStackedWidget>
#include <qcursor.h>
#include <qregexp.h>

#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace libfwbuilder;

void ActionsDialog::loadFWObject(FWObject *o)
{
    setRule(PolicyRule::cast(o));    

}
    
void ActionsDialog::changed()
{
    //apply->setEnabled( true );
    emit changed_sign();
}

void ActionsDialog::validate(bool *res)
{
    FWOptions *ruleopt =rule->getOptionsObject();

    *res=true;

    if (platform=="iptables")
    {
        /*
         *  http://www.netfilter.org/projects/patch-o-matic/pom-extra.html#pom-extra-ROUTE
         *  says:
         *
         * "Note that --iif, --continue, and --tee, are mutually exclusive."
         */

        string iif  = ruleopt->getStr("ipt_iif");
        int    cont = ruleopt->getBool("ipt_continue");
        int    tee  = ruleopt->getBool("ipt_tee");

        if ( (int(!iif.empty()) + cont + tee) > 1)
        {
            *res=false;
            QMessageBox::critical(this, "Firewall Builder",
                                  tr("'Chabge inbound interface', 'Continue packet inspection' and 'Make a copy' options are mutually exclusive"),
                                  tr("&Continue"), 0, 0,
                                  0 );
        }
    }
}

void ActionsDialog::isChanged(bool *res)
{
    //*res=apply->isEnabled();
}


void ActionsDialog::applyChanges()
{
    if (platform=="iptables" && editor=="AccountingStr")
    {
        QString rn = m_dialog->accountingvalue_str->text();
/* rule name for accounting may contain only alphanumeric characters
 * and no white spaces or spec. characters
 */
        if (rn.contains(QRegExp("[^a-zA-Z0-9_]"))!=0)
        {
            QMessageBox::information(
                this,"Firewall Builder", 
                tr("Rule name for accounting is converted to the iptables\nchain name and therefore may not contain white space\nand special characters."),
                tr("&Continue"), QString::null,QString::null,
                0, 1 );

            return;
        }
    }

    
    data.saveAll();
    
    FWOptions *ropt = rule->getOptionsObject();

    if (editor=="BranchChain" || editor=="BranchAnchor")
    {
        mw->setPolicyBranchTabName(rule->getBranch());
    }

    if (m_dialog->useDummyNetPipe->isChecked())
        ropt->setInt("ipfw_classify_method",DUMMYNETPIPE);
    else
        ropt->setInt("ipfw_classify_method",DUMMYNETQUEUE);



    om->updateLastModifiedTimestampForAllFirewalls(rule);
}

void ActionsDialog::discardChanges()
{
}

void ActionsDialog::tagvalueChanged(int)
{
    QString buf;
    buf.setNum(m_dialog->tagvalue_int->value());
    m_dialog->tagvalue_str->setText(buf);
}

void ActionsDialog::iptRouteContinueToggled()
{
    if (m_dialog->ipt_continue->isChecked())
    {
        m_dialog->ipt_iif->setCurrentIndex(0);
        m_dialog->ipt_tee->setChecked(false);
    }
    m_dialog->ipt_iif->setEnabled( ! m_dialog->ipt_continue->isChecked() );
    m_dialog->ipt_tee->setEnabled( ! m_dialog->ipt_continue->isChecked() );
}

void ActionsDialog::setRule(PolicyRule *r )
{
    rule=r;
    FWObject *o = rule;
    while (o!=NULL && Firewall::cast(o)==NULL) o=o->getParent();
    assert(o!=NULL);

    FWOptions *ropt = rule->getOptionsObject();
    
    Firewall *f=Firewall::cast(o);
    firewall=f;
    
    platform=f->getStr("platform");
    QString title=QString("%1 / %2 (%3)")
        .arg(QString::fromUtf8(f->getName().c_str()))
        .arg(rule->getPosition())
        .arg(rule->getActionAsString().c_str());
    m_dialog->action->setText(title);
    
    string act=rule->getActionAsString();
    
    QStringList actionsOnReject=getActionsOnReject( platform.c_str() );
    m_dialog->rejectvalue->clear();
    m_dialog->rejectvalue->addItems( getScreenNames( actionsOnReject ) );

    fillInterfaces(m_dialog->ipt_iif);
    fillInterfaces(m_dialog->ipt_oif);
    fillInterfaces(m_dialog->ipf_route_opt_if);
    fillInterfaces(m_dialog->pf_route_opt_if);

    editor = Resources::getActionEditor(platform,act);

    branchNameInput = NULL;

    if (ropt->getInt("ipfw_classify_method") == DUMMYNETPIPE)
    {
        m_dialog->useDummyNetPipe->setChecked(1);
    } else {
        m_dialog->useDummyNetQueue->setChecked(1);
    }

    if (platform=="iptables")
    {
        m_dialog->classify_txt_1->show();
        m_dialog->classify_terminating->show();
        m_dialog->tag_txt_1->show();
        m_dialog->tag_terminating->show();

        if (firewall->getOptionsObject()->getBool ("classify_mark_terminating"))
        {
            m_dialog->classify_terminating->setText(tr("Emulation is currently ON, rule will be terminating") );
            m_dialog->tag_terminating->setText(tr("Emulation is currently ON, rule will be terminating") );
        } else
        {
            m_dialog->classify_terminating->setText(tr("Emulation is currently OFF, rule will be non-terminating") );
            m_dialog->tag_terminating->setText(tr("Emulation is currently OFF, rule will be non-terminating") );
        }

    } else
    {
        m_dialog->classify_txt_1->hide();
        m_dialog->classify_terminating->hide();
        m_dialog->tag_txt_1->hide();
        m_dialog->tag_terminating->hide();
    }

    data.clear();

    data.registerOption ( m_dialog->ipt_mark_connections, ropt , "ipt_mark_connections");
//    data.registerOption ( ipt_mark_prerouting , ropt , "ipt_mark_prerouting");
    data.registerOption ( m_dialog->accountingvalue_str , ropt , "rule_name_accounting");
    data.registerOption ( m_dialog->usePortNum          , ropt , "ipfw_pipe_queue_num");
    data.registerOption ( m_dialog->divertPortNum       , ropt , "ipfw_pipe_port_num");
    data.registerOption ( m_dialog->classify_str        , ropt , "classify_str");
    data.registerOption ( m_dialog->custom_str          , ropt , "custom_str");

    // ROUTE action:

    // build a map for combobox so visible combobox items can be localized
    QStringList route_options = getRouteOptions_pf_ipf( platform.c_str() );

    // iptables
    data.registerOption ( m_dialog->ipt_iif             , ropt , "ipt_iif" );
    data.registerOption ( m_dialog->ipt_oif             , ropt , "ipt_oif" );
    data.registerOption ( m_dialog->ipt_gw              , ropt , "ipt_gw" );
    data.registerOption ( m_dialog->ipt_continue        , ropt , "ipt_continue" );
    data.registerOption ( m_dialog->ipt_tee             , ropt , "ipt_tee");

    // ipfilter
    data.registerOption ( m_dialog->ipf_route_option    , ropt , "ipf_route_option",
                          route_options);
    data.registerOption ( m_dialog->ipf_route_opt_if    , ropt , "ipf_route_opt_if");
    data.registerOption ( m_dialog->ipf_route_opt_addr  , ropt , "ipf_route_opt_addr");

    // pf
    data.registerOption ( m_dialog->pf_fastroute        , ropt , "pf_fastroute"     ); 
    data.registerOption ( m_dialog->pf_route_option     , ropt , "pf_route_option",
                          route_options); 
    data.registerOption ( m_dialog->pf_route_opt_if     , ropt , "pf_route_opt_if"  ); 
    data.registerOption ( m_dialog->pf_route_opt_addr   , ropt , "pf_route_opt_addr"); 

    // REJECT action:
    data.registerOption ( m_dialog->rejectvalue         , ropt , "action_on_reject"); 
    
    QWidget *w=m_dialog->NonePage;
    if (editor=="Reject")
    {
        w=m_dialog->RejectPage;
    }
    else if (editor=="TagInt")
    {
        w=m_dialog->TagIntPage;
        data.registerOption(m_dialog->tagvalue_int , ropt , "tagvalue");
    }
    else if (editor=="TagStr")
    {
        w=m_dialog->TagStrPage;
        data.registerOption(m_dialog->tagvalue_str , ropt , "tagvalue");
    }
    else if (editor=="AccountingStr")
    {
        w=m_dialog->AccountingStrPage;
    }
    else if (editor=="ClassifyArgsIPFW")
    {
        w=m_dialog->ClassifyArgsIPFW;
    }
    else if (editor=="PipeArgsIPFW")
    {
        w=m_dialog->PipeArgsIPFW;
    }
    else if (editor=="ClassifyStr")
    {
        w=m_dialog->ClassifyStrPage;
    }
    else if (editor=="CustomStr")
    {
        w=m_dialog->CustomStrPage;
    }
    else if (editor=="BranchChain")
    {
        w=m_dialog->BranchChainPage;
        data.registerOption ( m_dialog->branchChainName     , ropt , "branch_name" );
        data.registerOption ( m_dialog->ipt_branch_in_mangle, ropt , "ipt_branch_in_mangle" );
    }
    else if (editor=="BranchAnchor")
    {
        w=m_dialog->BranchAnchorPage;
        data.registerOption ( m_dialog->branchAnchorName    , ropt , "branch_name" );
    }
    else if (editor=="RouteIPT")
    {
        w=m_dialog->RouteIPTPage;
    }
    else if (editor=="RouteIPF")
    {
        w=m_dialog->RouteIPFPage;
    }
    else if (editor=="RoutePF")
    {
        w=m_dialog->RoutePFPage;
    }
    
    m_dialog->widgetStack->setCurrentWidget ( w );
    
    //rejectvalue->setCurrentText( ropt->getStr("action_on_reject") ); 
    data.loadAll();

    iptRouteContinueToggled();
}

void ActionsDialog::fillInterfaces(QComboBox* cb)
{
    cb->clear();
    cb->addItem("");
    
    FWObjectTypedChildIterator j=firewall->findByType(Interface::TYPENAME);
    for ( ; j!=j.end(); ++j )
    {
        cb->addItem(QString::fromUtf8( (*j)->getName().c_str()) );
    }
    
}

void ActionsDialog::closeEvent(QCloseEvent *e)
{
    if (fwbdebug)
        qDebug("ActionsDialog::closeEvent  got close event: %p",e);
    emit close_sign(e);
    //hide();
}

