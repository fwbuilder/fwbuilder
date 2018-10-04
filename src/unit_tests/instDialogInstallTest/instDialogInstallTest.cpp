/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: instDialogInstallTest.cpp 2786 2010-04-01 14:05:36Z a2k $

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

  ================================


  All tests in this module are designed to either fail installation or
  never supposed to complete it because they lack authentication
  credentials when they run unattended. Tests that perform successfull
  installs are located in a separate module fwbuilder-tests.


  Ticket 1358:

  using the same data file test.fwb but work with a copy because
  instDialog saves data to the disk and modifies the file

  keep common parts of all these tests in a separate function. There
  will be more tests like these in the future, the difference will be
  in the installer parameters and in the options stored in the
  firewall object. These tests check the status in the column on the
  left (Success or Error) and status of the buttons in the dialog
  (Finish). They also check if the file has been created and look for
  certain lines in the progress output panel.

  before running the test, check if files "test1.fw", "test2.fw",
  "test3.fw", "test4.fw" exist and delete them. If delete operation
  fails, fail the test. This should be done in the unit test code
  (C++) rather than in the runner shell script

  create subdirectory "test_install" in the current directory. If this
  directory exists, remove files in it.

  find firewall object test1, open it in the editor, click "Firewall
  settings" button and in the dialog change "Directory on the firewall
  where script should be installed" to the current
  directory. Alternatively, you can do this by manipulating data in
  the object instead of opening dialogs:

    FWOptions *fwoptions = firewall_object->getOptionsObject();
    fwoptions->setStr("firewall_dir", current_dir + "/test_install");


  test2

  test for "authentication failed" error:

  * before compiling and installing, set environment variable
  "SSH_AUTH_SOCK" to empty string. Save its value before destroying it
  and then restore when this test is done

  * repeat the test1 with address 127.0.0.1. Since you have turned off
  ssh-agent and left password field empty, the program should not be
  able to authenticate and you should see "Permission denied" in the
  progress output

  test3

  another kind of error is a timeout.

  * To simulate timeout repeat the test with ip address 127.0.0.2. Ssh
  should time out after some considerable time with appropriate error
  message in the output. Check for this message and make sure at that
  point button "Finish" is enabled and status is "Failed" (or is it
  "Error" ? )

  test4

  test cancellation.

  * Repeat the test using address 127.0.0.2 but click "Cancel" 1 sec
  after you start installation (this is too short for the ssh timeout
  to occur). Wait after that until the line "Stopping background
  process" appears, then check that the installer dialog closes.

*/

#include "instDialogInstallTest.h"

#include "unistd.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMenuBar>
#include <QTextBrowser>

#include "FWObjectClipboard.h"

using namespace std;
using namespace QTest;
using namespace libfwbuilder;


bool checkProgress(QTreeWidget *list)
{
    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        if ( (list->topLevelItem(i)->text(1).toStdString() == "Compiling ...") ||
             (list->topLevelItem(i)->text(1).toStdString() == "Installing ...") )
            return false;
    }
    return true;
}

void instDialogInstallTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->loadFile("test_work.fwb", false);
    ssh_auth_sock = getenv("SSH_AUTH_SOCK");
}

void instDialogInstallTest::cleanupTestCase()
{
    if (ssh_auth_sock != nullptr)
        setenv("SSH_AUTH_SOCK", ssh_auth_sock, 1);
    else
        unsetenv("SSH_AUTH_SOCK");
    QDir().rmdir("test_install");
    QFile::remove("test1.fw");
}

void instDialogInstallTest::resetDialogs()
{
    instOptionsDialog *optdlg = mw->findChild<instOptionsDialog*>("instOptionsDialog_q");
    if (optdlg && optdlg->isVisible()) optdlg->reject();
    instDialog *dlg = mw->findChild<instDialog*>();
    if (dlg && dlg->isVisible()) dlg->reject();
    QTest::qWait(500);
}

