/*

                          Firewall Builder

                 Copyright (C) 2003-2009 NetCitadel, LLC

  Author:  Illiya Yalovoy <yalovoy@gmail.com>


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

#ifndef RULESETVIEW_H
#define RULESETVIEW_H

#include <qglobal.h>
#include <QtWidgets/QTreeView>
#include <QUndoCommand>

#include "fwbuilder/Rule.h"

#include "ColDesc.h"

using namespace std;

namespace libfwbuilder {
    class FWObject;
    class Firewall;
    class FWReference;
    class RuleElement;
    class RuleSet;
    class Policy;
    class NAT;
    class Routing;
    class RuleElement;
    class RuleElementItf;
    class NATRule;
    class RoutingRule;
}

class ProjectPanel;
class FWObjectSelectionModel;
class RuleNode;
class RuleSetModel;

class SelectionMemento
{
    public:
    SelectionMemento() { rule_id = -1; column = -1; }
    int rule_id;
    int column;
};

class RuleSetView : public QTreeView
{
    Q_OBJECT;

public:
    RuleSetView(ProjectPanel *project, QWidget *parent);
    ~RuleSetView();

    static RuleSetView* getRuleSetViewByType(ProjectPanel *project,
                                             libfwbuilder::RuleSet *ruleset,
                                             QWidget *parent);

    virtual void init();
    virtual void initActions();

    void selectRE(QModelIndex index);
    /**
     * selects rule element a reference 'ref' belongs to
     */
    void selectRE(libfwbuilder::FWReference *ref);

    /**
     * makes specified column of a rule current.
     */
    void selectRE(libfwbuilder::Rule *rule, int col);
    void selectRE(libfwbuilder::Rule *rule, ColDesc::ColumnType type);

    /**
     * makes specified rule element current and selects object <obj> in it.
     */
    void selectRE(libfwbuilder::RuleElement *re, libfwbuilder::FWObject *obj);

    void makeCurrentRuleVisible();
    
    void selectObject(int position, int column, int number);

    void repaintSelection();
    void updateAll();
    void updateCurrentCell();

    void saveCurrentRowColumn(SelectionMemento &memento);
    void restoreCurrentRowColumn(SelectionMemento &memento);

    void unselect();

    libfwbuilder::FWObject* getSelectedObject();

    void moveRule() {} //This method is colled from the class ProjectPanel.cpp:944

    void restoreCollapsedGroups();

    int  getColByType(ColDesc::ColumnType) const;

    virtual int rowHeight(const QModelIndex&) const;
    void updateWidget();

    void setModel(QAbstractItemModel *model);
    void addToGroup(bool isAbove);

    void addRowMenuItemsToMenu(QMenu *menu) const;
    void addGenericMenuItemsToContextMenu(QMenu *menu) const;

    int selectedRulesCount() const  { return getSelectedRows().size(); }
    
public slots:

    void showContextMenu(const QPoint&);

    void insertRule();

    void insertNewRuleOnTop();
    void insertNewRuleAtBottom();
    void addRuleAfterCurrent();
    void moveRuleUp();
    void moveRuleDown();

    void removeRule();
    void disableRule();
    void enableRule();

    void copyRule();
    void cutRule();
    void pasteRuleAbove();
    void pasteRuleBelow();

    void renameGroup();
    void removeFromGroup();
    void newGroup();
    void addToGroupAbove();
    void addToGroupBelow();

    void setColorEmpty();
    void setColorRed();
    void setColorBlue();
    void setColorOrange();
    void setColorPurple();
    void setColorGray();
    void setColorYellow();
    void setColorGreen();
    void setRuleColor(const QString &c);

    void itemDoubleClicked(const QModelIndex& index);
    void editSelected();

    void changeDirectionToIn();
    void changeDirectionToOut();
    void changeDirectionToBoth();

    void changeActionToAccept();
    void changeActionToDeny();
    void changeActionToReject();
    void changeActionToAccounting();
    void changeActionToPipe();
    void changeActionToCustom();
    void changeActionToBranch();
    void changeActionToContinue();
    void changeActionToTranslate();
    void changeActionToNATBranch();

    void changeLogToOn();
    void changeLogToOff();
    void negateRE();
    void revealObjectInTree();
    void findWhereUsedSlot();
    void deleteSelectedObject();

    void copySelectedObject();
    void cutSelectedObject();
    void pasteObject();

    void convertToAny();

    void saveCollapsedGroups();

    void updateAllColumnsSize();
    void updateColumnSizeForIndex(QModelIndex);
    void updateSectionSizesForIndex(QModelIndex, QModelIndex);
    
    void updateObject(libfwbuilder::FWObject* object);

    void compileCurrentRule();
    void updateSelectionSensitiveActions();

    void setSelectedRows(const QModelIndex firstIndex,
                         const QModelIndex lastIndex);

