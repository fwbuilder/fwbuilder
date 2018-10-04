/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: TCPServiceDialogTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "TCPServiceDialogTest.h"

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
#include "TCPServiceDialogTest.h"
#include "StartTipDialog.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "IPServiceDialog.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/TCPService.h"
#include "TCPServiceDialog.h"

using namespace std;
using namespace libfwbuilder;

void TCPServiceDialogTest::initTestCase()
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

Library* TCPServiceDialogTest::findUserLibrary()
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

void TCPServiceDialogTest::testDialog()
{
    TCPService *service = TCPService::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), TCPService::TYPENAME), TCPService::TYPENAME, "testTCPService"));
    om->editObject(service);

    TCPServiceDialog *dialog = mw->findChild<TCPServiceDialog*>("w_TCPServiceDialog");
    QLineEdit *obj_name = dialog->findChild<QLineEdit*>("obj_name");
    QCheckBox *fin_s = dialog->findChild<QCheckBox*>("fin_s");
    QCheckBox *rst_s = dialog->findChild<QCheckBox*>("rst_s");
    QCheckBox *urg_s = dialog->findChild<QCheckBox*>("urg_s");
    QCheckBox *syn_s = dialog->findChild<QCheckBox*>("syn_s");
    QCheckBox *psh_s = dialog->findChild<QCheckBox*>("psh_s");
    QCheckBox *ack_s = dialog->findChild<QCheckBox*>("ack_s");
    QCheckBox *psh_m = dialog->findChild<QCheckBox*>("psh_m");
    QCheckBox *syn_m = dialog->findChild<QCheckBox*>("syn_m");
    QCheckBox *urg_m = dialog->findChild<QCheckBox*>("urg_m");
    QCheckBox *fin_m = dialog->findChild<QCheckBox*>("fin_m");
    QCheckBox *rst_m = dialog->findChild<QCheckBox*>("rst_m");
    QCheckBox *ack_m = dialog->findChild<QCheckBox*>("ack_m");
    QCheckBox *established = dialog->findChild<QCheckBox*>("established");
    QSpinBox *ss = dialog->findChild<QSpinBox*>("ss");
    QSpinBox *se = dialog->findChild<QSpinBox*>("se");
    QSpinBox *ds = dialog->findChild<QSpinBox*>("ds");
    QSpinBox *de = dialog->findChild<QSpinBox*>("de");
    TextEditWidget *comment = dialog->findChild<TextEditWidget*>("comment");

    obj_name->clear();
    QTest::keyClicks(obj_name, "TestTCPService");
    QTest::keyClick(obj_name, Qt::Key_Enter);
    QVERIFY(service->getName() == "TestTCPService");

    comment->clear();
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClick(comment, Qt::Key_Tab);
    QTest::qWait(100);
    QVERIFY (service->getComment() == "Test comment");

    // checking all small checkboxes
    QTest::mouseClick(urg_m, Qt::LeftButton);
    QVERIFY(service->getBool("urg_flag_mask"));
    QTest::mouseClick(urg_m, Qt::LeftButton);
    QVERIFY(!service->getBool("urg_flag_mask"));

    QTest::mouseClick(ack_m, Qt::LeftButton);
    QVERIFY(service->getBool("ack_flag_mask"));
    QTest::mouseClick(ack_m, Qt::LeftButton);
    QVERIFY(!service->getBool("ack_flag_mask"));

    QTest::mouseClick(psh_m, Qt::LeftButton);
    QVERIFY(service->getBool("psh_flag_mask"));
    QTest::mouseClick(psh_m, Qt::LeftButton);
    QVERIFY(!service->getBool("psh_flag_mask"));

    QTest::mouseClick(rst_m, Qt::LeftButton);
    QVERIFY(service->getBool("rst_flag_mask"));
    QTest::mouseClick(rst_m, Qt::LeftButton);
    QVERIFY(!service->getBool("rst_flag_mask"));

    QTest::mouseClick(syn_m, Qt::LeftButton);
    QVERIFY(service->getBool("syn_flag_mask"));
    QTest::mouseClick(syn_m, Qt::LeftButton);
    QVERIFY(!service->getBool("syn_flag_mask"));

    QTest::mouseClick(fin_m, Qt::LeftButton);
    QVERIFY(service->getBool("fin_flag_mask"));
    QTest::mouseClick(fin_m, Qt::LeftButton);
    QVERIFY(!service->getBool("fin_flag_mask"));

    QTest::mouseClick(urg_s, Qt::LeftButton);
    QVERIFY(service->getBool("urg_flag"));
    QTest::mouseClick(urg_s, Qt::LeftButton);
    QVERIFY(!service->getBool("urg_flag"));

    QTest::mouseClick(ack_s, Qt::LeftButton);
    QVERIFY(service->getBool("ack_flag"));
    QTest::mouseClick(ack_s, Qt::LeftButton);
    QVERIFY(!service->getBool("ack_flag"));

    QTest::mouseClick(psh_s, Qt::LeftButton);
    QVERIFY(service->getBool("psh_flag"));
    QTest::mouseClick(psh_s, Qt::LeftButton);
    QVERIFY(!service->getBool("psh_flag"));

    QTest::mouseClick(rst_s, Qt::LeftButton);
    QVERIFY(service->getBool("rst_flag"));
    QTest::mouseClick(rst_s, Qt::LeftButton);
    QVERIFY(!service->getBool("rst_flag"));

    QTest::mouseClick(syn_s, Qt::LeftButton);
    QVERIFY(service->getBool("syn_flag"));
    QTest::mouseClick(syn_s, Qt::LeftButton);
    QVERIFY(!service->getBool("syn_flag"));

    QTest::mouseClick(fin_s, Qt::LeftButton);
    QVERIFY(service->getBool("fin_flag"));
    QTest::mouseClick(fin_s, Qt::LeftButton);
    QVERIFY(!service->getBool("fin_flag"));

    QTest::mouseClick(established, Qt::LeftButton, Qt::NoModifier, QPoint(10,10));
    QVERIFY(service->getBool("established"));
    QTest::mouseClick(established, Qt::LeftButton, Qt::NoModifier, QPoint(10,10));
    QVERIFY(!service->getBool("established"));

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

    ds->setValue(0);
    de->setValue(0);
    for (int i=0; i<20; i++)
        QTest::keyClick(de, Qt::Key_Up);
    QTest::keyClick(de, Qt::Key_Enter);
    for (int i=0; i<10; i++)
        QTest::keyClick(ds, Qt::Key_Up);
    QTest::keyClick(ds, Qt::Key_Enter);
    QVERIFY(de->value() == 20);
}
