/*
 * transferDialog.cpp - config transfer dialog implementation
 *
 * Copyright (c) 2008 secunet Security Networks AG
 * Copyright (c) 2008 Adrian-Ken Rueegsegger <rueegsegger@swiss-it.ch>
 * Copyright (c) 2008 Reto Buerki <buerki@swiss-it.ch>
 *
 * This work is dual-licensed under:
 *
 * o The terms of the GNU General Public License as published by the Free
 *   Software Foundation, either version 2 of the License, or (at your option)
 *   any later version.
 *
 * o The terms of NetCitadel End User License Agreement
 */

#include <stdlib.h>
#include <sys/types.h>

#include "transferDialog.h"
#include "utils.h"
#include "utils_no_qt.h"
#include "global.h"

#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWException.h"
#include "fwbuilder/Resources.h"
#include "fwbuilder/Cluster.h"

#include "fwtransfer/TransferDevice.h"

#include "FWWindow.h"
#include "FWBSettings.h"

#include <QTextStream>
#include <qmessagebox.h>
#include <qfiledialog.h>

#define SEARCHSTRING "Transfer firewall "

using namespace std;
using namespace fwtransfer;
using namespace libfwbuilder;

transferDialog::transferDialog(QWidget *parent, set<libfwbuilder::Firewall*> fws)
    : QDialog(parent), transferDevices(NULL)
{
    // setup ui
    m_dialog = new Ui::transferDialog_q;
    m_dialog->setupUi(this);

    // init external process handling
    connect(&proc, SIGNAL(readyReadStandardOutput()),
            this, SLOT(readFromStdout()));
    connect(&proc, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(processExited(int)));
    proc.setProcessChannelMode(QProcess::MergedChannels);

    // disable transfer button until a volume is selected
    m_dialog->transferButton->setEnabled(false);

    // handle cluster selections
    foreach(Firewall* fw, fws)
    {
        if (Cluster::isA(fw))
        {
            list<Firewall*> members;
            Cluster::cast(fw)->getMembersList(members);
            firewalls.insert(firewalls.begin(), members.begin(), members.end());

        } else
        {
            firewalls.push_back(fw);
        }
    }

    if (firewalls.empty())
    {
        QMessageBox::critical(this, "Firewall Config Transfer",
                              tr("No firewalls selected for transfer"),
                              tr("&Continue"), QString::null, QString::null,
                              0, 1);
        return;
    }
    firewalls.sort(FWObjectNameCmpPredicate());

    // init volume list
    transferDevices = new TransferDeviceList;
    try
    {
        transferDevices->init();
    }
    catch (FWException &ex)
    {
        QMessageBox::critical(this, "Firewall Config Transfer",
                              tr("Transfer error: %1").arg(ex.toString().c_str()),
                              tr("&Continue"), QString::null, QString::null,
                              0, 1);
        return;
    }
    // display available volumes (if any)
    updateVolumeView();

    // connect transferDevices signals to our slots
    connect(transferDevices, SIGNAL(devicesChanged()),
            this, SLOT(updateDeviceList()));

    // display selected firewalls
    displayFirewalls();
}

transferDialog::~transferDialog()
{
    if (transferDevices != NULL)
    {
        delete transferDevices;
        transferDevices = NULL;
    }
    delete m_dialog;
}

void transferDialog::displayFirewalls()
{
    m_dialog->fwWorkList->clear();

    QTreeWidgetItem *titem;
    list<Firewall*>::const_iterator i;
    for (i = firewalls.begin(); i != firewalls.end(); ++i)
    {
        titem = new QTreeWidgetItem;
        titem->setText(0, (*i)->getName().c_str());

        // initial state is 'Waiting'
        titem->setText(1, tr("Waiting"));
        m_dialog->fwWorkList->insertTopLevelItem(0, titem);

        // store mapping
        opListMapping[(*i)] = titem;
    }
    m_dialog->fwWorkList->resizeColumnToContents(0);
    m_dialog->fwWorkList->sortByColumn(0, Qt::AscendingOrder);
}