void instDialogInstallTest::fillInstOptionsDialog(const QString &user_name,
                                                  const QString &passwd,
                                                  const QString &alt_address,
                                                  bool verbose_flag)
{
    instOptionsDialog *optdlg = mw->findChild<instOptionsDialog*>("instOptionsDialog_q");
    QVERIFY(optdlg != nullptr);
    QVERIFY(optdlg->isVisible() == true);

    QLineEdit *uname = optdlg->findChild<QLineEdit*>("uname");
    QLineEdit *pwd = optdlg->findChild<QLineEdit*>("pwd");
    QLineEdit *altAddress = optdlg->findChild<QLineEdit*>("altAddress");
    QCheckBox *verbose = optdlg->findChild<QCheckBox*>("verbose");

    if (!user_name.isEmpty()) uname->setText(user_name);
    if (!passwd.isEmpty()) pwd->setText(passwd);
    if (!alt_address.isEmpty()) altAddress->setText(alt_address);
    verbose->setChecked(verbose_flag);

    optdlg->findChild<QPushButton*>("okButton")->click();
}

void instDialogInstallTest::instOptionsForTest1()
{
    // set verbose on to reveal scp and ssh command line
    fillInstOptionsDialog(QString("root"), QString(""), QString("127.0.0.1"), true);
}

void instDialogInstallTest::instOptionsForTest2()
{
    // set verbose on to reveal scp and ssh command line
    fillInstOptionsDialog(QString("root"), QString(""), QString("127.0.0.1"), true);
}

void instDialogInstallTest::instOptionsForTest3()
{
    // set verbose on to reveal scp and ssh command line
    fillInstOptionsDialog(QString("root"), QString(""), QString("192.168.254.254"), true);
}

void instDialogInstallTest::instOptionsForTest4()
{
    fillInstOptionsDialog(QString("root"), QString(""), QString("192.168.254.254"), false);
}

void instDialogInstallTest::instOptionsForTest5()
{
    fillInstOptionsDialog(QString("root"), QString(""), QString("192.168.254.254"), false);
}

void instDialogInstallTest::removeFiles()
{
    if (QFileInfo("test1.fw").exists())
        QVERIFY(QFile::remove("test1.fw"));
    if (QFileInfo("test2.fw").exists())
        QVERIFY(QFile::remove("test2.fw"));
    if (QFileInfo("test3.fw").exists())
        QVERIFY(QFile::remove("test3.fw"));
    if (QFileInfo("test4.fw").exists())
        QVERIFY(QFile::remove("test4.fw"));
    if (QDir("test_install").exists())
    {
        foreach(QString file, QDir("test_install").entryList())
        {
            if (QFileInfo("test_install/"+file).isFile())
                QVERIFY(QFile::remove("test_install/"+file));
        }
        QVERIFY(QDir().rmdir("test_install"));
    }
    QVERIFY(QDir().mkdir("test_install"));
}


void instDialogInstallTest::verifyInstallSuccess(const QString &)
{
    instDialog *dlg = mw->findChild<instDialog*>();
    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");
    QTextBrowser *processLogDisplay = dlg->findChild<QTextBrowser*>("procLogDisplay");

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QString txt = processLogDisplay->toPlainText();
        if (list->topLevelItem(i)->text(1) != "Success")
        {
            foreach(QString line, txt.split("\n"))
                qDebug() << line;
            QFAIL("Installation failure detected, see debug above for details");
        }
    }
}

