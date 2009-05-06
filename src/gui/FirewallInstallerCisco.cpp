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

#include "FirewallInstallerCisco.h"
#include "instDialog.h"
#include "SSHPIX.h"
#include "SSHIOS.h"

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

bool FirewallInstallerCisco::packInstallJobsList(Firewall*)
{
    if (fwbdebug)
        qDebug("FirewallInstallerCisco::packInstallJobList  conffile=%s",
               cnf->conffile.toAscii().constData());
    job_list.clear();
    job_list.push_back(instJob(ACTIVATE_POLICY, cnf->conffile));
    return true;
}

void FirewallInstallerCisco::activatePolicy()
{
    // Someone may have external expect script to talk to PIX or a router
    // Let them run it too.
    Management *mgmt = cnf->fwobj->getManagementObject();
    assert(mgmt!=NULL);
    PolicyInstallScript *pis = mgmt->getPolicyInstallScript();
    if (pis->getCommand()!="" )
    {
        executeInstallScript();
        return;
    }

    QStringList args;

    packSSHArgs(args);
    if (cnf->verbose) inst_dlg->displayCommand(args);

    SSHPIX *ssh_object = NULL;
    if (cnf->fwobj->getStr("platform")=="pix" ||
        cnf->fwobj->getStr("platform")=="fwsm")
    {
        ssh_object = new SSHPIX(inst_dlg,
                                cnf->fwobj->getName().c_str(),
                                args,
                                cnf->pwd,
                                cnf->epwd,
                                list<string>());
    } else   // ios
    {
        ssh_object = new SSHIOS(inst_dlg,
                                cnf->fwobj->getName().c_str(),
                                args,
                                cnf->pwd,
                                cnf->epwd,
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

    cmd = cnf->getCmdFromResource("pre_config_commands");
    pre_config_commands =
        inst_dlg->replaceMacrosInCommand(cmd).split("\n", QString::SkipEmptyParts);

    if (cnf->rollback)
    {
        cmd = cnf->getCmdFromResource("schedule_rollback");
        pre_config_commands = pre_config_commands +
            inst_dlg->replaceMacrosInCommand(cmd).split("\n", QString::SkipEmptyParts);
    }

    cmd = cnf->getCmdFromResource("post_config_commands");
    post_config_commands =
        inst_dlg->replaceMacrosInCommand(cmd).split("\n", QString::SkipEmptyParts);

    if (cnf->cancelRollbackIfSuccess)
    {
        cmd = cnf->getCmdFromResource("cancel_rollback");
        post_config_commands = post_config_commands +
            inst_dlg->replaceMacrosInCommand(cmd).split("\n", QString::SkipEmptyParts);
    }

    if (cnf->saveStandby)
    {
        cmd = cnf->getCmdFromResource("save_standby");
        post_config_commands = post_config_commands +
            inst_dlg->replaceMacrosInCommand(cmd).split("\n", QString::SkipEmptyParts);
    }

    ssh_object->loadPreConfigCommands( pre_config_commands );
    ssh_object->loadPostConfigCommands( post_config_commands );

    runSSHSession(ssh_object);

    return;
}


