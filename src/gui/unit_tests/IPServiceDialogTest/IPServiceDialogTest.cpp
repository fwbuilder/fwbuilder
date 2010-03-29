/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: IPServiceDialogTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "IPServiceDialogTest.h"

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
#include "IPServiceDialogTest.h"
#include "StartTipDialog.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/Interface.h"
#include "IPServiceDialog.h"
#include "fwbuilder/IPService.h"

using namespace std;
using namespace libfwbuilder;

void IPServiceDialogTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();
    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    d->close();
    om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    QTest::qWait(1000);
}

Library* IPServiceDialogTest::findUserLibrary()
{
    Library *lib = NULL;
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


void IPServiceDialogTest::testTOS()
{
    IPService *service = IPService::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), IPService::TYPENAME), IPService::TYPENAME, "testIPService-TOS"));
    om->editObject(service);

    IPServiceDialog *dialog = mw->findChild<IPServiceDialog*>("w_IPServiceDialog");
    QRadioButton *use_tos = dialog->findChild<QRadioButton*>("use_tos");
    QRadioButton *use_dscp = dialog->findChild<QRadioButton*>("use_dscp");
    QLineEdit *code = dialog->findChild<QLineEdit*>("code");
    QLineEdit *obj_name = dialog->findChild<QLineEdit*>("obj_name");
    QSpinBox *protocolNum = dialog->findChild<QSpinBox*>("protocolNum");
    TextEditWidget *comment = dialog->findChild<TextEditWidget*>("comment");
    //options:
    QCheckBox *any_opt = dialog->findChild<QCheckBox*>("any_opt");
    QCheckBox *lsrr = dialog->findChild<QCheckBox*>("lsrr");
    QCheckBox *ssrr = dialog->findChild<QCheckBox*>("ssrr");
    QCheckBox *rr = dialog->findChild<QCheckBox*>("rr");
    QCheckBox *timestamp = dialog->findChild<QCheckBox*>("timestamp");
    QCheckBox *router_alert = dialog->findChild<QCheckBox*>("router_alert");

    QCheckBox *all_fragm = dialog->findChild<QCheckBox*>("all_fragments");
    QCheckBox *short_fragm = dialog->findChild<QCheckBox*>("short_fragments");

    obj_name->clear();
    QTest::keyClicks(obj_name, "TestIPService");
    QTest::keyClick(obj_name, Qt::Key_Enter);
    QVERIFY(service->getName() == "TestIPService");

    QTest::mouseClick(use_tos, Qt::LeftButton);
    QTest::keyClick(protocolNum, Qt::Key_Up);
    QTest::keyClick(protocolNum, Qt::Key_Up);
    QTest::keyClick(protocolNum, Qt::Key_Enter);
    QVERIFY(service->getProtocolNumber() == 2);

    QTest::keyClicks(code, "10");
    QTest::keyClick(code, Qt::Key_Enter);
    QVERIFY(service->getTOSCode() == "10");

    QTest::mouseClick(any_opt, Qt::LeftButton);
    service->getBool("any_opt") == true;    
    QTest::mouseClick(any_opt, Qt::LeftButton);
    service->getBool("any_opt") == false;

    QTest::mouseClick(lsrr, Qt::LeftButton);
    service->getBool("lsrr") == true;
    QTest::mouseClick(lsrr, Qt::LeftButton);
    service->getBool("lsrr") == false;

    QTest::mouseClick(ssrr, Qt::LeftButton);
    service->getBool("ssrr") == true;
    QTest::mouseClick(ssrr, Qt::LeftButton);
    service->getBool("ssrr") == false;

    QTest::mouseClick(rr, Qt::LeftButton);
    service->getBool("rr") == true;
    QTest::mouseClick(rr, Qt::LeftButton);
    service->getBool("rr") == false;

    QTest::mouseClick(timestamp, Qt::LeftButton);
    service->getBool("ts") == true;
    QTest::mouseClick(timestamp, Qt::LeftButton);
    service->getBool("ts") == false;

    QTest::mouseClick(router_alert, Qt::LeftButton);
    service->getBool("rtralt") == true;
    QTest::mouseClick(router_alert, Qt::LeftButton);
    service->getBool("rtralt") == false;

    QTest::mouseClick(all_fragm, Qt::LeftButton);
    service->getBool("fragm") == true;
    QTest::mouseClick(all_fragm, Qt::LeftButton);
    service->getBool("fragm") == false;

    QTest::mouseClick(short_fragm, Qt::LeftButton);
    service->getBool("short_fragm") == true;
    QTest::mouseClick(short_fragm, Qt::LeftButton);
    service->getBool("short_fragm") == false;

}

