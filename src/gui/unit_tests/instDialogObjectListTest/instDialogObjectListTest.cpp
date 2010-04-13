/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: instDialogObjectListTest.cpp 2786 2010-04-01 14:05:36Z a2k $

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

  ================================

  Tests implemented in this module:

  ticket 1089

  * user clicks "compile" button in the topmost toolbar. The list
  should include all firewalls, but only those that require recompile
  should have "compile" checkboxes checked.

  * user uses main menu "Rules/Compile". This is the same function as
  the previous one

  * user opens Policy of the firewall "test1" and clicks button
  "Compile" in the toolbar right above rule set view. The list should
  include only firewall "test1" and checkbox "Compile" should be checked
  because it requires recompile

  * the same test as the previous one, but with firewall test2. This
  one does not require recompile and checkbox "Compile" should not be
  checked.

  * user selects firewall test1 in the tree and opens context menu,
  then clicks item "Compile". This is the same test as 3.

  * user selects firewall test2 in the tree and opens context menu,
  then clicks item "Compile". This is the same test as 4.

  * Select two firewalls in the tree (test1 and test2), open context
  menu and click "Compile". Should get a list with both firewalls, with
  checkbox "Compile" selected for test2 and not selected for test1.

  * all previous tests tested "Compile" function. This test should test
  first step of the "Install" function.

  * emulate clicking "Install" button in the main toolbar

  * check that the first page of the dialog has column of "compile"
  checkboxes and column of "install" checkboxes

  * check that "compile" checkboxes are checked according to the rules
  described above in this ticket

  Ticket 1153:

  * tests page1_5(), page1_6(), page1_7() should emulate opening
  context menu and clicking menu item "Compile" (see ticket
  #1089). Please add check that the menu item "Compile" is present
  and enabled

  * test page1_8() should also check that checkboxes in column
  "compile" and "install" are visible and enabled. Right now it only
  checks their "checked" state

  * test page1_8() should also test checkboxes next to the firewall
  objects test3 and test4

*/

#include "instDialogObjectListTest.h"

#include "unistd.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QMenuBar>
#include <QApplication>
#include <QCoreApplication>

#include "FWObjectClipboard.h"

using namespace std;
using namespace QTest;
using namespace libfwbuilder;

void instDialogObjectListTest::initTestCase()
{
    mw = new FWWindow();
    mw->show();
    mw->loadFile("test_work.fwb", false);
    FWObjectClipboard *clip = new FWObjectClipboard();
}

void instDialogObjectListTest::openPolicy(QString fwname)
{
    Policy *p = NULL;
    foreach (FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == fwname.toStdString())
        {
            p = Firewall::cast(fw)->getPolicy();
        }
    }
    QVERIFY (p != NULL);
    QCoreApplication::postEvent(mw, new openRulesetImmediatelyEvent(mw->activeProject()->getFileName(), p->getId()));
    QTest::qWait(100);
}

void instDialogObjectListTest::verifyDialog(instDialog *dlg, int items)
{
    QVERIFY(dlg != NULL);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != NULL);
    if (items != -1)
        QVERIFY(table->topLevelItemCount() == items);
    QTreeWidgetItemIterator it(table, QTreeWidgetItemIterator::Enabled);
    while (*it)
    {
        if ((*it)->text(0) == "cluster1") QVERIFY((*it)->checkState(1) == Qt::Checked);
        if ((*it)->text(0) == "test1") QVERIFY((*it)->checkState(1) == Qt::Unchecked);
        if ((*it)->text(0) == "test2") QVERIFY((*it)->checkState(1) == Qt::Checked);
        it++;
    }
}

QPoint findItemPos(ObjectTreeViewItem *item, ObjectTreeView *tree)
{
    for (int h=10; h<tree->height(); h+=1)
    {
        for (int w=75; w<tree->width(); w+=1)
        {
            if(tree->itemAt(w,h) == item)
                return QPoint(w, h);
        }
    }
    return QPoint(-1,-1);
}

void instDialogObjectListTest::closeContextMenu()
{
    QMenu *menu;
    foreach(QWidget *w, QApplication::allWidgets())
    {
        if (w->objectName() == "objectTreeContextMenu")
        {
            menu = dynamic_cast<QMenu*>(w);
            break;
        }
    }
    menu->hide();
}

void instDialogObjectListTest::openContextMenu(ObjectManipulator *om, ObjectTreeViewItem *item, ObjectTreeView *tree, const QString &actionText)
{
    QTimer::singleShot(100, this, SLOT(closeContextMenu()));
    om->contextMenuRequested(findItemPos(item, tree));
    QMenu *menu;
    foreach(QWidget *w, QApplication::allWidgets())
    {
        if (w->objectName() == "objectTreeContextMenu")
        {
            menu = dynamic_cast<QMenu*>(w);
            break;
        }
    }
    foreach (QObject *act, menu->children())
    {
        QAction *action = dynamic_cast<QAction*>(act);
        if (action == NULL) continue;
        if (action->text() == actionText)
        {
            action->activate(QAction::Trigger);
            break;
        }
    }
}

