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

#include "SSHSession.h"
#include "instConf.h"

#include <qobject.h>
#include <qtimer.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qeventloop.h>
//#include <qcstring.h>
#include <qtextcodec.h>
#include <qapplication.h>
#include <qdatetime.h>
#include <QtDebug>

#include <iostream>

#ifdef _WIN32
#  include <windows.h>
#endif


using namespace std;

const char  *SSHSession::newKeyOpenSSH  ="Are you sure you want to continue connecting (yes/no)?";
const char  *SSHSession::newKeyPlink    ="Store key in cache? (y/n)";
const char  *SSHSession::newKeyVsh      ="Accept and save? (y/n)";
const char  *SSHSession::newKeySSHComm  ="You can get a public key's fingerprint by running";


const char  *SSHSession::fingerprintPrompt1="key fingerprint is";
const char  *SSHSession::fingerprintPrompt2="Key fingerprint:";



SSHSession::SSHSession(QWidget *_par,
                       const QString &_h,
                       const QStringList &_args,
                       const QString &_p,
                       const QString &_ep,
                       const list<string> &_in)
{
    parent = _par;
    host = _h;
    args = _args;
    pwd  = _p;
    epwd = _ep;
    input   = _in;
    quiet   = false;
    verbose = false;
    closeStdin = false;
    error = false;
    endOfCopy = false;
    send_keepalive = false;

    proc = NULL;
    retcode = 0;
    heartBeatTimer = new QTimer(this);
    connect(heartBeatTimer, SIGNAL(timeout()), this, SLOT(heartBeat()) );

    newKeyMsg = tr("You are connecting to the firewall <b>'%1'</b> for the first time. It has provided you its identification in a form of its host public key. The fingerprint of the host public key is: \"%2\" You can save the host key to the local database by pressing YES, or you can cancel connection by pressing NO. You should press YES only if you are sure you are really connected to the firewall <b>'%3'</b>.");


    fwb_prompt="";
    quiet=false;
    verbose=false;
    backup=false;
    incremental=false;
    dry_run=false;
    testRun=false;
    stripComments=false;
    wdir="";
    script="";
    backupFile="";
    save_diff="";
    diff_pgm="";
    diff_file="";

}

QString SSHSession::findKeyFingerprint(QString &buffer)
{
    const char *fp = fingerprintPrompt1;
    int n1,n2;

    if ( (n1=buffer.indexOf(fp))==-1)
    {
        fp = fingerprintPrompt2;
        if ( (n1=buffer.indexOf(fp))==-1)
            return QString("");
    }

    n1 += strlen(fp)+1;
    n2 = buffer.indexOf("\n", n1+4);
    return buffer.mid(n1,n2-n1);
}

void SSHSession::startSession()
{
    proc = new QProcess();
    retcode = -1;

    startHeartBeat();

    if (fwbdebug)
        qDebug("SSHSession::startSession  this=%p  proc=%p   heartBeatTimer=%p",
               this,proc,heartBeatTimer);

    connect(proc,SIGNAL(readyReadStandardOutput()),
            this,  SLOT(readFromStdout() ) );
    connect(proc,SIGNAL(readyReadStandardError()),
            this,  SLOT(readFromStderr() ) );
    connect(proc,SIGNAL(finished( int, QProcess::ExitStatus )),
            this,  SLOT(finished( int ) ) );

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("latin1"));

    assert(args.size() > 0);

    QStringList arguments;
    QStringList::const_iterator i=args.begin();
    QString program = *i;
    ++i;

    for ( ; i!=args.end(); ++i)
    {
        arguments << *i;
        //proc->addArgument( *i );
        cmd += *i;
    }


    QStringList env;

#ifdef _WIN32
    env.push_back( QString("APPDATA=")+getenv("APPDATA") );
    env.push_back( QString("HOMEPATH=")+getenv("HOMEPATH") );
    env.push_back( QString("HOMEDRIVE=")+getenv("HOMEDRIVE") );
    env.push_back( QString("ProgramFiles=")+getenv("ProgramFiles") );
