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

#include "../../config.h"
#include "global.h"
#include "utils.h"
#include "utils_no_qt.h"

#include "instDialog.h"
#include "ProjectPanel.h"
#include "FirewallInstaller.h"
#include "FWBSettings.h"
#include "SSHUnx.h"
#include "SSHPIX.h"
#include "SSHIOS.h"
#include "FWWindow.h"
#include "instOptionsDialog.h"
#include "instBatchOptionsDialog.h"

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qtimer.h>
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qprocess.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <qfile.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qcolor.h>
#include <qtablewidget.h>
#include <qtextcodec.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <QDateTime>
#include <QtDebug>

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwcompiler/BaseCompiler.h"

#ifndef _WIN32
#  include <unistd.h>     // for access(2) and getdomainname
#endif

#include <errno.h>
#include <iostream>

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


instDialog::instDialog(QWidget* p,
                       BatchOperation op,
                       set<Firewall*> reqFirewalls_) : QDialog(p)
{

    connect(this, SIGNAL(activateRule(ProjectPanel*, QString, QString, int)),
            p, SLOT(activateRule(ProjectPanel*, QString, QString, int)));

    m_dialog = new Ui::instDialog_q;
    m_dialog->setupUi(this);

    batch_inst_opt_dlg = NULL;

    project = mw->activeProject();

    setControlWidgets(this,
                      m_dialog->stackedWidget,
                      m_dialog->nextButton,
                      m_dialog->finishButton,
                      m_dialog->backButton,
                      m_dialog->cancelButton,
                      m_dialog->titleLabel);

    setWindowFlags(Qt::Dialog | Qt::WindowSystemMenuHint);

    installer = NULL;
    finished = false;

    page_1_op = INST_DLG_COMPILE;

    rejectDialogFlag = false;

    list<string> err_re;
    BaseCompiler::errorRegExp(&err_re);
    err_re.push_back("(Abnormal[^\n]*)");
    err_re.push_back("(fwb_[^:]*: \\S*\\.cpp:\\d{1,}: .*: Assertion .* failed.)");
    foreach(string re, err_re)
    {
        error_re.push_back(QRegExp(re.c_str()));
    }

    list<string> warn_re;
    BaseCompiler::warningRegExp(&warn_re);
    foreach(string re, warn_re)
    {
        warning_re.push_back(QRegExp(re.c_str()));
    }


    currentLog = m_dialog->procLogDisplay;
    QTextCursor cursor(currentLog->textCursor());
    normal_format = cursor.charFormat();
    error_format = normal_format;
    error_format.setForeground(QBrush(Qt::red));
    error_format.setAnchorHref("http://somewhere.com");
    error_format.setAnchor(true);
    // weight must be between 0 and 99. Qt 4.4.1 does not seem to mind if
    // it is >99 (just caps it) but older versions assert
    error_format.setProperty(QTextFormat::FontWeight, 99);
    warning_format = normal_format;
    warning_format.setForeground(QBrush(Qt::blue));
    warning_format.setProperty(QTextFormat::FontWeight, 99);
    warning_format.setAnchor(true);
    warning_format.setAnchorHref("http://somewhere.com");
    highlight_format = normal_format;
    highlight_format.setProperty(QTextFormat::FontWeight, 99);

    currentSaveButton = m_dialog->saveMCLogButton;
    currentSaveButton->setEnabled(true);
    currentStopButton = m_dialog->controlMCButton;
    currentProgressBar = m_dialog->compProgress;
    currentFirewallsBar = m_dialog->compFirewallProgress;
    currentLabel = m_dialog->infoMCLabel;
    currentFWLabel = m_dialog->fwMCLabel;
    currentLabel->setText("");

    connect(&proc, SIGNAL(readyReadStandardOutput()),
            this, SLOT(readFromStdout()) );

    // even though we set channel mode to "merged", QProcess
    // seems to not merge them on windows.
    proc.setProcessChannelMode(QProcess::MergedChannels);

    m_dialog->fwWorkList->setSortingEnabled(true);

    for (int page=0; page < pageCount(); page++)
        setFinishEnabled(page, false);

    lastPage=-1;
    reqFirewalls = reqFirewalls_;

    findFirewalls();
    if (firewalls.size()==0)
    {
        setTitle( pageCount()-1, tr("There are no firewalls to process.") );
        for (int i=0;i<pageCount()-1;i++)
        {
            setAppropriate(i,false);
        }
        showPage(pageCount()-1);
        return;
    }
    if (firewalls.size()==1) m_dialog->batchInstall->setEnabled(false);
    // setup wizard appropriate pages
    operation = op;

    creatingTable = false;

    m_dialog->selectTable->setFocus();

    showPage(0);

    if (reqFirewalls_.size() == 1)
        showPage(1);

    switch(op)
    {
    case BATCH_COMPILE:
    { // only compilation's requested
        m_dialog->selectInfoLabel->setText(
            tr("<p align=\"center\"><b><font size=\"+2\">Select firewalls to compile.</font></b></p>"));
        m_dialog->batchInstFlagFrame->hide();
        setAppropriate(2,false);
        m_dialog->selectTable->hideColumn(INSTALL_CHECKBOX_COLUMN);
        break;
    }

    case BATCH_INSTALL:
    { // full cycle's requested
        break;
    }

    default :
    {
        setTitle( pageCount()-1, tr("Unknown operation.") );
        for (int i=0;i<pageCount()-1;i++)
        {
            setAppropriate(i,false);
        }
        showPage(pageCount()-1);
    }
    }

    m_dialog->detailMCframe->show();
}

