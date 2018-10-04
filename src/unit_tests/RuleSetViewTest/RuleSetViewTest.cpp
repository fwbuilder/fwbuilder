    /*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: RuleSetViewTest.cpp 2786 2010-04-01 14:05:36Z a2k $

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

#include "RuleSetViewTest.h"

#include "unistd.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDesktopWidget>
#include <QDebug>
#include <QMenuBar>
#include <QInputDialog>
#include <QApplication>
#include <QCoreApplication>
#include "StartTipDialog.h"
#include "QMetaProperty"
#include "FWObjectClipboard.h"
#include "RuleSetModel.h"
#include "FWBApplication.h"

using namespace std;
using namespace QTest;
using namespace libfwbuilder;

#define getRuleForPosition( x ) ((RuleSetModel*)view->model())->findRuleForPosition( x )

void RuleSetViewTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->resize(QSize(800,600));
    mw->startupLoad();
    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    if (d) if (d!=nullptr) d->close();
    om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    Firewall::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), Firewall::TYPENAME), Firewall::TYPENAME, "testFirewall"));
    openPolicy("testFirewall");
    view = mw->findChild<RuleSetView*>("");
    om->closeObject();
}

/*
 * Opens context menu at point relative to RuleSetView widget
 */
void RuleSetViewTest::showContextMenu(QPoint point)
{
    view->showContextMenu(view->pos() + point);
}

void RuleSetViewTest::showContextMenu(int x, int y)
{
    view->showContextMenu(view->pos() + QPoint(x,y));
}

/*
 * Opens Policy of firewall with name fwname in RuleSetView of active project
 */
void RuleSetViewTest::openPolicy(QString fwname)
{
    Policy *p = nullptr;
    foreach (FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == fwname.toStdString())
        {
            p = Firewall::cast(fw)->getPolicy();
        }
    }
    QVERIFY (p != nullptr);
    QCoreApplication::postEvent(mw, new openRulesetImmediatelyEvent(mw->activeProject()->getFileName(), p->getId()));
    QTest::qWait(100);
}

/*
 * Returns libfwbuilder::Library with name "User"
 */
Library* RuleSetViewTest::findUserLibrary()
{
    Library *lib = nullptr;
    foreach (FWObject *obj, mw->db()->getByType(Library::TYPENAME))
    {
        if (obj->getName() == "User")
        {
            lib = Library::cast(obj);
            break;
        }
    }
    return lib;
}

/*
 * Returns menu-relative position of topmost pixel clicking
 * which will trigger action
 */
QPoint findActionPos(QMenu *menu, QAction *action)
{
    int x = menu->width()/2;
    for (int y=0; y<menu->height(); y++)
    {
        if (menu->actionAt(QPoint(x,y)) != nullptr && menu->actionAt(QPoint(x,y))->text() == action->text())
            return QPoint(x,y);
    }
    return QPoint(-1,-1);
}

/*
 * Clicks item in topmost visible modal context menu.
 * Should be called right before opening menu (it waits 10 ms for menu to appear).
 */
void RuleSetViewTest::clickMenuItem(QString item)
{
    clicked = false;
    itemToClick = item;
    QTimer::singleShot(100, this, SLOT(actuallyClickMenuItem()));
}

void RuleSetViewTest::actuallyClickMenuItem()
{
    QMenu *menu = dynamic_cast<QMenu*>(app->activePopupWidget());
    Q_ASSERT(menu != nullptr);
    foreach(QAction *action, menu->actions())
    {
        if (action->text() == itemToClick)
        {
            //qDebug() << "clicking menu item" << action << action->text();
            clicked = true;
            QPoint pos = findActionPos(menu, action);
            //qDebug() << pos;
            //QTest::mouseMove(menu, pos);
            QTest::mouseClick(menu, Qt::LeftButton, Qt::NoModifier, pos);
            //action->trigger();
            //menu->hide();
            return;
        }
    }
    // menu item not found. Include menu items that were actually
    // present in the menu in the error message to implify debugging
    QStringList items;
    foreach(QAction *action, menu->actions())
    {
        items.push_back(QString("\"%1\"").arg(action->text()));
    }

    // need to hide the menu, otherwise test just hangs
    menu->hide();
    QFAIL(QString("Menu item %1 not found. Menu consists of: %2")
          .arg(itemToClick)
          .arg(items.join(" ")).toLatin1().constData());
}

