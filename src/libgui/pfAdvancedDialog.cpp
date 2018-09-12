/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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
#include "platforms.h"

#include "pfAdvancedDialog.h"
#include "SimpleTextEditor.h"
#include "FWWindow.h"
#include "Help.h"
#include "FWCmdChange.h"

#include "CompilerDriver.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/XMLTools.h"

#include <memory>

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qstackedwidget.h>
#include <qregexp.h>
#include <qtextedit.h>
#include <QUndoStack>
#include <QFileInfo>


using namespace std;
using namespace libfwbuilder;

pfAdvancedDialog::pfAdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::pfAdvancedDialog_q;
    m_dialog->setupUi(this);
    obj=o;
    QStringList slm;

    string version = obj->getStr("version");

    FWOptions *fwopt=(Firewall::cast(obj))->getOptionsObject();
    assert(fwopt!=nullptr);

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=nullptr);

    if (fwbdebug)
        qDebug("%s", Resources::getTargetOptionStr(
                   obj->getStr("host_OS"),"user_can_change_install_dir").c_str());

    if (!Resources::getTargetOptionBool(
            obj->getStr("host_OS"),"user_can_change_install_dir"))
    {
        m_dialog->pf_fw_dir->setEnabled(false);
        fwopt->setStr("firewall_dir","");
    }

    // see #1888: we now support rc.conf format for the output
    // Set variables for backwards compatibility for users who configured
    // custom name for the output .fw script before.

    if (!fwopt->getBool("generate_shell_script") &&
        !fwopt->getBool("generate_rc_conf_file"))
    {
        fwopt->setBool("generate_shell_script", true);
    }

    if (!Resources::getTargetOptionBool(obj->getStr("host_OS"),
                                        "rc_conf_format_supported"))
    {
        fwopt->setBool("generate_shell_script", true);
        fwopt->setBool("generate_rc_conf_file", false);
    }

    m_dialog->generateShellScript->setEnabled(
        Resources::getTargetOptionBool(obj->getStr("host_OS"),
                                       "rc_conf_format_supported"));
    m_dialog->generateRcConfFile->setEnabled(
        Resources::getTargetOptionBool(obj->getStr("host_OS"),
                                       "rc_conf_format_supported"));


    QString init_script_name = QString::fromUtf8(
        fwopt->getStr("output_file").c_str()).trimmed();
    QString conf_file_name = QString::fromUtf8(
        fwopt->getStr("conf1_file").c_str()).trimmed();

    if (!init_script_name.isEmpty() && conf_file_name.isEmpty())
    {
        conf_file_name =
            fwcompiler::CompilerDriver::getConfFileNameFromFwFileName(
                init_script_name, ".conf");
        fwopt->setStr("conf1_file", conf_file_name.toUtf8().constData());
    }

    data.registerOption(m_dialog->ipv4before, fwopt,
                        "ipv4_6_order",
                        QStringList() <<  tr("IPv4 before IPv6")
                        <<"ipv4_first"
                        << tr("IPv6 before IPv4")
                        << "ipv6_first");

    data.registerOption( m_dialog->pf_log_prefix,fwopt, "log_prefix");
    data.registerOption( m_dialog->pf_fallback_log,fwopt, "fallback_log");
    data.registerOption( m_dialog->pf_do_timeout_interval, fwopt,
                         "pf_do_timeout_interval");
    data.registerOption( m_dialog->pf_timeout_interval, fwopt,
                         "pf_timeout_interval");
    data.registerOption( m_dialog->pf_do_timeout_frag,fwopt, "pf_do_timeout_frag");
    data.registerOption( m_dialog->pf_timeout_frag,fwopt, "pf_timeout_frag");
    data.registerOption( m_dialog->pf_do_limit_frags,fwopt, "pf_do_limit_frags");
    data.registerOption( m_dialog->pf_limit_frags,fwopt, "pf_limit_frags");
    data.registerOption( m_dialog->pf_do_limit_states,fwopt, "pf_do_limit_states");
    data.registerOption( m_dialog->pf_limit_states,fwopt, "pf_limit_states");
    data.registerOption( m_dialog->pf_do_limit_src_nodes,fwopt,
                         "pf_do_limit_src_nodes");
    data.registerOption( m_dialog->pf_limit_src_nodes, fwopt, "pf_limit_src_nodes");
    data.registerOption( m_dialog->pf_do_limit_tables, fwopt, "pf_do_limit_tables");
    data.registerOption( m_dialog->pf_limit_tables,fwopt, "pf_limit_tables");
    data.registerOption( m_dialog->pf_do_limit_table_entries,fwopt,
                         "pf_do_limit_table_entries");
    data.registerOption( m_dialog->pf_limit_table_entries,fwopt,"pf_limit_table_entries");

