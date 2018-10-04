/*

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

  Author:  Vadim Kurland vadim@fwbuilder.org

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
#include "utils_no_qt.h"

#include "FWBApplication.h"
#include "RCS.h"

// need this for FS_SEPARATOR
#include "fwbuilder/Tools.h"

//#include "FWWindow.h"

#include <qdir.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <qtextcodec.h>
#include <QtDebug>

#include <stdlib.h>

#if defined(_WIN32)
#  include <stdio.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <time.h>
#  include <sys/timeb.h>
#else
#  include <unistd.h>
#  include <string.h>
#  include <errno.h>
#  if defined(TM_IN_SYS_TIME)
#    include <sys/time.h>
#  else
#    include <time.h>
#  endif
#endif

#ifdef _WIN64
#define timezone _timezone
#endif

#include <iostream>

#ifndef PATH_MAX
#ifdef MAXPATHLEN
#define	PATH_MAX MAXPATHLEN
#else
#define	PATH_MAX 1024
#endif
#endif


using namespace std;
using namespace libfwbuilder;

QString     RCS::rcs_file_name = "";
QString     RCS::rlog_file_name = "";
QString     RCS::rcsdiff_file_name = "";
QString     RCS::ci_file_name = "";
QString     RCS::co_file_name = "";
RCSEnvFix*  RCS::rcsenvfix = nullptr;
bool        RCS::rcs_available = false;

/***********************************************************************
 *
 * class Revision
 *
 ***********************************************************************/
Revision::Revision()
{
}

Revision::Revision(const QString &file, const QString &r)
{
    filename = file;
    rev      = r;
}

Revision::Revision(const Revision &r)
{
    filename   = r.filename ;
    rev        = r.rev      ;
    date       = r.date     ;
    author     = r.author   ;
    locked_by  = r.locked_by;
    log        = r.log      ;
}

void Revision::operator=(const Revision &r)
{
    filename   = r.filename ;
    rev        = r.rev      ;
    date       = r.date     ;
    author     = r.author   ;
    locked_by  = r.locked_by;
    log        = r.log      ;
}

bool Revision::operator<(const Revision &r) const
{
    for(int i=1; ; i++)
    {
        QString v1=  rev.section(".",i,i);
        QString v2=r.rev.section(".",i,i);
        if (v1=="" && v2=="") return false;
        if (v1==v2) continue;
        if (v1=="" && v2!="") return true;
        if (v1!="" && v2=="") return false;
        if (v1.toInt()>v2.toInt()) return false;
        if (v1.toInt()<v2.toInt()) return true;
//        i++;
    }
    return true;
}

bool Revision::operator==(const Revision &r) const
{
    return rev==r.rev;
}

bool Revision::operator!=(const Revision &r) const
{
    return rev!=r.rev;
}

/***********************************************************************
 *
 * class RCSEnvFix
 *
 ***********************************************************************/
RCSEnvFix::RCSEnvFix()
{
    int tzoffset;
    QString tzsign = "";

    time_t clock;
    struct tm *ltm;
    time(&clock);
    ltm = (struct tm *) localtime(&clock);

#ifndef _WIN32
/*
 * struct tm has member tm_zone and should have member tm_gmtoff
 * autoconf checks only for tm_zone, but these two member fields come
 * together These fields are present in *BSD struct tm (including Mac
 * OS X). Linux manual does not mention them, but they are present
 * there as well. These files are missing in Windows version of time.h
 *
 * long tm_gmtoff is offset from UTC in seconds, it is negative in timezones
 * west of GMT
 */

    tzoffset = ltm->tm_gmtoff/60;
    if (tzoffset<0)
    {
        tzoffset = -1*tzoffset;
        tzsign = "-";
    } else {
        tzsign = "+";
    }

#else
// global variable timezone has seconds West of GMT (positive in
// timezones west of GMT)

    tzoffset = ((ltm->tm_isdst>0)?timezone-3600:timezone)/60;
    if (tzoffset<0)
    {
        tzoffset = -1*tzoffset;
        tzsign = "+";
    } else {
        tzsign = "-";
    }

#endif

    TZOffset.sprintf("%02d:%02d",tzoffset/60,tzoffset%60);
    TZOffset = tzsign + TZOffset;

    if (fwbdebug)
        qDebug("tzoffset: %d TZOffset: '%s'",tzoffset,TZOffset.toLatin1().constData());


#ifdef _WIN32
/* need this crap because Windows does not set environment variable TZ
 * by default, but rcs absolutely requires it. Even though I am using
 * option "-z" with all RCS commands, stupid RCS on windows does not
 * work if env var TZ is not set
 */
    env.push_back( QString("TZ=GMT")+TZOffset );

/*
 * NB: need to prepend installation directory in front of PATH on
 * windows, otherwise ci fails when GUI is launched by windows
 * explorer through file extension association. When the program is
 * launched from menu "Start", its working directory is the dir. where
 * it is installed. Since windows implies a '.' in front of PATH,
 * everything works. When the program is started with some other
 * directory as current dir, RCS tools fail without any error message.
 */
    env.push_back(
        QString("PATH=%1;%2").arg(getPathToBinary("").c_str()).arg(getenv("PATH")) );
#endif

/* also need to set env variable USER for rcs tools, but if the user name
 * contains spaces, replace them with underscores (like "John Smith")
 *
 * global variable QString user_name is set in common/init.cpp
 */
    QString uname = user_name;

    env.push_back( QString("USER=") + uname);
    env.push_back( QString("LOGNAME=") + uname);
    if (getenv("TMP") != nullptr)
        env.push_back( QString("TMP=") + getenv("TMP"));
    if (getenv("TEMP")!=nullptr)
        env.push_back( QString("TEMP=") + getenv("TEMP"));
}

