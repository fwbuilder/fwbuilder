/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id$

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

#include "FWWindowTest.h"
#include "FWWindow.h"
#include <QFile>
#include "global.h"
#include "FWBSettings.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

FWWindowTest::FWWindowTest(QWidget *parent)
    : QMainWindow(parent)
{
}

void FWWindowTest::initTestCase()
{
    st->setCheckUpdates(true);
    st->setBool("UI/NoStartTip", true);
}

void FWWindowTest::cleanupTestCase()
{
    if (QFile::exists("/tmp/update_4.0.0"))
        qDebug() << "file /tmp/update_4.0.0 exists";
    QFile::remove("/tmp/update_4.0.0");
}

void FWWindowTest::findMessageBox()
{
    QList<QMessageBox*> boxes = mw->findChildren<QMessageBox*>();
    QVERIFY(!boxes.empty());
    bool found = false;
    foreach(QMessageBox *box, boxes)
    {
        if (box->text().contains("http://www.fwbuilder.org"))
        {
            found = true;
            box->reject();
            break;
        }
    }
    QVERIFY(found);
}

void FWWindowTest::findNoMessageBox()
{
    QList<QMessageBox*> boxes = mw->findChildren<QMessageBox*>();
    if (!boxes.isEmpty())
        boxes.first()->reject();
    QVERIFY(boxes.empty());
}

void FWWindowTest::prepareMW()
{
    mw = new FWWindow();
    mw->show();
    QFile updatefile("/tmp/update_4.0.0");
    updatefile.open(QFile::ReadWrite);
    updatefile.write("update = 1\n");
    updatefile.close();
    setenv("FWBUILDER_CHECK_UPDATE_URL", "file:///tmp/update_4.0.0", 1);
    st->setCheckUpdates(true);
}

void FWWindowTest::checkForUpgrade_1()
{
    prepareMW();
    st->setTimeOfLastUpdateAvailableWarning(0);
    QTimer::singleShot(1000, this, SLOT(findMessageBox()));
    mw->startupLoad();
    QTest::qWait(1500);
    mw->hide();
}

void FWWindowTest::checkForUpgrade_2()
{
    prepareMW();
    st->setTimeOfLastUpdateAvailableWarning(QDateTime::currentDateTime().addSecs(-60*60*25).toTime_t());
    QTimer::singleShot(1000, this, SLOT(findMessageBox()));
    mw->startupLoad();
    QTest::qWait(1500);
    mw->hide();
}

void FWWindowTest::checkForUpgrade_3()
{
    prepareMW();
    st->setTimeOfLastUpdateAvailableWarning(QDateTime::currentDateTime().addSecs(-60*60*2).toTime_t());
    QTimer::singleShot(1000, this, SLOT(findNoMessageBox()));
    mw->startupLoad();
    QTest::qWait(1500);
    mw->hide();
}

void FWWindowTest::checkForUpgrade_4()
{
    prepareMW();
    QFile updatefile("/tmp/update_4.0.0");
    updatefile.open(QFile::WriteOnly);
    updatefile.resize(0);
    updatefile.close();
    st->setTimeOfLastUpdateAvailableWarning(QDateTime::currentDateTime().addSecs(-60*60*25).toTime_t());
    QTimer::singleShot(1000, this, SLOT(findNoMessageBox()));
    mw->startupLoad();
    QTest::qWait(1500);
    mw->hide();
}
