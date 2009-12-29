#ifndef NEWCLUSTERDIALOGTEST_H
#define NEWCLUSTERDIALOGTEST_H

#include <QTest>
#include "newClusterDialog.h"
#include "upgradePredicate.h"
#include "FWBTree.h"
#include "fwbuilder/Library.h"

class newClusterDialogTest : public QObject
{
    Q_OBJECT
private slots:
    void runTest();

};

#endif // NEWCLUSTERDIALOGTEST_H
