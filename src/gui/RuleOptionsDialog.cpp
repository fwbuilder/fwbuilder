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
#include "platforms.h"
#include "ProjectPanel.h"
#include "RuleOptionsDialog.h"
#include "RuleSetView.h"
#include "FWWindow.h"
#include "FWCmdChange.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Rule.h"

#include <memory>

#include <qstackedwidget.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qlabel.h>

#include <iostream>


using namespace libfwbuilder;
using namespace std;

RuleOptionsDialog::~RuleOptionsDialog()
{
    delete m_dialog;
}

RuleOptionsDialog::RuleOptionsDialog(QWidget *parent) :
    BaseObjectDialog(parent)
{
    m_dialog = new Ui::RuleOptionsDialog_q;
    m_dialog->setupUi(this);

    init=false;
}

void RuleOptionsDialog::getHelpName(QString *str)
{
    // can make help name different depending on the currently opened tab
    *str = help_name;
}

void RuleOptionsDialog::loadFWObject(FWObject *o)
{
    obj=o;
//    rsv=rv;

    FWObject *p=obj;
    // use Firewall::cast to match both Firewall and Cluster
    while (!Firewall::cast(p)) p = p->getParent();
    platform=p->getStr("platform").c_str();

    help_name = platform + "_rule_options";

    Rule      *rule = dynamic_cast<Rule*>(o);
    FWOptions *ropt = rule->getOptionsObject();

    // m_dialog->editorTitle->setText(QString("%1 / %2 / %3 ")
    //         .arg(QString::fromUtf8(p->getName().c_str()))
    //         .arg(rule->getTypeName().c_str())
    //         .arg(rule->getPosition()));

    int wid=0;
    if (platform=="iptables") wid=0;
    if (platform=="ipf")      wid=1;
    if (platform=="pf")       wid=2;
    if (platform=="ipfw")     wid=3;
    if (platform=="pix" || platform=="fwsm")      wid=4;
    if (platform=="iosacl")   wid=5;

    m_dialog->wStack->widget(wid)->raise();
    m_dialog->wStack->setCurrentWidget(m_dialog->wStack->widget(wid));

    QStringList  logLevels=getLogLevels( obj->getStr("platform").c_str() );
    m_dialog->ipt_logLevel->clear();
    m_dialog->ipt_logLevel->addItems(getScreenNames(logLevels));
    m_dialog->ipf_logLevel->clear();
    m_dialog->ipf_logLevel->addItems(getScreenNames(logLevels));
    m_dialog->pix_logLevel->clear();
    m_dialog->pix_logLevel->addItems(getScreenNames(logLevels));

    QStringList  logFacilities=getLogFacilities( obj->getStr("platform").c_str() );
    m_dialog->ipf_logFacility->clear();
    m_dialog->ipf_logFacility->addItems(getScreenNames(logFacilities));
    QStringList limitSuffixes=getLimitSuffixes( obj->getStr("platform").c_str() );
    m_dialog->ipt_limitSuffix->clear();
    m_dialog->ipt_limitSuffix->addItems(getScreenNames(limitSuffixes));

    m_dialog->ipt_hashlimit_suffix->clear();
    m_dialog->ipt_hashlimit_suffix->addItems(getScreenNames(limitSuffixes));

    data.clear();

    if (platform=="iptables")
    {
        data.registerOption(m_dialog->ipt_logPrefix, ropt,  "log_prefix");
        data.registerOption(m_dialog->ipt_logLevel, ropt,
                             "log_level", logLevels);
        data.registerOption(m_dialog->ipt_nlgroup, ropt,  "ulog_nlgroup");
        data.registerOption(m_dialog->ipt_limit, ropt,  "limit_value");
        data.registerOption(m_dialog->ipt_limitSuffix, ropt,
                             "limit_suffix", limitSuffixes);
        data.registerOption(m_dialog->ipt_burst, ropt,  "limit_burst");

        data.registerOption(m_dialog->ipt_connlimit, ropt,  "connlimit_value");
        data.registerOption(m_dialog->ipt_connlimit_masklen, ropt,
                            "connlimit_masklen");

        data.registerOption(m_dialog->ipt_hashlimit, ropt,  "hashlimit_value");
        data.registerOption(m_dialog->ipt_hashlimit_suffix, ropt,
                            "hashlimit_suffix");
        data.registerOption(m_dialog->ipt_hashlimit_burst, ropt,
                            "hashlimit_burst");
        data.registerOption(m_dialog->cb_srcip, ropt,  "hashlimit_mode_srcip");
        data.registerOption(m_dialog->cb_dstip, ropt,  "hashlimit_mode_dstip");
        data.registerOption(m_dialog->cb_srcport, ropt,
                            "hashlimit_mode_srcport");
        data.registerOption(m_dialog->cb_dstport, ropt,
                            "hashlimit_mode_dstport");
        data.registerOption(m_dialog->ipt_hashlimit_dstlimit, ropt,
                            "hashlimit_dstlimit");
        data.registerOption(m_dialog->ipt_hashlimit_name, ropt,
                            "hashlimit_name");
        data.registerOption(m_dialog->ipt_hashlimit_size, ropt,
                            "hashlimit_size");
        data.registerOption(m_dialog->ipt_hashlimit_max, ropt,
                            "hashlimit_max");
        data.registerOption(m_dialog->ipt_hashlimit_expire, ropt,
                            "hashlimit_expire");
        data.registerOption(m_dialog->ipt_hashlimit_gcinterval, ropt,
                            "hashlimit_gcinterval");

        // in v3.0 attribute "assume fw is part of any" used to be a
        // checkbox and therefore stored as boolean in the rule
        // options. Old "on" maps to the new "on", which means old "True"
        // maps to "1". Old "off" maps to "use global" though.
        string old_val = ropt->getStr("firewall_is_part_of_any_and_networks");
        if (old_val == "True") ropt->setStr("firewall_is_part_of_any_and_networks", "1");
        if (old_val == "False") ropt->setStr("firewall_is_part_of_any_and_networks", "");

        QStringList threeStateMapping;
        threeStateMapping.push_back(QObject::tr("Follow global setting"));
        threeStateMapping.push_back("");

        threeStateMapping.push_back(QObject::tr("On"));
        threeStateMapping.push_back("1");

        threeStateMapping.push_back(QObject::tr("Off"));
        threeStateMapping.push_back("0");

        data.registerOption(m_dialog->ipt_assume_fw_is_part_of_any, ropt,
                            "firewall_is_part_of_any_and_networks", threeStateMapping);
        data.registerOption(m_dialog->ipt_stateless, ropt,  "stateless");
    }


    if (platform=="ipf")
    {
        data.registerOption(m_dialog->ipf_logFacility, ropt,
                            "ipf_log_facility", logFacilities);
        data.registerOption(m_dialog->ipf_logLevel, ropt,
                            "log_level", logLevels);
        data.registerOption(m_dialog->ipf_masq_icmp, ropt,
                            "ipf_return_icmp_as_dest");
        data.registerOption(m_dialog->ipf_stateless, ropt,  "stateless");
        data.registerOption(m_dialog->ipf_keep_frags, ropt,  "ipf_keep_frags");
    }

    if (platform=="pf")
    {
        data.registerOption(m_dialog->pf_logPrefix, ropt,
                            "log_prefix");
        data.registerOption(m_dialog->pf_stateless, ropt,
                            "stateless");
        data.registerOption(m_dialog->pf_keep_state, ropt,
                            "pf_keep_state");
        data.registerOption(m_dialog->pf_sloppy_tracker, ropt,
                            "pf_sloppy_tracker");
        data.registerOption(m_dialog->pf_rule_max_state, ropt,
                            "pf_rule_max_state");
        data.registerOption(m_dialog->pf_source_tracking, ropt,
                            "pf_source_tracking");
        data.registerOption(m_dialog->pf_max_src_nodes, ropt,
                            "pf_max_src_nodes");
        data.registerOption(m_dialog->pf_max_src_states, ropt,
                            "pf_max_src_states");
        data.registerOption(m_dialog->pf_max_src_conn, ropt,
                            "pf_max_src_conn");
        data.registerOption(m_dialog->pf_overload_table, ropt,
                             "pf_max_src_conn_overload_table");
        data.registerOption(m_dialog->pf_flush, ropt,
                            "pf_max_src_conn_flush");
        data.registerOption(m_dialog->pf_global, ropt,
                            "pf_max_src_conn_global");
        data.registerOption(m_dialog->pf_max_src_conn_rate_num, ropt,
                             "pf_max_src_conn_rate_num");
        data.registerOption(m_dialog->pf_max_src_conn_rate_seconds, ropt,
                             "pf_max_src_conn_rate_seconds");
        data.registerOption(m_dialog->pf_modulate, ropt,
                             "pf_modulate_state");
        data.registerOption(m_dialog->pf_synproxy, ropt,
                             "pf_synproxy");
    }

    if (platform=="ipfw")
    {
        data.registerOption(m_dialog->ipfw_stateless, ropt,"stateless");
    }

    if (platform=="pix" || platform=="fwsm")
    {
        string vers="version_"+p->getStr("version");
        if (Resources::platform_res[platform.toAscii().constData()]->getResourceBool(
              "/FWBuilderResources/Target/options/"+vers+"/pix_rule_syslog_settings"))
        {
            m_dialog->pix_disable_rule_log->setEnabled(true);
            m_dialog->pix_logLevel->setEnabled(true);
            m_dialog->pix_log_interval->setEnabled(true);

            data.registerOption(m_dialog->pix_disable_rule_log, ropt,
                                "disable_logging_for_this_rule");
            data.registerOption(m_dialog->pix_logLevel, ropt,
                                "log_level",logLevels);
            data.registerOption(m_dialog->pix_log_interval, ropt,
                                "log_interval");
        } else
        {
            m_dialog->pix_disable_rule_log->setEnabled(false);
            m_dialog->pix_logLevel->setEnabled(false);
            m_dialog->pix_log_interval->setEnabled(false);
        }

    }

    init=true;
    data.loadAll();

    m_dialog->pf_max_src_nodes->setEnabled(
        m_dialog->pf_source_tracking->isChecked());
    m_dialog->pf_max_src_states->setEnabled(
        m_dialog->pf_source_tracking->isChecked());

    //apply->setEnabled(false);
    init=false;
}

