/*
 * secuwallAdvancedDialog.cpp - secuwall advanced host OS dialog implementation
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#include "global.h"
#include "platforms.h"

#include "secuwallAdvancedDialog.h"
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
#include <QUndoStack>

using namespace std;
using namespace libfwbuilder;

secuwallAdvancedDialog::~secuwallAdvancedDialog()
{
    delete m_dialog;
}

secuwallAdvancedDialog::secuwallAdvancedDialog(QWidget *parent, FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::secuwallAdvancedDialog_q;
    m_dialog->setupUi(this);

    obj=o;
    QStringList slm;

    string platform = obj->getStr("platform");
    string description = Resources::platform_res[platform]->
        getResourceStr("/FWBuilderResources/Target/description");
    setWindowTitle(QObject::tr("%1 advanced settings").arg(description.c_str()));

    FWOptions *fwoptions=(Firewall::cast(obj))->getOptionsObject();
    assert(fwoptions!=nullptr);

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=nullptr);

    data.registerOption(m_dialog->logTCPseq, fwoptions, "log_tcp_seq");
    data.registerOption(m_dialog->logTCPopt, fwoptions, "log_tcp_opt");
    data.registerOption(m_dialog->logIPopt, fwoptions, "log_ip_opt");
    data.registerOption(m_dialog->logNumsyslog, fwoptions,
                        "use_numeric_log_levels");

    slm = getLogLevels(obj->getStr("platform").c_str());
    m_dialog->logLevel->clear();
    m_dialog->logLevel->addItems(getScreenNames(slm));
    data.registerOption(m_dialog-> logLevel, fwoptions, "log_level", slm);

    data.registerOption(m_dialog->useULOG, fwoptions, "use_ULOG");
    data.registerOption(m_dialog->cprange, fwoptions, "ulog_cprange");
    data.registerOption(m_dialog->qthreshold, fwoptions, "ulog_qthreshold");
    data.registerOption(m_dialog->nlgroup, fwoptions, "ulog_nlgroup");
    data.registerOption(m_dialog->logprefix, fwoptions, "log_prefix");

    slm=getLimitSuffixes(obj->getStr("platform").c_str());
    m_dialog->logLimitSuffix->clear();
    m_dialog->logLimitSuffix->addItems(getScreenNames(slm));
    data.registerOption(m_dialog-> logLimitSuffix, fwoptions,
                        "limit_suffix", slm);

    data.registerOption(m_dialog->logLimitVal, fwoptions, "limit_value");
    data.registerOption(m_dialog->logAll, fwoptions, "log_all");
    data.registerOption(m_dialog->compiler, fwoptions, "compiler");
    data.registerOption(m_dialog->compilerArgs, fwoptions, "cmdline");
    data.registerOption(m_dialog->assumeFwIsPartOfAny,
                        fwoptions, "firewall_is_part_of_any_and_networks");
    data.registerOption(m_dialog->acceptSessions,
                        fwoptions, "accept_new_tcp_with_no_syn");
    data.registerOption(m_dialog->dropInvalid, fwoptions, "drop_invalid");
    data.registerOption(m_dialog->logInvalid, fwoptions, "log_invalid");
    data.registerOption(m_dialog->acceptESTBeforeFirst, fwoptions,
                        "accept_established");
    data.registerOption(m_dialog->bridge, fwoptions, "bridging_fw");
    data.registerOption(m_dialog->shadowing, fwoptions, "check_shading");
    data.registerOption(m_dialog->emptyGroups, fwoptions,
                        "ignore_empty_groups");
    data.registerOption(m_dialog->localNAT, fwoptions, "local_nat");
    data.registerOption(m_dialog->clampMSStoMTU, fwoptions, "clamp_mss_to_mtu");

    slm = getActionsOnReject(obj->getStr("platform").c_str());
    m_dialog->actionOnReject->clear();
    m_dialog->actionOnReject->addItems(getScreenNames(slm));
    data.registerOption(m_dialog-> actionOnReject,
                         fwoptions,"action_on_reject", slm);

    data.registerOption(m_dialog->mgmt_ssh, fwoptions, "mgmt_ssh");
    data.registerOption(m_dialog->mgmt_addr, fwoptions, "mgmt_addr");
    data.registerOption(m_dialog->add_mgmt_ssh_rule_when_stoped,
                        fwoptions, "add_mgmt_ssh_rule_when_stoped");
    data.registerOption(m_dialog->addVirtualsforNAT,
                        fwoptions, "manage_virtual_addr");

    data.registerOption(m_dialog->configureInterfaces,
                        fwoptions, "configure_interfaces");

    data.registerOption(m_dialog->iptDebug, fwoptions, "debug");
    data.registerOption(m_dialog->verifyInterfaces, fwoptions, "verify_interfaces");
    data.registerOption(m_dialog->allowReboot, fwoptions, "allow_reboot");
    data.registerOption(m_dialog->iptablesRestoreActivation,
                        fwoptions, "use_iptables_restore");
    data.registerOption(m_dialog->altAddress, fwoptions, "altAddress");
    data.registerOption(m_dialog->sshArgs, fwoptions, "sshArgs");
    data.registerOption(m_dialog->scpArgs, fwoptions, "scpArgs");
    data.registerOption(m_dialog->activationCmd, fwoptions, "activationCmd");

    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();

    m_dialog->installScript->setText(pis->getCommand().c_str());
    m_dialog->installScriptArgs->setText(pis->getArguments().c_str());

    /* page "Prolog/Epilog" */

    data.registerOption(m_dialog->prolog_script, fwoptions,
                        "prolog_script");

    QStringList prologPlaces_ipt;
    prologPlaces_ipt.push_back(QObject::tr("on top of the script"));
    prologPlaces_ipt.push_back("top");
    prologPlaces_ipt.push_back(QObject::tr("after interface configuration"));
    prologPlaces_ipt.push_back("after_interfaces");

    // bug #2820840: can't put prolog "after policy reset" if iptables-restore
    if (!fwoptions->getBool("use_iptables_restore"))
    {
        prologPlaces_ipt.push_back(QObject::tr("after policy reset"));
        prologPlaces_ipt.push_back("after_flush");
    }

    m_dialog->prologPlace->clear();
    m_dialog->prologPlace->addItems(getScreenNames(prologPlaces_ipt));
    data.registerOption(m_dialog-> prologPlace, fwoptions,
                        "prolog_place", prologPlaces_ipt);

    data.registerOption(m_dialog->epilog_script, fwoptions,
                        "epilog_script");

    data.loadAll();

    /* Now set sane values after loading data */
    /* secuwall supports currently only LOG, not ULOG */
    m_dialog->useLOG->setChecked(true);
    switchLOG_ULOG();
    m_dialog->useULOG->setEnabled(false);

    m_dialog->tabWidget->setCurrentIndex(0);
}

