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
#include "FWBApplication.h"

#include "FWObjectClipboard.h"

using namespace std;
using namespace QTest;
using namespace libfwbuilder;


#define COMPILE_CHECKBOX_COLUMN 1
#define INSTALL_CHECKBOX_COLUMN 2


void instDialogObjectListTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->loadFile("test_work.fwb", false);
}

void instDialogObjectListTest::openPolicy(QString fwname)
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

void instDialogObjectListTest::verifyCompileCheckboxes(QTreeWidget *table,
                                                       int items)
{
    if (items != -1)
        QVERIFY(table->topLevelItemCount() == items);

    QTreeWidgetItemIterator it(table, QTreeWidgetItemIterator::Enabled);
    while (*it)
    {
        if ((*it)->text(0) == "cluster1")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(COMPILE_CHECKBOX_COLUMN) == Qt::Checked);
        }

        if ((*it)->text(0) == "test1")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(COMPILE_CHECKBOX_COLUMN) == Qt::Checked);
        }

        if ((*it)->text(0) == "test2")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(COMPILE_CHECKBOX_COLUMN) == Qt::Unchecked);
        }

        if ((*it)->text(0) == "test3")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(COMPILE_CHECKBOX_COLUMN) == Qt::Unchecked);
        }

        if ((*it)->text(0) == "test4")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(COMPILE_CHECKBOX_COLUMN) == Qt::Unchecked);
        }

        it++;
    }
}

/*
 * This function checks the state of checkboxes after firewall test1 has been compiled
 */
void instDialogObjectListTest::verifyCompileCheckboxes_2(QTreeWidget *table)
{
    QTreeWidgetItemIterator it(table, QTreeWidgetItemIterator::Enabled);
    while (*it)
    {
        if ((*it)->text(0) == "cluster1")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(COMPILE_CHECKBOX_COLUMN) == Qt::Checked);
        }

        if ((*it)->text(0) == "test1")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(COMPILE_CHECKBOX_COLUMN) == Qt::Unchecked);
        }

        if ((*it)->text(0) == "test2")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(COMPILE_CHECKBOX_COLUMN) == Qt::Unchecked);
        }

        if ((*it)->text(0) == "test3")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(COMPILE_CHECKBOX_COLUMN) == Qt::Unchecked);
        }

        if ((*it)->text(0) == "test4")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(COMPILE_CHECKBOX_COLUMN) == Qt::Unchecked);
        }

        it++;
    }
}

void instDialogObjectListTest::verifyInstallCheckboxes(QTreeWidget *table,
                                                       int items)
{
    if (items != -1)
        Q_ASSERT(table->topLevelItemCount() == items);


    QTreeWidgetItemIterator it(table, QTreeWidgetItemIterator::Enabled);
    while (*it)
    {
        if ((*it)->text(0) == "cluster1")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(INSTALL_CHECKBOX_COLUMN) == Qt::Unchecked);
        }

        if ((*it)->text(0) == "test1")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(INSTALL_CHECKBOX_COLUMN) == Qt::Checked);
        }

        if ((*it)->text(0) == "test2")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(INSTALL_CHECKBOX_COLUMN) == Qt::Checked);
        }

        if ((*it)->text(0) == "test3")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(INSTALL_CHECKBOX_COLUMN) == Qt::Checked);
        }

        if ((*it)->text(0) == "test4")
        {
            QVERIFY(((*it)->flags() & Qt::ItemIsUserCheckable) != 0);
            QVERIFY((*it)->checkState(INSTALL_CHECKBOX_COLUMN) == Qt::Checked);
        }

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
    QMenu *menu = nullptr;
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

/*
 * This function finds and activates an item with given name in the
 * context menu. If item is absent in the menu or is disabled, it
 * fails the test.
 */
void instDialogObjectListTest::openContextMenu(ObjectManipulator *om,
                                               ObjectTreeViewItem *item, ObjectTreeView *tree,
                                               const QString &actionText)
{
    QTimer::singleShot(100, this, SLOT(closeContextMenu()));
    om->contextMenuRequested(findItemPos(item, tree));
    bool found_menu_item = false;
    QMenu *menu = nullptr;
    foreach(QWidget *w, QApplication::allWidgets())
    {
        if (w->objectName() == "objectTreeContextMenu")
        {
            menu = dynamic_cast<QMenu*>(w);
            break;
        }
    }
    QVERIFY(menu != nullptr);
    foreach (QObject *act, menu->children())
    {
        QAction *action = dynamic_cast<QAction*>(act);
        if (action == nullptr) continue;
        if (action->text() == actionText)
        {
            QVERIFY(action->isEnabled() == true);
            action->activate(QAction::Trigger);
            found_menu_item = true;
            break;
        }
    }
    QVERIFY2(found_menu_item == true,
             QString("Item %1 not found in the context menu").arg(actionText).toLatin1().constData());
}



/*
 * user clicks "compile" button in the topmost toolbar. The list
 * should include all firewalls, but only those that require
 * recompile should have "compile" checkboxes checked.
 */

