/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: AddressRangeDialogTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "AddressRangeDialogTest.h"

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
#include "FWBApplication.h"
#include "FWObjectClipboard.h"
#include "TextEditWidget.h"
#include "fwbuilder/AddressRange.h"
#include "StartTipDialog.h"
#include "FWBTree.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/FWObjectDatabase.h"

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

void AddressRangeDialogTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();
    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    if (d!=nullptr) d->close();
    QTest::qWait(10);
}

void AddressRangeDialogTest::checkMessageBox()
{
    QVERIFY(app->activeModalWidget()->metaObject()->className() == QMessageBox().metaObject()->className());
    QVERIFY(dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("300.300.300.300") ||
            dynamic_cast<QMessageBox*>(app->activeModalWidget())->text().contains("200.200.200.200") );
    dynamic_cast<QMessageBox*>(app->activeModalWidget())->reject();
}

void AddressRangeDialogTest::editSelectedObject()
{
//NOTUSED    QToolButton* newButton = mw->findChild<QToolButton*>("newButton");

    ObjectManipulator* om = dynamic_cast<ObjectManipulator*>(
        mw->getCurrentObjectTree()->parent()->parent());
    om->createObject(
        FWBTree().getStandardSlotForObject(
            om->getCurrentLib(), AddressRange::TYPENAME),
        AddressRange::TYPENAME, "Address Range");

    QTest::qWait(100);
    QTreeWidgetItem *item = mw->getCurrentObjectTree()->findItems("Address Range", Qt::MatchRecursive | Qt::MatchExactly, 0).first();
    mw->getCurrentObjectTree()->setCurrentItem(item, 0, QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);

    om->editSelectedObject();
    QTest::qWait(100);

    om->editSelectedObject();

    QWidget *address_range_dialog = mw->findChild<QWidget*>("w_AddressRangeDialog");
    QVERIFY(address_range_dialog != nullptr);

    QLineEdit *rangeStart = address_range_dialog->findChildren<QLineEdit*>("rangeStart").first();
    QLineEdit *rangeEnd = address_range_dialog->findChildren<QLineEdit*>("rangeEnd").first();
    QLineEdit *objName = address_range_dialog->findChild<QLineEdit*>("obj_name");
    QVERIFY(objName != nullptr);

    TextEditWidget *comment = address_range_dialog->findChild<TextEditWidget*>("comment");
    QVERIFY(comment != nullptr);

    objName->clear();
    QTest::keyClicks(objName, "TestAddressRange");
    QTest::keyClick(objName, Qt::Key_Enter);
    QVERIFY (dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject()->getName() == "TestAddressRange");

    QTest::qWait(100);

    comment->clear();
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClicks(comment, "Test comment");
    QTest::mouseClick(comment, Qt::LeftButton);
    QTest::keyClick(comment, Qt::Key_Tab);
    QTest::qWait(100);

    QVERIFY (dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject()->getComment() == "Test comment");


    rangeStart->clear();
    QTest::keyClicks(rangeStart, "1.1.1.1");
    QTest::keyClick(rangeStart, Qt::Key_Enter);
    QVERIFY (libfwbuilder::AddressRange::cast(dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject())->getRangeStart().toString() == "1.1.1.1");
    QTest::qWait(100);

    rangeEnd->clear();
    QTest::keyClicks(rangeEnd, "1.1.1.10");
    QTest::keyClick(rangeEnd, Qt::Key_Enter);
    QVERIFY (libfwbuilder::AddressRange::cast(dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject())->getRangeEnd().toString() == "1.1.1.10");


    QTimer::singleShot(1000, this, SLOT(checkMessageBox()));
    rangeStart->clear();
    QTest::keyClicks(rangeStart, "300.300.300.300");
    QTest::keyClick(rangeStart, Qt::Key_Enter);


    rangeStart->clear();
    QTest::keyClicks(rangeStart, "200.200.200.200");
    QTest::keyClick(rangeStart, Qt::Key_Enter);
    QTest::qWait(100);
    QTimer::singleShot(100, this, SLOT(checkMessageBox()));
    rangeEnd->clear();
    QTest::keyClicks(rangeEnd, "300.300.300.300");
    QTest::keyClick(rangeEnd, Qt::Key_Enter);

    QTest::qWait(1000);
}

