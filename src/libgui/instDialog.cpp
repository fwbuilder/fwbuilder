/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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
#include "ProjectPanel.h"
#include "FirewallInstaller.h"
#include "FWBSettings.h"
#include "SSHUnx.h"
#include "SSHPIX.h"
#include "SSHIOS.h"
#include "FWWindow.h"
#include "instOptionsDialog.h"
#include "instBatchOptionsDialog.h"
#include "FirewallCodeViewer.h"

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
#include <QTime>
#include <QWidget>

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
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace libfwbuilder;
using namespace fwcompiler;


instDialog::instDialog(QWidget *p) : QDialog(p)
{
    connect(this, SIGNAL(activateRule(ProjectPanel*, QString, QString, int)),
            p, SLOT(activateRule(ProjectPanel*, QString, QString, int)));

    m_dialog = new Ui::instDialog_q;
    m_dialog->setupUi(this);

    inst_opt_dlg = nullptr;

    //project = mw->activeProject();

    setControlWidgets(this,
                      m_dialog->stackedWidget,
                      m_dialog->nextButton,
                      m_dialog->finishButton,
                      m_dialog->backButton,
                      m_dialog->cancelButton,
                      m_dialog->titleLabel);

    setWindowFlags(Qt::Dialog | Qt::WindowSystemMenuHint);

    list<string> err_re;
    BaseCompiler::errorRegExp(&err_re);
    err_re.push_back("(Abnormal[^\n]*)");
    err_re.push_back("(fwb_[^:]*: \\S*\\.cpp:\\d{1,}: .*: Assertion .* failed.)");
    foreach(string re, err_re)
    {
        error_re.push_back(QRegExp(re.c_str(), Qt::CaseInsensitive));
    }

    list<string> warn_re;
    BaseCompiler::warningRegExp(&warn_re);
    foreach(string re, warn_re)
    {
        warning_re.push_back(QRegExp(re.c_str()));
    }


    QTextCursor cursor(m_dialog->procLogDisplay->textCursor());
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
    currentStopButton = m_dialog->stopButton;
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

    connect(currentFirewallsBar, SIGNAL(valueChanged(int)), this, SIGNAL(currentFirewallsBarValueChanged(int)));
    isAutoCompiling = false;
}

/*
 * list fws is empty when user hits main toolbar button Compile or
 * Install or uses main menu Rules/Compile or Rules/Install. This list
 * has firewall objects to be compiled/installed when user uses
 * "compile-this" or "install-this" button or context menu items
 * "Compile" or "Install".
 */
void instDialog::show(ProjectPanel *proj,
                      bool install,
                      bool onlySelected,
                      std::set<Firewall*> fws)
{
    canceledAll = false;
    if (isVisible() || isAutoCompiling) return;
    lastPage = -1;
    installer = nullptr;
    finished = false;
    page_1_op = INST_DLG_COMPILE;
    compile_complete = false;
    rejectDialogFlag = false;
    isAutoCompiling = false;

    m_dialog->selectTable->clear();
    this->project = proj;

    compile_only = ! install;

    m_dialog->warning_space->hide();
    m_dialog->procLogDisplay->clear();

    firewalls.clear();
    clusters.clear();
    compile_fw_list.clear();
    install_fw_list.clear();

    for (int page=0; page < pageCount(); page++)
        setFinishEnabled(page, false);

    disableStopButton();

    if (!onlySelected)
    {
        findFirewalls();
    }
    else
    {
        firewalls.clear();
        clusters.clear();
        foreach(Firewall* fw, fws)
        {
            if (Cluster::isA(fw))
                clusters.push_back(Cluster::cast(fw));
            else
                firewalls.push_back(fw);
        }
        firewalls.sort(FWObjectNameCmpPredicate());
        clusters.sort(FWObjectNameCmpPredicate());
        m_dialog->saveMCLogButton->setEnabled(true);
    }

    if (fwbdebug)
        qDebug() << "instDialog::show"
                 << "firewalls.size()=" << firewalls.size()
                 << "clusters.size()=" << clusters.size()
                 << "install=" << install;

    if (firewalls.size()==0 && clusters.size()==0)
    {
        setTitle(0, tr("There are no firewalls to process."));
        for (int i=0; i<pageCount(); i++)
        {
            setAppropriate(i, false);
        }
        showPage(CHOOSE_OBJECTS);
        return;
    }

    if (inst_opt_dlg)
        inst_opt_dlg->m_dialog->batchInstall->setChecked(false);

    if (firewalls.size() == 1 && clusters.size() == 0)
        m_dialog->selectAllNoneFrame->hide();
    else
        m_dialog->selectAllNoneFrame->show();

    //m_dialog->batchInstall->setChecked(false);

    creatingTable = false;

    m_dialog->selectTable->setFocus();

    m_dialog->selectInfoLabel->setText(
        QString("<p align=\"center\"><b><font size=\"+2\">%1</font></b></p>")
        .arg(tr("Select firewalls to compile.")));

    if (compile_only)
    {
        m_dialog->batchInstFlagFrame->hide();
        setAppropriate(2, false);
        m_dialog->selectTable->hideColumn(INSTALL_CHECKBOX_COLUMN);
    } else
    {
        m_dialog->batchInstFlagFrame->show();
        setAppropriate(2, true);
        m_dialog->selectTable->showColumn(INSTALL_CHECKBOX_COLUMN);
    }

    m_dialog->detailMCframe->show();
    this->setVisible(true);

    // we just started, there is nothing to inspect in the workflow of
    // this wizard yet. This button will be enabled when compile phase is complete
    m_dialog->inspectGeneratedFiles->setEnabled(compile_complete);

    showPage(CHOOSE_OBJECTS);
}

