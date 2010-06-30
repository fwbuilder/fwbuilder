/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: instDialogInspectTest.cpp 2786 2010-04-01 14:05:36Z a2k $

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

#include "instDialogInspectTest.h"

#include "unistd.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMenuBar>
#include <QTextBrowser>

#include "FWObjectClipboard.h"
#include "FWBApplication.h"

using namespace std;
using namespace QTest;
using namespace libfwbuilder;


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

void instDialogInspectTest::initTestCase()
{
    mw = new FWWindow();
    mw->show();
    mw->loadFile("test_work.fwb", false);
    FWObjectClipboard *clip = new FWObjectClipboard();
    dialogClosed = false;
}

void instDialogInspectTest::cleanupTestCase()
{
    if (QFileInfo("test1.fw").exists())
        QVERIFY(QFile::remove("test1.fw"));
    if (QFileInfo("test2.fw").exists())
        QVERIFY(QFile::remove("test2.fw"));
    if (QFileInfo("test3.fw").exists())
        QVERIFY(QFile::remove("test3.fw"));
    if (QFileInfo("test4.fw").exists())
        QVERIFY(QFile::remove("test4.fw"));
}

void instDialogInspectTest::closeInstallOptions()
{
    QVERIFY(dynamic_cast<instOptionsDialog*>(app->activeModalWidget()) != NULL);
    app->activeModalWidget()->close();
    dialogClosed = true;
}

void instDialogInspectTest::testInspect()
{
    mw->findChild<QAction*>("installAction")->trigger();
    instDialog *dlg = mw->findChild<instDialog*>();

    QTest::mouseClick(dlg->findChild<QPushButton*>("pushButton17"), Qt::LeftButton);

    QPushButton *back = dlg->findChild<QPushButton*>("backButton");
    QPushButton *next = dlg->findChild<QPushButton*>("nextButton");
    QPushButton *cancel = dlg->findChild<QPushButton*>("cancelButton");

    QPushButton *inspect = dlg->findChild<QPushButton*>("inspectGeneratedFiles");

    QTreeWidget *selectTable = dlg->findChild<QTreeWidget*>("selectTable");
    QTreeWidgetItem *test1item = selectTable->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first();
    test1item->setCheckState(1, Qt::Checked);
    test1item->setCheckState(2, Qt::Checked);

    QTest::mouseClick(next, Qt::LeftButton);

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");
    QTextBrowser *processLogDisplay = dlg->findChild<QTextBrowser*>("procLogDisplay");

    while (!checkProgress(list))
    {
        QVERIFY(!inspect->isEnabled());
        QTest::qWait(50);
    }
    QTest::qWait(50);
    QVERIFY(inspect->isEnabled());

    QString oldtext = processLogDisplay->toPlainText();
    QStackedWidget *stack = dlg->findChild<QStackedWidget*>();

    QVERIFY(stack->currentIndex() == 1);
    QTest::mouseClick(inspect, Qt::LeftButton);
    QVERIFY(stack->currentIndex() == 2);
    QVERIFY(back->isEnabled());
    QVERIFY(next->isEnabled());

    QTest::mouseClick(back, Qt::LeftButton);
    QVERIFY(stack->currentIndex() == 1);
    QVERIFY(oldtext == processLogDisplay->toPlainText());

    QTest::mouseClick(inspect, Qt::LeftButton);
    QTimer::singleShot(100, this, SLOT(closeInstallOptions()));
    QTest::mouseClick(next, Qt::LeftButton);
    QVERIFY(dialogClosed);
    QVERIFY(stack->currentIndex() == 1);

    QTest::mouseClick(cancel, Qt::LeftButton);
}
