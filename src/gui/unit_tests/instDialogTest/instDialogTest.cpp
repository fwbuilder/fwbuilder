#include "instDialogTest.h"


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

/*
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
    QTreeWidgetItemIterator it(table, QTreeWidgetItemIterator::Enabled);
    while (*it)
    {
        if ((*it)->text(0) == "cluster1") QVERIFY((*it)->checkState(1) == Qt::Checked);
        if ((*it)->text(0) == "cluster1") QVERIFY((*it)->checkState(2) == Qt::Unchecked);
        if ((*it)->text(0) == "test1") QVERIFY((*it)->checkState(1) == Qt::Unchecked);
        if ((*it)->text(0) == "test1") QVERIFY((*it)->checkState(2) == Qt::Checked);
        if ((*it)->text(0) == "test2") QVERIFY((*it)->checkState(1) == Qt::Checked);
        if ((*it)->text(0) == "test2") QVERIFY((*it)->checkState(2) == Qt::Checked);
        if ((*it)->text(0) == "test3") QVERIFY((*it)->checkState(2) == Qt::Checked);
        if ((*it)->text(0) == "test4") QVERIFY((*it)->checkState(2) == Qt::Checked);
        if ((*it)->text(0) == "test3") QVERIFY((*it)->checkState(1) == Qt::Unchecked);
        if ((*it)->text(0) == "test4") QVERIFY((*it)->checkState(1) == Qt::Unchecked);
        it++;
    }
    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(1000);
}
*/

void instDialogTest::page1_9()
{
    ObjectTreeView *tree = mw->getCurrentObjectTree();
    tree->expandAll();
    ObjectTreeViewItem *test3 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test3", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    tree->scrollToItem(test3);
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

void instDialogTest::page1_10()
{
    ObjectTreeView *tree = mw->getCurrentObjectTree();
    tree->expandAll();
    ObjectTreeViewItem *cluster1 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("cluster1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    tree->scrollToItem(cluster1);
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

void instDialogTest::page1_11()
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
