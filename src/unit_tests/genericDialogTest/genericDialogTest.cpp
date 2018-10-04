/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: genericDialogTest.cpp 2723 2010-03-16 17:32:18Z a2k $

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

#include "genericDialogTest.h"

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
#include <QRadioButton>
#include <QMessageBox>
#include <QWidget>
#include <QLineEdit>
#include <QUndoStack>
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
#include "genericDialogTest.h"
#include "fwbuilder/IPService.h"
#include "DialogFactory.h"
#include "FWCmdChange.h"
#include "RuleOptionsDialog.h"
#include "fwbuilder/Rule.h"
#include "fwbuilder/Policy.h"
#include "fwbuilder/Routing.h"
#include "fwbuilder/NAT.h"
#include "RoutingRuleOptionsDialog.h"
#include "platforms.h"
#include "NATRuleOptionsDialog.h"

using namespace std;
using namespace libfwbuilder;

void genericDialogTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();
    mw->resize(1200, 600);
    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    if (d!=nullptr) d->close();
    om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    init_platforms();
    QTest::qWait(1000);
}

QList<QWidget*> genericDialogTest::scanDialog(QWidget *dialog)
{
    QList<QWidget*> result;
    foreach(QLineEdit *item, dialog->findChildren<QLineEdit*>())
        result.append(dynamic_cast<QWidget*>(item));
    foreach(QCheckBox *item, dialog->findChildren<QCheckBox*>())
        result.append(dynamic_cast<QWidget*>(item));
    foreach(QComboBox *item, dialog->findChildren<QComboBox*>())
        result.append(dynamic_cast<QWidget*>(item));
    foreach(QRadioButton *item, dialog->findChildren<QRadioButton*>())
        result.append(dynamic_cast<QWidget*>(item));
    foreach(QTextEdit *item, dialog->findChildren<QTextEdit*>())
        result.append(dynamic_cast<QWidget*>(item));
    foreach(TextEditWidget *item, dialog->findChildren<TextEditWidget*>())
        result.append(dynamic_cast<QWidget*>(item));
    foreach(QSpinBox *item, dialog->findChildren<QSpinBox*>())
        result.append(dynamic_cast<QWidget*>(item));
    return result;
}

// Activates tab which contains widget
void genericDialogTest::activateTab(QWidget *widget)
{
    QWidget *current = widget;
    while (current->parent() != nullptr)
    {
        if (dynamic_cast<QTabWidget*>(current->parent()) != nullptr)
        {
            QTabWidget *tabs = dynamic_cast<QTabWidget*>(current->parent());
            for (int i=0; i<tabs->count(); i++)
            {
                if (tabs->widget(i)->findChildren<QWidget*>(widget->objectName()).contains(widget))
                {
                    tabs->setCurrentIndex(i);
                    break;
                }
            }
        }
        current = dynamic_cast<QWidget*>(current->parent());
    }

}

