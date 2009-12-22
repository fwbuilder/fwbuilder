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



#include "../../config.h"
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

#include <iostream>

#include <errno.h>
#ifndef errno
extern int errno;
#endif

using namespace std;

SSHPIX::SSHPIX(QWidget *_par,
               const QString &_h,
               const QStringList &args,
               const QString &_p,
               const QString &_ep,
               const std::list<std::string> &_in) : SSHSession(_par,_h,args,_p,_ep,_in)
{
    normal_prompt="> $";
    fwb_prompt="--**--**--";
    enable_prompt="# $|# *Access Rules Download Complete";
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
    errorsEnabledState.push_back("An object-group with the same id but different type");

    local_event_loop = new QEventLoop();
}

void SSHPIX::loadPreConfigCommands(const QStringList &cl)
{
    pre_config_commands = cl;
}

void SSHPIX::loadPostConfigCommands(const QStringList &cl)
{
    post_config_commands = cl;
}

SSHPIX::~SSHPIX()
{
}

QString SSHPIX::cmd(QProcess *proc,const QString &cmd)
{
    if (fwbdebug) qDebug("Command '%s'", cmd.toAscii().constData());
    stdoutBuffer="";

    proc->write( (cmd + "\n").toAscii() );
    state = EXECUTING_COMMAND;
    local_event_loop->exec();
    //qApp->processEvents();

    if (fwbdebug) qDebug("Command '%s' completed", cmd.toAscii().constData());

    return stdoutBuffer;
}


bool SSHPIX::checkForErrors()
{
    QStringList *errptr;

    switch (state)
    {
    case LOGGEDIN:  errptr= &errorsLoggedin;     break;
    case ENABLE:    errptr= &errorsEnabledState; break;
    default:        errptr= &errorsInit;         break;
    }

    for (QStringList::const_iterator i=errptr->begin();
         i!=errptr->end(); ++i)
    {
        if ( stdoutBuffer.lastIndexOf(*i,-1)!=-1 )
        {
            if (fwbdebug)
                qDebug(QString("Got known error message: %1").arg(stdoutBuffer).toAscii().constData());

            emit printStdout_sign( tr("\n*** Fatal error :") );
            emit printStdout_sign( stdoutBuffer+"\n" );
            stdoutBuffer="";
//            terminate();
            sessionComplete(true);   // finish with error status
            return true;
        }
    }
    return false;
}

