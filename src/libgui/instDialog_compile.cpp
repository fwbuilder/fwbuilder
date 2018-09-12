/*

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org


                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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

#include "instDialog.h"
#include "FWBSettings.h"
#include "FWWindow.h"
#include "instOptionsDialog.h"
#include "instBatchOptionsDialog.h"
#include "events.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwcompiler/BaseCompiler.h"

#include <errno.h>
#include <iostream>

#include <QTextCodec>
#include <QTimer>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QtDebug>

using namespace std;
using namespace libfwbuilder;


bool instDialog::runCompiler(Firewall *fw)
{
    if (fwbdebug)
    {
        qDebug() << "instDialog::runCompile";
        qDebug() << "Firewall: " << fw->getName().c_str();
    }
    // store pointer to the firewall so we can use it in
    // slot compilerFinished
    cnf.fwobj = fw;

    currentSearchString = tr("Compiling rule sets for firewall: ");

    currentFirewallsBar->setValue(compile_list_initial_size - 
                                  compile_fw_list.size());
    currentProgressBar->reset();
    currentProgressBar->setFormat("%v/%m");

    QTreeWidgetItem* item = opListMapping[fw->getId()];
    assert(item!=nullptr);

    currentFWLabel->setText(QString::fromUtf8(fw->getName().c_str()));
    m_dialog->fwWorkList->scrollToItem(item);
    setInProcessState(item);
    item->setText(1, tr("Compiling ..."));
    currentLabel->setText(tr("Compiling ..."));

    qApp->processEvents();

    addToLog("\n");
    addToLog(
        QObject::tr("Compiling rule sets for firewall: %1\n").
        arg(QString::fromUtf8(fw->getName().c_str()))
    );

    QStringList args = prepareArgForCompiler(fw);
    if (args.isEmpty()) return false;

    addToLog( args.join(" ") + "\n" );

    // compilers always write file names into manifest in Utf8
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Utf8"));

    // Launch compiler in the background
    QString path = args.at(0);
    args.pop_front();

    disconnect(currentStopButton, SIGNAL(clicked()) );
    connect(currentStopButton, SIGNAL(clicked()),
            this, SLOT(stopCompile()));

    currentStopButton->setText(tr("Stop"));
    currentStopButton->setEnabled(true);

    setUpProcessToCompile();
    if (!executeCommand(path, args))
        QTimer::singleShot( 0, this, SLOT(mainLoopCompile()));

    return true;
}

void instDialog::stopCompile()
{
    if( fwbdebug) qDebug("instDialog::stopCompile");
    stopProcessFlag = true;

    disconnect(currentStopButton, SIGNAL(clicked()) );
    currentStopButton->setEnabled(false);

    proc.terminate();                                  //try to close proc.
    QTimer::singleShot( 1000, &proc, SLOT( kill() ) ); //if it doesn't respond, kill it

    blockInstallForFirewall(cnf.fwobj);

    // to terminate whole compile sequence rather than just current
    // compiler process, clear the list.
    for (list<Firewall*>::iterator i=compile_fw_list.begin();
         i!=compile_fw_list.end(); ++i)
    {
        opCancelled(*i);
        blockInstallForFirewall(*i);
    }

    compile_fw_list.clear();
}

QStringList instDialog::prepareArgForCompiler(Firewall *fw)
{
    FWOptions *fwopt = fw->getOptionsObject();
    QStringList args;

/*
 * I should be able to specify custom compiler for firewall with
 * no platform (e.g. for experiments)
 */
    string compiler = fwopt->getStr("compiler");
    if (compiler=="")
    {
        compiler=Resources::platform_res[fw->getStr("platform")]->getCompiler();
    }

    if (compiler=="")
    {
        QMessageBox::warning(
            this,"Firewall Builder",
            tr("Firewall platform is not specified in this object.\n\
Can't compile firewall policy."),
            tr("&Continue"), QString::null,QString::null,
            0, 1 );
        return args; // still empty list
    }

/*
 * On Unix compilers are installed in the standard place and are
 * accessible via PATH. On Windows and Mac they get installed in
 * unpredictable directories and need to be found
 *
 * first, check if user specified an absolute path for the compiler,
 * then check  if compiler is registsred in preferences, and if not,
 * look for it in appRootDir and if it is not there, rely on PATH
 */
#if defined(Q_OS_WIN32) ||  defined(Q_OS_MACX)

    if ( ! QFile::exists( compiler.c_str() ) )
    {
        string ts = string("Compilers/")+compiler;
        QString cmppath = st->getStr( ts.c_str() );
        if (!cmppath.isEmpty()) compiler=cmppath.toLatin1().constData();
        else
        {
            /* try to find compiler in appRootDir. */
            string ts =  getPathToBinary(compiler);

            if (fwbdebug) qDebug("Checking compiler in %s", ts.c_str());

            if ( QFile::exists( ts.c_str() ) )
                compiler = ts;
        }
    }
