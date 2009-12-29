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
#include <qpixmapcache.h>

#include <QPixmap>
#include <QDateTime>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QTextBlockFormat>
#include <QBrush>
#include <QTextFormat>
#include <QtDebug>

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Cluster.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"
#include "fwbuilder/StateSyncClusterGroup.h"

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

bool instDialog::checkIfNeedToCompile(Firewall *fw)
{
    return (fw->needsCompile() && reqFirewalls.empty() && !fw->getInactive()) ||
           (!reqFirewalls.empty() && reqFirewalls.find(fw)!=reqFirewalls.end());
}

bool instDialog::checkIfNeedToInstall(Firewall *fw)
{
    return (operation==BATCH_INSTALL) &&
        ((fw->needsInstall() && reqFirewalls.empty() && !fw->getInactive()) ||
         (!reqFirewalls.empty() && reqFirewalls.find(fw)!=reqFirewalls.end()));
}

QTreeWidgetItem* instDialog::createTreeItem(QTreeWidgetItem* parent,
                                            Firewall *fw)
{
    QTreeWidgetItem* item;
    QStringList sl;
    sl.push_back(fw->getName().c_str());

    if (parent)
        item = new QTreeWidgetItem(parent, sl);
    else
        item = new QTreeWidgetItem(sl);

    QString icn_filename = (":/Icons/" + fw->getTypeName() + "/icon").c_str();
    QPixmap pm;
    if ( ! QPixmapCache::find(icn_filename, pm))
    {
        pm.load(icn_filename);
        QPixmapCache::insert(icn_filename, pm);
    }
    item->setIcon(0, QIcon(pm));

    item->setData(0, Qt::UserRole, QVariant(fw->getId()));

    // Mark cluster members
    // If parent!=NULL, new tree item corresponds to the cluster member
    item->setData(1, Qt::UserRole, QVariant(parent!=NULL));

    // it is useful to know how many members does this cluster have. If this is
    // not a cluster, store 0
    list<Firewall*> members;
    if (Cluster::isA(fw))
        Cluster::cast(fw)->getMembersList(members);
    int num_members = members.size();

    item->setData(2, Qt::UserRole, QVariant(num_members));

    return item;
}