QStringList* RCSEnvFix::getEnv()
{
    if (env.empty()) return nullptr;
    return &env;
}

/***********************************************************************
 *
 * class RCS
 *
 ***********************************************************************/

void RCS::init()
{
    if (rcs_file_name=="")
    {
        rcs_file_name = QStringLiteral("rcs");
        rlog_file_name = QStringLiteral("rlog");
        rcsdiff_file_name = QStringLiteral("rcsdiff");
        ci_file_name = QStringLiteral("ci");
        co_file_name = QStringLiteral("co");

#ifdef _WIN32
        rcs_file_name = QStringLiteral("rcs.exe");
        rlog_file_name = QStringLiteral("rlog.exe");
        rcsdiff_file_name = QStringLiteral("rcsdiff.exe");
        ci_file_name = QStringLiteral("ci.exe");
        co_file_name = QStringLiteral("co.exe");
#endif
    }

    // now check if rcs tools are available. To test, try to run rlog
    // with no arguments

    QStringList arglist;

    QProcess rcs_proc;
    rcs_proc.start( rlog_file_name, arglist );
    rcs_proc.waitForStarted();

    if (rcs_proc.state() != QProcess::Running)
    {
        rcs_proc.close();
        // rlog (and probably other RCS tools) are unavailable
        if (fwbdebug) qDebug() << "RCS tools unavailable";
        rcs_available = false;
        return;
    }

    rcs_proc.waitForFinished();
    rcs_proc.close();

    rcs_available = true;
}

