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


#ifndef  __SSHSESSION_H_
#define  __SSHSESSION_H_

#include "global.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qprocess.h>
#include <qobject.h>

#include <fstream>
#include <string>
#include <list>

class QTimer;
class instConf;
class QWidget;

class SSHSession : public QObject {

    Q_OBJECT

 protected:

    QWidget    *parent;
    QProcess   *proc;
    int         retcode;
    QTimer     *heartBeatTimer;
    QString     stdoutBuffer;
    QString     stderrBuffer;
    QString     ssh;
    QString     cmd;
    QStringList args;

    std::list<std::string> input;

    bool       closeStdin;
    bool       logged_in;
    bool       enable;
    bool       configure;
    bool       endOfCopy;
    bool       send_keepalive;
    bool       session_completed;
    
    enum State { NONE,
                 LOGGEDIN,
                 WAITING_FOR_ENABLE,
                 PRE_CONFIG_COMMANDS,
                 SCHEDULE_RELOAD_DIALOG,
                 ENABLE,
                 CONFIG,
                 COMMAND_SENT,
                 WAITING_FOR_SHOW_RUN,
                 WAITING_FOR_CONFIG_PROMPT,
                 CLEAR_CONFIG,
                 PUSHING_CONFIG,
                 GET_ACLS,
                 CLEAR_ACLS,
                 GET_OG,
                 CLEAR_OG,
                 EXIT_FROM_CONFIG,
                 SAVE_CONFIG,
                 SAVE_STANDBY,
                 RUN_SCRIPT,
                 EXIT,
                 FINISH,
                 EXECUTING_COMMAND,
                 COMMAND_DONE
    };

    enum State state;
    int        phase;
    bool       verbose;
    bool       quiet;

    bool        error;
    
    bool        backup;
    bool        incremental;
    bool        dry_run;
    bool        saveStandby;
    bool        stripComments;
    QString     wdir;
    QString     script;
    QString     backupFile;
    QString     diff_pgm;
    bool        save_diff;
    QString     diff_file;
    
    QString     normal_prompt;
    QString     fwb_prompt;
    QString     enable_prompt;
    QString     config_prompt;
    QString     pwd_prompt_1;
    QString     pwd_prompt_2;
    QString     putty_pwd_prompt;
    QString     ssh_pwd_prompt;
    QString     thinkfinger_pwd_prompt;
    QString     ssoft_prompt1;
    QString     ssoft_prompt2;
    QString     ssoft_config_prompt;
    QString     sudo_pwd_prompt_1;
    QString     sudo_pwd_prompt_2;
    QString     passphrase_prompt;
    QString     epwd_prompt;
    
    QStringList errorsInit;
    QStringList errorsLoggedin;
    QStringList errorsEnabledState;

    QString     pendingLogLine;

    QString pwd;
    QString epwd;
    QString host;

    static const char* newKeyOpenSSH;
    static const char* newKeyPlink;
    static const char* newKeyVsh;
    static const char* newKeySSHComm;
    static const char* fingerprintPrompt1;
    static const char* fingerprintPrompt2;
    
    QString newKeyMsg;

    
    bool cmpPrompt(const QString &str,const QString &prompt);
    bool cmpPrompt(const QString &str,const QRegExp &prompt);

    void startHeartBeat();
    void stopHeartBeat();

protected:

    virtual void sendCommand(const QString &cmd);
    void cleanUp();
    
public:
    
    SSHSession(QWidget *parent,
               const QString &host,
	       const QStringList &args,
	       const QString &pwd,
	       const QString &epwd,
	       const std::list<std::string> &in);
    virtual ~SSHSession();

    virtual bool checkForErrors();
    virtual void stateMachine();

    void startSession();
    void terminate();

    void setOptions(instConf *cnf);

    void setCloseStdin(bool f) { closeStdin=f; }
    
    void setFWBPrompt(const QString &p) { fwb_prompt=p; }
    void setQuiet(bool f) { quiet=f; }
    void setVerbose(bool f) { verbose=f; }
    void setBackup(bool f) { backup=f; }
    void setIncr(bool f) { incremental=f; }
    void setDryRun(bool f) { dry_run=f; }
    void setSaveStandby(bool f) { saveStandby=f; }
    void setStripComments(bool f) { stripComments=f; }
    void setWDir(const QString &wd) { wdir=wd; }
    void setScript(const QString &cf) { script=cf; }
    void setBackupFile(const QString &cf) { backupFile=cf; }
    void setSaveDiff(bool f) { save_diff=f; }
    void setDiffPgm(const QString &v) { diff_pgm=v; }
    void setDiffFile(const QString &v) { diff_file=v; }
    bool getErrorStatus() { return error; }

    void sessionComplete(bool err);

    QString findKeyFingerprint(QString &buffer);

public slots:
    virtual void readFromStdout();
    virtual void readFromStderr();
    virtual void finished( int code );
    void readyToSend();
    void sendLine();
    void allDataSent();
    void heartBeat();
     
 signals:

 void printStdout_sign(const QString &line);
 void sessionFinished_sign();
 void sessionFatalError_sign();
 void updateProgressBar_sign(int n,bool setsize);


};

#endif
