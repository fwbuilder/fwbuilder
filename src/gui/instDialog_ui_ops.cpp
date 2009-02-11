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

#include "instDialog.h"
#include "FirewallInstaller.h"
#include "FWBSettings.h"
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

void instDialog::enableStopButton()
{
    currentStopButton->setText(tr("Stop"));
    currentStopButton->setEnabled(true);
}

void instDialog::disableStopButton()
{
    currentStopButton->setEnabled(false);
}


/*
 * The following color and font manipulations are subject to QT bug
 * http://trolltech.no/developer/task-tracker/index_html?method=entry&id=212207
 *
 * This requires QT 4.4.1 or 4.3
 */
void instDialog::setSuccessState(QTreeWidgetItem *item)
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

void instDialog::setFailureState(QTreeWidgetItem *item)
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

void instDialog::setErrorState(QTreeWidgetItem *item)
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

void instDialog::setInProcessState(QTreeWidgetItem *item)
{
    QBrush b = item->foreground(1);
    b.setColor(Qt::black);
    item->setForeground(1,b);
    item->setForeground(0,b);

    QFont f = item->font(1);
    f.setBold(true);
    item->setFont(1,f);
    item->setFont(0,f);
}

void instDialog::opSuccess(Firewall *fw)
{
    opListMapping[(fw)->getId()]->setText(1,tr("Success"));
    setSuccessState(opListMapping[(fw)->getId()]);
    currentLabel->setText("");
}

void instDialog::opError(Firewall *fw)
{
    opListMapping[(fw)->getId()]->setText(1, tr("Error"));
    setErrorState(opListMapping[(fw)->getId()]);
    currentLabel->setText("");
}
    
void instDialog::opCancelled(Firewall *fw)
{
    opListMapping[(fw)->getId()]->setText(1, tr("Cancelled"));
    setErrorState(opListMapping[(fw)->getId()]);
    currentLabel->setText("");
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
        st->setValue(SETTINGS_PATH_PREFIX"/Installer/ShowDetails",false);
    }
    else
    {
        m_dialog->detailsButton->setText(tr("Hide details"));
        m_dialog->detailMCframe->show();
        st->setValue(SETTINGS_PATH_PREFIX"/Installer/ShowDetails",true);
    }
}

void instDialog::prepareInstConf(Firewall *)
{
    if (fwbdebug) qDebug("instDialog::prepareInstConf");
}

void instDialog::storeInstallerOptions()
{
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/incr",    cnf.incremental);
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/savediff",cnf.save_diff);
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/saveStandby",cnf.saveStandby);
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/dryrun"  ,cnf.dry_run);
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/quiet",   cnf.quiet);
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/verbose", cnf.verbose);
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/stripComments",
                 cnf.stripComments);
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/compressScript",
                 cnf.compressScript);
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/copyFWB",  cnf.copyFWB);
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/testRun",  cnf.testRun);
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/rollback", cnf.rollback);
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/rollbackTime",
                 cnf.rollbackTime);
    st->setValue(SETTINGS_PATH_PREFIX"/Installer/canceRollbackIfSuccess",
                 cnf.cancelRollbackIfSuccess);
}