/* NB: putty absolutely needs SystemRoot env. var. */
    env.push_back( QString("SystemRoot=")+getenv("SystemRoot") );
    env.push_back( QString("TEMP=")+getenv("TEMP") );
    env.push_back( QString("USERNAME=")+getenv("USERNAME") );
    env.push_back( QString("USERPROFILE=")+getenv("USERPROFILE") );

    env.push_back( QString("HOME=")+getenv("HOMEPATH") );
    env.push_back( QString("USER=")+getenv("USERNAME") );
#else
    env.push_back( QString("HOME=")+getenv("HOME") );
    env.push_back( QString("USER=")+getenv("USER") );
#endif

    env.push_back( QString("TMP=")+getenv("TMP") );
    env.push_back( QString("PATH=")+getenv("PATH") );
    env.push_back( QString("SSH_AUTH_SOCK=")+getenv("SSH_AUTH_SOCK") );

//    emit printStdout_sign( tr("Running command %1\n").arg(cmd) );

    proc->setEnvironment(env);

    if (fwbdebug)
    {
        qDebug("Launch external ssh client %s", program.toAscii().constData());
        qDebug("Arguments:");
        QStringList::const_iterator i;
        for (i=arguments.begin(); i!=arguments.end(); ++i)
            qDebug("    %s", (*i).toAscii().constData());
    }

    proc->start(program, arguments);

    if ( !proc->waitForStarted() )
    {
        emit printStdout_sign( tr("Failed to start ssh") + "\n" );
        return;
    }

    if (fwbdebug)
        qDebug("SSHSession::startSession   started child process");


    logged_in = false;
    enable    = false;
    configure = false;
    state = NONE;
}

SSHSession::~SSHSession()
{
    if (fwbdebug) qDebug("SSHSession::destructor");
    terminate();
    if (fwbdebug) qDebug("SSHSession::destructor done");
}

/*
 * this is redundant and wrong. Should just copy a pointer to instConf
 * object and use that instead of making local copy of each flag.
 */
void SSHSession::setOptions(instConf *cnf)
{
    setQuiet(cnf->quiet);
    setVerbose(cnf->verbose);
    setBackup(cnf->backup);
    setBackupFile(cnf->backup_file);
    setIncr(cnf->incremental);
    setDryRun(cnf->dry_run);
    setSaveStandby(cnf->saveStandby);
    setTestRun(cnf->testRun);
    setStripComments(cnf->stripComments);
    setWDir(cnf->wdir);
    setScript(cnf->script);
    setSaveDiff(cnf->save_diff);
    setDiffPgm(cnf->diff_pgm);
    setDiffFile(cnf->diff_file);
}

void SSHSession::terminate()
{
    if (fwbdebug)
        qDebug() << "SSHSession::terminate     this=" << this
                 << "proc=" << proc
                 << "heartBeatTimer=" << heartBeatTimer;

    stopHeartBeat();

    if (proc!=NULL)
    {
        disconnect(proc,SIGNAL(finished(int, QProcess::ExitStatus)),
                   this,SLOT(finished(int) ) );

        if (fwbdebug)
            qDebug() << "SSHSession::terminate   terminating child process";

#ifdef _WIN32
        if (proc->pid() != NULL)
#else
        if (proc->pid() != -1)
#endif
        {
            if (proc->state() == QProcess::Running)
            {
                Q_PID pid = proc->pid();

                if (fwbdebug)
                    qDebug() << "Process is running pid=" << pid;

                emit printStdout_sign(
                    QString("Stopping background process pid=%1").arg(pid));

                /*
                 * on windows proc->terminate() posts a WM_CLOSE
                 * message to all toplevel windows of the child
                 * process. However, since our child process is a
                 * console app (ssh client), this does nothing. Need
                 * to use proc->kill() on windows right away to avoid
                 * timeout.
                 */
#ifdef _WIN32
                proc->kill();
#else
                proc->terminate();
#endif

                if (fwbdebug)
                    qDebug() << "Terminate signal sent, waiting for it to finish";

                for (int timeout = 0; timeout < 10; ++timeout)
                {
                    QString s = QString(proc->readAllStandardOutput());
                    if (!quiet)
                    {
                        s.replace('\r',"");
                        emit printStdout_sign(s);
                    }
                    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
                    if (proc->state() != QProcess::Running) break;
                    sleep(1);
                }

                if (fwbdebug) qDebug() << "Reading last output buffers";

                QString s = QString(proc->readAllStandardOutput());
                if (!quiet)
                {
                    s.replace('\r',"");
                    emit printStdout_sign(s);
                }

                if (fwbdebug) qDebug() << "done reading I/O buffers";

                if (proc->state() == QProcess::Running)
                {
                    if (fwbdebug) qDebug() << "Still running, killing";
                    proc->kill();
                }
            }

            delete proc;
            proc = NULL;
            retcode = -1;
        }
    }

    if (fwbdebug) qDebug("SSHSession::terminate   done");
}

