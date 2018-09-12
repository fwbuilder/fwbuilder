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

#include "FirewallInstallerUnx.h"
#include "instDialog.h"
#include "SSHUnx.h"
#include "FWWindow.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"

#include <qpushbutton.h>
#include <qfile.h>
#include <qfileinfo.h>

#ifndef _WIN32
#  include <unistd.h>     // for access(2) and getdomainname
#endif

#include <errno.h>
#include <iostream>
#include <fcntl.h>

#include <QTextCodec>
#include <QTextStream>
#include <QTimer>
#include <QMessageBox>
#include <QDir>
#include <QtDebug>


using namespace std;
using namespace libfwbuilder;


bool FirewallInstallerUnx::packInstallJobsList(Firewall* fw)
{
    if (fwbdebug)
    {
        qDebug() << "FirewallInstallerUnx::packInstallJobList";
        qDebug() << "cnf->user=" << cnf->user;
    }

    job_list.clear();

    inst_dlg->addToLog(QString("Installation plan:\n"));

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

/* read manifest from the conf file */

    if (fwbdebug)
        qDebug("FirewallInstaller::packInstallJobsList read manifest from %s",
               cnf->script.toLatin1().constData());

/*
 * Note that if output file is specified in firewall settings dialog,
 * it can be an absolute path. In this case compiler puts additional
 * generated files (if any) in the same directory. The manifest in the
 * .fw file does not specify directory path so that the .fw file and
 * all additional files can be moved together someplace else. We take
 * dir path from the .fw file and if it is not empty, assume that all
 * other files are located there as well.
 */
    // compilers always write file names into manifest in Utf8
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Utf8"));

    //key: local_file_name  val: remote_file_name
    QMap<QString,QString> all_files;

    // readManifest() modifies cnf !
    if (readManifest(cnf->script, &all_files))
    {
        QMap<QString, QString>::iterator it;
        for (it=all_files.begin(); it!=all_files.end(); ++it)
        {
            QString local_name = it.key();
            QString remote_name = it.value();

            job_list.push_back(instJob(COPY_FILE, local_name, remote_name));
            inst_dlg->addToLog(QString("Copy file: %1 --> %2\n")
                               .arg(local_name)
                               .arg(remote_name).toLatin1().constData());
        }
    } else
    {
        inst_dlg->opError(fw);
        return false;
    }

    if (job_list.size()==0)
    {
        QMessageBox::critical(
            inst_dlg, "Firewall Builder",
            tr("Incorrect manifest format in generated script. "
               "Line with \"*\" is missing, can not find any files "
               "to copy to the firewall.\n%1").arg(cnf->script),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
        return false;
    }

    if (cnf->copyFWB)
    {
        QString dest_dir = getDestinationDir(cnf->fwdir);
        QFileInfo fwbfile_base(cnf->fwbfile);
        job_list.push_back(instJob(
                               COPY_FILE,
                               fwbfile_base.fileName(),
                               dest_dir));
        inst_dlg->addToLog(QString("Copy data file: %1 --> %2\n")
                           .arg(fwbfile_base.fileName())
                           .arg(dest_dir).toLatin1().constData());
    }

    QString cmd = getActivationCmd();

    job_list.push_back(instJob(ACTIVATE_POLICY, cmd, ""));
    inst_dlg->addToLog(QString("Run script %1\n").arg(cmd));
    inst_dlg->addToLog(QString("\n"));
    return true;
}

// ************************************************************************

void FirewallInstallerUnx::activatePolicy(const QString &cmd, const QString&)
{
    executeSession(cmd);
}

void FirewallInstallerUnx::executeSession(const QString &cmd)
{
    QStringList args;
    packSSHArgs(args);
    args.push_back( cmd );
    if (cnf->verbose) inst_dlg->displayCommand(args);

    inst_dlg->enableStopButton();

    qApp->processEvents();

    runSSHSession( new SSHUnx(inst_dlg,
                              cnf->fwobj->getName().c_str(),
                              args,
                              cnf->pwd,
                              "",
                              list<string>()), false );
}

