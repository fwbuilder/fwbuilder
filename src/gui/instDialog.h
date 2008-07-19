/* 

                          Firewall Builder

                 Copyright (C) 2004 NetCitadel, LLC

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


#ifndef __INSTDIALOG_H_
#define __INSTDIALOG_H_


#include "../../config.h"

#include <ui_instdialog_q.h>
#include "instConf.h"
#include "fakeWizard.h"

#include "instOptionsDialog.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qprocess.h>

#include <fstream>
#include <set>
#include <map>
#include <list>

class QEventLoop;
class SSHSession;
class QTextEdit;
class QListViewItem;
class QCheckListItem;
class QPushButton;
class QProgressBar;
class QStringList;
//class QCheckTableItem;

namespace libfwbuilder
{
    class Firewall;
}

enum BatchOperation {BATCH_INSTALL,BATCH_COMPILE} ;

typedef std::map<libfwbuilder::Firewall *,QTreeWidgetItem *> t_listMap;
typedef std::map<libfwbuilder::Firewall *,QTableWidgetItem *> t_tableMap;
typedef std::list<libfwbuilder::Firewall *> t_fwList;
typedef std::pair<QString,QString> t_procMess; // first - compilation result, second - installation result;
typedef std::set<libfwbuilder::Firewall*> t_fwSet;


class instDialog : public QDialog, public FakeWizard
{

    Q_OBJECT

    Ui::instDialog_q *m_dialog;
    bool        ready;
    bool        activationCommandDone;
    instConf    cnf;

    QString     ssh;
    
    QString     confScript;
    QStringList confFiles;
    
    QString     fwb_prompt;
    
    QString     newKeyMsg;
    
    int         phase;

    QString replaceMacrosInCommand(const QString &cmd);
    QString getActivationCmd();
    //libfwbuilder::Firewall * firewall;
    t_fwSet reqFirewalls;


    // session is used when e run built-in installer
    SSHSession  *session;

    // proc is used to launch external oprocess, such as compiler or
    // user-defined installer script
    QProcess       proc;

    std::map<libfwbuilder::Firewall *, t_procMess>    processedFirewalls;

    t_fwList      firewalls;
    t_fwList      opList;

    t_fwList::iterator   opListIterator;

    t_listMap            opListMapping;
    t_tableMap           compileMapping;
    t_tableMap           installMapping;
    
    QString              path; //path of the program to execute
    QStringList          args; //arguments for that program
        
    QTextEdit           *currentLog;
    QPushButton             *currentSaveButton;
    QPushButton             *currentStopButton;
    QProgressBar        *currentProgressBar;
    QProgressBar        *currentFirewallsBar;
    QLabel              *currentLabel;
    QLabel              *currentFWLabel;
    QString              currentSearchString;
    bool                 creatingTable;
    
    BatchOperation       operation;
    instOptionsDialog   *dlg;
    QString              pendingLogLine;

    int progress;
    int totalRules;
    int processedRules;
    int lastPage;
    bool stopProcessFlag;
    bool rejectDialogFlag;
    bool compileFlag;
    bool customScriptFlag;
    bool showSelectedFlag;

    void fillCompileSelectList();
    void selectAll(t_tableMap &mapping);
    void deselectAll(t_tableMap &mappin);
    void fillCompileOpList();
    void fillLastList();
    bool doInstallPage(libfwbuilder::Firewall*);
    void resetInstallSSHSession();    
    bool testFirewall(libfwbuilder::Firewall*);
    void finishInstall(bool success=true);
    void fillInstallOpList();
    void installNext();
    void initInstall();
    void analyseInstallQueue(bool &fPix, bool &fCustInst);
    libfwbuilder::Firewall *findFirewallbyListItem(QTreeWidgetItem* item);
    libfwbuilder::Firewall *findFirewallbyTableItem(QTableWidgetItem *item);
    
    
 public:
    instDialog(QWidget* p, BatchOperation op, t_fwSet reqFirewalls_);
    virtual ~instDialog();
    
    void setReady(bool f) { ready=f; }

    void summary();


    QWidget* page(int n) { return m_dialog->stackedWidget->widget(n); }
    
    void initiateCopy(const QString &file);
    void runSSH(SSHSession *s);
    void displayCommand(const QStringList &args);
    bool runCompile(libfwbuilder::Firewall *fw);
    bool runInstall(libfwbuilder::Firewall *fw);
    bool prepareArgForCompiler(libfwbuilder::Firewall *fw);
    bool isTableHasChecked();
    void clearReqFirewalls();
    void addReqFirewall(libfwbuilder::Firewall *f);
    void interpretLogLine(const QString &buf);
    
protected:

    virtual void showEvent( QShowEvent *ev);
    virtual void hideEvent( QHideEvent *ev);
    virtual void prepareInstallerOptions();
    
    virtual void prepareInstConf(libfwbuilder::Firewall *fw);
    virtual void storeInstallerOptions();
    virtual void findFirewalls();

    QString getFullPath(instConf &cnf, const QString &file );

 protected slots:
    void processExited(int code);
    void installerSuccess();
    void installerError();
    void installSelected();
    void showPage(const int page);
     
    void finishClicked();
    void cancelClicked();
     
    void testRunRequested();
     
    void addToLog(const QString &buf); 
    void updateProgressBar(int n,bool setsize);

    virtual void saveLog();
    virtual void togleDetailMC();

    virtual void readFromStdout();
    //virtual void readFromStderr();
    virtual void selectAllFirewalls();
    virtual void deselectAllFirewalls();
    
    virtual void nextClicked();
    virtual void backClicked();

    void stopSessionAndDisconnectSignals();
    void continueRun();
    void restartSession();
    void sessionCleanupOnError();
    
    void compileSelected();
    void stopCompile();
    void stopInstall();
    void findFirewallInCompileLog(QTreeWidgetItem*);
    void showSelected();
    void tableValueChanged(int row, int col);
    
    
};


#endif