void instDialogObjectListTest::page1_1()
{
    QAction *compile = mw->findChild<QAction*>("compileAction");
    compile->activate(QAction::Trigger);
    QTest::qWait(100);
    instDialog *dlg = NULL;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != NULL)
            dlg = dynamic_cast<instDialog*>(w);
    verifyDialog(dlg, 3);
    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(1000);
}

void instDialogObjectListTest::page1_2()
{
    QMenu *rules = mw->menuBar()->findChild<QMenu*>("RulesMenu");
    QAction *compile = NULL;

    foreach(QAction *itm, rules->actions())
        if (itm->objectName() == "compileAction")
            compile = dynamic_cast<QAction*>(itm);
    QVERIFY(compile != NULL);
    compile->activate(QAction::Trigger);
    QTest::qWait(100);
    instDialog *dlg = NULL;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != NULL)
            dlg = dynamic_cast<instDialog*>(w);
    verifyDialog(dlg, 3);
    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(1000);
}

void instDialogObjectListTest::page1_3()
{
    openPolicy("test1");
    QTest::qWait(10);
    QToolButton* compileThis = mw->findChild<QToolButton*>("compile_this_fw");
    QVERIFY(compileThis != NULL);
    QTest::mouseClick (compileThis, Qt::LeftButton);
    QTest::qWait(100);
    instDialog *dlg = NULL;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != NULL)
            dlg = dynamic_cast<instDialog*>(w);
    verifyDialog(dlg, 1);
    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(1000);
}

void instDialogObjectListTest::page1_4()
{
    openPolicy("test2");
    QTest::qWait(100);
    QToolButton* compileThis = mw->findChild<QToolButton*>("compile_this_fw");
    QVERIFY(compileThis != NULL);
    QTest::mouseClick (compileThis, Qt::LeftButton);
    QTest::qWait(100);
    instDialog *dlg = NULL;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != NULL)
            dlg = dynamic_cast<instDialog*>(w);
    verifyDialog(dlg, 1);
    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(1000);
}

void instDialogObjectListTest::page1_5()
{
    ObjectTreeView *tree = mw->getCurrentObjectTree();
    tree->expandAll();
    ObjectTreeViewItem *test1 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    ObjectTreeViewItem *test2 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test2", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    tree->scrollToItem(test1);
    tree->selectionModel()->select(tree->indexAt(findItemPos(test1, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(test1);
    ObjectManipulator *om = mw->findChild<ObjectManipulator*>("om");
    openContextMenu(om, test1, tree, "Compile");
    instDialog *dlg = NULL;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != NULL)
            dlg = dynamic_cast<instDialog*>(w);
    verifyDialog(dlg, 1);
    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(1000);
}

void instDialogObjectListTest::page1_6()
{
    ObjectTreeView *tree = mw->getCurrentObjectTree();
    tree->expandAll();
    ObjectTreeViewItem *test1 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    ObjectTreeViewItem *test2 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test2", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    tree->scrollToItem(test2);
    tree->selectionModel()->select(tree->indexAt(findItemPos(test2, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(test2);
    ObjectManipulator *om = mw->findChild<ObjectManipulator*>("om");
    openContextMenu(om, test2, tree, "Compile");
    instDialog *dlg = NULL;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != NULL)
            dlg = dynamic_cast<instDialog*>(w);
    verifyDialog(dlg, 1);
    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(1000);
}

void instDialogObjectListTest::page1_7()
{
    ObjectTreeView *tree = mw->getCurrentObjectTree();
    tree->expandAll();
    ObjectTreeViewItem *test1 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    ObjectTreeViewItem *test2 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test2", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    tree->scrollToItem(test1);
    tree->selectionModel()->select(tree->indexAt(findItemPos(test1, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(test1);
    tree->selectionModel()->select(tree->indexAt(findItemPos(test2, tree)), QItemSelectionModel::Select);
    ObjectManipulator *om = mw->findChild<ObjectManipulator*>("om");
    openContextMenu(om, test2, tree, "Compile");
    instDialog *dlg = NULL;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != NULL)
            dlg = dynamic_cast<instDialog*>(w);
    verifyDialog(dlg, 2);
    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(1000);
}

void instDialogObjectListTest::page1_8()
{
    ObjectTreeView *tree = mw->getCurrentObjectTree();
    ObjectTreeViewItem *test3 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test3", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    tree->selectionModel()->select(tree->indexAt(findItemPos(test3, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(test3);
    ObjectManipulator *om = mw->findChild<ObjectManipulator*>("om");
    openContextMenu(om, test3, tree, "Compile");
    instDialog *dlg = NULL;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != NULL)
            dlg = dynamic_cast<instDialog*>(w);
    QVERIFY(dlg != NULL);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != NULL);
    QVERIFY(table->topLevelItemCount() == 1);
    QVERIFY(table->topLevelItem(0)->text(0) == "test3");

    QFrame *warning_space = dlg->findChild<QFrame*>("warning_space");
    QLabel *warning_message_1 = dlg->findChild<QLabel*>("warning_message_1");
    QLabel *warning_message_2 = dlg->findChild<QLabel*>("warning_message_2");

    QVERIFY(warning_space->isVisible());
    QVERIFY(warning_message_1->isVisible());
    QVERIFY(warning_message_2->isVisible());

    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(1000);
}