instDialog::~instDialog()
{
    if (batch_inst_opt_dlg != NULL) delete batch_inst_opt_dlg;
    delete m_dialog;
}

// ========================================================================

/* 
 * main loop: use lists compile_fw_list and install_fw_list to iterate
 * all firewalls and do everything.
 */
void instDialog::mainLoopCompile()
{
    if (finished) return;

    // first compile all
    if (compile_fw_list.size())
    {
        Firewall *fw = compile_fw_list.front();
        compile_fw_list.pop_front();
        cnf.clear();
        runCompiler(fw);
        return;
    } else
    {
        // Compile is done or there was no firewalls to compile to
        // begin with. Check if we have any firewalls to install. Note
        // that we "uncheck" "install" checkboxes in the first page of
        // the wizard on compile failure, so we need to rebuild install_fw_list
        // here.
        fillInstallOpList();
        if (install_fw_list.size() > 0)
        {
            page_1_op = INST_DLG_INSTALL;
            setNextEnabled(1, true);
            setFinishEnabled(currentPage(), false);
        } else
        {
            finished = true;
            setFinishEnabled(currentPage(), true);
        }
    }
}

void instDialog::mainLoopInstall()
{
    if (fwbdebug)
        qDebug("instDialog::mainLoopInstall:  %d firewalls to install",
               int(install_fw_list.size()));

    if (finished) return;

    if (install_fw_list.size())
    {
        if (install_fw_list.size() == install_list_initial_size)
        {
            // this is the first firewall. If batch mode was requested, show
            // installer options dialog (this will be the only time it is 
            // shown)
            if (m_dialog->batchInstall->isChecked() && !getBatchInstOptions())
                return;
        }

        Firewall *fw = install_fw_list.front();
        install_fw_list.pop_front();
        runInstaller(fw);
        return;
    }

    finished = true;
    setFinishEnabled(currentPage(), true);
}

// ========================================================================


