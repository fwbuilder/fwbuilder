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


#ifndef  __OBJECTMANIPULATOR_H_
#define  __OBJECTMANIPULATOR_H_


#include "global.h"
#include "utils.h"

#include <QDialog>
#include <QTreeWidget>
#include <QToolTip>
#include <QMenu>

#include <ui_objectmanipulator_q.h>

#include "fwbuilder/FWObject.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/ObjectGroup.h"

#include "UsageResolver.h"

#include <set>

class ObjectTreeView;
class ObjectTreeViewItem;
class QComboBox;
class QPixmap;
class QUndoCommand;

class ListOfLibrariesModel;
class ProjectPanel;


namespace libfwbuilder
{
    class Firewall;
    class Cluster;
    class Library;
}


class HistoryItem
{
    ObjectTreeViewItem *itm;
    int                 objId;

 public:
    HistoryItem(ObjectTreeViewItem *oi, int id) { itm=oi; objId=id; }
    ~HistoryItem();
    ObjectTreeViewItem* item() { return itm;   }
    int id() const { return objId; }
};


class FindHistoryItemByObjectId
{
    int id;
public:
    FindHistoryItemByObjectId(int i) { id = i; }
    bool operator()(const HistoryItem &itm);
};


class ObjectManipulator : public QWidget
{
    Q_OBJECT;

    ListOfLibrariesModel *libs_model;
//    std::vector<libfwbuilder::FWObject*> idxToLibs;
//    std::vector<QTreeWidget*> idxToTrees;

    int previous_lib_index;
    QSet<int> ids ;

    /*
     * this is where we keep information about user's object browsing
     * history. Even though latest versions of the GUI do not offer
     * buttons "Back" and "Forward", history is still used to
     * determine which rule set object to open when user double clicks
     * on a firewall. We want to open the rule set they looked at
     * last. This whole browsing history infrastructure might be an
     * overkill for the simple purpose like that, but I'll keep it
     * around beause I do not want to make drastic changes so close to
     * the release date and it may become useful again in the
     * future. Otherwise, we can get rid of it later. --vk 07/22/2011
     */
    std::list<HistoryItem> history;
    std::list<HistoryItem>::iterator current_history_item;
    int cacheHits;

    //libfwbuilder::FWObject *currentObj;
    ObjectTreeView *current_tree_view;

    int treeWidth;
    int treeHeight;

    bool active;

    QMenu *popup_menu;

    ObjectTreeViewItem *lastClickedItem;

/* this is a reverse idex of all objects in all trees. We use it to
 * quickly locate given object in the tree and open it
 */
    std::map<libfwbuilder::FWObject*, ObjectTreeViewItem*> allItems;

    ProjectPanel *m_project;

    int dedup_marker_global_counter;

    void buildNewObjectMenu();
    
    ObjectTreeViewItem* insertObject(ObjectTreeViewItem *itm,
                                     libfwbuilder::FWObject *obj);
    void insertSubtree( ObjectTreeViewItem *itm,libfwbuilder::FWObject *obj );

    QString getTreeLabel(libfwbuilder::FWObject *obj, int col);

    void showObjectInTree(ObjectTreeViewItem *otvi);

    void setObjectIcon(libfwbuilder::FWObject *obj, QPixmap *pm);

    void removeLib(int idx);

    void makeNameUnique(libfwbuilder::FWObject* p,libfwbuilder::FWObject* obj);

    void expandOrCollapseCurrentTreeNode(QTreeWidgetItem*, bool);
         
    /* find the name of the interface that was created last */
    QString findNewestInterfaceName(libfwbuilder::FWObject *parent);

    libfwbuilder::FWObject* actuallyCreateObject(
        libfwbuilder::FWObject *parent,
        const QString &objType,
        const QString &objName,
        libfwbuilder::FWObject *copyFrom=nullptr,
        QUndoCommand* macro = 0);

    void extractFirewallsFromGroup(libfwbuilder::ObjectGroup *gr,
                                   std::set<libfwbuilder::Firewall*> &fo);