void instDialog::summary()
{
    addToLog( "<hr>" + QObject::tr("<b>Summary:</b>\n") );
    addToLog( QObject::tr("* firewall name : %1\n")
              .arg(QString::fromUtf8(cnf.fwobj->getName().c_str())) );
    addToLog( QObject::tr("* user name : %1\n")
              .arg(cnf.user));
    addToLog( QObject::tr("* management address : %1\n").arg(cnf.maddr) );
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
            QObject::tr("* Configuration diff will be saved in file %1\n").
            arg(cnf.diff_file));
    }
    if (cnf.dry_run)
    {
        addToLog(
            QObject::tr(
                "* Commands will not be executed on the firewall\n"));
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

        time_t lm = f->getInt("lastModified");
        time_t lc = f->getInt("lastCompiled");
        time_t li = f->getInt("lastInstalled");

        citem = new QTableWidgetItem;
        citem->setText(QString::fromUtf8(f->getName().c_str()));

        m_dialog->selectTable->setItem(row,2,citem);
        //m_dialog->selectTable->setColumnReadOnly(2,true);

        // in fact, if someone use same names for several libraries,
        // additional collumn with library names doesn't help to
        // identify a firewall
        if (!show_library && tmp_libname != "" && tmp_libname != f->getLibraryName())
            show_library = true;
        tmp_libname = f->getLibraryName();

        citem = new QTableWidgetItem;
        citem->setText(QString::fromUtf8(tmp_libname.c_str()));
        m_dialog->selectTable->setItem(row,3,citem);
        //m_dialog->selectTable->setColumnReadOnly(3,true);

        citem=new QTableWidgetItem; //usual type
        bool checked = (f->needsCompile() && reqFirewalls.empty() && !f->getInactive()) ||
                (!reqFirewalls.empty() && reqFirewalls.find(f)!=reqFirewalls.end());
        citem->setCheckState(checked?Qt::Checked:Qt::Unchecked);
        m_dialog->selectTable->setItem(row,0,citem);
        compileMapping[f]=citem;

        citem = new QTableWidgetItem; //usual type
        checked = (operation==BATCH_INSTALL) &&
                ((f->needsInstall() && reqFirewalls.empty() && !f->getInactive()) ||
                (!reqFirewalls.empty() && reqFirewalls.find(f)!=reqFirewalls.end()));
        citem->setCheckState(checked?Qt::Checked:Qt::Unchecked);
        m_dialog->selectTable->setItem(row,1,citem);
        installMapping[f]=citem;

        dt.setTime_t(lm);
        citem = new QTableWidgetItem;
        citem->setText((lm)?dt.toString():QString("Never"));
        m_dialog->selectTable->setItem(row,4,citem);

        dt.setTime_t(lc);
        citem = new QTableWidgetItem;
        citem->setText((lc)?dt.toString():QString("Never"));
        m_dialog->selectTable->setItem(row,5,citem);

        dt.setTime_t(li);
        citem = new QTableWidgetItem;
        citem->setText((li)?dt.toString():QString("Never"));
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
        txt.replace(QRegExp("(Error(:| )[^\n]*)\n"), 
                    QString("<b><font color=\"red\">\\1</font></b>\n"));
        txt.replace(QRegExp("(Abnormal[^\n]*)\n"), 
                    QString("<b><font color=\"red\">\\1</font></b>\n"));

        // the following regex matches assertion errors
        txt.replace(QRegExp("(fwb_[a-z]{1,}: \\S*\\.cpp:\\d{1,}: .*: Assertion .* failed.)"), 
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
        bool ok;
        processedRules = words[1].toInt(&ok);
        if (ok)
            currentProgressBar->setValue(processedRules);

        if (fwbdebug)
            qDebug("instDialog::interpretLogLine set progress bar current=%d",
                   processedRules);
    } else
    {
        if (words.first().indexOf("processing")>=0)
        {
            currentProgressBar->reset();
            bool ok;
            int totalRules = words[1].toInt(&ok);
            if (ok)
                currentProgressBar->setMaximum(totalRules);
            processedRules = 0;

            if (fwbdebug)
                qDebug("instDialog::interpretLogLine set progress bar max=%d",
                       totalRules);
        } else
        {
            if (words.first().indexOf("Compiling")>=0)
            {
                //currentLabel->setText(line.trimmed());
                currentProgressBar->reset();
            } else
            {
                if (line.indexOf("Compiled successfully")>=0)
                {
                    //currentLabel->setText(line.trimmed());
                    currentProgressBar->setValue(currentProgressBar->maximum());
                    if (fwbdebug)
                        qDebug("instDialog::interpretLogLine set progress "
                               "bar to max");
                }
            }
        }
    }
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents,1);
}

void instDialog::readFromStdout()
{
    char buf[2048];
    int read_status = 0;
    while ((read_status = proc.readLine(buf, sizeof(buf)))>0)
    {
        if (fwbdebug)
        {
            qDebug("instDialog::readFromStdout: read_status=%d buf=%s",
                   read_status, buf);
        }
        addToLog(buf);
        interpretLogLine(buf);
    }
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
    compile_fw_list.clear();
    t_fwList::iterator i;
    for(i=firewalls.begin(); i!=firewalls.end(); ++i)
    {
        if(compileMapping[*i]->checkState() == Qt::Checked)
            compile_fw_list.push_back(*i);
    }
    compile_list_initial_size = compile_fw_list.size();
}

void instDialog::fillCompileUIList()
{
    m_dialog->fwWorkList->clear();
    opListMapping.clear();

    Firewall * f;
    InstallFirewallViewItem * item;
    list<Firewall*>::iterator i;
    for(i=compile_fw_list.begin(); i!=compile_fw_list.end(); ++i)
    {
        f=(*i);
        item = new InstallFirewallViewItem(
            NULL,//m_dialog->fwWorkList,
            QString::fromUtf8(f->getName().c_str()),
            false);

        m_dialog->fwWorkList->insertTopLevelItem(0, item);

        opListMapping[f->getId()] = item;
    }

    m_dialog->fwWorkList->resizeColumnToContents(0);
    m_dialog->fwWorkList->sortByColumn(0, Qt::AscendingOrder);
}

void instDialog::fillInstallOpList()
{
    if (fwbdebug) qDebug("instDialog::fillInstallOpList");
    install_fw_list.clear();
    t_fwList::iterator i;
    for(i=firewalls.begin(); i!=firewalls.end(); ++i)
    {
        if (installMapping[*i]->checkState() == Qt::Checked)
            install_fw_list.push_back(*i);
    }
    install_list_initial_size = install_fw_list.size();
}

