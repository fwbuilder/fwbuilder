/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

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
#include "utils.h"
#include "utils_no_qt.h"

#include "FirewallInstallerCisco.h"
#include "instDialog.h"
#include "SSHPIX.h"
#include "SSHIOS.h"
#include "SSHNXOS.h"
#include "Configlet.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/XMLTools.h"

#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QtDebug>

using namespace std;
using namespace libfwbuilder;


FirewallInstallerCisco::FirewallInstallerCisco(instDialog *_dlg,
                                               instConf *_cnf, const QString &_p):
    FirewallInstaller(_dlg, _cnf, _p)
{
//     string platform = cnf->fwobj->getStr("platform");
//     if (cnf->fwdir.isEmpty())
//     {
//         if (platform=="nxosacl") cnf->fwdir = "volatile:";
//         else cnf->fwdir = "flash:";
//     }
}

bool FirewallInstallerCisco::packInstallJobsList(Firewall*)
{
    if (fwbdebug)
        qDebug("FirewallInstallerCisco::packInstallJobList  script=%s",
               cnf->script.toLatin1().constData());
    job_list.clear();

    Management *mgmt = cnf->fwobj->getManagementObject();
    assert(mgmt!=nullptr);
    PolicyInstallScript *pis = mgmt->getPolicyInstallScript();
    if (pis->getCommand()!="")
    {
        QString cmd = pis->getCommand().c_str();
        QString args = pis->getArguments().c_str();
        job_list.push_back(
            instJob(RUN_EXTERNAL_SCRIPT, cmd, args));
        inst_dlg->addToLog(QString("Run script %1 %2\n").arg(cmd).arg(args));
        return true;
    }

    // Load configuration file early so we can abort installation if
    // it is not accessible

    // Note about option "install only acl, icmp, telnet, ssh, nat,
    // global and static" for PIX. This option used to read generated
    // config but cuts off everything before the magic comment line
    // "!################". This way, it only read object-group,
    // access-list, access-group, nat, static and global commands. It
    // skipped all interface configurations, timeouts and inspector
    // commands. It is difficult to implement now that we (can) use
    // scp to copy configuration to the firewall. We would have to
    // create temporary file with modified configuration in order to
    // do this.  To avoid hassles with temporary files, we move the
    // same function to the compiler. The checkbox moves to the
    // "script" tab of the pix advanced settings dialog and when it is on,
    // compiler generates the script with only acl, icmp, telnet, ssh
    // nat,static and global commands
    //
    // This mode of installation is not supported on IOS at all.
    
    QString ff;
    QFileInfo script_info(cnf->script);
    if (script_info.isAbsolute()) ff = cnf->script;
    else ff = cnf->wdir + "/" + cnf->script;

    QFile data(ff);
    if (data.open(QFile::ReadOnly))
    {
        QTextStream strm(&data);
        QString line;
        do
        {
            line = strm.readLine();
            config_lines.push_back(line.trimmed());
        } while (!strm.atEnd());
    } else
    {
        QMessageBox::critical(
            inst_dlg, "Firewall Builder",
            tr("Can not read generated script %1").arg(ff),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
        return false;
    }

    string platform = cnf->fwobj->getStr("platform");

    if (cnf->useSCPForRouter)
    {
        QMap<QString,QString> all_files;

        // readManifest() modifies cnf (assigns cnf->remote_script) !
        if (readManifest(cnf->script, &all_files))
        {
            QMap<QString, QString>::iterator it;
            for (it=all_files.begin(); it!=all_files.end(); ++it)
            {
                QString local_name = it.key();
                QString remote_name = it.value();
                job_list.push_back(instJob(COPY_FILE, local_name, remote_name));
            }
        }

        QString cmd = getActivationCmd();
        job_list.push_back(instJob(ACTIVATE_POLICY, cmd, ""));
    } else
    {
        job_list.push_back(instJob(ACTIVATE_POLICY, cnf->script, ""));
    }

    return true;
}

void FirewallInstallerCisco::activatePolicy(const QString&, const QString&)
{
    QStringList args;

    packSSHArgs(args);
    if (cnf->verbose) inst_dlg->displayCommand(args);

    SSHCisco *ssh_object = nullptr;
    if (cnf->fwobj->getStr("platform")=="pix" ||
        cnf->fwobj->getStr("platform")=="fwsm")
    {
        ssh_object = new SSHPIX(inst_dlg,
                                cnf->fwobj->getName().c_str(),
                                args,
                                cnf->pwd,
                                cnf->epwd,
                                list<string>());
    } else if (cnf->fwobj->getStr("platform")=="nxosacl")
    {
        ssh_object = new SSHNXOS(inst_dlg,
                                cnf->fwobj->getName().c_str(),
                                args,
                                cnf->pwd,
                                cnf->epwd,
                                list<string>());
    } else   // ios
    {
        ssh_object = new SSHIOS(inst_dlg,
                                cnf->fwobj->getName().c_str(),
                                args,
                                cnf->pwd,
                                cnf->epwd,
                                list<string>());
    }
    /*
     * TODO:
     * the structure of scriptlets (command templates) for PIX and
     * IOS is nice and generic, it uses generalized "pre_config"
     * and "post_config" hooks in SSHPIX / SSHIOS classes. Need to
     * do the same for Unix firewalls.
     */

    QString cmd = "";
    QStringList pre_config_commands;
    QStringList post_config_commands;

    string version = cnf->fwobj->getStr("version");
    bool version_lt_124 = XMLTools::version_compare(version, "12.4") < 0;
    bool version_ge_124 = XMLTools::version_compare(version, "12.4") >= 0;

    string host_os = cnf->fwobj->getStr("host_OS");
    string os_family = Resources::os_res[host_os]->
        getResourceStr("/FWBuilderResources/Target/family");

    // installer configlets should be different for each OS, but if
    // some OS can use the same script, it will be placed in the file
    // under os_family name. For example:
    // for PIX configlet is in src/res/configlets/pix_os
    // but since fwsm and pix can use the same script and fwsm_os.xml
    // declares family as "pix_os", it uses the same configlet.

    Configlet pre_config(host_os, os_family, "installer_commands_pre_config");
    pre_config.removeComments();

    // test run and rollback were deprecated in 4.2.0. On Linux, BSD
    // and PIX rollback was implemented by rebooting firewall which is
    // too heavy-handed and it did not work on BSD at all.
    pre_config.setVariable("test", false);
    pre_config.setVariable("run", true);
    pre_config.setVariable("schedule_rollback", false);
    pre_config.setVariable("cancel_rollback", false);

    pre_config.setVariable("save_standby", cnf->saveStandby);
    pre_config.setVariable("version_lt_124", version_lt_124);
    pre_config.setVariable("version_ge_124", version_ge_124);

    replaceMacrosInCommand(&pre_config);

    Configlet post_config(host_os, os_family, "installer_commands_post_config");
    post_config.removeComments();

    post_config.setVariable("test", false);
    post_config.setVariable("run", true);
    post_config.setVariable("schedule_rollback", false);
    post_config.setVariable("cancel_rollback", false);

    post_config.setVariable("save_standby", cnf->saveStandby);
    post_config.setVariable("version_lt_124", version_lt_124);
    post_config.setVariable("version_ge_124", version_ge_124);

    replaceMacrosInCommand(&post_config);

    ssh_object->loadPreConfigCommands(
        pre_config.expand().split("\n", QString::SkipEmptyParts) );

    ssh_object->loadPostConfigCommands(
        post_config.expand().split("\n", QString::SkipEmptyParts) );

    Configlet activation(host_os, os_family, "installer_commands_reg_user");
    activation.removeComments();

    replaceMacrosInCommand(&activation);

    activation.setVariable("using_scp",       cnf->useSCPForRouter);
    activation.setVariable("not_using_scp", ! cnf->useSCPForRouter);

    activation.setVariable("using_nxos_session", cnf->useNXOSSession);
    activation.setVariable("not_using_nxos_session", ! cnf->useNXOSSession);

    if ( ! cnf->useSCPForRouter)
    {
        activation.setVariable("fwbuilder_generated_configuration_lines",
                               config_lines.join("\n"));
    }

    ssh_object->loadActivationCommands(
        activation.expand().split("\n", QString::SkipEmptyParts) );

    runSSHSession(ssh_object);

    return;
}

bool FirewallInstallerCisco::readManifest(const QString &script, 
                                          QMap<QString, QString> *all_files)
{
    if (fwbdebug)
        qDebug("FirewallInstaller::readManifest");
    QString dest_dir = getDestinationDir(cnf->fwdir);
    // path returned by getDestinationDir always ends with separator
    // in case of IOS, it is ":"
    QFileInfo file_base(script);
    QString remote_file = dest_dir + file_base.fileName();
    qDebug() << "001 REMOTE FILE:" << remote_file;
    QString local_name = script;
    cnf->remote_script = remote_file;
    (*all_files)[local_name] = remote_file;
    return true;
}

QString FirewallInstallerCisco::getDestinationDir(const QString &fwdir)
{
    if (fwbdebug)
        qDebug() << "FirewallInstallerCisco::getDestinationDir:  "
                 << "fwdir=" << fwdir;

    QString dir = fwdir;
    if (!dir.endsWith(":")) return dir + ":";
    return dir;
}


