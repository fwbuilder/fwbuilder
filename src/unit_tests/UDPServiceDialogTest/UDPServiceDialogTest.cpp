/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: UDPServiceDialogTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "UDPServiceDialogTest.h"

//#include "../../global.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QTest>
#include <iostream>

#include <QProcess>
#include <QRegExp>
#include <QDebug>

#include <QToolButton>
#include <QMessageBox>
#include <QWidget>
#include <QLineEdit>
#include "FWWindow.h"
#include "ProjectPanel.h"
#include "ObjectTreeView.h"
#include "ObjectTreeViewItem.h"
#include "ObjectEditor.h"
#include "FWObjectClipboard.h"
#include "TextEditWidget.h"
#include "fwbuilder/Address.h"
#include "fwbuilder/IPv4.h"
#include "fwbuilder/IPv6.h"
#include "FWBTree.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "UDPServiceDialogTest.h"
#include "StartTipDialog.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "IPServiceDialog.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/UDPService.h"
#include "UDPServiceDialog.h"

using namespace std;
using namespace libfwbuilder;

void UDPServiceDialogTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();
    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    if (d!=nullptr) d->close();
    om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    QTest::qWait(1000);
}

Library* UDPServiceDialogTest::findUserLibrary()
{
    Library *lib = nullptr;
    foreach (FWObject *obj, mw->db()->getByType(Library::TYPENAME))
    {
        if (obj->getName() == "User")
        {
            lib = Library::cast(obj);
            break;
        }
    }
    return lib;
}

void UDPServiceDialogTest::testDialog()
{
    UDPService *service = UDPService::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), UDPService::TYPENAME), UDPService::TYPENAME, "testUDPService"));
    om->editObject(service);

    UDPServiceDialog *dialog = mw->findChild<UDPServiceDialog*>("w_UDPServiceDialog");
    QLineEdit *obj_name = dialog->findChild<QLineEdit*>("obj_name");
    QSpinBox *ss = dialog->findChild<QSpinBox*>("ss");
    QSpinBox *se = dialog->findChild<QSpinBox*>("se");
    QSpinBox *ds = dialog->findChild<QSpinBox*>("ds");
    QSpinBox *de = dialog->findChild<QSpinBox*>("de");
    TextEditWidget *comment = dialog->findChild<TextEditWidget*>("comment");

    obj_name->clear();
    QTest::keyClicks(obj_name, "TestUDPService");
    QTest::keyClick(obj_name, Qt::Key_Enter);
    QVERIFY(service->getName() == "TestUDPService");

    comment->clear();
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClick(comment, Qt::Key_Tab);
    QTest::qWait(100);
    QVERIFY (service->getComment() == "Test comment");

    // check that it is not possible to set value more than 65535
    se->setValue(65530);
    for (int i=0; i<100; i++)
        QTest::keyClick(se, Qt::Key_Up);
    QTest::keyClick(se, Qt::Key_Enter);
    QVERIFY(service->getSrcRangeEnd() == 65535);
    ss->setValue(65530);
    for (int i=0; i<65560; i++)
        QTest::keyClick(ss, Qt::Key_Up);
    QTest::keyClick(ss, Qt::Key_Enter);
    QVERIFY(service->getSrcRangeStart() == 65535);

    de->setValue(65530);
    for (int i=0; i<100; i++)
        QTest::keyClick(de, Qt::Key_Up);
    QTest::keyClick(de, Qt::Key_Enter);
    QVERIFY(service->getDstRangeEnd() == 65535);
    ds->setValue(65530);
    for (int i=0; i<100; i++)
        QTest::keyClick(ds, Qt::Key_Up);
    QTest::keyClick(ds, Qt::Key_Enter);
    QVERIFY(service->getDstRangeStart() == 65535);

    // check that range end change when range start is more
    se->clear();
    for (int i=0; i<11; i++)
        QTest::keyClick(se, Qt::Key_Up);
    QTest::keyClick(se, Qt::Key_Enter);
    ss->clear();
    for (int i=0; i<21; i++)
        QTest::keyClick(ss, Qt::Key_Up);
    QTest::keyClick(ss, Qt::Key_Enter);
    QVERIFY(ss->value() == se->value());

    de->clear();
    for (int i=0; i<11; i++)
        QTest::keyClick(de, Qt::Key_Up);
    QTest::keyClick(de, Qt::Key_Enter);
    ds->clear();
    for (int i=0; i<21; i++)
        QTest::keyClick(ds, Qt::Key_Up);
    QTest::keyClick(ds, Qt::Key_Enter);
    QVERIFY(ds->value() == de->value());


    // check that range end does not change if range start is less
    ss->setValue(0);
    se->setValue(0);
    for (int i=0; i<20; i++)
        QTest::keyClick(se, Qt::Key_Up);
    QTest::keyClick(se, Qt::Key_Enter);
    for (int i=0; i<10; i++)
        QTest::keyClick(ss, Qt::Key_Up);
    QTest::keyClick(ss, Qt::Key_Enter);
    QVERIFY(se->value() == 20);

    //same for destination
    ds->setValue(0);
    de->setValue(0);
    for (int i=0; i<20; i++)
        QTest::keyClick(de, Qt::Key_Up);
    QTest::keyClick(de, Qt::Key_Enter);
    for (int i=0; i<10; i++)
        QTest::keyClick(ds, Qt::Key_Up);
    QTest::keyClick(ds, Qt::Key_Enter);
    QVERIFY(de->value() == 20);
    qDebug() << "done"; // it sometimes hangs without this
}
