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

#include "SSHCisco.h"

#include <qobject.h>
#include <qtimer.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <qfileinfo.h>
#include <QtDebug>

#include <iostream>

#include <errno.h>
#ifndef errno
extern int errno;
#endif

using namespace std;

SSHCisco::SSHCisco(QWidget *_par,
               const QString &_h,
               const QStringList &args,
               const QString &_p,
               const QString &_ep,
               const std::list<std::string> &_in) : SSHSession(_par,_h,args,_p,_ep,_in)
{
    normal_prompt="> $";
    fwb_prompt="--**--**--";
    enable_prompt="# $|# *Access Rules Download Complete";
    config_prompt="\\(config(|-.*)\\)#";
    pwd_prompt_1="'s password: $";
    pwd_prompt_2="'s password: $";
    epwd_prompt="Password: ";
    ssh_pwd_prompt="'s password: ";
    ssoft_config_prompt="> ";
    putty_pwd_prompt="Password: ";
    passphrase_prompt="Enter passphrase for key ";

    errorsInit.push_back("Permission denied");
    errorsInit.push_back("Invalid password");
    errorsInit.push_back("Access denied");
    errorsInit.push_back("Unable to authenticate");
    errorsInit.push_back("Too many authentication failures");

    errorsLoggedin.push_back("Invalid password");
    errorsLoggedin.push_back("ERROR: ");
    errorsLoggedin.push_back("Not enough arguments");
    errorsLoggedin.push_back("cannot find");

    errorsEnabledState.push_back("ERROR: ");
    errorsEnabledState.push_back("Type help");
    errorsEnabledState.push_back("Not enough arguments");
    errorsEnabledState.push_back("Invalid");
    errorsEnabledState.push_back("invalid");
    errorsEnabledState.push_back("cannot find");
    errorsEnabledState.push_back(
        "An object-group with the same id but different type");
    errorsEnabledState.push_back("cannot add route entry");

    local_event_loop = new QEventLoop();
    
    comment_symbol = '!';
}

void SSHCisco::loadPreConfigCommands(const QStringList &cl)
{
    pre_config_commands = cl;
}

void SSHCisco::loadPostConfigCommands(const QStringList &cl)
{
    post_config_commands = cl;
}

void SSHCisco::loadActivationCommands(const QStringList &cl)
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

SSHCisco::~SSHCisco()
{
}

QString SSHCisco::cmd(QProcess*, const QString &cmd)
{
    if (fwbdebug) qDebug("Command '%s'", cmd.toLatin1().constData());
    sendCommand(cmd);
    //stdoutBuffer = "";
    //proc->write( (cmd + "\n").toLatin1() );
    state = EXECUTING_COMMAND;
    local_event_loop->exec();
    if (fwbdebug) qDebug("Command '%s' completed", cmd.toLatin1().constData());
    return stdoutBuffer;
}


bool SSHCisco::checkForErrors()
{
    QStringList *errptr;

    switch (state)
    {
    case LOGGEDIN:
    case WAITING_FOR_ENABLE:
        errptr = &errorsLoggedin;
        break;

    case ENABLE:
        errptr = &errorsEnabledState;
        break;

    default:
        errptr = &errorsInit;
        break;
    }

    for (QStringList::const_iterator i=errptr->begin();
         i!=errptr->end(); ++i)
    {
        QString line = *i;
        if ( stdoutBuffer.lastIndexOf(line, -1)!=-1 )
        {
            if (fwbdebug)
                qDebug() << "Got known error message: " << line;

            emit printStdout_sign( tr("\n*** Fatal error :") );
            emit printStdout_sign( line + "\n" );
            stdoutBuffer="";
//            terminate();
            sessionComplete(true);   // finish with error status
            return true;
        }
    }
    return false;
}

