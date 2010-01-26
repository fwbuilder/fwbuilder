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
    mw->loadFile("test.fwb", false);
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

void instDialogTest::page1_5()
{
    ObjectTreeView *tree = mw->getCurrentObjectTree();
    tree->expandAll();
    ObjectTreeViewItem *test1 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    ObjectTreeViewItem *test2 = dynamic_cast<ObjectTreeViewItem*>(tree->findItems("test2", Qt::MatchExactly | Qt::MatchRecursive, 0).first());
    tree->scrollToItem(test1);
    tree->selectionModel()->select(tree->indexAt(findItemPos(test1, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    ObjectManipulator *om = mw->findChild<ObjectManipulator*>("om");
    om->compile();
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
    tree->scrollToItem(test1);
    tree->selectionModel()->select(tree->indexAt(findItemPos(test2, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    ObjectManipulator *om = mw->findChild<ObjectManipulator*>("om");
    om->compile();
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
    tree->selectionModel()->select(tree->indexAt(findItemPos(test2, tree)), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    tree->selectionModel()->select(tree->indexAt(findItemPos(test1, tree)), QItemSelectionModel::Select);
    ObjectManipulator *om = mw->findChild<ObjectManipulator*>("om");
    om->compile();
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
        if ((*it)->text(0) == "test2") QVERIFY((*it)->checkState(1) == Qt::Checked);
        it++;
    }
    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QTest::qWait(1000);
}
