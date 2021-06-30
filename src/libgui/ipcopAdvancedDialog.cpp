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

#include "ipcopAdvancedDialog.h"
#include "SimpleTextEditor.h"
#include "FWWindow.h"
#include "Help.h"
#include "FWCmdChange.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"

#include <memory>

#include <qcheckbox.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qstackedwidget.h>
#include <qregexp.h>
#include <qtextedit.h>

using namespace std;
using namespace libfwbuilder;

ipcopAdvancedDialog::~ipcopAdvancedDialog()
{
    delete m_dialog;
}

ipcopAdvancedDialog::ipcopAdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::ipcopAdvancedDialog_q;
    m_dialog->setupUi(this);

    obj=o;
    QStringList slm;

    /*
     * Set dialog title dynamically to reflect description set in the
     * platform resource file. This is useful because the same dialog
     * is used for ipcop, endian and oneshield platforms.
     */
    string platform = obj->getStr("platform");
    string description = Resources::platform_res[platform]->
        getResourceStr("/FWBuilderResources/Target/description");
    setWindowTitle(QObject::tr("%1 advanced settings").arg(description.c_str()));

    FWOptions *fwoptions=(Firewall::cast(obj))->getOptionsObject();
    assert(fwoptions!=nullptr);

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=nullptr);

    /*
    fwoptions->setStr("firewall_dir", "/etc/rc.d/");
    fwoptions->setStr("admUser", "admin");
    fwoptions->setStr("activationCmd", "/etc/rc.d/rc.firewall");
    fwoptions->setStr("output_file", "rc.firewall.local");
    */

    //QString s = fwoptions->getStr("ipv4_6_order")
    data.registerOption(m_dialog->ipv4before, fwoptions, "ipv4_6_order",
                        QStringList() <<  tr("IPv4 before IPv6")
                        <<"ipv4_first" << tr("IPv6 before IPv4") << "ipv6_first");

    data.registerOption(m_dialog->logTCPseq, fwoptions, "log_tcp_seq");
    data.registerOption(m_dialog->logTCPopt, fwoptions, "log_tcp_opt");
    data.registerOption(m_dialog->logIPopt, fwoptions, "log_ip_opt");
    data.registerOption(m_dialog->logNumsyslog, fwoptions,
                        "use_numeric_log_levels");

    slm = getLogLevels( platform.c_str());
    m_dialog->logLevel->clear();
    m_dialog->logLevel->addItems( getScreenNames(slm));
    data.registerOption(m_dialog-> logLevel, fwoptions, "log_level", slm);

    data.registerOption(m_dialog->useULOG, fwoptions, "use_ULOG");
    data.registerOption(m_dialog->cprange, fwoptions, "ulog_cprange");
    data.registerOption(m_dialog->qthreshold, fwoptions, "ulog_qthreshold");
    data.registerOption(m_dialog->nlgroup, fwoptions, "ulog_nlgroup");
    data.registerOption(m_dialog->logprefix, fwoptions, "log_prefix");

    slm=getLimitSuffixes( platform.c_str());
    m_dialog->logLimitSuffix->clear();
    m_dialog->logLimitSuffix->addItems(getScreenNames(slm));
    data.registerOption(m_dialog-> logLimitSuffix, fwoptions,
                        "limit_suffix", slm);

    data.registerOption(m_dialog->logLimitVal, fwoptions, "limit_value");
    data.registerOption(m_dialog->logAll, fwoptions, "log_all");
    data.registerOption(m_dialog->compiler, fwoptions, "compiler");
    data.registerOption(m_dialog->compilerArgs, fwoptions, "cmdline");
    data.registerOption(m_dialog->outputFileName, fwoptions, "output_file");
    data.registerOption(m_dialog->assumeFwIsPartOfAny,
                        fwoptions, "firewall_is_part_of_any_and_networks");
    data.registerOption(m_dialog->acceptSessions,
                        fwoptions, "accept_new_tcp_with_no_syn");
    data.registerOption(m_dialog->bridge, fwoptions, "bridging_fw");
    data.registerOption(m_dialog->shadowing, fwoptions, "check_shading");
    data.registerOption(m_dialog->emptyGroups, fwoptions,
                        "ignore_empty_groups");
    data.registerOption(m_dialog->localNAT, fwoptions, "local_nat");
    slm=getActionsOnReject( platform.c_str());
    m_dialog->actionOnReject->clear();
    m_dialog->actionOnReject->addItems(getScreenNames(slm));
    data.registerOption(m_dialog-> actionOnReject,
                         fwoptions,"action_on_reject", slm);

    data.registerOption(m_dialog->mgmt_ssh, fwoptions, "mgmt_ssh");
    data.registerOption(m_dialog->mgmt_addr, fwoptions, "mgmt_addr");
    data.registerOption(m_dialog->iptDebug, fwoptions, "debug");
    data.registerOption(m_dialog->verifyInterfaces, fwoptions, "verify_interfaces");
    data.registerOption(m_dialog->ipt_fw_dir, fwoptions, "firewall_dir");
    data.registerOption(m_dialog->ipt_user, fwoptions, "admUser");
    data.registerOption(m_dialog->altAddress, fwoptions, "altAddress");
    data.registerOption(m_dialog->sshArgs, fwoptions, "sshArgs");
    data.registerOption( m_dialog->scpArgs, fwoptions, "scpArgs");
    data.registerOption(m_dialog->activationCmd, fwoptions, "activationCmd");

    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();

    m_dialog->installScript->setText(     pis->getCommand().c_str());
    m_dialog->installScriptArgs->setText( pis->getArguments().c_str());


    /* page "Prolog/Epilog" */

    data.registerOption(m_dialog->prolog_script, fwoptions, "prolog_script");
    data.registerOption(m_dialog->epilog_script, fwoptions, "epilog_script");

    data.loadAll();
    switchLOG_ULOG();

    m_dialog->tabWidget->setCurrentIndex(0);
}