void instDialog::autoCompile(ProjectPanel *project)
{
        show(project, false, false, std::set<Firewall*>());
        hide();
        isAutoCompiling = true;
        selectAllFirewalls();
        nextButton->click();
}

instDialog::~instDialog()
{
    if (inst_opt_dlg != nullptr) delete inst_opt_dlg;
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

    enableStopButton();

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
        disableStopButton();

        if (compile_only)
        {
            compile_complete = true;
            finished = true;
            setFinishEnabled(currentPage(), true);
            this->m_dialog->finishButton->setDefault(true);

            QStringList files;
            int n = findFilesToInspect(files);
            m_dialog->inspectGeneratedFiles->setEnabled(n != 0);
        } else
        {
            compile_complete = true;
            showPage(COMPILE_INSTALL);
//             setNextEnabled(currentPage(), true);
//             setFinishEnabled(currentPage(), false);
//             m_dialog->inspectGeneratedFiles->setEnabled(compile_complete);
        }

        if (isAutoCompiling) {
            finishButton->click();
            isAutoCompiling = false;
            emit autoCompileDone();
            deleteLater();
        }
    }
}

void instDialog::mainLoopInstall()
{
    if (fwbdebug)
        qDebug("instDialog::mainLoopInstall:  %d firewalls to install",
               int(install_fw_list.size()));

    if (finished) return;

    enableStopButton();

    if (install_fw_list.size() && !canceledAll)
    {
        Firewall *fw = install_fw_list.front();
        install_fw_list.pop_front();
        runInstaller(fw, install_fw_list.size()>0);
        return;
    }
    if (canceledAll)
    {
        foreach(Firewall *fw, install_fw_list)
        {
            this->opCancelled(fw);
        }
        install_fw_list.clear();
    }

    finished = true;
    setFinishEnabled(currentPage(), true);
    this->m_dialog->finishButton->setDefault(true);
    disableStopButton();
}

// ========================================================================


