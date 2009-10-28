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
#include "FirewallInstallerCisco.h"
#include "FirewallInstallerUnx.h"
#include "FWBSettings.h"
#include "FWWindow.h"
#include "instOptionsDialog.h"
#include "instBatchOptionsDialog.h"
#include "events.h"

#include "fwbuilder/Resources.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/XMLTools.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Management.h"

#include <errno.h>
#include <iostream>

#include <QTextCodec>
#include <QTimer>
#include <QMessageBox>


using namespace std;
using namespace libfwbuilder;

bool instDialog::runInstaller(Firewall *fw)
{
    if (fwbdebug) qDebug("instDialog::runInstaller");

    cnf.fwobj = fw;
    cnf.maddr = "";

    if (!getInstOptions(fw)) 
    {
        QTimer::singleShot( 0, this, SLOT(mainLoopInstall()));
        return false;
    }

//    args.clear();

/* need to save settings so that if the user just changed ssh/scp, the
 * wrapper will pick changes up
 */
    st->save();

    summary();

    if (checkSSHPathConfiguration(fw))
    {
        setTitle(
            1,
            QObject::tr("Installing policy rules on firewall '%1'").
            arg(QString::fromUtf8(fw->getName().c_str() )));

        currentSearchString = tr("Install firewall: ");
        currentFirewallsBar->setValue(install_list_initial_size - 
                                      install_fw_list.size());
        currentProgressBar->reset();
        currentProgressBar->setFormat("%v/%m");

        QTreeWidgetItem* item = opListMapping[fw->getId()];
        assert(item!=NULL);

        currentFWLabel->setText(QString::fromUtf8(fw->getName().c_str()));
        m_dialog->fwWorkList->scrollToItem(item);
        setInProcessState(item);
        item->setText(1, tr("Installing ..."));
        currentLabel->setText(tr("Installing ..."));
        
        qApp->processEvents();

        addToLog("\n");

        if (fwbdebug) qDebug("built-in installer firewall %s",
                             fw->getName().c_str());

        if (installer!=NULL)
            delete installer;

        if (isCiscoFamily())
            installer = new FirewallInstallerCisco(this, &cnf, fwb_prompt);
        else
            installer = new FirewallInstallerUnx(this, &cnf, fwb_prompt);

        if (!installer->packInstallJobsList(fw))
        {
            QTimer::singleShot( 0, this, SLOT(mainLoopInstall()));
            return false;
        }

        currentProgressBar->setValue(0);

        disconnect(currentStopButton, SIGNAL(clicked()));
        connect(currentStopButton, SIGNAL(clicked()),
                this, SLOT(stopInstall()));

        installer->runJobs();
    }
    else
    {
        opListMapping[fw->getId()]->setText(1, tr("Failure"));
        setFailureState(opListMapping[fw->getId()]);

        QTimer::singleShot( 0, this, SLOT(mainLoopInstall()));
    }
    return true;
}

void instDialog::stopInstall()
{
    currentStopButton->setEnabled(false);
    stopProcessFlag=true;

    disconnect(currentStopButton, SIGNAL(clicked()) );
    currentStopButton->setEnabled(false);

    proc.terminate();                                  //try to close proc.
    QTimer::singleShot( 1000, &proc, SLOT( kill() ) ); //if it doesn't respond, kill it

    // to terminate whole install sequence rather than just current
    // process, clear the list.
    for (list<Firewall*>::iterator i=install_fw_list.begin();
         i!=install_fw_list.end(); ++i)
        opCancelled(*i);

    install_fw_list.clear();
    setFinishEnabled(currentPage(), true);
}

void instDialog::installerFinished(int ret_code, QProcess::ExitStatus status)
{
    if( fwbdebug) qDebug("instDialog::installerFinished "
                         "exit code = %d exit_status=%d",
                         ret_code, status);
    // run readFromStdout() and processEvents() to make sure all
    // events that pass output from the external installer script have
    // been processed. Otherwise the output from the next installer
    // pass in batch install mixes with the tail of the output from
    // the previous one.
    readFromStdout();
    qApp->processEvents();
    if (ret_code==0 && status==QProcess::NormalExit)
        installerSuccess();
    else
        installerError();
}

void instDialog::installerSuccess()
{
    opSuccess(cnf.fwobj);
//    project->updateLastInstalledTimestamp(cnf.fwobj);
    QCoreApplication::postEvent(
        mw, new updateLastInstalledTimestampEvent(project->db()->getFileName().c_str(),
                                                  cnf.fwobj->getId()));

    currentProgressBar->setValue(currentProgressBar->maximum());
    QTimer::singleShot( 1000, this, SLOT(mainLoopInstall()));
}

void instDialog::installerError()
{
    opError(cnf.fwobj);
    currentProgressBar->setValue(currentProgressBar->maximum());
    QTimer::singleShot( 0, this, SLOT(mainLoopInstall()));
    opListMapping[cnf.fwobj->getId()]->setText(1, tr("Failure"));
}

