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


#include "definitions.h"
#include "global.h"
#include "utils.h"
#include "platforms.h"
#include "ProjectPanel.h"
#include "RuleOptionsDialog.h"
#include "RuleSetView.h"
#include "FWWindow.h"
#include "FWCmdRule.h"
#include "FWObjectDropArea.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/FailoverClusterGroup.h"

#include "fwbuilder/Rule.h"
#include "fwbuilder/FWOptions.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/RuleSet.h"
#include "fwbuilder/XMLTools.h"

#include <memory>

#include <qstackedwidget.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qlabel.h>

#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QObject>

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

    m_dialog->pfTagDropArea->addAcceptedTypes("TagService");
    m_dialog->iptTagDropArea->addAcceptedTypes("TagService");

    connectSignalsOfAllWidgetsToSlotChange();

    firewall = nullptr;
    init=false;
}

void RuleOptionsDialog::loadFWObject(FWObject *o)
{
    obj = o;
    firewall = o;
    // use Firewall::cast to match both Firewall and Cluster
    while (!Firewall::cast(firewall)) firewall = firewall->getParent();
    platform = firewall->getStr("platform").c_str();
    string version = firewall->getStr("version");

    // build a map for combobox so visible combobox items can be localized
    QStringList route_options = getRouteOptions_pf_ipf(platform);
    QStringList route_load_options = getRouteLoadOptions_pf(platform);
    QStringList classify_options_ipfw = getClassifyOptions_ipfw(platform);

    Rule      *rule = dynamic_cast<Rule*>(o);
    FWOptions *ropt = rule->getOptionsObject();
    PolicyRule *policy_rule = PolicyRule::cast(rule);

    int wid=0;
    if (platform=="iptables") wid=1;
    if (platform=="ipf")      wid=2;
    if (platform=="pf")       wid=3;
    if (platform=="ipfw")     wid=4;
    if (platform=="pix" || platform=="fwsm")      wid=5;
    if (platform=="iosacl" || platform=="procurve_acl")   wid=6;
	if (platform=="junosacl") wid=7;

    m_dialog->wStack->widget(wid)->raise();
    m_dialog->wStack->setCurrentWidget(m_dialog->wStack->widget(wid));

    QStringList  logLevels=getLogLevels( obj->getStr("platform").c_str() );
    m_dialog->ipt_logLevel->clear();
    m_dialog->ipt_logLevel->addItems(getScreenNames(logLevels));
    m_dialog->ipf_logLevel->clear();
    m_dialog->ipf_logLevel->addItems(getScreenNames(logLevels));
    m_dialog->pix_logLevel->clear();
    m_dialog->pix_logLevel->addItems(getScreenNames(logLevels));

    QStringList logFacilities=getLogFacilities( obj->getStr("platform").c_str());
    m_dialog->ipf_logFacility->clear();
    m_dialog->ipf_logFacility->addItems(getScreenNames(logFacilities));
    QStringList limitSuffixes=getLimitSuffixes( obj->getStr("platform").c_str());
    m_dialog->ipt_limitSuffix->clear();
    m_dialog->ipt_limitSuffix->addItems(getScreenNames(limitSuffixes));

    m_dialog->ipt_hashlimit_suffix->clear();
    m_dialog->ipt_hashlimit_suffix->addItems(getScreenNames(limitSuffixes));

    fillInterfaces(m_dialog->ipt_iif);
    fillInterfaces(m_dialog->ipt_oif);
    fillInterfaces(m_dialog->ipf_route_opt_if);
    fillInterfaces(m_dialog->pf_route_opt_if);


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
        data.registerOption(m_dialog->ipt_limit_not, ropt,  "limit_value_not");
        data.registerOption(m_dialog->ipt_burst, ropt,  "limit_burst");

        data.registerOption(m_dialog->ipt_connlimit, ropt,  "connlimit_value");
        data.registerOption(m_dialog->ipt_connlimit_above_not, ropt,
                            "connlimit_above_not");
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
                            "firewall_is_part_of_any_and_networks",
                            threeStateMapping);
        data.registerOption(m_dialog->ipt_stateless, ropt,  "stateless");

        data.registerOption(m_dialog->ipt_mark_connections, ropt,
                            "ipt_mark_connections");

        data.registerOption(m_dialog->classify_str, ropt, "classify_str");

        // Route
        data.registerOption(m_dialog->ipt_iif, ropt, "ipt_iif" );
        data.registerOption(m_dialog->ipt_oif, ropt, "ipt_oif" );
        data.registerOption(m_dialog->ipt_gw, ropt, "ipt_gw" );
        data.registerOption(m_dialog->ipt_continue, ropt, "ipt_continue" );
        data.registerOption(m_dialog->ipt_tee, ropt, "ipt_tee");

        FWObject *o = policy_rule->getTagObject();
        m_dialog->iptTagDropArea->setObject(o);
        m_dialog->iptTagDropArea->update();
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

        // Route
        data.registerOption(m_dialog->ipf_route_option, ropt,
                            "ipf_route_option", route_options);
        data.registerOption(m_dialog->ipf_route_opt_if, ropt,
                            "ipf_route_opt_if");
        data.registerOption(m_dialog->ipf_route_opt_addr, ropt,
                            "ipf_route_opt_addr");
    }

    if (platform=="pf")
    {
        bool ge_4_5 = XMLTools::version_compare(version, "4.5")>=0;

        m_dialog->pf_no_sync->setEnabled(ge_4_5);
        m_dialog->pf_pflow->setEnabled(ge_4_5);
        
        data.registerOption(m_dialog->pf_logPrefix, ropt,
                            "log_prefix");
        data.registerOption(m_dialog->pf_stateless, ropt,
                            "stateless");
        data.registerOption(m_dialog->pf_keep_state, ropt,
                            "pf_keep_state");
        data.registerOption(m_dialog->pf_no_sync, ropt,
                            "pf_no_sync");
        data.registerOption(m_dialog->pf_pflow, ropt,
                            "pf_pflow");
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

        // Tag
        FWObject *o = policy_rule->getTagObject();
        m_dialog->pfTagDropArea->setObject(o);
        m_dialog->pfTagDropArea->update();

        // Classify
        data.registerOption(m_dialog->pf_classify_str, ropt, "pf_classify_str");

        // Route
        data.registerOption(m_dialog->pf_fastroute, ropt, "pf_fastroute");
        data.registerOption(m_dialog->pf_route_load_option, ropt,
                            "pf_route_load_option", route_load_options);
        data.registerOption(m_dialog->pf_route_option, ropt, "pf_route_option",
                            route_options);
        data.registerOption(m_dialog->pf_route_opt_if, ropt, "pf_route_opt_if");
        data.registerOption(m_dialog->pf_route_opt_addr, ropt, "pf_route_opt_addr");
    }

    if (platform=="ipfw")
    {
        data.registerOption(m_dialog->ipfw_stateless, ropt, "stateless");

/* #2367 */
        // Classify
        data.registerOption(m_dialog->ipfw_classify_method, ropt,
                            "ipfw_classify_method", classify_options_ipfw);
        data.registerOption(m_dialog->usePortNum, ropt, "ipfw_pipe_queue_num");
    }

    if (platform=="iosacl" || platform=="procurve_acl")
    {
        data.registerOption(m_dialog->iosacl_add_mirror_rule,
                            ropt, "iosacl_add_mirror_rule");
    }

    if (platform=="pix" || platform=="fwsm")
    {
        string vers = "version_" + version;
        if (Resources::platform_res[platform.toLatin1().constData()]->getResourceBool(
              "/FWBuilderResources/Target/options/" +
              vers + "/pix_rule_syslog_settings"))
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

    if (platform=="junosacl")
    {
        data.registerOption(m_dialog->counterLineEdit, ropt, "counter_name");
    }



    init = true;
    data.loadAll();

    m_dialog->pf_max_src_nodes->setEnabled(
        m_dialog->pf_source_tracking->isChecked());
    m_dialog->pf_max_src_states->setEnabled(
        m_dialog->pf_source_tracking->isChecked());


    connlimitAboveLabelChange();
    limitLabelChange();

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

    iptRouteContinueToggled(); // #2367

    BaseObjectDialog::changed();
}