void instDialog::showPage(const int page)
{
    FakeWizard::showPage(page);

    if (fwbdebug && reqFirewalls.empty())
        qDebug("instDialog::showPage reqFirewalls is empty");

    if (fwbdebug) qDebug("instDialog::showPage");
    int p = page;

    if (fwbdebug)
        qDebug(QString("to page: %1  from page: %2").
               arg(p).arg(lastPage).toAscii().constData());

    if (p==2)
    {
        showPage(1);
        return;
    }

    switch (p)
    {
    case 0: // select firewalls for compiling and installing
    {
        if (lastPage<0) fillCompileSelectList();
        setNextEnabled(0, tableHasCheckedItems());
        break;
    }

    case 1: // compile and install firewalls
    {
        setBackEnabled(1, false);
        setNextEnabled(1, false);

        fillCompileOpList();
        fillInstallOpList();

        // Page 1 of the wizard does both compile and install
        // controlled by flag page_1_op
        switch (page_1_op)
        {
        case INST_DLG_COMPILE:
        {
            if (fwbdebug) qDebug("Page 1 compile");
            if (compile_fw_list.size()==0)
            {
                if (install_fw_list.size()==0)
                {
                    showPage(0);
                    return;
                }
                page_1_op = INST_DLG_INSTALL;
                showPage(1);
                return;
            }

            mw->fileSave();

            currentFirewallsBar->reset();
            currentFirewallsBar->setMaximum(compile_list_initial_size);
            currentLog->clear();
            fillCompileUIList();
            qApp->processEvents();
            mainLoopCompile();
            break;
        }

        case INST_DLG_INSTALL:
        {
            if (fwbdebug) qDebug("Page 1 install");
            if (install_fw_list.size() > 0)
            {
                currentFirewallsBar->reset();
                currentFirewallsBar->setMaximum(install_list_initial_size);
                currentLog->clear();
                fillInstallUIList();
                qApp->processEvents();
                mainLoopInstall();
                break;
            }
        }
        }
        break;
    }

    default: { }
    }

    lastPage = currentPage();
    setCurrentPage(page);
}

void instDialog::replaceMacrosInCommand(Configlet *conf)
{
/* replace macros in activation commands:
 *
 * {{$fwbpromp}}   -- "magic" prompt that installer uses to detect when it is logged in
 * {{$fwdir}}      -- directory on the firewall
 * {{$fwscript}}   -- script name on the firewall
 * {{$rbtimeout}}  -- rollbak timeout
 */

/*
 * TODO: it does not make sense to split remote_script and then
 * reassemble it again from the file name and cnf.fwdir. We should set
 * variable $remote_script and use it in the configlets instead, but
 * keep $fwbscript and $fwdir for backwards compatibility
 */

/*
 * remote_script is a full path, which in case of Cisco can be
 * something like "flash:file.fw". This means we have a problem with
 * QFileInfo that interprets it as path:filename on Window or just
 * file name with no directory path on Unix. As the result, fwbscript
 * becomes just "file.fw" on Windows and stays "flash:file.fw" on
 * Unix.
 */

    QString fwbscript = QFileInfo(cnf.remote_script).fileName();
    if (fwbscript.indexOf(":")!=-1) fwbscript = fwbscript.section(':', 1, 1);

    if (fwbdebug)
    {
        qDebug() << "Macro substitutions:";
        qDebug() << "  $fwdir=" << cnf.fwdir;
        qDebug() << "  cnf.script=" << cnf.script;
        qDebug() << "  cnf.remote_script=" << cnf.remote_script;
        qDebug() << "  $fwscript=" << fwbscript;
    }

    conf->setVariable("fwbprompt", fwb_prompt);
    conf->setVariable("fwdir", cnf.fwdir);
    conf->setVariable("fwscript", fwbscript);

    conf->setVariable("rbtimeout", cnf.rollbackTime);
    conf->setVariable("rbtimeout_sec", cnf.rollbackTime * 60);
}

void instDialog::testRunRequested()
{
}