/*
 * Returns topmost pixel which belongs to first cell
 * of rule in RuleSetView
 */
QPoint RuleSetViewTest::findRulePosition(Rule *rule)
{
    // Scroll view to make rule visible
    view->scrollTo(((RuleSetModel*)view->model())->indexForPosition(rule->getPosition()),
                   QAbstractItemView::EnsureVisible);
    // Find top left pixel of rule's first cell
    for (int x=0; x<view->width(); x++)
    {
        for(int y=0; y<view->height(); y++)
        {
            QModelIndex index = view->indexAt(QPoint(x,y));
            Rule *found = ((RuleSetModel*)view->model())->getRule(view->indexAt(QPoint(x,y)));
            if (found == rule && index.column() == 0)
                return QPoint(x,y);
        }
    }
    return QPoint(-1,-1);
}

/*
 * Scrolls view to bottom and returns middle pixel of view's middle line.
 * There
 */
QPoint RuleSetViewTest::getViewBottomPoint()
{
    view->scrollToBottom();
    view->viewport()->mapToParent(QPoint(view->viewport()->width()/2, view->viewport()->height()-2));
    return QPoint(view->viewport()->width()/2, view->viewport()->height()-2);
}

/*
 * Returns topmost pixel which belongs to first cell
 * of row #rule in RuleSetView
 */
QPoint RuleSetViewTest::findRulePosition(int rule)
{
    return findRulePosition(getRuleForPosition(rule));
}

/*
 * Fills group name request modal dialog with group name
 * and clicks OK.
 * Should be executed right before opening dialog.
 */
void RuleSetViewTest::createGroup(QString name)
{
    groupToCreate = name;
    // timeout should be more than clickMenuItem timeout
    QTimer::singleShot(150, this, SLOT(actuallyCreateGroup()));
}

void RuleSetViewTest::actuallyCreateGroup()
{
    QInputDialog *dlg = dynamic_cast<QInputDialog*>(app->activeModalWidget());
    Q_ASSERT(dlg != nullptr);
    QLineEdit *name = dlg->findChild<QLineEdit*>();
    QTest::keyClicks(name, groupToCreate);
    dlg->accept();
}