void instDialog::fillInstallUIList()
{
    if (fwbdebug) qDebug("instDialog::fillInstallUIList");
    m_dialog->fwWorkList->clear();
    opListMapping.clear();

    Firewall * f;
    InstallFirewallViewItem * item;
    list<Firewall*>::iterator i;
    for(i=install_fw_list.begin(); i!=install_fw_list.end(); ++i)
    {
        f=(*i);
        item = new InstallFirewallViewItem(
            NULL,
            QString::fromUtf8(f->getName().c_str()),
            false);

        m_dialog->fwWorkList->insertTopLevelItem(0, item);
        opListMapping[f->getId()] = item;
    }
    m_dialog->fwWorkList->resizeColumnToContents(0);
    m_dialog->fwWorkList->sortByColumn(0, Qt::AscendingOrder);
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
    std::map<int,QTreeWidgetItem*>::iterator i;

    for(i=opListMapping.begin(); i!=opListMapping.end(); ++i)
    {
        if ((*i).second==item)
        {
            int id = (*i).first;
            res = Firewall::cast(mw->db()->findInIndex(id));
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
    showSelectedFlag = !showSelectedFlag;
}

void instDialog::tableValueChanged(int row, int col)
{
    if (creatingTable) return;
    if (fwbdebug) qDebug("instDialog::tableValueChanged");
    QTableWidgetItem *item;
    Firewall *f;

    item = m_dialog->selectTable->item(row,col);
    f = findFirewallbyTableItem(item);

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

    setNextEnabled(0, tableHasChecked());
}

bool instDialog::tableHasChecked()
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

void instDialog::clearReqFirewalls()
{
    reqFirewalls.clear();
}

void instDialog::addReqFirewall(Firewall *f)
{
    reqFirewalls.insert(f);
}

/*
 * getInstOptions() fills attributes of the cnf object
 */
bool instDialog::getInstOptions(Firewall *fw)
{
    cnf.fwobj = fw;

    readInstallerOptionsFromSettings();
    readInstallerOptionsFromFirewallObject(fw);

    if (!m_dialog->batchInstall->isChecked())
    {
        instOptionsDialog *inst_opt_dlg = new instOptionsDialog(this, &cnf);
        if (inst_opt_dlg->exec()==QDialog::Rejected)
        {
            delete inst_opt_dlg;
            return false;
        }
        readInstallerOptionsFromDialog(fw, inst_opt_dlg);
    }

    completeInstallerOptions();
    return true;
}

/*
 * getBatchInstOptions() fills attributes of the cnf object for batch install
 */
bool instDialog::getBatchInstOptions()
{
    cnf.fwobj = NULL;

    readInstallerOptionsFromSettings();

    instBatchOptionsDialog *inst_opt_dlg = new instBatchOptionsDialog(this,
                                                                      &cnf);
    if (inst_opt_dlg->exec()==QDialog::Rejected)
    {
        delete inst_opt_dlg;
        stopProcessFlag = true;
        showPage(0);
        return false;
    }
    // clear aternative address in the dialog
    inst_opt_dlg->m_dialog->altAddress->setText("");
    readInstallerOptionsFromDialog(NULL, inst_opt_dlg);
    completeInstallerOptions();
    return true;
}

void instDialog::readInstallerOptionsFromSettings()
{
    if (fwbdebug) qDebug("instDialog::readInstallerOptionsFromSettings");

    fwb_prompt="--**--**--";

    cnf.batchInstall = m_dialog->batchInstall->isChecked();

    cnf.incremental= st->value(SETTINGS_PATH_PREFIX"/Installer/incr").toBool();
    cnf.save_diff = st->value(SETTINGS_PATH_PREFIX"/Installer/savediff").toBool();
    cnf.saveStandby = st->value(SETTINGS_PATH_PREFIX"/Installer/saveStandby").toBool();
    cnf.dry_run = st->value(SETTINGS_PATH_PREFIX"/Installer/dryrun").toBool();
    cnf.quiet = st->value(SETTINGS_PATH_PREFIX"/Installer/quiet").toBool();
    cnf.verbose = st->value(SETTINGS_PATH_PREFIX"/Installer/verbose" ).toBool();
    cnf.stripComments  = st->value(SETTINGS_PATH_PREFIX"/Installer/stripComments").toBool();
    cnf.compressScript = st->value(SETTINGS_PATH_PREFIX"/Installer/compressScript").toBool();
    cnf.copyFWB = st->value(SETTINGS_PATH_PREFIX"/Installer/copyFWB").toBool();
    cnf.testRun = st->value(SETTINGS_PATH_PREFIX"/Installer/testRun").toBool();
    cnf.rollback = st->value(SETTINGS_PATH_PREFIX"/Installer/rollback").toBool();
    cnf.rollbackTime = st->value(SETTINGS_PATH_PREFIX"/Installer/rollbackTime").toInt();
    cnf.cancelRollbackIfSuccess =
        st->value(SETTINGS_PATH_PREFIX"/Installer/canceRollbackIfSuccess").toBool();
}

void instDialog::readInstallerOptionsFromFirewallObject(Firewall *fw)
{
    FWOptions *fwopt = NULL;
    if (fw)
    {
        fwopt = fw->getOptionsObject();
        cnf.user = fwopt->getStr("admUser").c_str();
        QString aaddr = fwopt->getStr("altAddress").c_str();
        if (!aaddr.isEmpty()) cnf.maddr = aaddr;
        else
        {
            const InetAddr *mgmt_addr = cnf.fwobj->getManagementAddress();
            if (mgmt_addr)
                cnf.maddr = mgmt_addr->toString().c_str();
            else 
                cnf.maddr = "";
        }
        if (fwbdebug) qDebug("management address: %s",
                             cnf.maddr.toAscii().constData());
        /*
         * if user requested test run, store firewall script in a temp
         * file.  Always store it in a temp file on linksys
         */
        QString s;

        /* user_can_change_install_dir */
        bool uccid = Resources::getTargetOptionBool(
            cnf.fwobj->getStr("host_OS"),"user_can_change_install_dir");

        if (uccid) s = fwopt->getStr("firewall_dir").c_str();

        if (s.isEmpty()) s = Resources::getTargetOptionStr(
            cnf.fwobj->getStr("host_OS"), "activation/fwdir").c_str();

        cnf.fwdir = s;

        cnf.conffile = FirewallInstaller::getGeneratedFileFullPath(fw);
        cnf.fwbfile = mw->db()->getFileName().c_str();
        cnf.wdir = getFileDir( mw->getRCS()->getFileName() );
        cnf.diff_file = QString(cnf.fwobj->getName().c_str())+".diff";
        cnf.diff_pgm = Resources::platform_res[cnf.fwobj->getStr("platform")]->
            getResourceStr("/FWBuilderResources/Target/diff").c_str();
        cnf.diff_pgm = getPathToBinary(
            cnf.diff_pgm.toAscii().constData()).c_str();
#ifdef _WIN32
        cnf.diff_pgm = cnf.diff_pgm + ".exe";
#endif
        cnf.sshArgs = fwopt->getStr("sshArgs").c_str();
        cnf.scpArgs = fwopt->getStr("scpArgs").c_str();
        cnf.activationCmd = fwopt->getStr("activationCmd").c_str();

        cnf.rollbackTimeUnit =
            Resources::getTargetOptionStr(cnf.fwobj->getStr("host_OS"),
                                          "activation/timeout_units").c_str();
    }
}

void instDialog::readInstallerOptionsFromDialog(Firewall *fw,
                                                instOptionsDialog *dlg)
{
    if (fwbdebug) qDebug("instDialog::readInstallerOptionsFromDialog");

    cnf.fwobj = fw;
    FWOptions *fwopt = NULL;
    if (fw) fwopt = cnf.fwobj->getOptionsObject();

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

    QString aaddr = dlg->m_dialog->altAddress->text();
    if (!aaddr.isEmpty())
    {
/* alternative address can also be putty session name. In any case,
 * leave it up to ssh to resolve it and signal an error if it can't be
 * resolved ( Putty session name does not have to be in DNS at all ).
 */
        cnf.maddr = aaddr;
        if (fwbdebug)
            qDebug("alternative addr %s", aaddr.toAscii().constData());
    }

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
    cnf.cancelRollbackIfSuccess =
        dlg->m_dialog->cancelRollbackIfSuccess->isChecked();

    storeInstallerOptions();
}

void instDialog::completeInstallerOptions()
{
/* check for a common error when multiple interfaces are marked as
 * 'management'
 */
    if (cnf.fwobj)
    {
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
                QObject::tr("Only one interface of the firewall '%1' "
                            "must be marked as management interface.\n")
                .arg(QString::fromUtf8(cnf.fwobj->getName().c_str())) );
            return;
        }
        if (nmi==0)
        {
            addToLog(
                QObject::tr("One of the interfaces of the firewall '%1' "
                            "must be marked as management interface.\n")
                .arg(QString::fromUtf8(cnf.fwobj->getName().c_str())));
            return;
        }
        if (cnf.maddr == "" ||
            cnf.maddr == QString(InetAddr::getAny().toString().c_str()))
        {
            addToLog(
                QObject::tr("Management interface does not have IP address, "
                            "can not communicate with the firewall.\n") );
            return;
        }
    }
}