void instDialog::findFirewalls()
{
    firewalls.clear();
    clusters.clear();
    
    if (project)
    {
        project->m_panel->om->findAllFirewalls(firewalls);
        project->m_panel->om->findAllClusters(clusters);
    }

    firewalls.sort(FWObjectNameCmpPredicate());
    clusters.sort(FWObjectNameCmpPredicate());

    m_dialog->saveMCLogButton->setEnabled(true);
}

bool instDialog::checkSSHPathConfiguration(Firewall *fw)
{
    if (fwbdebug) qDebug("instDialog::checkSSHPathConfiguration");
    customScriptFlag = false;

    Management *mgmt = fw->getManagementObject();
    assert(mgmt!=NULL);
    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();

/* we don't care about ssh settings if external installer is to be used */

    if ( pis->getCommand()=="" && (
             st->getSSHPath().isEmpty() || st->getSCPPath().isEmpty()))
    {
        QMessageBox::critical(this, "Firewall Builder",
   tr("Policy installer uses Secure Shell to communicate with the firewall.\n"
      "Please configure directory path to the secure shell utility \n"
      "installed on your machine using Preferences dialog"),
                              tr("&Continue") );

        addToLog("Please configure directory path to the secure \n "
                 "shell utility installed on your machine using \n"
                 "Preferences dialog\n");
        return false;
    }

    return true;
}

bool instDialog::isCiscoFamily()
{
    string platform = cnf.fwobj->getStr("platform");
    return (platform=="pix" || platform=="fwsm" || platform=="iosacl");
}

/*
 * "uncheck" checkbox in the "install" column to make sure we do not
 * try to install this firewall. Used in instDialog_compile on failure.
 */
void instDialog::blockInstallForFirewall(Firewall *fw)
{
    if (Cluster::isA(fw))
    {
        list<Firewall*> members;
        Cluster::cast(fw)->getMembersList(members);
        for (list<Firewall*>::iterator it=members.begin(); it!=members.end(); ++it)
            blockInstallForFirewall(*it);
    } else
    {
        QList<QTreeWidgetItem*> items = m_dialog->selectTable->findItems("*", Qt::MatchWildcard);
        QList<QTreeWidgetItem*>::iterator i;
        for (i=items.begin(); i!=items.end(); ++i)
        {
            int obj_id = (*i)->data(0, Qt::UserRole).toInt();
            if (obj_id == fw->getId())
                (*i)->setCheckState(INSTALL_CHECKBOX_COLUMN, Qt::Unchecked);
        }
    }
}

void instDialog::setUpProcessToCompile()
{
    connect(&proc, SIGNAL(readyReadStandardOutput()),
            this, SLOT(readFromStdout()) );

    // even though we set channel mode to "merged", QProcess
    // seems to not merge them on windows.
    proc.setProcessChannelMode(QProcess::MergedChannels);

    proc.disconnect(SIGNAL(finished(int,QProcess::ExitStatus)));
    connect(&proc, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(compilerFinished(int,QProcess::ExitStatus)) );
}

void instDialog::setUpProcessToInstall()
{
    connect(&proc, SIGNAL(readyReadStandardOutput()),
            this, SLOT(readFromStdout()) );

    // even though we set channel mode to "merged", QProcess
    // seems to not merge them on windows.
    proc.setProcessChannelMode(QProcess::MergedChannels);

    proc.disconnect(SIGNAL(finished(int,QProcess::ExitStatus)));
    connect(&proc, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(installerFinished(int,QProcess::ExitStatus)) );
}

/*
 * This method is used to launch compiler AND user-defined external
 * installation script.
 */
bool instDialog::executeCommand(const QString &path, QStringList &args)
{
    // set codecs so that command line parameters can be encoded
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Utf8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Utf8"));
    enableStopButton();
    proc.start(path, args);
    if ( !proc.waitForStarted() )
    {
        opError(cnf.fwobj);
        addToLog( tr("Error: Failed to start program") );
        addToLog(path);
        //blockInstallForFirewall(cnf.fwobj);
        return false;
    }
    return true;
}

