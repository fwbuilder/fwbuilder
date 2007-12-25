/* 

                          Firewall Builder

                 Copyright (C) 2003 NetCitadel, LLC

  Author:  Vadim Kurland     vadim@fwbuilder.org

  $Id: FWWindow.h,v 1.80 2007/06/13 02:58:48 vkurland Exp $

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


#ifndef  __FWWINDOW_H_
#define  __FWWINDOW_H_

#include <ui_FWBMainWindow_q.h>
#include "RCS.h"
#include "ObjectEditor.h"
#include "FindObjectWidget.h"
#include "FindWhereUsedWidget.h"

#include "fwbuilder/FWObject.h"

#include <qstring.h>
#include <QCloseEvent>
#include <QShowEvent>
#include <QHideEvent>

#include <vector>
#include <list>

namespace libfwbuilder {
    class FWObjectDatabase;
    class Firewall;
    class PolicyRule;
    class RuleSet;
};

class QTabWidget;
class RuleSetView;
class QTimer;
class QPrinter;

class FWWindow : public QMainWindow {

    Q_OBJECT

    RCS                                    *rcs; 
    bool                                    systemFile;
    bool                                    editingStandardLib;
    bool                                    editingTemplateLib;
    bool                                    changingTabs;
    bool                                    safeMode;
    bool                                    ruleSetRedrawPending;

    QString                                 startupFileName;

    std::vector<libfwbuilder::FWObject*>    firewalls;
    std::map<libfwbuilder::FWObject*, RuleSetView*> ruleSetViews;
    int                                     ruleSetTabIndex;

    libfwbuilder::FWObject                 *visibleFirewall;
    libfwbuilder::FWObjectDatabase         *objdb;
    libfwbuilder::FWObject                 *shownInInfo;

    QWidget                                *editorOwner;
    QWidget                                *instd;
    
    QTimer                                 *autosaveTimer;
    QTimer                                 *instDialogOnScreenTimer;
    QString                                 noFirewalls;
    QPrinter                               *printer;
    libfwbuilder::FWObject                 *searchObject;
    libfwbuilder::FWObject                 *replaceObject;
    int                                    lastFirewallIdx;

    void clearFirewallTabs();
    
 public slots:

     virtual void search();

     virtual void openFirewall( int idx );
     virtual void reopenFirewall();
     virtual void redrawRuleSets();
     virtual void deleteFirewall(libfwbuilder::FWObject *fw);
     virtual void changeInfoStyle();
     virtual void ruleSetTabChanged(int tab);
     virtual void restoreRuleSetTab();

     virtual void editFind();
     virtual void editRedo();
     virtual void editUndo();
     virtual void helpContents();
     virtual void helpContentsAction();
     virtual void helpIndex();
     
     virtual void fileNew();
     virtual void fileOpen();
     virtual void fileClose();
     virtual void fileSave();
     virtual void fileSaveAs();
     virtual void fileDiscard();
     virtual void fileCommit();
     virtual void fileImport();
     virtual void fileExport();
     virtual void filePrint();
     virtual void fileExit();
     virtual void fileProp();
     virtual void fileAddToRCS();
     virtual void fileCompare();
     virtual void editCopy();
     virtual void editCut();
     virtual void editDelete();
     virtual void editPaste();
     virtual void editPrefs();
     virtual void importPolicy();

     virtual void startupLoad();

     virtual void helpAbout();
     virtual void debug();
     
     virtual void compile(std::set<libfwbuilder::Firewall * > vf);
     virtual void compile();
     virtual void install(std::set<libfwbuilder::Firewall * > vf);
     virtual void install();

     virtual void insertRule();
     virtual void addRuleAfterCurrent();
     virtual void moveRule();
     virtual void moveRuleUp();
     virtual void moveRuleDown();
     virtual void removeRule(); 

     virtual void copyRule();
     virtual void cutRule();
     virtual void pasteRuleAbove();
     virtual void pasteRuleBelow();

     virtual void newObject();
     virtual void lockObject();
     virtual void unlockObject();
     virtual void prepareObjectMenu();
     virtual void toolsDiscoveryDruid();
     virtual void closeAuxiliaryPanel();
     virtual void closeEditorPanel();
     virtual void openEditorPanel();

     virtual void rollBackSelectionSameWidget();
     virtual void rollBackSelectionDifferentWidget();

     virtual void killInstDialog();

 signals:
     void restoreSelection_sign(bool same_widget);

 public:
     Ui::FWBMainWindow_q *m_mainWindow;
     
    FindObjectWidget *findObjectWidget;
    FindWhereUsedWidget *findWhereUsedWidget;  
    
    FWWindow();
    ~FWWindow();

    virtual void closeEvent( QCloseEvent * );
    RCS * getRCS(); 
        
    void load(QWidget *dialogs_parent,RCS *rcs);
    void load(QWidget *dialogs_parent);
    void loadLibrary(const std::string &libfpath);
    void save();
    bool checkin(bool unlock);
    void showFirewalls(bool open_first_firewall=true);
    void showFirewall(libfwbuilder::FWObject *f);
    void addFirewallToList(libfwbuilder::FWObject *f);
    void removeFirewallFromList(libfwbuilder::FWObject *f);
    int  findFirewallInList(libfwbuilder::FWObject *f);
    void updateTreeViewItemOrder();

    bool editingLibrary();

    void ensureObjectVisibleInRules(libfwbuilder::FWReference *obj);

    QString chooseNewFileName(const QString &fname,
                              bool checkPresence, const QString &title);
    void setFileName(const QString &fname);
    
    bool saveIfModified();

    void showFirewallRuleSets( libfwbuilder::FWObject *fw );
    
    void updateFirewallName(libfwbuilder::FWObject *obj,const QString &oldName);
    void updateRuleSetView();
    void updateRuleOptions();
    void updateRuleSetViewSelection();
    
    /**
     * unselects whatever is selected in policy
     */
    void unselectRules();

    /**
     * selects whatever is current in rules
     */
    void selectRules();

    libfwbuilder::FWObjectDatabase* db() { return objdb; }

    libfwbuilder::FWObject* getVisibleFirewall() { return visibleFirewall; }
    QString getCurrentFileName();
    
    void info(libfwbuilder::FWObject *o, bool forced = false);

    void setupAutoSave();
    void findObject(libfwbuilder::FWObject *);
    void findWhereUsed(libfwbuilder::FWObject *);
    RuleSetView* getRuleSetViews(libfwbuilder::FWObject *o) 
    {return ruleSetViews[o];};

    void addPolicyBranchTab(libfwbuilder::RuleSet *subset);
    void removePolicyBranchTab(libfwbuilder::RuleSet *subset);
    void setPolicyBranchTabName(libfwbuilder::RuleSet *subset);

    /**
     * panel that wants to open an object in the editor
     * uses this method to request permission to do so and
     * to register itself as an owner of the editor
     */
    bool requestEditorOwnership(QWidget *w,
                                libfwbuilder::FWObject *o,
                                ObjectEditor::OptType   otype,
                                bool validate = true);
    void releaseEditor();
    void connectEditor(QWidget *w);
    bool exportLibraryTest(std::list<libfwbuilder::FWObject*> &selectedLibs);
    void exportLibraryTo(QString fname,std::list<libfwbuilder::FWObject*> &selectedLibs, bool rof);

    void findExternalRefs(libfwbuilder::FWObject *lib,
                       libfwbuilder::FWObject *root,
                       std::list<libfwbuilder::FWReference*> &extRefs);
    
    void setSafeMode(bool f) { safeMode=f; }
    void setStartupFileName(const QString &fn) { startupFileName = fn; }

    void scheduleRuleSetRedraw();

    // semi-intelligent way to guess most appropriate
    // destination directory for various file save or file open
    // operations. If working directory is configured in
    // preferences, then getDestDir returns that. If it is not
    // configured and file name is given on the command line,
    // directory where that file is located is returned. If
    // parameter filename is empty, then current directory
    // is returned (however on windows and mac userDataDir is returned)

    QString getDestDir(const QString &filename);

 protected:

    virtual void showEvent( QShowEvent *ev);
    virtual void hideEvent( QHideEvent *ev);
    
};

#endif