void RuleSetViewTest::test_add_remove()
{
    QVERIFY(view->model()->rowCount(QModelIndex()) == 0);

    // Adding one new rule
    clickMenuItem("Insert New Rule");
    showContextMenu(getViewBottomPoint());
    QVERIFY(view->model()->rowCount(QModelIndex()) == 1);
    Rule *rule1 = ((RuleSetModel*)view->model())->findRuleForPosition(0);

    // Adding rule at the bottom
    clickMenuItem("Add New Rule at the Bottom");
    showContextMenu(getViewBottomPoint());
    QVERIFY(view->model()->rowCount(QModelIndex()) == 2);
    Rule *rule2 = getRuleForPosition(1);
    QVERIFY(rule1!=rule2);
    QVERIFY(getRuleForPosition(0) == rule1);

    // Adding rule on top
    clickMenuItem("Add New Rule on Top");
    showContextMenu(getViewBottomPoint());
    QVERIFY(view->model()->rowCount(QModelIndex()) == 3);
    Rule *rule3 = getRuleForPosition(0);
    QVERIFY(rule1!=rule2 && rule1!=rule3 && rule2!=rule3);
    QVERIFY(getRuleForPosition(1) == rule1);
    QVERIFY(getRuleForPosition(2) == rule2);

    // Remove last rule
    view->selectRE(rule1, 0);
    clickMenuItem("Insert New Rule");
    showContextMenu(findRulePosition(rule1));
    QVERIFY(view->model()->rowCount(QModelIndex()) == 4);
    QVERIFY(getRuleForPosition(0) == rule3);
    QVERIFY(getRuleForPosition(2) == rule1);
    QVERIFY(getRuleForPosition(3) == rule2);
    Rule *rule4 = getRuleForPosition(1);

    // Remove last rule
    view->selectRE(rule2, 0);
    clickMenuItem("Remove Rule");
    showContextMenu(findRulePosition(rule2));
    QVERIFY(view->model()->rowCount(QModelIndex()) == 3);
    QVERIFY(getRuleForPosition(0) == rule3);
    QVERIFY(getRuleForPosition(1) == rule4);
    QVERIFY(getRuleForPosition(2) == rule1);

    // Remove first rule
    view->selectRE(rule3, 0);
    clickMenuItem("Remove Rule");
    showContextMenu(findRulePosition(rule3));
    QVERIFY(view->model()->rowCount(QModelIndex()) == 2);
    QVERIFY(getRuleForPosition(0) == rule4);
    QVERIFY(getRuleForPosition(1) == rule1);

    // Remove all created rules
    view->selectRE(rule4, 0);
    clickMenuItem("Remove Rule");
    showContextMenu(findRulePosition(rule4));
    view->selectRE(rule1, 0);
    clickMenuItem("Remove Rule");
    showContextMenu(findRulePosition(rule1));
    QVERIFY(view->model()->rowCount(QModelIndex()) == 0);
}


