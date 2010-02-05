#ifndef NEWCLUSTERDIALOGTEST_H
#define NEWCLUSTERDIALOGTEST_H

#include <QTest>
#include "newClusterDialog.h"
#include "upgradePredicate.h"
#include "FWBTree.h"
#include "fwbuilder/Library.h"
#include "ObjectManipulator.h"

class newClusterDialogTest : public QObject
{
    Q_OBJECT
    void openContextMenu(ObjectManipulator *om, ObjectTreeViewItem *item, ObjectTreeView *tree, const QString &actionText);

private slots:
    void test1();
    void test2();
    void test3();
public slots:
    void closeContextMenu();
    void test3_part2();
};

#endif // NEWCLUSTERDIALOGTEST_H
