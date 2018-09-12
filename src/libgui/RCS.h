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


#ifndef __RCS_H_FLAG__
#define __RCS_H_FLAG__


#include <qobject.h>
#include <qprocess.h>
#include <qstringlist.h>


#include "fwbuilder/FWException.h"

class RCS;
class RCSFilePreview;

class Revision {

    friend class RCS;

 public:

    QString filename;
    QString rev;
    QString date;
    QString author;
    QString locked_by;
    QString log;

    Revision();
    Revision(const Revision &r);
    Revision(const QString &file, const QString &rev="");

    bool operator<(const Revision &r) const;
    bool operator==(const Revision &r) const;
    bool operator!=(const Revision &r) const;
    void operator=(const Revision &r);
};

/*
 * this class just sets environment variable TZ on Windows if it is not set
 */
class RCSEnvFix {
    QStringList env;
    QString     TZOffset;
 public:
    RCSEnvFix();
    QStringList* getEnv();
    QString      getTZOffset() { return TZOffset; }
};

class RCS : public QObject {

    friend class RCSFilePreview;

    Q_OBJECT;

    /*
     * RCSEnvFix object should be initialized in constructor of RCS so
     * it is created _after_ we complete initialization and assign
     * appRootDir because it needs appRootDir to set up PATH on
     * Windows.
     */
    static RCSEnvFix     *rcsenvfix;
    static QString        rcs_file_name;
    static QString        rcsdiff_file_name;
    static QString        rlog_file_name;
    static QString        ci_file_name;
    static QString        co_file_name;
    static bool           rcs_available;

    QString               stdoutBuffer;
    QString               stderrBuffer;
    QProcess             *proc;
    QProcess             *ciproc;
    bool                  ciRunning;;
    bool                  tracking_file;
    bool                  inrcs;
    bool                  checked_out;
    bool                  locked;
    QString               locked_by;
    QString               locked_rev;
    QString               head;         // head revision
    QString               selectedRev;  // selected revision
    bool                  ro;           // if file is to be opened read-only

    QString               filename;
    bool                  temp;         // if filename is a temporary file
    QList<Revision>  revisions;

    /**
     * Retrieves RCS log.
     */
    QString rlog();


 public:

    RCS( const QString &filename );
    virtual ~RCS();

    static void init();

    /**
     * returns head revision of the file
     */
    QString getFileName()                  { return filename; }
    void    setFileName(const QString &fn);

    QList<Revision>::iterator begin() { return revisions.begin(); }
    QList<Revision>::iterator end()   { return revisions.end();   }

    void  add();

    /**
     * this makes RCS object "forget" about the file
     */
    void  abandon();

    /**
     * tells whether the file associated with RCS object is in RCS
     */
    bool   isInRCS();

    /**
     * RCS checkout. Returns true if successfull and false if file is
     * not in RCS. In case of error throws exception
     */
    bool  co(const QString &rev,bool force=false);

    /**
     * checks out currently selected revision (set using setSelectedRev)
     */
    bool  co(bool force=false);

    /**
     * RCS checkin. Returns true if successfull and false if file is
     * not in RCS. In case of error throws exception
     */
    bool  ci(const QString &logmsg =" ", bool unlock=false);

    /**
     * Retrieves RCS diff.
     */
    QStringList rcsdiff(const QString &rev="");

    /**
     * checks if the working copy of the file is different from RCS
     * revision 'rev'. If rev is empty string, selected revision is
     * used. If no revision has been selected, the latest revision of
     * the default branch is used.
     *
     * This is essentially just a code returned by rcsdiff with all
     * its output ignored.
     */
    bool isDiff(const QString &rev="");

    /**
     * these two methods just return status
     */
    bool    isCheckedOut() { return checked_out; }
    bool    isLocked()     { return locked;      }
    QString getLockedBy()  { return locked_by;   }

    /**
     * class RCS helps carry flag 'read-only' together with the rest
     * of the file info
     */
    void setRO(bool f) { ro=f; }
    bool isRO() { return ro; }

    /**
     * flag 'temp' indicates checkout has been done into temporary file
     */
    bool isTemp() { return temp; }

    /**
     * returns head revision of the file
     */
    QString getHead();

    /**
     * returns selected revision of the file. If the file has been
     * checked out, this is the revision that was chosen for checkout;
     * if the file is not in RCS, this method returns an empty string;
     * if file was not checked out, or a head revision was checked
     * out, this method returns the head.
     */
    QString getSelectedRev();

    void    setSelectedRev(const QString &rev) { selectedRev=rev; }

    static QStringList* getEnv();
    static RCSEnvFix*   getRCSEnvFix();

    static const QString & getRcsFileName() { return rcs_file_name; }
    static const QString & getRcsdiffFileName() { return rcsdiff_file_name; }
    static const QString & getRlogFileName() { return rlog_file_name; }
    static const QString & getCiFileName() { return ci_file_name; }
    static const QString & getCoFileName() { return co_file_name; }

 public slots:

     virtual void readFromStdout();
     virtual void readFromStderr();
};


#endif
