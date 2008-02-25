/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: iptAdvancedDialog.cpp,v 1.29 2007/07/07 19:29:05 vkurland Exp $

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

#include "stdio.h"

#include "config.h"
#include "global.h"
#include "platforms.h"

#include "iptAdvancedDialog.h"
#include "SimpleTextEditor.h"
#include "FWWindow.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/Resources.h"

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

iptAdvancedDialog::~iptAdvancedDialog()
{
    delete m_dialog;
}

iptAdvancedDialog::iptAdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::iptAdvancedDialog_q;
    m_dialog->setupUi(this);

    obj=o;
    QStringList slm;

    FWOptions *fwoptions=(Firewall::cast(obj))->getOptionsObject();
    assert(fwoptions!=NULL);

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=NULL);

    if (fwbdebug)
        qDebug("%s",Resources::getTargetOptionStr(
                   obj->getStr("host_OS"),"user_can_change_install_dir").c_str());

    if (!Resources::getTargetOptionBool(
            obj->getStr("host_OS"),"user_can_change_install_dir"))
    {
        m_dialog->ipt_fw_dir->setEnabled(false);
        fwoptions->setStr("firewall_dir","");
    }

    data.registerOption(m_dialog->logTCPseq,    fwoptions, "log_tcp_seq"               );
    data.registerOption(m_dialog->logTCPopt,    fwoptions, "log_tcp_opt"               );
    data.registerOption(m_dialog->logIPopt,     fwoptions, "log_ip_opt"                );
    data.registerOption(m_dialog->logNumsyslog, fwoptions, "use_numeric_log_levels"    );

    slm = getLogLevels( obj->getStr("platform").c_str() );
    m_dialog->logLevel->clear();
    m_dialog->logLevel->addItems( getScreenNames(slm));
    data.registerOption(m_dialog-> logLevel,   fwoptions,  "log_level", slm);

    data.registerOption(m_dialog->useULOG,    fwoptions, "use_ULOG"        );
    data.registerOption(m_dialog->cprange,    fwoptions, "ulog_cprange"    );
    data.registerOption(m_dialog->qthreshold, fwoptions, "ulog_qthreshold" );
    data.registerOption(m_dialog->nlgroup,    fwoptions, "ulog_nlgroup"    );
    data.registerOption(m_dialog->logprefix,  fwoptions, "log_prefix"      );

    slm=getLimitSuffixes( obj->getStr("platform").c_str() );
    m_dialog->logLimitSuffix->clear();
    m_dialog->logLimitSuffix->addItems(getScreenNames(slm));
    data.registerOption(m_dialog-> logLimitSuffix,   fwoptions,  "limit_suffix", slm);

    data.registerOption(m_dialog->logLimitVal,          fwoptions, "limit_value");
    data.registerOption(m_dialog->logAll,               fwoptions, "log_all");
    data.registerOption(m_dialog->compiler,             fwoptions, "compiler");
    data.registerOption(m_dialog->compilerArgs,         fwoptions, "cmdline");
    data.registerOption(m_dialog->outputFileName,       fwoptions, "output_file");
    data.registerOption(m_dialog->assumeFwIsPartOfAny,
                        fwoptions, "firewall_is_part_of_any_and_networks");
    data.registerOption(m_dialog->acceptSessions,
                        fwoptions, "accept_new_tcp_with_no_syn");
    data.registerOption(m_dialog->dropInvalid,          fwoptions, "drop_invalid");
    data.registerOption(m_dialog->logInvalid,           fwoptions, "log_invalid");
    data.registerOption(m_dialog->acceptESTBeforeFirst, fwoptions, "accept_established");
    data.registerOption(m_dialog->bridge,               fwoptions, "bridging_fw");
    data.registerOption(m_dialog->shadowing,            fwoptions, "check_shading");
    data.registerOption(m_dialog->emptyGroups,          fwoptions, "ignore_empty_groups");
    data.registerOption(m_dialog->localNAT,             fwoptions, "local_nat");
    data.registerOption(m_dialog->clampMSStoMTU,        fwoptions, "clamp_mss_to_mtu");
    data.registerOption(m_dialog->makeTagClassifyTerminating,
                        fwoptions, "classify_mark_terminating");
    data.registerOption(m_dialog->skipIPv6,
                        fwoptions, "no_ipv6_default_policy");
    slm=getActionsOnReject( obj->getStr("platform").c_str() );
    m_dialog->actionOnReject->clear();
    m_dialog->actionOnReject->addItems(getScreenNames(slm));
    data.registerOption(m_dialog-> actionOnReject,
                         fwoptions,"action_on_reject", slm);

    data.registerOption(m_dialog->mgmt_ssh,             fwoptions, "mgmt_ssh"    );
    data.registerOption(m_dialog->mgmt_addr,            fwoptions, "mgmt_addr"    );
    data.registerOption(m_dialog->addVirtualsforNAT,
                        fwoptions, "manage_virtual_addr"    );
    data.registerOption(m_dialog->configureInterfaces,
                        fwoptions, "configure_interfaces"    );
    data.registerOption(m_dialog->iptDebug,             fwoptions, "debug"    );
    data.registerOption(m_dialog->verifyInterfaces,     fwoptions, "verify_interfaces"    );
    data.registerOption(m_dialog->loadModules,          fwoptions, "load_modules"    );
    data.registerOption(m_dialog->iptablesRestoreActivation,
                        fwoptions, "use_iptables_restore"    );
    data.registerOption(m_dialog->ipt_fw_dir,           fwoptions, "firewall_dir"    );
    data.registerOption(m_dialog->ipt_user,             fwoptions, "admUser"    );
    data.registerOption(m_dialog->altAddress,           fwoptions, "altAddress"    );
    data.registerOption(m_dialog->sshArgs,              fwoptions, "sshArgs"    );
    data.registerOption(m_dialog->activationCmd,        fwoptions, "activationCmd"    );


    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();

    m_dialog->installScript->setText(     pis->getCommand().c_str() );
    m_dialog->installScriptArgs->setText( pis->getArguments().c_str() );


    /* page "Prolog/Epilog" */

    data.registerOption(m_dialog->prolog_script    ,fwoptions, "prolog_script"    );

    slm = getPrologPlaces( obj->getStr("platform").c_str() );
    m_dialog->prologPlace->clear();
    m_dialog->prologPlace->addItems(getScreenNames(slm));
    data.registerOption(m_dialog-> prologPlace,   fwoptions,   "prolog_place", slm);

    data.registerOption(m_dialog->epilog_script    ,fwoptions, "epilog_script"    );

    data.loadAll();
    switchLOG_ULOG();