void SSHPIX::stateMachine()
{
    if (checkForErrors()) return;

    switch (state)
    {
    case NONE:
    {
        if ( cmpPrompt(stdoutBuffer,QRegExp(pwd_prompt_1)) ||
             cmpPrompt(stdoutBuffer,QRegExp(pwd_prompt_2)) )
        {
            stdoutBuffer="";
            proc->write( (pwd + "\n").toAscii() );
            // proc->write( "\n" );
            break;
        }
/* we may get to LOGGEDIN state directly from NONE, for example when
 * password is supplied on command line to plink.exe
 */
        if (cmpPrompt(stdoutBuffer,QRegExp(normal_prompt)) )
        {
            stdoutBuffer="";
            state=LOGGEDIN;
            emit printStdout_sign( "\n");
            emit printStdout_sign( tr("Logged in") + "\n" );
            emit printStdout_sign( tr("Switching to enable mode...") + "\n");
            stdoutBuffer="";
            proc->write( "enable\n" );
            //proc->write( "\n" );
        }

/* we may even get straight to the enable prompt, e.g. if
 * user account is configured with "privilege 15"
 */
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
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
            if (!epwd.isEmpty()) proc->write( (epwd + "\n").toAscii() );
            else                 proc->write( "\n" );
            state=WAITING_FOR_ENABLE;
        }
        break;

    case WAITING_FOR_ENABLE:
        if ( cmpPrompt(stdoutBuffer,QRegExp(epwd_prompt)) )
        {
            stdoutBuffer="";
            if (!epwd.isEmpty()) proc->write( (epwd + "\n").toAscii() );
            else                 proc->write( "\n" );
            state=WAITING_FOR_ENABLE;
            break;
        }
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
            emit printStdout_sign( tr("In enable mode."));
            emit printStdout_sign( "\n");
            state=ENABLE;  // and go to ENABLE target in switch

            /* give classes derived from SSHPIX a chance to do
             * something before we switch to config mode. If <this> is
             * SSHPIX class, the stateMachine method will simply call
             * itself and will fall through to the ENABLE state.
             */
            stateMachine();
            break;
        }

    case ENABLE:
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
            if (pre_config_commands.size()>0)
            {
                stdoutBuffer="";

                QString cmd = pre_config_commands.front();
                pre_config_commands.pop_front();

                if (cmd.indexOf("reload in")!=-1)
                    state = SCHEDULE_RELOAD_DIALOG;

                proc->write( (cmd + "\n").toAscii() );
                // proc->write( "\n" );
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

            proc->write( "config t\n" );
            //proc->write( "\n" );
            state=WAITING_FOR_CONFIG_PROMPT;
        }
        break;

    case SCHEDULE_RELOAD_DIALOG:
        if ( cmpPrompt(stdoutBuffer,QRegExp("System config.* modified\\. Save?")) )
        {
            stdoutBuffer="";
            proc->write( "n" );  // no \n needed
            break;
        }
        if ( cmpPrompt(stdoutBuffer,QRegExp("Proceed with reload?")) )
        {
            stdoutBuffer="";
            proc->write( "y" );  // no \n needed
            break;
        }
        if ( cmpPrompt(stdoutBuffer,QRegExp("SHUTDOWN")) )
        {
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
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
            state=CONFIG;

/* install full policy */
            QString ff;
            QFileInfo script_info(script);
            if (script_info.isAbsolute()) ff = script;
            else ff = wdir + "/" + script;

            config_file = new ifstream(ff.toLatin1().constData());
            if ( ! *config_file)
            {
                emit printStdout_sign(
                    QObject::tr("Can not open file %1").arg(ff) + "\n"
                );
                state=FINISH;
                break;
            } else
            {
/* read the whole file */
                string s0;
                nLines =0;
                bool   store=!incremental;
                while ( !config_file->eof() )
                {
                    getline( *config_file, s0);
                    if (!store)
                    {
                        store=(s0.find("!################")==0);
                    }
                    if (store)
                    {
                        QString s(s0.c_str());
                        s = s.trimmed();
                        allConfig.push_back(s);
                        nLines++;
/*
 * store names of access-lists and object-groups actually used in the config
 */
                        if (s.indexOf("access-list ")==0)
                            newAcls.push_back(s.section(' ',1,1));
                        if (s.indexOf("object-group ")==0)
                            newObjectGroups.push_back(s.section(' ',1,1));
                    }
                }
                config_file->close();
                delete config_file;
                config_file=NULL;

                emit updateProgressBar_sign(nLines,true);
            }
            state=PUSHING_CONFIG; // and drop to PUSHING_CONFIG case
            if (!dry_run)
                emit printStdout_sign(tr("Pushing firewall configuration"));
            emit printStdout_sign( "\n");
            ncmd=0;
        }
#if 0
        else
        {
/* install incrementally */

            QTimer::singleShot( 1, this, SLOT(PIXincrementalInstall()) );
            break;
        }
#endif

    case PUSHING_CONFIG:
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
        loop1:
            if ( allConfig.size()!=0 )
            {
                QString s;

                do {
                    s = allConfig.front();
                    allConfig.pop_front();
                } while (stripComments && s[0]=='!');

                emit updateProgressBar_sign(allConfig.size(),false);

                s.replace('\"','\'');

                if (!verbose)
                {
                    QString rl="";
                    if (s.indexOf("! Rule ")!=-1)  rl=s.mid(7);
                    if ( !rl.isEmpty())
                    {
                        emit printStdout_sign( tr("Rule %1").arg(rl) + "\n" );
                        //emit printStdout_sign( "\n");
                    }
                }

                if (!dry_run)
                {
                    if ( !s.isEmpty())  ncmd++;
                    stdoutBuffer="";
                    proc->write( (s+"\n").toAscii() ); // send even if s is empty
                    qApp->processEvents();
                    break;
                } else
                {
                    emit printStdout_sign( s+"\n" );
                    goto loop1;
                }
                break;
            } else
            {
                /* allConfig.size()==0 */

//                state=GET_ACLS;
//                goto entry;

                state = EXIT_FROM_CONFIG;
                emit printStdout_sign( tr("End") + "\n" );
                proc->write( "exit\n" );
            }
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


    case EXIT_FROM_CONFIG:
        if ( cmpPrompt(stdoutBuffer,QRegExp(enable_prompt)) )
        {
            /*
             * NOTE: at this point we are still in the config mode!
             *
             * Execute post_config_commands and exit from config mode.
             */
            if (post_config_commands.size()>0)
            {
                stdoutBuffer="";

                QString cmd = post_config_commands.front();
                post_config_commands.pop_front();

                proc->write( (cmd + "\n").toAscii() );
                //proc->write( "\n" );
                break;
            }

            stdoutBuffer="";
            state=EXIT;
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
        ofs << cfg.toAscii().constData();
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
                qDebug("clear access-list %s",ca.toAscii().constData());

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
                qDebug("clear object-group %s",ca.toAscii().constData());

            cmd(proc, QString("clear object-group %1").arg(ca));

/* if state changed to FINISH, there was an error and ssh terminated */
            if (state==FINISH) return;
        }
    }

    state = EXIT_FROM_CONFIG;
    emit printStdout_sign( tr("*** End ") + "\n" );
    proc->write( "exit\n" );
}

