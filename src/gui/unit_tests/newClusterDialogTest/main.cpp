#include "newClusterDialogTest.h"
#include "FWWindow.h"
#include "FWBSettings.h"

int fwbdebug = 0;
FWWindow *mw = NULL;
FWBSettings *st = NULL;
char** args;
QApplication *app = NULL;

int main( int argc, char** argv)
{
    app = new QApplication(argc, argv, 0);
    newClusterDialogTest tst;
    QTest::qExec(&tst);
}

//QTEST_MAIN(newClusterDialogTest)
