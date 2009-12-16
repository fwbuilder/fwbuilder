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
#include "ProjectPanel.h"
#include "Configlet.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qprocess.h>
#include <QRegExp>

#include <fstream>
#include <set>
#include <map>
#include <list>

#include "fwbuilder/FWObjectDatabase.h"

class FirewallInstaller;
class instBatchOptionsDialog;

class QEventLoop;
class QTextEdit;
class QListViewItem;
class QCheckListItem;
class QPushButton;
class QProgressBar;
class QStringList;
class QTreeWidgetItem;
class QTextCharFormat;
//class QCheckTableItem;

namespace libfwbuilder
{
    class Firewall;
    class Cluster;
}

enum BatchOperation {BATCH_INSTALL, BATCH_COMPILE} ;
enum Page1Operation {INST_DLG_COMPILE, INST_DLG_INSTALL};

typedef std::pair<QString,QString> t_procMess; // first - compilation result, second - installation result;

#define FIREWALL_NAME_COLUMN 0
#define COMPILE_CHECKBOX_COLUMN 1
#define INSTALL_CHECKBOX_COLUMN 2
#define LAST_MODIFIED_COLUMN 3
#define LAST_COMPILED_COLUMN 4
#define LAST_INSTALLED_COLUMN 5


class instDialog : public QDialog, public FakeWizard
{

    Q_OBJECT;

    Ui::instDialog_q *m_dialog;
    instConf cnf;
    Page1Operation page_1_op;
    FirewallInstaller *installer;
    ProjectPanel *project;
    instBatchOptionsDialog *batch_inst_opt_dlg;

    // proc is used to launch external oprocess, such as compiler or
    // user-defined installer script
    QProcess       proc;

    QString fwb_prompt;
    
    std::set<libfwbuilder::Firewall*> reqFirewalls;

    std::list<libfwbuilder::Firewall*> firewalls;
    std::list<libfwbuilder::Cluster*> clusters;
    
    std::list<libfwbuilder::Firewall*> compile_fw_list;
    std::list<libfwbuilder::Firewall*>::size_type compile_list_initial_size;
    std::list<libfwbuilder::Firewall*> install_fw_list;
    std::list<libfwbuilder::Firewall*>::size_type install_list_initial_size;
    
    std::map<int,QTreeWidgetItem*> opListMapping;
    std::list<QRegExp> error_re;
    std::list<QRegExp> warning_re;

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
    bool finished;

    QTextBrowser *currentLog;
    QTextCharFormat normal_format;
    QTextCharFormat error_format;
    QTextCharFormat warning_format;
    QTextCharFormat highlight_format;
    
    QPushButton *currentSaveButton;
    QPushButton *currentStopButton;
    QProgressBar *currentProgressBar;
    QProgressBar *currentFirewallsBar;
    QLabel *currentLabel;
    QLabel *currentFWLabel;
    QString currentSearchString;

    
    void fillCompileSelectList();
    void setSelectStateAll(int column, Qt::CheckState);

    void fillCompileOpList();
    void fillCompileUIList();
    void fillInstallOpList();
    void fillInstallUIList();

    bool checkSSHPathConfiguration(libfwbuilder::Firewall*);
    
    //libfwbuilder::Firewall *findFirewallbyTableItem(QTableWidgetItem *item);
    
    void setSuccessState(QTreeWidgetItem *item);
    void setFailureState(QTreeWidgetItem *item);
    void setErrorState(QTreeWidgetItem *item);
    void setInProcessState(QTreeWidgetItem *item);

    bool checkIfNeedToCompile(libfwbuilder::Firewall *fw);
    bool checkIfNeedToInstall(libfwbuilder::Firewall *fw);
    QTreeWidgetItem *createTreeItem(QTreeWidgetItem* parent,
                                    libfwbuilder::Firewall *fw);
    
 public:
   
    instDialog(QWidget* p, BatchOperation op,
               std::set<libfwbuilder::Firewall*> reqFirewalls_);
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
    bool tableHasCheckedItems();
    void clearReqFirewalls();
    void addReqFirewall(libfwbuilder::Firewall *f);
    void replaceMacrosInCommand(Configlet *configlet);
    QString getActivationCmd();
    
    void enableStopButton();
    void disableStopButton();

    void setUpProcessToCompile();
    void setUpProcessToInstall();
    bool executeCommand(const QString &path, QStringList &args);

    bool isFinished() { return finished; }
    
protected:
    
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
    bool verifyManagementAddress();

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

    void readFromStdout();
    void selectAllFirewalls();
    void deselectAllFirewalls();
    
    void nextClicked();
    void backClicked();

    void mainLoopCompile();
    void mainLoopInstall();
    
    void stopCompile();
    void stopInstall();

    void tableItemChanged(QTreeWidgetItem * item, int column);
    void findFirewallInCompileLog(QTreeWidgetItem* item);

    void logItemClicked(QUrl);

signals:
    void activateRule(ProjectPanel*, QString, QString, int);
};


#endif