    libfwbuilder::FWObject* actuallyPasteTo(libfwbuilder::FWObject *target,
                                            libfwbuilder::FWObject *obj,
                                            std::map<int,int> &map_ids);

    void actuallyDeleteObject(libfwbuilder::FWObject *obj, QUndoCommand* macro = 0);

    void findWhereUsedRecursively(libfwbuilder::FWObject *obj,
                                  libfwbuilder::FWObject *top,
                                  std::set<libfwbuilder::FWObject*> &resset);

    void refreshSubtree(QTreeWidgetItem *parent, QTreeWidgetItem *itm);

    libfwbuilder::FWObject* newLibrary(QUndoCommand* macro);
    libfwbuilder::FWObject* newHost(QUndoCommand* macro);
    libfwbuilder::FWObject* newInterface(QUndoCommand* macro);
    libfwbuilder::FWObject* newFirewall(QUndoCommand* macro);
    libfwbuilder::FWObject* newCluster(QUndoCommand* macro, bool fromSelected = false);
    libfwbuilder::FWObject* newClusterIface(QUndoCommand* macro);
    libfwbuilder::FWObject* newStateSyncClusterGroup(QUndoCommand* macro);
    libfwbuilder::FWObject* newFailoverClusterGroup(QUndoCommand* macro);
    libfwbuilder::FWObject* newInterfaceAddress(QUndoCommand* macro);
    libfwbuilder::FWObject* newInterfaceAddressIPv6(QUndoCommand* macro);
    libfwbuilder::FWObject* newPhysicalAddress(QUndoCommand* macro);
    libfwbuilder::FWObject* newPolicyRuleSet (QUndoCommand* macro);
    libfwbuilder::FWObject* newNATRuleSet (QUndoCommand* macro);
    libfwbuilder::FWObject* newAttachedNetworks(QUndoCommand* macro);

   
public slots:
     virtual void libChanged(int l);
     virtual void switchingTrees(QWidget* w);
     virtual void currentTreePageChanged(int i);

     void expandCurrentTreeNode();
     void collapseCurrentTreeNode();
     
     void newClusterFromSelected();
     
     void selectionChanged(QTreeWidgetItem *cur);

     void removeUserFolder();
     void moveItems(ObjectTreeViewItem *dest,
                    const std::list<libfwbuilder::FWObject *> &items);

     /**
      * open object obj in the editor. Does not open editor panel
      * if it is closed. Asks FWWindow permission to own editor.
      */
     bool switchObjectInEditor(libfwbuilder::FWObject *obj);

     /**
      * same as above but opens editor panel if it is closed. This is
      * an entry point for menu items 'edit', all 'new object' as well
      * as doubleclick
      */
     bool editObject(libfwbuilder::FWObject *obj);

     /*
      * forget about currently opened object; close editor panel if it is open
      */
     void closeObject();

     void editSelectedObject();
     void openSelectedRuleSet();

     void contextMenuRequested(const QPoint &pos);

     /*
      * Internal: this method is used in actuallyPasteTo(). This
      * method checks if the target object is appropriate and replaces
      * it with parent if needed. Also does validation and shows error
      * dialogs if validation fails. Returns new parent or nullptr if
      * validation fails.
      */
     libfwbuilder::FWObject* prepareForInsertion(libfwbuilder::FWObject *target,
                                                 libfwbuilder::FWObject *obj);

     libfwbuilder::FWObject* createObject(const QString &objType,
                                          const QString &objName,
                                          libfwbuilder::FWObject *copyFrom=nullptr,
                                          QUndoCommand* macro = 0);

     libfwbuilder::FWObject* createObject(libfwbuilder::FWObject *parent,
                                          const QString &objType,
                                          const QString &objName,
                                          libfwbuilder::FWObject *copyFrom=nullptr,
                                          QUndoCommand* macro = 0);

     void newObject();

     void createNewObject();
     void newFirewallSlot();
     
