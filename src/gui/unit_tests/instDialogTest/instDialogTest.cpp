/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

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

#include "instDialogTest.h"

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

void instDialogTest::initTestCase()
{
    mw = new FWWindow();
    mw->show();
    mw->loadFile("test_work.fwb", false);
    FWObjectClipboard *clip = new FWObjectClipboard();
    ssh_auth_sock = getenv("SSH_AUTH_SOCK");
}

void instDialogTest::cleanupTestCase()
{
    setenv("SSH_AUTH_SOCK", ssh_auth_sock, 1);
    QDir().rmdir("test_install");
    QFile::remove("test1.fw");
}

void instDialogTest::openPolicy(QString fwname)
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

void instDialogTest::verifyDialog(instDialog *dlg, int items)
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

void instDialogTest::closeContextMenu()
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

void instDialogTest::openContextMenu(ObjectManipulator *om, ObjectTreeViewItem *item, ObjectTreeView *tree, const QString &actionText)
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

void instDialogTest::page1_1()
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

void instDialogTest::page1_2()
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

void instDialogTest::page1_3()
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

void instDialogTest::page1_4()
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

void instDialogTest::page1_5()
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

void instDialogTest::page1_6()
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

void instDialogTest::page1_7()
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

void instDialogTest::page1_8()
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

void instDialogTest::page1_9()
{
    ObjectTreeView *tree = mw->getCurrentObjectTree();
    ObjectTreeViewItem *cluster1 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("cluster1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    tree->selectionModel()->select(tree->indexAt(findItemPos(cluster1, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->setCurrentItem(cluster1);
    ObjectManipulator *om = mw->findChild<ObjectManipulator*>("om");
    openContextMenu(om, cluster1, tree, "Compile");
    instDialog *dlg = NULL;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != NULL)
            dlg = dynamic_cast<instDialog*>(w);
    QVERIFY(dlg != NULL);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != NULL);
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

void instDialogTest::page1_10()
{
    QAction *compile = mw->findChild<QAction*>("compileAction");
    compile->activate(QAction::Trigger);
    QTest::qWait(100);
    instDialog *dlg = NULL;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != NULL)
            dlg = dynamic_cast<instDialog*>(w);
    QVERIFY(dlg != NULL);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != NULL);
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

void instDialogTest::page1_11()
{
    QAction *compile = mw->findChild<QAction*>("installAction");
    compile->activate(QAction::Trigger);
    QTest::qWait(100);
    instDialog *dlg = NULL;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != NULL)
            dlg = dynamic_cast<instDialog*>(w);
    QVERIFY(dlg != NULL);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != NULL);
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

void instDialogTest::testCompile()
{
    if (QFileInfo("test1.fw").exists())
        QVERIFY(QFile("test1.fw").remove());
    if (QFileInfo("test2.fw").exists())
        QVERIFY(QFile("test2.fw").remove());
    if (QFileInfo("test3.fw").exists())
        QVERIFY(QFile("test3.fw").remove());
    if (QFileInfo("test4.fw").exists())
        QVERIFY(QFile("test4.fw").remove());

    mw->findChild<QAction*>("compileAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton16")->click();
    QTest::qWait(500);
    dlg->findChild<QPushButton*>("nextButton")->click();

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");

    int waited = 0;

    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Success");
    }
    dlg->reject();

    QVERIFY(QFileInfo("test1.fw").exists() && QFileInfo("test1.fw").size());
    QVERIFY(QFileInfo("test2.fw").exists() && QFileInfo("test2.fw").size());
    QVERIFY(QFileInfo("test3.fw").exists() && QFileInfo("test3.fw").size());
    QVERIFY(QFileInfo("test4.fw").exists() && QFileInfo("test4.fw").size());
    QFile::remove("test1.fw");
    QFile::remove("test2.fw");
    QFile::remove("test3.fw");
    QFile::remove("test4.fw");
    QTest::qWait(1000);
}