void SSHPIX::PIXincrementalInstall()
{
    QString current_config;

    bool sv=verbose;
    verbose=false;

    emit printStdout_sign(tr("Reading current firewall configuration"));
    emit printStdout_sign( "\n");

    current_config = cmd(proc, "show run | grep ^telnet|^ssh|^icmp");
    if (state==FINISH) return;
    current_config += cmd(proc, "show object-group");
    if (state==FINISH) return;
    current_config += cmd(proc, "show access-list");
    if (state==FINISH) return;
    current_config += cmd(proc, "show global");
    if (state==FINISH) return;
    current_config += cmd(proc, "show nat");
    if (state==FINISH) return;
    current_config += cmd(proc, "show static");
    if (state==FINISH) return;

    verbose=sv;

    if (state==COMMAND_DONE)
    {
        QString statefile;
        QFileInfo script_info(script);
        if (script_info.isAbsolute()) statefile = script + "_current";
        else statefile = wdir + "/" + script + "_current";

        ofstream ofs(statefile.toLatin1().constData());
        ofs << current_config.toAscii().constData();
        ofs.close();

        emit printStdout_sign(tr("Generating configuration diff"));
        emit printStdout_sign( "\n");

        QString cm = diff_pgm + " \"" + statefile + "\" \"";
        if (script_info.isAbsolute()) cm += wdir + "/";
        cm += script + "\"";

//        emit printStdout_sign(tr("Running command: %1\n").arg(cm));

#ifdef _WIN32
        FILE *f = _popen( cm.toLatin1().constData(), "r");
#else
        FILE *f = popen( cm.toLatin1().constData(), "r");
#endif
        if (f==NULL)
        {
            emit printStdout_sign(
                tr("Fork failed for %1").arg(diff_pgm));
            emit printStdout_sign( "\n");
            switch (errno)
            {
            case EAGAIN:
            case ENOMEM:
                emit printStdout_sign(tr("Not enough memory.") + "\n");
                break;
            case EMFILE:
            case ENFILE:
                emit printStdout_sign(
                    tr("Too many opened file descriptors in the system.") + "\n");
                break;

            }
            emit printStdout_sign( "\n");
            state=FINISH;
            proc->write( "\n" );
            return;
        }

        char buf[1024];
        int  nLines=0;
        while (fgets(buf,1024,f))
        {
            allConfig += buf;
            nLines++;
        }
#ifdef _WIN32
        _pclose(f);
#else
        pclose(f);
#endif

	if (allConfig.isEmpty())
	{
	    allConfig=QStringList();
            emit printStdout_sign(tr("Empty configuration diff"));
            emit printStdout_sign( "\n");
	}

        if (save_diff)
        {
            ofstream odiff((wdir+"/"+diff_file).toLatin1().constData());
            odiff << allConfig.join("").toAscii().constData();
            odiff.close();
        }

        state=PUSHING_CONFIG;
        emit updateProgressBar_sign(nLines,true);
        if (!dry_run)
            emit printStdout_sign(tr("Pushing firewall configuration") + "\n");
    }
    proc->write( "\n" );
}