     void copyObj();
     void cutObj();
     void pasteObj();
     void delObj(QUndoCommand* macro = 0);
     void dumpObj();
     void compile();
     void install();
     void inspect();

     void duplicateObj(QAction*);
     void moveObj(QAction*);

     void undeleteLibrary();
     
     void groupObjects();

     void openObjectInTree(QTreeWidgetItem *otvi);
     void openObjectInTree(libfwbuilder::FWObject *obj);

     void find();
     void findObject();

     virtual void lockObject();
     virtual void unlockObject();
     virtual void simulateInstall();
     virtual void findWhereUsedSlot();

     void addSubfolderSlot();
     void addNewKeywordSlot();
     void processKeywordSlot();

     void makeSubinterface(QAction*);
     
public:

     Ui::ObjectManipulator_q *m_objectManipulator;
     
     ObjectManipulator(QWidget *parent);
     ~ObjectManipulator();

     void setupProject(ProjectPanel *project);

     void libChangedById(int id);

     libfwbuilder::FWObject* getNextUserLib(libfwbuilder::FWObject *after_this=nullptr);

     std::vector<QTreeWidget*> getTreeWidgets();

     void filterFirewallsFromSelection(
         std::vector<libfwbuilder::FWObject*> &so,
         std::set<libfwbuilder::Firewall*> &fo);

     QString getStandardName(libfwbuilder::FWObject *parent,
                             const std::string &objtype,
                             const std::string &namesuffix);

     QString makeNameUnique(libfwbuilder::FWObject* parent,
                            const QString &obj_name, const QString &obj_type);

     void autorename(libfwbuilder::FWObject *obj);
     void autorename(std::list<libfwbuilder::FWObject*> &obj_list,
                     const std::string &objtype,
                     const std::string &namesuffix);
     void autorenameVlans(std::list<libfwbuilder::FWObject*> &obj_list);

     std::string getFolderNameString(libfwbuilder::FWObject *obj);

     void reload();

     void loadObjects();
     void clearObjects();

     /* Add/remove user folder from tree */
     void addUserFolderToTree(libfwbuilder::FWObject *obj,
                              const QString &folder);
     void removeUserFolderFromTree(libfwbuilder::FWObject *obj,
                                   const QString &folder);
     void moveToFromUserFolderInTree(libfwbuilder::FWObject *parent,
                                     libfwbuilder::FWObject *objToMove,
                                     const QString &oldFolder,
                                     const QString &newFolder);

     void reopenCurrentItemParent();

     void insertSubtree(libfwbuilder::FWObject *parent,
                        libfwbuilder::FWObject *obj);
     void removeObjectFromTreeView(libfwbuilder::FWObject *obj );

     void addLib(libfwbuilder::FWObject *lib);
     void removeLib(libfwbuilder::FWObject *lib);
     void openLibForObject(libfwbuilder::FWObject *obj);

     void openObjectInTree(libfwbuilder::FWObject *obj, bool register_in_history);
     void openObjectInTree(ObjectTreeViewItem *otvi,    bool register_in_history);

     void removeObjectFromHistory(libfwbuilder::FWObject *obj);
     void addObjectToHistory(ObjectTreeViewItem* otvi,
                             libfwbuilder::FWObject *obj);
     ObjectTreeViewItem* getCurrentHistoryItem();
     
     /*
      * searches history trying to find an object that has given
      * parent. Used to find which rule set of the firewall user
      * looked at last.
      */
     libfwbuilder::FWObject* findRuleSetInHistoryByParentFw(
         libfwbuilder::FWObject* parent);
         
     void expandObjectInTree(libfwbuilder::FWObject *obj);
     
     libfwbuilder::FWObject* duplicateObject(libfwbuilder::FWObject *target,
                                             libfwbuilder::FWObject *obj);

     void moveObject(libfwbuilder::FWObject *target,
                     libfwbuilder::FWObject *obj);

     void moveObject(const QString &targetLibName, libfwbuilder::FWObject *obj);

