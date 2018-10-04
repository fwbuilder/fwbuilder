/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: FirewallDialogTestTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "FirewallDialogTest.h"

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
#include "FirewallDialog.h"
#include "StartTipDialog.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "FirewallDialogTest.h"
#include "fwbuilder/IPService.h"
#include "FWBApplication.h"

using namespace std;
using namespace libfwbuilder;

void FirewallDialogTest::initTestCase()
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

Library* FirewallDialogTest::findUserLibrary()
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

void FirewallDialogTest::rejectDialog()
{
    QMessageBox *box = dynamic_cast<QMessageBox*>(app->activeModalWidget());
    Q_ASSERT(box != nullptr);
    box->reject();
}


void FirewallDialogTest::testDialog()
{
    firewall = Firewall::cast(
        om->createObject(
            FWBTree().getStandardSlotForObject(findUserLibrary(),
                                               Firewall::TYPENAME),
            Firewall::TYPENAME, "TestFirewall"));

    QTest::qWait(1000);

    FirewallDialog *dialog = mw->findChild<FirewallDialog*>("w_FirewallDialog");
    QVERIFY(dialog != nullptr);

    QLineEdit *obj_name = dialog->findChild<QLineEdit*>("obj_name");
    TextEditWidget *comment = dialog->findChild<TextEditWidget*>("comment");

    QComboBox *platform = dialog->findChild<QComboBox*>("platform");
    QComboBox *version = dialog->findChild<QComboBox*>("version");
    QComboBox *hostOS = dialog->findChild<QComboBox*>("hostOS");

    QCheckBox *inactive = dialog->findChild<QCheckBox*>("inactive");

    QLabel *last_modified = dialog->findChild<QLabel*>("last_modified");
    QLabel *last_compiled = dialog->findChild<QLabel*>("last_compiled");
    QLabel *last_installed = dialog->findChild<QLabel*>("last_installed");

    // setting object name

    obj_name->clear();
    QTest::keyClicks(obj_name, "TestFirewallName");
    QTimer::singleShot(10, this, SLOT(rejectDialog()));
    QTest::keyClick(obj_name, Qt::Key_Enter);
    QVERIFY(firewall->getName() == "TestFirewallName");

    // setting comment
    comment->clear();
    QTest::qWait(2000);
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(obj_name, Qt::LeftButton);
    //QTest::mouseClick(comment, Qt::LeftButton);
    //QTest::keyClick(comment, Qt::Key_Tab);
    qDebug() << "Dialog comment text=" << comment->toPlainText();
    qDebug() << "Object comment=" << QString(firewall->getComment().c_str());
    QVERIFY (firewall->getComment() == "Test comment");

    // switching inactive
    QTest::mouseClick(inactive, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(firewall->getInactive() == true);
    QTest::mouseClick(inactive, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(firewall->getInactive() == false);

    // Testing that platform changes in firewall object and
    // version and hostOS combo boxes changes their values
    // when changing platform
    string oldplatform = firewall->getStr("platform");
    bool versionChanges = false;
    bool osChanges = false;
    QString versionstr = version->currentText();
    QString hostOSstr = hostOS->currentText();
    for (int i=0; i<platform->count(); i++)
    {
        if (platform->itemText(i).isEmpty()) continue;
        oldplatform = firewall->getStr("platform");
        platform->setCurrentIndex(i);
        dialog->changed();
        QVERIFY2(firewall->getStr("platform") != oldplatform, oldplatform.c_str());
        if (version->currentText() != versionstr)
            versionChanges = true;
        if (hostOS->currentText() != hostOSstr)
            osChanges = true;
    }
    QVERIFY(osChanges);
    QVERIFY(versionChanges);

    // setting platform to iptables to test version changing
    int iptidx = -1;
    for (int i=0; i<platform->count(); i++)
    {
        if (platform->itemText(i) == "iptables")
        {
            iptidx = i;
            break;
        }
    }
    platform->setCurrentIndex(iptidx);
    dialog->changed();

    version->setCurrentIndex(0);
    dialog->changed();
    for (int i=1; i<version->count(); i++)
    {
        string oldversion = firewall->getStr("version");
        version->setCurrentIndex(i);
        dialog->changed();
        QVERIFY(oldversion != firewall->getStr("version"));
    }

    // Checking that host OS in firewall object changes when switching
    // it in combo box
    hostOS->setCurrentIndex(0);
    dialog->changed();
    for (int i=1; i<hostOS->count(); i++)
    {
        string oldos = firewall->getStr("host_OS");
        hostOS->setCurrentIndex(i);
        dialog->changed();
        QVERIFY(oldos != firewall->getStr("host_OS"));
    }


    // Checking last_modified, last_compiled and last_installed labels
    QDateTime dt;
    time_t t;

    firewall->setInt("lastModified", 0);
    dialog->changed();
    QVERIFY(last_modified->text() == "-");

    firewall->setInt("lastModified", 123456789);
    t = 123456789;
    dt.setTime_t(t);
    dialog->changed();
    QVERIFY(last_modified->text() == dt.toString());


    firewall->setInt("lastCompiled", 0);
    dialog->changed();
    QVERIFY(last_compiled->text() == "-");

    firewall->setInt("lastCompiled", 123456789);
    t = 123456789;
    dt.setTime_t(t);
    dialog->changed();
    QVERIFY(last_compiled->text() == dt.toString());


    firewall->setInt("lastInstalled", 0);
    dialog->changed();
    QVERIFY(last_installed->text() == "-");

    firewall->setInt("lastInstalled", 123456789);
    t = 123456789;
    dt.setTime_t(t);
    dialog->changed();
    QVERIFY(last_installed->text() == dt.toString());
}
