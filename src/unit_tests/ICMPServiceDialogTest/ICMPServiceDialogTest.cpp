/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: ICMPServiceDialogTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "ICMPServiceDialogTest.h"

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
#include "ICMPServiceDialogTest.h"
#include "StartTipDialog.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "IPServiceDialog.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/ICMPService.h"
#include "ICMPServiceDialog.h"

using namespace std;
using namespace libfwbuilder;

void ICMPServiceDialogTest::initTestCase()
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

Library* ICMPServiceDialogTest::findUserLibrary()
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


void ICMPServiceDialogTest::testDialog()
{
    ICMPService *service = ICMPService::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), ICMPService::TYPENAME), ICMPService::TYPENAME, "testICMPService"));
    om->editObject(service);

    ICMPServiceDialog *dialog = mw->findChild<ICMPServiceDialog*>("w_ICMPServiceDialog");
    QLineEdit *obj_name = dialog->findChild<QLineEdit*>("obj_name");
    QSpinBox *type = dialog->findChild<QSpinBox*>("icmpType");
    QSpinBox *code = dialog->findChild<QSpinBox*>("icmpCode");
    TextEditWidget *comment = dialog->findChild<TextEditWidget*>("comment");

    obj_name->clear();
    QTest::keyClicks(obj_name, "TestICMPService");
    QTest::keyClick(obj_name, Qt::Key_Enter);
    QVERIFY(service->getName() == "TestICMPService");

    comment->clear();
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClick(comment, Qt::Key_Tab);
    QTest::qWait(100);
    QVERIFY (service->getComment() == "Test comment");

    type->clear();
    for (int i=0; i<52; i++)
        QTest::keyClick(type, Qt::Key_Up);
    QTest::keyClick(type, Qt::Key_Enter);
    QVERIFY(service->getInt("type") == 50);

    for (int i=0; i<210; i++)
        QTest::keyClick(type, Qt::Key_Up);

    QTest::keyClick(type, Qt::Key_Enter);
    QVERIFY(service->getInt("type") == 255);

    code->clear();
    for (int i=0; i<52; i++)
        QTest::keyClick(code, Qt::Key_Up);
    QTest::keyClick(type, Qt::Key_Enter);
    QVERIFY(service->getInt("code") == 50);

    for (int i=0; i<210; i++)
        QTest::keyClick(code, Qt::Key_Up);
    QTest::keyClick(type, Qt::Key_Enter);
    QVERIFY(service->getInt("code") == 255);
}
