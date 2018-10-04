/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: startTipDialogTest.cpp 3043 2010-06-30 14:19:36Z a2k $

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

#include "startTipDialogTest.h"
#include "FWWindow.h"
#include "FWBApplication.h"
#include "StartTipDialog.h"
#include <QFile>
#include "global.h"
#include "FWBSettings.h"
#include <QMessageBox>
#include <QTextBrowser>
#include <QDebug>
#include <QTimer>
#include "common/commoninit.h"
#include "platforms.h"

startTipDialogTest::startTipDialogTest(QWidget *parent)
    : QObject(parent)
{
}

void startTipDialogTest::testDialogAppear()
{
    st->setBool("UI/NoStartTip", false);
    st->setBool("UI/FirstRun", true);
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();

    StartTipDialog *dialog = nullptr;
    for(int i=0; i<10; i++)
    {
        qDebug() << "app->topLevelWidgets():";
        foreach (QWidget *widget, QApplication::topLevelWidgets())
        {
            if (dynamic_cast<StartTipDialog*>(widget) != nullptr)
            {
                qDebug() << widget << "isHidden()=" << widget->isHidden();
                if (widget->objectName() == "StartTipDialog_q")
                    dialog = dynamic_cast<StartTipDialog*>(widget);
            }
        }
        // qDebug() << "app->activeWindow()=" << app->activeWindow();
        // dialog = dynamic_cast<StartTipDialog*>(app->activeWindow());

        qDebug() << "--";

        if (dialog == nullptr) QTest::qWait(1000);
        else break;
    }
    Q_ASSERT(dialog != nullptr);

    QPushButton *prevTip = dialog->findChild<QPushButton*>("prevTipButton");
    QPushButton *nextTip = dialog->findChild<QPushButton*>("nextTipButton");
    QTextBrowser *textview = dialog->findChild<QTextBrowser*>("textview");

    QString oldtext = textview->toPlainText();
    QTest::mouseClick(prevTip, Qt::LeftButton);
    QVERIFY(oldtext != textview->toPlainText());
    oldtext = textview->toPlainText();
    QTest::mouseClick(nextTip, Qt::LeftButton);
    QVERIFY(oldtext != textview->toPlainText());
    dialog->reject();
    mw->hide();
    mw->deleteLater();
}

void startTipDialogTest::testDialogNotAppear()
{
    st->setBool("UI/NoStartTip", true);
    st->setBool("UI/FirstRun", false);
    mw = new FWWindow();
    mw->show();
    mw->startupLoad();

    StartTipDialog *dialog = nullptr;
    for(int i=0; i<10; i++)
    {
        dialog = dynamic_cast<StartTipDialog*>(app->activeWindow());
        if (dialog == nullptr)
            QTest::qWait(1000);
        else
            break;
    }
    Q_ASSERT(dialog == nullptr);
}

void startTipDialogTest::cleanupTestCase()
{

}