     // This method is called toprocess event objectMovedEvent. this event is
     // posted by FWCmdMoveObject after the object is moved from one
     // part of the tree to another, including when an object moves to the
     // Deleted Objects library
     void objectMoved(libfwbuilder::FWObject *obj);

     libfwbuilder::FWObject* pasteTo(libfwbuilder::FWObject *target,
                                     libfwbuilder::FWObject *obj);

     void updateLibColor(libfwbuilder::FWObject *lib);
     void updateLibName(libfwbuilder::FWObject *lib);

     void autoRenameChildren(libfwbuilder::FWObject *obj,
                             const QString &oldName);
     void updateObjectInTree(libfwbuilder::FWObject *obj, bool subtree=false);

     void updateCreateObjectMenu(libfwbuilder::FWObject* lib);
     
     ObjectTreeView* getCurrentObjectTree();
     libfwbuilder::FWObject* getSelectedObject();

     /**
      *  this method opens given library in the tree
      */
     void openLib(libfwbuilder::FWObject *lib);

     /**
      * returns pointer at a library that is currently opened in the tree
      */
     libfwbuilder::FWObject*  getCurrentLib();

     /**
      *  this method makes sure the system library is NOT opened in the
      *  tree. If it is, it switches to the 'User' library. If one of
      *  the user's libraries is already opened, it does nothing.
      */
     void closeSystemLib();

     void deleteObject(libfwbuilder::FWObject *obj, QUndoCommand* macro = 0);

     /**
      * select whatever object is current in the tree (used to restore
      * selected state of the tree item after it was unselected)
      */
     void select();

     /**
      * unselect whatever object is currently selected
      */
     void unselect();

     /**
      * controls whether "Deleted Objects" library is shown
      */
     void showDeletedObjects(bool f);


     /**
      * checks if currently selected object can be locked. Note that
      * if its parent or any object all the way up to the library is
      * read-only, then the state of the current object can not be
      * changed which makes it not lockable.
      */
     bool isCurrentObjectLockable();
     bool isCurrentObjectUnlockable();
     
     /**
      * get boolean flags that describe state of the menu items.
      * Can be used for both pop-up context menu and the main menu.
      */
     void getMenuState(bool haveMoveTargets,
                       bool &dupMenuItem,
                       bool &moveMenuItem,
                       bool &copyMenuItem,
                       bool &pasteMenuItem,
                       bool &delMenuItem,
                       bool &newMenuItem,
                       bool &inDeletedObjects);

     bool getDeleteMenuState(libfwbuilder::FWObject *obj);

     void updateLastInstalledTimestamp(libfwbuilder::FWObject *o);
     void updateLastCompiledTimestamp(libfwbuilder::FWObject *o);

     std::list<libfwbuilder::Firewall*> findFirewallsForObject(
         libfwbuilder::FWObject *o);

     void findAllFirewalls(std::list<libfwbuilder::Firewall*> &fws);

     std::list<libfwbuilder::Cluster*> findClustersUsingFirewall(libfwbuilder::FWObject *fw);
     void findAllClusters(std::list<libfwbuilder::Cluster*> &fws);

     void loadExpandedTreeItems();
     void saveExpandedTreeItems();

     void loadSectionSizes();
     void saveSectionSizes();

     void setAttributesColumnEnabled(bool f);

     QAction* addNewObjectMenuItem(QMenu *menu,
                                   const char* type_name,
                                   const QString &text=QString(),
                                   int add_to_group_id=-1);

     void addSubinterfaceSubmenu(
         QMenu *menu,
         const std::list<libfwbuilder::FWObject*> &top_level_interfaces);
     
     bool isObjectAllowed(const QString &type_name);
     bool isObjectAllowed(libfwbuilder::FWObject *target,
                          libfwbuilder::FWObject *obj);
    
     void reminderAboutStandardLib();
     
     void addSubfolderActions(QList<QAction*> &AddObjectActions, libfwbuilder::FWObject *currentObj, ObjectTreeViewItem *item, bool &addSubfolder);
signals:
     void libraryAccessChanged(bool writable);


};

#endif
