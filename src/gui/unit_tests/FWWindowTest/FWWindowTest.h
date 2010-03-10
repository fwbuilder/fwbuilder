#ifndef FWWINDOWTEST_H
#define FWWINDOWTEST_H

#include <QtGui/QMainWindow>
#include <QTest>
#include <QMessageBox>

class FWWindowTest : public QMainWindow
{
    Q_OBJECT

public:
    FWWindowTest(QWidget *parent = 0);
public slots:
    void findMessageBox();
    void findNoMessageBox();
    void prepareMW();
private slots:
    void initTestCase();
    void checkForUpgrade_1();
    void checkForUpgrade_2();
    void checkForUpgrade_3();
    void checkForUpgrade_4();
};

#endif // FWWINDOWTEST_H