void transferDialog::updateVolumeView()
{
    // set row count to fit transferDevices size
    m_dialog->transferTable->setRowCount(transferDevices->size());

    // display appropriate label, if no volumes -> return
    if (transferDevices->empty())
    {
        m_dialog->volumeLabel->setText("<b>No volumes found! Please connect"
                                       " any usbdisk to update the volume list.</b>");
        m_dialog->transferTable->setEnabled(false);
        m_dialog->transferButton->setEnabled(false);
        return;
    }
    else
    {
        m_dialog->volumeLabel->setText("Select usbstick volume to start "
                                       "firewall config transfer:");
        m_dialog->transferTable->setEnabled(true);
    }

    // fill in available sticks, disable auto-sort first
    m_dialog->transferTable->setSortingEnabled(false);
    QTableWidgetItem *citem;
    int row = 0;
    TransferDeviceList::const_iterator it;
    for (it = transferDevices->begin(); it != transferDevices->end(); it++)
    {
        // name
        citem = new QTableWidgetItem;
        citem->setText((*it).getDeviceName());
        m_dialog->transferTable->setItem(row, 0, citem);

        // size (in MB)
        citem = new QTableWidgetItem;
        citem->setText((*it).getVolumeSizeStr());
        m_dialog->transferTable->setItem(row, 1, citem);

        // mounted?
        QString mounted = (*it).isMounted() ? "Yes" : "No";
        citem = new QTableWidgetItem;
        citem->setText(mounted);
        m_dialog->transferTable->setItem(row, 2, citem);

        // mountpoint, if any
        citem = new QTableWidgetItem;
        citem->setText((*it).getMountpoint());
        m_dialog->transferTable->setItem(row, 3, citem);

        // filesystem
        citem = new QTableWidgetItem;
        citem->setText((*it).getVolumeFS());
        m_dialog->transferTable->setItem(row, 4, citem);
        row++;
    }
    m_dialog->transferTable->setSortingEnabled(true);
    m_dialog->transferTable->sortByColumn(0, Qt::AscendingOrder);
    m_dialog->transferTable->resizeColumnToContents(0);
}

bool transferDialog::runTransfer(Firewall *fw, const QString &volume)
{
    // process events to update display first
    // important when transfering multiple firewalls
    qApp->processEvents();

    // prepare args for transfer agent call
    QStringList args;
    if (!prepareArgs(args, fw, volume))
    {
        return false;
    }

    log(QObject::tr("Executing:"));
    log(args.join(" "));

    // execute binary
    QString path = args.at(0);
    args.pop_front();
    proc.start(path, args);

    if (!proc.waitForStarted())
    {
        log(tr("Error: Failed to start program"));
        setTreeStatus(fw, "Failure");
        return false;
    }
    args.push_front(path);

    if (!proc.waitForFinished())
    {
        log(tr("Error: Waiting for program termination"));
        setTreeStatus(fw, "Failure");
        return false;
    }

    if (proc.exitCode())
    {
        log(tr("Error: Program returned failure status"));
        setTreeStatus(fw, "Failure");
        return false;
    }

    setTreeStatus(fw, "Success");

    return true;
}

bool transferDialog::prepareArgs(QStringList &args, libfwbuilder::Firewall *fw,
                               const QString &volume)
{

    FWOptions *fwopt = fw->getOptionsObject();

    // try to find suitable config transfer agent
    string agent = fwopt->getStr("agent");
    if (agent.empty())
    {
        agent = Resources::os_res[fw->getStr("host_OS")]->getTransferAgent();
    }

    if (agent.empty())
    {
        // no transfer agent for this host_OS found
        log("Cannot find suitable transfer agent");
        setTreeStatus(fw, "Failure");
        return false;
    }
    log(QObject::tr("Using transfer agent '%1'").arg(agent.c_str()));

    QString wdir = getFileDir(mw->getRCS()->getFileName());

    // fill argument list
    args.push_back(agent.c_str());
    QString qs = fwopt->getStr("cmdline").c_str();
    args += qs.split(" ", QString::SkipEmptyParts);
    args.push_back("-f");
    args.push_back(fw->getRoot()->getFileName().c_str());
    if (!wdir.isEmpty())
    {
        args.push_back("-d");
        args.push_back(wdir);
    }
    args.push_back("-v");
    args.push_back(volume);

    // append fw object name to tarball when writing multiple configs
    // to a volume
    if (firewalls.size() > 1)
    {
        log(QObject::tr("Appending fw object name to tarball"));
        args.push_back("-n");
    }

    // append template directory content to transfer tarball
    if (fwopt->getBool("secuwall_add_files"))
    {
        QString tmpldir = fwopt->getStr("secuwall_add_files_dir").c_str();
        args.push_back("-a");
        args.push_back(tmpldir);
    }

    args.push_back(QString::fromUtf8(fw->getName().c_str()));

    return true;
}