// Prepare mapping for pf_optimization:
    slm.clear();
    slm.push_back("");
    slm.push_back("");
    slm.push_back(QObject::tr("Aggressive"));
    slm.push_back("aggressive");
    slm.push_back(QObject::tr("Conservative"));
    slm.push_back("conservative");
    slm.push_back(QObject::tr("For high latency"));
    slm.push_back("high-latency");
    slm.push_back(QObject::tr("Normal"));
    slm.push_back("normal");
    m_dialog->pf_optimization->clear();
    m_dialog->pf_optimization->addItems(getScreenNames(slm));
    data.registerOption( m_dialog->pf_optimization, fwopt, "pf_optimization", slm);

// Prepare state_policy combo box
    slm.clear();
    slm.push_back("");
    slm.push_back("");
    slm.push_back(QObject::tr("Bound to interfaces"));
    slm.push_back("if-bound");
    slm.push_back(QObject::tr("Floating"));
    slm.push_back("floating");
    m_dialog->pf_state_policy->clear();
    m_dialog->pf_state_policy->addItems(getScreenNames(slm));
    data.registerOption( m_dialog->pf_state_policy, fwopt, "pf_state_policy", slm);
    m_dialog->pf_state_policy->setEnabled(
        XMLTools::version_compare(version, "3.5") >= 0);

// Prepare block_policy combo box
    slm.clear();
    slm.push_back("");
    slm.push_back("");
    slm.push_back(QObject::tr("Drop"));
    slm.push_back("drop");
    slm.push_back(QObject::tr("Return"));
    slm.push_back("return");
    m_dialog->pf_block_policy->clear();
    m_dialog->pf_block_policy->addItems(getScreenNames(slm));
    data.registerOption( m_dialog->pf_block_policy, fwopt, "pf_block_policy", slm);
    m_dialog->pf_block_policy->setEnabled(
        XMLTools::version_compare(version, "3.5") >= 0);

// set debug combo box
    slm.clear();
    slm.push_back("");
    slm.push_back("");
    slm.push_back("emerg");
    slm.push_back("emerg");
    slm.push_back("alert");
    slm.push_back("alert");
    slm.push_back("crit");
    slm.push_back("crit");
    slm.push_back("err");
    slm.push_back("err");
    slm.push_back("warning");
    slm.push_back("warning");
    slm.push_back("notice");
    slm.push_back("notice");
    slm.push_back("info");
    slm.push_back("info");
    slm.push_back("debug");
    slm.push_back("debug");

    m_dialog->pf_set_debug->clear();
    m_dialog->pf_set_debug->addItems(getScreenNames(slm));
    data.registerOption( m_dialog->pf_set_debug, fwopt, "pf_set_debug", slm);
    m_dialog->pf_set_debug->setEnabled(
        XMLTools::version_compare(version, "3.5") >= 0);



    data.registerOption( m_dialog->pf_check_shadowing,fwopt, "check_shading");
    data.registerOption( m_dialog->pf_preserve_group_names, fwopt,
                         "preserve_group_names");
    data.registerOption( m_dialog->pf_ignore_empty_groups,fwopt,
                         "ignore_empty_groups");
//    data.registerOption( pf_use_tables, fwopt, "use_tables");
    data.registerOption( m_dialog->pf_accept_new_tcp_with_no_syn,fwopt, "accept_new_tcp_with_no_syn");
    data.registerOption( m_dialog->pf_modulate_state,fwopt, "pf_modulate_state");

    data.registerOption( m_dialog->pf_scrub_random_id,fwopt, "pf_scrub_random_id");
    data.registerOption( m_dialog->pf_do_scrub,fwopt, "pf_do_scrub");

