/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: UserServiceDialogTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "UserServiceDialogTest.h"

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
#include "UserServiceDialogTest.h"
#include "StartTipDialog.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "IPServiceDialog.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/UserService.h"
#include "UserDialog.h"

using namespace std;
using namespace libfwbuilder;

void UserServiceDialogTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();
    mw->resize(1200, 600);

    QTest::qWait(2000);

    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    if (d!=nullptr) d->close();
    om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());

}

Library* UserServiceDialogTest::findUserLibrary()
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


void UserServiceDialogTest::testDialog()
{
    UserService *service = UserService::cast(
        om->createObject(
            FWBTree().getStandardSlotForObject(findUserLibrary(),
                                               UserService::TYPENAME),
            UserService::TYPENAME, "testUserService"));

    QTest::qWait(1000);
    //om->editObject(service);

    UserDialog *dialog = mw->findChild<UserDialog*>("w_UserDialog");
    QVERIFY(dialog != nullptr);

    QLineEdit *obj_name = dialog->findChild<QLineEdit*>("obj_name");
    QLineEdit *userid = dialog->findChild<QLineEdit*>("userid");
    TextEditWidget *comment = dialog->findChild<TextEditWidget*>("comment");

    obj_name->clear();
    QTest::keyClicks(obj_name, "TestUserService");
    QTest::keyClick(obj_name, Qt::Key_Enter);
    QTest::qWait(100);
    QVERIFY(service->getName() == "TestUserService");

    userid->clear();
    QTest::keyClicks(userid, "username");
    QTest::keyClick(userid, Qt::Key_Enter);
    QTest::qWait(100);
    QVERIFY(service->getUserId() == "username");

    // need to click inside the comment input field to make sure it
    // has focus. Looks like keyClicks() works even when widget has no
    // focus, so clicking outside of it does not trigger "edit
    // finished" signal. also looks like waiting a little after
    // clicking inside the text editor widget makes test more stable.
    comment->clear();
    QTest::mouseClick(comment, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QTest::qWait(100);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(obj_name, Qt::LeftButton);
    QTest::qWait(100);

    qDebug() << "Dialog comment text=" << comment->toPlainText();
    qDebug() << "Object comment=" << QString(service->getComment().c_str());

    QVERIFY(service->getComment() == "Test comment");
}
 
 
 