RCS::RCS(const QString &file)
{
    if (rcsenvfix==nullptr) rcsenvfix = new RCSEnvFix();

    if (fwbdebug) qDebug() << "RCS::RCS(" << file << ")";

    // Using absoluteFilePath() rather than canonicalFilePath, see #1334
    QFileInfo fi(file);
    if (fi.exists()) filename = fi.absoluteFilePath();
    else filename = file;

    if (fwbdebug) qDebug() << "filename=" << filename;

    checked_out   = false;
    locked        = false;
    inrcs         = false;
    tracking_file = false;
    ro            = false;
    temp          = false;

    ciproc = new QProcess();
    proc = new QProcess();

    connect(proc, SIGNAL(readyReadStandardOutput()),
            this,  SLOT(readFromStdout() ) );
    connect(proc, SIGNAL(readyReadStandardError()),
            this,  SLOT(readFromStderr() ) );

    if (!fi.exists())
    {
        inrcs = false;
        tracking_file = true;
        return;
    }

    try
    {
        QString  rcspath = filename.left( filename.lastIndexOf("/") );
        QDir     rcsdir;
        rcsdir.cd(rcspath);

/*
 * rlog is started with environment defined by RCSEnvFix, which does
 * not have env. var LANG so it always runs in english
 */
        QString rl = rlog();
        QStringList split_log = rl.split(QRegExp("------|======"));

        QString head_section = split_log[0];

        QRegExp head_rx("head:\\s+([0-9\\.]+)\\s*\\n");
        int pos = head_rx.indexIn( head_section );
        if (pos>-1) head = head_rx.cap(1);

        QStringList::iterator i;
        for (i=split_log.begin(),++i; i!=split_log.end(); ++i)
        {
            QString section = *i;
            if (section.length()==0) continue;

            int match = -1;

            Revision r(filename);
            r.rev = "";
            r.log = "";

            QRegExp rev_rx("revision\\s+([0-9\\.]+)");
            match = rev_rx.indexIn( section );
            if (match>-1)
            {
                r.rev = rev_rx.cap(1);
            }

            QRegExp lock_rx("revision\\s+([0-9\\.]+)\\s+locked by:\\s+(\\S+);");
            lock_rx.setMinimal(true);
            match = lock_rx.indexIn( section );
            if (match>-1)
            {
                r.locked_by = lock_rx.cap(2);
                locked      = true;
                locked_by   = lock_rx.cap(2);
                locked_rev  = r.rev;
            }

// older implementation copied revision and "locked by" to r.log
// we'll do the same here to maintain compatibility
            QRegExp rev2_rx("(revision.+)\\n");
            rev2_rx.setMinimal(true);
            match = rev2_rx.indexIn( section );
            if (match>-1)
            {
                r.log += rev2_rx.cap(1) + "\n";
            }


            QRegExp date_rx("date:\\s+([^;]+);\\s+author:\\s+(\\S+);");
            date_rx.setMinimal(true);
            match = date_rx.indexIn( section );
            if (match>-1)
            {
                r.date   = date_rx.cap(1);
                r.author = date_rx.cap(2);
            }

            QRegExp log_rx("date:.*\\n(.*)$");
            log_rx.setMinimal(true);
            match = log_rx.indexIn( section );
            if (match>-1)
                r.log += log_rx.cap(1);

            r.log.replace('\r',"");


            if (r.rev != "")
            {
                revisions.push_back(r);
                if (fwbdebug)
                    qDebug("revision %s: '%s'",
                           r.rev.toLatin1().constData(),
                           r.log.toLatin1().constData());
            }
        }
        // sort list revisions; its defined like this:
        // QList<Revision> revisions
        qSort(revisions);

        inrcs         = true;
        tracking_file = true;
        selectedRev   = head;
    }
    catch (FWException &ex)
    {
        inrcs         = false;
        tracking_file = true;
    }
}

RCS::~RCS()
{
    delete ciproc;
    delete proc;
}

QStringList* RCS::getEnv()
{
    if (rcsenvfix==nullptr) rcsenvfix = new RCSEnvFix();
    return rcsenvfix->getEnv();
}

RCSEnvFix*   RCS::getRCSEnvFix()
{
    if (rcsenvfix==nullptr) rcsenvfix = new RCSEnvFix();
    return rcsenvfix;
}


void RCS::readFromStdout()
{
    QString s = QString(proc->readAllStandardOutput());
    //qDebug("RCS::readFromStdout() reads: %s",s.toLatin1().constData());
    stdoutBuffer=stdoutBuffer + s;
}

void RCS::readFromStderr()
{
    QString s = QString(proc->readAllStandardError());
    //qDebug("RCS::readFromStderr() reads: %s", s.toLatin1().constData());
    stderrBuffer=stderrBuffer + s;
}

void RCS::setFileName(const QString &fn)
{
    QFileInfo fi(fn);
    if (fi.exists()) filename = fi.absoluteFilePath();
    else filename = fn;
    if (fwbdebug)
        qDebug() << "RCS::setFileName fn =" << fn << "filename =" << filename;
}

/*********************************************************************
 *  trivial RCS integration
 */

