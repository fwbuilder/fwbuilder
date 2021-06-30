/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: CustomServiceDialogTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "CustomServiceDialogTest.h"

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
#include <QRadioButton>
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
#include "CustomServiceDialogTest.h"
#include "StartTipDialog.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "IPServiceDialog.h"
#include "fwbuilder/IPService.h"
#include "fwbuilder/CustomService.h"
#include "CustomServiceDialog.h"
#include "platforms.h"

using namespace std;
using namespace libfwbuilder;

void CustomServiceDialogTest::initTestCase()
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

Library* CustomServiceDialogTest::findUserLibrary()
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

void CustomServiceDialogTest::selectComboItem(QWidget *widget, QString name)
{
    QComboBox * combo = dynamic_cast<QComboBox*>(widget);
    Q_ASSERT(combo != nullptr);
    int id = combo->findText(name);
    combo->setCurrentIndex(id);
}


void CustomServiceDialogTest::testDialog()
{
    CustomService *service = CustomService::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), CustomService::TYPENAME), CustomService::TYPENAME, "testCustomService"));
    om->editObject(service);

    CustomServiceDialog *dialog = mw->findChild<CustomServiceDialog*>("w_CustomServiceDialog");
    QLineEdit *obj_name = dialog->findChild<QLineEdit*>("obj_name");
    QComboBox *platform = dialog->findChild<QComboBox*>("platform");
    QLineEdit *code = dialog->findChild<QLineEdit*>("code");
    QComboBox *protocol = dialog->findChild<QComboBox*>("protocol");
    QRadioButton *ipv4 = dialog->findChild<QRadioButton*>("ipv4");
    QRadioButton *ipv6 = dialog->findChild<QRadioButton*>("ipv6");
    TextEditWidget *comment = dialog->findChild<TextEditWidget*>("comment");

    obj_name->clear();
    QTest::keyClicks(obj_name, "TestCustomService");
    QTest::keyClick(obj_name, Qt::Key_Enter);
    QVERIFY(service->getName() == "TestCustomService");

    comment->clear();
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClick(comment, Qt::Key_Tab);
    QTest::qWait(100);
    QVERIFY (service->getComment() == "Test comment");

    // testing saving platform code to service object
    selectComboItem(platform, "iptables");
    dialog->platformChanged();
    QTest::keyClicks(code, "code for iptables");
    QTest::keyClick(code, Qt::Key_Tab);
    QTest::keyClick(code, Qt::Key_Enter);
    QTest::qWait(100);
    QVERIFY(service->getCodeForPlatform("iptables") == "code for iptables");

    // platform code should clear when we change platform to one that currently does not have code
    selectComboItem(platform, "PF");
    dialog->platformChanged();
    QVERIFY(code->text().isEmpty());
    // platform code for iptables should not change when setting code for PF or changing platform
    QVERIFY(service->getCodeForPlatform("iptables") == "code for iptables");
    QTest::keyClicks(code, "code for pf");
    QTest::keyClick(code, Qt::Key_Enter);
    QVERIFY(service->getCodeForPlatform("iptables") == "code for iptables");
    QVERIFY(service->getCodeForPlatform("pf") == "code for pf");

    // testing saving platform code to service object for all platforms
    QMap<QString, QString> platforms = getAllPlatforms();
    foreach (QString key, platforms.keys())
    {
        selectComboItem(platform, platforms[key]);
        dialog->platformChanged();
        code->clear();
        QTest::keyClicks(code, "code for "+key);
        QTest::keyClick(code, Qt::Key_Tab);
        QTest::keyClick(code, Qt::Key_Enter);
        QTest::qWait(100);
        QVERIFY2(service->getCodeForPlatform(key.toStdString().c_str()) == (string("code for ")+key.toStdString()),
                 (string("failed for platform ") + key.toStdString()).c_str());
    }

    // testing changing protocol to tcp
    selectComboItem(platform, "iptables");
    dialog->platformChanged();
    selectComboItem(protocol, "tcp");
    dialog->applyChanges();
    QTest::qWait(100);
    QVERIFY(service->getProtocol() == "tcp");

    selectComboItem(platform, "pf");
    dialog->platformChanged();
    QVERIFY(service->getProtocol() == "tcp");

    // testing saving address family to service object
    selectComboItem(platform, "iptables");
    dialog->platformChanged();
    QTest::mouseClick(ipv6, Qt::LeftButton, Qt::NoModifier, QPoint(10,10));
    QVERIFY(service->getAddressFamily() == AF_INET6);
    selectComboItem(platform, "PF");
    dialog->platformChanged();
    QVERIFY(service->getAddressFamily() == AF_INET6);

    // testing that changing address family does not change platform code
    foreach (QString key, platforms.keys())
    {
        string oldcode = service->getCodeForPlatform(key.toStdString().c_str());
        string oldprotocol = service->getProtocol();
        QTest::mouseClick(ipv4, Qt::LeftButton, Qt::NoModifier, QPoint(10,10));
        QVERIFY(service->getAddressFamily() == AF_INET);
        QVERIFY(oldcode == service->getCodeForPlatform(key.toStdString().c_str()));
        QVERIFY(oldprotocol == service->getProtocol());
        QTest::mouseClick(ipv6, Qt::LeftButton, Qt::NoModifier, QPoint(10,10));
        QVERIFY(service->getAddressFamily() == AF_INET6);
        QVERIFY(oldcode == service->getCodeForPlatform(key.toStdString().c_str()));
        QVERIFY(oldprotocol == service->getProtocol());
    }

}