bool SSHSession::checkForErrors()
{
    return true;
}

void SSHSession::stateMachine()
{
}

/*
 * signal wroteToStdin is connected to slot readyToSend. Can not send
 * next line in this slot because on win32 it emits the signal and
 * thus calls the same slot recursively, without exiting first. On
 * Linux and Mac it seems to exit and then emit the signal and call
 * slot on the next pass of the even loop. Since on win32 this does
 * not happen, need to schedule sending next line via single shot
 * timer instead of calling it directly.
 */
void SSHSession::readyToSend()
{
    QTimer::singleShot( 0, this, SLOT(sendLine()) );
}

void SSHSession::sendLine()
{
    int n=0;
    while (input.size()!=0 && n<10)
    {
        string s = input.front();
        s = s + "\n";
#if STATE_MACHINE_DEBUG
        if (fwbdebug)
            qDebug("SSHSession::sendLine : %d lines to go -- %s",
                   int(input.size()), s.c_str());
#endif
        input.pop_front();

        stdoutBuffer="";

/* it is important that we use writeToStdin(QByteArray &) rather than
 * writeToStdin(QString &) because the latter performs implicit
 * conversion into local locale assuming the string is in Unicode. The
 * string in our case is actually in whatever encoding the firewall
 * script is written to the local filesystem, which may or may not be
 * UTF-8 but is definitely not Unicode. The conversion not only breaks
 * comments that were entered in UTF-8, it makes QProcess miscalculate
 * number of characters in comment lines using UTF-8 which in turns
 * breaks the script even worse because it glues consequitive lines
 * together. Apparently this has been fixed in latest versions of QT
 * 3.x but this is still broken in QT 3.1 which is shipping with
 * RedHat 9 and some other still popular distributions. Since we need
 * to support old QT 3.x, the code must work around this problem.
 */
        QByteArray buf;
        buf = s.c_str();
        proc->write/*ToStdin*/(buf);

        n++;
    }
    emit updateProgressBar_sign(input.size(),false);

    if (input.size()==0)
    {
        if (fwbdebug) qDebug("SSHUnx::sendLine - entire file sent, closeStdin=%d",
                             closeStdin);
        endOfCopy = true;
    }
}

void SSHSession::allDataSent()
{
    if (fwbdebug)
        qDebug("SSHSession::allDataSent   closing stdin");

    disconnect(proc,SIGNAL(bytesWritten(qint64)),this,SLOT(readyToSend()));

#ifdef _WIN32
    Sleep(2000);
#endif
    proc->closeWriteChannel();
#ifdef _WIN32
    Sleep(1000);
#endif
    readFromStdout();
}

void SSHSession::startHeartBeat()
{
    if (fwbdebug) qDebug("SSHSession::startHeartBeat");
    heartBeatTimer->start(1000);
}

void SSHSession::stopHeartBeat()
{
    if (fwbdebug) qDebug("SSHSession::stopHeartBeat");
    heartBeatTimer->stop();
    send_keepalive = false;
}

void SSHSession::heartBeat()
{
    if (fwbdebug) qDebug("SSHSession::heartBeat");
    if (send_keepalive) proc->write("\n");
    readFromStderr();
    readFromStdout();
    if (endOfCopy && closeStdin)
    {
        allDataSent();
        endOfCopy = false;
    }
}