#ifdef HAVE_LIBSSL

//    int port=fwbdm->getPort();
//    if (port==-1)
//        port= Resources::global_res->getResourceInt("/FWBuilderResources/FWBD/port");
//    mgmt_fwbd_port->set_value( port );
//
//    fillListOfCertificates();
//
//    const Key *key=fwbdm->getPublicKey();
//    if (key) mgmt_fw_key->set_text( key->getFingerprint() );
//
//    if (pis->isEnabled()) mgmt_use_install_script->set_active(true);
//    else                  mgmt_use_fwbd->set_active(true);

#else
//    mgmt_use_fwbd->set_sensitive(false);
//    mgmt_use_install_script->set_active(true);
//
//    disableFWBDoptions();
#endif

}

void iptAdvancedDialog::switchLOG_ULOG()
{
    m_dialog->useLOG->setChecked(!m_dialog->useULOG->isChecked());
    if (m_dialog->useLOG->isChecked())  m_dialog->logTargetStack->setCurrentIndex(0);
    else                      m_dialog->logTargetStack->setCurrentIndex(1);
}
/*
 * store all data in the object
 */
void iptAdvancedDialog::accept()
{
    FWOptions *fwoptions=(Firewall::cast(obj))->getOptionsObject();
    assert(fwoptions!=NULL);

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=NULL);

    data.saveAll();

/*********************  data for fwbd and install script **************/
    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();

    mgmt->setAddress( (Firewall::cast(obj))->getAddress() );

    pis->setCommand( m_dialog->installScript->text().toLatin1().constData() );
    pis->setArguments( m_dialog->installScriptArgs->text().toLatin1().constData() );

    mw->updateLastModifiedTimestampForAllFirewalls(obj);
    QDialog::accept();
}

void iptAdvancedDialog::reject()
{
    QDialog::reject();
}

void iptAdvancedDialog::editProlog()
{
    SimpleTextEditor edt(this,
                         m_dialog->prolog_script->toPlainText(),
                          true, tr( "Script Editor" ) );
    if ( edt.exec() == QDialog::Accepted )
        m_dialog->prolog_script->setText( edt.text() );
}

void iptAdvancedDialog::editEpilog()
{
    SimpleTextEditor edt(this,
                         m_dialog->epilog_script->toPlainText(),
                          true, tr( "Script Editor" ) );
    if ( edt.exec() == QDialog::Accepted )
        m_dialog->epilog_script->setText( edt.text() );
}


