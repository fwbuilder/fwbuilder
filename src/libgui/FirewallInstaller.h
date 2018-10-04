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



#include "instConf.h"
#include "instDialog.h"
#include "Configlet.h"

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

enum instJobType {COPY_FILE, EXECUTE_COMMAND, ACTIVATE_POLICY, RUN_EXTERNAL_SCRIPT};

class instJob
{
public:
    instJobType job;
    QString argument1;
    QString argument2;

    instJob(instJobType jt, const QString &a1, const QString &a2)
    { job = jt; argument1 = a1; argument2 = a2; }
};

class FirewallInstaller : public QObject
{
    Q_OBJECT

protected:

    instDialog *inst_dlg;
    instConf *cnf;
    QProcess  proc;
    // session is used when we run built-in installer
    SSHSession  *session;
    std::list<instJob> job_list;
    QString fwb_prompt;

    void runSSHSession(SSHSession *s, bool intermediate=false);
    QString getFullPath(const QString &file );

    bool parseManifestLine(const QString &line,
                           QString *local_file_name,
                           QString *remote_file_name,
                           bool *main_script);

    void executeExternalInstallScript(const QString &script,
                                      const QString &script_args);

public:

    FirewallInstaller(instDialog *_dlg, instConf *_cnf, const QString &prompt)
    {
        inst_dlg = _dlg;
        cnf = _cnf;
        fwb_prompt = prompt;
        session = nullptr;
    }

    void packSSHArgs(QStringList &args);
    void packSCPArgs(const QString &local_name, const QString &remote_name, QStringList &args);
    QString getActivationCmd();
    void replaceMacrosInCommand(Configlet *configlet);
    virtual QString getDestinationDir(const QString &dir);

    void terminate();
    
    virtual bool packInstallJobsList(libfwbuilder::Firewall*);
    virtual void copyFile(const QString &local_name, const QString &remote_name);
    virtual void executeCommand(const QString &cmd);
    virtual void activatePolicy(const QString &script, const QString &args);

    static QString getGeneratedFileFullPath(libfwbuilder::Firewall *fw);
    static QString getGeneratedFileName(libfwbuilder::Firewall *fw);
    virtual bool readManifest(const QString &conffie,
                              QMap<QString, QString> *all_files);

    
public slots:
    void runJobs();
    
};


#endif
