/*

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

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



#include "global.h"
#include "utils.h"

#include "SSHJUNOS.h"

#include <QObject>
#include <QTimer>
#include <QRegExp>
#include <QMessageBox>
#include <QApplication>
#include <QEventLoop>
#include <QFileInfo>
#include <QtDebug>

#include <iostream>

#include <errno.h>
#ifndef errno
extern int errno;
#endif

using namespace std;

SSHJunos::SSHJunos(QWidget *_par,
               const QString &_h,
               const QStringList &args,
               const QString &_p,
               const QString &_ep,
               const std::list<std::string> &_in) :
    SSHSession(_par,_h,args,_p,_ep,_in)
{
    normal_prompt="% $"; // shell
    fwb_prompt="--**--**--";
    enable_prompt="> $"; // operational prompt
    config_prompt="# $"; // configuration prompt
    pwd_prompt_1="'s password: $";
/*
 * TODO
 * Do not change pwd_prompt_2 without extensive testing
 * This must be tested on actual hardware/os
 * Will prevent login on wrong regex, with silent error - timeout
 */
    pwd_prompt_2="Password:\\w?";
    epwd_prompt="Password: ";
    ssh_pwd_prompt="'s password: ";
    ssoft_config_prompt="> ";
    putty_pwd_prompt="Password: ";
    passphrase_prompt="Enter passphrase for key ";

    errorsInit.clear();
    errorsInit.push_back("Permission denied");
    errorsInit.push_back("Invalid password");
    errorsInit.push_back("Access denied");
    errorsInit.push_back("Unable to authenticate");
    errorsInit.push_back("Too many authentication failures");

    errorsLoggedin.clear();
    errorsLoggedin.push_back("Invalid password");
    errorsLoggedin.push_back("ERROR: ");
    errorsLoggedin.push_back("Not enough arguments");
    errorsLoggedin.push_back("cannot find");

    errorsEnabledState.clear();
    errorsEnabledState.push_back("ERROR: ");
    errorsEnabledState.push_back("Type help");
    errorsEnabledState.push_back("Not enough arguments");
    errorsEnabledState.push_back("invalid input detected");
    errorsEnabledState.push_back("Invalid");
    errorsEnabledState.push_back("cannot find");
    errorsEnabledState.push_back("error: configuration database modified");
    errorsEnabledState.push_back("error: configuration database locked by:");

    errorsConfigState.clear();
    errorsConfigState.push_back("error: configuration check-out failed");
    errorsConfigState.push_back("syntax error");

}

SSHJunos::~SSHJunos()
{
}

void SSHJunos::loadPreConfigCommands(const QStringList &cl)
{
    pre_config_commands = cl;
}

void SSHJunos::loadPostConfigCommands(const QStringList &cl)
{
    post_config_commands = cl;
}

void SSHJunos::loadActivationCommands(const QStringList &cl)
{
    activation_commands = cl;
    foreach(QString line, activation_commands)
    {
        /*
         * store names of access-lists and object-groups
         * actually used in the config
         */
        if (line.indexOf("access-list ")==0)
            newAcls.push_back(line.section(' ',1,1));
        if (line.indexOf("object-group ")==0)
            newObjectGroups.push_back(line.section(' ',1,1));
    }
    emit updateProgressBar_sign(activation_commands.size(), true);
}

bool SSHJunos::checkForErrors()
{
    QStringList *errptr;

    switch(state)
    {
    case LOGGEDIN:
        errptr = &errorsLoggedin;
        break;

    case ENABLE:
    case WAITING_FOR_CONFIG_PROMPT:
        errptr = &errorsEnabledState;
        break;

    case CONFIG:
    case PUSHING_CONFIG:
        errptr = &errorsConfigState;
        break;

    default:
        errptr = &errorsInit;
        break;
    }

    for (QStringList::const_iterator i=errptr->begin(); i!=errptr->end(); ++i)
    {
        QString line = *i;
        if (stdoutBuffer.lastIndexOf(line, -1) != -1)
        {
            error = true;

            if (fwbdebug)
                qDebug() << "Got known error message: " << line;

            emit printStdout_sign(tr("\n*** Fatal error :"));
            emit printStdout_sign(line + "\n");
            stdoutBuffer="";

            if (state == CONFIG || state == PUSHING_CONFIG) {
                emit printStdout_sign(tr("\n*** Doing a rollback"));
                sendCommand("rollback 0");
            }

            if (state == WAITING_FOR_CONFIG_PROMPT) {
                state = EXIT_FROM_CONFIG;
                return true;
            }

            sessionComplete(true);  // finish with error status
            terminate();
            return true;
        }
    }
    return false;
}

