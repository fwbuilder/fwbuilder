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

#include "ipfAdvancedDialog.h"
#include "SimpleTextEditor.h"
#include "FWWindow.h"
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
#include <QUndoStack>


using namespace std;
using namespace libfwbuilder;

ipfAdvancedDialog::~ipfAdvancedDialog()
{
    delete m_dialog;
}

ipfAdvancedDialog::ipfAdvancedDialog(QWidget *parent,FWObject *o)
    : QDialog(parent)
{
    m_dialog = new Ui::ipfAdvancedDialog_q;
    m_dialog->setupUi(this);

    obj=o;
    QStringList slm;

    FWOptions *fwopt=(Firewall::cast(obj))->getOptionsObject();
    assert(fwopt!=nullptr);

    Management *mgmt=(Firewall::cast(obj))->getManagementObject();
    assert(mgmt!=nullptr);

    if (fwbdebug)
        qDebug("%s",Resources::getTargetOptionStr(
                   obj->getStr("host_OS"),"user_can_change_install_dir").c_str());

    if (!Resources::getTargetOptionBool(
            obj->getStr("host_OS"),"user_can_change_install_dir"))
    {
        m_dialog->ipf_fw_dir->setEnabled(false);
        fwopt->setStr("firewall_dir","");
    }

    m_dialog->tabWidget->setTabEnabled(6,false); //Disable tab
    data.registerOption(m_dialog->ipv4before_2, fwopt, "ipv4_6_order",
                        QStringList() <<  tr("IPv4 before IPv6") <<"ipv4_first" << tr("IPv6 before IPv4") << "ipv6_first");
    data.registerOption( m_dialog->ipf_log_or_block,fwopt, "ipf_log_or_block"    );
    data.registerOption( m_dialog->ipf_log_body,fwopt, "ipf_log_body"        );
    data.registerOption( m_dialog->ipf_check_shadowing,fwopt, "check_shading"       );
    data.registerOption( m_dialog->ipf_eliminate_duplicates,fwopt, "eliminate_duplicates");
    data.registerOption( m_dialog->ipf_accept_new_tcp_with_no_syn,fwopt, "accept_new_tcp_with_no_syn");
    data.registerOption( m_dialog->ipf_ignore_empty_groups,fwopt, "ignore_empty_groups");
    data.registerOption( m_dialog->ipf_return_icmp_as_dest,fwopt, "ipf_return_icmp_as_dest");
    data.registerOption( m_dialog->ipf_nat_raudio_proxy,fwopt, "ipf_nat_raudio_proxy");
    data.registerOption( m_dialog->ipf_nat_h323_proxy,fwopt, "ipf_nat_h323_proxy");
    data.registerOption( m_dialog->ipf_nat_ipsec_proxy,fwopt, "ipf_nat_ipsec_proxy");
    data.registerOption( m_dialog->ipf_nat_pptp_proxy,fwopt, "ipf_nat_pptp_proxy");
    data.registerOption( m_dialog->ipf_nat_irc_proxy,fwopt, "ipf_nat_irc_proxy");
    data.registerOption( m_dialog->ipf_nat_ftp_proxy,fwopt, "ipf_nat_ftp_proxy");
    data.registerOption( m_dialog->ipf_nat_rcmd_proxy,fwopt, "ipf_nat_rcmd_proxy");
    data.registerOption( m_dialog->ipf_nat_krcmd_proxy,fwopt, "ipf_nat_krcmd_proxy");
    data.registerOption( m_dialog->ipf_nat_ekshell_proxy,fwopt, "ipf_nat_ekshell_proxy");
    data.registerOption( m_dialog->ipf_fw_dir,fwopt, "firewall_dir"      );
    data.registerOption( m_dialog->ipf_user,fwopt, "admUser"           );
    data.registerOption( m_dialog->altAddress, fwopt, "altAddress");
    data.registerOption( m_dialog->sshArgs, fwopt, "sshArgs");
    data.registerOption( m_dialog->scpArgs, fwopt, "scpArgs");
    data.registerOption( m_dialog->activationCmd, fwopt, "activationCmd");

    data.registerOption( m_dialog->ipf_manage_virtual_addr,fwopt, "manage_virtual_addr");
    data.registerOption( m_dialog->ipf_configure_interfaces,fwopt, "configure_interfaces");
    data.registerOption( m_dialog->ipf_debug,fwopt, "debug"            );
    data.registerOption( m_dialog->ipf_optimize,fwopt, "optimize"  );
    data.registerOption( m_dialog->ipf_dynAddr,fwopt, "dynAddr"   );

    slm = getLogLevels( obj->getStr("platform").c_str() );
    m_dialog->logLevel->clear();
    m_dialog->logLevel->addItems( getScreenNames( slm ));
    data.registerOption( m_dialog->logLevel, fwopt, "ipf_log_level", slm);

    slm = getLogFacilities( obj->getStr("platform").c_str() );
    m_dialog->logFacility->clear();
    m_dialog->logFacility->addItems( getScreenNames( slm ));
    data.registerOption( m_dialog->logFacility, fwopt, "ipf_log_facility", slm);

    data.registerOption( m_dialog->compiler, fwopt, "compiler" );
    data.registerOption( m_dialog->compilerArgs, fwopt, "cmdline"  );
    data.registerOption( m_dialog->outputFileName, fwopt, "output_file"  );
    data.registerOption( m_dialog->fileNameOnFw, fwopt, "script_name_on_firewall");
    data.registerOption( m_dialog->ipfConfFileNameOnFw, fwopt, "ipf_conf_file_name_on_firewall");
    data.registerOption( m_dialog->natConfFileNameOnFw, fwopt, "nat_conf_file_name_on_firewall");

    slm=getActionsOnReject( obj->getStr("platform").c_str() );
    m_dialog->actionOnReject->clear();
    m_dialog->actionOnReject->addItems(getScreenNames(slm));

    data.registerOption( m_dialog->actionOnReject, fwopt, "action_on_reject",slm);
    data.registerOption( m_dialog->mgmt_ssh, fwopt, "mgmt_ssh"  );
    data.registerOption( m_dialog->mgmt_addr, fwopt, "mgmt_addr" );

    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();

    m_dialog->installScript->setText(     pis->getCommand().c_str() );
    m_dialog->installScriptArgs->setText( pis->getArguments().c_str() );

/* page "Prolog/Epilog" */
    data.registerOption( m_dialog->prolog_script, fwopt, "prolog_script"  );

    data.registerOption( m_dialog->epilog_script, fwopt, "epilog_script"  );

    data.loadAll();

    m_dialog->tabWidget->setCurrentIndex(0);
}

/*
 * store all data in the object
 */
void ipfAdvancedDialog::accept()
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
    pis->setCommand( m_dialog->installScript->text().toLatin1().constData() );
    pis->setArguments( m_dialog->installScriptArgs->text().toLatin1().constData() );

    if (!cmd->getOldState()->cmp(new_state, true))
        project->undoStack->push(cmd.release());
    
    QDialog::accept();
}

void ipfAdvancedDialog::reject()
{
    QDialog::reject();
}

void ipfAdvancedDialog::editProlog()
{
    SimpleTextEditor edt(this,
                         m_dialog->prolog_script->toPlainText(),
                          true, tr( "Script Editor" ) );
    if ( edt.exec() == QDialog::Accepted )
        m_dialog->prolog_script->setText( edt.text() );
}

void ipfAdvancedDialog::editEpilog()
{
    SimpleTextEditor edt(this,
                         m_dialog->epilog_script->toPlainText(),
                          true, tr( "Script Editor" ) );
    if ( edt.exec() == QDialog::Accepted )
        m_dialog->epilog_script->setText( edt.text() );
}