void instDialogTest::removeFiles()
{
    if (QFileInfo("test1.fw").exists())
        QVERIFY(QFile::remove("test1.fw"));
    if (QFileInfo("test2.fw").exists())
        QVERIFY(QFile::remove("test2.fw"));
    if (QFileInfo("test3.fw").exists())
        QVERIFY(QFile::remove("test3.fw"));
    if (QFileInfo("test4.fw").exists())
        QVERIFY(QFile::remove("test4.fw"));
    if (QDir("test_install").exists())
    {
        foreach(QString file, QDir("test_install").entryList())
        {
            if (QFileInfo("test_install/"+file).isFile())
                QVERIFY(QFile::remove("test_install/"+file));
        }
        QVERIFY(QDir().rmdir("test_install"));
    }
    QVERIFY(QDir().mkdir("test_install"));
}

void instDialogTest::testInstall1()
{
    removeFiles();
    Firewall *test1 = NULL;
    foreach(FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == "test1")
        {
            test1 = Firewall::cast(fw);
            break;
        }
    }
    QVERIFY(test1 != NULL);
    FWOptions *fwoptions = test1->getOptionsObject();
    fwoptions->setStr("firewall_dir", (QDir::currentPath()+"/test_install").toStdString());

    mw->findChild<QAction*>("installAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton17")->click();

    QTreeWidget *selectTable = dlg->findChild<QTreeWidget*>("selectTable");
    QTreeWidgetItem *test1item = selectTable->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first();
    test1item->setCheckState(1, Qt::Checked);
    test1item->setCheckState(2, Qt::Checked);
    QTest::qWait(500);
    dlg->findChild<QPushButton*>("nextButton")->click();

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");

    int waited = 0;

    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Success");
    }
    QTest::qWait(500);

    QVERIFY(dlg->findChild<QPushButton*>("nextButton")->isEnabled());
    QVERIFY(!dlg->findChild<QPushButton*>("finishButton")->isEnabled());

    QVERIFY(QFile::exists("test1.fw"));
    QVERIFY(QFile::remove("test1.fw"));
    QFile testfile("test1.fw");
    testfile.open(QFile::WriteOnly);
    testfile.write("#!/bin/sh\n#\n#  This is automatically generated file. DO NOT MODIFY !\n#\n#  Firewall Builder  fwb_ipt v4.0.0-2784\n#\n#  Generated Wed Mar 31 16:41:46 2010 EEST by a2k\n#\n# files: * test1.fw\n#\n# Compiled for iptables (any version)\n#\n# This firewall has two interfaces. Eth0 faces outside and has a dynamic address; eth1 faces inside.\n# Policy includes basic rules to permit unrestricted outbound access and anti-spoofing rules. Access to the firewall is permitted only from internal network and only using SSH. The firewall uses one of the machines on internal network for DNS. Internal network is configured with address 192.168.1.0/255.255.255.0\n\n\necho \"Testing policy activation script\"\n");
    testfile.close();
    testfile.setPermissions(testfile.permissions() | QFile::ExeOwner);

    QTimer::singleShot(200, this, SLOT(testInstall1_part2()));
    dlg->findChild<QPushButton*>("nextButton")->click();
    // now runs testInstall_part2 and fills install options dialog
    waited = 0;
    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Success");
    }

    QString text = dlg->findChild<QTextBrowser*>("procLogDisplay")->toPlainText();
    QVERIFY(text.contains("Testing policy activation script"));
    QVERIFY(!text.contains("*** Fatal error"));
    QVERIFY(!text.isEmpty());
    dlg->reject();
    QTest::qWait(1000);
}

void instDialogTest::testInstall1_part2() // sets install options in modal dialog
{
    instOptionsDialog *optdlg = mw->findChild<instOptionsDialog*>("instOptionsDialog_q");
    QLineEdit *uname = optdlg->findChild<QLineEdit*>("uname");
    QLineEdit *pwd = optdlg->findChild<QLineEdit*>("pwd");
    QLineEdit *altAddress = optdlg->findChild<QLineEdit*>("altAddress");
    uname->setText(getenv("USER"));
    altAddress->setText("127.0.0.1");
    optdlg->findChild<QPushButton*>("okButton")->click();
}