void instDialog::showPage(const int page)
{
    // see #1044   Hide batch install label and checkbox once user moves to
    // the install phase, otherwise it looks confusing.
    if (page_1_op == INST_DLG_INSTALL)
    {
        m_dialog->batchInstFlagFrame->hide();
    }

    QStringList tmp;
    m_dialog->inspectGeneratedFiles->setEnabled(
        compile_complete && page_1_op == INST_DLG_COMPILE && findFilesToInspect(tmp));

    if (fwbdebug)
        qDebug() << "instDialog::showPage  page "
                 << lastPage << "--->" << page << "page_1_op=" << page_1_op;

    FakeWizard::showPage(page);

    switch (page)
    {
    case CHOOSE_OBJECTS: // select firewalls for compiling and installing
    {
        // if user returned to this page from "compile" page, assume they
        // want to recompile
        finished = false;
        compile_complete = false;
        fillCompileSelectList();
        setAppropriate(1, tableHasCheckedItems());
        setNextEnabled(page, tableHasCheckedItems());
        m_dialog->selectTable->setFocus();
        m_dialog->nextButton->setDefault(true);
        break;
    }

    case COMPILE_INSTALL:
    {
        // compile, install firewalls and inspect files, depending on
        // the value of page_1_op

        setNextEnabled(page, false);
        setBackEnabled(page, false);

        fillCompileOpList();
        fillInstallOpList(); // fill install_fw_list

        if (compile_fw_list.size()==0 && install_fw_list.size()==0)
        {
            showPage(CHOOSE_OBJECTS);
            return;
        }

        if (compile_fw_list.size()==0) page_1_op = INST_DLG_INSTALL;

        m_dialog->stackedWidget->widget(1)->layout()->removeWidget(m_dialog->logFrame);
        m_dialog->stackedWidget->widget(1)->layout()->addWidget(m_dialog->firewallListFrame);
        m_dialog->stackedWidget->widget(1)->layout()->addWidget(m_dialog->logFrame);

        // Page 1 of the wizard does both compile and install
        // controlled by flag page_1_op. May be it would be less hacky if we
        // used sepaarte page for installs, but that page would look exactly
        // like the page for compile
        switch (page_1_op)
        {
        case INST_DLG_COMPILE:
        {
            if (fwbdebug) qDebug() << "Page 1 compile"
                                   << "compile_complete=" << compile_complete;

            // run full compile cycle only if we haven't done it before
            // User may click Back on the "Inspect" page, this should return
            // them to the "compile" page but not trigger full recompile.
            // Flag compile_complete is set in instDialog::mainLoopCompile()
            if (compile_complete)
            {
                // See #2037: enable "Next" button only if there is at
                // least one firewall that was successfully compiled.
                bool can_install = false;
                list<Firewall*>::iterator i;
                for(i=compile_fw_list.begin(); i!=compile_fw_list.end(); ++i)
                {
                    Firewall *fw = *i;
                    if (compile_status[fw] ==
                        fwcompiler::BaseCompiler::FWCOMPILER_SUCCESS
                            || fwcompiler::BaseCompiler::FWCOMPILER_WARNING)
                    {
                        setNextEnabled(page, true);
                        m_dialog->nextButton->setDefault(true);
                        can_install = true;
                        break;
                    }
                }
                if (!can_install) setFinishEnabled(page, true);
                setBackEnabled(page, true);
            } else
            {
                if (!isAutoCompiling)
                    mw->fileSave();
                currentFirewallsBar->reset();
                currentFirewallsBar->setFormat("%v/%m");
                currentFirewallsBar->setMaximum(compile_list_initial_size);
                emit currentFirewallsBarMaximumValueChanged(currentFirewallsBar->maximum());
                m_dialog->procLogDisplay->clear();
                fillCompileUIList();
                qApp->processEvents();
                mainLoopCompile();
            }
            setBackEnabled(page, true);
            break;
        }

        case INST_DLG_INSTALL:
        {
            if (fwbdebug) qDebug("Page 1 install");
            if (install_fw_list.size() > 0)
            {
                currentFirewallsBar->reset();
                currentFirewallsBar->setMaximum(install_list_initial_size);
                m_dialog->procLogDisplay->clear();
                fillInstallUIList();
                qApp->processEvents();
                mainLoopInstall();
            }
            setBackEnabled(page, false);
            break;
        }

        case INST_DLG_INSPECT:
        {
            QStringList files;
            int no_files = findFilesToInspect(files);

            if (no_files == 0)
            {
                QMessageBox::critical(this, tr("Error"), tr("No files were generated, there is nothing to show."));
                return;
            }

            if (m_dialog->stackedWidget->count() == 3 )
                m_dialog->stackedWidget->removeWidget(m_dialog->stackedWidget->widget(2));

            FirewallCodeViewer *viewer;
            if (firewalls.size() == 1)
                viewer = new FirewallCodeViewer(
                    files,
                    QString("<b>") + firewalls.front()->getName().c_str() + "</b>",
                    project,
                    this);
            else
                viewer = new FirewallCodeViewer(
                    files, tr("<b>Multiple firewalls</b>"), project, this);
            viewer->hideCloseButton();

            viewer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            QWidget *container = new QWidget(this);
            container->setObjectName("codeViewer");
            QHBoxLayout *layout = new QHBoxLayout(container);
            layout->setContentsMargins(0,0,0,0);
            layout->setSpacing(6);
            QFrame *frame = new QFrame(container);
            frame->setLayout(new QHBoxLayout());
            frame->layout()->addWidget(viewer);
            frame->setFrameShape(QFrame::Box);
            frame->setFrameShadow(QFrame::Plain);
            frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            m_dialog->firewallListFrame->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum);
            m_dialog->progress_page->layout()->removeWidget(m_dialog->firewallListFrame);
            layout->addWidget(m_dialog->firewallListFrame);
            layout->addWidget(frame);
            frame->layout()->setContentsMargins(0,0,0,0);

            m_dialog->stackedWidget->addWidget(container);
            m_dialog->stackedWidget->setCurrentIndex(m_dialog->stackedWidget->count()-1);

            setNextEnabled(page, !compile_only);
            setBackEnabled(page, true);
            if (compile_only)
                m_dialog->finishButton->setDefault(true);
            else
                m_dialog->nextButton->setDefault(true);

            break;
        }
        } // end switch(page_1_op)

        break;
    } // end case 1:

    default: { }
    }

    lastPage = currentPage();
    setCurrentPage(page);
}

