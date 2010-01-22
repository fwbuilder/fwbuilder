#include "instDialogTest.h"


#include <QDebug>
#include <QMenuBar>
#include <QApplication>
#include <QCoreApplication>

using namespace std;
using namespace libfwbuilder;

void instDialogTest::initTestCase()
{
    mw = new FWWindow();
    mw->show();
    mw->loadFile("test.fwb", false);
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

void instDialogTest::page1_1()
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
    QVERIFY(table->topLevelItemCount() == 2);

    QTreeWidgetItemIterator it(table, QTreeWidgetItemIterator::Enabled);
    while (*it)
    {
        if ((*it)->text(0) == "test1") QVERIFY((*it)->checkState(1) == Qt::Unchecked);
        if ((*it)->text(0) == "test2") QVERIFY((*it)->checkState(1) == Qt::Checked);
        it++;
    }
    dlg->reject();
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
    QVERIFY(dlg != NULL);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != NULL);
    QVERIFY(table->topLevelItemCount() == 2);

    QTreeWidgetItemIterator it(table, QTreeWidgetItemIterator::Enabled);
    while (*it)
    {
        if ((*it)->text(0) == "test1") QVERIFY((*it)->checkState(1) == Qt::Unchecked);
        if ((*it)->text(0) == "test2") QVERIFY((*it)->checkState(1) == Qt::Checked);
        it++;
    }
    dlg->reject();
}

void instDialogTest::page1_3()
{
    openPolicy("test1");
    QTest::qWait(100);
    QToolButton* compileThis = mw->findChild<QToolButton*>("compile_this_fw");
    QVERIFY(compileThis != NULL);
    QTest::mouseClick (compileThis, Qt::LeftButton);
    QTest::qWait(100);
    instDialog *dlg = NULL;
    foreach (QWidget *w, app->allWidgets())
        if (dynamic_cast<instDialog*>(w) != NULL)
            dlg = dynamic_cast<instDialog*>(w);
    QVERIFY(dlg != NULL);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != NULL);
    QVERIFY(table->topLevelItemCount() == 1);

    QTreeWidgetItemIterator it(table, QTreeWidgetItemIterator::Enabled);
    while (*it)
    {
        if ((*it)->text(0) == "test1") QVERIFY((*it)->checkState(1) == Qt::Unchecked);
        if ((*it)->text(0) == "test2") QVERIFY((*it)->checkState(1) == Qt::Checked);
        it++;
    }
    dlg->reject();
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
    QVERIFY(dlg != NULL);

    QTreeWidget *table = dlg->findChild<QTreeWidget*>("selectTable");
    QVERIFY(table != NULL);
    QVERIFY(table->topLevelItemCount() == 1);

    QTreeWidgetItemIterator it(table, QTreeWidgetItemIterator::Enabled);
    while (*it)
    {
        if ((*it)->text(0) == "test1") QVERIFY((*it)->checkState(1) == Qt::Unchecked);
        if ((*it)->text(0) == "test2") QVERIFY((*it)->checkState(1) == Qt::Checked);
        it++;
    }
    dlg->reject();
}