// radio buttons

    // the following pf_scrub options are available in PF <= 4.5
    data.registerOption( m_dialog->pf_scrub_reassemble, fwopt,
                         "pf_scrub_reassemble");
    data.registerOption( m_dialog->pf_scrub_fragm_crop, fwopt,
                         "pf_scrub_fragm_crop");
    data.registerOption( m_dialog->pf_scrub_fragm_drop_ovl, fwopt,
                         "pf_scrub_fragm_drop_ovl");
    // pf_scrub_reassemble_tcp is available in all versions
    data.registerOption( m_dialog->pf_scrub_reassemble_tcp, fwopt,
                         "pf_scrub_reassemble_tcp");

    data.registerOption( m_dialog->pf_scrub_use_minttl, fwopt,
                         "pf_scrub_use_minttl");
    data.registerOption( m_dialog->pf_scrub_use_maxmss, fwopt,
                         "pf_scrub_use_maxmss");
    data.registerOption( m_dialog->pf_scrub_maxmss,fwopt, "pf_scrub_maxmss");
    data.registerOption( m_dialog->pf_scrub_minttl,fwopt, "pf_scrub_minttl");
    data.registerOption( m_dialog->pf_scrub_no_df,fwopt, "pf_scrub_no_df");
    data.registerOption( m_dialog->pf_fw_dir,fwopt, "firewall_dir");
    data.registerOption( m_dialog->pf_user,fwopt, "admUser");
    data.registerOption( m_dialog->altAddress,fwopt, "altAddress");
    data.registerOption( m_dialog->sshArgs, fwopt, "sshArgs");
    data.registerOption( m_dialog->scpArgs, fwopt, "scpArgs");
    data.registerOption( m_dialog->activationCmd, fwopt, "activationCmd");

    data.registerOption( m_dialog->pf_manage_virtual_addr, fwopt,
                         "manage_virtual_addr");
    data.registerOption( m_dialog->pf_configure_interfaces, fwopt,
                         "configure_interfaces");
    data.registerOption( m_dialog->pf_configure_carp_interfaces, fwopt,
                         "configure_carp_interfaces");
    data.registerOption( m_dialog->pf_configure_pfsync_interfaces, fwopt,
                         "configure_pfsync_interfaces");
    data.registerOption( m_dialog->pf_configure_vlan_interfaces, fwopt,
                         "configure_vlan_interfaces");
    data.registerOption( m_dialog->pf_configure_bridge_interfaces, fwopt,
                         "configure_bridge_interfaces");

    data.registerOption( m_dialog->pf_debug,fwopt, "debug");
    data.registerOption( m_dialog->pf_flush_states, fwopt, "pf_flush_states");

    data.registerOption( m_dialog->compiler,fwopt, "compiler");
    data.registerOption( m_dialog->compilerArgs,fwopt, "cmdline");

    data.registerOption( m_dialog->generateShellScript, fwopt,
                         "generate_shell_script");
    data.registerOption( m_dialog->generateRcConfFile, fwopt,
                         "generate_rc_conf_file");

    data.registerOption( m_dialog->outputFileName, fwopt, "output_file");
    data.registerOption( m_dialog->confFileName, fwopt, "conf1_file");

    data.registerOption( m_dialog->fileNameOnFw, fwopt,
                         "script_name_on_firewall");
    data.registerOption( m_dialog->confFileNameOnFw, fwopt,
                         "conf_file_name_on_firewall");

    data.registerOption( m_dialog->mgmt_ssh,fwopt, "mgmt_ssh");
    data.registerOption( m_dialog->mgmt_addr,fwopt, "mgmt_addr");

    data.registerOption( m_dialog->pf_set_tcp_first, fwopt, "pf_set_tcp_first");
    data.registerOption( m_dialog->pf_tcp_first, fwopt, "pf_tcp_first");
    data.registerOption( m_dialog->pf_set_tcp_opening, fwopt,
                         "pf_set_tcp_opening");
    data.registerOption( m_dialog->pf_tcp_opening, fwopt, "pf_tcp_opening");
    data.registerOption( m_dialog->pf_set_tcp_established, fwopt,
                         "pf_set_tcp_established");
    data.registerOption( m_dialog->pf_tcp_established, fwopt,
                         "pf_tcp_established");
    data.registerOption( m_dialog->pf_set_tcp_closing, fwopt,
                         "pf_set_tcp_closing");
    data.registerOption( m_dialog->pf_tcp_closing, fwopt, "pf_tcp_closing");
    data.registerOption( m_dialog->pf_set_tcp_finwait, fwopt,
                         "pf_set_tcp_finwait");
    data.registerOption( m_dialog->pf_tcp_finwait, fwopt,
                         "pf_tcp_finwait");
    data.registerOption( m_dialog->pf_set_tcp_closed, fwopt,
                         "pf_set_tcp_closed");
    data.registerOption( m_dialog->pf_tcp_closed, fwopt,
                         "pf_tcp_closed");
    data.registerOption( m_dialog->pf_set_udp_first, fwopt,
                         "pf_set_udp_first");
    data.registerOption( m_dialog->pf_udp_first, fwopt,
                         "pf_udp_first");
    data.registerOption( m_dialog->pf_set_udp_single, fwopt,
                         "pf_set_udp_single");
    data.registerOption( m_dialog->pf_udp_single, fwopt, "pf_udp_single");
    data.registerOption( m_dialog->pf_set_udp_multiple, fwopt,
                         "pf_set_udp_multiple");
    data.registerOption( m_dialog->pf_udp_multiple, fwopt, "pf_udp_multiple");
    data.registerOption( m_dialog->pf_set_icmp_first, fwopt,
                         "pf_set_icmp_first");
    data.registerOption( m_dialog->pf_icmp_first, fwopt, "pf_icmp_first");
    data.registerOption( m_dialog->pf_set_icmp_error, fwopt,
                         "pf_set_icmp_error");
    data.registerOption( m_dialog->pf_icmp_error, fwopt, "pf_icmp_error");
    data.registerOption( m_dialog->pf_set_other_first, fwopt,
                         "pf_set_other_first");
    data.registerOption( m_dialog->pf_other_first, fwopt, "pf_other_first");
    data.registerOption( m_dialog->pf_set_other_single, fwopt,
                         "pf_set_other_single");
    data.registerOption( m_dialog->pf_other_single, fwopt, "pf_other_single");
    data.registerOption( m_dialog->pf_set_other_multiple, fwopt,
                         "pf_set_other_multiple");
    data.registerOption( m_dialog->pf_other_multiple, fwopt,
                         "pf_other_multiple");

    data.registerOption( m_dialog->pf_set_adaptive, fwopt,
                         "pf_set_adaptive");
    data.registerOption( m_dialog->pf_adaptive_start, fwopt,
                         "pf_adaptive_start");
    data.registerOption( m_dialog->pf_adaptive_end, fwopt,
                         "pf_adaptive_end");

    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();

    m_dialog->installScript->setText(     pis->getCommand().c_str());
    m_dialog->installScriptArgs->setText( pis->getArguments().c_str());

