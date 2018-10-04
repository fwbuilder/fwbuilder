/* 

                          Firewall Builder

                 Copyright (C) 2008 NetCitadel, LLC

  Author:  alek@codeminders.com


                 Copyright (C) 2013 UNINETT AS

  Author:  Sirius Bakke <sirius.bakke@uninett.no>

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

#ifndef PROJECTPANEL_H
#define PROJECTPANEL_H

#include "ui_projectpanel_q.h"

#include <set>
#include <map>



namespace libfwbuilder {
    class FWObjectDatabase;
    class Firewall;
    class PolicyRule;
    class RuleSet;
    class Rule;
    class RuleElement;
    class FWObject;
    class FWReference;
};

class QWidget;
class QMdiSubWindow;
class QTextEdit;
class QUndoStack;

class ObjectTreeView;
class ObjectManipulator;
class findDialog;
class FWWindow;
class RuleSetView;
class RCS;

class FWBTree;

#define DEFAULT_H_SPLITTER_POSITION 250
#define DEFAULT_V_SPLITTER_POSITION 450

namespace DiffType {
    enum Type {
        None,
        Add,
        Remove,
        Edit,
        Move
    };
}

class ProjectPanel: public QWidget {
    Q_OBJECT;

    FWWindow *mainW;
    RCS *rcs;
    bool systemFile;
    bool safeMode;
    bool editingStandardLib;
    bool editingTemplateLib;
    bool ruleSetRedrawPending;
    bool ready;
    
    libfwbuilder::FWObjectDatabase *objdb, *origObjdb;
    
    findDialog *fd;
        
    QTimer *autosaveTimer;
    std::map<libfwbuilder::FWObject*, RuleSetView*> ruleSetViews;
    int ruleSetTabIndex;
    
    libfwbuilder::FWObject *visibleFirewall;
    libfwbuilder::RuleSet *visibleRuleSet ;
    std::vector<libfwbuilder::FWObject*> firewalls;

    int lastFirewallIdx;
    bool changingTabs;
    QString noFirewalls;
    bool loading_state;

    // set of object IDs for objects that have been modified recently.
    // Method updateLastModifiedTimestampForAllFirewalls() uses this
    // to update lastModified timestamp of all firewalls using these objects.
    std::set<int> lastModifiedTimestampChangePool;

    // IDs of objects that need to be updated in the tree. Method
    // updateObjectInTree() processes them. The key in the map is object ID,
    // value is a boolean flag, true means need to update the object and 
    // subtree under it.
    std::map<int, bool> updateObjectsInTreePool;

    bool treeReloadPending;
    QHash<int, int> m_diffLog;
    QHash<QString, QString> m_renamedGroups;
    QHash<QPair<int, DiffType::Type>, int> m_statistics;

public:

    QMdiSubWindow *mdiWindow;
    Ui::ProjectPanel_q *m_panel;
    QSet<QString> copySet;

    QUndoStack *undoStack;
    
    void readyStatus(bool f) { ready=f; }

    libfwbuilder::RuleSet* getCurrentRuleSet () {return visibleRuleSet;};
    RuleSetView* getCurrentRuleSetView() ;

    void openRuleSet(libfwbuilder::FWObject *obj, bool immediately=false);
    void closeRuleSet(libfwbuilder::FWObject *obj)
    { if ((libfwbuilder::FWObject*)(visibleRuleSet)==obj) visibleRuleSet=nullptr;};
    
    ProjectPanel(QWidget *parent);
    ~ProjectPanel();


    void initMain(FWWindow *main);
    void reset();
    
    void loadObjects();
    void loadObjects(libfwbuilder::FWObjectDatabase *db);
    void clearObjects();
    libfwbuilder::FWObjectDatabase* db() { return objdb; };
    libfwbuilder::FWObjectDatabase* origDb() { return origObjdb; }
    bool hasObject(libfwbuilder::FWObject* obj)
    { return objdb->findInIndex(obj->getId()); };


    void setDiffType(int id, enum DiffType::Type type) { m_diffLog.insert(id, type); }
    int getDiffType(int id) { return m_diffLog.value(id, DiffType::None); }
    const QHash<int, int>& getDiffLog() { return m_diffLog; }
    QHash<QString, QString>& getRenamedGroups() { return m_renamedGroups; }
    QHash<QPair<int, DiffType::Type>, int>& getStatistics() { return m_statistics; }
    int getStatistics(int ruleSetId, DiffType::Type diffType)
    { return m_statistics.value(QPair<int, DiffType::Type>(ruleSetId, diffType), 0); }

    const QString getTemporaryDirPath() const;

    // libfwbuilder::RuleElement* getRE(libfwbuilder::Rule* r, int col );

    //wrapers for some ObjectManipulator functions
    
    libfwbuilder::FWObject*  getCurrentLib();

    libfwbuilder::FWObject* createObject(const QString &objType,
                                         const QString &objName,
                                         libfwbuilder::FWObject *copyFrom=nullptr);

    libfwbuilder::FWObject* createObject(libfwbuilder::FWObject *parent,
                                         const QString &objType,
                                         const QString &objName,
                                         libfwbuilder::FWObject *copyFrom=nullptr);

    void updateObjectInTree(libfwbuilder::FWObject *obj, bool subtree=false);

    FWWindow* getWindow (){ return mainW;}     
    void moveObject(libfwbuilder::FWObject *target,
                    libfwbuilder::FWObject *obj);

    void moveObject(const QString &targetLibName,
                    libfwbuilder::FWObject *obj);

    void registerModifiedObject(libfwbuilder::FWObject *o);
    void registerObjectToUpdateInTree(libfwbuilder::FWObject *o, bool update_subtree);
    void registerTreeReloadRequest();
    void registerRuleSetRedrawRequest();

    libfwbuilder::FWObject* pasteTo(libfwbuilder::FWObject *target,
                                    libfwbuilder::FWObject *obj);

    ObjectTreeView* getCurrentObjectTree();

    void findAllFirewalls(std::list<libfwbuilder::Firewall *> &fws);

    void showDeletedObjects(bool f);
    void select();
    void unselect();

    void copyObj();
    bool isManipulatorSelected();
    void cutObj();
    void pasteObj();
    
    void deleteObj();
    libfwbuilder::FWObject* getSelectedObject();
    void reopenCurrentItemParent();
    
    void setManipulatorFocus();
    void clearManipulatorFocus();
    
    //find dialog functions wrapers
    void setFDObject(libfwbuilder::FWObject *o);
    
    void resetFD();
    
    void clearFirewallTabs();
    void closeRuleSetPanel();
    void ensureObjectVisibleInRules(libfwbuilder::FWReference *obj);
    
    libfwbuilder::FWObject* getVisibleFirewall() { return visibleFirewall; }
    RuleSetView* getRuleSetViews(libfwbuilder::FWObject *o) 
    {return ruleSetViews[o];};

    int  findFirewallInList(libfwbuilder::FWObject *f);
    void updateFirewallName();
    void selectRules();
    void unselectRules();
    void editCopy();
    void editCut();
    void editDelete();
    void editPaste();
    
    bool saveIfModified(bool include_discard_button=true);

    QString chooseNewFileName(const QString &fname, const QString &title);
    void setFileName(const QString &fname);
    void restoreDepends(libfwbuilder::FWObject *obj_old,
                        libfwbuilder::FWObject *nobj, 
                        const std::map<int,
                        libfwbuilder::FWObject *> &objByIds);

    QString getPageTitle(bool file_path=false);

    void setActive();

 private:

 public slots:
    void newObject();
    
    virtual void lockObject();
    virtual void unlockObject();
 
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

    virtual void reopenFirewall();
    virtual void redrawRuleSets();
    virtual void restoreRuleSetTab();

    virtual void fileProp();
    virtual bool fileNew();

    virtual void fileClose();
    virtual void fileSave();
    virtual void fileSaveAs();
    virtual void fileCommit();
    virtual void fileDiscard();
    virtual void fileAddToRCS();
    
    virtual void fileImport();
    virtual void fileCompare();
    virtual void fileExport();
    
    virtual void compile(std::set<libfwbuilder::Firewall*> vf);
    virtual void compile();
    virtual void install(std::set<libfwbuilder::Firewall*> vf);
    virtual void install();
    virtual void inspect(std::set<libfwbuilder::Firewall*> vf);

    void splitterMoved ( int pos, int index );

    virtual void autoSave();
    virtual void compileThis();
    virtual void installThis();
    virtual void inspectThis();
    virtual void inspectAll();
    virtual void addRule();
    virtual void diffThis();

    void updateLastModifiedTimestampForAllFirewalls();
    void updateObjectInTree();
    void reloadTree();

    void aboutToActivate();
    
    void splitterPositionChanged(int, int);

public:
    QString getFileName();
    bool editingLibrary();
    void createRCS( const QString &filename);
    bool loadFromRCS(RCS *rcs);
    void loadStandardObjects();
    bool loadFile(const QString &fileName, bool load_rcs_head);

    void save();

    void saveState();
    void loadState(bool open_objects=true);

    void saveMainSplitter();
    void loadMainSplitter();
    
    void loadOpenedRuleSet();
    void saveOpenedRuleSet();

    void saveLastOpenedLib();
    void loadLastOpenedLib();
    void loadFirstNonStandardLib();
    
    bool checkin(bool unlock);
    libfwbuilder::FWObject* loadLibrary(const std::string &libfpath);
    
    bool exportLibraryTest(std::list<libfwbuilder::FWObject*> &selectedLibs);
    void exportLibraryTo(QString fname,
                         std::list<libfwbuilder::FWObject*> &selectedLibs,
                         bool rof);

    void findExternalRefs(libfwbuilder::FWObject *lib,
                       libfwbuilder::FWObject *root,
                       std::list<libfwbuilder::FWReference*> &extRefs);
    
    void setSafeMode(bool f) { safeMode=f; }
    void setupAutoSave();
    RCS * getRCS();
    QString printHeader();

    void toggleViewTree(bool f);
    
protected:
    int oldState ;
    virtual void showEvent(QShowEvent *ev);
    virtual void hideEvent(QHideEvent *ev);
    virtual void closeEvent(QCloseEvent *ev);
    virtual void resizeEvent(QResizeEvent *ev);
    virtual bool event(QEvent *ev);

    void setMainSplitterPosition(int w1, int w2);
    void collapseTree();
    void collapseRules();
};

#endif