/*
  test for "authentication failed" error:

  * before compiling and installing, set environment variable
  "SSH_AUTH_SOCK" to empty string. Save its value before destroying it
  and then restore when this test is done

  * repeat the test1 with address 127.0.0.1. Since you have turned off
  ssh-agent and left password field empty, the program should not be
  able to authenticate and you should see "Permission denied" in the
  progress output

*/
void instDialogInstallTest::testInstall2()
{
    resetDialogs();

    removeFiles();
    Firewall *test1 = nullptr;
    foreach(FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == "test1")
        {
            test1 = Firewall::cast(fw);
            break;
        }
    }
    QVERIFY(test1 != nullptr);

    // reset additional args for scp and ssh
    FWOptions *fwoptions = test1->getOptionsObject();
    fwoptions->setStr("scpArgs", "");
    fwoptions->setStr("sshArgs", "");

    fwoptions->setStr("firewall_dir", (QDir::currentPath()+"/test_install").toStdString());


    setenv("SSH_AUTH_SOCK", "nothinghere", 1);
    QTest::qWait(500);
    mw->findChild<QAction*>("installAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton17")->click();


    QPushButton *back = dlg->findChild<QPushButton*>("backButton");
    QPushButton *next = dlg->findChild<QPushButton*>("nextButton");
    QPushButton *finish = dlg->findChild<QPushButton*>("finishButton");
    QPushButton *cancel = dlg->findChild<QPushButton*>("cancelButton");

    QTreeWidget *selectTable = dlg->findChild<QTreeWidget*>("selectTable");
    QTreeWidgetItem *test1item = selectTable->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first();
    test1item->setCheckState(1, Qt::Checked);
    test1item->setCheckState(2, Qt::Checked);
    QTest::qWait(500);
    next->click();

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");

    int waited = 0;

    while (!checkProgress(list))
    {
        QVERIFY(back->isEnabled() == true);
        QVERIFY(next->isEnabled() == false);
        QVERIFY(cancel->isEnabled() == true);
        QVERIFY2(finish->isEnabled() == false, "Button Finish is enabled during operation");

        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    qDebug() << "Test 2 continues";

    verifyInstallSuccess("instDialogInstallTest::testInstall2()");

    QTest::qWait(500);

    QVERIFY(next->isEnabled());
    QVERIFY(!dlg->findChild<QPushButton*>("finishButton")->isEnabled());

    QVERIFY(QFile::exists("test1.fw"));
    QVERIFY(QFile::remove("test1.fw"));
    QFile testfile("test1.fw");
    testfile.open(QFile::WriteOnly);
    testfile.write("#!/bin/sh\n#\n#  This is automatically generated file. DO NOT MODIFY !\n#\n#  Firewall Builder  fwb_ipt v4.0.0-2784\n#\n#  Generated Wed Mar 31 16:41:46 2010 EEST by a2k\n#\n# files: * test1.fw\n#\n# Compiled for iptables (any version)\n#\n# This firewall has two interfaces. Eth0 faces outside and has a dynamic address; eth1 faces inside.\n# Policy includes basic rules to permit unrestricted outbound access and anti-spoofing rules. Access to the firewall is permitted only from internal network and only using SSH. The firewall uses one of the machines on internal network for DNS. Internal network is configured with address 192.168.1.0/255.255.255.0\n\n\necho \"Testing policy activation script\"\n");
    testfile.close();
    testfile.setPermissions(testfile.permissions() | QFile::ExeOwner);

    QTimer::singleShot(200, this, SLOT(instOptionsForTest2()));

    next->click();
    QTest::qWait(500);

    waited = 0;
    while (!checkProgress(list))
    {
        QVERIFY(back->isEnabled() == false);
        QVERIFY(next->isEnabled() == false);
        QVERIFY(cancel->isEnabled() == true);
        QVERIFY2(finish->isEnabled() == false, "Button Finish is enabled during operation");

        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    QTest::qWait(500);
    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Failure");
    }
    if (ssh_auth_sock != nullptr)
        setenv("SSH_AUTH_SOCK", ssh_auth_sock, 1);
    else
        unsetenv("SSH_AUTH_SOCK");
    QString text = dlg->findChild<QTextBrowser*>("procLogDisplay")->toPlainText();
    QVERIFY(!text.isEmpty());

//     foreach(QString line, text.split("\n"))
//         qDebug() << line;

    QVERIFY(text.contains("lost connection"));
    QVERIFY(text.contains("SSH session terminated, exit status: 1"));

    dlg->reject();
    QTest::qWait(500);

    qDebug() << "Test 2 done";
}

