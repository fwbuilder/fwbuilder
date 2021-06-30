/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: IPDialogTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "IPDialogTest.h"

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
#include "IPDialogTest.h"
#include "StartTipDialog.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "FWBApplication.h"

using namespace std;
using namespace libfwbuilder;

void IPDialogTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();
    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    if (d!=nullptr) d->close();
    QTest::qWait(1000);
}

void IPDialogTest::checkMessageBox()
{
    QVERIFY(app->activeModalWidget()->metaObject()->className() == QMessageBox().metaObject()->className());
    QTimer::singleShot(10, app->activeModalWidget(), SLOT(reject()));
    QVERIFY(dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("300.300.300.300") ||
            dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("foo:345:1")  ||
            dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("145") );
}

void IPDialogTest::checkNoMessageBox()
{
    foreach(QWidget *w, app->topLevelWidgets())
        QVERIFY(w->metaObject()->className() != QMessageBox().metaObject()->className());
}

void setLineEditText(QLineEdit *line, QString text)
{
    line->clear();
    QTest::keyClicks(line, text);
    QTest::keyClick(line, Qt::Key_Enter);
}

void IPDialogTest::testIPv4Dialog()
{
    ObjectManipulator *om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    Library *lib = nullptr;
    foreach (FWObject *obj, mw->db()->getByType(Library::TYPENAME))
    {
        if (obj->getName() == "User")
        {
            lib = Library::cast(obj);
            break;
        }
    }

    IPv4 *addrobj = IPv4::cast(om->createObject(FWBTree().getStandardSlotForObject(lib, IPv4::TYPENAME), IPv4::TYPENAME, "testAddress"));

    QTest::qWait(100);
    om->editObject(addrobj);
    QLineEdit *name, *addr, *mask;
    TextEditWidget *comment;
    QWidget *ipv4dialog = mw->findChild<QWidget*>("w_IPv4Dialog");
    name = ipv4dialog->findChild<QLineEdit*>("obj_name");
    addr = ipv4dialog->findChild<QLineEdit*>("address");
    mask = ipv4dialog->findChild<QLineEdit*>("netmask");
    comment = ipv4dialog->parent()->findChild<TextEditWidget*>("comment");

    // test1
    qDebug() << "test1";
    QTimer::singleShot(200, this, SLOT(checkNoMessageBox()));
    setLineEditText(name, "testNetwork");
    QTest::qWait(300);
    QVERIFY (addrobj->getName() == "testNetwork");
    QTest::qWait(100);

    comment->clear();
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClick(comment, Qt::Key_Tab);
    QTest::qWait(100);
    QVERIFY (addrobj->getComment() == "Test comment");
    QTest::qWait(100);


    //test2
    qDebug() << "test2";
    QTimer::singleShot(200, this, SLOT(checkNoMessageBox()));
    setLineEditText(addr, "192.0.2.1");
    QTest::qWait(300);
    QVERIFY (addrobj->getAddressPtr()->toString() == "192.0.2.1");

    //test3
    qDebug() << "test3";
    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(addr, "300.300.300.300");
    QTest::qWait(400);
    QVERIFY (addrobj->getAddressPtr()->toString() == "192.0.2.1");

    //test4
    qDebug() << "test4";
    Firewall *fw = Firewall::cast(om->createObject(FWBTree().getStandardSlotForObject(lib, Firewall::TYPENAME), Firewall::TYPENAME, "newFirewall"));
    Interface *intf = Interface::cast(om->createObject(fw, Interface::TYPENAME, "newInterface"));
    IPv4 *addr2obj = IPv4::cast(om->createObject(intf, IPv4::TYPENAME, "newAddress"));
    QTest::qWait(100);

    QTest::qWait(500);
    QTimer::singleShot(200, this, SLOT(checkNoMessageBox()));
    setLineEditText(addr, "192.0.2.1");
    QTest::qWait(300);
    QVERIFY (addr2obj->getAddressPtr()->toString() == "192.0.2.1");
    QTimer::singleShot(200, this, SLOT(checkNoMessageBox()));
    setLineEditText(mask, "255.255.0.0");
    QTest::qWait(300);
    QVERIFY (addr2obj->getNetmaskPtr()->toString() == "255.255.0.0");

    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(addr, "300.300.300.300");
    QTest::qWait(400);
    QVERIFY (addr2obj->getAddressPtr()->toString() == "192.0.2.1");

    QTest::qWait(100);
    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(mask, "300.300.300.300");
    QTest::qWait(400);
    QVERIFY (addr2obj->getNetmaskPtr()->toString() == "255.255.0.0");
}