void secuwallAdvancedDialog::switchLOG_ULOG()
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
void secuwallAdvancedDialog::accept()
{
    ProjectPanel *project = mw->activeProject();
    std::unique_ptr<FWCmdChange> cmd(new FWCmdChange(project, obj));

    // new_state  is a copy of the fw object
    FWObject* new_state = cmd->getNewState();
    FWOptions* fwoptions = Firewall::cast(new_state)->getOptionsObject();
    assert(fwoptions!=nullptr);

    Management *mgmt = (Firewall::cast(new_state))->getManagementObject();
    assert(mgmt!=nullptr);

    data.saveAll(fwoptions);

    /*********************  data for fwbd and install script **************/
    PolicyInstallScript *pis = mgmt->getPolicyInstallScript();

    // find first interface marked as "management"
    const InetAddr *mgmt_addr = Firewall::cast(obj)->getManagementAddress();
    if (mgmt_addr)
    {
        mgmt->setAddress(*mgmt_addr);
    }

    pis->setCommand(m_dialog->installScript->text().toLatin1().constData());
    pis->setArguments(m_dialog->installScriptArgs->text().toLatin1().constData());

    if (!cmd->getOldState()->cmp(new_state, true))
    {
        project->undoStack->push(cmd.release());
    }

    QDialog::accept();
}

void secuwallAdvancedDialog::reject()
{
    QDialog::reject();
}

void secuwallAdvancedDialog::editProlog()
{
    SimpleTextEditor edt(this,
                         m_dialog->prolog_script->toPlainText(),
                         true,
                         tr("Script Editor" ));
    if (edt.exec() == QDialog::Accepted)
    {
        m_dialog->prolog_script->setText(edt.text());
    }
}

void secuwallAdvancedDialog::editEpilog()
{
    SimpleTextEditor edt(this,
                         m_dialog->epilog_script->toPlainText(),
                         true,
                         tr("Script Editor" ));
    if (edt.exec() == QDialog::Accepted)
    {
        m_dialog->epilog_script->setText(edt.text());
    }
}

void secuwallAdvancedDialog::help()
{
    QString tab_title = m_dialog->tabWidget->tabText(
            m_dialog->tabWidget->currentIndex());
    QString anchor = tab_title.replace('/', '-').replace(' ', '-').toLower();
    Help *h = Help::getHelpWindow(this);
    h->setName("Firewall platform: iptables");
    h->setSource(QUrl("secuwallAdvancedDialog.html#" + anchor));
    h->show();
    h->raise();
}