void ipcopAdvancedDialog::switchLOG_ULOG()
{
    m_dialog->useLOG->setChecked(!m_dialog->useULOG->isChecked());

    if (m_dialog->useLOG->isChecked())
        m_dialog->logTargetStack->setCurrentIndex(0);
    else
        m_dialog->logTargetStack->setCurrentIndex(1);
}
/*
 * store all data in the object
 */
void ipcopAdvancedDialog::accept()
{
    ProjectPanel *project = mw->activeProject();
    std::unique_ptr<FWCmdChange> cmd( new FWCmdChange(project, obj));

    // new_state  is a copy of the fw object
    FWObject* new_state = cmd->getNewState();
    FWOptions* fwoptions = Firewall::cast(new_state)->getOptionsObject();
    assert(fwoptions!=nullptr);

    Management *mgmt=(Firewall::cast(new_state))->getManagementObject();
    assert(mgmt!=nullptr);

    data.saveAll(fwoptions);

/*********************  data for fwbd and install script **************/
    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();

    // find first interface marked as "management"
    const InetAddr *mgmt_addr = Firewall::cast(obj)->getManagementAddress();
    if (mgmt_addr)
        mgmt->setAddress(*mgmt_addr);

    pis->setCommand( m_dialog->installScript->text().toLatin1().constData());
    pis->setArguments( m_dialog->installScriptArgs->text().toLatin1().constData());

    if (!cmd->getOldState()->cmp(new_state, true))
        project->undoStack->push(cmd.release());
    
    QDialog::accept();
}

void ipcopAdvancedDialog::reject()
{
    QDialog::reject();
}

void ipcopAdvancedDialog::editProlog()
{
    SimpleTextEditor edt(this,
                         m_dialog->prolog_script->toPlainText(),
                          true, tr( "Script Editor" ));
    if ( edt.exec() == QDialog::Accepted )
        m_dialog->prolog_script->setText( edt.text());
}

void ipcopAdvancedDialog::editEpilog()
{
    SimpleTextEditor edt(this,
                         m_dialog->epilog_script->toPlainText(),
                          true, tr( "Script Editor" ));
    if ( edt.exec() == QDialog::Accepted )
        m_dialog->epilog_script->setText( edt.text());
}

void ipcopAdvancedDialog::help()
{
    QString tab_title = m_dialog->tabWidget->tabText(
        m_dialog->tabWidget->currentIndex());
    QString anchor = tab_title.replace('/', '-').replace(' ', '-').toLower();
    Help *h = Help::getHelpWindow(this);
    h->setName("Firewall platform: IPCOP");
    h->setSource(QUrl("ipcopAdvancedDialog.html#" + anchor));
    h->raise();
    h->show();
}