void RuleOptionsDialog::validate(bool *res)
{
    *res=true;
}

void RuleOptionsDialog::applyChanges()
{

    std::unique_ptr<FWCmdChange> cmd( new FWCmdRuleChangeOptions(m_project, obj));
    // new_state  is a copy of the rule object
    FWObject* new_state = cmd->getNewState();
    FWOptions* new_rule_options = Rule::cast(new_state)->getOptionsObject();

    init = true;
    data.saveAll(new_rule_options);
    init = false;

/*  #2367 */

    PolicyRule *policy_rule = PolicyRule::cast(new_state);
    if (policy_rule)
    {
        FWOptions *ropt = policy_rule->getOptionsObject();

        if (platform=="iptables")
        {
            FWObject *tag_object = m_dialog->iptTagDropArea->getObject();
            // if tag_object==nullptr, setTagObject clears setting in the rule
            policy_rule->setTagging(tag_object != nullptr);
            policy_rule->setTagObject(tag_object);

            policy_rule->setClassification(
                ! ropt->getStr("classify_str").empty());

            policy_rule->setRouting( ! ropt->getStr("ipt_iif").empty() ||
                                     ! ropt->getStr("ipt_oif").empty() ||
                                     ! ropt->getStr("ipt_gw").empty());
        }
    
        if (platform=="pf")
        {
            FWObject *tag_object = m_dialog->pfTagDropArea->getObject();
            // if tag_object==nullptr, setTagObject clears setting in the rule
            policy_rule->setTagging(tag_object != nullptr);
            policy_rule->setTagObject(tag_object);

            policy_rule->setClassification(
                ! new_rule_options->getStr("pf_classify_str").empty());

            policy_rule->setRouting(
                ! new_rule_options->getStr("pf_route_option").empty() &&
                new_rule_options->getStr("pf_route_option") != "none");
        }

        if (platform=="ipf")
        {
            policy_rule->setRouting(
                ! new_rule_options->getStr("ipf_route_option").empty() &&
                new_rule_options->getStr("ipf_route_option") != "none");
        }

        if (platform=="ipfw")
        {
            policy_rule->setClassification(
                new_rule_options->getInt("ipfw_classify_method") > -1);
        }
    }

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

void RuleOptionsDialog::connlimitAboveLabelChange()
{
    if (m_dialog->ipt_connlimit_above_not->isChecked())
        m_dialog->ipt_connlimit_above_label->setText(
            "Match if the number of existing connections is below this "
            "(translates into option ! --connlimit-above)");
    else
        m_dialog->ipt_connlimit_above_label->setText(
            "Match if the number of existing connections is above this "
            "(translates into option --connlimit-above)");

    changed();
}

void RuleOptionsDialog::limitLabelChange()
{
    if (m_dialog->ipt_limit_not->isChecked())
        m_dialog->ipt_limit_label->setText(
            "Maximum average matching rate (negated) "
            "(translates into option ! --limit rate)");
    else
        m_dialog->ipt_limit_label->setText(
            "Maximum average matching rate "
            "(translates into option --limit rate)");

    changed();
}

void RuleOptionsDialog::iptRouteContinueToggled()
{
    if (m_dialog->ipt_continue->isChecked())
    {
        m_dialog->ipt_iif->setCurrentIndex(0);
        m_dialog->ipt_tee->setChecked(false);
    }
    m_dialog->ipt_iif->setEnabled( ! m_dialog->ipt_continue->isChecked() );
    m_dialog->ipt_tee->setEnabled( ! m_dialog->ipt_continue->isChecked() );
}

void RuleOptionsDialog::fillInterfaces(QComboBox* cb)
{
    QSet<QString> deduplicated_interface_names;

    list<FWObject*> interfaces = firewall->getByTypeDeep(Interface::TYPENAME);
    for (list<FWObject*>::iterator i=interfaces.begin(); i!=interfaces.end(); ++i)
    {
        Interface *iface = Interface::cast(*i);
        assert(iface);

        if (iface->isLoopback()) continue;

        deduplicated_interface_names.insert(iface->getName().c_str());

        if (Cluster::isA(firewall))
        {
            FailoverClusterGroup *failover_group =
                FailoverClusterGroup::cast(
                    iface->getFirstByType(FailoverClusterGroup::TYPENAME));
            if (failover_group)
            {
                for (FWObject::iterator it=failover_group->begin();
                     it!=failover_group->end(); ++it)
                {
                    FWObject *mi = FWReference::getObject(*it);
                    if (Interface::isA(mi) && ! iface->isLoopback())
                    {
                        deduplicated_interface_names.insert(mi->getName().c_str());
                    }
                }
            }
        }
    }

    QStringList sorted_interfaces;
    QSetIterator<QString> it(deduplicated_interface_names);
    while (it.hasNext())
    {
        sorted_interfaces << it.next();
    }
    sorted_interfaces.sort();

    cb->clear();
    cb->addItem("");
    cb->addItems(sorted_interfaces);
}


