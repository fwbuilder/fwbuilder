/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: instDialogClusterTest.cpp 2786 2010-04-01 14:05:36Z a2k $

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

  Tests for clusters:

  * when user clicks "install", checkboxes "install" should appear only
  next to the firewalls and should not appear next to the clusters in
  the list

  Ticket 1308:

  test2:

  * select cluster "cluster1" in the tree and use context menu to
  compile it

  * check that the list has objects "cluster1", "test3" and "test4"

  * check that the warning message is hidden

  test3:

  * regardless of the selection in the tree, simulate clicking on the
  main toolbar button "Compile"

  * check that the list includes objects "cluster1", "test3", "test4",
  "test1", "test2" and the warning message is hidden

  the focus of these tests is on the warning message. We verify
  settings of the "Compile" checkboxes in other tests.

*/

#include "instDialogClusterTest.h"

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
#include "StartTipDialog.h"


using namespace std;
using namespace QTest;
using namespace libfwbuilder;

void instDialogClusterTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();

    QTest::qWait(5000);

    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    if (d!=nullptr) d->close();

    mw->loadFile("test_work.fwb", false);

    QTest::qWait(1000);

    ObjectManipulator *om = mw->activeProject()->findChild<ObjectManipulator*>("om");

    FWObject *user_lib = mw->db()->findObjectByName(Library::TYPENAME, "User");
    QVERIFY(user_lib != nullptr);

    om->openLibForObject(user_lib);

    FWObject *cl = om->getCurrentLib();

    QVERIFY(cl != nullptr);
    QVERIFY(cl == user_lib);
}

void instDialogClusterTest::openPolicy(QString fwname)
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

void instDialogClusterTest::verifyDialog(instDialog *dlg, int items)
{
    QVERIFY(dlg != nullptr);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
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

void instDialogClusterTest::closeContextMenu()
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

void instDialogClusterTest::openContextMenu(ObjectManipulator *om,
                                            ObjectTreeViewItem *item,
                                            ObjectTreeView *tree,
                                            const QString &actionText)
{
    QTimer::singleShot(100, this, SLOT(closeContextMenu()));
    QPoint item_pos = findItemPos(item, tree);
    om->contextMenuRequested(item_pos);

    QMenu *menu = nullptr;
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
        if (action == nullptr) continue;
        if (action->text() == actionText)
        {
            action->activate(QAction::Trigger);
            break;
        }
    }
}

void instDialogClusterTest::page1_8()
{
    ObjectTreeView *tree = mw->getCurrentObjectTree();
    QVERIFY(tree != nullptr);

    ObjectTreeViewItem *test3 = dynamic_cast<ObjectTreeViewItem*>(
        tree->findItems(
            "test3", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    QVERIFY(test3 != nullptr);

    tree->selectionModel()->select(
        tree->indexAt(findItemPos(test3, tree)),
        QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(test3);

    ObjectManipulator *om = mw->activeProject()->findChild<ObjectManipulator*>("om");

    openContextMenu(om, test3, tree, "Compile");

    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);
    QVERIFY(dlg != nullptr);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
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

void instDialogClusterTest::page1_9()
{
    ObjectTreeView *tree = mw->getCurrentObjectTree();
    ObjectTreeViewItem *cluster1 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("cluster1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    tree->selectionModel()->select(tree->indexAt(findItemPos(cluster1, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(cluster1);
    ObjectManipulator *om = mw->activeProject()->findChild<ObjectManipulator*>("om");
    openContextMenu(om, cluster1, tree, "Compile");
    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);
    QVERIFY(dlg != nullptr);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->topLevelItemCount() == 1);
    QVERIFY(table->topLevelItem(0)->text(0) == "cluster1");
    QVERIFY(table->topLevelItem(0)->childCount() == 2);
    QVERIFY(table->topLevelItem(0)->child(0)->text(0) == "test3");
    QVERIFY(table->topLevelItem(0)->child(1)->text(0) == "test4");

    QFrame *warning_space = dlg->findChild<QFrame*>("warning_space");
    QVERIFY(warning_space->isHidden());

    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(1000);
}

void instDialogClusterTest::page1_10()
{
    QAction *compile = mw->findChild<QAction*>("compileAction");
    compile->activate(QAction::Trigger);
    QTest::qWait(100);
    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);
    QVERIFY(dlg != nullptr);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->topLevelItemCount() == 3);
    QVERIFY(! table->findItems("cluster1", Qt::MatchExactly, 0).isEmpty());
    QVERIFY(! table->findItems("test1", Qt::MatchExactly, 0).isEmpty());
    QVERIFY(! table->findItems("test2", Qt::MatchExactly, 0).isEmpty());
    QVERIFY(! table->findItems("test3", Qt::MatchExactly | Qt::MatchRecursive, 0).isEmpty());
    QVERIFY(! table->findItems("test4", Qt::MatchExactly | Qt::MatchRecursive, 0).isEmpty());
    QFrame *warning_space = dlg->findChild<QFrame*>("warning_space");
    QVERIFY(warning_space->isHidden());
    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(1000);

}

void instDialogClusterTest::page1_11()
{
    QAction *compile = mw->findChild<QAction*>("installAction");
    compile->activate(QAction::Trigger);
    QTest::qWait(100);
    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);
    QVERIFY(dlg != nullptr);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != nullptr);
    QVERIFY(table->topLevelItemCount() == 3);
    //QTest::qWait(100000);
    for (int i=0; i< table->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = table->topLevelItem(i);
        if (item->text(0) == "cluster1")
        {
            QVERIFY(item->checkState(2) == Qt::Unchecked);
            QVERIFY(item->checkState(1) == Qt::Checked);
            QVERIFY(item->child(0)->checkState(1) == Qt::Unchecked);
            QVERIFY(item->child(0)->checkState(2) == Qt::Checked);
            QVERIFY(item->child(1)->checkState(1) == Qt::Unchecked);
            QVERIFY(item->child(1)->checkState(2) == Qt::Checked);
        }
        if (item->text(0) == "test1")
        {
            QVERIFY(item->checkState(1) == Qt::Unchecked);
            QVERIFY(item->checkState(2) == Qt::Checked);
        }
        if (item->text(0) == "test2")
        {
            QVERIFY(item->checkState(1) == Qt::Checked);
            QVERIFY(item->checkState(2) == Qt::Checked);
        }

    }
    QTest::qWait(1000);
    //dlg->findChild<QPushButton*>("cancelButton")->click();
    dlg->reject();
    QTest::qWait(1000);
}