protected:

    FWObjectSelectionModel *fwosm;

    virtual void mousePressEvent( QMouseEvent* ev );
    virtual void mouseReleaseEvent( QMouseEvent* ev );
    virtual void mouseMoveEvent( QMouseEvent* ev );

    virtual void dragEnterEvent( QDragEnterEvent *ev);
    virtual void dropEvent( QDropEvent *ev);
    virtual void dragMoveEvent( QDragMoveEvent *ev);

    virtual QDrag* dragObject();
    virtual bool event( QEvent * event );

    virtual void keyPressEvent( QKeyEvent* ev );

    bool validateForInsertion(QModelIndex index, libfwbuilder::FWObject *obj);
    void deleteObject(QModelIndex index, libfwbuilder::FWObject *obj,
                      QString text, QUndoCommand* makro = 0);
    bool insertObject(QModelIndex index, libfwbuilder::FWObject *obj,
                      QString text, QUndoCommand* makro = 0);

    bool validateForInsertion(libfwbuilder::RuleElement *re,
                              libfwbuilder::FWObject *obj, bool quiet=false);
    bool validateForInsertionToInterfaceRE(libfwbuilder::RuleElementItf *re,
                                           libfwbuilder::FWObject *obj);

    QAction* createAction(QString label, const char* member,
                          const QKeySequence &shortcut = 0);


private:

    //this bool var is needed for starting drag when user moves the mouse
    //but not when he just clicks selected record
    bool startingDrag;
    QAction *compileRuleAction;
    QAction *removeFromGroupAction;
    QAction *newGroupAction;
    QAction *moveRuleUpAction;
    QAction *moveRuleDownAction;
    QAction *insertRuleAction;
    QAction *addRuleAfterCurrentAction;
    QAction *addToGroupAboveAction;
    QAction *addToGroupBelowAction;
    QAction *removeRuleAction;

    QAction *copyRuleAction;
    QAction *cutRuleAction;
    QAction *pasteRuleAboveAction;
    QAction *pasteRuleBelowAction;
    QAction *disableRuleAction;
    QAction *enableRuleAction;

    QAction *setColorEmptyAction;
    QAction *setColorRedAction;
    QAction *setColorBlueAction;
    QAction *setColorOrangeAction;
    QAction *setColorPurpleAction;
    QAction *setColorGrayAction;
    QAction *setColorYellowAction;
    QAction *setColorGreenAction;

    QMenu *popup_menu;

    libfwbuilder::FWObject *getObject(const QPoint &pos, const QModelIndex &index);
    libfwbuilder::FWObject *getObject(int number, const QModelIndex &index);
    int getObjectNumber(libfwbuilder::FWObject *object, const QModelIndex &index);

    void selectObject(libfwbuilder::FWObject *object, const QModelIndex &index);
    ProjectPanel* project;

    void addGroupMenuItemsToContextMenu(QMenu *menu) const;

    void addChangeColorSubmenu(QMenu *menu) const;
    void addCommonRowItemsToContextMenu(QMenu *menu) const;

    void setEnabledRow(bool flag);

    QModelIndexList getSelectedRows() const;

    bool isOnlyTopLevelRules(const QModelIndexList &list) const;
    bool isOneLevelRules(const QModelIndexList &list);

    void editSelected(const QModelIndex &index);
    bool switchObjectInEditor(const QModelIndex &index, bool validate=true);

    void addColumnRelatedMenu(QMenu *menu,const QModelIndex &index, RuleNode *node, const QPoint& pos);

    void changeDitection(libfwbuilder::PolicyRule::Direction dir);
    void changeAction(int act);
    void changeLogging(bool flag);
    void configureGroups();
    bool showToolTip(QEvent *event);
    void resizeColumns();
    void copyAndInsertObject(QModelIndex &index, libfwbuilder::FWObject *object);
    libfwbuilder::RuleElement* getRE(QModelIndex index);

    bool canChange(RuleSetModel* md);
    void insertRule(QModelIndex index, bool isAfter = false);
    libfwbuilder::FWObject* createInsertTemplate(ProjectPanel* proj_p, int id);
    void setActionState(QAction *action, bool state);

    bool useDummy(libfwbuilder::RuleElement* re) const;
    void setDummy(libfwbuilder::RuleElement* re);
    bool isDummy(libfwbuilder::RuleElement* re, int child) const;
};

class PolicyView : public RuleSetView
{

public:
    PolicyView(ProjectPanel *project, libfwbuilder::Policy *p, QWidget *parent);
    virtual ~PolicyView() {}
private:


};

class NATView : public RuleSetView
{
public:
    NATView(ProjectPanel *project, libfwbuilder::NAT *p, QWidget *parent);
    virtual ~NATView(){}

};

class RoutingView : public RuleSetView
{
public:
    RoutingView(ProjectPanel *project, libfwbuilder::Routing *p, QWidget *parent);
    virtual ~RoutingView() {}

};

#endif // RULESETVIEW_H
