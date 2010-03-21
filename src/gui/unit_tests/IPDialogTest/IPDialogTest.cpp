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

#include "../../../../config.h"
//#include "../../global.h"

#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
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
#include "fwbuilder/Interface.h"""

using namespace std;
using namespace libfwbuilder;

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

void IPDialogTest::checkMessageBox()
{
    QVERIFY(app->activeModalWidget()->metaObject()->className() == QMessageBox().metaObject()->className());
    QTimer::singleShot(10, app->activeModalWidget(), SLOT(reject()));
    QVERIFY(dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("300.300.300.300") ||
            dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("foo:345:1")  ||
            dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("145") );
}

void setLineEditText(QLineEdit *line, QString text)
{
    line->clear();
    QTest::keyClicks(line, text);
    QTest::keyClick(line, Qt::Key_Enter);
}

void IPDialogTest::testDialog()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();
    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    d->close();
    QTest::qWait(1000);
    QToolButton* newButton = mw->findChild<QToolButton*>("newButton");
    mw->findChild<QAction*>(QString("newObject_") + IPv4::TYPENAME)->trigger();

    QTest::qWait(100);
    QTreeWidgetItem *item = mw->getCurrentObjectTree()->findItems("Address", Qt::MatchRecursive | Qt::MatchExactly, 0).first();

    mw->getCurrentObjectTree()->setCurrentItem(item, 0, QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    ObjectManipulator *om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    om->editSelectedObject();
    QTest::qWait(100);
    QLineEdit *name, *addr, *mask, *name6, *addr6, *mask6;
    TextEditWidget *comment, *comment6;
    foreach(QFrame* frame, mw->findChildren<QFrame*>("frame14"))
    {
        if (frame->parent()->parent()->objectName() == "w_IPv4Dialog")
        {
            name = frame->findChild<QLineEdit*>("obj_name");
            addr = frame->findChild<QLineEdit*>("address");
            mask = frame->findChild<QLineEdit*>("netmask");
            comment = frame->parent()->findChild<TextEditWidget*>("comment");
        }
        else
        {
            name6 = frame->findChild<QLineEdit*>("obj_name");
            addr6 = frame->findChild<QLineEdit*>("address");
            mask6 = frame->findChild<QLineEdit*>("netmask");
            comment6 = frame->parent()->findChild<TextEditWidget*>("comment");
        }
    }

    // test1
    qDebug() << "test1";
    setLineEditText(name, "New Network");
    QVERIFY (dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject()->getName() == "New Network");
    QTest::qWait(100);

    comment->clear();
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClick(comment, Qt::Key_Tab);
    QTest::qWait(100);
    QVERIFY (dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject()->getComment() == "Test comment");
    QTest::qWait(100);


    //test2
    qDebug() << "test2";
    setLineEditText(addr, "192.0.2.1");
    QVERIFY (libfwbuilder::IPv4::cast(dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject())->getAddressPtr()->toString() == "192.0.2.1");

    //test3
    qDebug() << "test3";
    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(addr, "300.300.300.300");
    QTest::qWait(400);
    QVERIFY (libfwbuilder::IPv4::cast(dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject())->getAddressPtr()->toString() == "192.0.2.1");

    //test4
    qDebug() << "test4";
    Library *lib = NULL;
    foreach (FWObject *obj, mw->db()->getByType(Library::TYPENAME))
    {
        if (obj->getName() == "User")
        {
            lib = Library::cast(obj);
            break;
        }
    }
    Firewall *fw = Firewall::cast(om->createObject(FWBTree().getStandardSlotForObject(lib, Firewall::TYPENAME), Firewall::TYPENAME, "newFirewall"));
    Interface *intf = Interface::cast(om->createObject(fw, Interface::TYPENAME, "newInterface"));
    IPv4 *addr2obj = IPv4::cast(om->createObject(intf, IPv4::TYPENAME, "newAddress"));
    QTest::qWait(100);

    QLineEdit* addr2 = mw->findChild<QWidget*>("w_IPv4Dialog")->findChild<QLineEdit*>("address");
    QLineEdit* mask2 =  mw->findChild<QWidget*>("w_IPv4Dialog")->findChild<QLineEdit*>("netmask");

    QTest::qWait(500);
    setLineEditText(addr2, "192.0.2.1");
    QVERIFY (addr2obj->getAddressPtr()->toString() == "192.0.2.1");
    setLineEditText(mask2, "255.255.0.0");
    QVERIFY (addr2obj->getNetmaskPtr()->toString() == "255.255.0.0");

    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(addr2, "300.300.300.300");
    QTest::qWait(400);
    QVERIFY (addr2obj->getAddressPtr()->toString() == "192.0.2.1");

    QTest::qWait(100);
    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(mask2, "300.300.300.300");
    QTest::qWait(400);
    QVERIFY (addr2obj->getNetmaskPtr()->toString() == "255.255.0.0");

    //test5
    qDebug() << "test5";
    mw->findChild<QAction*>(QString("newObject_") + IPv6::TYPENAME)->trigger();
    QTest::qWait(100);
    item = mw->getCurrentObjectTree()->findItems("Address IPv6", Qt::MatchRecursive | Qt::MatchExactly, 0).first();
    QVERIFY(item != NULL);
    setLineEditText(name6, "New Network 6");
    QVERIFY (dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject()->getName() == "New Network 6");
    QTest::qWait(300);
    comment6->clear();
    QTest::mouseClick(comment6, Qt::LeftButton);
    QTest::keyClicks(comment6, "Test comment");
    QTest::mouseClick(comment6, Qt::LeftButton);
    QTest::keyClick(comment6, Qt::Key_Tab);
    QTest::qWait(100);
    QVERIFY (dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject()->getComment() == "Test comment");
    QTest::qWait(100);
    setLineEditText(addr6, "2001:db8:1:1::1");
    QTest::qWait(100);
    QVERIFY (libfwbuilder::IPv6::cast(dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject())->getAddressPtr()->toString() == "2001:db8:1:1::1");


    //test6
    qDebug() << "test6";
    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(addr6, "foo:345:1");
    QTest::qWait(400);
    QVERIFY (libfwbuilder::IPv6::cast(dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject())->getAddressPtr()->toString() == "2001:db8:1:1::1");

    //test7
    qDebug() << "test7";
    IPv6 *addr62obj = IPv6::cast(om->createObject(intf, IPv6::TYPENAME, "newAddress2"));
    QTest::qWait(100);
    addr2 = mw->findChild<QWidget*>("w_IPv6Dialog")->findChild<QLineEdit*>("address");
    mask2 =  mw->findChild<QWidget*>("w_IPv6Dialog")->findChild<QLineEdit*>("netmask");

    QTest::qWait(500);
    setLineEditText(addr2, "2001:db8:1:1::1");
    QVERIFY (addr62obj->getAddressPtr()->toString() == "2001:db8:1:1::1");
    setLineEditText(mask2, "120");
    QVERIFY (addr62obj->getNetmaskPtr()->toString() == "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ff00");

    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(addr2, "foo:345:1");
    QVERIFY (addr62obj->getAddressPtr()->toString() == "2001:db8:1:1::1");
    QTest::qWait(300);
    QTimer::singleShot(300, this, SLOT(checkMessageBox()));
    setLineEditText(mask2, "145");
    QTest::qWait(400);
    QVERIFY (addr62obj->getNetmaskPtr()->toString() == "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ff00");

    QTest::qWait(100);
}