void IPDialogTest::testIPv6Dialog()
{   
    ObjectManipulator *om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    Library *lib = nullptr;
    foreach (FWObject *obj, mw->db()->getByType(Library::TYPENAME))
    {
        if (obj->getName() == "User")
        {
            lib = Library::cast(obj);
            break;
        }
    }
    QLineEdit *name, *addr, *mask;
    TextEditWidget *comment;
    QWidget *ipv6dialog = mw->findChild<QWidget*>("w_IPv6Dialog");

    name = ipv6dialog->findChild<QLineEdit*>("obj_name");
    addr = ipv6dialog->findChild<QLineEdit*>("address");
    mask = ipv6dialog->findChild<QLineEdit*>("netmask");
    comment = ipv6dialog->parent()->findChild<TextEditWidget*>("comment");

    //test5
    qDebug() << "test5";
    IPv6 *addrobj = IPv6::cast(om->createObject(FWBTree().getStandardSlotForObject(lib, IPv6::TYPENAME), IPv6::TYPENAME, "testAddress6"));
    om->editObject(addrobj);
    setLineEditText(name, "testNetwork6");
    QVERIFY (addrobj->getName() == "testNetwork6");
    QTest::qWait(300);
    comment->clear();
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClick(comment, Qt::Key_Tab);
    QTest::qWait(100);
    QVERIFY (addrobj->getComment() == "Test comment");
    QTest::qWait(100);
    QTimer::singleShot(200, this, SLOT(checkNoMessageBox()));
    setLineEditText(addr, "2001:db8:1:1::1");
    QTest::qWait(300);
    QVERIFY (libfwbuilder::IPv6::cast(addrobj)->getAddressPtr()->toString() == "2001:db8:1:1::1");

    //test6
    qDebug() << "test6";
    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(addr, "foo:345:1");
    QTest::qWait(400);
    QVERIFY (libfwbuilder::IPv6::cast(addrobj)->getAddressPtr()->toString() == "2001:db8:1:1::1");

    //test7
    qDebug() << "test7";
    Interface *intf = Interface::cast(dynamic_cast<ObjectTreeViewItem*>(mw->getCurrentObjectTree()->findItems("newInterface", Qt::MatchExactly | Qt::MatchRecursive, 0).first())->getFWObject());
    IPv6 *addr62obj = IPv6::cast(om->createObject(intf, IPv6::TYPENAME, "newAddress2"));
    QTest::qWait(100);

    QTest::qWait(500);
    setLineEditText(addr, "2001:db8:1:1::1");
    QVERIFY (addr62obj->getAddressPtr()->toString() == "2001:db8:1:1::1");
    QTimer::singleShot(200, this, SLOT(checkNoMessageBox()));
    setLineEditText(mask, "120");
    QTest::qWait(300);
    QVERIFY (addr62obj->getNetmaskPtr()->toString() == "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ff00");

    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(addr, "foo:345:1");
    QVERIFY (addr62obj->getAddressPtr()->toString() == "2001:db8:1:1::1");
    QTest::qWait(300);
    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(mask, "145");
    QTest::qWait(400);
    QVERIFY (addr62obj->getNetmaskPtr()->toString() == "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ff00");

    QTest::qWait(100);
}
