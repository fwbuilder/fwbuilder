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

using namespace std;
using namespace QTest;
using namespace libfwbuilder;

void instDialogCompileTest::initTestCase()
{
    mw = new FWWindow();
    mw->show();
    mw->loadFile("test_work.fwb", false);
    FWObjectClipboard *clip = new FWObjectClipboard();
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

    mw->findChild<QAction*>("compileAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton16")->click();
    QTest::qWait(500);
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
    QFile::remove("test1.fw");
    QFile::remove("test2.fw");
    QFile::remove("test3.fw");
    QFile::remove("test4.fw");
    QTest::qWait(1000);
}