void instDialogObjectListTest::test_compile_1()
{
    QAction *compile = mw->findChild<QAction*>("compileAction");
    compile->activate(QAction::Trigger);
    QTest::qWait(100);
    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->isColumnHidden(COMPILE_CHECKBOX_COLUMN) == false);
    QVERIFY(table->isColumnHidden(INSTALL_CHECKBOX_COLUMN) == true);
    verifyCompileCheckboxes(table, 3);

    QTest::qWait(100);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(100);
}

/*
 * user uses main menu "Rules/Compile". This is the same function as
 * the previous one
 */
void instDialogObjectListTest::test_compile_2()
{
    QMenu *rules = mw->menuBar()->findChild<QMenu*>("RulesMenu");
    QAction *compile = nullptr;

    foreach(QAction *itm, rules->actions())
        if (itm->objectName() == "compileAction")
            compile = dynamic_cast<QAction*>(itm);
    QVERIFY(compile != nullptr);
    compile->activate(QAction::Trigger);
    QTest::qWait(100);
    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->isColumnHidden(COMPILE_CHECKBOX_COLUMN) == false);
    QVERIFY(table->isColumnHidden(INSTALL_CHECKBOX_COLUMN) == true);
    verifyCompileCheckboxes(table, 3);

    QTest::qWait(100);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(100);
}

/*
 * user opens Policy of the firewall "test1" and clicks button
 * "Compile" in the toolbar right above rule set view. The list should
 * include only firewall "test1" and checkbox "Compile" should be checked
 * because it requires recompile
 */
void instDialogObjectListTest::test_compile_3()
{
    openPolicy("test1");
    QTest::qWait(10);
    QToolButton* compileThis = mw->activeProject()->findChild<QToolButton*>("compile_this_fw");
    QVERIFY(compileThis != nullptr);
    QTest::mouseClick (compileThis, Qt::LeftButton);
    QTest::qWait(100);
    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->isColumnHidden(COMPILE_CHECKBOX_COLUMN) == false);
    QVERIFY(table->isColumnHidden(INSTALL_CHECKBOX_COLUMN) == true);
    QVERIFY(table->topLevelItemCount() == 1);
    verifyCompileCheckboxes(table, 1);

    QTest::qWait(100);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(100);
}

/*
 * the same test as the previous one, but with firewall test2. This
 * one does not require recompile and checkbox "Compile" should not
 * be checked.
 */
void instDialogObjectListTest::test_compile_4()
{
    openPolicy("test2");
    QTest::qWait(100);
    QToolButton* compileThis = mw->activeProject()->findChild<QToolButton*>("compile_this_fw");
    QVERIFY(compileThis != nullptr);
    QTest::mouseClick (compileThis, Qt::LeftButton);
    QTest::qWait(100);
    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->isColumnHidden(COMPILE_CHECKBOX_COLUMN) == false);
    QVERIFY(table->isColumnHidden(INSTALL_CHECKBOX_COLUMN) == true);
    verifyCompileCheckboxes(table, 1);

    QTest::qWait(100);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(100);
}

/*
 * user selects firewall test1 in the tree and opens context menu,
 * then clicks item "Compile". This is the same test as 3.
*/