void instDialog::setFlags(QTreeWidgetItem* item)
{
    int obj_id = item->data(0, Qt::UserRole).toInt();
    Firewall *fw = Firewall::cast(project->db()->findInIndex(obj_id));
    QTreeWidgetItem* parent = item->parent();

    time_t lm = fw->getInt("lastModified");
    time_t lc = fw->getInt("lastCompiled");
    time_t li = fw->getInt("lastInstalled");
    QDateTime dt;

    // need to skip the secondary cluster members if platform only
    // allows installations on the primary (e.g. PIX).  Note that
    // platform attribute must be the same in the cluster and member
    // firewalls objects. See #998

    string platform = fw->getStr("platform");
    bool install_only_on_primary_member = Resources::getTargetCapabilityBool(
        platform, "install_only_on_primary");

    Cluster *cluster = NULL;
    FWObject *master_interface = NULL;

    if (parent)
    {
        int obj_id = parent->data(0, Qt::UserRole).toInt();
        cluster = Cluster::cast(project->db()->findInIndex(obj_id));
        if (cluster)
        {
            FWObject *state_sync_group =
                cluster->getFirstByType(StateSyncClusterGroup::TYPENAME);
            string master_id = state_sync_group->getStr("master_iface");
            for (FWObjectTypedChildIterator grp_it =
                     state_sync_group->findByType(FWObjectReference::TYPENAME);
                 grp_it != grp_it.end(); ++grp_it)
            {
                FWObject *iface = FWObjectReference::getObject(*grp_it);
                if (FWObjectDatabase::getStringId(iface->getId()) == master_id)
                {
                    master_interface = iface;
                    break;
                }
            }
        }
    }

    // Real firewalls get checkbox for install
    if (Firewall::isA(fw))
    {
        bool checked = false;
        if (operation==BATCH_INSTALL)
        {
            checked = checkIfNeedToInstall(fw);
            if (cluster)
            {
                // override if checkIfNeedToCompile() is true for the
                // parent cluster.
                if (checkIfNeedToCompile(cluster))
                {
                    checked = true;
                }
            }
            item->setCheckState(INSTALL_CHECKBOX_COLUMN,
                                checked?Qt::Checked:Qt::Unchecked);
        }

        // If this platform requires installation only on
        // the master, disable and uncheck checkbox for the standby.
        if (install_only_on_primary_member && master_interface != NULL)
        {
            QString txt = item->text(0);
            if (master_interface->isChildOf(fw))
            {
                // Master
                item->setText(0, QString("%1 (master)").arg(txt));
            } else
            {
                // Standby
                item->setText(0, QString("%1 (standby)").arg(txt));
                item->setCheckState(INSTALL_CHECKBOX_COLUMN, Qt::Unchecked);
                item->setFlags(0);
            }
        }

        if (cluster==NULL)
        {
            // we are adding firewall that is not cluster member, it
            // needs "compile" checkbox
            checked = checkIfNeedToCompile(fw);
            item->setCheckState(COMPILE_CHECKBOX_COLUMN,
                                checked?Qt::Checked:Qt::Unchecked);
        }
    }

    int num_members = 0;
    // Clusters only get checkbox for compile, and only if they have members.
    if (Cluster::isA(fw))
    {
        list<Firewall*> members;
        Cluster::cast(fw)->getMembersList(members);
        num_members = members.size();
        if (num_members)
        {
            bool checked = checkIfNeedToCompile(fw);
            // if any of the member firewalls are in reqFirewalls, re-check
            // if we need to compile the cluster. checkIfNeedToCompile()
            // uses different condition if reqFirewalls is not empty.
            for (list<Firewall*>::iterator it=members.begin(); it!=members.end(); ++it)
            {
                if (reqFirewalls.find(*it)!=reqFirewalls.end() &&
                    fw->needsCompile() && !fw->getInactive())
                    checked = true;
            }
            item->setCheckState(COMPILE_CHECKBOX_COLUMN,
                                checked?Qt::Checked:Qt::Unchecked);
        }
    }

    dt.setTime_t(lm);
    item->setText(LAST_MODIFIED_COLUMN, (lm)?dt.toString():QString("Never"));

    dt.setTime_t(lc);
    item->setText(LAST_COMPILED_COLUMN, (lc)?dt.toString():QString("Never"));

    dt.setTime_t(li);
    item->setText(LAST_INSTALLED_COLUMN, (li)?dt.toString():QString("Never"));
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

void instDialog::prepareInstConf(Firewall *)
{
    if (fwbdebug) qDebug("instDialog::prepareInstConf");
}

void instDialog::storeInstallerOptions()
{
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
    QStringList str;

    str.append(QObject::tr("Summary:"));
    str.append(QObject::tr("* Running as user : %1").arg(user_name));
    str.append(QObject::tr("* Firewall name : %1")
               .arg(QString::fromUtf8(cnf.fwobj->getName().c_str())));
    str.append(QObject::tr("* Installer uses user name : %1").arg(cnf.user));
    str.append(QObject::tr("* Management address : %1").arg(cnf.maddr));
    str.append(QObject::tr("* Platform : %1")
              .arg(cnf.fwobj->getStr("platform").c_str()));
    str.append(QObject::tr("* Host OS : %1")
              .arg(cnf.fwobj->getStr("host_OS").c_str()));
    str.append(QObject::tr("* Loading configuration from file %1")
              .arg(cnf.fwbfile));

    if (cnf.save_diff)
        str.append(QObject::tr("* Configuration diff will be saved in file %1").
                   arg(cnf.diff_file));

    if (cnf.dry_run)
        str.append(QObject::tr("* Commands will not be executed on the firewall"));

    str.append("");

    QTextCursor cursor = currentLog->textCursor();
    cursor.insertBlock();
    cursor.insertText(str.join("\n"), highlight_format);
}

void instDialog::fillCompileSelectList()
{
    if (fwbdebug) qDebug("instDialog::fillCompileSelectList");

    Firewall *fw;
    Cluster *cl;
    QDateTime dt;

    if (fwbdebug && reqFirewalls.empty())
        qDebug("instDialog::fillCompileSelectList reqFirewalls is empty");

    creatingTable = true;

    list<Firewall*> working_list_of_firewalls = firewalls;

    for (list<Cluster *>::iterator i=clusters.begin(); i!=clusters.end(); ++i)
    {
        cl = *i;

        QTreeWidgetItem* cluster_item = createTreeItem(NULL, cl);
        m_dialog->selectTable->addTopLevelItem(cluster_item);

        list<Firewall*> members;
        cl->getMembersList(members);

        for (list<Firewall*>::iterator member=members.begin();
             member!=members.end(); ++member)
        {
            QTreeWidgetItem *itm = createTreeItem(cluster_item, *member);
            working_list_of_firewalls.remove(*member);
        }
        cluster_item->setExpanded(true);
    }

    for (list<Firewall *>::iterator i=working_list_of_firewalls.begin();
         i!=working_list_of_firewalls.end(); ++i)
    {
        fw = *i;
        QTreeWidgetItem* fw_item = createTreeItem(NULL, fw);
        m_dialog->selectTable->addTopLevelItem(fw_item);
    }

    QTreeWidgetItemIterator it(m_dialog->selectTable);
    while (*it)
    {
        setFlags(*it);
        ++it;
    }

    creatingTable = false;

    for (int i=0; i<m_dialog->selectTable->columnCount(); i++)
        m_dialog->selectTable->resizeColumnToContents(i);

    setNextEnabled(0, tableHasCheckedItems());

    //m_dialog->selectTable->resizeRowsToContents();
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
    finished = true;
    accept();
}

/* user clicked 'Cancel' */
void instDialog::cancelClicked()
{
    if (fwbdebug) qDebug("instDialog::cancelClicked()");
    finished = true;

    if (proc.state() == QProcess::Running)
    {
        rejectDialogFlag = true;
        proc.kill();
    }

    if (installer != NULL)
    {
        if (fwbdebug)
            qDebug() << "instDialog::cancelClicked  killing installer";
        installer->terminate();
        delete installer;
        installer = NULL;
    }

    QDialog::reject();
}


void instDialog::showEvent( QShowEvent *ev)
{
    st->restoreGeometry(this, QRect(200,100,780,500) );
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
    // if (fwbdebug)
    //     qDebug("instDialog::addToLog: '%s'", line.toLatin1().constData());

    if (line.isEmpty()) return;

    if (currentLog)
    {
        QString txt = line.trimmed();

        QTextCharFormat format = normal_format;

        list<QRegExp>::const_iterator it;
        for (it=error_re.begin(); it!=error_re.end(); ++it)
        {
            if ((*it).indexIn(txt) != -1)
            {
                format = error_format;
                break;
            }
        }

        for (it=warning_re.begin(); it!=warning_re.end(); ++it)
        {
            if ((*it).indexIn(txt) != -1)
            {
                format = warning_format;
                break;
            }
        }

        /* See sourceforge bug https://sourceforge.net/tracker/?func=detail&aid=2847263&group_id=5314&atid=1070394
         *
         * QTextEditor::insertHtml() becomes incrementally slow as the
         * amount of text already in the QTextEditor
         * increases. Compiling ~10 firewalls with few dozen rules
         * each slows the output to a crawl on Windows.  Keeping each
         * line in a separate block makes it much faster.
         */

        txt = line;
        while (!txt.isEmpty() && (txt.endsWith("\n") || txt.endsWith("\r")))
            txt.chop(1);

        if (format == error_format || format == warning_format )
            format.setAnchorHref(txt);

        QTextCursor cursor = currentLog->textCursor();
        cursor.insertBlock();
        cursor.insertText(txt, format);

        //m_dialog->procLogDisplayList->addItem(txt);

        currentLog->ensureCursorVisible();

        qApp->processEvents();
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
    if (operation==BATCH_INSTALL)
        setSelectStateAll(INSTALL_CHECKBOX_COLUMN, Qt::Checked);
    setSelectStateAll(COMPILE_CHECKBOX_COLUMN, Qt::Checked);
    tableItemChanged(NULL, 0);
}

void instDialog::deselectAllFirewalls()
{
    if (operation==BATCH_INSTALL)
        setSelectStateAll(INSTALL_CHECKBOX_COLUMN, Qt::Unchecked);
    setSelectStateAll(COMPILE_CHECKBOX_COLUMN, Qt::Unchecked);
    tableItemChanged(NULL, 0);
}

void instDialog::setSelectStateAll(int column, Qt::CheckState select)
{
    QTreeWidgetItemIterator it(m_dialog->selectTable);
    while (*it)
    {
        int obj_id = (*it)->data(0, Qt::UserRole).toInt();
        FWObject *o = project->db()->findInIndex(obj_id);
        bool cluster_member = (*it)->data(1, Qt::UserRole).toBool();
        int num_members = (*it)->data(2, Qt::UserRole).toInt();

        Qt::ItemFlags flags = (*it)->flags();
        if ((flags & Qt::ItemIsUserCheckable) != 0)
        {
            // firewalls only get checkboxes for install,
            if (column == INSTALL_CHECKBOX_COLUMN && Firewall::isA(o))
                (*it)->setCheckState(column, select);

            // Cluster gets checkbox for compile.
            // Cluster should never get a checkbox if it has no members.
            // Firewall that is not a cluster member gets compile checkbox
            if ((column == COMPILE_CHECKBOX_COLUMN && Cluster::isA(o) && num_members) ||
                (Firewall::isA(o) && !cluster_member))
                (*it)->setCheckState(column, select);
        }
        ++it;
    }
}

void instDialog::fillCompileOpList()
{
    compile_fw_list.clear();
    QTreeWidgetItemIterator it(m_dialog->selectTable);
    while (*it)
    {
        if ((*it)->checkState(COMPILE_CHECKBOX_COLUMN))
        {
            int obj_id = (*it)->data(0, Qt::UserRole).toInt();
            FWObject *o = project->db()->findInIndex(obj_id);
            compile_fw_list.push_back(Firewall::cast(o));
        }
        ++it;
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
        f = (*i);
        item = new InstallFirewallViewItem(
            NULL,//m_dialog->fwWorkList,
            QString::fromUtf8(f->getName().c_str()),
            false);

        item->setData(0, Qt::UserRole, QVariant(f->getId()));
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
    QTreeWidgetItemIterator it(m_dialog->selectTable);
    while (*it)
    {
        if ((*it)->checkState(INSTALL_CHECKBOX_COLUMN))
        {
            int obj_id = (*it)->data(0, Qt::UserRole).toInt();
            FWObject *o = project->db()->findInIndex(obj_id);
            install_fw_list.push_back(Firewall::cast(o));
            if (fwbdebug)
                qDebug("fillInstallOpList: Install requested for %s", o->getName().c_str());
        }
        ++it;
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

    m_dialog->detailMCframe->show();
    qApp->processEvents();
    QString fw_name = item->text(0);

    m_dialog->procLogDisplay->moveCursor( QTextCursor::End );
    m_dialog->procLogDisplay->find(currentSearchString +
            fw_name,
            QTextDocument::FindWholeWords |
            QTextDocument::FindCaseSensitively |
            QTextDocument::FindBackward);
}

void instDialog::tableItemChanged(QTreeWidgetItem*, int)
{
    if (!creatingTable)
        setNextEnabled(0, tableHasCheckedItems());
}

bool instDialog::tableHasCheckedItems()
{
    QTreeWidgetItemIterator it(m_dialog->selectTable);
    while (*it)
    {
        if ((*it)->checkState(COMPILE_CHECKBOX_COLUMN) ||
            (*it)->checkState(INSTALL_CHECKBOX_COLUMN))
            return true;
        ++it;
    }
    return false;
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

    if (m_dialog->batchInstall->isChecked() && batch_inst_opt_dlg)
    {
        // in batch install mode we use the same dialog to fill cnf
        // without showing it to the user again
        readInstallerOptionsFromDialog(fw, batch_inst_opt_dlg);
    } else
    {
        // In non-batch mode installer options from the dialog
        // overwrite options set in the fw object itself.
        instOptionsDialog *inst_opt_dlg = new instOptionsDialog(this, &cnf);
        if (inst_opt_dlg->exec()==QDialog::Rejected)
        {
            delete inst_opt_dlg;
            return false;
        }
        readInstallerOptionsFromDialog(fw, inst_opt_dlg);
    }

    return verifyManagementAddress();
}

/*
 * getBatchInstOptions() fills attributes of the cnf object for batch install
 */
bool instDialog::getBatchInstOptions()
{
    cnf.fwobj = NULL;

    readInstallerOptionsFromSettings();

    if (batch_inst_opt_dlg != NULL) delete batch_inst_opt_dlg;

    batch_inst_opt_dlg = new instBatchOptionsDialog(this, &cnf);

    if (batch_inst_opt_dlg->exec()==QDialog::Rejected)
    {
        stopProcessFlag = true;
        this->m_dialog->finishButton->setEnabled(true);
        foreach(Firewall *fw, this->install_fw_list)
            this->opCancelled(fw);
        return false;
    }
    // clear aternative address in the dialog
    batch_inst_opt_dlg->m_dialog->altAddress->setText("");
    readInstallerOptionsFromDialog(NULL, batch_inst_opt_dlg);

    if (fwbdebug)
        qDebug() << "instDialog::getBatchInstOptions():  cnf.user=" << cnf.user;

    return verifyManagementAddress();
}

void instDialog::readInstallerOptionsFromSettings()
{
    if (fwbdebug) qDebug("instDialog::readInstallerOptionsFromSettings");

    fwb_prompt="--**--**--";

    cnf.batchInstall = m_dialog->batchInstall->isChecked();

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

        string platform = cnf.fwobj->getStr("platform");
        string host_OS = cnf.fwobj->getStr("host_OS");

        cnf.user = fwopt->getStr("admUser").c_str();
        QString aaddr = fwopt->getStr("altAddress").c_str();
        if (!aaddr.isEmpty()) cnf.maddr = aaddr;
        else
        {
            // Note that Host::getManagementAddress() scans interfaces and
            // finds one marked as "management" and takes its address.
            // It does not use Management child object.
            const InetAddr *mgmt_addr = cnf.fwobj->getManagementAddress();
            if (mgmt_addr)
                cnf.maddr = mgmt_addr->toString().c_str();
            else
                cnf.maddr = "";
        }
        /*
         * if user requested test run, store firewall script in a temp
         * file.  Always store it in a temp file on linksys
         */
        QString s;

        /* user_can_change_install_dir */
        bool uccid = Resources::getTargetOptionBool(
            host_OS, "user_can_change_install_dir");

        if (uccid) s = fwopt->getStr("firewall_dir").c_str();

        if (s.isEmpty()) s = Resources::getTargetOptionStr(
            host_OS, "activation/fwdir").c_str();

        cnf.fwdir = s;

        cnf.script = FirewallInstaller::getGeneratedFileFullPath(fw);
        cnf.remote_script = ""; // filled in FirewallInstaller::readManifest()
        cnf.fwbfile = project->db()->getFileName().c_str();
        cnf.wdir = getFileDir( project->getRCS()->getFileName() );
        cnf.diff_file = QString(cnf.fwobj->getName().c_str())+".diff";
        cnf.diff_pgm = Resources::platform_res[platform]->
            getResourceStr("/FWBuilderResources/Target/diff").c_str();
        cnf.diff_pgm = getPathToBinary(
            cnf.diff_pgm.toAscii().constData()).c_str();
#ifdef _WIN32
        cnf.diff_pgm = cnf.diff_pgm + ".exe";
#endif
        cnf.sshArgs = fwopt->getStr("sshArgs").c_str();
        cnf.scpArgs = fwopt->getStr("scpArgs").c_str();
        cnf.useSCPForCisco = fwopt->getBool("use_scp");

        cnf.activationCmd = fwopt->getStr("activationCmd").c_str();

        if (fwbdebug)
        {
            qDebug("platform: %s", platform.c_str());
            qDebug("host_OS: %s", host_OS.c_str());
            qDebug("user_can_change_install_dir=%d", uccid);
            qDebug("firewall_dir='%s'", fwopt->getStr("firewall_dir").c_str());
            qDebug("management address: %s", cnf.maddr.toAscii().constData());
            qDebug("cnf.fwdir='%s'", cnf.fwdir.toAscii().constData());
            qDebug("activationCmd='%s'", cnf.activationCmd.toAscii().constData());
        }

    }
}

void instDialog::readInstallerOptionsFromDialog(Firewall *fw,
                                                instOptionsDialog *dlg)
{
    if (fwbdebug) qDebug("instDialog::readInstallerOptionsFromDialog");

    cnf.fwobj = fw;
    QString adm_user;

    FWOptions *fwopt = NULL;
    if (fw)
    {
        fwopt = cnf.fwobj->getOptionsObject();
        adm_user = fwopt->getStr("admUser").c_str();
    }

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

    // user name set in the dialog overrides that set in the fw object
    // But the dialog user name input field can be left blank, in which
    // case we use the one configured in the object
    if (!adm_user.isEmpty()) cnf.user = adm_user;
    if (!dlg->m_dialog->uname->text().isEmpty())
        cnf.user = dlg->m_dialog->uname->text();

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

    dlg->savePassword();
    storeInstallerOptions();
}

bool instDialog::verifyManagementAddress()
{
/* check for a common error when none or multiple interfaces are marked as
 * 'management'
 */
    if (cnf.fwobj)
    {
        int nmi = 0;
        list<FWObject*> ll = cnf.fwobj->getByTypeDeep(Interface::TYPENAME);
        for (FWObject::iterator i=ll.begin(); i!=ll.end(); i++)
        {
            Interface *intf = Interface::cast( *i );
            if (intf->isManagement()) nmi++;
        }
        if (nmi>1)
        {
            QString err = QObject::tr("Only one interface of the firewall '%1' "
                                      "must be marked as management interface.\n")
                .arg(QString::fromUtf8(cnf.fwobj->getName().c_str()));

            QMessageBox::critical(this, "Firewall Builder", err,
                                  tr("&Continue") );
            addToLog(err);
            return false;
        }
        if (nmi==0)
        {
            QString err = QObject::tr("One of the interfaces of the firewall '%1' "
                                      "must be marked as management interface.\n")
                .arg(QString::fromUtf8(cnf.fwobj->getName().c_str()));

            QMessageBox::critical(this, "Firewall Builder", err,
                                  tr("&Continue") );
            addToLog(err);
            return false;
        }
        if (cnf.maddr == "" ||
            cnf.maddr == QString(InetAddr::getAny().toString().c_str()))
        {
            QString err = QObject::tr("Management interface does not have IP address, "
                                      "can not communicate with the firewall.\n");

            QMessageBox::critical(this, "Firewall Builder", err,
                                  tr("&Continue") );
            addToLog(err);
            return false;
        }
    }
    return true;
}

void instDialog::logItemClicked(QUrl data)
{
    QStringList parts = data.toString().split(':');
    if (parts[0] == "Error") return;
    if (parts.size()<3)
    {
        if(fwbdebug)
            cout << "Wrong error message clicked" << endl;
        return;
    }
    emit activateRule(project, parts[0], parts[1], parts[2].toInt());
}

void instDialog::closeEvent(QCloseEvent *event)
{
    if (fwbdebug) qDebug() << "instDialog::closeEvent";
    if (proc.state() == QProcess::Running)
    {
        if (fwbdebug)
            qDebug() << "instDialog::closeEvent  killing process";
        proc.kill();
    }
    if (installer != NULL)
    {
        if (fwbdebug)
            qDebug() << "instDialog::closeEvent  killing installer";
        installer->terminate();
        delete installer;
        installer = NULL;
    }
}
