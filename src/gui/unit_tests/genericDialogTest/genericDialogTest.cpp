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
    d->close();
    om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
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

bool genericDialogTest::testControl(QWidget *control)
{
    QString className = control->metaObject()->className();
    if (dynamic_cast<QSpinBox*>(control) != NULL)
    {
        QSpinBox *box = dynamic_cast<QSpinBox*>(control);
        QTest::keyClick(box, Qt::Key_Up);
        QTest::keyClick(box, Qt::Key_Up);
        QTest::keyClick(box, Qt::Key_Enter);
        QTest::keyClick(box, Qt::Key_Tab);
    }
    else if (dynamic_cast<QLineEdit*>(control) != NULL)
    {
        QLineEdit *line = dynamic_cast<QLineEdit*>(control);
        line->clear();
        QTest::keyClicks(line, QString("Some text for %1").arg(line->objectName()));
        QTest::keyClick(line, Qt::Key_Enter);
    }
    else if (dynamic_cast<QCheckBox*>(control) != NULL)
    {
        QCheckBox *box = dynamic_cast<QCheckBox*>(control);
        QTest::mouseClick(box, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    }
    else if (dynamic_cast<QRadioButton*>(control) != NULL)
    {
        QRadioButton *box = dynamic_cast<QRadioButton*>(control);
        // if it is not checked, jut clicking it
        if (!box->isChecked())
            QTest::mouseClick(box, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
        else
        { 
            if (box->group()->buttons().count() < 2) return false;
            // looking for first radio button in same group that is not checked and clicking it
            foreach(QAbstractButton *button, box->group()->buttons())
            {
                if (button->isChecked() == false && button != box)
                {
                    QTest::mouseClick(button, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
                    break;
                }
            }
        }
    }
    else if (dynamic_cast<QComboBox*>(control) != NULL)
    {
        QComboBox *box = dynamic_cast<QComboBox*>(control);
        if (box->count() < 2) return false;
        box->setCurrentIndex((box->currentIndex() + 1) % box->count());
    }
    else return false;

    return true;
}

void genericDialogTest::testDialog(QDialog *dialog)
{
    QList<QWidget*> widgets = scanDialog(dialog);
    Firewall *oldfw = dynamic_cast<Firewall*>(mw->db()->create(Firewall::TYPENAME));
    for (int i=0; i<widgets.size(); i++)
    {
        oldfw->duplicate(firewall);

        QWidget *widget = widgets.at(i);
        dialog->open();
        if (!widget->isVisible()) continue;
        if (!testControl(scanDialog(dialog).at(i)))
        {
            QWARN(QString("Dont know how to test widget %1. It might be unknown class, empty QComboBox or QRadioButton with not other QRadio button in group.")
                  .arg(widgets.at(i)->objectName()).toAscii().data());
            continue;
        }
        dialog->accept();
        QVERIFY2(!oldfw->getOptionsObject()->cmp(firewall->getOptionsObject(), true),
                 QString("Widget %1 does not affect object").arg(widget->objectName()).toAscii().data());
    }
}

void genericDialogTest::testFirewallSettingsDialog_iptables()
{
    firewall = Firewall::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), Firewall::TYPENAME), Firewall::TYPENAME, "TestFirewall"));
    firewall->setStr("platform", "iptables");
    firewall->setStr("host_OS", "linux24");

    QDialog *dialog = dynamic_cast<QDialog*>(DialogFactory::createFWDialog(mw, firewall));
    testDialog(dialog);
}

void genericDialogTest::testHostOSSettingsDialog_linux24()
{
    firewall = Firewall::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), Firewall::TYPENAME), Firewall::TYPENAME, "TestFirewall"));
    firewall->setStr("platform", "iptables");
    firewall->setStr("host_OS", "linux24");

    QDialog *dialog = dynamic_cast<QDialog*>(DialogFactory::createOSDialog(mw, firewall));
    testDialog(dialog);
}

Library* genericDialogTest::findUserLibrary()
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
