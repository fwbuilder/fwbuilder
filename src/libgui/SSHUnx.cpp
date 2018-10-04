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

#include "SSHUnx.h"

#include <qobject.h>
#include <qtimer.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <QtDebug>

#include <iostream>

using namespace std;

SSHUnx::SSHUnx(QWidget *_par,
               const QString &_h,
               const QStringList &args,
               const QString &_p,
               const QString &_ep,
               const list<string> &_in) : SSHSession(_par,_h,args,_p,_ep,_in)
{
    normal_prompt = "> ";
    enable_prompt = "# ";
    pwd_prompt_1 = "'s password: ";
    pwd_prompt_2 = "'s password: ";
    thinkfinger_pwd_prompt = "Password or swipe finger:";
    epwd_prompt = "Password: ";
    ssh_pwd_prompt = "'s password: ";
    ssoft_config_prompt = "> ";
    sudo_pwd_prompt_1 = "Password:";
    sudo_pwd_prompt_2 = "[sudo] password for ";
    putty_pwd_prompt = "Password: ";
    passphrase_prompt = "Enter passphrase for key ";

    errorsInit << "Permission denied";
    errorsInit << "Invalid password";
    errorsInit << "Unable to authenticate";
    errorsInit << "Sorry, try again";
    errorsInit << "Too many authentication failures";

    errorsLoggedin << "No such file or directory";
    errorsLoggedin << "Cannot allocate memory";

    shell_errors << "Syntax error:";
    shell_errors << "No command .* found";
    shell_errors << "Command not found";
    shell_errors << "[fF]ile .* does not exist";

    // some ifconfig errors
    shell_errors << "[iI]nterface .* does not exist";
    shell_errors << "ifconfig: .*: Device busy";
    shell_errors << "permission denied";
    shell_errors << "interface name too long";
    shell_errors << "cloning name too long";
    shell_errors << "error in parsing address";
    shell_errors << "can't set";
    shell_errors << ".* malformed";
    shell_errors << ".*(?<!PEM_read_PrivateKey) failed";
    shell_errors << ".* not allowed for the AF";
    shell_errors << "internal error";
    shell_errors << "unable to allocate .*";
    shell_errors << "unable to get .*";
    shell_errors << "unknown .* protocol";
    shell_errors << "[iI]nvalid .* protocol";
    shell_errors << "Can't assign requested address";

    // some /sbin/ip errors
    shell_errors << "Object .* is unknown, try \"ip help\"";
    shell_errors << "Cannot find device";
    shell_errors << "Error: an inet prefix is expected rather than";

    iptables_errors << "'iptables --help' for more information.";
    iptables_errors << "'iptables-restore --help' for more information.";
    iptables_errors << "iptables-restore: line .* failed";

    pfctl_errors << "pfctl: Syntax error in config file:";
    pfctl_errors << "Syntax error in config file:";
    pfctl_errors << "skipping rule due to errors";
    pfctl_errors << "errors in queue definition";
    pfctl_errors << "error setting skip interface(s)";
    pfctl_errors << "errors in altq config";

    route_add_errors << "Error: Routing rule .* couldn't be activated";
}

SSHUnx::~SSHUnx()
{
}

bool SSHUnx::checkForErrors(QStringList *errptr)
{
#if STATE_MACHINE_DEBUG
    if (fwbdebug)
        qDebug(
            QString("SSHUnx::stateMachine:  Checking for errors. Buffer='%1'").
            arg(stdoutBuffer).toLatin1().constData());

#endif

    foreach (QString err, *errptr)
    {
        if (stdoutBuffer.lastIndexOf(QRegExp(err), -1) != -1)
        {
            if (fwbdebug)
                qDebug("SSHUnx::stateMachine: MATCH. Error detected.");

            emit printStdout_sign( tr("\n*** Fatal error :") );
            emit printStdout_sign( stdoutBuffer + "\n" );
            stdoutBuffer = "";
            sessionComplete(true);   // finish with error status
            return true;
        }
    }
    return false;
}

bool SSHUnx::checkForErrors()
{
    switch (state)
    {
    case LOGGEDIN:
        if (checkForErrors(&errorsLoggedin)) return true;
        break;

    default:
        if (checkForErrors(&errorsInit)) return true;
        break;
    }

    if (checkForErrors(&iptables_errors)) return true;
    if (checkForErrors(&pfctl_errors)) return true;
    if (checkForErrors(&route_add_errors)) return true;
    if (checkForErrors(&shell_errors)) return true;

    return false;
}