void SSHJunos::stateMachine()
{
    if (checkForErrors()) return;

    if (fwbdebug)
        qDebug() << "SSHJunos::stateMachine() state=" << state
                 << "(ENABLE=" << ENABLE << ")"
                 << "(CONFIG=" << CONFIG << ")"
                 << "(PUSHING_CONFIG=" << PUSHING_CONFIG << ")"
                 << " stdoutBuffer=" << stdoutBuffer;

    switch (state)
    {
    case NONE:
    {
        if ( cmpPrompt(stdoutBuffer, QRegExp(pwd_prompt_1)) ||
             cmpPrompt(stdoutBuffer, QRegExp(pwd_prompt_2)) )
        {
            stdoutBuffer="";
            proc->write( (pwd + "\n").toLatin1() );
            break;
        }

/* we may get to LOGGEDIN state directly from NONE, for example when
 * password is supplied on command line to plink.exe.
 * This only happens with the root user
 */
        if (cmpPrompt(stdoutBuffer, QRegExp(normal_prompt)))
        {
            stdoutBuffer="";
            state=LOGGEDIN;
            emit printStdout_sign("\n");
            emit printStdout_sign( tr("Logged in") + "\n");
            emit printStdout_sign( tr("Switching to operational promt...") + "\n");
            stdoutBuffer="";
            proc->write("cli\n");
        }

/* we get straight to operational prompt as a normal user
 */
        if (cmpPrompt(stdoutBuffer, QRegExp(enable_prompt)))
        {
            state = WAITING_FOR_ENABLE;
            stateMachine();
            break;
        }

        QString fingerprint;

        if (stdoutBuffer.indexOf(newKeyOpenSSH) != -1 ||
            stdoutBuffer.indexOf(newKeyPlink) != -1   ||
            stdoutBuffer.indexOf(newKeySSHComm) != -1)
        {
            /* new key */
            bool unix_y_n = (stdoutBuffer.indexOf(newKeyOpenSSH) != -1 ||
                             stdoutBuffer.indexOf(newKeySSHComm) != -1);

            if (fwbdebug) qDebug("New host key message detected");

            fingerprint = findKeyFingerprint(stdoutBuffer);

            QString msg = newKeyMsg.arg(host).arg(fingerprint).arg(host);

            stopHeartBeat();

            int res = QMessageBox::warning(parent, tr("New RSA key"), msg,
                                           tr("Yes"), tr("No"), 0,
                                           0, -1);

            if (fwbdebug)
                qDebug("User said: red=%d", res);

            startHeartBeat();

            stdoutBuffer="";
            if (res==0)
            {
                if (unix_y_n) proc->write("yes\n");
                else          proc->write("y\n");
                break;
            } else
            {
                sessionComplete(true);  // finish with error status
                return;
            }
        }

        if (stdoutBuffer.indexOf("Terminal type?") != -1)
        {
            stdoutBuffer="";
            proc->write("ansi\n");
            return;
        }

    }
    break;

    case LOGGEDIN:
        if (cmpPrompt(stdoutBuffer, QRegExp(normal_prompt)))
        {
            stdoutBuffer="";
            proc->write("cli\n");
            state=WAITING_FOR_ENABLE;
            stdoutBuffer="";
            break;
        }
    /* FALLTHRU */
    case WAITING_FOR_ENABLE:
        if (cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)))
        {
            emit printStdout_sign( tr("In operational prompt."));
            emit printStdout_sign("\n");
            state=ENABLE;
            stateMachine();
            break;
        }
    /* FALLTHRU */
    case ENABLE:
        if (cmpPrompt(stdoutBuffer, QRegExp(enable_prompt)))
        {
            if (pre_config_commands.size() > 0)
            {
                stdoutBuffer="";
                QString cmd = pre_config_commands.front();
                pre_config_commands.pop_front();
                sendCommand(cmd);
                break;
            }

            stdoutBuffer="";
            state = WAITING_FOR_CONFIG_PROMPT;
            // Trying to get exclusive configuration prompt
            proc->write("configure exclusive\n");

            // kick it so we get some output from the router and
            // continue the state machine
            proc->write("\n");
        }
        break;

    case WAITING_FOR_CONFIG_PROMPT:
        if (cmpPrompt(stdoutBuffer, QRegExp(config_prompt)))
        {
            /* install full policy */
            state = PUSHING_CONFIG;
            if (!dry_run)
                emit printStdout_sign(tr("Pushing firewall configuration"));
            emit printStdout_sign("\n");
            stdoutBuffer="";
            proc->write("\n");
            ncmd=0;
        }
        break;

    case PUSHING_CONFIG:
        if (cmpPrompt(stdoutBuffer, QRegExp(config_prompt)))
        {
            // see SF bug 2973136 , fwbuilder bug #1347
            // looks like if user hits Cancel to cancel install at just right
            // moment, the process can get killed when control is already
            // inside this block. Adding test for proc != nullptr to be sure.
            if (activation_commands.size() != 0 && proc != nullptr)
            {
                QString s;

                do {
                    s = activation_commands.front();
                    activation_commands.pop_front();

                    emit updateProgressBar_sign(activation_commands.size(), false);

                    s.replace('\"', '\'');

                    if (!quiet)
                    {
                        QString rl="";
                        if (s.indexOf(QString("%1 Rule ").arg(comment_symbol)) != -1)
                            rl = s.mid(7);

                        if ( !rl.isEmpty())
                        {
                            emit printStdout_sign( tr("Rule %1").arg(rl) + "\n");
                        }
                    }
                } while (stripComments && s[0] == comment_symbol);
                sendCommand(s);
            } else {
                /* activation_commands.size() == 0 */
                proc->write("exit\n");
                state = EXIT_FROM_CONFIG;
                emit printStdout_sign( tr("End") + "\n");
                // kick it so we get some output from the router and
                // continue the state machine
                if (proc) proc->write("\n");
            }
        }
        break;

    case EXIT_FROM_CONFIG:
        if (cmpPrompt(stdoutBuffer, QRegExp(enable_prompt)))
        {
            /*
             * Execute post_config_commands
             */
            if (post_config_commands.size() > 0)
            {
                stdoutBuffer="";
                QString cmd = post_config_commands.front();
                post_config_commands.pop_front();
                sendCommand(cmd);
                break;
            }

            stdoutBuffer="";
            state = EXIT;
            proc->write("\n");
        }
        break;

    case EXIT:
        if (cmpPrompt(stdoutBuffer, QRegExp(enable_prompt)) ||
                cmpPrompt(stdoutBuffer, QRegExp(normal_prompt)) )
        {
            stdoutBuffer="";
            proc->write("exit\n");

            if (error) {
                sessionComplete(true);  // finish with error status
                terminate();
            }
        }
        break;

    case FINISH:
        break;

    default:
        break;
    }
}