/**
 * Finds files that were generated for the firewalls scheduled for
 * compile and fills the list. Returns number of files found.
 *
 */
int instDialog::findFilesToInspect(QStringList &files)
{

    QList<Firewall*> fwlist;
    foreach(Firewall *f, firewalls)
        fwlist.append(f);
    foreach(Cluster *c, clusters)
    {
        std::list<Firewall*> cfws;
        c->getMembersList(cfws);
        foreach(Firewall *f, cfws)
            fwlist.append(f);
    }

    foreach(Firewall *f, fwlist)
    {
        /*
         * get full path to the generated file. The path is built from
         * the file name returned by
         * FirewallInstaller::getGeneratedFileName() and directory
         * path from the .fwb file. Note that we use the same
         * algorithm when GUI launches policy compiler, except there
         * the path is passed to it via "-d" command line option.
         */
        QString mainFile = FirewallInstaller::getGeneratedFileFullPath(f);
//        QString mainFile = FirewallInstaller::getGeneratedFileName(f);

        if (!QFile::exists(mainFile)) continue;
        instConf cnf;
        cnf.fwobj = f;
        cnf.script = mainFile;
        QMap<QString, QString> res;
        FirewallInstaller(nullptr, &cnf, "").readManifest(mainFile, &res);
        foreach(QString item, res.keys())
            if (QFile::exists(item))
                files.append(item);
    }
    return files.size();
}
 
struct CaseInsensitiveComparison :
    public std::binary_function<libfwbuilder::FWObject*, libfwbuilder::FWObject*, bool>
{
    bool operator()(libfwbuilder::FWObject *a,libfwbuilder::FWObject *b)
    {
        return QString(a->getName().c_str()).toLower() < QString(b->getName().c_str()).toLower();
    }
};

void instDialog::findFirewalls()
{
    firewalls.clear();
    clusters.clear();
    
    if (project)
    {
        project->m_panel->om->findAllFirewalls(firewalls);
        project->m_panel->om->findAllClusters(clusters);
    }

    firewalls.sort(CaseInsensitiveComparison());
    clusters.sort(CaseInsensitiveComparison());

    m_dialog->saveMCLogButton->setEnabled(true);
}

bool instDialog::checkSSHPathConfiguration(Firewall *fw)
{
    if (fwbdebug) qDebug("instDialog::checkSSHPathConfiguration");
    customScriptFlag = false;

    Management *mgmt = fw->getManagementObject();
    assert(mgmt!=nullptr);
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
    return (platform=="pix" || platform=="fwsm" || platform=="iosacl" || platform=="nxosacl");
}

bool instDialog::isProcurve()
{
    string platform = cnf.fwobj->getStr("platform");
    return (platform=="procurve_acl");
}

bool instDialog::isJuniper()
{
    string platform = cnf.fwobj->getStr("platform");
    return (platform=="junosacl");
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
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Utf8"));
    enableStopButton();
    QTime start_time;
    start_time.start();
    proc.start(path, args);
    if ( !proc.waitForStarted() )
    {
        QProcess::ProcessError err = proc.error();
        opError(cnf.fwobj);
        addToLog( tr("Error: Failed to start program") );
        addToLog(path);
        addToLog( tr("Last error:") );
        switch (err)
        {
        case QProcess::FailedToStart:
            addToLog( tr("The process failed to start") );
            addToLog(QString("PATH: %1").arg(getenv("PATH")));
            break;
        case QProcess::Crashed:
            addToLog( tr("The process crashed some time after starting successfully.") );
            break;
        case QProcess::Timedout:
            addToLog( tr("The last waitFor...() function timed out. Elapsed time: %1 ms").arg(start_time.elapsed()) );
            break;
        case QProcess::WriteError:
            addToLog( tr("An error occurred when attempting to write to the process.") );
            break;
        case QProcess::ReadError:
            addToLog( tr("An error occurred when attempting to read from the process. ") );
            break;
        default:
            addToLog( tr("An unknown error occurred.") );
            break;
        }
        addToLog( tr("Current state of QProcess:") );
        switch (proc.state())
        {
        case QProcess::NotRunning:
            addToLog(tr("The process is not running."));
            break;
        case QProcess::Starting:
            addToLog(tr("The process is starting, but the program has not yet been invoked."));
            break;
        case QProcess::Running:
            addToLog(tr("The process is running and is ready for reading and writing."));
            break;
        }

        //blockInstallForFirewall(cnf.fwobj);
        return false;
    }
    return true;
}