void instDialogTest::testInstall2()
{
    removeFiles();
    Firewall *test1 = NULL;
    foreach(FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == "test1")
        {
            test1 = Firewall::cast(fw);
            break;
        }
    }
    QVERIFY(test1 != NULL);
    FWOptions *fwoptions = test1->getOptionsObject();
    fwoptions->setStr("firewall_dir", (QDir::currentPath()+"/test_install").toStdString());

    setenv("SSH_AUTH_SOCK", "nothinghere", 1);
    QTest::qWait(500);
    mw->findChild<QAction*>("installAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton17")->click();

    QTreeWidget *selectTable = dlg->findChild<QTreeWidget*>("selectTable");
    QTreeWidgetItem *test1item = selectTable->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first();
    test1item->setCheckState(1, Qt::Checked);
    test1item->setCheckState(2, Qt::Checked);
    QTest::qWait(500);
    dlg->findChild<QPushButton*>("nextButton")->click();

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");

    int waited = 0;

    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Success");
    }
    QTest::qWait(1000);

    QVERIFY(dlg->findChild<QPushButton*>("nextButton")->isEnabled());
    QVERIFY(!dlg->findChild<QPushButton*>("finishButton")->isEnabled());

    QVERIFY(QFile::exists("test1.fw"));
    QVERIFY(QFile::remove("test1.fw"));
    QFile testfile("test1.fw");
    testfile.open(QFile::WriteOnly);
    testfile.write("#!/bin/sh\n#\n#  This is automatically generated file. DO NOT MODIFY !\n#\n#  Firewall Builder  fwb_ipt v4.0.0-2784\n#\n#  Generated Wed Mar 31 16:41:46 2010 EEST by a2k\n#\n# files: * test1.fw\n#\n# Compiled for iptables (any version)\n#\n# This firewall has two interfaces. Eth0 faces outside and has a dynamic address; eth1 faces inside.\n# Policy includes basic rules to permit unrestricted outbound access and anti-spoofing rules. Access to the firewall is permitted only from internal network and only using SSH. The firewall uses one of the machines on internal network for DNS. Internal network is configured with address 192.168.1.0/255.255.255.0\n\n\necho \"Testing policy activation script\"\n");
    testfile.close();
    testfile.setPermissions(testfile.permissions() | QFile::ExeOwner);

    QTimer::singleShot(200, this, SLOT(testInstall1_part2()));
    dlg->findChild<QPushButton*>("nextButton")->click();
    // now runs testInstall_part2 and fills install options dialog
    waited = 0;
    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    QTest::qWait(500);
    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Failure");
    }
    setenv("SSH_AUTH_SOCK", ssh_auth_sock, 1);
    QString text = dlg->findChild<QTextBrowser*>("procLogDisplay")->toPlainText();
    QVERIFY(!text.isEmpty());
    QVERIFY(text.contains("lost connection"));
    QVERIFY(text.contains("SSH session terminated, exit status: 1"));

    dlg->reject();
    QTest::qWait(1000);
}


void instDialogTest::testInstall3()
{
    removeFiles();
    Firewall *test1 = NULL;
    foreach(FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == "test1")
        {
            test1 = Firewall::cast(fw);
            break;
        }
    }
    QVERIFY(test1 != NULL);
    FWOptions *fwoptions = test1->getOptionsObject();
    fwoptions->setStr("firewall_dir", (QDir::currentPath()+"/test_install").toStdString());

    mw->findChild<QAction*>("installAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton17")->click();

    QTreeWidget *selectTable = dlg->findChild<QTreeWidget*>("selectTable");
    QTreeWidgetItem *test1item = selectTable->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first();
    test1item->setCheckState(1, Qt::Checked);
    test1item->setCheckState(2, Qt::Checked);
    QTest::qWait(500);
    dlg->findChild<QPushButton*>("nextButton")->click();

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");

    int waited = 0;

    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Success");
    }
    QTest::qWait(500);

    QVERIFY(dlg->findChild<QPushButton*>("nextButton")->isEnabled());
    QVERIFY(!dlg->findChild<QPushButton*>("finishButton")->isEnabled());

    QVERIFY(QFile::exists("test1.fw"));
    QVERIFY(QFile::remove("test1.fw"));
    QFile testfile("test1.fw");
    testfile.open(QFile::WriteOnly);
    testfile.write("#!/bin/sh\n#\n#  This is automatically generated file. DO NOT MODIFY !\n#\n#  Firewall Builder  fwb_ipt v4.0.0-2784\n#\n#  Generated Wed Mar 31 16:41:46 2010 EEST by a2k\n#\n# files: * test1.fw\n#\n# Compiled for iptables (any version)\n#\n# This firewall has two interfaces. Eth0 faces outside and has a dynamic address; eth1 faces inside.\n# Policy includes basic rules to permit unrestricted outbound access and anti-spoofing rules. Access to the firewall is permitted only from internal network and only using SSH. The firewall uses one of the machines on internal network for DNS. Internal network is configured with address 192.168.1.0/255.255.255.0\n\n\necho \"Testing policy activation script\"\n");
    testfile.close();
    testfile.setPermissions(testfile.permissions() | QFile::ExeOwner);

    QTimer::singleShot(200, this, SLOT(testInstall3_part2()));
    dlg->findChild<QPushButton*>("nextButton")->click();
    // now runs testInstall_part2 and fills install options dialog
    waited = 0;
    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 1000000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Failure");
    }

    QString text = dlg->findChild<QTextBrowser*>("procLogDisplay")->toPlainText();
    QVERIFY(text.contains("lost connection"));
    QVERIFY(text.contains("SSH session terminated, exit status: 1"));
    QVERIFY(!text.isEmpty());
    dlg->reject();
    QTest::qWait(1000);
}