void RCS::abandon()
{
    if (!isInRCS() || !rcs_available) return;

/* check out head revision and unlock it */
    QStringList arglist;

    arglist << "-q" << "-f" << QString("-z") + rcsenvfix->getTZOffset() << QString("-u") << filename ;

    stdoutBuffer="";
    stderrBuffer="";

    if (fwbdebug) qDebug("starting co with environment '%s'",
                         rcsenvfix->getEnv()->join(" ").toLatin1().constData());
    if (fwbdebug) qDebug("executing command '%s %s'",
                         co_file_name.toLatin1().constData(),
                         arglist.join(" ").toLatin1().constData());

    proc->setEnvironment(*rcsenvfix->getEnv());
    proc->start( co_file_name, arglist );
    proc->waitForStarted();

    if (fwbdebug) qDebug("running co");

    if (proc->state() == QProcess::Running)
    {
        proc->waitForFinished();
        if (proc->exitCode() == 0 && proc->state() == QProcess::NotRunning)
        {
            if (fwbdebug) qDebug("finished successfully");
            checked_out = false;
            locked      = false;
            selectedRev = head;
            return;
        }
    }
/* error. */

    selectedRev = "";

    checked_out=false;

    QString err = tr("Error checking file out: %1").arg(stderrBuffer);
    QMessageBox::critical(app->activeWindow(), "Firewall Builder", err, tr("&Continue") );

    throw(FWException(err.toLatin1().constData()));
}

/**
 *  initial RCS checkin
 */
void RCS::add()
{
    int i = filename.lastIndexOf("/");
    QString rcspath = filename.left(i);
    QDir rcsdir;

    if (!rcs_available)
    {
        QString err = QObject::tr("RCS tools are unavailable");
        if (fwbdebug) qDebug() << err;
        throw(FWException(err.toStdString()));
    }

    if (fwbdebug) qDebug() << "RCS::add()  will run " << rcs_file_name;

    rcsdir.cd(rcspath);

    if (!rcsdir.exists("RCS")) rcsdir.mkdir("RCS");

    QStringList arglist;

    arglist << "-q" << "-i" << "-kb" << QString("-z") + rcsenvfix->getTZOffset() << "-t-\"Initial checkin\"" << filename;

    stdoutBuffer="";
    stderrBuffer="";

    proc->setEnvironment(*rcsenvfix->getEnv());
    proc->start( rcs_file_name, arglist );
    proc->waitForStarted();

    if (proc->state() == QProcess::Running)
    {
        proc->waitForFinished();
        if (proc->state() == QProcess::NotRunning && proc->exitCode()==0)
        {
            arglist.clear();

            arglist << "-q" << "-u" << QString("-z") + rcsenvfix->getTZOffset() << filename;

            stdoutBuffer="";
            stderrBuffer="";

            proc->setEnvironment(*rcsenvfix->getEnv());
            proc->start( ci_file_name, arglist );
            proc->waitForStarted();

            if (proc->state() == QProcess::Running)
            {
                proc->waitForFinished();
                if (proc->state() == QProcess::NotRunning && proc->exitCode()==0)
                {
                    inrcs       = true;
                    selectedRev = "1.1";
                    head        = "1.1";
                    return;
                }
            }
        }
    }
    QByteArray outp = proc->readAllStandardOutput();
    QString msg = QObject::tr("Fatal error during initial RCS checkin of file %1 :\n %2\nExit status %3")
        .arg(filename).arg(outp.data()).arg(proc->exitCode());
    throw(FWException( msg.toLatin1().constData()  ));
}

bool RCS::isInRCS()
{
    if (!rcs_available) return false;

    if (tracking_file) return inrcs;

    QStringList arglist;

    arglist << QString("-z") + rcsenvfix->getTZOffset() << "-R" << filename;

    stdoutBuffer="";
    stderrBuffer="";

    proc->setEnvironment(*rcsenvfix->getEnv());

    proc->start( rlog_file_name, arglist );
    proc->waitForStarted();
    if (proc->state() != QProcess::Running)
        throw(FWException("Fatal error running rlog "));

    while (proc->state() == QProcess::Running) ; // cxx_sleep(1);

    if (proc->state() == QProcess::NotRunning && proc->exitCode()==1)
    {
/* exist status '1' means the file is not in RCS */
        inrcs=false;
    if (fwbdebug)
    {
          QByteArray outp = proc->readAllStandardOutput();
          qDebug("Error running rlog: %s",outp.data());
    }
        return false;
    }
    inrcs=true;
    return true;
}

bool RCS::co(bool force)
{
    return co(selectedRev,force);
}

/**
 *  RCS checkout
 *
 * possible situations:
 *
 * 1. file is not in RCS - do nothing, return false
 *
 * 2. need to open file read-only
 *
 *   2.1 requested revision is emty or the head: no need to
 *   checkout, just return true
 *
 *   2.2 need to open read-only, older revision: do checkout of that
 *   revision into temporary file without locking, change file name,
 *   set flag 'temp'
 *
 * 3. need to open read-write, but file is locked
 *
 *   3.1 file is locked by the same user: offer user a choice
 *   open read-only or continue editing or cancel
 *
 *   3.2 file is locked by another user: offer a choice open read-only
 *   or cancel
 *
 * 4. need to open read-write, any revision: do normal checkout and
 * lock
 *
 */