/* page "Prolog/Epilog" */

    QStringList prologPlaces_pf;
    prologPlaces_pf.push_back(QObject::tr("in the activation shell script"));
    prologPlaces_pf.push_back("fw_file");

    prologPlaces_pf.push_back(QObject::tr("in the pf rule file, at the very top"));
    prologPlaces_pf.push_back("pf_file_top");

    prologPlaces_pf.push_back(QObject::tr("in the pf rule file, after set comamnds"));
    prologPlaces_pf.push_back("pf_file_after_set");

    prologPlaces_pf.push_back(QObject::tr("in the pf rule file, after scrub comamnds"));
    prologPlaces_pf.push_back("pf_file_after_scrub");

    prologPlaces_pf.push_back(QObject::tr("in the pf rule file, after table definitions"));
    prologPlaces_pf.push_back("pf_file_after_tables");

    m_dialog->prologPlace->clear();
    m_dialog->prologPlace->addItems(getScreenNames(prologPlaces_pf));
    data.registerOption( m_dialog->prologPlace, fwopt, "prolog_place",
                         prologPlaces_pf);

    data.registerOption( m_dialog->prolog_script, fwopt, "prolog_script");
    data.registerOption( m_dialog->epilog_script, fwopt, "epilog_script");



    data.loadAll();

    doScrubToggled();
    ltToggled();

    m_dialog->tabWidget->setCurrentIndex(0);
}

pfAdvancedDialog::~pfAdvancedDialog()
{
    delete m_dialog;
}

void pfAdvancedDialog::doScrubToggled()
{
    string version = obj->getStr("version");

    bool f_old_reassemble = m_dialog->pf_do_scrub->isChecked();
    bool f_reassemble_tcp = f_old_reassemble;
    if (XMLTools::version_compare(version, "4.6")>=0)
        f_old_reassemble = false;

    m_dialog->pf_scrub_reassemble->setEnabled(f_old_reassemble);
    m_dialog->pf_scrub_fragm_crop->setEnabled(f_old_reassemble);
    m_dialog->pf_scrub_fragm_drop_ovl->setEnabled(f_old_reassemble);
    m_dialog->pf_scrub_reassemble_tcp->setEnabled(f_reassemble_tcp);

    if (!m_dialog->pf_scrub_reassemble->isChecked() &&
        !m_dialog->pf_scrub_fragm_crop->isChecked() &&
        !m_dialog->pf_scrub_fragm_drop_ovl->isChecked() &&
        !m_dialog->pf_scrub_reassemble_tcp->isChecked())
    {
        m_dialog->pf_scrub_reassemble_tcp->setChecked(true);
    }
}

