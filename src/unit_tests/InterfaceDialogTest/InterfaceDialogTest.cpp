/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: InterfaceDialogTestTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "InterfaceDialogTest.h"

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
#include "InterfaceDialog.h"
#include "StartTipDialog.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "InterfaceDialogTest.h"
#include "fwbuilder/IPService.h"

using namespace std;
using namespace libfwbuilder;

void InterfaceDialogTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();
//    mw->resize(1200, 600);
    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    if (d!=nullptr) d->close();
    om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    QTest::qWait(1000);
    firewall = Firewall::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), Firewall::TYPENAME), Firewall::TYPENAME, "TestFirewall"));
    firewall->setStr("platform", "pix"); // using pix platforms as it supports all dialog options
    interface = Interface::cast(om->createObject(firewall, Interface::TYPENAME, "TestInterface"));
    QVERIFY(interface!=nullptr);
}

Library* InterfaceDialogTest::findUserLibrary()
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


void InterfaceDialogTest::testDialog()
{
    om->editObject(interface);
    InterfaceDialog *dialog = mw->findChild<InterfaceDialog*>("w_InterfaceDialog");
    QVERIFY(dialog != nullptr);

    QLineEdit *obj_name = dialog->findChild<QLineEdit*>("obj_name");
    QLineEdit *label = dialog->findChild<QLineEdit*>("label");
    TextEditWidget *comment = dialog->findChild<TextEditWidget*>("comment");

    //options:
    QSpinBox *seclevel = dialog->findChild<QSpinBox*>("seclevel");
    QComboBox *netzone = dialog->findChild<QComboBox*>("netzone");

    QCheckBox *management = dialog->findChild<QCheckBox*>("management");
    QCheckBox *unprotected = dialog->findChild<QCheckBox*>("unprotected");
    QCheckBox *dedicated_failover = dialog->findChild<QCheckBox*>("dedicated_failover");

    QRadioButton *regular = dialog->findChild<QRadioButton*>("regular");
    QRadioButton *dynamic = dialog->findChild<QRadioButton*>("dynamic");
    QRadioButton *unnumbered = dialog->findChild<QRadioButton*>("unnumbered");

    // setting object name
    obj_name->clear();
    QTest::keyClicks(obj_name, "TestInterfaceName");
    QTest::keyClick(obj_name, Qt::Key_Enter);
    QVERIFY(interface->getName() == "TestInterfaceName");

    // setting label
    label->clear();
    QTest::keyClicks(label, "TestInterfaceLabel");
    QTest::keyClick(label, Qt::Key_Enter);
    QVERIFY(interface->getLabel() == "TestInterfaceLabel");

    // setting comment
    comment->clear();
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClick(comment, Qt::Key_Tab);
    QTest::qWait(100);
    QVERIFY (interface->getComment() == "Test comment");

    // setting security level
    QTest::mouseClick(seclevel, Qt::LeftButton);
    QTest::keyClick(seclevel, Qt::Key_Up);
    QTest::keyClick(seclevel, Qt::Key_Up);
    QTest::keyClick(seclevel, Qt::Key_Enter);
    QVERIFY(interface->getSecurityLevel() == 2);


    // testing management
    QVERIFY(interface->isManagement() == false);
    QTest::mouseClick(management, Qt::LeftButton);
    QVERIFY(interface->isManagement() == true);
    QTest::mouseClick(management, Qt::LeftButton);
    QVERIFY(interface->isManagement() == false);

    // testing unprotected
    QVERIFY(interface->isUnprotected() == false);
    QTest::mouseClick(unprotected, Qt::LeftButton);
    QVERIFY(interface->isUnprotected() == true);
    QTest::mouseClick(unprotected, Qt::LeftButton);
    QVERIFY(interface->isUnprotected() == false);

    // testing dedicated failover
    QVERIFY(interface->isDedicatedFailover() == false);
    QTest::mouseClick(dedicated_failover, Qt::LeftButton);
    QVERIFY(interface->isDedicatedFailover() == true);
    QTest::mouseClick(dedicated_failover, Qt::LeftButton);
    QVERIFY(interface->isDedicatedFailover() == false);

    // testing regular/dynamic/unnumbered switch
    QTest::mouseClick(regular, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(interface->isRegular() == true);
    QVERIFY(interface->isDyn() == false);
    QVERIFY(interface->isUnnumbered() == false);
    QTest::mouseClick(dynamic, Qt::LeftButton);
    QVERIFY(interface->isRegular() == false);
    QVERIFY(interface->isDyn() == true);
    QVERIFY(interface->isUnnumbered() == false);
    QTest::mouseClick(unnumbered, Qt::LeftButton);
    QVERIFY(interface->isRegular() == false);
    QVERIFY(interface->isDyn() == false);
    QVERIFY(interface->isUnnumbered() == true);

    // testing that changing netzone combo value changed interface's property
    string zone = interface->getStr("network_zone");
    bool changed = false;
    for(int i=0; i<netzone->count(); i++)
    {
        netzone->setCurrentIndex(i);
        dialog->changed();
        if (interface->getStr("network_zone") != zone)
        {
            changed = true;
            break;
        }
    }
    QVERIFY(changed);
}
