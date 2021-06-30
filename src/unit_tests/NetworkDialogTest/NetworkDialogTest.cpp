/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: NetworkDialogTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "NetworkDialogTest.h"

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
#include "NetworkDialogTest.h"
#include "StartTipDialog.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "fwbuilder/Network.h"
#include "fwbuilder/NetworkIPv6.h"
#include "FWBApplication.h"

using namespace std;
using namespace libfwbuilder;


void NetworkDialogTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();
    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    if (d!=nullptr) d->close();
    QTest::qWait(1000);
}

QPoint findItemPos(ObjectTreeViewItem *item, ObjectTreeView *tree)
{
    for (int h=10; h<tree->height(); h+=1)
    {
        for (int w=75; w<tree->width(); w+=1)
        {
            if(tree->itemAt(w,h) == item)
                return QPoint(w, h);
        }
    }
    return QPoint(-1,-1);
}


void NetworkDialogTest::checkMessageBox()
{
    QVERIFY(app->activeModalWidget()->metaObject()->className() == QMessageBox().metaObject()->className());
    QTimer::singleShot(10, app->activeModalWidget(), SLOT(reject()));
    QVERIFY(dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("300.300.300.300") ||
            dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("foo:345:1") ||
            dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("145") ||
            dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("300") ||
            dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("0") ||
            dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("0.0.0.0") ||
            dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("255.300.300.0") );
}

void NetworkDialogTest::checkNoMessageBox()
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

Library* NetworkDialogTest::findUserLibrary()
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

void NetworkDialogTest::testNetwork4Dialog()
{
    ObjectManipulator *om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    Library *lib = findUserLibrary();

    Network *obj = Network::cast(om->createObject(FWBTree().getStandardSlotForObject(lib, Network::TYPENAME), Network::TYPENAME, "testNetwork"));

    QTest::qWait(100);
    om->editObject(obj);
    QLineEdit *name, *addr, *mask;
    TextEditWidget *comment;
    QWidget *ipv4dialog = mw->findChild<QWidget*>("w_NetworkDialog");
    name = ipv4dialog->findChild<QLineEdit*>("obj_name");
    addr = ipv4dialog->findChild<QLineEdit*>("address");
    mask = ipv4dialog->findChild<QLineEdit*>("netmask");
    comment = ipv4dialog->parent()->findChild<TextEditWidget*>("comment");

    // test1
    qDebug() << "test1";
    QTimer::singleShot(200, this, SLOT(checkNoMessageBox()));
    setLineEditText(name, "testNetwork");
    QTest::qWait(300);
    QVERIFY (obj->getName() == "testNetwork");
    QTest::qWait(100);

    comment->clear();
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClick(comment, Qt::Key_Tab);
    QTest::qWait(100);
    QVERIFY (obj->getComment() == "Test comment");

    //test2
    qDebug() << "test2";
    QTimer::singleShot(200, this, SLOT(checkNoMessageBox()));
    setLineEditText(addr, "192.0.2.1");
    QTest::qWait(300);
    QVERIFY (obj->getAddressPtr()->toString() == "192.0.2.1");

    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(addr, "300.300.300.300");
    QTest::qWait(400);
    QVERIFY (obj->getAddressPtr()->toString() == "192.0.2.1");

    QTimer::singleShot(200, this, SLOT(checkNoMessageBox()));
    setLineEditText(mask, "255.255.0.0");
    QTest::qWait(300);
    QVERIFY (obj->getNetmaskPtr()->toString() == "255.255.0.0");

    //test3
    qDebug() << "test3";
    QTest::qWait(100);
    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(mask, "300.300.300.300");
    QTest::qWait(400);
    QVERIFY (obj->getNetmaskPtr()->toString() == "255.255.0.0");

    //test4
    qDebug() << "test4";
    QTest::qWait(100);
    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(mask, "255.300.300.0");
    QTest::qWait(400);
    QVERIFY (obj->getNetmaskPtr()->toString() == "255.255.0.0");

    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(mask, "0.0.0.0");
    QTest::qWait(500);
    QVERIFY (obj->getNetmaskPtr()->toString() == "255.255.0.0");
}

void NetworkDialogTest::testNetwork6Dialog()
{
    ObjectManipulator *om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    Library *lib = findUserLibrary();
    QLineEdit *name, *addr, *mask;
    TextEditWidget *comment;

    QWidget *ipv6dialog = mw->findChild<QWidget*>("w_NetworkDialogIPv6");

    name = ipv6dialog->findChild<QLineEdit*>("obj_name");
    addr = ipv6dialog->findChild<QLineEdit*>("address");
    mask = ipv6dialog->findChild<QLineEdit*>("netmask");
    comment = ipv6dialog->parent()->findChild<TextEditWidget*>("comment");

    //test5
    qDebug() << "test5";
    NetworkIPv6 *obj = NetworkIPv6::cast(om->createObject(FWBTree().getStandardSlotForObject(lib, NetworkIPv6::TYPENAME), NetworkIPv6::TYPENAME, "testNetworkIPv6"));
    om->editObject(obj);

    QTimer::singleShot(200, this, SLOT(checkNoMessageBox()));
    setLineEditText(name, "testNetworkIPv6");
    QTest::qWait(300);
    QVERIFY (obj->getName() == "testNetworkIPv6");

    comment->clear();
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClick(comment, Qt::Key_Tab);
    QTest::qWait(100);
    QVERIFY (obj->getComment() == "Test comment");


    QTimer::singleShot(200, this, SLOT(checkNoMessageBox()));
    setLineEditText(addr, "2001:db8::");
    QTest::qWait(300);
    QVERIFY (obj->getAddressPtr()->toString() == "2001:db8::");

    QTimer::singleShot(200, this, SLOT(checkNoMessageBox()));
    setLineEditText(mask, "120");
    QTest::qWait(300);
    QVERIFY (obj->getNetmaskPtr()->toString() == "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ff00");

    //test6
    qDebug() << "test6";
    QTimer::singleShot(200, this, SLOT(checkMessageBox()));
    setLineEditText(addr, "foo:345:1");
    QTest::qWait(300);
    QVERIFY (obj->getAddressPtr()->toString() == "2001:db8::");

    //test7
    qDebug() << "test7";

    QTimer::singleShot(200, this, SLOT(checkMessageBox()));
    setLineEditText(mask, "300");
    QTest::qWait(300);
    QVERIFY (obj->getNetmaskPtr()->toString() == "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ff00");

    QTimer::singleShot(200, this, SLOT(checkMessageBox()));
    setLineEditText(mask, "0");
    QTest::qWait(300);
    QVERIFY (obj->getNetmaskPtr()->toString() == "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ff00");

    QTest::qWait(1000);
}