bool genericDialogTest::testControl(QWidget *control)
{
    if (dynamic_cast<QSpinBox*>(control) != nullptr)
    {
        QSpinBox *box = dynamic_cast<QSpinBox*>(control);
        QTest::keyClick(box, Qt::Key_Up);
        QTest::keyClick(box, Qt::Key_Up);
        QTest::keyClick(box, Qt::Key_Enter);
        QTest::keyClick(box, Qt::Key_Tab);
    }
    else if (dynamic_cast<QLineEdit*>(control) != nullptr)
    {
        QLineEdit *line = dynamic_cast<QLineEdit*>(control);
        line->clear();
        QTest::keyClicks(line, QString("Some text for %1").arg(line->objectName()));
        //line->setText(QString("Some text for %1").arg(control->objectName()));
        QTest::keyClick(line, Qt::Key_Enter);
    }
    else if (dynamic_cast<QCheckBox*>(control) != nullptr)
    {
        QCheckBox *box = dynamic_cast<QCheckBox*>(control);
        QTest::mouseClick(box, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    }
    else if (dynamic_cast<QRadioButton*>(control) != nullptr)
    {
        QRadioButton *box = dynamic_cast<QRadioButton*>(control);
        // if it is not checked, jut clicking it
        if (!box->isChecked())
            QTest::mouseClick(box, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
        else
        {
            QList<QAbstractButton*> buttons;
            if (box->group() == nullptr)
            {
                foreach(QRadioButton *button, box->parent()->findChildren<QRadioButton*>())
                {
                    if (button->group() == nullptr)
                        buttons.append(button);
                }

                if (buttons.isEmpty())
                {
                    qDebug() << "Can not test QRadioButton" << box << "that is not in group and has no buttons nearby.";
                   return false;
                }
            }
            else
            {
                buttons = box->group()->buttons();
            }
            if (buttons.count() < 2)
            {
                qDebug() << "Can not test QRadioButton" << box << " that is only one button in group.";
                return false;
            }
            // looking for first radio button in same group that is not checked and clicking it
            foreach(QAbstractButton *button, buttons)
            {
                if (button->isChecked() == false && button != box)
                {
                    QTest::mouseClick(button, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
                    return true;
                }
            }
        }
    }
    else if (dynamic_cast<QComboBox*>(control) != nullptr)
    {
        QComboBox *box = dynamic_cast<QComboBox*>(control);
        if (box->count() < 2)
        {
            for (int i=0; i<box->count(); i++)
                qDebug() << box->itemText(i);
            qDebug() << "Can not change value of QComboBox" << box << "which has less than two items.";
            return false;
        }
        box->setCurrentIndex((box->currentIndex() + 1) % box->count());
    }
    else if (dynamic_cast<QTextEdit*>(control) != nullptr)
    {
        QTextEdit *edit = dynamic_cast<QTextEdit*>(control);
        QTest::mouseClick(edit, Qt::LeftButton, Qt::NoModifier);
        QTest::keyClicks(edit, "Some test input for " + edit->objectName());
        QTest::mouseClick(edit, Qt::LeftButton, Qt::NoModifier);
        QTest::keyClick(edit, Qt::Key_Tab);
    }
    else return false;

    return true;
}

void genericDialogTest::testDialog(QWidget *dialog, FWObject *object)
{
    qDebug() << "testing dialog" << dialog;
    QList<QWidget*> widgets = scanDialog(dialog);
    qDebug() << "it contains" << widgets.size() << "controls";
    QList<QTabWidget*> tabs = dialog->findChildren<QTabWidget*>();
    FWObject *old = mw->db()->create(object->getTypeName());
    for (int i=0; i<widgets.size(); i++)
    {
        //qDebug() << "testing control" << widgets.at(i);
        old->duplicate(object);
        QWidget *widget = widgets.at(i);

        // Skipping QSpinBox (which inherits QLineEdit) with QLineEdit type
        // there should be another one with right type in list
        if (widget->objectName() == "qt_spinbox_lineedit") continue;

        if (dynamic_cast<QDialog*>(dialog) != nullptr)
            dynamic_cast<QDialog*>(dialog)->open();

        activateTab(widget);
        if (!widget->isVisible() || !widget->isEnabled()) continue;

        if (!testControl(widget))
        {
            QWARN(QString("Dont know how to test widget %1. It might be unknown class, empty QComboBox or QRadioButton with not other QRadio button in group.")
                  .arg(widgets.at(i)->objectName()).toLatin1().data());
            continue;
        }
        if (dynamic_cast<QDialog*>(dialog) != nullptr)
            dynamic_cast<QDialog*>(dialog)->accept();
        else
        {
            QMetaObject::invokeMethod(dialog, "changed");
            QMetaObject::invokeMethod(dialog, "applyChanges");
        }
        QVERIFY2(!old->cmp(object, true),
                 QString("Widget %1 does not affect object").arg(widget->objectName()).toLatin1().data());
    }
}

void genericDialogTest::testFirewallSettingsDialog_iptables()
{
    Firewall *firewall = Firewall::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), Firewall::TYPENAME), Firewall::TYPENAME, "TestFirewall"));
    firewall->setStr("platform", "iptables");
    firewall->setStr("host_OS", "linux24");

    QDialog *dialog = dynamic_cast<QDialog*>(DialogFactory::createFWDialog(mw, firewall));
    testDialog(dialog, firewall);
}