void instDialogObjectListTest::test_compile_5()
{
    ObjectTreeView *tree = mw->activeProject()->getCurrentObjectTree();
    tree->expandAll();
    ObjectTreeViewItem *test1 =
        dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    ObjectTreeViewItem *test2 =
        dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test2", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    tree->scrollToItem(test1);  // <<<<<<<<<<<<<<
    tree->selectionModel()->select(tree->indexAt(findItemPos(test1, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(test1);
    ObjectManipulator *om = mw->activeProject()->findChild<ObjectManipulator*>("om");
    openContextMenu(om, test1, tree, "Compile");
    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->isColumnHidden(COMPILE_CHECKBOX_COLUMN) == false);
    QVERIFY(table->isColumnHidden(INSTALL_CHECKBOX_COLUMN) == true);
    verifyCompileCheckboxes(table, 1);

    QTest::qWait(100);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(100);
}

/*
 * user selects firewall test2 in the tree and opens context menu,
 * then clicks item "Compile". This is the same test as 4.
 */
void instDialogObjectListTest::test_compile_6()
{
    ObjectTreeView *tree = mw->activeProject()->getCurrentObjectTree();
    tree->expandAll();
    ObjectTreeViewItem *test1 =
        dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    ObjectTreeViewItem *test2 =
        dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test2", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    tree->scrollToItem(test2);  // <<<<<<<<<<<<<<
    tree->selectionModel()->select(tree->indexAt(findItemPos(test2, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(test2);
    ObjectManipulator *om = mw->activeProject()->findChild<ObjectManipulator*>("om");
    openContextMenu(om, test2, tree, "Compile");
    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->isColumnHidden(COMPILE_CHECKBOX_COLUMN) == false);
    QVERIFY(table->isColumnHidden(INSTALL_CHECKBOX_COLUMN) == true);
    verifyCompileCheckboxes(table, 1);

    QTest::qWait(100);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(100);
}

/*
 * Select two firewalls in the tree (test1 and test2), open context
 * menu and click "Compile". Should get a list with both firewalls, with
 * checkbox "Compile" selected for test2 and not selected for test1.
 */
void instDialogObjectListTest::test_compile_7()
{
    ObjectTreeView *tree = mw->activeProject()->getCurrentObjectTree();
    tree->expandAll();
    ObjectTreeViewItem *test1 =
        dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    ObjectTreeViewItem *test2 =
        dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test2", Qt::MatchExactly | Qt::MatchRecursive, 0).first());

    tree->scrollToItem(test1);
    tree->selectionModel()->select(
        tree->indexAt(findItemPos(test1, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(test1);
    tree->selectionModel()->select(
        tree->indexAt(findItemPos(test2, tree)), QItemSelectionModel::Select);

    ObjectManipulator *om = mw->activeProject()->findChild<ObjectManipulator*>("om");
    openContextMenu(om, test2, tree, "Compile");
    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->isColumnHidden(COMPILE_CHECKBOX_COLUMN) == false);
    QVERIFY(table->isColumnHidden(INSTALL_CHECKBOX_COLUMN) == true);
    verifyCompileCheckboxes(table, 2);

    QTest::qWait(100);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(100);
}


/*
 * all previous tests tested "Compile" function. This test should test
 * first step of the "Install" function.
 */
void instDialogObjectListTest::test_install_1()
{
    QAction *compile = mw->findChild<QAction*>("installAction");
    compile->activate(QAction::Trigger);
    QTest::qWait(100);
    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->isColumnHidden(COMPILE_CHECKBOX_COLUMN) == false);
    QVERIFY(table->isColumnHidden(INSTALL_CHECKBOX_COLUMN) == false);
    verifyCompileCheckboxes(table, 3);
    verifyInstallCheckboxes(table, 3);

    QTest::qWait(100);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(100);
}



/*
 * Select two firewalls in the tree (test1 and test2), open context
 * menu and click "Compile". Should get a list with both firewalls,
 * with checkbox "Compile" selected for test2 and not selected for
 * test1. Then click "Next" to compile, wait until done, then click
 * "Finish". Select the same firewalls in the tree and open instDialog
 * again, now both checkboxes should be turned off.
 * 
 * This test changes "last_compiled" timestamp of the firewall object
 * test2 so it should run after all tests above because it changes the
 * state of the "compile" checkbox next to this firewall in the list.
 */
void instDialogObjectListTest::test_actually_compile_1()
{
    if (QFileInfo("test1.fw").exists())
        QVERIFY(QFile("test1.fw").remove());

    ObjectTreeView *tree = mw->activeProject()->getCurrentObjectTree();
    tree->expandAll();
    ObjectTreeViewItem *test1 =
        dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    ObjectTreeViewItem *test2 =
        dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test2", Qt::MatchExactly | Qt::MatchRecursive, 0).first());

    tree->scrollToItem(test1);
    tree->selectionModel()->select(
        tree->indexAt(findItemPos(test1, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(test1);
    tree->selectionModel()->select(
        tree->indexAt(findItemPos(test2, tree)), QItemSelectionModel::Select);

    ObjectManipulator *om = mw->activeProject()->findChild<ObjectManipulator*>("om");
    openContextMenu(om, test2, tree, "Compile");
    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->isColumnHidden(COMPILE_CHECKBOX_COLUMN) == false);
    QVERIFY(table->isColumnHidden(INSTALL_CHECKBOX_COLUMN) == true);
    verifyCompileCheckboxes(table, 2);

    QTest::qWait(100);
    dlg->findChild<QPushButton*>("nextButton")->click();

    QPushButton *finish = dlg->findChild<QPushButton*>("finishButton");
    QVERIFY(finish != nullptr);

    int timeout_counter = 0;
    while (!finish->isEnabled())
    {
        timeout_counter++;
        QVERIFY2(timeout_counter < 600,
                 "Compile takes too long (over 1 min) or button \"Finish\" "
                 "is not enabled properly when compile is done");
        QTest::qWait(100);
    }

    finish->click();

    // Now select the same firewalls and open compile/install dialog again
    tree->scrollToItem(test1);
    tree->selectionModel()->select(
        tree->indexAt(findItemPos(test1, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(test1);
    tree->selectionModel()->select(
        tree->indexAt(findItemPos(test2, tree)), QItemSelectionModel::Select);

    openContextMenu(om, test2, tree, "Compile");
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);

    table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->isColumnHidden(COMPILE_CHECKBOX_COLUMN) == false);
    QVERIFY(table->isColumnHidden(INSTALL_CHECKBOX_COLUMN) == true);
    verifyCompileCheckboxes_2(table);

    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(100);

    QVERIFY(QFileInfo("test1.fw").exists() && QFileInfo("test1.fw").size());
    QFile::remove("test1.fw");
}