void SSHCisco::stateMachine()
{
    if (checkForErrors()) return;

    if (fwbdebug)
        qDebug() << "SSHCisco::stateMachine()  state=" << state
                 << "(ENABLE=" << ENABLE << ")"
                 << "(PUSHING_CONFIG=" << PUSHING_CONFIG << ")"
                 << " stdoutBuffer=" << stdoutBuffer;

    switch (state)
    {
    case NONE:
    {
        if ( cmpPrompt(stdoutBuffer,QRegExp(pwd_prompt_1)) ||
             cmpPrompt(stdoutBuffer,QRegExp(pwd_prompt_2)) )
        {
            stdoutBuffer="";
            proc->write( (pwd + "\n").toLatin1() );
            break;
        }

/* we may get to LOGGEDIN state directly from NONE, for example when
 * password is supplied on command line to plink.exe
 */
        if (cmpPrompt(stdoutBuffer, QRegExp(normal_prompt)) )
        {
            stdoutBuffer="";
            state=LOGGEDIN;
            emit printStdout_sign( "\n");
            emit printStdout_sign( tr("Logged in") + "\n" );
            emit printStdout_sign( tr("Switching to enable mode...") + "\n");
            stdoutBuffer="";
            proc->write( "enable\n" );
        }

/* we may even get straight to the enable prompt, e.g. if
 * user account is configured with "privilege 15"
 */
        if ( cmpPrompt(stdoutBuffer, QRegExp(enable_prompt)) )
        {
            state=WAITING_FOR_ENABLE;
            stateMachine();
            break;
        }

        QString fingerprint;
        //int n1,n2;
        if (stdoutBuffer.indexOf(newKeyOpenSSH)!=-1 ||
            stdoutBuffer.indexOf(newKeyPlink)!=-1   ||
            stdoutBuffer.indexOf(newKeySSHComm)!=-1)
        {
/* new key */
            bool unix_y_n = (stdoutBuffer.indexOf(newKeyOpenSSH)!=-1 ||
                             stdoutBuffer.indexOf(newKeySSHComm)!=-1);

            if (fwbdebug) qDebug("New host key message detected");

            fingerprint = findKeyFingerprint(stdoutBuffer);

            QString msg = newKeyMsg.arg(host).arg(fingerprint).arg(host);

            stopHeartBeat();

            int res = QMessageBox::warning( parent, tr("New RSA key"), msg,
                                            tr("Yes"), tr("No"), 0,
                                            0, -1 );

            if (fwbdebug)
                qDebug("User said: res=%d", res);

            startHeartBeat();

            stdoutBuffer="";
            if (res==0)
            {
                if (unix_y_n) proc->write( "yes\n" );
                else          proc->write( "y\n" );
                break;
            } else
            {
                sessionComplete(true);   // finish with error status
                return;
//                state=EXIT;
//                goto entry;
            }
        }
    }
    break;

    case LOGGEDIN:
        if ( cmpPrompt(stdoutBuffer,QRegExp(epwd_prompt)) )
        {
            stdoutBuffer="";
            if (!epwd.isEmpty()) proc->write( (epwd + "\n").toLatin1() );
            else                 proc->write( "\n" );
            state=WAITING_FOR_ENABLE;
        }
        break;

    case WAITING_FOR_ENABLE:
        if ( cmpPrompt(stdoutBuffer,QRegExp(epwd_prompt)) )
        {
            stdoutBuffer="";
            if (!epwd.isEmpty()) proc->write( (epwd + "\n").toLatin1() );
            else                 proc->write( "\n" );
            state=WAITING_FOR_ENABLE;
            break;
        }
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
            emit printStdout_sign( tr("In enable mode."));
            emit printStdout_sign( "\n");
            state=ENABLE;  // and go to ENABLE target in switch

            /* give classes derived from SSHCisco a chance to do
             * something before we switch to config mode. If <this> is
             * SSHCisco class, the stateMachine method will simply call
             * itself and will fall through to the ENABLE state.
             */
            stateMachine();
            break;
        }
    /* FALLTHRU */
    case ENABLE:
        if ( cmpPrompt(stdoutBuffer, QRegExp(enable_prompt)) )
        {
            if (pre_config_commands.size()>0)
            {
                stdoutBuffer="";
                QString cmd = pre_config_commands.front();
                pre_config_commands.pop_front();
                if (cmd.indexOf("reload in")!=-1) state = SCHEDULE_RELOAD_DIALOG;
                sendCommand(cmd);
                //proc->write( (cmd + "\n").toLatin1() );
                break;
            }

            stdoutBuffer="";

            if (backup)
            {
                /* the problem is that QProcess uses select and thus
                 * is tightly integrated into event loop. QT uses
                 * internal private flag inside QProcess to
                 * specifically prevent recursive calls to
                 * readyReadStdout (look for d->socketReadCalled in
                 * kernel/qprocess_unix.cpp ). So, I _must_ exit this
                 * callback before I can send commands to the process
                 * and collect the output.
                 */
                QTimer::singleShot( 0, this, SLOT(PIXbackup()) );
                break;
            }

            state = WAITING_FOR_CONFIG_PROMPT;
            // kick it so we get some output from the router and
            // continue the state machine
            proc->write("\n"); 
        }
        break;

    case SCHEDULE_RELOAD_DIALOG:
        if ( cmpPrompt(stdoutBuffer,
                       QRegExp("System config.* modified\\. Save?")) )
        {
            stdoutBuffer="";
            proc->write( "n" );  // no \n needed
            break;
        }
        if ( cmpPrompt(stdoutBuffer, QRegExp("Proceed with reload?")) )
        {
            stdoutBuffer="";
            proc->write( "y" );  // no \n needed
            break;
        }
        if ( cmpPrompt(stdoutBuffer, QRegExp("SHUTDOWN")) )
        {
            stdoutBuffer="";
            proc->write( "\n" );
            state = ENABLE;
            break;
        }
        if ( cmpPrompt(stdoutBuffer, QRegExp(enable_prompt)) )
        {
            // reload did not ask for confirmation
            stdoutBuffer="";
            proc->write( "\n" );
            state = ENABLE;
            break;
        }
        break;

    case EXECUTING_COMMAND:
        if ( cmpPrompt(stdoutBuffer, QRegExp(enable_prompt)) )
        {
            //QCoreApplication::exit();
            state = COMMAND_DONE;
            if (fwbdebug) qDebug("Switching to COMMAND_DONE state; state=%d",
                                 state);
            if (local_event_loop->isRunning()) local_event_loop->exit();
        }
        break;

    case WAITING_FOR_CONFIG_PROMPT:
        if ( cmpPrompt(stdoutBuffer, QRegExp(enable_prompt)) )
        {
            /* install full policy */
            state = PUSHING_CONFIG; // and drop to PUSHING_CONFIG case
            if (!dry_run)
                emit printStdout_sign(tr("Pushing firewall configuration"));
            emit printStdout_sign( "\n");
            stdoutBuffer = "";
            proc->write("\n");
            ncmd=0;
        }
        break;

    case PUSHING_CONFIG:
        if ( cmpPrompt(stdoutBuffer, QRegExp(enable_prompt)))  // config_prompt)) )
        {
            // see SF bug 2973136 , fwbuilder bug #1347
            // looks like if user hits Cancel to cancel install at just right
            // moment, the process can get killed when control is already
            // inside this block. Adding test for proc != nullptr to be sure.
            if ( activation_commands.size() != 0 && proc != nullptr)
            {
                QString s;

                do {
                    s = activation_commands.front();
                    activation_commands.pop_front();

                    emit updateProgressBar_sign(activation_commands.size(),false);

                    s.replace('\"','\'');

                    if (!quiet)
                    {
                        QString rl="";
                        if (s.indexOf(QString("%1 Rule ").arg(comment_symbol)) != -1)
                            rl = s.mid(7);

                        if ( !rl.isEmpty())
                        {
                            emit printStdout_sign( tr("Rule %1").arg(rl) + "\n" );
                        }
                    }

                } while (stripComments && s[0] == comment_symbol);

                sendCommand(s);

            } else
            {
                /* activation_commands.size()==0 */
                state = EXIT_FROM_CONFIG;
                emit printStdout_sign( tr("End") + "\n" );
                // kick it so we get some output from the router and
                // continue the state machine
                if (proc) proc->write("\n"); 
            }
        }
        break;

    case EXIT_FROM_CONFIG:
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
            /*
             * Execute post_config_commands
             */
            if (post_config_commands.size()>0)
            {
                stdoutBuffer = "";
                QString cmd = post_config_commands.front();
                post_config_commands.pop_front();
                sendCommand(cmd);
                break;
            }

            stdoutBuffer="";
            state = EXIT;
            proc->write( "exit\n");
        }
        break;

    case EXIT:
//        emit printStdout_sign( tr("Terminating session\n") );
//        terminate();
//        state=FINISH;
        break;

    case FINISH:
        break;

    default:    break;
    }
}