#endif

    QString wdir = getFileDir(project->getRCS()->getFileName() );

    args.clear();

    args.push_back(compiler.c_str());

    QString qs = fwopt->getStr("cmdline").c_str();
    args += qs.split(" ", QString::SkipEmptyParts);

    args.push_back("-v");
    args.push_back("-f");
    args.push_back(project->getRCS()->getFileName());

    // If we are compiling in the background (for diff), set wdir to temp directory
    if (isAutoCompiling) {
        QDir tempDir(project->getTemporaryDirPath());
        if (!tempDir.exists())
            tempDir.mkdir(project->getTemporaryDirPath());
        wdir = tempDir.absolutePath();
    }

    if (wdir!="")
    {
        args.push_back("-d");
        args.push_back(wdir);
    }

    // Always pass "-o file_name" parameter to the compiler. If user
    // specified it in the "compiler" tab, then use that. If not,
    // compose it from the name of the firewall and extension
    // ".fw". This way we can properly encode file name for the
    // encoding and locale used on the system. Compiler simply takes
    // the name of the fw object from XML file and uses that for the
    // generated file name, but since the name in XML is encoded in
    // Utf8, the file name ended up in Utf8 regardless of the OS
    // encoding and locale. This caused problems, such as installer
    // could not then find file created by the compiler if fw name had
    // non-ascii characters.

    // If object being compiled is a Cluster, use -O instead of
    // -o. The parameter is a list of pairs:
    // member_fw_id_1,output_file_name_1,member_fw_id_2,output_file_name_2
    // (all separated by commas)

    if (Cluster::isA(fw))
    {
        args.push_back("-O");
        QStringList name_pairs;
        list<Firewall*> members;
        Cluster::cast(fw)->getMembersList(members);
        for (list<Firewall*>::iterator it=members.begin(); it!=members.end(); ++it)
        {
            QString fw_id = project->db()->getStringId((*it)->getId()).c_str();
            name_pairs.push_back(
                fw_id + "," + FirewallInstaller::getGeneratedFileName(*it)
            );
        }
        args.push_back(name_pairs.join(","));
    } else 
    {
        args.push_back("-o");
//        args.push_back(FirewallInstaller::getGeneratedFileFullPath(fw));
        args.push_back(FirewallInstaller::getGeneratedFileName(fw));
    }

    args.push_back("-i");

    args.push_back( project->db()->getStringId(fw->getId()).c_str() );
    return args;
}

void instDialog::compilerFinished(int ret_code, QProcess::ExitStatus status)
{
    if( fwbdebug) qDebug("instDialog::compilerFinished "
                         "exit code = %d exit_status=%d",
                         ret_code, status);

    readFromStdout();

    if (rejectDialogFlag)
    {
        rejectDialogFlag = false;
        QDialog::reject();
        return;
    }

//    if (ret_code==0 && status==QProcess::NormalExit)
    if ((ret_code==fwcompiler::BaseCompiler::FWCOMPILER_SUCCESS
         || ret_code==fwcompiler::BaseCompiler::FWCOMPILER_WARNING)
            && status==QProcess::NormalExit && !isAutoCompiling)
    {
        if (ret_code==fwcompiler::BaseCompiler::FWCOMPILER_WARNING)
            opWarning(cnf.fwobj);
        else
            opSuccess(cnf.fwobj);
//        mw->updateLastCompiledTimestamp(cnf.fwobj);

        QCoreApplication::postEvent(
            mw, new updateLastCompiledTimestampEvent(
                project->db()->getFileName().c_str(), cnf.fwobj->getId()));

        if (Cluster::isA(cnf.fwobj))
        {

            list<Firewall*> members;
            Cluster::cast(cnf.fwobj)->getMembersList(members);
            for (list<Firewall*>::iterator it=members.begin(); it!=members.end(); ++it)
            {
//                project->updateLastCompiledTimestamp(*it);
                QCoreApplication::postEvent(
                    mw, new updateLastCompiledTimestampEvent(
                        project->db()->getFileName().c_str(), (*it)->getId()));
            }
        }
    }
    else
    {
        blockInstallForFirewall(cnf.fwobj);
        opError(cnf.fwobj);
    }
    currentProgressBar->setValue(currentProgressBar->maximum());

    QTimer::singleShot( 0, this, SLOT(mainLoopCompile()));
    return;
}
