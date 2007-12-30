/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: SSHUnx.cpp,v 1.24 2007/07/13 05:32:55 vkurland Exp $

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



#include "config.h"
#include "global.h"
#include "utils.h"

#include "SSHUnx.h"

#include <qobject.h>
#include <qtimer.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qeventloop.h>

#include <iostream>

using namespace std;

SSHUnx::SSHUnx(QWidget *_par,
               const QString &_h,
               const QStringList &args,
               const QString &_p,
               const QString &_ep,
               const list<string> &_in) : SSHSession(_par,_h,args,_p,_ep,_in)
{
    normal_prompt="> ";
    enable_prompt="# ";
    pwd_prompt="'s password: ";
    epwd_prompt="Password: ";
    ssh_pwd_prompt="'s password: ";
    ssoft_config_prompt="> ";
    sudo_pwd_prompt="Password:";
    putty_pwd_prompt="Password: ";
    passphrase_prompt="Enter passphrase for key ";

    errorsInit.push_back("Permission denied");
    errorsInit.push_back("Invalid password");
    errorsInit.push_back("Unable to authenticate");
    errorsInit.push_back("Sorry, try again");
    errorsInit.push_back("Too many authentication failures");

    errorsLoggedin.push_back("No such file or directory");
    errorsLoggedin.push_back("Cannot allocate memory");
    errorsLoggedin.push_back("pfctl: Syntax error in config file:");

    iptables_errors.push_back("'iptables --help' for more information.");
    iptables_errors.push_back("'iptables-restore --help' for more information.");
    iptables_errors.push_back("iptables-restore: line .* failed");
}

SSHUnx::~SSHUnx()
{
}

bool SSHUnx::checkForErrors(QStringList *errptr)
{
    if (fwbdebug)
        qDebug(QString("SSHUnx::stateMachine:  Checking for errors. Buffer='%1'").arg(stdoutBuffer).toAscii().constData());

    for (QStringList::const_iterator i=errptr->begin(); i!=errptr->end(); ++i)
    {
        if (fwbdebug)
            qDebug(QString("SSHUnx::stateMachine:  error='%1'").arg(*i).toAscii().constData());

        if ( stdoutBuffer.lastIndexOf(QRegExp(*i),-1)!=-1 )
        {
            if (fwbdebug)
                qDebug("SSHUnx::stateMachine: MATCH. Error detected.");

            emit printStdout_sign( tr("\n*** Fatal error :") );
            emit printStdout_sign( stdoutBuffer+"\n" );
            stdoutBuffer="";
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

    return false;
}

void SSHUnx::stateMachine()
{
    if (checkForErrors()) return;

    //entry:
    switch (state)
    {
    case NONE:
    {
        if ( cmpPrompt(stdoutBuffer,ssh_pwd_prompt) ||
             cmpPrompt(stdoutBuffer,putty_pwd_prompt) ||
             stdoutBuffer.lastIndexOf(passphrase_prompt,-1)!=-1 ||

             cmpPrompt(stdoutBuffer,sudo_pwd_prompt) ||
             cmpPrompt(stderrBuffer,sudo_pwd_prompt) ) 
        {
            stdoutBuffer="";
            proc->write( pwd.toAscii() );
            proc->write( "\n" );
            break;
        }
/* we may get to LOGGEDIN state directly from NONE, for example when
 * password is supplied on command line to plink.exe
 */
        if (cmpPrompt(stdoutBuffer,normal_prompt) ||
            cmpPrompt(stdoutBuffer,fwb_prompt))
        {
            state=PUSHING_CONFIG;
            if (!quiet) emit printStdout_sign( tr("Logged in") + "\n" );
            if (fwbdebug)
                qDebug("SSHUnx::stateMachine logged in");
//            proc->write( "\n" );
//            stdoutBuffer="";
            goto push_files;
        }

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
        if ( cmpPrompt(stdoutBuffer,sudo_pwd_prompt) ||
             cmpPrompt(stderrBuffer,sudo_pwd_prompt) ) 
        {
            stdoutBuffer="";
            proc->write( pwd.toAscii() );
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
        if ( (proc->state()==QProcess::NotRunning) && (proc->exitStatus()==QProcess::NormalExit))
        {
            emit printStdout_sign( "\n");
            emit printStdout_sign( tr("Done") );
            emit printStdout_sign( "\n");

            delete proc;
            proc=NULL;

            state=NONE;

            break;
        } else
        {
            emit printStdout_sign( "\n");
            emit printStdout_sign( tr("Error in SSH") );
            emit printStdout_sign( "\n");

//            terminate();
            sessionComplete(true);   // finish with error status
            proc=NULL;
        }

        emit sessionFinished_sign();
        break;

    default: break;
    }
}

