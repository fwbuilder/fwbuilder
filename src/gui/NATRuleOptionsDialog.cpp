/*

                          Firewall Builder

                 Copyright (C) 2006 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>

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
#include "platforms.h"

#include "NATRuleOptionsDialog.h"
#include "RuleSetView.h"
#include "FWWindow.h"
#include "ProjectPanel.h"
#include "FWCmdRule.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Resources.h"

#include <memory>

#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlabel.h>

#include <iostream>


using namespace libfwbuilder;
using namespace std;

NATRuleOptionsDialog::~NATRuleOptionsDialog()
{
    delete m_dialog;
}

NATRuleOptionsDialog::NATRuleOptionsDialog(QWidget *parent) : BaseObjectDialog(parent)
{
    m_dialog = new Ui::NATRuleOptionsDialog_q;
    m_dialog->setupUi(this);

    init=false;
}

void NATRuleOptionsDialog::getHelpName(QString *str)
{
    *str = "NATRuleOptionsDialog";
}

void NATRuleOptionsDialog::loadFWObject(FWObject *o)
{
    obj=o;

    FWObject *p=obj;
    while ( !Firewall::cast(p) ) p=p->getParent();
    platform=p->getStr("platform").c_str();

    Rule      *rule = dynamic_cast<Rule*>(o);
    FWOptions *ropt = rule->getOptionsObject();

    // m_dialog->editorTitle->setText(QString("%1 / %2 / %3 ")
    //         .arg(QString::fromUtf8(p->getName().c_str()))
    //         .arg(rule->getTypeName().c_str())
    //         .arg(rule->getPosition()));

    int wid=0;
    if (platform=="ipf")      wid=0;
    if (platform=="ipfw")     wid=0;
    if (platform=="pix" || platform=="fwsm") wid=0;
    if (platform=="iptables") wid=1;
    if (platform=="pf")       wid=2;

    m_dialog->widgetStack->setCurrentIndex(wid);
    m_dialog->widgetStack->widget(wid)->raise();

    data.clear();

    if (platform=="iptables")
    {
        data.registerOption(m_dialog->ipt_use_snat_instead_of_masq, ropt,
                            "ipt_use_snat_instead_of_masq");
        data.registerOption(m_dialog->ipt_snat_random, ropt,
                            "ipt_snat_random");
    }

    if (platform=="pf")
    {
        data.registerOption(m_dialog->pf_pool_type_none, ropt,  "pf_pool_type_none" );
        data.registerOption(m_dialog->pf_bitmask       , ropt,  "pf_bitmask" );
        data.registerOption(m_dialog->pf_random        , ropt,  "pf_random" );
        data.registerOption(m_dialog->pf_source_hash   , ropt,  "pf_source_hash" );
        data.registerOption(m_dialog->pf_round_robin   , ropt,  "pf_round_robin" );
        data.registerOption(m_dialog->pf_static_port   , ropt,  "pf_static_port" );
    }

    init=true;
    data.loadAll();
    //apply->setEnabled( false );
    init=false;
}

void NATRuleOptionsDialog::validate(bool *res)
{
    *res=true;
}

void NATRuleOptionsDialog::applyChanges()
{

    std::auto_ptr<FWCmdRuleChange> cmd( new FWCmdRuleChangeOptions(m_project, obj));
    // new_state  is a copy of the rule object
    FWObject* new_state = cmd->getNewState();
    FWOptions* new_rule_options = Rule::cast(new_state)->getOptionsObject();

    init = true;
    data.saveAll(new_rule_options);
    init = false;

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }

}

