/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: DiscoveryDruidTest.cpp 2786 2010-04-01 14:05:36Z a2k $

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

#include "DiscoveryDruidTest.h"

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
#include "DiscoveryDruid.h"

using namespace std;
using namespace QTest;
using namespace libfwbuilder;


void DiscoveryDruidTest::initTestCase()
{
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();
    new FWObjectClipboard();
}

void DiscoveryDruidTest::cleanupTestCase()
{
    if (QFileInfo("result.fwb").exists())
         QVERIFY(QFile::remove("result.fwb"));
}

void DiscoveryDruidTest::compareFwbFiles(QString expected_result_file_name,
                                         QString obtained_result_file_name)
{
    QStringList obtained_result;

    QFile rr(obtained_result_file_name);
    rr.open(QFile::ReadOnly);
    QString result_file = rr.readAll();
    rr.close();
    obtained_result = result_file.split("\n");

    QFile er(expected_result_file_name);
    er.open(QFile::ReadOnly);
    result_file = er.readAll();
    er.close();
    QStringList expected_result = result_file.split("\n");

    // find all lastModified attributes and replace them with identical values
    // because they are always going to be different
    Q_ASSERT_X(expected_result.size() == obtained_result.size(), "result comparison",
               "Sizes of the generated .fwb and test files are different");

    QRegExp last_mod_re("lastModified=\"\\d+\"");
    QRegExp id_re("id=\"\\w+\"");
    int max_idx = max(expected_result.size(), obtained_result.size());
    for (int i=0; i < max_idx; ++i)
    {
        QString os = obtained_result[i];
        obtained_result[i] = os.replace(last_mod_re, "lastModified=\"0000000000\"")
                             .remove(id_re);

        QString es = expected_result[i];
        expected_result[i] = es.replace(last_mod_re, "lastModified=\"0000000000\"")
                             .remove(id_re);
    }

    for (int i=0; i < max_idx; ++i)
    {
        QString err = QString("Line %1:\nExpected: '%2'\nResult: '%3'\n")
            .arg(i).arg(expected_result[i]).arg(obtained_result[i]);
        Q_ASSERT_X(expected_result[i] == obtained_result[i], "result comparison", err.toUtf8().constData());
    }
}

void DiscoveryDruidTest::testHostsImportDialog()
{
    DiscoveryDruid *dlg = dynamic_cast<DiscoveryDruid*>(app->activeModalWidget());
    QVERIFY(dlg!=NULL);

    QPushButton *next = dlg->findChild<QPushButton*>("nextButton");
    QPushButton *finish = dlg->findChild<QPushButton*>("finishButton");

    // Selecting "Read file in hosts format" radio button
    QTest::mouseClick(dlg->findChild<QRadioButton*>("dm_fromfile"), Qt::LeftButton, 0, QPoint(5,5));
    QTest::mouseClick(next, Qt::LeftButton);

    dlg->findChild<QLineEdit*>("filename")->setText("test.hosts");

    QTest::mouseClick(next, Qt::LeftButton);

    // Waiting for parsing to wait. We are using small and simple hosts
    // file, so it should not take more than 1 second
    QTest::qWait(1000);
    QVERIFY(next->isEnabled());
    QTest::mouseClick(next, Qt::LeftButton);

    QListWidget *unused = dlg->findChild<QListWidget*>("objectresultlist");
    QVERIFY(unused->count() == 3);
    unused->selectAll();
    QTest::mouseClick(dlg->findChild<QPushButton*>("addObjButton"), Qt::LeftButton);
    QVERIFY(dlg->findChild<QListWidget*>("objectlist")->count() == 3);
    QTest::mouseClick(next, Qt::LeftButton);
    QTest::mouseClick(next, Qt::LeftButton); // using default library ("User")

    QVERIFY(finish->isEnabled()); // it should be enabled if everything is ok
    QTest::mouseClick(finish, Qt::LeftButton);
    // hosts should be imported to "User" database now and dialog is closed
}

void DiscoveryDruidTest::testHostsImport()
{
    // Dialog is modal, so we have to test it in another thread
    QTimer::singleShot(100, this, SLOT(testHostsImportDialog()));
    mw->findChild<QAction*>("DiscoveryDruidAction")->trigger();

    // this is running after import dialog is closed
    mw->activeProject()->setFileName("result.fwb");
    mw->activeProject()->db()->setPredictableIds();
    mw->activeProject()->save();

    compareFwbFiles("output.fwb", "result.fwb");
}

