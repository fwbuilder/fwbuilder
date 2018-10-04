/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: instDialogCompileTest.cpp 2786 2010-04-01 14:05:36Z a2k $

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

  Ticket 1357:

  before running the test check if files "test1.fw", "test2.fw", "test3.fw", "test4.fw" exist and delete them if they are there. If delete operation fails for any reason, fail the test
  click on the toolbar button "Compile"
  click "CompileALL"
  click "Next"
  let it run until it is done
  check that status in the list in the column on the left is "Success" for all objects: cluster1, test1 and test2
  check that it produced files "test1.fw", "test2.fw", "test3.fw", "test4.fw"
  check that each of these files has non-zero length
*/

#include "instDialogCompileTest.h"

#include "unistd.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QMenuBar>
#include <QApplication>
#include <QCoreApplication>
#include "FWObjectClipboard.h"
#include "FWBApplication.h"

using namespace std;
using namespace QTest;
using namespace libfwbuilder;

void instDialogCompileTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->loadFile("test_work.fwb", false);
}

bool checkProgress(QTreeWidget *list)
{
    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        if ( (list->topLevelItem(i)->text(1).toStdString() == "Compiling ...") ||
             (list->topLevelItem(i)->text(1).toStdString() == "Installing ...") )
            return false;
    }
    return true;
}

QPoint findItemPos(ObjectTreeViewItem *item, ObjectTreeView *tree)
{
     QRect rect = tree->visualItemRect(item);
     return QPoint(rect.x() + 1, rect.y() + 1);
}

void instDialogCompileTest::closeContextMenu()
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
void instDialogCompileTest::openContextMenu(ObjectManipulator *om,
                                            ObjectTreeViewItem *item,
                                            ObjectTreeView *tree,
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


void instDialogCompileTest::testSelectButtonsVisibility()
{
    ObjectManipulator *om = mw->activeProject()->findChild<ObjectManipulator*>("om");
    ObjectTreeView *tree = om->getCurrentObjectTree();

    tree->expandAll();

    ObjectTreeViewItem *test1 =
        dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    ObjectTreeViewItem *test2 =
        dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test2", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    ObjectTreeViewItem *cluster1 =
        dynamic_cast<ObjectTreeViewItem*>(tree->findItems("cluster1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());


    // case when compiling only one firewall: buttons should not be visible
    tree->clearSelection();

    tree->scrollToItem(test1);

    test1->setSelected(true);
    tree->setCurrentItem(test1);

    //QTest::qWait(2000);

    openContextMenu(om, test1, tree, "Compile");

    instDialog *dlg = nullptr;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != nullptr)
            dlg = dynamic_cast<instDialog*>(w);
    QFrame *selectFrame = dlg->findChild<QFrame*>("selectAllNoneFrame");

    QTest::qWait(1000);
    QVERIFY(selectFrame->isHidden());
    dlg->reject();

    // case when compiling more than one firewall: button should be visible

    tree->clearSelection();

    tree->scrollToItem(test1);

    test1->setSelected(true);
    tree->setCurrentItem(test1);
    test2->setSelected(true);

    openContextMenu(om, test1, tree, "Compile");

    //QTest::qWait(2000);


    QTest::qWait(1000);
    QVERIFY(!selectFrame->isHidden());
    dlg->reject();

    // case when compiling cluster: buttons should be visible

    tree->clearSelection();

    tree->scrollToItem(cluster1);

    cluster1->setSelected(true);
    tree->setCurrentItem(cluster1);

    openContextMenu(om, cluster1, tree, "Compile");

    QTest::qWait(1000);
    QVERIFY(!selectFrame->isHidden());
    dlg->reject();

    // case when compiling one firewall and one cluster: buttons should be visible

    tree->clearSelection();

    tree->scrollToItem(cluster1);

    cluster1->setSelected(true);
    tree->setCurrentItem(cluster1);
    test2->setSelected(true);

    openContextMenu(om, cluster1, tree, "Compile");

    QTest::qWait(1000);
    QVERIFY(!selectFrame->isHidden());
    dlg->reject();
}

void instDialogCompileTest::testCompile()
{
    if (QFileInfo("test1.fw").exists())
        QVERIFY(QFile("test1.fw").remove());
    if (QFileInfo("test2.fw").exists())
        QVERIFY(QFile("test2.fw").remove());
    if (QFileInfo("test3.fw").exists())
        QVERIFY(QFile("test3.fw").remove());
    if (QFileInfo("test4.fw").exists())
        QVERIFY(QFile("test4.fw").remove());
    if (QFileInfo("pf firewall.fw").exists())
        QVERIFY(QFile("pf firewall.fw").remove());
    if (QFileInfo("ipfilter firewall.fw").exists())
        QVERIFY(QFile("ipfilter firewall.fw").remove());

    mw->findChild<QAction*>("compileAction")->trigger();

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton16")->click();
    dlg->findChild<QPushButton*>("nextButton")->click();

    QTreeWidget *list = dlg->findChild<QTreeWidget*>("fwWorkList");
    QPushButton *back = dlg->findChild<QPushButton*>("backButton");
    QPushButton *next = dlg->findChild<QPushButton*>("nextButton");
    QPushButton *finish = dlg->findChild<QPushButton*>("finishButton");
    QPushButton *cancel = dlg->findChild<QPushButton*>("cancelButton");

    int waited = 0;

    while (!checkProgress(list))
    {
        // test state of the buttons
        QVERIFY(back->isEnabled() == true);
        QVERIFY(next->isEnabled() == false);
        QVERIFY(cancel->isEnabled() == true);
        QVERIFY(finish->isEnabled() == false);

        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Success");
    }

    // test state of the buttons
    QVERIFY(back->isEnabled() == true);
    QVERIFY(next->isEnabled() == false);
    QVERIFY(cancel->isEnabled() == true);
    QVERIFY(finish->isEnabled() == true);

    dlg->reject();

    QVERIFY(QFileInfo("test1.fw").exists() && QFileInfo("test1.fw").size());
    QVERIFY(QFileInfo("test2.fw").exists() && QFileInfo("test2.fw").size());
    QVERIFY(QFileInfo("test3.fw").exists() && QFileInfo("test3.fw").size());
    QVERIFY(QFileInfo("test4.fw").exists() && QFileInfo("test4.fw").size());
    QVERIFY(QFileInfo("pf firewall.fw").exists() &&
            QFileInfo("pf firewall.conf").exists() &&
            QFileInfo("pf firewall.fw").size() &&
            QFileInfo("pf firewall.conf").size());
    QVERIFY(QFileInfo("ipfilter firewall.fw").exists() &&
            QFileInfo("ipfilter firewall-ipf.conf").exists() &&
            QFileInfo("ipfilter firewall-nat.conf").exists() &&
            QFileInfo("ipfilter firewall.fw").size() &&
            QFileInfo("ipfilter firewall-ipf.conf").size() &&
            QFileInfo("ipfilter firewall-nat.conf").size());
    QFile::remove("test1.fw");
    QFile::remove("test2.fw");
    QFile::remove("test3.fw");
    QFile::remove("test4.fw");
    QFile::remove("pf firewall.fw");
    QFile::remove("pf firewall.conf");
    QFile::remove("ipfilter firewall.fw");
    QFile::remove("ipfilter firewall-ipf.conf");
    QFile::remove("ipfilter firewall-nat.conf");
}