void pfAdvancedDialog::ltToggled()
{
    m_dialog->pf_limit_frags->setEnabled( m_dialog->pf_do_limit_frags->isChecked());
    m_dialog->pf_limit_states->setEnabled( m_dialog->pf_do_limit_states->isChecked());
    m_dialog->pf_limit_src_nodes->setEnabled( m_dialog->pf_do_limit_src_nodes->isChecked());
    m_dialog->pf_limit_tables->setEnabled( m_dialog->pf_do_limit_tables->isChecked());
    m_dialog->pf_limit_table_entries->setEnabled( m_dialog->pf_do_limit_table_entries->isChecked());

    m_dialog->pf_timeout_interval->setEnabled( m_dialog->pf_do_timeout_interval->isChecked());
    m_dialog->pf_timeout_frag->setEnabled( m_dialog->pf_do_timeout_frag->isChecked());

    m_dialog->pf_tcp_first->setEnabled( m_dialog->pf_set_tcp_first->isChecked());
    m_dialog->pf_tcp_opening->setEnabled( m_dialog->pf_set_tcp_opening->isChecked());
    m_dialog->pf_tcp_established->setEnabled( m_dialog->pf_set_tcp_established->isChecked());
    m_dialog->pf_tcp_closing->setEnabled( m_dialog->pf_set_tcp_closing->isChecked());
    m_dialog->pf_tcp_finwait->setEnabled( m_dialog->pf_set_tcp_finwait->isChecked());
    m_dialog->pf_tcp_closed->setEnabled( m_dialog->pf_set_tcp_closed->isChecked());
    m_dialog->pf_udp_first->setEnabled( m_dialog->pf_set_udp_first->isChecked());
    m_dialog->pf_udp_single->setEnabled( m_dialog->pf_set_udp_single->isChecked());
    m_dialog->pf_udp_multiple->setEnabled( m_dialog->pf_set_udp_multiple->isChecked());
    m_dialog->pf_icmp_first->setEnabled( m_dialog->pf_set_icmp_first->isChecked());
    m_dialog->pf_icmp_error->setEnabled( m_dialog->pf_set_icmp_error->isChecked());
    m_dialog->pf_other_first->setEnabled( m_dialog->pf_set_other_first->isChecked());
    m_dialog->pf_other_single->setEnabled( m_dialog->pf_set_other_single->isChecked());
    m_dialog->pf_other_multiple->setEnabled( m_dialog->pf_set_other_multiple->isChecked());

    m_dialog->pf_adaptive_start->setEnabled( m_dialog->pf_set_adaptive->isChecked());
    m_dialog->pf_adaptive_end->setEnabled( m_dialog->pf_set_adaptive->isChecked());
}

/*
 * store all data in the object
 */
void pfAdvancedDialog::accept()
{
    ProjectPanel *project = mw->activeProject();
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(project, obj));

    // new_state  is a copy of the fw object
    FWObject* new_state = cmd->getNewState();
    FWOptions* fwoptions = Firewall::cast(new_state)->getOptionsObject();
    assert(fwoptions!=nullptr);

    Management *mgmt = (Firewall::cast(new_state))->getManagementObject();
    assert(mgmt!=nullptr);

    data.saveAll(fwoptions);

    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();
    pis->setCommand( m_dialog->installScript->text().toLatin1().constData());
    pis->setArguments( m_dialog->installScriptArgs->text().toLatin1().constData());

    if (!cmd->getOldState()->cmp(new_state, true))
        project->undoStack->push(cmd.release());
    
    QDialog::accept();
}

void pfAdvancedDialog::reject()
{
    QDialog::reject();
}

void pfAdvancedDialog::editProlog()
{
    SimpleTextEditor edt(this,
                         m_dialog->prolog_script->toPlainText(),
                          true, tr( "Script Editor"));
    if ( edt.exec() == QDialog::Accepted)
        m_dialog->prolog_script->setText( edt.text());
}

void pfAdvancedDialog::editEpilog()
{
    SimpleTextEditor edt(this,
                         m_dialog->epilog_script->toPlainText(),
                          true, tr( "Script Editor"));
    if ( edt.exec() == QDialog::Accepted)
        m_dialog->epilog_script->setText( edt.text());
}

void pfAdvancedDialog::help()
{
    QString tab_title = m_dialog->tabWidget->tabText(
        m_dialog->tabWidget->currentIndex());
    QString anchor = tab_title.replace('/', '-').replace(' ', '-').toLower();
    Help *h = Help::getHelpWindow(this);
    h->setName("Firewall platform: pf");
    h->setSource(QUrl("pfAdvancedDialog.html#" + anchor));
    h->raise();
    h->show();
}




