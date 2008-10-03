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


#ifndef  __FWWINDOW_H_
#define  __FWWINDOW_H_

#include <ui_FWBMainWindow_q.h>
#include "RCS.h"
#include "ObjectEditor.h"
#include "HttpGet.h"

#include <qstring.h>
#include <QShowEvent>
#include <QHideEvent>

#include <vector>
#include <list>

namespace libfwbuilder {
    class FWObjectDatabase;
    class Firewall;
    class PolicyRule;
    class RuleSet;
    class Rule;
    class FWObject;
};

class ObjectManipulator;
class ObjectTreeView;
class ProjectPanel;

class QMdiArea;
class QMdiSubWindow;
class QPrinter;
class QTabWidget;
class QTextEdit;
class QTimer;
class QTreeWidgetItem;
class RuleSetView;
class findDialog;

class FWWindow : public QMainWindow {

    Q_OBJECT

    QMdiArea *m_space;
    QWidget *instd;
    HttpGet *current_version_http_getter;
    
    QTimer *autosaveTimer;
    QTimer *instDialogOnScreenTimer;
    QString noFirewalls;
    QPrinter *printer;
    libfwbuilder::FWObject *searchObject;
    libfwbuilder::FWObject *replaceObject;
    //int                                    lastFirewallIdx;
    
    void clearFirewallTabs();
    ProjectPanel *newProjectPanel();
    void showSub(ProjectPanel *projectW);
    
public:
    QVector <QString> windowsTitles;
    QVector <QMdiSubWindow*> windowsPainters;
    QStringList openDocFiles;

    ProjectPanel* activeProject();
    void updateWindowTitle ();
    void updateTreeFont ();
    
public slots:
    void selectActiveSubWindow (/*const QString & text*/);

    void minimize ();
    void maximize ();
    void changeActiveSubwindow (  );
    virtual void search();

    virtual void reopenFirewall();
    virtual void redrawRuleSets();
    virtual void changeInfoStyle();
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

    virtual void back();
    virtual void newObject();
    virtual void lockObject();
    virtual void unlockObject();

    virtual void prepareEditMenu();
    virtual void prepareObjectMenu();
    virtual void prepareFileMenu();
    virtual void prepareWindowsMenu ();

    virtual void toolsDiscoveryDruid();
    virtual void closeAuxiliaryPanel();
    virtual void closeEditorPanel();
    virtual void openEditorPanel();

    virtual void killInstDialog();

    virtual void checkForUpgrade(const QString&);

    virtual void projectWindowClosed();
    
 public:
    Ui::FWBMainWindow_q *m_mainWindow;
     
    FWWindow();
    ~FWWindow();


    QMdiArea* getMdiArea () {return m_space;}
    RCS * getRCS(); 

    libfwbuilder::FWObject* getVisibleFirewalls();
    void load(QWidget *dialogs_parent,RCS *rcs);
    void load(QWidget *dialogs_parent);
    void loadLibrary(const std::string &libfpath);
    void loadFile(const QString &filename);
    void save();
    bool checkin(bool unlock);
    int  findFirewallInList(libfwbuilder::FWObject *f);
    void updateTreeViewItemOrder();

    void reloadAllWindowsWithFile(ProjectPanel *pp);
    void closeRuleSetInAllWindowsWhereOpen(libfwbuilder::RuleSet *rs);
    void closeObjectInAllWindowsWhereOpen(libfwbuilder::FWObject *obj);
    
    bool editingLibrary();

    void ensureObjectVisibleInRules(libfwbuilder::FWReference *obj);
    void ensureRuleIsVisible(libfwbuilder::Rule *rule, int col=0);

    QString chooseNewFileName(const QString &fname, const QString &title);
    void setFileName(const QString &fname);
    
    bool saveIfModified();
    
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
    void disableActions(bool havePolicies);
    void setActionsEnabled(bool en);
    void setEnabledAfterRF();

    QString getCurrentFileName();
    
    void info(libfwbuilder::FWObject *o, bool forced = false);

    void setupAutoSave();
    void findObject(libfwbuilder::FWObject *);

    void findWhereUsed(libfwbuilder::FWObject *);
    
    /**
     * panel that wants to open an object in the editor
     * uses this method to request permission to do so and
     * to register itself as an owner of the editor
     */
    bool requestEditorOwnership(QWidget *w,
                                libfwbuilder::FWObject *o,
                                ObjectEditor::OptType   otype,
                                bool validate = true);
    bool exportLibraryTest(std::list<libfwbuilder::FWObject*> &selectedLibs);
    void exportLibraryTo(QString fname,std::list<libfwbuilder::FWObject*> &selectedLibs, bool rof);

