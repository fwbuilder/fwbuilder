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
#include "FWBSettings.h"
#include "SSHUnx.h"
#include "SSHPIX.h"
#include "SSHIOS.h"
#include "FWWindow.h"
#include "InstallFirewallViewItem.h"
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

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"

#ifndef _WIN32
#  include <unistd.h>     // for access(2) and getdomainname
#endif

#include <errno.h>
#include <iostream>

using namespace std;
using namespace libfwbuilder;


instDialog::instDialog(QWidget* p,
                       BatchOperation op,
                       t_fwSet reqFirewalls_) : QDialog(p)
{
    m_dialog = new Ui::instDialog_q;
    m_dialog->setupUi(this);

    setControlWidgets(this, m_dialog->stackedWidget,
                      m_dialog->nextButton,
                      m_dialog->finishButton,
                      m_dialog->backButton,
                      m_dialog->cancelButton,
                      m_dialog->titleLabel);

    setWindowFlags(Qt::Dialog | Qt::WindowSystemMenuHint);

    session=NULL;
    dlg=NULL;
    currentLog = NULL;
    currentSaveButton = NULL;
    currentStopButton = NULL;
    showSelectedFlag=false;
    pendingLogLine = "";
    rejectDialogFlag=false;

    /* object proc is used to launch policy compilers as background
     * processes. SSH sessions in installers are controlled by class
     * SSHSession (and classes derived from it). This leads to some
     * duplication, such as all the apparatus for reading from stdout
     * of the background process is duplicated in SSHSession and here.
     *
     * The same object is also used to launch custom installer scripts.
     *
     * TODO(vadim): need to move everything that deals with compiler
     * process into its own class CompilerSession derived from
     * SSHSession. Perhaps also rename SSHSession to BackgroundSession
     * or something.
     */

    connect(&proc, SIGNAL(readyReadStandardOutput()),
            this, SLOT(readFromStdout()) );
    connect(&proc, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(processExited(int)) );

    // even though we set channel mode to "merged", QProcess
    // seems to not merge them on windows.
    proc.setProcessChannelMode(QProcess::MergedChannels);

    m_dialog->fwWorkList->setSortingEnabled(true);

    setFinishEnabled(pageCount()-1, true);

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
    operation=op;

    creatingTable = false;

    showPage(0);

    switch(op)
    {
    case BATCH_COMPILE:
    { // only compilation's requested
        m_dialog->selectInfoLabel->setText(tr("<p align=\"center\"><b><font size=\"+2\">Select firewalls for compilation.</font></b></p>"));
        m_dialog->batchInstFlagFrame->hide();
        setAppropriate(2,false);


        m_dialog->selectTable->hideColumn(1);
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

    //hide all details
    bool fs=st->value("/FirewallBuilder2/Installer/ShowDetails"    ).toBool();
    if (fs)
        m_dialog->detailMCframe->show();
    else
        m_dialog->detailMCframe->hide();

    togleDetailMC();
}

instDialog::~instDialog()
{
    delete m_dialog;
    if (dlg)
    {
        delete dlg;
        dlg=NULL;
    }
}

void setSuccessState(QTreeWidgetItem *item)
{
    QBrush b = item->foreground(1);
    b.setColor(Qt::darkGreen);
    item->setForeground(1,b);
    item->setForeground(0,b);

    QFont f = item->font(1);
    f.setBold(true);
    item->setFont(1,f);
    item->setFont(0,f);
}

void setFailureState(QTreeWidgetItem *item)
{
    QBrush b = item->foreground(1);
    b.setColor(Qt::darkRed);
    item->setForeground(1,b);
    item->setForeground(0,b);

    QFont f = item->font(1);
    f.setBold(true);
    item->setFont(1,f);
    item->setFont(0,f);
}

void setErrorState(QTreeWidgetItem *item)
{
    QBrush b = item->foreground(1);
    b.setColor(Qt::darkRed);
    item->setForeground(1,b);
    item->setForeground(0,b);

    QFont f = item->font(1);
    f.setBold(true);
    item->setFont(1,f);
    item->setFont(0,f);
}

void setInProcessState(QTreeWidgetItem *item)
{
// disable font manipulation, it does not work right on windows
#if 0
    QFont f = item->font(1);
    f.setBold(true);
    item->setFont(1,f);
    item->setFont(0,f);
#endif
}

void instDialog::nextClicked()
{
    if (nextRelevant( currentPage() ) > -1)
        showPage(nextRelevant( currentPage() ));
}

void instDialog::backClicked()
{
    if (previousRelevant( currentPage() ) > -1)
        showPage(previousRelevant( currentPage() ));
}

void instDialog::togleDetailMC()
{
    if (m_dialog->detailMCframe->isVisible())
    {
        m_dialog->detailsButton->setText(tr("Show details"));
        m_dialog->detailMCframe->hide();
        st->setValue("/FirewallBuilder2/Installer/ShowDetails",false);
    }
    else
    {
        m_dialog->detailsButton->setText(tr("Hide details"));
        m_dialog->detailMCframe->show();
        st->setValue("/FirewallBuilder2/Installer/ShowDetails",true);
    }
}

void instDialog::prepareInstConf(Firewall *)
{
    if (fwbdebug) qDebug("instDialog::prepareInstConf");

}

void instDialog::prepareInstallerOptions()
{
    if (fwbdebug) qDebug("instDialog::prepareInstallerOptions");
    ready=false;
    activationCommandDone = false;
    FWOptions  *fwopt = cnf.fwobj->getOptionsObject();

    fwb_prompt="--**--**--";
    session = NULL;

    cnf.batchInstall = m_dialog->batchInstall->isChecked();

    cnf.incremental    = st->value("/FirewallBuilder2/Installer/incr"    ).toBool();
    cnf.save_diff      = st->value("/FirewallBuilder2/Installer/savediff").toBool();
    cnf.saveStandby    = st->value("/FirewallBuilder2/Installer/saveStandby").toBool();
    cnf.dry_run        = st->value("/FirewallBuilder2/Installer/dryrun"  ).toBool();
    cnf.quiet          = st->value("/FirewallBuilder2/Installer/quiet"   ).toBool();
    cnf.verbose        = st->value("/FirewallBuilder2/Installer/verbose" ).toBool();
    cnf.stripComments  = st->value("/FirewallBuilder2/Installer/stripComments" ).toBool();
    cnf.compressScript = st->value("/FirewallBuilder2/Installer/compressScript" ).toBool();
    cnf.copyFWB        = st->value("/FirewallBuilder2/Installer/copyFWB" ).toBool();
    cnf.testRun        = st->value("/FirewallBuilder2/Installer/testRun" ).toBool();
    cnf.rollback       = st->value("/FirewallBuilder2/Installer/rollback" ).toBool();
    cnf.rollbackTime   = st->value("/FirewallBuilder2/Installer/rollbackTime").toInt();
    cnf.cancelRollbackIfSuccess =
        st->value("/FirewallBuilder2/Installer/canceRollbackIfSuccess" ).toBool();
/* TODO: set cnf.pgm to ssh path here */

    QString platform = cnf.fwobj->getStr("platform").c_str();

    //bool f = dlg->testRun->isChecked();

    //QSize pix_options_frame_size = dlg->PIXgroupBox->sizeHint();

    cnf.rollbackTimeUnit=
        Resources::getTargetOptionStr(cnf.fwobj->getStr("host_OS"),
                                      "activation/timeout_units").c_str();


/* we initialize these in FWBSettings constructor on Unix, but do not
 * do it on Windows since there is no standard ssh package there. User
 * is supposed to fill these in in the Preferences dialog, otherwise
 * they can't use installer
 */

    ssh = st->getSSHPath();

/* as of v2.0.3, build 437, incremental install actually installs only
 * ACL and nat commands on PIX. It does not use fwb_pix_diff so there
 *  is no need to disable it anymore
 *
    if ( access(cnf.diff_pgm.latin1(), F_OK|X_OK)!=0 )
    {
        cerr << "could not access " << cnf.diff_pgm << endl;

        incr->setChecked(false);
        incr->setEnabled(false);
        saveDiff->setChecked(false);
        saveDiff->setEnabled(false);
    }
*/

    try
    {
        if (cnf.fwobj!=NULL && ! cnf.fwbfile.isEmpty())
        {
            QString aaddr = fwopt->getStr("altAddress").c_str();
            if (!aaddr.isEmpty())
                cnf.maddr = aaddr;
            else
            {
                const InetAddr *mgmt_addr = cnf.fwobj->getManagementAddress();
                if (mgmt_addr)
                    cnf.maddr = mgmt_addr->toString().c_str();
                else 
                    cnf.maddr = "";
            }
        }

        setReady(true);

    } catch(FWException &ex)
    {
        setReady(false);
        //showPage( page(1) );
        currentLog->append( ex.toString().c_str() );
    } catch (std::string s) {
        setReady(false);
        //showPage( page(1) );
        currentLog->append( s.c_str() );
    } catch (std::exception ex) {
        setReady(false);
        //showPage( page(1) );
        currentLog->append( ex.what() );
    } catch (...) {
        setReady(false);
        //showPage( page(1) );
        currentLog->append( QObject::tr("Unsupported exception") );
    }

}


void instDialog::storeInstallerOptions()
{
    st->setValue("/FirewallBuilder2/Installer/incr",    cnf.incremental);
    st->setValue("/FirewallBuilder2/Installer/savediff",cnf.save_diff  );
    st->setValue("/FirewallBuilder2/Installer/saveStandby",cnf.saveStandby);
    st->setValue("/FirewallBuilder2/Installer/dryrun"  ,cnf.dry_run    );
    st->setValue("/FirewallBuilder2/Installer/quiet",   cnf.quiet      );
    st->setValue("/FirewallBuilder2/Installer/verbose", cnf.verbose    );
    st->setValue("/FirewallBuilder2/Installer/stripComments", cnf.stripComments);
    st->setValue("/FirewallBuilder2/Installer/compressScript", cnf.compressScript);
    st->setValue("/FirewallBuilder2/Installer/copyFWB",  cnf.copyFWB);
    st->setValue("/FirewallBuilder2/Installer/testRun",  cnf.testRun);
    st->setValue("/FirewallBuilder2/Installer/rollback", cnf.rollback);
    st->setValue("/FirewallBuilder2/Installer/rollbackTime", cnf.rollbackTime);
    st->setValue("/FirewallBuilder2/Installer/canceRollbackIfSuccess",
      cnf.cancelRollbackIfSuccess);
}

void instDialog::summary()
{
    addToLog( "<hr>" + QObject::tr("<b>Summary:</b>\n") );
    if (!customScriptFlag)
    {
        addToLog( QObject::tr("* firewall name : %1\n")
                .arg(cnf.fwobj->getName().c_str()) );
        addToLog( QObject::tr("* user name : %1\n")
                .arg(cnf.user) );
        addToLog( QObject::tr("* management address : %1\n")
                .arg(cnf.maddr) );
        addToLog( QObject::tr("* platform : %1\n")
                .arg(cnf.fwobj->getStr("platform").c_str())  );
        addToLog( QObject::tr("* host OS : %1\n")
                .arg(cnf.fwobj->getStr("host_OS").c_str()) );
        addToLog( QObject::tr("* Loading configuration from file %1\n")
                .arg(cnf.fwbfile));

        if (cnf.incremental)
        {
            addToLog( QObject::tr("* Incremental install\n"));
        }
        if (cnf.save_diff && cnf.incremental)
        {
            addToLog(
    QObject::tr("* Configuration diff will be saved in file %1\n").arg(cnf.diff_file));
        }
        if (cnf.dry_run)
        {
            addToLog(
    QObject::tr("* Commands will not be executed on the firewall\n"));
        }
    }
    else
    {
        addToLog( QObject::tr("* firewall name : %1\n")
                .arg( (*opListIterator)->getName().c_str()) );
    }
    addToLog("<hr>\n");
}


void instDialog::fillCompileSelectList()
{
    if (fwbdebug) qDebug("instDialog::fillCompileSelectList");

    compileMapping.clear();
    installMapping.clear();

    m_dialog->selectTable->setRowCount(firewalls.size());


    QTableWidgetItem * citem;

    Firewall* f;
    QDateTime dt;
    int row=0;

    bool show_library=false;
    string tmp_libname="";

    if (fwbdebug && reqFirewalls.empty())
        qDebug("instDialog::fillCompileSelectList reqFirewalls is empty");

    creatingTable = true;
    for (std::list<libfwbuilder::Firewall *>::iterator i=firewalls.begin();
            i!=firewalls.end(); ++i)
    {
        f=*i;

        time_t lm=f->getInt("lastModified");
        time_t lc=f->getInt("lastCompiled");
        time_t li=f->getInt("lastInstalled");

        citem=new QTableWidgetItem;
        citem->setText(QString::fromUtf8(f->getName().c_str()));
        m_dialog->selectTable->setItem(row,2,citem);
        //m_dialog->selectTable->setColumnReadOnly(2,true);

        // in fact, if someone use same names for several libraries,
        // additional collumn with library names doesn't help to
        // identify a firewall
        if (!show_library && tmp_libname != "" && tmp_libname != f->getLibraryName())
            show_library = true;
        tmp_libname = f->getLibraryName();

        citem=new QTableWidgetItem;
        citem->setText(QString::fromUtf8(tmp_libname.c_str()));
        m_dialog->selectTable->setItem(row,3,citem);
        //m_dialog->selectTable->setColumnReadOnly(3,true);

        citem=new QTableWidgetItem; //usual type
        bool checked = (f->needsCompile() && reqFirewalls.empty() && !f->getInactive()) ||
                (!reqFirewalls.empty() && reqFirewalls.find(f)!=reqFirewalls.end());
        citem->setCheckState(checked?Qt::Checked:Qt::Unchecked);
        m_dialog->selectTable->setItem(row,0,citem);
        compileMapping[f]=citem;

        citem=new QTableWidgetItem; //usual type
        checked = (operation==BATCH_INSTALL) &&
                ((f->needsInstall() && reqFirewalls.empty() && !f->getInactive()) ||
                (!reqFirewalls.empty() && reqFirewalls.find(f)!=reqFirewalls.end()));
        citem->setCheckState(checked?Qt::Checked:Qt::Unchecked);
        m_dialog->selectTable->setItem(row,1,citem);
        installMapping[f]=citem;

        dt.setTime_t(lm);
        citem = new QTableWidgetItem;
        citem->setText((lm)?dt.toString():QString("---"));
        m_dialog->selectTable->setItem(row,4,citem);

        dt.setTime_t(lc);
        citem = new QTableWidgetItem;
        citem->setText((lm)?dt.toString():QString("---"));
        m_dialog->selectTable->setItem(row,5,citem);

        dt.setTime_t(li);
        citem = new QTableWidgetItem;
        citem->setText((lm)?dt.toString():QString("---"));
        m_dialog->selectTable->setItem(row,6,citem);

        row++;
    }

    creatingTable = false;

    if (show_library) m_dialog->selectTable->showColumn(3);
    else              m_dialog->selectTable->hideColumn(3);

    for (int i=0;i<m_dialog->selectTable->columnCount();i++)
    {
        if (i<4)
            m_dialog->selectTable->resizeColumnToContents(i);
        else
            m_dialog->selectTable->setColumnWidth(i,200);
    }

    //selectTable->setColumnStretchable(2,true);
    //selectTable->sortColumn(2,true,true);
    m_dialog->selectTable->resizeRowsToContents();
}

void instDialog::showPage(const int page)
{
    FakeWizard::showPage(page);

    if (fwbdebug && reqFirewalls.empty())
        qDebug("instDialog::showPage reqFirewalls is empty");

    if (fwbdebug) qDebug("instDialog::showPage");
    int p = page;
    if (fwbdebug)
        qDebug(QString("to page: %1  from page: %2").arg(p).arg(lastPage).toAscii().constData());

    switch (p)
    {
    case 0: // select firewalls for compiling and installing
    {
        if (lastPage<0) fillCompileSelectList();
        setNextEnabled(0, isTableHasChecked());
        break;
    }
    case 1: // compiling (installing) firewalls
    {
        setBackEnabled(1, false);
        setNextEnabled(1, false);
        if (lastPage<1)
        { // starting process
            fillCompileOpList();
            opListIterator=opList.begin();
            if (opList.size()==0)
            {// there are no firewalls for compilation
                // what about installation?

                compileFlag=false;

                fillInstallOpList();
                opListIterator=opList.begin();
                if(opList.size()==0)
                {//there are no firewalls for installation
                    showPage(2);
                }
                else
                {
                    lastPage=p;
                    initInstall();
                    installSelected();
                    if (stopProcessFlag) return;
                }
            }
            else
            {
                compileSelected();
            }
        }
        else
        {
            if (opList.size()==0)
            {
                showPage(2);
            }
            else
            {
                installSelected();
                if (stopProcessFlag) return;
            }
        }

        break;
    }
    case 2: // fin
    {
        if (fwbdebug)
        {
            qDebug("instDialog::showPage:  Page 2; "
                   "currentPage()=%d", currentPage());
        }
        setBackEnabled(2,false);
        setNextEnabled(2, false);
        setFinishEnabled(2, true);
        if (compileFlag && operation==BATCH_INSTALL)
        {
            fillInstallOpList();
            initInstall();
            compileFlag=false;
            if (opList.size()>0)
            {
                opListIterator=opList.begin();
                showPage(1);
            }
            else
            {
                fillLastList();
            }
            break;
        }

        if (
            !compileFlag &&
            opList.end()!=opListIterator &&
            operation==BATCH_INSTALL &&
            !stopProcessFlag)
        {
            showPage(1);
            break;
        }
        fillLastList();
        break;
    }
    default: { }
    }

    lastPage = currentPage();
    setCurrentPage(page);
}

QString instDialog::getFullPath(instConf &cnf, const QString &file )
{
    if (QDir::isRelativePath(file)) return cnf.wdir + "/" + file;
    else return file;
}

bool instDialog::doInstallPage(Firewall* f)
{
    if (fwbdebug) qDebug("instDialog::doInstallPage");

/* change of the page when flag ready is 'true' means we should start
 * operation */
    cnf.fwobj = f;

    if (fwbdebug) qDebug(QString("instDialog::doInstallPage: firewall: %1")
                         .arg(f->getName().c_str()).toAscii().constData());
    cnf.incremental = dlg->m_dialog->incr->isChecked();
    cnf.dry_run     = dlg->m_dialog->test->isChecked();
    cnf.backup_file = dlg->m_dialog->backupConfigFile->text();
    cnf.backup      = !cnf.backup_file.isEmpty();
    cnf.save_diff   = dlg->m_dialog->saveDiff->isChecked();
    cnf.saveStandby = dlg->m_dialog->saveStandby->isChecked();

/* Alternative address:
    - first, check dialog. User could have overriden it using dialog
    - then check firewall options, user could have set it in the "Install"
      tab of firewall settings dialog
    - last, if all overrides are empty, take it from the management interface
 */

    QString aaddr    = dlg->m_dialog->altAddress->text();
    if (!aaddr.isEmpty())
    {
/* alternative address can also be putty session name. In any case,
 * leave it up to ssh to resolve it and signal an error if it can't be
 * resolved ( Putty session name does not have to be in DNS at all ).
 */
        cnf.maddr = aaddr;
        if (fwbdebug)
            qDebug(
                QString("instDialog::doInstallPage: alternative address %1")
                .arg(aaddr).toAscii().constData());

    } else
    {
        FWOptions  *fwopt = cnf.fwobj->getOptionsObject();
        aaddr = fwopt->getStr("altAddress").c_str();
        if (!aaddr.isEmpty())
            cnf.maddr = aaddr;
        else
        {
            const InetAddr *mgmt_addr = cnf.fwobj->getManagementAddress();
            if (mgmt_addr)
                cnf.maddr = mgmt_addr->toString().c_str();
            else 
                cnf.maddr = "";
        }
    }

    if (fwbdebug)
        qDebug(
            QString("instDialog::doInstallPage: management address: %1")
            .arg(cnf.maddr).toAscii().constData());

//        cnf.maddr         = altAddress->text();
    cnf.user          = dlg->m_dialog->uname->text();
    cnf.pwd           = dlg->m_dialog->pwd->text();
    cnf.epwd          = dlg->m_dialog->epwd->text();
    cnf.quiet         = dlg->m_dialog->quiet->isChecked();
    cnf.verbose       = dlg->m_dialog->verbose->isChecked();
    cnf.stripComments = dlg->m_dialog->stripComments->isChecked();
    cnf.compressScript= dlg->m_dialog->compressScript->isChecked();
    cnf.copyFWB       = dlg->m_dialog->copyFWB->isChecked();
    cnf.testRun       = dlg->m_dialog->testRun->isChecked();
    cnf.rollback      = dlg->m_dialog->rollback->isChecked();
    cnf.rollbackTime  = dlg->m_dialog->rollbackTime->value();
    cnf.cancelRollbackIfSuccess = dlg->m_dialog->cancelRollbackIfSuccess->isChecked();

    storeInstallerOptions();
/* check for a common error when multiple interfaces are marked as
 * 'management'
 */
    int nmi = 0;
    list<FWObject*> ll = cnf.fwobj->getByType(Interface::TYPENAME);
    for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
    {
        Interface *intf = Interface::cast( *i );
        if (intf->isManagement()) nmi++;
    }
    if (nmi>1)
    {
        addToLog(
            QObject::tr("Only one interface of the firewall '%1' must be marked as management interface.\n")
            .arg(cnf.fwobj->getName().c_str()).toLatin1().constData() );
        return false;
    }
    if (nmi==0)
    {
        addToLog(
            QObject::tr("One of the interfaces of the firewall '%1' must be marked as management interface.\n")
            .arg(cnf.fwobj->getName().c_str()).toLatin1().constData() );
        return false;
    }
    if (cnf.maddr == "" ||
        cnf.maddr == QString(InetAddr::getAny().toString().c_str()))
    {
        addToLog(
            QObject::tr("Management interface does not have IP address, can not communicate with the firewall.\n") );
        return false;
    }

    confScript="";

    confFiles.clear();
    if (cnf.copyFWB)
    {
        QFileInfo fwbfile_base(cnf.fwbfile);

        if (fwbdebug)
            qDebug( QString("Will copy data file: %1").arg(fwbfile_base.fileName()).toAscii().constData());

        confFiles.push_back( fwbfile_base.fileName() );
    }
/* read manifest from the conf file */

    QString conffile_path = getFullPath(cnf,cnf.conffile);
    QFile cf( conffile_path  );
    if (cf.open( QIODevice::ReadOnly ) )
    {
        QTextStream stream(&cf);
        QString line;
        while (!stream.atEnd())
        {
            line    = stream.readLine();
            int pos = -1;
            if ( (pos=line.indexOf(MANIFEST_MARKER))!=-1 )
            {
                int n = pos + QString(MANIFEST_MARKER).length();
//                if (line[n]=='*')  confScript=line.mid(n+2);
//                else               confFiles.push_back( line.mid(n+2) );

                confFiles.push_back( line.mid(n+2) );

                if (fwbdebug)
                    qDebug("instDialog: adding %c %s",
                           line[n].toLatin1(),
                           line.mid(n+2).toAscii().constData());

            }
            line = "";
        }
        cf.close();
    } else
    {
        QMessageBox::critical(this, "Firewall Builder",
                              tr("File %1 not found.").arg(conffile_path),
                              tr("&Continue") );
        return false;
    }

//    if (confScript.isEmpty()) confScript=cnf.conffile;
    if (confFiles.size()==0) confFiles.push_back(cnf.conffile);

    currentProgressBar->setValue(0);

    summary();

    QStringList  args;

    if (cnf.fwobj->getStr("platform")=="pix" ||
        cnf.fwobj->getStr("platform")=="fwsm" ||
        cnf.fwobj->getStr("platform")=="iosacl"
    )
    {
#ifdef _WIN32
        args.push_back(ssh);

/*
 * putty ignores protocol and port specified in the session file if
 * command line option -ssh is given.
 *
 * On the other hand,the sign of session usage is an empty user name,
 * so we can check for that. If user name is empty, then putty will
 * use current Windows account name to log in to the firewall and this
 * is unlikely to work anyway. This seems to be a decent workaround.
 */
        if (!cnf.user.isEmpty() && ssh.toLower().indexOf("plink.exe")!=-1)
        {
            args.push_back("-ssh");
            args.push_back("-pw");
            args.push_back(cnf.pwd);
        }

        if (!cnf.sshArgs.isEmpty())
            args += cnf.sshArgs.split(" ", QString::SkipEmptyParts);

        if (cnf.verbose) args.push_back("-v");
        if (!cnf.user.isEmpty())
        {
            args.push_back("-l");
            args.push_back(cnf.user);
            args.push_back(cnf.maddr);
        } else
            args.push_back(cnf.maddr);
#else
        args.push_back(argv0.c_str());
        args.push_back("-X");   // fwbuilder works as ssh wrapper
        //if (fwbdebug)
        //    args.push_back("-d");
        args.push_back("-t");
        args.push_back("-t");

        if (!cnf.sshArgs.isEmpty())
            args += cnf.sshArgs.split(" ", QString::SkipEmptyParts);

        if (cnf.verbose) args.push_back("-v");
        if (!cnf.user.isEmpty())
        {
            args.push_back("-l");
            args.push_back(cnf.user);
            args.push_back(cnf.maddr);
        } else
            args.push_back(cnf.maddr);
#endif

        if (cnf.verbose) displayCommand(args);

        phase=1;

        SSHPIX *ssh_object = NULL;
        if (cnf.fwobj->getStr("platform")=="pix" ||
            cnf.fwobj->getStr("platform")=="fwsm")
        {
            ssh_object = new SSHPIX(this,
                                    cnf.fwobj->getName().c_str(),
                                    args,
                                    cnf.pwd,
                                    cnf.epwd,
                                    list<string>());
        } else   // ios
        {
            ssh_object = new SSHIOS(this,
                                    cnf.fwobj->getName().c_str(),
                                    args,
                                    cnf.pwd,
                                    cnf.epwd,
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

        cmd = cnf.getCmdFromResource("pre_config_commands");
        pre_config_commands = replaceMacrosInCommand(cmd).split("\n", QString::SkipEmptyParts);

        if (cnf.rollback)
        {
            cmd = cnf.getCmdFromResource("schedule_rollback");
            pre_config_commands = pre_config_commands +
                    replaceMacrosInCommand(cmd).split("\n", QString::SkipEmptyParts);
        }

        cmd = cnf.getCmdFromResource("post_config_commands");
        post_config_commands = replaceMacrosInCommand(cmd).split("\n", QString::SkipEmptyParts);

        if (cnf.cancelRollbackIfSuccess)
        {
            cmd = cnf.getCmdFromResource("cancel_rollback");
            post_config_commands = post_config_commands +
                    replaceMacrosInCommand(cmd).split("\n", QString::SkipEmptyParts);
        }

        if (cnf.saveStandby)
        {
            cmd = cnf.getCmdFromResource("save_standby");
            post_config_commands = post_config_commands +
                    replaceMacrosInCommand(cmd).split("\n", QString::SkipEmptyParts);
        }

        ssh_object->loadPreConfigCommands( pre_config_commands );
        ssh_object->loadPostConfigCommands( post_config_commands );

        runSSH(ssh_object);
        return true;
    }
    // all other Unix-based platforms

/*
 * if user requested test run, store firewall script in a temp file.
 * Always store it in a temp file on linksys
 */
    QString s;

/* user_can_change_install_dir */
    bool uccid=Resources::getTargetOptionBool(
        cnf.fwobj->getStr("host_OS"),"user_can_change_install_dir");

    if (uccid)
        s=cnf.fwobj->getOptionsObject()->getStr("firewall_dir").c_str();

    if (s.isEmpty()) s=Resources::getTargetOptionStr(
        cnf.fwobj->getStr("host_OS"),
        "activation/fwdir").c_str();

    cnf.fwdir     = s;

    continueRun();

    return true;
}

/* reset ssh session to continue the same installation process, such as
 * when we need to copy several files to the firewall
 */
void instDialog::resetInstallSSHSession()
{
    if (fwbdebug) qDebug("instDialog::resetInstallSSHSession");

    if (session!=NULL)
        QTimer::singleShot( 0, this, SLOT(stopSessionAndDisconnectSignals()));

    activationCommandDone = false;

    if (fwbdebug) qDebug("instDialog::resetInstallSSHSession  done");
}

/* instDialog::stopSessionAndDisconnectSignals runs when we have no
 * other events in the events queue. This is necessary because call to
 * instDialog::finishInstall can come from inside the state machine
 * (e.g. when error was detected). This means we are trying to
 * terminate working session right in the middle, when there could be
 * some more output from its stdout to be collected. To avoid race
 * conditions with events that have not been processed, we schedule
 * all termination and clean-up operations so they will be done at
 * idle time when there are no events in the queue
 */
void instDialog::stopSessionAndDisconnectSignals()
{
    if (fwbdebug)
        qDebug("instDialog::stopSessionAndDisconnectSignals() session=%p",
               session);

    if (session!=NULL)
    {
        disconnect(session,SIGNAL(printStdout_sign(const QString&)),
                   this,SLOT(addToLog(const QString&)));

        disconnect(session,SIGNAL(sessionFinished_sign()),
                   this,SLOT(installerSuccess()));

        disconnect(session,SIGNAL(sessionFatalError_sign()),
                   this,SLOT(installerError()));

        disconnect(session,SIGNAL(updateProgressBar_sign(int,bool)),
                   this,SLOT(updateProgressBar(int,bool)));

        session->terminate();

        delete session;
        session=NULL;
    }

    if (fwbdebug)
        qDebug("instDialog::stopSessionAndDisconnectSignals() done");
}

/*
 * This method builds and returns activation command
 * This method is used for all firewall platforms but PIX
 */
QString instDialog::getActivationCmd()
{
    if (!cnf.activationCmd.isEmpty())
    {
        return cnf.activationCmd;
    }

    QString cmd="";

    string optpath="activation/";

    if (cnf.user=="root")   optpath += "root/";
    else                     optpath += "reg_user/";

    if (cnf.testRun)
    {
        optpath += "test/";
        if (cnf.rollback) optpath += "rollback/";
        else               optpath += "no_rollback/";
    } else
    {
        optpath += "run/";
        if (cnf.compressScript)  optpath += "compression/";
        else                     optpath += "no_compression/";
    }

    cmd=Resources::getTargetOptionStr(cnf.fwobj->getStr("host_OS"),
                                      optpath).c_str();
    return replaceMacrosInCommand(cmd);
}

QString instDialog::replaceMacrosInCommand(const QString &ocmd)
{

/* replace macros in activation command:
 *
 * %FWSCRIPT%, %FWDIR%, %FWBPROMPT%, %RBTIMEOUT%
 *
 * check if cnf.conffile is a full path. If it is, strip the path part
 * and use only the file name for %FWSCRIPT%
 */
    QString cmd = ocmd;

    QString clean_conffile = cnf.conffile.section(QDir::separator(),-1);
    if (fwbdebug)
    {
        qDebug("Macro substitutions:");
        qDebug(QString("  cnf.conffile=%1").arg(cnf.conffile).toAscii().constData());
        qDebug(QString("  %%FWSCRIPT%%=%1").arg(clean_conffile).toAscii().constData());
        qDebug(QString("  %%FWDIR%%=%1").arg(cnf.fwdir).toAscii().constData());
    }

    cmd.replace("%FWSCRIPT%",clean_conffile);
    cmd.replace("%FWDIR%",cnf.fwdir);
    cmd.replace("%FWBPROMPT%",fwb_prompt);

    int r = cnf.rollbackTime;
    if (cnf.rollbackTimeUnit=="sec")  r = r*60;

    QString rbt;
    rbt.setNum(r);
    cmd.replace("%RBTIMEOUT%",rbt);
    return cmd;
}

void instDialog::initiateCopy(const QString &file)
{
    QStringList args;
    list<string> allConfig;

    if (fwbdebug)
        qDebug("instDialog::initiateCopy for the file %s",file.toAscii().constData());

    QString platform=cnf.fwobj->getStr("platform").c_str();
    //if (platform!="pix" && platform!="fwsm") progressBar->show();

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("latin1"));

    std::ifstream   *wfile;

    QString file_with_path = getFullPath(cnf,file);

    wfile = new ifstream(file_with_path.toLatin1().constData());
    if ( ! *wfile)
    {
        file_with_path = file;  // .fwb file path already includes wdir
        wfile = new ifstream(file_with_path.toLatin1().constData());
        if ( ! *wfile)
        {
            addToLog(QObject::tr("Can not open file %1\n").arg(file_with_path));
            delete wfile;
            return;
        }
    }

    /* need to convert strings of the config file from Utf-8 to
     * internal presentation of QT (unicode) so we can process them
     * using methods of QString; will convert them back to Utf-8
     * before sending to the installer process
     */
    string s0;
    while ( !wfile->eof() )
    {
        getline( *wfile, s0);
//        QString s = QString::fromUtf8(s0.c_str());
//        QString s = s0.c_str();
//        s.stripWhiteSpace();

        if (fwbdebug) qDebug("instDialog::initiateCopy s='%s'",s0.c_str());

        allConfig.push_back(s0);
    }
    wfile->close();
    delete wfile;
    wfile=NULL;

//    allConfig.push_back("\004");

#ifdef _WIN32
    args.push_back(ssh);

    if (!cnf.user.isEmpty() && ssh.toLower().indexOf("plink.exe")!=-1)
    {
        args.push_back("-ssh");
        args.push_back("-pw");
        args.push_back(cnf.pwd);
    }

#else
    args.push_back(argv0.c_str());
    args.push_back("-X");   // fwbuilder works as ssh wrapper
    //if (fwbdebug)
    //    args.push_back("-d");

//    args.push_back("-t");
//    args.push_back("-t");
#endif
/* do not change destination, we do chmod on it later */
//        args.push_back( cnf.wdir+"/"+file);

    if (!cnf.sshArgs.isEmpty())
        args += cnf.sshArgs.split(" ", QString::SkipEmptyParts);

    if (cnf.verbose) args.push_back("-v");
    if (!cnf.user.isEmpty())
    {
        args.push_back("-l");
        args.push_back(cnf.user);
        args.push_back(cnf.maddr);
    } else
        args.push_back(cnf.maddr);

    string optpath="activation/";

    if (cnf.user=="root")   optpath += "root/";
    else                     optpath += "reg_user/";

    if (cnf.testRun)        optpath += "test/";
    else                     optpath += "run/";

    optpath+="copy";

    QString cmd=Resources::getTargetOptionStr(cnf.fwobj->getStr("host_OS"),
                                              optpath).c_str();

/* replace macros in activation command:
 *
 * %FWSCRIPT%, %FWDIR%, %FWBPROMPT%, %RBTIMEOUT%
 *
 * check if cnf.conffile is a full path. If it is, strip the path part
 * and use only the file name for %FWSCRIPT%
 */
    QString file_name_no_spaces = file;
    if (file_name_no_spaces.indexOf(" ")!=-1)
    {
        file_name_no_spaces.replace(" ","\\ ");
    }

    QString clean_file = file_with_path.section(QDir::separator(),-1);
    if (fwbdebug)
    {
        qDebug("Macro substitutions:");
        qDebug(QString("  cnf.conffile=%1").arg(cnf.conffile).toAscii().constData());
        qDebug(QString("  %%FWSCRIPT%%=%1").arg(file_name_no_spaces).toAscii().constData());
        qDebug(QString("  %%FWDIR%%=%1").arg(cnf.fwdir).toAscii().constData());
    }

    cmd.replace("\n","");
    cmd.replace("%FWSCRIPT%", file_name_no_spaces);
    cmd.replace("%FWDIR%", cnf.fwdir);
    cmd.replace("%FWBPROMPT%", fwb_prompt);

    args.push_back(cmd);

    addToLog( tr("Copying %1 -> %2:%3\n")
                .arg(file_with_path).arg(cnf.maddr).arg(cnf.fwdir) );

    if (cnf.verbose) displayCommand(args);

    phase=1;

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("latin1"));

    SSHSession *s= new SSHUnx(this,
                              cnf.fwobj->getName().c_str(),
                              args,
                              cnf.pwd,
                              "",
                              allConfig);

    s->setCloseStdin(true);
    runSSH(s);
}

void instDialog::displayCommand(const QStringList &args)
{
    QStringList a1 = args;

    for (QStringList::iterator i=a1.begin(); i!=a1.end(); i++)
    {
        if ( (*i)=="-pw" )
        {
            i++;
            *i = "XXXXXX";
            break;
        }
    }
    QString s=a1.join(" ");
    addToLog( tr("Running command '%1'\n").arg(s) );
}

void instDialog::installNext()
{
    opListIterator++;

    if(opListIterator!=opList.end() && !stopProcessFlag)
    {
        if (fwbdebug) 
            qDebug("instDialog::installNext  schedule install of "
                   "the next selected firewall object.");
        QTimer::singleShot( 0, this, SLOT(installSelected()));
    }
}

/*
 * continueRun is called via idle event handler after the session object
 * is destroyed in stopSessionAndDisconnectSignals.
 *
 * Various methods call resetInstallSSHSession, which schedules call
 * to stopSessionAndDisconnectSignals. installerSuccess() also
 * schedules call to continueRun() right after that. So continueRun()
 * is always called when we have no active session object.
 */
void instDialog::continueRun()
{
    if (fwbdebug) qDebug("instDialog::continueRun");

    if (activationCommandDone)
    {
        if (fwbdebug) qDebug("activationCommandDone");
        addToLog( tr("Done\n") );
        finishInstall();
        return;
    }

    if (cnf.fwobj->getStr("platform")=="pix" ||
        cnf.fwobj->getStr("platform")=="fwsm" ||
        cnf.fwobj->getStr("platform")=="iosacl"
    )
    {
        finishInstall();
        return;
    } else
    {

        if (!confFiles.empty())
        {
            QString cnffile = confFiles.front();
            confFiles.pop_front();
            initiateCopy( cnffile );
        } else
        {
            QStringList args;

            //progressBar->hide();

#ifdef _WIN32
            args.push_back(ssh);

            if (!cnf.user.isEmpty() && ssh.indexOf("plink.exe")!=-1)
                args.push_back("-ssh");

            args.push_back("-pw");
            args.push_back(cnf.pwd);
#else
            args.push_back(argv0.c_str());
            args.push_back("-X");   // fwbuilder works as ssh wrapper
            //if (fwbdebug)
            //    args.push_back("-d");
            args.push_back("-t");
            args.push_back("-t");
#endif
            if (!cnf.sshArgs.isEmpty())
                args += cnf.sshArgs.split(" ");

            if (cnf.verbose) args.push_back("-v");

            if (!cnf.user.isEmpty())
            {
                args.push_back("-l");
                args.push_back(cnf.user);
                args.push_back(cnf.maddr);
            } else
                args.push_back(cnf.maddr);
#if 0
            if (!cnf.user.isEmpty())
                args.push_back(cnf.user + "@" + cnf.maddr);
            else
                args.push_back(cnf.maddr);
#endif

            QString cmd = getActivationCmd();
            args.push_back( cmd );

            addToLog( tr("Activating new policy\n") );
            addToLog( "\n");

            if (cnf.verbose) displayCommand(args);

            activationCommandDone = true;

            runSSH( new SSHUnx(this,
                               cnf.fwobj->getName().c_str(),
                               args,
                               cnf.pwd,
                               "",
                               list<string>()) );
        }

    }
}

void instDialog::runSSH(SSHSession *s)
{
    if (fwbdebug) qDebug("instDialog::runSSH()");

    session = s;

    session->setOptions(&cnf);
    session->setFWBPrompt(fwb_prompt);

    connect(session,SIGNAL(printStdout_sign(const QString&)),
            this, SLOT(addToLog(const QString&)));

    connect(session,SIGNAL(sessionFinished_sign()),
            this, SLOT(installerSuccess()));

    connect(session,SIGNAL(sessionFatalError_sign()),
            this, SLOT(installerError()));

    connect(session,SIGNAL(updateProgressBar_sign(int,bool)),
            this, SLOT(updateProgressBar(int,bool)));

    session->startSession();
}

void instDialog::updateProgressBar(int n, bool setsize)
{
    if (fwbdebug)
        qDebug("instDialog::updateProgressBar  n=%d setsize=%d",n,setsize);

    if (setsize) currentProgressBar->setMaximum(n);
    else
        currentProgressBar->setValue(currentProgressBar->maximum()-n);
}

void instDialog::finishClicked()
{
    accept();
}

/* user clicked 'Cancel' */
void instDialog::cancelClicked()
{
    if (fwbdebug) qDebug("instDialog::cancelClicked()");
    stopSessionAndDisconnectSignals();

    // What is this? Do we need this? This code is not present in 2.1.16.
    if (proc.state() == QProcess::Running)
    {
        rejectDialogFlag = true;
        proc.kill();
    }
    else
        QDialog::reject();
}


void instDialog::showEvent( QShowEvent *ev)
{
    st->restoreGeometry(this, QRect(200,100,480,500) );
    QDialog::showEvent(ev);
}

void instDialog::hideEvent( QHideEvent *ev)
{
    st->saveGeometry(this);
    QDialog::hideEvent(ev);
}

void instDialog::testRunRequested()
{
#if 0
#endif
}

void instDialog::saveLog()
{
    QString dir;
    if (currentLog==NULL) return;
    dir=st->getWDir();
    if (dir.isEmpty())  dir=st->getOpenFileDir();
    if (dir.isEmpty())  dir="~";

    /*
     * We use QTextEdit::append to add lines to the log buffer, each
     append creates a new paragraph so QTextEdit::text returns only
     contents of the last paragraph. Need to reassemble the whole text
     adding text from each paragraph separately.
     */
    QString logText;
    logText = currentLog->toPlainText();
    //logText = currentLog->toHtml();

    QString s = QFileDialog::getSaveFileName(
                    this,
                    "Choose a file",
                    dir,
                    "Text file (*.txt)");

    if (fwbdebug)
        qDebug( "Saving log to file %s", s.toAscii().constData() );

    if (!s.isEmpty())
    {
        if (!s.endsWith(".txt"))
        {
            s+=".txt";
        }

        QFile f(s);
        if (f.open( QIODevice::WriteOnly ))
        {
            QTextStream str( &f );
            str << logText;
            f.close();
        }
    }
}

void instDialog::findFirewalls()
{
    firewalls.clear();
    mw->findAllFirewalls(firewalls);
    firewalls.sort(FWObjectNameCmpPredicate());
    m_dialog->saveMCLogButton->setEnabled(true);
}


bool instDialog::runCompile(Firewall *fw)
{
    if (fwbdebug)
    {
        qDebug("instDialog::runCompile");
        qDebug(("Firewall:"+fw->getName()).c_str());
    }

    addToLog("\n");
    addToLog(
        QObject::tr("Compiling rule sets for firewall: %1\n").
        arg(fw->getName().c_str())
    );

    prepareArgForCompiler(fw);

    addToLog( args.join(" ") + "\n" );

    QString path = args.at(0);
    args.pop_front();
    proc.start(path, args);

    if ( !proc.waitForStarted() )
    {
        currentLog->append( tr("Error: Failed to start program") );
        return false;
    }
    args.push_front(path);

    return true;
}


bool instDialog::testFirewall(Firewall *fw)
{
    if (fwbdebug) qDebug("instDialog::testFirewall");
    FWOptions  *fwopt=fw->getOptionsObject();
    customScriptFlag=false;

    Management *mgmt=fw->getManagementObject();
    assert(mgmt!=NULL);
    PolicyInstallScript *pis   = mgmt->getPolicyInstallScript();

/* we don't care about ssh settings if external installer is to be used */

    if ( (/* ! pis->isEnabled() || */ pis->getCommand()=="") &&
         st->getSSHPath().isEmpty())
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

/* need to save settings so that if the user just changed ssh/scp, the
 * wrapper will pick changes up
 */
    st->save();

    QString ofname = fwopt->getStr("output_file").c_str();
    if (ofname.isEmpty()) ofname = QString(fw->getName().c_str()) + ".fw";

    QString fwfname = getFileDir( mw->getRCS()->getFileName() ) + "/" + ofname;

/* bug #1617501: "Install fails after compile". Check ofname, just in
 * case user put full path name for the output script name in options
 */
    if ( !QFile::exists(fwfname) && !QFile::exists(ofname))
    {
/* need to recompile */
        addToLog(tr("Firewall isn't compiled.\n"));
        if (fwbdebug) qDebug("Firewall isn't compiled.");
        return false;
    }


    args.clear();


    if ( /*! pis->isEnabled() || */ pis->getCommand()=="" )
    {
        //instConf cnf;

        cnf.user          = fwopt->getStr("admUser").c_str();
        cnf.maddr         = fwopt->getStr("altAddress").c_str();
        cnf.sshArgs       = fwopt->getStr("sshArgs").c_str();
        cnf.activationCmd = fwopt->getStr("activationCmd").c_str();

        cnf.fwobj     = fw;
        cnf.fwbfile   = mw->db()->getFileName().c_str();
        cnf.conffile  = ofname;
        cnf.diff_file = QString(fw->getName().c_str())+".diff";
        cnf.wdir      = getFileDir( mw->getRCS()->getFileName() );
        cnf.diff_pgm  = Resources::platform_res[fw->getStr("platform")]->
            getResourceStr("/FWBuilderResources/Target/diff").c_str();

/* set this in instDialog now

        QString s=fwopt->getStr("firewall_dir").c_str();
        if (s.isEmpty()) s="/etc/fw";
        cnf.fwdir     = s;
*/

        cnf.diff_pgm = getPathToBinary(
            cnf.diff_pgm.toAscii().constData()).c_str();

#ifdef _WIN32
	cnf.diff_pgm = cnf.diff_pgm + ".exe";
#endif

        //instDialog *id = new instDialog( &cnf );

        //int exec_result=id->exec();

    } else
    {
        customScriptFlag=true;
        string inst_script=pis->getCommand();

        QString wdir = getFileDir( mw->getRCS()->getFileName() );


        args.push_back(inst_script.c_str());

        QString qs = pis->getArguments().c_str();
        args += qs.split(" ", QString::SkipEmptyParts);

        args.push_back("-f");
        args.push_back(mw->db()->getFileName().c_str());

        if (wdir!="")
        {
            args.push_back("-d");
            args.push_back(wdir);
        }

        args.push_back( QString("/%1/%2")
                        .arg(QString::fromUtf8(fw->getLibrary()->getName().c_str()))
                        .arg(fw->getPath(true).c_str() ) );
    }
    return true;
}

bool instDialog::prepareArgForCompiler(Firewall *fw)
{
    FWOptions *fwopt=fw->getOptionsObject();

/*
 * I should be able to specify custom compiler for firewall with
 * no platform (e.g. for experiments)
 */
    string compiler=fwopt->getStr("compiler");
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
        return false;
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

#if 0
// if we use WDir for the "-d" option for compiler
    QString wdir;
    if (st->getWDir().isEmpty())
    {
        QString of = rcs->getFileName();
        wdir = of.left( of.findRev('/',-1) );
    } else
    {
        wdir=st->getWDir();
    }
#endif

    QString wdir = getFileDir(mw->getRCS()->getFileName() );

    args.clear();

    args.push_back(compiler.c_str());

    QString qs = fwopt->getStr("cmdline").c_str();
    args += qs.split(" ", QString::SkipEmptyParts);

    args.push_back("-v");
    args.push_back("-f");
    args.push_back(mw->db()->getFileName().c_str());

    if (wdir!="")
    {
        args.push_back("-d");
        args.push_back(wdir);
    }

    QString ofname = QString::fromUtf8(fwopt->getStr("output_file").c_str());
    if (!ofname.isEmpty())
    {
        args.push_back("-o");
        args.push_back(ofname);
    }

/* there has been a change in v2.1: now resources are installed in
 * directory /usr/share/fwbuilder-2.1 (it used to be just
 * /usr/share/fwbuilder) Compilers that are packaged separately need
 * to know about this but I do not want to hard-code it. It is easier
 * to pass the path on the command line
 *
 * Update 01/16/06:
 *
 * We now package a copy of resource files with externally packaged
 * compilers (such as fwbuilder-pix), therefore flag "-r" is not
 * needed anyore
 */

#if 0
    args.push_back("-r");
    args.push_back(respath);
#endif

    args.push_back( QString::fromUtf8(fw->getName().c_str()) );
    return true;
}

/*
 * Adds one line of text to the log
 *
 */
void instDialog::addToLog(const QString &line)
{
    if (fwbdebug) qDebug("instDialog::addToLog");
    if (line.isEmpty()) return;

    if (currentLog)
    {
        QString txt = line;
        txt.replace(QRegExp("(Error:[^\n]*)\n"), 
                    QString("<b><font color=\"red\">\\1</font></b>\n"));
        txt.replace('\n', "<br>\n");
        currentLog->insertHtml( txt );
        currentLog->ensureCursorVisible();
    }
}

void instDialog::interpretLogLine(const QString &line)
{
    if (fwbdebug)
        qDebug("instDialog::interpretLogLine %s", line.toAscii().constData() );

    QStringList words = line.trimmed().split(" ");

    if (fwbdebug)
    {
        for (int i=0; i<words.size(); ++i)
            qDebug("instDialog::interpretLogLine words[%d]='%s'",
                   i, words[i].toAscii().constData());
    }

    if (words.first().indexOf("rule")>=0)
    {
        processedRules = words[1].toInt();
        currentProgressBar->setValue(processedRules);

        if (fwbdebug)
            qDebug("instDialog::interpretLogLine set progress bar current=%d",
                   processedRules);
    } else
    {
        if (words.first().indexOf("processing")>=0)
        {
            currentProgressBar->reset();
            totalRules = words[1].toInt();
            currentProgressBar->setMaximum(totalRules);
            processedRules = 0;

            if (fwbdebug)
                qDebug("instDialog::interpretLogLine set progress bar max=%d",
                       totalRules);
        } else
        {
            if (words.first().indexOf("Compiling")>=0)
            {
                currentLabel->setText(line.trimmed());
                currentProgressBar->reset();
            } else
            {
                if (line.indexOf("Compiled successfully")>=0)
                {
                    currentLabel->setText(line.trimmed());
                    currentProgressBar->setValue(currentProgressBar->maximum());
                }
            }
        }
    }
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,1);
}

void instDialog::readFromStdout()
{
    QString buf = proc.readAllStandardOutput();

    if (fwbdebug) qDebug("instDialog::readFromStdout: %s",
                         buf.toAscii().constData());

    addToLog(buf);

    bool endsWithLF = buf.endsWith("\n");
    QString lastLine = "";

    if (pendingLogLine.isEmpty() && buf.isEmpty()) return;

    // split on LF
    QStringList bufLines = buf.split("\n");

    if (fwbdebug)
    {
        qDebug("buf=%s",buf.toAscii().constData());
        qDebug("endsWithLF=%d",endsWithLF);
        qDebug("bufLines.size()=%d",bufLines.size());
    }

    // if buf ends with a LF character, the last element in the list is
    // an empty string
    if (endsWithLF && bufLines.last().isEmpty())  bufLines.pop_back();

    // if buf does not end with LF, last element in the list is
    // incomplete line of text
    if (!endsWithLF)
    {
        lastLine = bufLines.last();
        bufLines.pop_back();
    }

    // elements that are left in the list  are all complete lines of text
    for (QStringList::Iterator i=bufLines.begin(); i!=bufLines.end(); ++i)
    {
        interpretLogLine( pendingLogLine + *i );
        pendingLogLine = "";
    }

    pendingLogLine += lastLine;
}

/*
 * slot called by SSHSession in case of successfull termination
 */
void instDialog::installerSuccess()
{
    if( fwbdebug) qDebug("instDialog::installerSuccess");

    if (session->getErrorStatus())
    {
        installerError();
    } else
    {
        // session object is destroyed in stopSessionAndDisconnectSignals()
        // restartSession() calls stopSessionAndDisconnectSignals(),
        // then continueRun(). It seems the order in which slots are
        // called when we schedule two timer events with time =0
        // is different on different OS. On Linux (and windows?) slots
        // are called in the order they were scheduled, but on Mac
        // the order is reverse. Use slot restartSession to make
        // sure stopSessionAndDisconnectSignals() and  continueRun()
        // are called in the right order.
        
        // first, disconnect signals so that when session really
        // terminates it does not call any slots. (this happens
        // because SSHSession can emit one of two signals it uses to
        // indicate the end of the session even before the child
        // process finishes. So we get the signal, do our work here,
        // and then later get signal again. This is especially bad
        // because signal we get second time may indicate "success"
        // even though the session detected error and reported it via
        // signal before.
        
        disconnect(session,SIGNAL(sessionFinished_sign()),
                   this,SLOT(installerSuccess()));

        disconnect(session,SIGNAL(sessionFatalError_sign()),
                   this,SLOT(installerError()));

        if (fwbdebug) qDebug("schedule call to restartSession()");
        QTimer::singleShot( 0, this, SLOT(restartSession()));
        return;
    }

    if (fwbdebug) qDebug("schedule call to continueRun()");
    QTimer::singleShot( 0, this, SLOT(continueRun()) );
}

/*
 * slot called by SSHSession in case of fatal error
 */
void instDialog::installerError()
{
    if (fwbdebug) qDebug("instDialog::installerError  session=%p", session);

    // installerError is a slot that is called when SSHSession
    // detects fatal error. Since we need to destroy session in this case
    // and session object is destroyed in stopSessionAndDisconnectSignals(),
    // we can not call stopSessionAndDisconnectSignals() from the slot called
    // by signal emitted by the object we are about to destroy. Schedule
    // call to slot sessionCleanupOnError which will do this.

    // first, disconnect signals so that when session really
    // terminates it does not call any slots. (this happens
    // because SSHSession can emit one of two signals it uses to
    // indicate the end of the session even before the child
    // process finishes. So we get the signal, do our work here,
    // and then later get signal again. This is especially bad
    // because signal we get second time may indicate "success"
    // even though the session detected error and reported it via
    // signal before.
        
    disconnect(session,SIGNAL(sessionFinished_sign()),
               this,SLOT(installerSuccess()));

    disconnect(session,SIGNAL(sessionFatalError_sign()),
               this,SLOT(installerError()));

    QTimer::singleShot( 0, this, SLOT(sessionCleanupOnError()));
}

void instDialog::sessionCleanupOnError()
{
    addToLog( tr("Error: Terminating install sequence\n") );

    // session object is destroyed in stopSessionAndDisconnectSignals()
    // schedule call to stopSessionAndDisconnectSignals()
    // before calling finishInstall to properly terminate and clean up
    // session. This should be done  before calling installSelected
    // which is scheduled inside finishInstall()
    //QTimer::singleShot( 0, this, SLOT(stopSessionAndDisconnectSignals()));
    stopSessionAndDisconnectSignals();
    finishInstall(false);
}

void instDialog::finishInstall(bool success)
{
    if (fwbdebug) qDebug("instDialog::finishInstall success=%d", success);

    if (success)
    {
        mw->updateLastInstalledTimestamp(*opListIterator);
        opListMapping[*opListIterator]->setText(1,tr("Success"));
        processedFirewalls[*opListIterator].second=tr("Success");
        setSuccessState(opListMapping[*opListIterator]);
    }
    else
    {
        opListMapping[*opListIterator]->setText(1,tr("Error"));
        processedFirewalls[*opListIterator].second=tr("Error");
        setErrorState(opListMapping[*opListIterator]);
    }

    if (fwbdebug) qDebug("instDialog::finishInstall calling installNext()");

    installNext();

    if (fwbdebug) qDebug("instDialog::finishInstall done");

    setNextEnabled(1, false);
    setFinishEnabled(1, true);
}

void instDialog::restartSession()
{
    stopSessionAndDisconnectSignals();
    continueRun();
}

void instDialog::processExited(int res)
{
    if( fwbdebug) qDebug("instDialog::processExited exit code = %d", res);

    readFromStdout();

    if (rejectDialogFlag)
    {
        rejectDialogFlag = false;
        QDialog::reject();
        return;
    }

    if (!compileFlag && customScriptFlag)
    {
        if( fwbdebug) qDebug("Custom script installer");

        if(res==0)
        {
            mw->updateLastInstalledTimestamp(*opListIterator);
            processedFirewalls[*opListIterator].second="Success";
            opListMapping[*opListIterator]->setText(1,tr("Success"));
            setSuccessState(opListMapping[*opListIterator]);
        }
        else
        {
            processedFirewalls[*opListIterator].second="Error";
            opListMapping[*opListIterator]->setText(1,tr("Error"));
            setErrorState(opListMapping[*opListIterator]);
        }
        currentProgressBar->setValue(currentProgressBar->maximum () );
        qApp->processEvents();

        opListIterator++;

        if (opListIterator!=opList.end() && m_dialog->batchInstall->isChecked() && !stopProcessFlag)
        {
            QTimer::singleShot( 0, this, SLOT(installSelected()));
        }
        else
        {
            currentSaveButton->setEnabled(true);
            setNextEnabled(1, true);
            m_dialog->saveMCLogButton->setEnabled(true);
        }
        return;
    }


    if(res==0 && proc.state()==QProcess::NotRunning && !stopProcessFlag)
    {
        mw->updateLastCompiledTimestamp(*opListIterator);
        processedFirewalls[*opListIterator].first=tr("Success");
        opListMapping[*opListIterator]->setText(1,tr("Success"));
        setSuccessState(opListMapping[*opListIterator]);
    }
    else
    {
        if (!stopProcessFlag)
            addToLog( tr("Abnormal program termination\n") );
        else
            addToLog( tr("Program terminated by user\n") );

        // WARNING:
        // If compilation of the firewall failed we should not
        // install it
        //
        installMapping[*opListIterator]->setCheckState(Qt::Unchecked);
        processedFirewalls[*opListIterator].second=tr("Skipped");
        processedFirewalls[*opListIterator].first=tr("Error");
        opListMapping[*opListIterator]->setText(1,tr("Error"));
        setErrorState(opListMapping[*opListIterator]);
    }

    if (fwbdebug)
        qDebug( QString(" Current log buffer contents: '%1'").
                arg(currentLog->toPlainText()).toAscii().constData() );



    Firewall *f;
    QTreeWidgetItem* item;

    opListIterator++;

    while (opListIterator!=opList.end() && !stopProcessFlag)
    {
        if (currentFirewallsBar) currentFirewallsBar->setValue(++progress);
        qApp->processEvents();

        f=*opListIterator;
        item=opListMapping[f];
        currentFWLabel->setText(QString::fromUtf8(f->getName().c_str()));


        m_dialog->fwWorkList->scrollToItem( opListMapping[f]  );

        if(runCompile(f))
        {
            item->setText(1,tr("Compiling ..."));

            setInProcessState(item);

            return;
        }
        else
        {
            item->setText(1,tr("Failure"));

            setFailureState(item);

        }
        ++opListIterator;
    }
    if (currentFirewallsBar)
        currentFirewallsBar->setValue(currentFirewallsBar->maximum());

    if (currentStopButton)
    {
        currentStopButton->setText(tr("Recompile"));
        disconnect (currentStopButton , SIGNAL(clicked()), this ,SLOT(stopCompile()));
        connect(currentStopButton,SIGNAL(clicked()),this,SLOT(compileSelected()));
        currentStopButton->setEnabled(true);

    }
    currentSaveButton->setEnabled(true);
    if (operation==BATCH_COMPILE)
        setFinishEnabled(1, true);
    else
        setNextEnabled(1, true);

    currentSaveButton->setEnabled(true);

}

void instDialog::selectAllFirewalls()
{
    if (fwbdebug) qDebug("instDialog::selectAllFirewalls");
    if (operation==BATCH_INSTALL)selectAll(installMapping);
    selectAll(compileMapping);
    tableValueChanged(0,0);
}


void instDialog::deselectAllFirewalls()
{
    if (operation==BATCH_INSTALL)deselectAll(installMapping);
    deselectAll(compileMapping);
    tableValueChanged(0,0);
}

void instDialog::selectAll(t_tableMap &mapping)
{
    if (fwbdebug) qDebug("instDialog::selectAll");

    t_tableMap::iterator i;

    QTableWidgetItem *item;

    for(i=mapping.begin();i!=mapping.end();++i)
    {
        item=(*i).second;
        item->setCheckState(Qt::Checked);
    }
}
void instDialog::deselectAll(t_tableMap &mapping)
{
    if (fwbdebug) qDebug("instDialog::deselectAll");
    t_tableMap::iterator i;
    QTableWidgetItem *item;
    for(i=mapping.begin();i!=mapping.end();++i)
    {
        item=(*i).second;
        item->setCheckState(Qt::Unchecked);
    }
}
void instDialog::fillCompileOpList()
{
    m_dialog->fwWorkList->clear();
    opList.clear();
    processedFirewalls.clear();
    opListMapping.clear();

    Firewall * f;
    InstallFirewallViewItem * item;
    t_fwList::reverse_iterator i;
    for(i=firewalls.rbegin();i!=firewalls.rend();++i)
    {
        if(compileMapping[(*i)]->checkState() == Qt::Checked)
        {
            f=(*i);
            opList.push_front(f);
            item=new InstallFirewallViewItem(NULL,//m_dialog->fwWorkList,
                                     QString::fromUtf8(f->getName().c_str()),
                                     false);
            m_dialog->fwWorkList->insertTopLevelItem(0, item);

            opListMapping[f]=item;

            processedFirewalls[f]=make_pair("","");
        }
    }
    m_dialog->fwWorkList->resizeColumnToContents(0);
    m_dialog->fwWorkList->sortByColumn(0, Qt::AscendingOrder);
}

void instDialog::compileSelected()
{
    if (fwbdebug) qDebug("instDialog::compileSelected");
    setTitle(1,tr("Batch policy rules compilation"));

    currentLog = m_dialog->procLogDisplay;
    currentSaveButton = m_dialog->saveMCLogButton;
    currentSaveButton->setEnabled(true);
    currentStopButton = m_dialog->controlMCButton;
    currentProgressBar = m_dialog->compProgress;
    currentFirewallsBar = m_dialog->compFirewallProgress;
    currentLabel = m_dialog->infoMCLabel;
    currentFWLabel = m_dialog->fwMCLabel;
    currentSearchString = tr("Compiling rule sets for firewall: ");
    setNextEnabled(0, false);

    mw->fileSave();
    compileFlag=true;

    currentProgressBar->reset();
    currentProgressBar->setFormat("%v/%m");

    currentFirewallsBar->reset();
    currentFirewallsBar->setMaximum(opList.size());
    progress=0;
    stopProcessFlag=false;

    currentLog->clear();

    if (currentStopButton)
    {
        disconnect(currentStopButton, SIGNAL(clicked()),
                   this, SLOT(compileSelected()));
        connect(currentStopButton, SIGNAL(clicked()),
                this, SLOT(stopCompile()));

        currentStopButton->setText(tr("Stop"));
        currentStopButton->setEnabled(true);
    }

    Firewall *f;
    QTreeWidgetItem* item;

    opListIterator=opList.begin();

    while (opListIterator!=opList.end() && !stopProcessFlag)
    {
        if (currentFirewallsBar) currentFirewallsBar->setValue(++progress);
        qApp->processEvents();

        f=*opListIterator;
        item=opListMapping[f];
        currentFWLabel->setText(QString::fromUtf8(f->getName().c_str()));


        m_dialog->fwWorkList->scrollToItem( opListMapping[f]  );

        if(runCompile(f))
        {
            setInProcessState(item);
            item->setText(1, tr("Compiling ..."));

            return;
        }
        else
        {
            item->setText(1, tr("Failure"));
            setFailureState(item);
        }
        ++opListIterator;

        m_dialog->fwWorkList->update();
        /*m_dialog->fwWorkList->dataChanged ( m_dialog->fwWorkList->indexFromItem(item,0), m_dialog->fwWorkList->indexFromItem(item,1) );*/
    }

}
void instDialog::stopCompile()
{
    if( fwbdebug) qDebug("instDialog::stopCompile");
    stopProcessFlag=true;

    currentStopButton->setEnabled(false);

    proc.terminate();                                  //try to close proc.
    QTimer::singleShot( 1000, &proc, SLOT( kill() ) ); //if it doesn't respond, kill it
}

void instDialog::stopInstall()
{
    currentStopButton->setEnabled(false);
    stopProcessFlag=true;
}

void instDialog::fillLastList()
{
    m_dialog->lastListView->clear();

    QTreeWidgetItem *item;
    Firewall* f;
    t_procMess m;

    for (map<Firewall*, t_procMess>::iterator i=processedFirewalls.begin();
            i!=processedFirewalls.end(); ++i)
    {
        f=(*i).first;
        m=(*i).second;

        item = new QTreeWidgetItem(
            m_dialog->lastListView,
            QStringList(QString::fromUtf8(f->getName().c_str())));

        item->setText(1, m.first);
        item->setText(2, m.second);
    }

    m_dialog->lastListView->setSortingEnabled(true);
    m_dialog->lastListView->sortByColumn(0, Qt::DescendingOrder);
}

bool instDialog::runInstall(Firewall *fw)
{
    if (fwbdebug) qDebug("instDialog::runInstall");
    if (customScriptFlag)
    {
        if (fwbdebug) qDebug("custom script");
        summary();

        addToLog(args.join(" "));

        QString path = args[0];
        args.pop_front();
        proc.start(path, args);

        if (!proc.waitForStarted())
        {
            addToLog( tr("Error: Failed to start program\n") );
            return false;
        }

        args.push_front(path); //return to previous state
    }
    else
    {
        if (fwbdebug) qDebug("build-in installer");
        return doInstallPage(fw);
    }
    return true;
}

void instDialog::fillInstallOpList()
{
    if (fwbdebug) qDebug("instDialog::fillInstallOpList");
    m_dialog->fwWorkList->clear();
    opListMapping.clear();
    opList.clear();

    InstallFirewallViewItem * item;
    Firewall * f;

    t_fwList::reverse_iterator i;
    for(i=firewalls.rbegin();i!=firewalls.rend();++i)
    {
        f=(*i);

        if (installMapping[f]->checkState() == Qt::Checked)
        {
            opList.push_front(f);
            item=new InstallFirewallViewItem(NULL,
                                     QString::fromUtf8(f->getName().c_str()),
                                     false);

            m_dialog->fwWorkList->insertTopLevelItem(0, item);

            opListMapping[f]=item;
            if (processedFirewalls.find(f)==processedFirewalls.end())
                processedFirewalls[f]=make_pair("","");
        }
    }
    m_dialog->fwWorkList->resizeColumnToContents(0);
    m_dialog->fwWorkList->sortByColumn(0, Qt::AscendingOrder);
}

void instDialog::initInstall()
{
    if (fwbdebug)
        qDebug("instDialog::initInstall()");

    currentFirewallsBar = m_dialog->compFirewallProgress;
    currentFirewallsBar->reset();
    currentFirewallsBar->setMaximum(opList.size());
    currentStopButton = m_dialog->controlMCButton;
    currentStopButton->setText(tr("Stop"));

    disconnect(currentStopButton, SIGNAL(clicked()));
    connect(currentStopButton, SIGNAL(clicked()),
            this, SLOT(stopInstall()));

    currentSaveButton = m_dialog->saveMCLogButton;
    currentLog = m_dialog->procLogDisplay;
    currentProgressBar = m_dialog->compProgress;
    currentLabel = m_dialog->fwMCLabel;
    currentLog->clear();
    currentSearchString = tr("Install firewall: ");
    m_dialog->infoMCLabel->setText("");
    progress = 0;
    stopProcessFlag = false;
}

void instDialog::installSelected()
{
    if (fwbdebug) qDebug("instDialog::installSelected");
    if (fwbdebug) qDebug(("firewall:"+(*opListIterator)->getName()).c_str());

    setTitle(1, tr("Installing firewalls"));
    setNextEnabled(1, false);
    setNextEnabled(1, false);
    setFinishEnabled(1, true);
    m_dialog->saveMCLogButton->setEnabled(true);

    bool fPix=false,fCustInst=true;

    if (opListIterator==opList.begin() && m_dialog->batchInstall->isChecked())
    {
        // check if this is PIX and if we use custom
        // install script, set flags fPix and fCustInst accordingly
        analyseInstallQueue(fPix, fCustInst);
    }
    while ( opListIterator!=opList.end())
    {
        currentSaveButton->setEnabled(true);
        currentProgressBar->reset();
        currentProgressBar->setMaximum(100);
        currentProgressBar->setFormat("%p%");

        currentLabel->setText(QString::fromUtf8((*opListIterator)->getName().c_str()));
        compileFlag=false;

        resetInstallSSHSession();
        currentFirewallsBar->setValue(++progress);

        addToLog("<hr>");
        addToLog(QString("<b>") + currentSearchString +
                   QString::fromUtf8((*opListIterator)->getName().c_str())+"</b>");
        addToLog("\n");
        //qApp->processEvents();


        if (testFirewall(*opListIterator))
        {
            opListMapping[*opListIterator]->setText(1, tr("Installing ..."));
            setInProcessState(opListMapping[*opListIterator]);
            //qApp->processEvents();

            if (customScriptFlag && fCustInst)
            { // custom installer
                if (fwbdebug) qDebug("custom install script.");

            }
            else
            { // buildin installer
                if (fwbdebug) qDebug("buildin installer");
                // Show options dialog
                if (!m_dialog->batchInstall->isChecked() ||
                    opListIterator==opList.begin())
                {
                    if (dlg)
                    {
                        delete dlg;
                        dlg=NULL;
                    }

                    prepareInstallerOptions();

                    if (m_dialog->batchInstall->isChecked())
                        dlg = new instBatchOptionsDialog(this, &cnf);
                    else
                        dlg = new instOptionsDialog(this, &cnf);

                    if (dlg->exec()==QDialog::Rejected)
                    {
                        delete dlg;
                        dlg = NULL;

                        if (m_dialog->batchInstall->isChecked())
                        {
                            stopProcessFlag=true;
                            showPage(0);
                            return;
                        }
                        else
                        {
                            processedFirewalls[*opListIterator].second =
                                "Cancelled";
                            opListMapping[*opListIterator]->setText(
                                1, tr("Cancelled"));
                            setFailureState(opListMapping[*opListIterator]);

                            installNext();

                            return;
                        }
                    }
                    // clear aternative address in the dialog if this is batch install.
                    if (m_dialog->batchInstall->isChecked())
                        dlg->m_dialog->altAddress->setText("");
                    setTitle(
                        1,
                        QObject::tr("Installing policy rules on firewall '%1'").
                        arg((*opListIterator)->getName().c_str() ));
                    //qApp->processEvents();
                    if (!runInstall(*opListIterator))
                    {
                        if (fwbdebug) qDebug("start error");
                        processedFirewalls[*opListIterator].second = 
                            "start error";
                        opListMapping[*opListIterator]->setText(
                            1, tr("Failure"));
                        setFailureState(opListMapping[*opListIterator]);
                        opListIterator++;
                        setNextEnabled(1, true);
                        m_dialog->saveMCLogButton->setEnabled(true);
                        if (opList.end()!=opListIterator &&
                            m_dialog->batchInstall->isChecked()) showPage(2);
                        return;
                    }

                    return;
                }
            }
            if (!runInstall(*opListIterator))
            {
                processedFirewalls[*opListIterator].second="start error";
                opListMapping[*opListIterator]->setText(1,tr("Failure"));
                setFailureState(opListMapping[*opListIterator]);
                opListIterator++;
                setNextEnabled(1, true);
            }
            else
            {
                return;
            }
        }
        else
        {
            processedFirewalls[*opListIterator].second="init error";
            opListMapping[*opListIterator]->setText(1,tr("Failure"));
            setFailureState(opListMapping[*opListIterator]);
            opListIterator++;
            setNextEnabled(1, true);
        }
    }
}

void instDialog::findFirewallInCompileLog(QTreeWidgetItem* item)
{
    if (fwbdebug) qDebug("instDialog::findFirewallInCompileLog");
    Firewall *fw;
    //int p=1,i=0;

    m_dialog->detailMCframe->show();
    qApp->processEvents();
    fw=findFirewallbyListItem(item);
    m_dialog->procLogDisplay->moveCursor( QTextCursor::End );
    m_dialog->procLogDisplay->find(currentSearchString +
            QString::fromUtf8(fw->getName().c_str()),
            QTextDocument::FindWholeWords |
            QTextDocument::FindCaseSensitively |
            QTextDocument::FindBackward);
}

Firewall * instDialog::findFirewallbyListItem(QTreeWidgetItem *item)
{
    Firewall * res=NULL;
    t_listMap::iterator i;

    for(i=opListMapping.begin();i!=opListMapping.end();++i)
    {
        if ((*i).second==item)
        {
            res=(*i).first;
            break;
        }
    }
    return res;
}

Firewall * instDialog::findFirewallbyTableItem(QTableWidgetItem *item)
{
    Firewall * res=NULL;
    t_tableMap::iterator i;

    for(i=compileMapping.begin();i!=compileMapping.end();++i)
    {
        if ((*i).second==item)
        {
            res=(*i).first;
            return res;
        }
    }

    for(i=installMapping.begin();i!=installMapping.end();++i)
    {
        if ((*i).second==item)
        {
            res=(*i).first;
            return res;
        }
    }

    return res;
}
void instDialog::showSelected()
{

    QTableWidgetItem* item;
    Firewall *f;

    t_fwList::iterator i;
    bool sel;

    for(i=firewalls.begin();i!=firewalls.end();++i)
    {
        sel=false;

        f=(*i);
        item=compileMapping[f];
        sel|=item->checkState()==Qt::Checked;

        item=installMapping[f];
        sel|=item->checkState()==Qt::Checked;

        if(!sel )
        {
            if (showSelectedFlag)
            {
                m_dialog->selectTable->showRow(item->row());
            }
            else
            {
                m_dialog->selectTable->hideRow(item->row());
            }
        }
    }
    if (showSelectedFlag)
    {
        m_dialog->showSelButton->setText(tr("Show selected"));
        m_dialog->pushButton16->setEnabled(true);
        m_dialog->pushButton17->setEnabled(true);
    }
    else
    {
        m_dialog->showSelButton->setText(tr("Show all"));
        m_dialog->pushButton16->setEnabled(false);
        m_dialog->pushButton17->setEnabled(false);
    }
    showSelectedFlag=!showSelectedFlag;

}

void instDialog::tableValueChanged(int row, int col)
{
    if (creatingTable) return;
    if (fwbdebug) qDebug("instDialog::tableValueChanged");
    QTableWidgetItem *item;
    Firewall *f;

    item=m_dialog->selectTable->item(row,col);
    f=findFirewallbyTableItem(item);


    if (col==0)
    { // Compilation flag has been changed
        if (
                (item->checkState()==Qt::Unchecked) &&
                f->getInt("lastCompiled")==0 &&
                (installMapping[f]->checkState()==Qt::Checked))
        {
            installMapping[f]->setCheckState(Qt::Unchecked);
        }
    }
    else if (col==1)
    { // Installation flag has been changed
        if (
                (item->checkState()==Qt::Checked) &&
                f->getInt("lastCompiled")==0)
        {
            compileMapping[f]->setCheckState(Qt::Checked);
        }

    }

    setNextEnabled(0, isTableHasChecked());
}

bool instDialog::isTableHasChecked()
{
    QTableWidgetItem *item;
    Firewall *f;

    t_fwList::iterator i;

    bool res=false;

    for(i=firewalls.begin();i!=firewalls.end();++i)
    {
        f=(*i);
        item=compileMapping[f];
        if(!item) return false;
        if(item->checkState()==Qt::Checked) res = true;

        item=installMapping[f];
        if(!item) return false;
        if(item->checkState()==Qt::Checked) res = true;
    }
    return res;
}

void instDialog::analyseInstallQueue(bool &fPix, bool &fCustInst)
{
    if (fwbdebug) qDebug("instDialog::analyseInstallQueue");
    Firewall *f;
    //FWOptions *fwopt;
    Management *mgmt;
    PolicyInstallScript *pis;

    fPix=false;
    fCustInst=true;

    t_fwList::iterator i;
    for(i=opList.begin(); i!=opList.end(); ++i)
    {
        f=(*i);
        //fwopt=f->getOptionsObject();
        mgmt=f->getManagementObject();
        pis   = mgmt->getPolicyInstallScript();

        fPix = fPix || f->getStr("platform")=="pix" || f->getStr("platform")=="fwsm" || f->getStr("platform")=="iosacl";
        fCustInst =  fCustInst && !( pis->getCommand()=="" );

        if (fwbdebug)
        {
            qDebug(("f:"+f->getName()).c_str());
            qDebug(("p:"+f->getStr("platform")).c_str());
            qDebug((QString("fPix:")+(fPix?"true":"false")).toAscii().constData());
        }

        if (fPix && !fCustInst) return;// nothing can change if we continue loop
    }
}

void instDialog::clearReqFirewalls()
{
    reqFirewalls.clear();
}

void instDialog::addReqFirewall(Firewall *f)
{
    reqFirewalls.insert(f);
}