void RuleSetViewTest::test_group()
{
    // Adding one new rule
    clickMenuItem("Insert New Rule");
    showContextMenu(getViewBottomPoint());
    QVERIFY(view->model()->rowCount(QModelIndex()) == 1);
    Rule *rule1 = ((RuleSetModel*)view->model())->findRuleForPosition(0);

    // Adding seven new rules at the bottom
    Rule *rules[7];
    for (int i=0; i<7;i++)
    {
        clickMenuItem("Add New Rule at the Bottom");
        showContextMenu(getViewBottomPoint());
        QVERIFY(view->model()->rowCount(QModelIndex()) == i+2);
        rules[i] = getRuleForPosition(i+1);
    }

    // Create new group
    view->selectRE(rules[2], 0);
    createGroup("Test Group Name");
    clickMenuItem("New Group");
    showContextMenu(findRulePosition(rules[2]));

    QList<QModelIndex> groups;
    ((RuleSetModel*)view->model())->getGroups(groups);
    QVERIFY(groups.size() == 1);
    QVERIFY(rules[2]->getRuleGroupName() == "Test Group Name");

    // Add two more rules to the group
    view->selectRE(rules[3], 0);
    clickMenuItem("Add To the Group Test Group Name");
    showContextMenu(findRulePosition(rules[3]));

    view->clearSelection();
    view->selectRE(rules[1], 0);
    clickMenuItem("Add To the Group Test Group Name");
    showContextMenu(findRulePosition(rules[1]));

    QList<QModelIndex> groups2;
    ((RuleSetModel*)view->model())->getGroups(groups2);
    QVERIFY(groups.size() == 1);
    QVERIFY(rules[1]->getRuleGroupName() == "Test Group Name");
    QVERIFY(rules[2]->getRuleGroupName() == "Test Group Name");
    QVERIFY(rules[3]->getRuleGroupName() == "Test Group Name");
    QVERIFY(groups2.first().internalId() == groups.first().internalId());
    
    // Expanding created group
    view->expandAll();

    // Creating new rule in group using "Add Rule Below"
    view->selectRE(rules[2], 0);
    clickMenuItem("Add New Rule Below");
    showContextMenu(findRulePosition(rules[2]));
    Rule *newrule1 = ((RuleSetModel*)view->model())->findRuleForPosition(4);
    for (int i=0; i<5; i++)
        QVERIFY(newrule1 != rules[i]);
    QVERIFY(newrule1 != rule1);
    QVERIFY(newrule1->getRuleGroupName() == "Test Group Name");

    // Creating new rule in group using "Insert Rule"
    view->selectRE(rules[2], 0);
    clickMenuItem("Insert New Rule");
    showContextMenu(findRulePosition(rules[2]));
    Rule *newrule2 = ((RuleSetModel*)view->model())->findRuleForPosition(3);
    for (int i=0; i<5; i++)
        QVERIFY(newrule2 != rules[i]);
    QVERIFY(newrule2 != rule1);
    QVERIFY(newrule2 != newrule1);
    QVERIFY(newrule2->getRuleGroupName() == "Test Group Name");

    // Select and add to group two rules under it
    view->clearSelection();
    view->selectionModel()->select(((RuleSetModel*)view->model())->indexForPosition(rules[4]->getPosition()),
                                   QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    view->selectionModel()->select(((RuleSetModel*)view->model())->indexForPosition(rules[5]->getPosition()),
                                   QItemSelectionModel::Select);
    clickMenuItem("Add To the Group Test Group Name");
    showContextMenu(findRulePosition(rules[4]));
    QList<QModelIndex> groups3;
    ((RuleSetModel*)view->model())->getGroups(groups3);
    QVERIFY(groups3.size() == 1);
    QVERIFY(rules[4]->getRuleGroupName() == "Test Group Name");
    QVERIFY(rules[5]->getRuleGroupName() == "Test Group Name");
    QVERIFY(groups3.first().internalId() == groups.first().internalId());

    // Remoe first rule from group
    QVERIFY(view->model()->rowCount(groups3.first()) == 7);
    view->selectRE(rules[1], 0);
    clickMenuItem("Remove From the Group");
    showContextMenu(findRulePosition(rules[1]));
    QVERIFY(rules[1]->getRuleGroupName() == "");
    QVERIFY(view->model()->rowCount(groups3.first()) == 6);

    // Remoe last rule from group
    view->selectRE(rules[5], 0);
    clickMenuItem("Remove From the Group");
    showContextMenu(findRulePosition(rules[5]));
    QVERIFY(rules[5]->getRuleGroupName() == "");
    QVERIFY(view->model()->rowCount(groups3.first()) == 5);


    // Remove rule inside group
    view->selectRE(rules[3], 0);
    clickMenuItem("Remove Rule");
    showContextMenu(findRulePosition(rules[3]));
    QVERIFY(view->model()->rowCount(groups3.first()) == 4);

    // Remove all created rules
    for (int i=0; i<7; i++)
    {
        if (i==3) continue;
        view->selectRE(rules[i], 0);
        clickMenuItem("Remove Rule");
        showContextMenu(findRulePosition(rules[i]));
    }
    view->selectRE(newrule1, 0);
    clickMenuItem("Remove Rule");
    showContextMenu(findRulePosition(newrule1));
    view->selectRE(newrule2, 0);
    clickMenuItem("Remove Rule");
    showContextMenu(findRulePosition(newrule2));
    view->selectRE(rule1, 0);
    clickMenuItem("Remove Rule");
    showContextMenu(findRulePosition(rule1));

    QVERIFY(view->model()->rowCount() == 0);
}

void RuleSetViewTest::test_move()
{
    // Adding one new rule
    clickMenuItem("Insert New Rule");
    showContextMenu(getViewBottomPoint());
    QVERIFY(view->model()->rowCount(QModelIndex()) == 1);
    Rule *rules[4];
    rules[0] = ((RuleSetModel*)view->model())->findRuleForPosition(0);;

    // Adding three new rules at the bottom
    for (int i=1; i<4;i++)
    {
        clickMenuItem("Add New Rule at the Bottom");
        showContextMenu(getViewBottomPoint());
        QVERIFY(view->model()->rowCount(QModelIndex()) == i+1);
        rules[i] = getRuleForPosition(i);
    }

    // Verifying that all rules' positions match their indexes
    for (int i=0; i<4; i++)
        QVERIFY(rules[i]->getPosition() == i);

    // Moving second rule up
    view->selectRE(rules[1], 0);
    clickMenuItem("Move Rule Up");
    showContextMenu(findRulePosition(rules[1]));
    QVERIFY(rules[1]->getPosition() == 0);
    QVERIFY(rules[0]->getPosition() == 1);

    // Moving first rule down
    view->selectRE(rules[1], 0);
    clickMenuItem("Move Rule Down");
    showContextMenu(findRulePosition(rules[1]));
    QVERIFY(rules[1]->getPosition() == 1);
    QVERIFY(rules[0]->getPosition() == 0);

    // Moving first rule up (rule order should not change)
    view->selectRE(rules[0], 0);
    clickMenuItem("Move Rule Up");
    showContextMenu(findRulePosition(rules[0]));
    for (int i=0; i<4; i++)
        QVERIFY(rules[i]->getPosition() == i);

    // Moving last rule down (rule order should not change)
    view->selectRE(rules[3], 0);
    clickMenuItem("Move Rule Down");
    showContextMenu(findRulePosition(rules[3]));
    for (int i=0; i<4; i++)
        QVERIFY(rules[i]->getPosition() == i);

    // Selecting two rules and moving them up
    view->selectRE(rules[1], 0);
    view->selectionModel()->select(((RuleSetModel*)view->model())->indexForPosition(2),
                                   QItemSelectionModel::Select);
    clickMenuItem("Move Rules Up");
    showContextMenu(findRulePosition(rules[1]));
    QVERIFY(rules[1]->getPosition() == 0);
    QVERIFY(rules[2]->getPosition() == 1);
    QVERIFY(rules[0]->getPosition() == 2);
    QVERIFY(rules[3]->getPosition() == 3);

    // Selecting two rules and moving them down
    view->selectRE(rules[2], 0);
    view->selectionModel()->select(((RuleSetModel*)view->model())->indexForPosition(2),
                                   QItemSelectionModel::Select);
    clickMenuItem("Move Rules Down");
    showContextMenu(findRulePosition(rules[2]));
    QVERIFY(rules[1]->getPosition() == 0);
    QVERIFY(rules[3]->getPosition() == 1);
    QVERIFY(rules[2]->getPosition() == 2);
    QVERIFY(rules[0]->getPosition() == 3);

    // Adding two more rules
    Rule *newrules[6] = { rules[1], rules[3], rules[2], rules[0], nullptr, nullptr };
    for (int i=4; i<6;i++)
    {
        clickMenuItem("Add New Rule at the Bottom");
        showContextMenu(getViewBottomPoint());
        QVERIFY(view->model()->rowCount(QModelIndex()) == i+1);
        newrules[i] = getRuleForPosition(i);
    }

    // Creating a group to test moving rules inside of it
    view->selectRE(newrules[1], 0);
    for (int i=2; i<5; i++)
        view->selectionModel()->select(((RuleSetModel*)view->model())->indexForPosition(i),
                                       QItemSelectionModel::Select);
    createGroup("Test Group Name");
    clickMenuItem("New Group");
    showContextMenu(findRulePosition(newrules[1]));
    for (int i=1; i<5; i++)
        QVERIFY(newrules[i]->getRuleGroupName() == "Test Group Name");

    // Expanding created group
    view->expandAll();

    // Moving second rule of group up
    view->selectRE(newrules[2], 0);
    clickMenuItem("Move Rule Up");
    showContextMenu(findRulePosition(newrules[2]));
    QVERIFY(newrules[2]->getPosition() == 1);
    QVERIFY(newrules[1]->getPosition() == 2);

    // Moving first rule of group up. It should leave the group.
    view->selectRE(newrules[2], 0);
    clickMenuItem("Move Rule Up");
    showContextMenu(findRulePosition(newrules[2]));
    QVERIFY(newrules[2]->getPosition() == 1);
    QVERIFY(newrules[0]->getPosition() == 0);
    QVERIFY(newrules[1]->getPosition() == 2);
    QVERIFY(newrules[2]->getRuleGroupName() == "");

    // Moving first rule above group down. It should be added to group.
    view->selectRE(newrules[2], 0);
    clickMenuItem("Move Rule Down");
    showContextMenu(findRulePosition(newrules[2]));
    QVERIFY(newrules[2]->getPosition() == 1);
    QVERIFY(newrules[0]->getPosition() == 0);
    QVERIFY(newrules[1]->getPosition() == 2);
    QVERIFY(newrules[2]->getRuleGroupName() == "Test Group Name");

    // Moving last rule of group down. It should leave the group.
    view->selectRE(newrules[4], 0);
    clickMenuItem("Move Rule Down");
    showContextMenu(findRulePosition(newrules[4]));
    QVERIFY(newrules[4]->getPosition() == 4);
    QVERIFY(newrules[5]->getPosition() == 5);
    QVERIFY(newrules[3]->getPosition() == 3);
    QVERIFY(newrules[4]->getRuleGroupName() == "");

    // Moving first rule below group up. It should be added to group.
    view->selectRE(newrules[4], 0);
    clickMenuItem("Move Rule Up");
    showContextMenu(findRulePosition(newrules[4]));
    QVERIFY(newrules[4]->getPosition() == 4);
    QVERIFY(newrules[5]->getPosition() == 5);
    QVERIFY(newrules[3]->getPosition() == 3);
    QVERIFY(newrules[4]->getRuleGroupName() == "Test Group Name");

    // Removing all created rules
    for (int i=0; i<6; i++)
    {
        view->selectRE(newrules[i], 0);
        clickMenuItem("Remove Rule");
        showContextMenu(findRulePosition(newrules[i]));
    }
    QVERIFY(view->model()->rowCount() == 0);
}

void RuleSetViewTest::test_copy_paste()
{
    // Adding one new rule
    clickMenuItem("Insert New Rule");
    showContextMenu(getViewBottomPoint());
    QVERIFY(view->model()->rowCount(QModelIndex()) == 1);
    Rule *rules[4];
    rules[0] = ((RuleSetModel*)view->model())->findRuleForPosition(0);

    // Adding three new rules at the bottom
    for (int i=1; i<4;i++)
    {
        clickMenuItem("Add New Rule at the Bottom");
        showContextMenu(getViewBottomPoint());
        QVERIFY(view->model()->rowCount(QModelIndex()) == i+1);
        rules[i] = getRuleForPosition(i);
    }

    // Cutting second rule
    view->selectRE(rules[1], 0);
    clickMenuItem("Cut Rule");
    showContextMenu(findRulePosition(rules[1]));
    QVERIFY(view->model()->rowCount() == 3);

    // Pasting cutted rule above first
    view->selectRE(rules[0], 0);
    clickMenuItem("Paste Rule Above");
    showContextMenu(findRulePosition(rules[0]));
    QVERIFY(view->model()->rowCount() == 4);
    rules[1] = getRuleForPosition(0);

    // Copying second rule
    view->selectRE(rules[1], 0);
    clickMenuItem("Copy Rule");
    showContextMenu(findRulePosition(rules[1]));
    QVERIFY(view->model()->rowCount() == 4);

    // Pasting rule after first rule
    view->selectRE(rules[1], 0);
    clickMenuItem("Paste Rule Below");
    showContextMenu(findRulePosition(rules[1]));
    QVERIFY(view->model()->rowCount() == 5);

    // Copy two rules
    view->selectRE(getRuleForPosition(1), 0);
    view->selectionModel()->select(((RuleSetModel*)view->model())->indexForPosition(2),
                                   QItemSelectionModel::Select);
    clickMenuItem("Copy Rules");
    showContextMenu(findRulePosition(getRuleForPosition(1)));
    QVERIFY(view->model()->rowCount() == 5);

    // Paste rules at the bottom
    view->selectRE(getRuleForPosition(4), 0);
    clickMenuItem("Paste Rule Below");
    showContextMenu(findRulePosition(getRuleForPosition(4)));
    QVERIFY(view->model()->rowCount() == 7);

}