void SSHSession::readFromStdout()
{
    if (proc)
    {
        QByteArray ba = proc->readAllStandardOutput();
        int  basize  = ba.size();
        if (basize==0) return;

        QString buf(ba);

        stdoutBuffer.append(buf);

        if (fwbdebug)
            qDebug("SSHSession::readFromStdout: '%s'", buf.toAscii().constData());

        bool endsWithLF = buf.endsWith("\n");
        QString lastLine = "";

        // split on LF
        QStringList bufLines = buf.split("\n", QString::KeepEmptyParts);

        // if buf ends with a LF character, the last element in the list is
        // an empty string
        if (endsWithLF && bufLines.last().isEmpty())  bufLines.pop_back();

        // if buf does not end with LF, last element in the list is
        // incomplete line of text
        if (!endsWithLF)
        {
            lastLine = bufLines.last();
            bufLines.pop_back();
        }

        if (bufLines.size() > 0)
        {
            /*
             * elements that are left in the list are all complete
             * lines of text.
             *
             * IMPORTANT: QT processes events when we emit signal
             * here. This means SSHSession::readFromStdout() (this
             * method) gets called recursively. If we print log lines
             * one by one, more log lines will be printed after the
             * first and they end up appearing in a strange order.
             *
             * See bug #465
             */
            QString s = pendingLogLine + bufLines.join("\n");
            pendingLogLine = "";
            if (!quiet)
            {
                s.replace('\r', "");
                emit printStdout_sign(s);
            }
        }

        pendingLogLine += lastLine;

        stateMachine();
    }
}

/*
 * note: we set qprocess channel mode to "merged" but despite that
 * QProcess does not seem to merge stdout and stderr on Windows 
 * (QT 4.4.0)
 * Will merge them here
 */
void SSHSession::readFromStderr()
{
    if (proc)
    {
        QByteArray ba = proc->readAllStandardError();
        if (ba.size()!=0)
        {
            QString s=QString(ba);
            if (fwbdebug) qDebug("SSHSession::readFromStderr  buf=%s",
                                 s.toAscii().constData());
            emit printStdout_sign(s);
            stdoutBuffer.append(s);
            stateMachine();
        }
    }
}

void SSHSession::sessionComplete(bool err)
{
    if (fwbdebug) qDebug("SSHSession::sessionComplete  err=%d", err);
    heartBeatTimer->disconnect(SIGNAL(timeout()));

    error = err;
    if (error)
        emit sessionFatalError_sign();
    else
        emit sessionFinished_sign();
}

void SSHSession::finished(int retcode)
{
    if (fwbdebug) qDebug("SSHSession::processExited");
    if (fwbdebug) qDebug("SSHSession::processExited   proc=%p retcode=%d",
                         proc, retcode);

    // background process has exited now, we do not need proc object anymore
    delete proc;
    proc=NULL;

    QString exitStatus = (retcode)?QObject::tr("ERROR"):QObject::tr("OK");

    emit printStdout_sign(tr("SSH session terminated, exit status: %1").arg(
                              retcode) + "\n");
    sessionComplete( retcode!=0 );
//    if (retcode) error=true;
//    emit sessionFinished_sign();
}

bool SSHSession::cmpPrompt(const QString &str, const QString &prompt)
{
#if STATE_MACHINE_DEBUG
    if (fwbdebug)
        qDebug("SSHSession::cmpPrompt: str='%s' prompt='%s'",
               str.toAscii().constData(),prompt.toAscii().constData());
#endif

    bool res = false;
    if (!str.isEmpty())
    {
        res = (str.lastIndexOf(prompt,-1) != -1);
        if (!res)
        {
            QString s = str.trimmed();
            res = (s.lastIndexOf(prompt,-1) != -1);
        }
    }
#if STATE_MACHINE_DEBUG
    if (fwbdebug) qDebug("SSHSession::cmpPrompt: res=%d",res);
#endif
    return res;
}

bool SSHSession::cmpPrompt(const QString &str,const QRegExp &prompt)
{
#if STATE_MACHINE_DEBUG
    if (fwbdebug)
        qDebug("SSHSession::cmpPrompt: str='%s' prompt='%s' (regexp)",
               str.toAscii().constData(),prompt.pattern().toAscii().constData());
#endif
    if (str.isEmpty()) return false;

    bool res=(str.lastIndexOf(prompt,-1)!=-1);
#if STATE_MACHINE_DEBUG
    if (fwbdebug)
        qDebug("SSHSession::cmpPrompt: res=%d",res);
#endif
    return res;
}