void IPServiceDialogTest::testDSCP()
{   

    IPService *service = IPService::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), IPService::TYPENAME), IPService::TYPENAME, "testIPService-DSCP"));
    om->editObject(service);

    IPServiceDialog *dialog = mw->findChild<IPServiceDialog*>("w_IPServiceDialog");
    QRadioButton *use_tos = dialog->findChild<QRadioButton*>("use_tos");
    QRadioButton *use_dscp = dialog->findChild<QRadioButton*>("use_dscp");
    QLineEdit *code = dialog->findChild<QLineEdit*>("code");
    QLineEdit *obj_name = dialog->findChild<QLineEdit*>("obj_name");
    QSpinBox *protocolNum = dialog->findChild<QSpinBox*>("protocolNum");
    TextEditWidget *comment = dialog->findChild<TextEditWidget*>("comment");
    //options:
    QCheckBox *any_opt = dialog->findChild<QCheckBox*>("any_opt");
    QCheckBox *lsrr = dialog->findChild<QCheckBox*>("lsrr");
    QCheckBox *ssrr = dialog->findChild<QCheckBox*>("ssrr");
    QCheckBox *rr = dialog->findChild<QCheckBox*>("rr");
    QCheckBox *timestamp = dialog->findChild<QCheckBox*>("timestamp");
    QCheckBox *router_alert = dialog->findChild<QCheckBox*>("router_alert");

    QCheckBox *all_fragm = dialog->findChild<QCheckBox*>("all_fragments");
    QCheckBox *short_fragm = dialog->findChild<QCheckBox*>("short_fragments");

    obj_name->clear();
    QTest::keyClicks(obj_name, "TestIPService-2");
    QTest::keyClick(obj_name, Qt::Key_Enter);
    QVERIFY(service->getName() == "TestIPService-2");

    QTest::mouseClick(use_dscp, Qt::LeftButton);
    QTest::keyClick(protocolNum, Qt::Key_Up);
    QTest::keyClick(protocolNum, Qt::Key_Up);
    QTest::keyClick(protocolNum, Qt::Key_Enter);
    QVERIFY(service->getProtocolNumber() == 2);

    QTest::keyClicks(code, "10");
    QTest::keyClick(code, Qt::Key_Enter);
    QVERIFY(service->getDSCPCode() == "10");

    QTest::mouseClick(any_opt, Qt::LeftButton);
    service->getBool("any_opt") == true;
    QTest::mouseClick(any_opt, Qt::LeftButton);
    service->getBool("any_opt") == false;

    QTest::mouseClick(lsrr, Qt::LeftButton);
    service->getBool("lsrr") == true;
    QTest::mouseClick(lsrr, Qt::LeftButton);
    service->getBool("lsrr") == false;

    QTest::mouseClick(ssrr, Qt::LeftButton);
    service->getBool("ssrr") == true;
    QTest::mouseClick(ssrr, Qt::LeftButton);
    service->getBool("ssrr") == false;

    QTest::mouseClick(rr, Qt::LeftButton);
    service->getBool("rr") == true;
    QTest::mouseClick(rr, Qt::LeftButton);
    service->getBool("rr") == false;

    QTest::mouseClick(timestamp, Qt::LeftButton);
    service->getBool("ts") == true;
    QTest::mouseClick(timestamp, Qt::LeftButton);
    service->getBool("ts") == false;

    QTest::mouseClick(router_alert, Qt::LeftButton);
    service->getBool("rtralt") == true;
    QTest::mouseClick(router_alert, Qt::LeftButton);
    service->getBool("rtralt") == false;

    QTest::mouseClick(all_fragm, Qt::LeftButton);
    service->getBool("fragm") == true;
    QTest::mouseClick(all_fragm, Qt::LeftButton);
    service->getBool("fragm") == false;

    QTest::mouseClick(short_fragm, Qt::LeftButton);
    service->getBool("short_fragm") == true;
    QTest::mouseClick(short_fragm, Qt::LeftButton);
    service->getBool("short_fragm") == false;
}