void RuleOptionsDialog::changed()
{
    //apply->setEnabled(true);

    m_dialog->pf_max_src_nodes->setEnabled(
        m_dialog->pf_source_tracking->isChecked());
    m_dialog->pf_max_src_states->setEnabled(
        m_dialog->pf_source_tracking->isChecked());

    bool enable_overload_options = (
        m_dialog->pf_max_src_conn->value()>0 || (
            m_dialog->pf_max_src_conn_rate_num->value()>0 &&
            m_dialog->pf_max_src_conn_rate_seconds->value()>0)
   );

    m_dialog->pf_overload_table->setEnabled(enable_overload_options);
    m_dialog->pf_flush->setEnabled(enable_overload_options);
    m_dialog->pf_global->setEnabled(enable_overload_options);

    BaseObjectDialog::changed();
}

void RuleOptionsDialog::validate(bool *res)
{
    *res=true;
}



void RuleOptionsDialog::applyChanges()
{

    std::auto_ptr<FWCmdChange> cmd( new FWCmdChangeRuleOptions(m_project, obj));
    // new_state  is a copy of the rule object
    FWObject* new_state = cmd->getNewState();
    FWOptions* new_rule_options = Rule::cast(new_state)->getOptionsObject();

    init=true;
    data.saveAll(new_rule_options);
    init=false;

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        if (obj->isReadOnly()) return;
        m_project->undoStack->push(cmd.release());
    }

}

void RuleOptionsDialog::cancelChanges()
{
    //apply->setEnabled(false);
    close();
}