    void findExternalRefs(libfwbuilder::FWObject *lib,
                       libfwbuilder::FWObject *root,
                       std::list<libfwbuilder::FWReference*> &extRefs);
    
    void setSafeMode(bool f);
    void setStartupFileName(const QString &fn);

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
    
    //wrapers for some ObjectManipulator functions
    libfwbuilder::FWObject* getOpened();
    
    libfwbuilder::FWObject*  getCurrentLib();

    libfwbuilder::FWObject* createObject(const QString &objType,
                                          const QString &objName,
                                          libfwbuilder::FWObject *copyFrom=NULL);

    libfwbuilder::FWObject* createObject(libfwbuilder::FWObject *parent,
                                          const QString &objType,
                                          const QString &objName,
                                          libfwbuilder::FWObject *copyFrom=NULL);

    void moveObject(libfwbuilder::FWObject *target,
                    libfwbuilder::FWObject *obj);

    void moveObject(const QString &targetLibName,
                    libfwbuilder::FWObject *obj);

    void autorename(libfwbuilder::FWObject *obj,
                    const std::string &objtype,
                    const std::string &namesuffix);

    void updateLibColor(libfwbuilder::FWObject *lib);
    void updateLibName(libfwbuilder::FWObject *lib);

    void updateObjName(libfwbuilder::FWObject *obj,
                       const QString &oldName,
                       bool  askForAutorename=true);
    void updateObjName(libfwbuilder::FWObject *obj,
                       const QString &oldName,
                       const QString &oldLabel,
                       bool  askForAutorename=true);

    void updateLastModifiedTimestampForOneFirewall(libfwbuilder::FWObject *o);
    void updateLastModifiedTimestampForAllFirewalls(libfwbuilder::FWObject *o);   
    void updateLastInstalledTimestamp(libfwbuilder::FWObject *o);
    void updateLastCompiledTimestamp(libfwbuilder::FWObject *o);
       
    void loadDataFromFw(libfwbuilder::Firewall *fw);

    libfwbuilder::FWObject* pasteTo(libfwbuilder::FWObject *target,
                                    libfwbuilder::FWObject *obj);
    void delObj(libfwbuilder::FWObject *obj,bool openobj=true);
    ObjectTreeView* getCurrentObjectTree();
    void openObject(QTreeWidgetItem *otvi);
    void openObject(libfwbuilder::FWObject *obj);
    bool editObject(libfwbuilder::FWObject *obj);
    void findAllFirewalls (std::list<libfwbuilder::Firewall *> &fws);

    libfwbuilder::FWObject* duplicateObject(libfwbuilder::FWObject *target,
                                            libfwbuilder::FWObject *obj,
                                            const QString &name = QString::null,
                                            bool  askForAutorename=true);
    void showDeletedObjects(bool f);
    void select();
    void unselect();
    void info();

    
    void setManipulatorFocus();
    void clearManipulatorFocus();
    
    //wrapers for some Object Editor functions
    bool isEditorVisible();
    bool isEditorModified();
    
    void showEditor();
    void hideEditor();
    void closeEditor();
    
    void openEditor(libfwbuilder::FWObject *o);
    void openOptEditor(libfwbuilder::FWObject *, ObjectEditor::OptType t);
    void blankEditor();
    
    libfwbuilder::FWObject* getOpenedEditor();
    ObjectEditor::OptType getOpenedOptEditor();
    
    void selectObjectInEditor(libfwbuilder::FWObject *o);
    
    void actionChangedEditor(libfwbuilder::FWObject *o);
    bool validateAndSaveEditor();
    //find dialog functions wrapers
    void setFDObject(libfwbuilder::FWObject *o);
    
    QPrinter* getPrinter();
    libfwbuilder::FWObjectDatabase* db();
    QString printHeader();

    static void printFirewallFromFile(QString fileName,
                                      QString firewallName,
                                      QString outputFileName);


    listOfLibraries *getAddOnLibs();

    bool isSystem(libfwbuilder::FWObject *obj);


 protected:

    virtual void showEvent( QShowEvent *ev);
    virtual void hideEvent( QHideEvent *ev);
    virtual void closeEvent( QCloseEvent * );
    
};

#endif


