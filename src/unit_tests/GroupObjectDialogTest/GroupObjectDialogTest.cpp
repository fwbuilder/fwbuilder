/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: GroupObjectDialogTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "GroupObjectDialogTest.h"

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
#include "FWBTree.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "GroupObjectDialogTest.h"
#include "StartTipDialog.h"
#include "fwbuilder/FWObjectDatabase.h"
#include "fwbuilder/FWServiceReference.h"
#include "FWBApplication.h"

#include "GroupObjectDialog.h"

using namespace std;
using namespace libfwbuilder;


void GroupObjectDialogTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();
    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    if (d!=nullptr) d->close();
    om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    QTest::qWait(1000);


    address = createObject<IPv4>("address");
    address6 = createObject<IPv6>("address6");
    network = createObject<Network>("network");
    network6 = createObject<NetworkIPv6>("netowork6");
    range = createObject<AddressRange>("range");
    table = createObject<AddressTable>("table");
    dns = createObject<DNSName>("dns");
    ipservice = createObject<IPService>("ipservice");
    icmpservice = createObject<ICMPService>("icmpservice");
    udpservice = createObject<UDPService>("udpservice");
    tcpservice = createObject<TCPService>("tcpservice");
    tagservice = createObject<TagService>("tagservice");
    userservice = createObject<UserService>("userservice");

    FWObject *object_group = createObject<ObjectGroup>("Object Group");
    FWObject *service_group = createObject<ServiceGroup>("Service Group");

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


void GroupObjectDialogTest::checkMessageBox()
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

void GroupObjectDialogTest::checkNoMessageBox()
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

Library* GroupObjectDialogTest::findUserLibrary()
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

template <class TYPE> ObjectTreeViewItem* findItemForType(ObjectTreeView *tree)
{
    foreach(QTreeWidgetItem* item, tree->findItems("", Qt::MatchRecursive | Qt::MatchContains, 0))
    {
        if (TYPE::isA(dynamic_cast<ObjectTreeViewItem*>(item)->getFWObject()) != nullptr)
            return dynamic_cast<ObjectTreeViewItem*>(item);
    }
    return nullptr;
}

template <class FWTYPE> FWTYPE* GroupObjectDialogTest::createObject(QString name)
{
    return FWTYPE::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), FWTYPE::TYPENAME), FWTYPE::TYPENAME, name));
}

bool checkObjectInsertion(GroupObjectDialog *dlg, Group *grp, FWObject *obj)
{
    qDebug() << "Trying to add object"
             << obj->getName().c_str() << "(" << obj->getTypeName().c_str() << ")"
             << "to group" << grp->getPath().c_str();

    dlg->insertObject(obj);
    dlg->applyChanges();

    for (Group::iterator i = grp->begin(); i!= grp->end(); i++)
    {
        if (FWReference::cast(*i)->getPointer() == obj)
            return true;
    }

    //object was not inserted into group
    return false;
}

/*
 * Test insertion of objects of different types into an object group
 */
void GroupObjectDialogTest::testObjectGroup()
{
    ObjectTreeViewItem *groupItem = dynamic_cast<ObjectTreeViewItem*>(
        mw->getCurrentObjectTree()->findItems(
            "Object Group", Qt::MatchRecursive, 0).first());

    QVERIFY(groupItem != nullptr);

    mw->getCurrentObjectTree()->setCurrentItem(
        groupItem, 0,
        QItemSelectionModel::SelectCurrent | QItemSelectionModel::Clear);

    ObjectGroup *group = ObjectGroup::cast(groupItem->getFWObject());
    QVERIFY(group != nullptr);
    om->editObject(group);
    QTest::qWait(50);

    GroupObjectDialog *groupdialog = mw->findChild<GroupObjectDialog*>(
        "w_ObjectGroupDialog");

    QVERIFY(groupdialog != nullptr);

    QVERIFY(checkObjectInsertion(groupdialog, group, address));
    QVERIFY(checkObjectInsertion(groupdialog, group, address6));
    QVERIFY(checkObjectInsertion(groupdialog, group, network));
    QVERIFY(checkObjectInsertion(groupdialog, group, network6));
    QVERIFY(checkObjectInsertion(groupdialog, group, range));
    QVERIFY(checkObjectInsertion(groupdialog, group, table));
    QVERIFY(checkObjectInsertion(groupdialog, group, dns));

    QVERIFY(checkObjectInsertion(groupdialog, group, group));

    QVERIFY(!checkObjectInsertion(groupdialog, group, ipservice));
    QVERIFY(!checkObjectInsertion(groupdialog, group, icmpservice));
    QVERIFY(!checkObjectInsertion(groupdialog, group, udpservice));
    QVERIFY(!checkObjectInsertion(groupdialog, group, tcpservice));
    QVERIFY(!checkObjectInsertion(groupdialog, group, tagservice));
    QVERIFY(!checkObjectInsertion(groupdialog, group, userservice));

    QPushButton *newButton = mw->findChild<GroupObjectDialog*>(
        "w_ObjectGroupDialog")->findChild<QPushButton*>("newButton");
    QVERIFY(newButton != nullptr);

    QTimer::singleShot(50, this, SLOT(checkObjectGroupMenu()));
    QTest::mouseClick(newButton, Qt::LeftButton);
}

