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

#include "SSHPIX.h"

#include <qobject.h>
#include <qtimer.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <qfileinfo.h>
#include <QtDebug>


using namespace std;

SSHPIX::SSHPIX(QWidget *_par,
               const QString &_h,
               const QStringList &args,
               const QString &_p,
               const QString &_ep,
               const std::list<std::string> &_in) :
    SSHCisco(_par,_h,args,_p,_ep,_in)
{
}

SSHPIX::~SSHPIX()
{
}

void SSHPIX::stateMachine()
{
    if (checkForErrors()) return;

    if (fwbdebug)
        qDebug() << "SSHPIX::stateMachine()  state=" << state
                 << "(ENABLE=" << ENABLE << ")"
                 << "(PUSHING_CONFIG=" << PUSHING_CONFIG << ")"
                 << " stdoutBuffer=" << stdoutBuffer;

    switch (state)
    {
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

    case GET_ACLS:
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
            QTimer::singleShot( 0, this, SLOT(getACLs()) );
        }
        break;

    case GET_OG:
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
            QTimer::singleShot( 0, this, SLOT(getObjectGroups()) );
        }
        break;


    case CLEAR_ACLS:
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
            QTimer::singleShot( 0, this, SLOT(clearACLs()) );
        }
        break;

    case CLEAR_OG:
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
            QTimer::singleShot( 0, this, SLOT(clearObjectGroups()) );
        }
        break;

    case EXIT:
        break;

    case FINISH:
        break;

    default:
        SSHCisco::stateMachine();
        break;
    }
}

void SSHPIX::PIXbackup()
{
    if (fwbdebug) qDebug("SSHPIX::PIXbackup  ");

    bool sv = verbose;
    verbose = false;

    emit printStdout_sign(
        tr("Making backup copy of the firewall configuration"));
    emit printStdout_sign( "\n");

    cmd(proc, "terminal pager 0");
    if (state==FINISH) return;

    if (fwbdebug) qDebug("terminal pager 0  done");

    QString cfg = cmd(proc, "show run");

    if (fwbdebug) qDebug("show run  done");

    verbose = sv;

    if (fwbdebug) qDebug("state=%d", state);

/* if state changed to FINISH, there was an error and ssh terminated */
    if (state==FINISH) return;
    if (state==COMMAND_DONE)
    {
        ofstream ofs(backupFile.toLatin1().constData());
        ofs << cfg.toLatin1().constData();
        ofs.close();

        backup = false;  // backup is done
        state = ENABLE;
    }

    proc->write( "\n" );
}

void SSHPIX::getACLs()
{
    if (fwbdebug) qDebug("SSHPIX::getACLs  ");

    bool sv=verbose;
    bool sq=quiet;
    verbose=false;
    quiet=true;

    QString sa = cmd(proc,"show access-list");

    QStringList showAcls;
    showAcls=sa.split("\n");

    verbose=sv;
    quiet=sq;

/* if state changed to FINISH, there was an error and ssh terminated */
    if (state==FINISH) return;
    if (state==COMMAND_DONE)
    {
        for (QStringList::iterator i=showAcls.begin(); i!=showAcls.end(); i++)
        {
//            if (fwbdebug) qDebug("%s",(*i).ascii());
            if ((*i).indexOf("access-list ")==0 && (*i).indexOf(";")==-1)
            {
                QString an=(*i).section(' ',1,1);
                if (an!="cached" && currentAcls.indexOf(an)==-1)
                    currentAcls.push_back(an);
            }
        }
        state=GET_OG;
    }

    proc->write( "\n" );
}

void SSHPIX::clearACLs()
{
    if (fwbdebug) qDebug("SSHPIX::clearACLs  ");

    emit printStdout_sign( "\n");
    emit printStdout_sign(tr("*** Clearing unused access lists"));
    emit printStdout_sign( "\n");

    QString ca;

    while (currentAcls.size()!=0)
    {
        ca=currentAcls.front();
        currentAcls.pop_front();
        if (newAcls.indexOf(ca)==-1)//newAcls.end())
        {
            if (fwbdebug)
                qDebug("clear access-list %s",ca.toLatin1().constData());

            cmd(proc,QString("clear access-list %1").arg(ca));

/* if state changed to FINISH, there was an error and ssh terminated */
            if (state==FINISH) return;
        }
    }

    state=CLEAR_OG;
    proc->write( "\n" );
}

void SSHPIX::getObjectGroups()
{
    if (fwbdebug) qDebug("SSHPIX::getObjectGroups  ");

    bool sv=verbose;
    bool sq=quiet;
    verbose=false;
    quiet=true;

    QString sog = cmd(proc,"show object-group");

    QStringList showOG;
    showOG=sog.split("\n");

    verbose=sv;
    quiet=sq;

/* if state changed to FINISH, there was an error and ssh terminated */
    if (state==FINISH) return;
    if (state==COMMAND_DONE)
    {
        for (QStringList::iterator i=showOG.begin(); i!=showOG.end(); i++)
        {
//            if (fwbdebug) qDebug("%s",(*i).ascii());
            if ((*i).indexOf("object-group ")==0)
            {
                QString ogn=(*i).section(' ',1,1);
                if (currentObjectGroups.indexOf(ogn)==-1)//currentObjectGroups.end())
                    currentObjectGroups.push_back(ogn);
            }
        }
        state=CLEAR_ACLS;
    }

    proc->write( "\n" );
}

void SSHPIX::clearObjectGroups()
{
    if (fwbdebug) qDebug("SSHPIX::clearObjectGroups  ");

    emit printStdout_sign( "\n");
    emit printStdout_sign(tr("*** Clearing unused object groups"));
    emit printStdout_sign( "\n");

    QString ca;

    while (currentObjectGroups.size()!=0)
    {
        ca=currentObjectGroups.front();
        currentObjectGroups.pop_front();
        if (newObjectGroups.indexOf(ca)==-1)//==newObjectGroups.end())
        {
            if (fwbdebug)
                qDebug("clear object-group %s",ca.toLatin1().constData());

            cmd(proc, QString("clear object-group %1").arg(ca));

/* if state changed to FINISH, there was an error and ssh terminated */
            if (state==FINISH) return;
        }
    }

    state = EXIT_FROM_CONFIG;
    emit printStdout_sign( tr("*** End ") + "\n" );
    proc->write( "exit\n" );
}

