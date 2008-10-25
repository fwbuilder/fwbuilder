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
#include "FirewallInstaller.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qprocess.h>

#include <fstream>
#include <set>
#include <map>
#include <list>

class FirewallInstaller;

class QEventLoop;
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

enum BatchOperation {BATCH_INSTALL, BATCH_COMPILE} ;
enum Page1Operation {COMPILE, INSTALL};

typedef std::map<libfwbuilder::Firewall *,QTableWidgetItem *> t_tableMap;
typedef std::list<libfwbuilder::Firewall *> t_fwList;
typedef std::pair<QString,QString> t_procMess; // first - compilation result, second - installation result;
typedef std::set<libfwbuilder::Firewall*> t_fwSet;

class instDialog : public QDialog, public FakeWizard
{

    Q_OBJECT

    Ui::instDialog_q *m_dialog;
    instConf cnf;
    Page1Operation page_1_op;
    FirewallInstaller *installer;
        
    // proc is used to launch external oprocess, such as compiler or
    // user-defined installer script
    QProcess       proc;

    QString fwb_prompt;
    
    t_fwSet reqFirewalls;

    t_fwList firewalls;
    
    std::list<libfwbuilder::Firewall*> compile_fw_list;
    std::list<libfwbuilder::Firewall*>::size_type compile_list_initial_size;
    std::list<libfwbuilder::Firewall*> install_fw_list;
    std::list<libfwbuilder::Firewall*>::size_type install_list_initial_size;
    
    std::map<int,QTreeWidgetItem*> opListMapping;
    
    t_tableMap compileMapping;
    t_tableMap installMapping;
    
    QString path; //path of the program to execute
//    QStringList args; //arguments for that program
        
    bool creatingTable;
    
    BatchOperation operation;

    int processedRules;
    int lastPage;
    bool stopProcessFlag;
    bool rejectDialogFlag;
    bool compileFlag;
    bool customScriptFlag;
    bool showSelectedFlag;

    QTextEdit *currentLog;
    QPushButton *currentSaveButton;
    QPushButton *currentStopButton;
    QProgressBar *currentProgressBar;
    QProgressBar *currentFirewallsBar;
    QLabel *currentLabel;
    QLabel *currentFWLabel;
    QString currentSearchString;

    
    void fillCompileSelectList();
    void selectAll(t_tableMap &mapping);
    void deselectAll(t_tableMap &mappin);

    void fillCompileOpList();
    void fillCompileUIList();
    void fillInstallOpList();
    void fillInstallUIList();

    bool testFirewall(libfwbuilder::Firewall*);
    
    //void analyseInstallQueue(bool &fPix, bool &fCustInst);
    libfwbuilder::Firewall *findFirewallbyListItem(QTreeWidgetItem* item);
    libfwbuilder::Firewall *findFirewallbyTableItem(QTableWidgetItem *item);
    
    void setSuccessState(QTreeWidgetItem *item);
    void setFailureState(QTreeWidgetItem *item);
    void setErrorState(QTreeWidgetItem *item);
    void setInProcessState(QTreeWidgetItem *item);

 public:
   
    instDialog(QWidget* p, BatchOperation op, t_fwSet reqFirewalls_);
    virtual ~instDialog();
    
    void summary();
    void opSuccess(libfwbuilder::Firewall *fw);
    void opError(libfwbuilder::Firewall *fw);
    void opCancelled(libfwbuilder::Firewall *fw);

    QWidget* page(int n) { return m_dialog->stackedWidget->widget(n); }
    
    void displayCommand(const QStringList &args);
    bool runCompiler(libfwbuilder::Firewall *fw);
    bool runInstaller(libfwbuilder::Firewall *fw);

    QStringList prepareArgForCompiler(libfwbuilder::Firewall *fw);
    bool tableHasChecked();
    void clearReqFirewalls();
    void addReqFirewall(libfwbuilder::Firewall *f);
    QString replaceMacrosInCommand(const QString &cmd);
    QString getActivationCmd();
    
    void enableStopButton();
    void disableStopButton();

    
protected:
    void executeCommand(QStringList &args);
    
    virtual void showEvent( QShowEvent *ev);
    virtual void hideEvent( QHideEvent *ev);

    bool getInstOptions(libfwbuilder::Firewall *fw);
    bool getBatchInstOptions();
    
    void prepareInstConf(libfwbuilder::Firewall *fw);

    void blockInstallForFirewall(libfwbuilder::Firewall *fw);

    void readInstallerOptionsFromSettings();
    void readInstallerOptionsFromFirewallObject(libfwbuilder::Firewall *fw);
    void readInstallerOptionsFromDialog(libfwbuilder::Firewall *fw,
                                                instOptionsDialog *dlg);
    void completeInstallerOptions();

    void storeInstallerOptions();
    void findFirewalls();

    bool isCiscoFamily();

    void interpretLogLine(const QString &buf);
    
public slots:

    void compilerFinished(int ret_code, QProcess::ExitStatus);
    void installerFinished(int ret_code, QProcess::ExitStatus);
    void installerSuccess();
    void installerError();

    void showPage(const int page);
     
    void finishClicked();
    void cancelClicked();
     
    void testRunRequested();
     
    void addToLog(const QString &buf); 
    void updateProgressBar(int n,bool setsize);

    void saveLog();
    void togleDetailMC();

    void readFromStdout();
    //virtual void readFromStderr();
    void selectAllFirewalls();
    void deselectAllFirewalls();
    
    void nextClicked();
    void backClicked();

    void mainLoopCompile();
    void mainLoopInstall();
    
    void stopCompile();
    void stopInstall();
    void findFirewallInCompileLog(QTreeWidgetItem*);
    void showSelected();
    void tableValueChanged(int row, int col);
    
};


#endif