bool RCS::co(const QString &rev,bool force)
{
/* first check if filename is already in RCS */

    if (!rcs_available || !isInRCS()) return false;

    if (ro)
    {
        if (rev==head || rev=="") return true;

/* check out requested revision to stdout
 *
 * TODO: right now it loads the whole file into memory, then writes it
 * to the temp file. It should be more efficient to read and write in
 * chunks.
 *
 */
        QStringList arglist;

        arglist << QString("-q") << QString("-kb") <<
                QString("-z") + rcsenvfix->getTZOffset() << QString("-p")+rev << filename;

        stdoutBuffer="";
        stderrBuffer="";

        if (fwbdebug) qDebug("starting co with environment '%s'",
                             rcsenvfix->getEnv()->join("\n").toLatin1().constData());
        if (fwbdebug) qDebug("executing command '%s %s'",
                         co_file_name.toLatin1().constData(),
                         arglist.join(" ").toLatin1().constData());

        proc->setEnvironment(*rcsenvfix->getEnv());
        proc->start( co_file_name, arglist );
        proc->waitForStarted();

        if (fwbdebug) qDebug("running co");

        if (proc->state() == QProcess::Running)
        {
            proc->waitForFinished();

            if (proc->state() == QProcess::NotRunning && proc->exitCode()==0)
            {
                if (fwbdebug) qDebug("finished successfully");
#ifdef _WIN32
                char tname[1024];
                strncpy(tname, filename.left(filename.lastIndexOf("/")+1).toLatin1().constData(),sizeof(tname)-20);
                strcat(tname,"tmpXXXXXX");
                _mktemp(tname);
                int fd = _open(tname, _O_RDWR|_O_CREAT|_O_EXCL|_O_BINARY , _S_IREAD|_S_IWRITE );
#else
                char tname[PATH_MAX];
                strncpy(tname, filename.toLatin1().constData(), sizeof(tname)-20 );
                strcat(tname,"_temp_XXXXXX");
                int fd = mkstemp(tname);
#endif
        if (fd<0)
        {
                    QString err = tr("Error creating temporary file ")+tname+QString(" :\n")+strerror(errno);
                    QMessageBox::critical(app->activeWindow(), "Firewall Builder", err, tr("&Continue") );
                    throw(FWException(err.toLatin1().constData()));
        }
#ifdef _WIN32
                if (_write(fd,stdoutBuffer.toLatin1().constData(),stdoutBuffer.length() )<0)
        {
            _close(fd);
#else
                if ( write(fd,stdoutBuffer.toLatin1().constData(),stdoutBuffer.length() )<0)
        {
            close(fd);
#endif
                    QString err = tr("Error writing to temporary file ")+tname+QString(" :\n")+strerror(errno);
                    QMessageBox::critical(app->activeWindow(), "Firewall Builder", err, tr("&Continue") );
                    throw(FWException(err.toLatin1().constData()));
                }
                close(fd);

                filename    = tname;
                temp        = true;
                checked_out = false;
                locked      = false;
                selectedRev = rev;
                return true;
            }
        }

        selectedRev = head;

        QString err = tr("Error checking file out: %1").arg(stderrBuffer);
        QMessageBox::critical(app->activeWindow(), "Firewall Builder", err, tr("&Continue") );
        throw(FWException(err.toLatin1().constData()));

    } else
    {
        /* global variable QString user_name is set in common/init.cpp */
        QString me = user_name;
        if (locked)
        {
/* the file is already locked, can not just check it out like that */

            if (me!=locked_by)
            {
                switch (QMessageBox::warning(
                            app->activeWindow(),"Firewall Builder",
                            tr("File is opened and locked by %1.\nYou can only open it read-only.")
                            .arg(locked_by),
                            "Open &read-only", "&Cancel", QString::null,
                            0, 1 ) )
                {
                case 0:  ro=true;   return false;
                case 1:  throw(FWException("cancel opening file"));  break;
                }
            }

            if (force) goto checkout;

            switch ( QMessageBox::warning(app->activeWindow(), "Firewall Builder",
                                          tr("Revision %1 of this file has been checked out and locked by you earlier.\n\
The file may be opened in another copy of Firewall Builder or was left opened\n\
after the program crashed.").arg(locked_rev),
                                           tr("Open &read-only"), tr("&Open and continue editing"), tr("&Cancel"),
                                          0, 2 ) )
            {
            case 0:  ro=true;  return false;
            case 1:
/* continue working with the file */
                checked_out = true;
                locked      = true;
                selectedRev = locked_rev;
                return true;
            case 2:  throw(FWException("cancel opening file"));  break;
            }
        }

/* if the user wanted specific revision and it should be opened
 * read-only, we need to check it out into a temporary file without
 * locking
 */

 checkout:

/* check out and lock */
        QStringList arglist;
         arglist.clear();

        arglist << "-q";
        if (force)  arglist << "-f";
        arglist << QString("-l")+rev << QString("-z") + rcsenvfix->getTZOffset() << filename;

        stdoutBuffer="";
        stderrBuffer="";

        if (fwbdebug) qDebug("starting co with environment '%s'",
                             rcsenvfix->getEnv()->join("\n").toLatin1().constData());
        if (fwbdebug) qDebug("executing command '%s %s'",
                             co_file_name.toLatin1().constData(),
                             arglist.join(" ").toLatin1().constData());

        proc->setEnvironment(*rcsenvfix->getEnv());
        proc->start( co_file_name, arglist );
        proc->waitForStarted();

        if (fwbdebug) qDebug("running co");

        if (proc->state() == QProcess::Running)
        {
            proc->waitForFinished();
            if (proc->state() == QProcess::NotRunning && proc->exitCode()==0)
            {
                if (fwbdebug) qDebug("finished successfully");
                checked_out = true;
                locked      = true;
                selectedRev = rev;
                return true;
            }
        }
/* error. */

        selectedRev = head;

        QString err = tr("Error checking file out: %1").arg(stderrBuffer);
        QMessageBox::critical(app->activeWindow(), "Firewall Builder", err, tr("&Continue") );

        throw(FWException(err.toLatin1().constData()));
    }
    return false;
}


bool RCS::ci( const QString &_lm,
              bool unlock)
{
/* first check if filename is already in RCS */
    if (!rcs_available || !isInRCS()) return false;

    QString logmsg = _lm;

    if (logmsg.isEmpty()) logmsg="_";  // otherwise ci adds "*** empty log message ***"

    if (fwbdebug)
        qDebug("RCS::ci  log message (%d characters): '%s'",
               logmsg.length(), logmsg.toLatin1().constData());

    QStringList arglist;

    if (unlock) arglist << "-u";
    else        arglist << "-l";
    arglist << QString("-z") + rcsenvfix->getTZOffset();
    arglist << filename;

    stdoutBuffer="";
    stderrBuffer="";

    if (fwbdebug) qDebug("starting ci with environment '%s'",
                         rcsenvfix->getEnv()->join("\n").toLatin1().constData());

    QByteArray rcslog = logmsg.toUtf8();

    QString obuf;


/*
 * under some circumstances, ci may exit immediately (e.g. when there
 * were no changes done to the file and it won't expect any rcs log
 * record on stdin). In this case slot completeCI is called
 * immediately, even before we have a chance to enter event loop. We
 * need to make sure we do not enter event loop if this happens. We
 * use flag ciRunning to check for that.
 *
 * Also it seems on windows all data is sent to the process and slot
 * is called while we still are inside launch, so that once we exit
 * from it, all is done and there is no need to enter event loop.
 */
    ciRunning=true;
    ciproc->setEnvironment(*rcsenvfix->getEnv());
    ciproc->start( ci_file_name, arglist );
    ciproc->waitForStarted();
    if (ciproc->state() != QProcess::Running)
    {//if not started
        if (fwbdebug) qDebug("Checkin error: file=%s error=%s",
                             filename.toLatin1().constData(),obuf.toLatin1().constData());

        throw( FWException( (obuf+"\n"+
                             arglist.join(" ")+"\n"+
                             rcsenvfix->getEnv()->join("\n")).toLatin1().constData() ) );
    }

/* make a copy, omitting trailing '\0' so it won't get sent to ci */
    QByteArray rcslogCopy;
    rcslogCopy = rcslog;

    ciproc->write((const char*)rcslogCopy, rcslog.length());

    QByteArray arr;
    arr = "\n.\n";
    ciproc->write((const char*)(arr),arr.length());

    if (fwbdebug) qDebug("all data sent to ci");

    ciproc->waitForFinished();

    if (fwbdebug) qDebug("ci exited");

    if (ciproc->state() == QProcess::NotRunning && ciproc->exitCode()==0)
    {
        if (fwbdebug) qDebug("ci exited normally");
        if (unlock)
        {
            checked_out = false;
            locked      = false;
        }
        return true;
    }

    return true;
}

/**
 * rlog - run rlog in the background and collect RCS log
 *
 * As it turns out, we can not trust rlog option "-zLT" to properly
 * convert timezone information on Windows. This might be abug in the
 * ported rlog. When timezone is east of GMT, ci properly converts
 * when file is checked in, but rlog uses wrong sing and substracts
 * offset instead of adding it. Suppose we are in Japan time zone
 * (GMT+9), and file is checked in at 15:00 local time. Ci properly
 * writes checkin time as 6:00 GMT, but rlog reports it as 21:00 on a
 * previous day (it does -9 hours instead of +9 hours ). Option
 * "-z+09:00" works properly
 *
 */
QString RCS::rlog()
{
    if (!rcs_available)
        throw(FWException(QObject::tr("RCS tools are unavailable").toStdString()));

    QStringList arglist;

    arglist << QString("-z") + rcsenvfix->getTZOffset() << filename;
//    proc->addArgument( "-zLT" );

    if (fwbdebug)
        qDebug("Running rlog: %s %s",
               rlog_file_name.toLocal8Bit().constData(),
               arglist.join(" ").toLocal8Bit().constData());

    stdoutBuffer = "";
    stderrBuffer = "";

    //proc->setEnvironment(*rcsenvfix->getEnv());
    proc->start( rlog_file_name, arglist );
    proc->waitForStarted();

    if (proc->state() != QProcess::Running)
    {
        proc->close();
        throw(FWException("Fatal error running rlog "));
    }

    if (fwbdebug) qDebug("Running rlog");

    proc->waitForFinished();

    if (fwbdebug) qDebug("Running rlog: finished reading");
    // Note: we convert rlog comments to Utf8. Local8Bit does not seem
    // to work on windows, produces '????'
    QString rlogTxt = QString::fromUtf8(stdoutBuffer.toLatin1().constData());

    if (proc->state() == QProcess::NotRunning && proc->exitCode()==0)
        return rlogTxt;

    QString msg=QObject::tr("Fatal error running rlog for %1").arg(filename);
    throw( FWException( msg.toLatin1().constData() ) );
}

QStringList RCS::rcsdiff(const QString&)
{
    isDiff();
    QString temp = stdoutBuffer;
    return temp.split("\n");
}

bool RCS::isDiff(const QString &rev)
{
    if (!rcs_available)
        throw(FWException(QObject::tr("RCS tools are unavailable").toStdString()));

    QStringList arglist;

    arglist << "-q";
    if (rev!="") arglist << QString("-r")+rev;
    else
    {
        if (selectedRev!="") arglist << QString("-r")+selectedRev;
    }
    arglist << QString("-z") + rcsenvfix->getTZOffset() << filename;

    stdoutBuffer="";
    stderrBuffer="";

    proc->setEnvironment(*rcsenvfix->getEnv());
    proc->start( rcsdiff_file_name, arglist );
    proc->waitForStarted();


    if (proc->state() == QProcess::Running)
    {
        proc->waitForFinished();
        /*while (proc->state() == QProcess::Running)
        {
            QByteArray ba = proc->readAllStandardOutput();
            if (ba.size()!=0)  stdoutBuffer=stdoutBuffer + QString(ba);
        }*/
    } else
        throw(FWException("Fatal error running rcsdiff "));

//    while (proc->state() == QProcess::Running) ; // cxx_sleep(1);

    if (proc->state() == QProcess::NotRunning) return (proc->exitCode()!=0);
    QString msg = QObject::tr(
        "Fatal error running rcsdiff for file %1").arg(filename);
    throw( FWException( msg.toLatin1().constData() ) );
}

QString RCS::getHead()
{
    if (isInRCS()) return head;
    return "";
}

QString RCS::getSelectedRev()
{
    if (isInRCS()) return selectedRev;
    return "";
}