/*
 * Another kind of error is a timeout.

  * To simulate timeout repeat the test with an ip address that does
  * not exist. Ssh should time out after some considerable time with
  * appropriate error message in the output. Check for this message
  * and make sure at that point button "Finish" is enabled and status
  * is "Failed" (or is it "Error" ? )

  * this test also tests custom scp command line argument
  * addition. The argument is ConnectTimeout, we both test the code
  * path that adds custom arguments for scp and make the test run
  * faster by shirtening the timeout.

*/
void instDialogInstallTest::testInstall3()
{
    qDebug() << "Test 3 begins";

    resetDialogs();

    removeFiles();
    Firewall *test1 = nullptr;
    foreach(FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == "test1")
        {
            test1 = Firewall::cast(fw);
            break;
        }
    }
    QVERIFY(test1 != nullptr);

    // reset additional args for scp and ssh
    FWOptions *fwoptions = test1->getOptionsObject();
    fwoptions->setStr("scpArgs", "");
    fwoptions->setStr("sshArgs", "");

    fwoptions->setStr("firewall_dir", (QDir::currentPath()+"/test_install").toStdString());

    // reduce timeout time to make test run faster
    fwoptions->setStr("scpArgs", "-o ConnectTimeout=2");

    mw->findChild<QAction*>("installAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton17")->click();

    QPushButton *back = dlg->findChild<QPushButton*>("backButton");
    QPushButton *next = dlg->findChild<QPushButton*>("nextButton");
    QPushButton *finish = dlg->findChild<QPushButton*>("finishButton");
    QPushButton *cancel = dlg->findChild<QPushButton*>("cancelButton");

    QTreeWidget *selectTable = dlg->findChild<QTreeWidget*>("selectTable");
    QTreeWidgetItem *test1item = selectTable->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first();
    test1item->setCheckState(1, Qt::Checked);
    test1item->setCheckState(2, Qt::Checked);
    QTest::qWait(500);
    next->click();

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");

    int waited = 0;

    while (!checkProgress(list))
    {
        QVERIFY(back->isEnabled() == true);
        QVERIFY(next->isEnabled() == false);
        QVERIFY(cancel->isEnabled() == true);
        QVERIFY2(finish->isEnabled() == false, "Button Finish is enabled during operation");

        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    verifyInstallSuccess("instDialogInstallTest::testInstall3()");

    QTest::qWait(500);

    QVERIFY(next->isEnabled());
    QVERIFY(!finish->isEnabled());

    QVERIFY(QFile::exists("test1.fw"));
    QVERIFY(QFile::remove("test1.fw"));
    QFile testfile("test1.fw");
    testfile.open(QFile::WriteOnly);
    testfile.write("#!/bin/sh\n#\n#  This is automatically generated file. DO NOT MODIFY !\n#\n#  Firewall Builder  fwb_ipt v4.0.0-2784\n#\n#  Generated Wed Mar 31 16:41:46 2010 EEST by a2k\n#\n# files: * test1.fw\n#\n# Compiled for iptables (any version)\n#\n# This firewall has two interfaces. Eth0 faces outside and has a dynamic address; eth1 faces inside.\n# Policy includes basic rules to permit unrestricted outbound access and anti-spoofing rules. Access to the firewall is permitted only from internal network and only using SSH. The firewall uses one of the machines on internal network for DNS. Internal network is configured with address 192.168.1.0/255.255.255.0\n\n\necho \"Testing policy activation script\"\n");
    testfile.close();
    testfile.setPermissions(testfile.permissions() | QFile::ExeOwner);

    QTimer::singleShot(200, this, SLOT(instOptionsForTest3()));

    next->click();
    QTest::qWait(500);

    waited = 0;
    while (!checkProgress(list))
    {
        QVERIFY(back->isEnabled() == false);
        QVERIFY(next->isEnabled() == false);
        QVERIFY(cancel->isEnabled() == true);
        QVERIFY2(finish->isEnabled() == false, "Button Finish is enabled during operation");

        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 1000000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Failure");
    }

    QString text = dlg->findChild<QTextBrowser*>("procLogDisplay")->toPlainText();
    // check that additional scp command line argument was indeed used
    QVERIFY(text.contains("-o ConnectTimeout=2"));
    // check that ssh timed out as expected
    QVERIFY(text.contains("lost connection"));
    QVERIFY(text.contains("SSH session terminated, exit status: 1"));
    QVERIFY(!text.isEmpty());
    dlg->reject();
    QTest::qWait(500);
}


