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

#include "../../config.h"
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


using namespace std;
using namespace libfwbuilder;

bool FirewallInstallerUnx::packInstallJobsList(Firewall* fw)
{
    if (fwbdebug) qDebug("FirewallInstallerUnx::packInstallJobList");

    job_list.clear();

    Management *mgmt = cnf->fwobj->getManagementObject();
    assert(mgmt!=NULL);
    PolicyInstallScript *pis = mgmt->getPolicyInstallScript();
    if (pis->getCommand()!="")
    {
        job_list.push_back(instJob(ACTIVATE_POLICY, ""));
        return true;
    }

    if (cnf->copyFWB)
    {
        QFileInfo fwbfile_base(cnf->fwbfile);

        if (fwbdebug)
            qDebug( QString("Will copy data file: %1").arg(
                        fwbfile_base.fileName()).toAscii().constData());

        job_list.push_back(instJob(COPY_FILE, fwbfile_base.fileName()));
    }

/* read manifest from the conf file */

    if (fwbdebug)
        qDebug("FirewallInstaller::packInstallJobsList read manifest from %s",
               cnf->conffile.toAscii().constData());

/*
 * Note that if output file is specified in firewall settings dialog,
 * it can be an absolute path. In this case compiler puts additional
 * generated files (if any) in the same directory. The manifest in the
 * .fw file does not specify directory path so that the .fw file and
 * all additional files can be moved together someplace else. We take
 * dir path from the .fw file and if it is not empty, assume that all
 * other files are located there as well.
 */
    QFileInfo cnf_file_info(cnf->conffile);
    QString dir_path = "";
    if (cnf_file_info.isAbsolute())
        dir_path = cnf_file_info.dir().path() + "/";

    // compilers always write file names into manifest in Utf8
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Utf8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Utf8"));

    QFile cf;
    int fd = open(cnf->conffile.toAscii().constData(), O_RDONLY);
    if (cf.open(fd, QIODevice::ReadOnly ))
    {
        QTextStream stream(&cf);
        QString line;
        while (!stream.atEnd())
        {
            line = stream.readLine();
            int pos = -1;
            if ( (pos=line.indexOf(MANIFEST_MARKER))!=-1 )
            {
                int n = pos + QString(MANIFEST_MARKER).length();
                QString conf_file = line.mid(n+2);
                job_list.push_back(instJob(COPY_FILE, dir_path + conf_file));

                if (fwbdebug)
                    qDebug("FirewallInstaller: adding %c %s",
                           line[n].toLatin1(),
                           line.mid(n+2).toAscii().constData());
            }
            line = "";
        }
        cf.close();
    } else
    {
        QMessageBox::critical(
            inst_dlg, "Firewall Builder",
            tr("Generated script file %1 not found.").arg(cnf->conffile),
            tr("&Continue") );
        inst_dlg->opError(fw);
        return false;
    }

    if (job_list.size()==0)
        job_list.push_back(instJob(COPY_FILE, cnf->conffile));

    job_list.push_back(instJob(ACTIVATE_POLICY, ""));
    return true;
}

// ************************************************************************

void FirewallInstallerUnx::activatePolicy()
{
    Management *mgmt = cnf->fwobj->getManagementObject();
    assert(mgmt!=NULL);
    PolicyInstallScript *pis = mgmt->getPolicyInstallScript();
    if (pis->getCommand()=="" )
        executeSession(getActivationCmd());
    else
        executeInstallScript();
}

void FirewallInstallerUnx::executeInstallScript()
{
    Management *mgmt = cnf->fwobj->getManagementObject();
    assert(mgmt!=NULL);
    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();

    QString wdir = getFileDir( mw->getRCS()->getFileName() );
    QStringList args;
    args.push_back(pis->getCommand().c_str());

    QString qs = pis->getArguments().c_str();
    args += qs.split(" ", QString::SkipEmptyParts);

    args.push_back("-f");
    args.push_back(mw->db()->getFileName().c_str());

    if (wdir!="")
    {
        args.push_back("-d");
        args.push_back(wdir);
    }
    args.push_back(cnf->fwobj->getName().c_str());

    if (cnf->verbose) inst_dlg->displayCommand(args);
    qApp->processEvents();

    executeCommand(args);
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

// ************************************************************************

void FirewallInstallerUnx::copyFile(const QString &file_name)
{
    QString platform = cnf->fwobj->getStr("platform").c_str();

//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("latin1"));

    QStringList args;
    packSCPArgs(file_name, args);

    QString file_with_path = getFullPath(file_name);
    inst_dlg->addToLog( tr("Copying %1 -> %2:%3\n")
                        .arg(QString::fromUtf8(file_with_path.toAscii().constData()))
                        .arg(cnf->maddr).arg(cnf->fwdir));

    if (cnf->verbose) inst_dlg->displayCommand(args);
    qApp->processEvents();

    // Need session for scp copy because we need to enter password
    runSSHSession( new SSHUnx(inst_dlg,
                              cnf->fwobj->getName().c_str(),
                              args,
                              cnf->pwd,
                              "",
                              list<string>()), true );
}

// ************************************************************************

void FirewallInstallerUnx::executeCommand(const QString &cmd)
{
    QStringList args;
    packSSHArgs(args);
    args.push_back( cmd );
    if (cnf->verbose) inst_dlg->displayCommand(args);
    qApp->processEvents();

    executeCommand(args);
}

// ************************************************************************

/*
 * All other methods operate with SSHSession objects because they are
 * interactive (even if only to enter password). We do not need
 * interactivity to run single command so here we use QProcess instead
 * of SSHSession.
 */
void FirewallInstallerUnx::executeCommand(QStringList &args)
{
    connect(&proc, SIGNAL(readyReadStandardOutput()),
            inst_dlg, SLOT(readFromStdout()) );

    // even though we set channel mode to "merged", QProcess
    // seems to not merge them on windows.
    proc.setProcessChannelMode(QProcess::MergedChannels);

    // set codecs so that command line parameters can be encoded
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Utf8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Utf8"));

    QString path = args.at(0);
    args.pop_front();

    proc.disconnect(SIGNAL(finished(int,QProcess::ExitStatus)));
    connect(&proc, SIGNAL(finished(int,QProcess::ExitStatus)),
            inst_dlg, SLOT(installerFinished(int,QProcess::ExitStatus)) );
    inst_dlg->enableStopButton();

    proc.start(path, args);

    if ( !proc.waitForStarted() )
    {
        inst_dlg->addToLog(tr("Error: Failed to start program:"));
        inst_dlg->addToLog(path);
        inst_dlg->opError(cnf->fwobj);
        QTimer::singleShot( 0, inst_dlg, SLOT(mainLoopInstall()));
        return;
    }
    args.push_front(path);
}

