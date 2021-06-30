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

#include "../cisco_lib/ASA8TwiceNatLogic.h"

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

void NATRuleOptionsDialog::loadFWObject(FWObject *o)
{
    obj = o;

    FWObject *p = obj;
    while ( !Firewall::cast(p) ) p = p->getParent();
    platform = p->getStr("platform").c_str();

    string version = p->getStr("version");

    Rule      *rule = dynamic_cast<Rule*>(o);
    FWOptions *ropt = rule->getOptionsObject();

    // m_dialog->editorTitle->setText(QString("%1 / %2 / %3 ")
    //         .arg(QString::fromUtf8(p->getName().c_str()))
    //         .arg(rule->getTypeName().c_str())
    //         .arg(rule->getPosition()));

    int wid = 0;
    if (platform=="ipf")      wid=0;
    if (platform=="ipfw")     wid=0;
    if (platform=="pix" || platform=="fwsm") wid = 3;
    if (platform=="iptables") wid=1;
    if (platform=="pf")       wid=2;

    m_dialog->widgetStack->setCurrentIndex(wid);
    m_dialog->widgetStack->widget(wid)->raise();

    data.clear();

    if (platform=="iptables")
    {
        data.registerOption(m_dialog->ipt_use_snat_instead_of_masq, ropt,
                            "ipt_use_snat_instead_of_masq");
        data.registerOption(m_dialog->ipt_use_masq, ropt,
                            "ipt_use_masq");
        data.registerOption(m_dialog->ipt_nat_random, ropt,
                            "ipt_nat_random");
        data.registerOption(m_dialog->ipt_nat_persistent, ropt,
                            "ipt_nat_persistent");
    }

    if (platform=="pf")
    {
        data.registerOption(m_dialog->pf_pool_type_none, ropt,
                            "pf_pool_type_none");
        data.registerOption(m_dialog->pf_bitmask, ropt, "pf_bitmask");
        data.registerOption(m_dialog->pf_random, ropt, "pf_random");
        data.registerOption(m_dialog->pf_source_hash, ropt, "pf_source_hash");
        data.registerOption(m_dialog->pf_round_robin, ropt, "pf_round_robin");
        data.registerOption(m_dialog->pf_static_port, ropt, "pf_static_port");
    }

    if (platform=="pix" || platform=="fwsm")
    {
        if (libfwbuilder::XMLTools::version_compare(version,"8.3")>=0)
        {
            m_dialog->asa8_nat_dns->setEnabled(true);
            m_dialog->asa8_nat_auto->setEnabled(true);
            m_dialog->asa8_nat_dynamic->setEnabled(true);
            m_dialog->asa8_nat_static->setEnabled(true);

            data.registerOption(m_dialog->asa8_nat_dns, ropt,
                                "asa8_nat_dns");

            NATRule *nat_rule = NATRule::cast(rule);
            ASA8TwiceNatStaticLogic twice_nat_logic(nat_rule);

            // set asa8_nat_auto to True if none of these are set yet
            if (!ropt->getBool("asa8_nat_dynamic") && 
                !ropt->getBool("asa8_nat_static"))
            {
                ropt->setBool("asa8_nat_auto", true);
            }

            data.registerOption(m_dialog->asa8_nat_auto, ropt,
                                "asa8_nat_auto");
            data.registerOption(m_dialog->asa8_nat_dynamic, ropt,
                                "asa8_nat_dynamic");
            data.registerOption(m_dialog->asa8_nat_static, ropt,
                                "asa8_nat_static");

            // update text label of radio button asa8_nat_auto
            QString rule_state_auto;

            switch (twice_nat_logic.getAutomaticType())
            {
            case ASA8TwiceNatStaticLogic::STATIC:
                rule_state_auto = "static";
                break;
            case ASA8TwiceNatStaticLogic::DYNAMIC:
                rule_state_auto = "dynamic";
                break;
            }

            QString button_txt = tr(
                "Automatically detect NAT type \"static\" or \"dynamic\". "
                "This rule is currently set to type \"%1\"");

            m_dialog->asa8_nat_auto->setText(button_txt.arg(rule_state_auto));

        } else
        {
            m_dialog->asa8_nat_dns->setEnabled(false);
            m_dialog->asa8_nat_auto->setEnabled(false);
            m_dialog->asa8_nat_dynamic->setEnabled(false);
            m_dialog->asa8_nat_static->setEnabled(false);
        }
    }

    init = true;
    data.loadAll();
    //apply->setEnabled( false );
    init=false;
}

void NATRuleOptionsDialog::validate(bool *res)
{
    *res = true;
}

void NATRuleOptionsDialog::applyChanges()
{
    std::unique_ptr<FWCmdRuleChange> cmd(
        new FWCmdRuleChangeOptions(m_project, obj));

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