/*
 * Test cancellation.
 *
 * This function does two tests: it runs install but hits Cancel or Stop
 * button (per @button_name argument) to interrupt the process and then
 * checks that the dialog is still open or not (per @dialog_should_stay_open
 * arg)
 *
 * Repeat the test using address that does not exist but click
 * "Cancel" 1 sec after you start installation (this is too short for
 * the ssh timeout to occur). Wait after that until the line "Stopping
 * background process" appears, then check that the installer dialog
 * closes.
 *
 */

void instDialogInstallTest::executeCancelAndStopTests(const QString &button_name,
                                                      bool dialog_should_stay_open)
{
    resetDialogs();

    removeFiles();
    Firewall *test1 = nullptr;
    foreach(FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == "test1")
        {
            test1 = Firewall::cast(fw);
            break;
        }
    }
    QVERIFY(test1 != nullptr);

    // reset additional args for scp and ssh
    FWOptions *fwoptions = test1->getOptionsObject();
    fwoptions->setStr("scpArgs", "");
    fwoptions->setStr("sshArgs", "");

    fwoptions->setStr("firewall_dir", (QDir::currentPath()+"/test_install").toStdString());

    mw->findChild<QAction*>("installAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton17")->click();

    QPushButton *back = dlg->findChild<QPushButton*>("backButton");
    QPushButton *next = dlg->findChild<QPushButton*>("nextButton");
    QPushButton *finish = dlg->findChild<QPushButton*>("finishButton");
    QPushButton *cancel = dlg->findChild<QPushButton*>("cancelButton");

    QTreeWidget *selectTable = dlg->findChild<QTreeWidget*>("selectTable");
    QTreeWidgetItem *test1item = selectTable->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first();
    test1item->setCheckState(1, Qt::Checked);
    test1item->setCheckState(2, Qt::Checked);
    QTest::qWait(500);
    next->click();

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");

    int waited = 0;

    while (!checkProgress(list))
    {
        QVERIFY(back->isEnabled() == true);
        QVERIFY(next->isEnabled() == false);
        QVERIFY(cancel->isEnabled() == true);
        QVERIFY2(finish->isEnabled() == false, "Button Finish is enabled during operation");

        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Success");
    }
    QTest::qWait(500);

    QVERIFY(next->isEnabled());
    QVERIFY(!finish->isEnabled());

    QVERIFY(QFile::exists("test1.fw"));
    QVERIFY(QFile::remove("test1.fw"));
    QFile testfile("test1.fw");
    testfile.open(QFile::WriteOnly);
    testfile.write("#!/bin/sh\n#\n#  This is automatically generated file. DO NOT MODIFY !\n#\n#  Firewall Builder  fwb_ipt v4.0.0-2784\n#\n#  Generated Wed Mar 31 16:41:46 2010 EEST by a2k\n#\n# files: * test1.fw\n#\n# Compiled for iptables (any version)\n#\n# This firewall has two interfaces. Eth0 faces outside and has a dynamic address; eth1 faces inside.\n# Policy includes basic rules to permit unrestricted outbound access and anti-spoofing rules. Access to the firewall is permitted only from internal network and only using SSH. The firewall uses one of the machines on internal network for DNS. Internal network is configured with address 192.168.1.0/255.255.255.0\n\n\necho \"Testing policy activation script\"\n");
    testfile.close();
    testfile.setPermissions(testfile.permissions() | QFile::ExeOwner);

    QTimer::singleShot(200, this, SLOT(instOptionsForTest4()));

    next->click();

    QTest::qWait(1000);
    // Now click button to interrupt the process
    dlg->findChild<QPushButton*>(button_name)->click();
    QTest::qWait(2000);

    QString text = dlg->findChild<QTextBrowser*>("procLogDisplay")->toPlainText();
    QVERIFY(text.contains("Stopping background process"));
    QTest::qWait(500);
    QVERIFY(mw->findChild<instDialog*>()->isVisible() == dialog_should_stay_open);
    QTest::qWait(500);
}


void instDialogInstallTest::testInstall4()
{
    executeCancelAndStopTests("cancelButton", false);
}

/*
 * Test "Stop" button. This is just like "Cancel", except the dialog
 * stays open.
 *
 */
void instDialogInstallTest::testInstall5()
{
    executeCancelAndStopTests("stopButton", true);
}