void SSHUnx::stateMachine()
{
    if (checkForErrors()) return;
#if STATE_MACHINE_DEBUG
    if (fwbdebug)
        qDebug("SSHUnx::stateMachine: state=%d",state);
#endif

    //entry:
    switch (state)
    {
    case NONE:
    {
        if ( cmpPrompt(stdoutBuffer, ssh_pwd_prompt) ||
             cmpPrompt(stdoutBuffer, putty_pwd_prompt) ||
             cmpPrompt(stdoutBuffer, thinkfinger_pwd_prompt) ||
             stdoutBuffer.lastIndexOf(passphrase_prompt, -1)!=-1 ||
             cmpPrompt(stdoutBuffer, sudo_pwd_prompt_1) ||
             cmpPrompt(stderrBuffer, sudo_pwd_prompt_1) ||
             cmpPrompt(stdoutBuffer, sudo_pwd_prompt_2) ||
             cmpPrompt(stderrBuffer, sudo_pwd_prompt_2)
        )
        {
            stdoutBuffer="";
            proc->write( pwd.toLatin1() );
            proc->write( "\n" );
            break;
        }
        // we may get to LOGGEDIN state directly from NONE, for
        // example when password is supplied on command line to
        // plink.exe
        if (cmpPrompt(stdoutBuffer,normal_prompt) ||
            cmpPrompt(stdoutBuffer,fwb_prompt))
        {
            state = PUSHING_CONFIG;

            // start sending keepalive chars (just "\n", done in
            // SSHSession::heartBeat()) to keep session alive and to
            // force firewall to restore session state after policy
            // has been reloaded  and state possibly purged.
            //
            // Disabled for SF bug 3020381
            // We should really use ssh kkepalives instead. See
            // FirewallInstaller::packSSHArgs() where we add command lne
            // options to activate ssh keepalive. This does not work
            // on Windows with plink.exe that does not support these
            // command line options !
            // 
            //
#ifdef _WIN32
            send_keepalive = true;
#endif

            if (!quiet) emit printStdout_sign( tr("Logged in") + "\n" );
            if (fwbdebug) qDebug("SSHUnx::stateMachine logged in");
            goto push_files;
        }

        if (fwbdebug) qDebug() << stdoutBuffer;

        QString fingerprint;
        //int n1,n2;
        if (stdoutBuffer.indexOf(newKeyOpenSSH)!=-1 ||
            stdoutBuffer.indexOf(newKeyPlink)!=-1   ||
            stdoutBuffer.indexOf(newKeyVsh)!=-1     ||
            stdoutBuffer.indexOf(newKeySSHComm)!=-1)
        {
/* new key */
            bool unix_y_n = (stdoutBuffer.indexOf(newKeyOpenSSH)!=-1 ||
                             stdoutBuffer.indexOf(newKeySSHComm)!=-1);

            fingerprint = findKeyFingerprint(stdoutBuffer);

            QString msg = newKeyMsg.arg(host).arg(fingerprint).arg(host);

            stopHeartBeat();

            int res =QMessageBox::warning( parent, tr("New RSA key"), msg,
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

/* in this state we may need to enter sudo password */
    case PUSHING_CONFIG:
 push_files:
        if ( cmpPrompt(stdoutBuffer, sudo_pwd_prompt_1) ||
             cmpPrompt(stderrBuffer, sudo_pwd_prompt_1) ||
             cmpPrompt(stdoutBuffer, sudo_pwd_prompt_2) ||
             cmpPrompt(stderrBuffer, sudo_pwd_prompt_2)
        )
        {
            stdoutBuffer="";
            proc->write( pwd.toLatin1() );
            proc->write( "\n" );
            break;
        }
/*
        if (!quiet && !verbose)
        {
            emit printStdout_sign( stdoutBuffer );
        }
*/
        stdoutBuffer="";

        if (input.size()!=0)
        {
            if (fwbdebug) qDebug("SSHUnx::stateMachine - sending a file");
            emit updateProgressBar_sign(input.size(),true);
            connect(proc,SIGNAL(bytesWritten(qint64)),this,SLOT(readyToSend()));
            sendLine();
            break;
        }
        break;

/* we get to this state when previous ssh or scp command terminates */
    case FINISH:
        if ( (proc->state()==QProcess::NotRunning) &&
             (proc->exitStatus()==QProcess::NormalExit))
        {
            emit printStdout_sign( "\n");
            emit printStdout_sign( tr("Done") );
            emit printStdout_sign( "\n");

            delete proc;
            proc=nullptr;

            state=NONE;

            break;
        } else
        {
            emit printStdout_sign( "\n");
            emit printStdout_sign( tr("Error in SSH") );
            emit printStdout_sign( "\n");

//            terminate();
            sessionComplete(true);   // finish with error status
            proc=nullptr;
        }

        emit sessionFinished_sign();
        break;

    default: break;
    }
}