void transferDialog::log(const QString message)
{
    if (message.isEmpty())
    {
        return;
    }

    QString txt = message;
    if (!txt.endsWith("\n"))
    {
        txt += "<br>";
    }
    else
    {
        txt.replace('\n', "<br>");
    }
    m_dialog->procLogDisplay->insertHtml(txt);
    m_dialog->procLogDisplay->ensureCursorVisible();
}

void transferDialog::setTreeStatus(Firewall *fw, const QString &status)
{
    QTreeWidgetItem* item = opListMapping[fw];
    if (item == NULL)
    {
        return;
    }

    item->setText(1, status);
}

QString transferDialog::getVolumeName() const
{
    QList<QTableWidgetItem *> itemlist;
    itemlist = m_dialog->transferTable->selectedItems();

    return itemlist[0]->text();
}

void transferDialog::accept()
{
    int current = m_dialog->transferTable->currentRow();
    if (current == -1)
    {
        QMessageBox::information(this, "Transfer",
                                 "Please select volume or press 'Cancel' button.");
        return;
    }
    else
    {
        // start the transfer

        // move cursor to the end of log widget
        m_dialog->procLogDisplay->moveCursor(QTextCursor::End);

        QString volume = getVolumeName();
        log("<b>Using volume : <i>" + volume + "</i></b></br>");

        Firewall *fw;
        list<Firewall*>::const_iterator i;
        for (i = firewalls.begin(); i != firewalls.end(); ++i)
        {
            fw = *i;
            QString fwname = fw->getName().c_str();

            log("<br><b>" + QString(SEARCHSTRING) + fwname + ":</b>");
            setTreeStatus(fw, "Transfering ...");
            if (!runTransfer(fw, volume))
            {
                log(QObject::tr("Could not transfer %1 config to '%2'").
                    arg(fwname).arg(volume));
            }
        }

        // enable save log button after first run
        if (!m_dialog->saveLogButton->isEnabled())
        {
            m_dialog->saveLogButton->setEnabled(true);
        }
    }
}

void transferDialog::saveLog()
{
    QString dir;
    dir = st->getWDir();

    if (dir.isEmpty())
    {
        dir = st->getOpenFileDir();
    }
    if (dir.isEmpty())
    {
        dir = "~";
    }

    QString logText;
    logText = m_dialog->procLogDisplay->toPlainText();

    QString s = QFileDialog::getSaveFileName(this, "Choose a file",
                                             dir, "Text file (*.txt)");

    if (fwbdebug)
    {
        qDebug("Saving log to file %s", s.toAscii().constData());
    }

    if (!s.isEmpty())
    {
        if (!s.endsWith(".txt"))
        {
            s += ".txt";
        }

        QFile f(s);
        if (f.open(QIODevice::WriteOnly))
        {
            QTextStream str(&f);
            str << logText;
            f.close();
        }
    }
}

void transferDialog::selected()
{
    m_dialog->transferButton->setEnabled(true);
}

void transferDialog::readFromStdout()
{
    QString buf = proc.readAllStandardOutput();

    if (fwbdebug)
    {
        qDebug("transferDialog::readFromStdout: %s",
               buf.toAscii().constData());
    }

    log(buf);
}

void transferDialog::processExited(int res)
{
    if (fwbdebug)
    {
        qDebug("transferDialog::processExited, exit code = %d", res);
    }

    readFromStdout();
}

void transferDialog::findFirewallInLog(QTreeWidgetItem* item)
{
    if (fwbdebug)
    {
        qDebug("transferDialog::findFirewallInLog");
    }

    qApp->processEvents();
    m_dialog->procLogDisplay->moveCursor(QTextCursor::End);
    m_dialog->procLogDisplay->find(QString(SEARCHSTRING) + item->text(0) + ":",
                                   QTextDocument::FindWholeWords |
                                   QTextDocument::FindCaseSensitively |
                                   QTextDocument::FindBackward);
}

void transferDialog::updateDeviceList()
{
    updateVolumeView();
}