void instDialogTest::testInstall3_part2() // sets install options in modal dialog
{
    instOptionsDialog *optdlg = mw->findChild<instOptionsDialog*>("instOptionsDialog_q");
    QLineEdit *uname = optdlg->findChild<QLineEdit*>("uname");
    QLineEdit *pwd = optdlg->findChild<QLineEdit*>("pwd");
    QLineEdit *altAddress = optdlg->findChild<QLineEdit*>("altAddress");
    uname->setText(getenv("USER"));
    altAddress->setText("1.2.3.4");
    optdlg->findChild<QPushButton*>("okButton")->click();
}



void instDialogTest::testInstall4()
{
    removeFiles();
    Firewall *test1 = NULL;
    foreach(FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == "test1")
        {
            test1 = Firewall::cast(fw);
            break;
        }
    }
    QVERIFY(test1 != NULL);
    FWOptions *fwoptions = test1->getOptionsObject();
    fwoptions->setStr("firewall_dir", (QDir::currentPath()+"/test_install").toStdString());

    mw->findChild<QAction*>("installAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton17")->click();

    QTreeWidget *selectTable = dlg->findChild<QTreeWidget*>("selectTable");
    QTreeWidgetItem *test1item = selectTable->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first();
    test1item->setCheckState(1, Qt::Checked);
    test1item->setCheckState(2, Qt::Checked);
    QTest::qWait(500);
    dlg->findChild<QPushButton*>("nextButton")->click();

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");

    int waited = 0;

    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Success");
    }
    QTest::qWait(500);

    QVERIFY(dlg->findChild<QPushButton*>("nextButton")->isEnabled());
    QVERIFY(!dlg->findChild<QPushButton*>("finishButton")->isEnabled());

    QVERIFY(QFile::exists("test1.fw"));
    QVERIFY(QFile::remove("test1.fw"));
    QFile testfile("test1.fw");
    testfile.open(QFile::WriteOnly);
    testfile.write("#!/bin/sh\n#\n#  This is automatically generated file. DO NOT MODIFY !\n#\n#  Firewall Builder  fwb_ipt v4.0.0-2784\n#\n#  Generated Wed Mar 31 16:41:46 2010 EEST by a2k\n#\n# files: * test1.fw\n#\n# Compiled for iptables (any version)\n#\n# This firewall has two interfaces. Eth0 faces outside and has a dynamic address; eth1 faces inside.\n# Policy includes basic rules to permit unrestricted outbound access and anti-spoofing rules. Access to the firewall is permitted only from internal network and only using SSH. The firewall uses one of the machines on internal network for DNS. Internal network is configured with address 192.168.1.0/255.255.255.0\n\n\necho \"Testing policy activation script\"\n");
    testfile.close();
    testfile.setPermissions(testfile.permissions() | QFile::ExeOwner);

    QTimer::singleShot(200, this, SLOT(testInstall3_part2()));
    dlg->findChild<QPushButton*>("nextButton")->click();
    // now runs testInstall_part2 and fills install options dialog
    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QString text = dlg->findChild<QTextBrowser*>("procLogDisplay")->toPlainText();
    QVERIFY(text.contains("Stopping background process"));
    QTest::qWait(3000);
    QVERIFY(mw->findChild<instDialog*>() == NULL); // check that dialog is closed
    QTest::qWait(1000);
}