void GroupObjectDialogTest::checkObjectGroupMenu()
{
    QMenu *menu = mw->findChild<GroupObjectDialog*>("w_ObjectGroupDialog")->findChild<QMenu*>("GroupObjectDialog_newObjectMenu");

    QList<QString> names;
    names << (QString("newObject_") + Host::TYPENAME)
          << (QString("newObject_") + Firewall::TYPENAME)
          << (QString("newObject_") + Cluster::TYPENAME)
          << (QString("newObject_") + Network::TYPENAME)
          << (QString("newObject_") + NetworkIPv6::TYPENAME)
          << (QString("newObject_") + IPv4::TYPENAME)
          << (QString("newObject_") + IPv6::TYPENAME)
          << (QString("newObject_") + DNSName::TYPENAME)
          << (QString("newObject_") + AddressRange::TYPENAME)
          << (QString("newObject_") + AddressTable::TYPENAME);
    QList<QString> menuNames;

    foreach(QAction *act, menu->actions())
        menuNames.append(act->objectName());

    QVERIFY(menuNames.size() == names.size());
    foreach(QString name, names)
        QVERIFY(menuNames.contains(name));
    menu->close();
}

void GroupObjectDialogTest::checkObjectServiceGroupMenu()
{
    QMenu *menu = mw->findChild<GroupObjectDialog*>("w_ServiceGroupDialog")->findChild<QMenu*>("GroupObjectDialog_newObjectMenu");

    QList<QString> names;
    names << (QString("newObject_") + IPService::TYPENAME)
          << (QString("newObject_") + ICMPService::TYPENAME)
          << (QString("newObject_") + ICMP6Service::TYPENAME)
          << (QString("newObject_") + TCPService::TYPENAME)
          << (QString("newObject_") + UDPService::TYPENAME)
          << (QString("newObject_") + CustomService::TYPENAME)
          << (QString("newObject_") + TagService::TYPENAME)
          << (QString("newObject_") + UserService::TYPENAME);
    QList<QString> menuNames;

    foreach(QAction *act, menu->actions())
        menuNames.append(act->objectName());

    QVERIFY(menuNames.size() == names.size());
    foreach(QString name, names)
        QVERIFY(menuNames.contains(name));
    menu->close();
}


void GroupObjectDialogTest::testServiceGroup()
{
    ObjectTreeViewItem *groupItem = dynamic_cast<ObjectTreeViewItem*>(
        mw->getCurrentObjectTree()->findItems(
            "Service Group", Qt::MatchRecursive, 0).first());

    mw->getCurrentObjectTree()->setCurrentItem(
        groupItem, 0,
        QItemSelectionModel::SelectCurrent | QItemSelectionModel::Clear);

    QVERIFY(groupItem != nullptr);
    ServiceGroup *group = ServiceGroup::cast(groupItem->getFWObject());
    QVERIFY(group != nullptr);
    om->editObject(group);
    QTest::qWait(50);

    GroupObjectDialog *groupdialog = mw->findChild<GroupObjectDialog*>(
        "w_ServiceGroupDialog");
    QVERIFY(groupdialog != nullptr);

    QVERIFY(!checkObjectInsertion(groupdialog, group, address));
    QVERIFY(!checkObjectInsertion(groupdialog, group, address6));
    QVERIFY(!checkObjectInsertion(groupdialog, group, network));
    QVERIFY(!checkObjectInsertion(groupdialog, group, network6));
    QVERIFY(!checkObjectInsertion(groupdialog, group, range));
    QVERIFY(!checkObjectInsertion(groupdialog, group, table));
    QVERIFY(!checkObjectInsertion(groupdialog, group, dns));

    QVERIFY(checkObjectInsertion(groupdialog, group, group));
    FWObject *objGroup = dynamic_cast<ObjectTreeViewItem*>(mw->getCurrentObjectTree()->findItems("Object Group", Qt::MatchRecursive, 0).first())->getFWObject();
    QVERIFY(!checkObjectInsertion(groupdialog, group, objGroup));
    QVERIFY(checkObjectInsertion(groupdialog, group, ipservice));
    QVERIFY(checkObjectInsertion(groupdialog, group, icmpservice));
    QVERIFY(checkObjectInsertion(groupdialog, group, udpservice));
    QVERIFY(checkObjectInsertion(groupdialog, group, tcpservice));
    QVERIFY(checkObjectInsertion(groupdialog, group, tagservice));
    QVERIFY(checkObjectInsertion(groupdialog, group, userservice));


    QPushButton *newButton = mw->findChild<GroupObjectDialog*>("w_ServiceGroupDialog")->findChild<QPushButton*>("newButton");
    QVERIFY(newButton != nullptr);
    QTimer::singleShot(50, this, SLOT(checkObjectServiceGroupMenu()));
    QTest::mouseClick(newButton, Qt::LeftButton);
}