void genericDialogTest::testHostOSSettingsDialog_linux24()
{
    Firewall *firewall = Firewall::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), Firewall::TYPENAME), Firewall::TYPENAME, "TestFirewall"));
    firewall->setStr("platform", "iptables");
    firewall->setStr("host_OS", "linux24");

    QDialog *dialog = dynamic_cast<QDialog*>(DialogFactory::createOSDialog(mw, firewall));
    testDialog(dialog, firewall);
}

#if 0
// rule options dialog uses stacked widget with only one page visible,
// depending on the firewall platform. Some widgets in invisible pages
// are not even initialized, also depending on the platform. Need to
// devise better test that would take this into account.

void genericDialogTest::testRuleOptionsDialog()
{
    Firewall *firewall = Firewall::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), Firewall::TYPENAME), Firewall::TYPENAME, "TestFirewall"));
    QMap<QString, QString> platforms = getAllPlatforms();
    platforms.remove("unknown"); // dialog does not set options for it
    foreach(QString platform, platforms.keys())
    {
        qDebug() << "Testing platform:" << platform;
        firewall->setStr("platform", platform.toStdString());
        PolicyRule *rule = PolicyRule::cast(firewall->getPolicy()->createRule());
        firewall->getPolicy()->add(rule);
        QWidget *dialog = dynamic_cast<QWidget*>(DialogFactory::createDialog(mw->activeProject(), Rule::TYPENAME));
        dynamic_cast<BaseObjectDialog*>(dialog)->attachToProjectWindow(mw->activeProject());
        dialog->setVisible(true);
        dynamic_cast<RuleOptionsDialog*>(dialog)->loadFWObject(rule);
        testDialog(dynamic_cast<QWidget*>(dialog), FWObject::cast(rule));
    }
}

void genericDialogTest::testRoutingRuleOptionsDialog()
{
    Firewall *firewall = Firewall::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), Firewall::TYPENAME), Firewall::TYPENAME, "TestFirewall"));
/*    QMap<QString, QString> platforms = getAllPlatforms();
    platforms.remove("unknown"); // dialog does not set options for it
    foreach(QString platform, platforms.keys())
    {
        firewall->setStr("platform", platform.toStdString());
*/
    // it currently works only with iptables
    firewall->setStr("platform", "iptables");
    RoutingRule *rule = RoutingRule::cast(firewall->getRouting()->createRule());
    firewall->getRouting()->add(rule);
    QWidget *dialog = dynamic_cast<QWidget*>(DialogFactory::createDialog(mw->activeProject(), RoutingRule::TYPENAME));
    dynamic_cast<BaseObjectDialog*>(dialog)->attachToProjectWindow(mw->activeProject());
    dialog->setVisible(true);
    dynamic_cast<RoutingRuleOptionsDialog*>(dialog)->loadFWObject(rule);
    testDialog(dynamic_cast<QWidget*>(dialog), FWObject::cast(rule));
}

void genericDialogTest::testNATRuleOptionsDialog()
{
    Firewall *firewall = Firewall::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), Firewall::TYPENAME), Firewall::TYPENAME, "TestFirewall"));
    QMap<QString, QString> platforms = getAllPlatforms();
    platforms.remove("unknown"); // dialog does not set options for it
    foreach(QString platform, platforms.keys())
    {
        qDebug() << "Testing platform:" << platform;
        firewall->setStr("platform", platform.toStdString());
        NATRule *rule = NATRule::cast(firewall->getNAT()->createRule());
        firewall->getNAT()->add(rule);
        QWidget *dialog = dynamic_cast<QWidget*>(DialogFactory::createDialog(mw->activeProject(), NATRule::TYPENAME));
        dynamic_cast<BaseObjectDialog*>(dialog)->attachToProjectWindow(mw->activeProject());
        dialog->setVisible(true);
        dynamic_cast<NATRuleOptionsDialog*>(dialog)->loadFWObject(rule);
        testDialog(dynamic_cast<QWidget*>(dialog), FWObject::cast(rule));
    }
}
#endif


Library* genericDialogTest::findUserLibrary()
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
