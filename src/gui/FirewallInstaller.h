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


#ifndef __FIREWALLINSTALLER_H_
#define __FIREWALLINSTALLER_H_


#include "../../config.h"

#include "instConf.h"
#include "instDialog.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qprocess.h>
#include <qobject.h>

#include <fstream>
#include <set>
#include <map>
#include <list>

class SSHSession;
class instDialog;

namespace libfwbuilder
{
    class Firewall;
}

enum instJobType {COPY_FILE, EXECUTE_COMMAND, ACTIVATE_POLICY};

class instJob
{
public:
    instJobType job;
    QString argument;

    instJob(instJobType jt, const QString &a) { job=jt; argument=a; }
};

class FirewallInstaller : public QObject
{
    Q_OBJECT


protected:

    instDialog *inst_dlg;
    instConf *cnf;
    // session is used when e run built-in installer
    SSHSession  *session;
    std::list<instJob> job_list;
    QString fwb_prompt;

    void runSSHSession(SSHSession *s, bool intermediate=false);
    QString getFullPath(const QString &file );

    
public:

    FirewallInstaller(instDialog *_dlg, instConf *_cnf, const QString &prompt)
    {
        inst_dlg = _dlg;
        cnf = _cnf;
        fwb_prompt = prompt;
    }

    void packSSHArgs(QStringList &args);
    void packSCPArgs(const QString &file_name, QStringList &args);
    QString getActivationCmd();
    QString getDestinationDir();

    virtual bool packInstallJobsList(libfwbuilder::Firewall*);
    virtual void copyFile(const QString &file_name);
    virtual void executeCommand(const QString &cmd);
    virtual void activatePolicy();

    static QString getGeneratedFileFullPath(libfwbuilder::Firewall *fw);

    
public slots:
    void runJobs();
    
};


#endif
