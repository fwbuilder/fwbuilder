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

  test1

  * click "Install" toolbar button
  * uncheck all checkboxes
  * check "compile" and "install" for test1
  * click Next
  * check that the status is "Success" in the list on the left
  * check that "Next" button is enabled and "Finish" button is disabled
  * check that file test1.fw has been created in directory "test_install"
    - delete file test1.fw in "test_install"
    - create this file again, with contents as follows:

#!/bin/sh
echo "Testing policy activation script"

  * set executable bit on this file (equivalent to chmod +x)
  * now click "Next"
  * set user name to the name of the account that is running the test
  * leave password empty
  * replace the address of the firewall with 127.0.0.1
  * click OK to start install
  * note that since you left password empty, you should have ssh authentication via ssh-agent set up for this to work
  * read lines from the installer progress output. Look for the line "Testing policy activation script". If you can't find it, fail the test
  * in all tests verify status in the column on the left.
  * most likely during debugging you are going to run into different errors with ssh. Make a note of these errors and add lines to the test to fail it if they appear in the installer progress output
    - delete file test1.fw in "test_install" and delete directory "test_install"

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

void instDialogInstallTest::initTestCase()
{
    mw = new FWWindow();
    mw->show();
    mw->loadFile("test_work.fwb", false);
    FWObjectClipboard *clip = new FWObjectClipboard();
    ssh_auth_sock = getenv("SSH_AUTH_SOCK");
}

void instDialogInstallTest::cleanupTestCase()
{
    setenv("SSH_AUTH_SOCK", ssh_auth_sock, 1);
    QDir().rmdir("test_install");
    QFile::remove("test1.fw");
}

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

void instDialogInstallTest::testInstall1()
{
    removeFiles();
    Firewall *test1 = NULL;
    foreach(FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == "test1")
        {
            test1 = Firewall::cast(fw);
            break;
        }
    }
    QVERIFY(test1 != NULL);
    FWOptions *fwoptions = test1->getOptionsObject();
    fwoptions->setStr("firewall_dir", (QDir::currentPath()+"/test_install").toStdString());

    mw->findChild<QAction*>("installAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton17")->click();

    QTreeWidget *selectTable = dlg->findChild<QTreeWidget*>("selectTable");
    QTreeWidgetItem *test1item = selectTable->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first();
    test1item->setCheckState(1, Qt::Checked);
    test1item->setCheckState(2, Qt::Checked);
    QTest::qWait(500);
    dlg->findChild<QPushButton*>("nextButton")->click();

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");
    QTextBrowser *processLogDisplay = dlg->findChild<QTextBrowser*>("procLogDisplay");

    QVERIFY(list!=NULL);
    QVERIFY(processLogDisplay!=NULL);

    int waited = 0;

    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Success");
    }
    QTest::qWait(500);

    QVERIFY(dlg->findChild<QPushButton*>("nextButton")->isEnabled());
    QVERIFY(!dlg->findChild<QPushButton*>("finishButton")->isEnabled());

    QVERIFY(QFile::exists("test1.fw"));
    QVERIFY(QFile::remove("test1.fw"));
    QFile testfile("test1.fw");
    testfile.open(QFile::WriteOnly);
    testfile.write("#!/bin/sh\n#\n#  This is automatically generated file. DO NOT MODIFY !\n#\n#  Firewall Builder  fwb_ipt v4.0.0-2784\n#\n#  Generated Wed Mar 31 16:41:46 2010 EEST by a2k\n#\n# files: * test1.fw\n#\n# Compiled for iptables (any version)\n#\n# This firewall has two interfaces. Eth0 faces outside and has a dynamic address; eth1 faces inside.\n# Policy includes basic rules to permit unrestricted outbound access and anti-spoofing rules. Access to the firewall is permitted only from internal network and only using SSH. The firewall uses one of the machines on internal network for DNS. Internal network is configured with address 192.168.1.0/255.255.255.0\n\n\necho \"Testing policy activation script\"\n");
    testfile.close();
    testfile.setPermissions(testfile.permissions() | QFile::ExeOwner);

    QTimer::singleShot(200, this, SLOT(testInstall1_part2()));
    dlg->findChild<QPushButton*>("nextButton")->click();
    // now runs testInstall_part2 and fills install options dialog
    waited = 0;
    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY2(list->topLevelItem(i)->text(1) == "Success",
                 QString("%1\n").arg(processLogDisplay->toPlainText()).toAscii().constData()
        );
    }

    QString text = dlg->findChild<QTextBrowser*>("procLogDisplay")->toPlainText();
    QVERIFY(text.contains("Testing policy activation script"));
    QVERIFY(!text.contains("*** Fatal error"));
    QVERIFY(!text.isEmpty());
    dlg->reject();
    QTest::qWait(1000);
}

void instDialogInstallTest::testInstall1_part2() // sets install options in modal dialog
{
    instOptionsDialog *optdlg = mw->findChild<instOptionsDialog*>("instOptionsDialog_q");
    QLineEdit *uname = optdlg->findChild<QLineEdit*>("uname");
    QLineEdit *pwd = optdlg->findChild<QLineEdit*>("pwd");
    QLineEdit *altAddress = optdlg->findChild<QLineEdit*>("altAddress");
    uname->setText("root");//getenv("USER"));
    altAddress->setText("127.0.0.1");
    optdlg->findChild<QPushButton*>("okButton")->click();
}

void instDialogInstallTest::testInstall2()
{
    removeFiles();
    Firewall *test1 = NULL;
    foreach(FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == "test1")
        {
            test1 = Firewall::cast(fw);
            break;
        }
    }
    QVERIFY(test1 != NULL);
    FWOptions *fwoptions = test1->getOptionsObject();
    fwoptions->setStr("firewall_dir", (QDir::currentPath()+"/test_install").toStdString());

    setenv("SSH_AUTH_SOCK", "nothinghere", 1);
    QTest::qWait(500);
    mw->findChild<QAction*>("installAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton17")->click();

    QTreeWidget *selectTable = dlg->findChild<QTreeWidget*>("selectTable");
    QTreeWidgetItem *test1item = selectTable->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first();
    test1item->setCheckState(1, Qt::Checked);
    test1item->setCheckState(2, Qt::Checked);
    QTest::qWait(500);
    dlg->findChild<QPushButton*>("nextButton")->click();

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");

    int waited = 0;

    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Success");
    }
    QTest::qWait(1000);

    QVERIFY(dlg->findChild<QPushButton*>("nextButton")->isEnabled());
    QVERIFY(!dlg->findChild<QPushButton*>("finishButton")->isEnabled());

    QVERIFY(QFile::exists("test1.fw"));
    QVERIFY(QFile::remove("test1.fw"));
    QFile testfile("test1.fw");
    testfile.open(QFile::WriteOnly);
    testfile.write("#!/bin/sh\n#\n#  This is automatically generated file. DO NOT MODIFY !\n#\n#  Firewall Builder  fwb_ipt v4.0.0-2784\n#\n#  Generated Wed Mar 31 16:41:46 2010 EEST by a2k\n#\n# files: * test1.fw\n#\n# Compiled for iptables (any version)\n#\n# This firewall has two interfaces. Eth0 faces outside and has a dynamic address; eth1 faces inside.\n# Policy includes basic rules to permit unrestricted outbound access and anti-spoofing rules. Access to the firewall is permitted only from internal network and only using SSH. The firewall uses one of the machines on internal network for DNS. Internal network is configured with address 192.168.1.0/255.255.255.0\n\n\necho \"Testing policy activation script\"\n");
    testfile.close();
    testfile.setPermissions(testfile.permissions() | QFile::ExeOwner);

    QTimer::singleShot(200, this, SLOT(testInstall1_part2()));
    dlg->findChild<QPushButton*>("nextButton")->click();
    // now runs testInstall_part2 and fills install options dialog
    waited = 0;
    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    QTest::qWait(500);
    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Failure");
    }
    setenv("SSH_AUTH_SOCK", ssh_auth_sock, 1);
    QString text = dlg->findChild<QTextBrowser*>("procLogDisplay")->toPlainText();
    QVERIFY(!text.isEmpty());
    QVERIFY(text.contains("lost connection"));
    QVERIFY(text.contains("SSH session terminated, exit status: 1"));

    dlg->reject();
    QTest::qWait(1000);
}


void instDialogInstallTest::testInstall3()
{
    removeFiles();
    Firewall *test1 = NULL;
    foreach(FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == "test1")
        {
            test1 = Firewall::cast(fw);
            break;
        }
    }
    QVERIFY(test1 != NULL);
    FWOptions *fwoptions = test1->getOptionsObject();
    fwoptions->setStr("firewall_dir", (QDir::currentPath()+"/test_install").toStdString());

    mw->findChild<QAction*>("installAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton17")->click();

    QTreeWidget *selectTable = dlg->findChild<QTreeWidget*>("selectTable");
    QTreeWidgetItem *test1item = selectTable->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first();
    test1item->setCheckState(1, Qt::Checked);
    test1item->setCheckState(2, Qt::Checked);
    QTest::qWait(500);
    dlg->findChild<QPushButton*>("nextButton")->click();

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");

    int waited = 0;

    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Success");
    }
    QTest::qWait(500);

    QVERIFY(dlg->findChild<QPushButton*>("nextButton")->isEnabled());
    QVERIFY(!dlg->findChild<QPushButton*>("finishButton")->isEnabled());

    QVERIFY(QFile::exists("test1.fw"));
    QVERIFY(QFile::remove("test1.fw"));
    QFile testfile("test1.fw");
    testfile.open(QFile::WriteOnly);
    testfile.write("#!/bin/sh\n#\n#  This is automatically generated file. DO NOT MODIFY !\n#\n#  Firewall Builder  fwb_ipt v4.0.0-2784\n#\n#  Generated Wed Mar 31 16:41:46 2010 EEST by a2k\n#\n# files: * test1.fw\n#\n# Compiled for iptables (any version)\n#\n# This firewall has two interfaces. Eth0 faces outside and has a dynamic address; eth1 faces inside.\n# Policy includes basic rules to permit unrestricted outbound access and anti-spoofing rules. Access to the firewall is permitted only from internal network and only using SSH. The firewall uses one of the machines on internal network for DNS. Internal network is configured with address 192.168.1.0/255.255.255.0\n\n\necho \"Testing policy activation script\"\n");
    testfile.close();
    testfile.setPermissions(testfile.permissions() | QFile::ExeOwner);

    QTimer::singleShot(200, this, SLOT(testInstall3_part2()));
    dlg->findChild<QPushButton*>("nextButton")->click();
    // now runs testInstall_part2 and fills install options dialog
    waited = 0;
    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 1000000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Failure");
    }

    QString text = dlg->findChild<QTextBrowser*>("procLogDisplay")->toPlainText();
    QVERIFY(text.contains("lost connection"));
    QVERIFY(text.contains("SSH session terminated, exit status: 1"));
    QVERIFY(!text.isEmpty());
    dlg->reject();
    QTest::qWait(1000);
}

void instDialogInstallTest::testInstall3_part2() // sets install options in modal dialog
{
    instOptionsDialog *optdlg = mw->findChild<instOptionsDialog*>("instOptionsDialog_q");
    QLineEdit *uname = optdlg->findChild<QLineEdit*>("uname");
    QLineEdit *pwd = optdlg->findChild<QLineEdit*>("pwd");
    QLineEdit *altAddress = optdlg->findChild<QLineEdit*>("altAddress");
    uname->setText("root");
    //uname->setText(getenv("USER"));
    altAddress->setText("1.2.3.4");
    optdlg->findChild<QPushButton*>("okButton")->click();
}

void instDialogInstallTest::testInstall4()
{
    removeFiles();
    Firewall *test1 = NULL;
    foreach(FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == "test1")
        {
            test1 = Firewall::cast(fw);
            break;
        }
    }
    QVERIFY(test1 != NULL);
    FWOptions *fwoptions = test1->getOptionsObject();
    fwoptions->setStr("firewall_dir", (QDir::currentPath()+"/test_install").toStdString());

    mw->findChild<QAction*>("installAction")->trigger();
    QTest::qWait(500);

    instDialog *dlg = mw->findChild<instDialog*>();
    dlg->findChild<QPushButton*>("pushButton17")->click();

    QTreeWidget *selectTable = dlg->findChild<QTreeWidget*>("selectTable");
    QTreeWidgetItem *test1item = selectTable->findItems("test1", Qt::MatchExactly | Qt::MatchRecursive, 0).first();
    test1item->setCheckState(1, Qt::Checked);
    test1item->setCheckState(2, Qt::Checked);
    QTest::qWait(500);
    dlg->findChild<QPushButton*>("nextButton")->click();

    QTreeWidget *list= dlg->findChild<QTreeWidget*>("fwWorkList");

    int waited = 0;

    while (!checkProgress(list))
    {
        QTest::qWait(500);
        waited += 500;
        QVERIFY(waited < 10000);
    }

    for(int i=0; i<list->topLevelItemCount(); i++)
    {
        QVERIFY(list->topLevelItem(i)->text(1) == "Success");
    }
    QTest::qWait(500);

    QVERIFY(dlg->findChild<QPushButton*>("nextButton")->isEnabled());
    QVERIFY(!dlg->findChild<QPushButton*>("finishButton")->isEnabled());

    QVERIFY(QFile::exists("test1.fw"));
    QVERIFY(QFile::remove("test1.fw"));
    QFile testfile("test1.fw");
    testfile.open(QFile::WriteOnly);
    testfile.write("#!/bin/sh\n#\n#  This is automatically generated file. DO NOT MODIFY !\n#\n#  Firewall Builder  fwb_ipt v4.0.0-2784\n#\n#  Generated Wed Mar 31 16:41:46 2010 EEST by a2k\n#\n# files: * test1.fw\n#\n# Compiled for iptables (any version)\n#\n# This firewall has two interfaces. Eth0 faces outside and has a dynamic address; eth1 faces inside.\n# Policy includes basic rules to permit unrestricted outbound access and anti-spoofing rules. Access to the firewall is permitted only from internal network and only using SSH. The firewall uses one of the machines on internal network for DNS. Internal network is configured with address 192.168.1.0/255.255.255.0\n\n\necho \"Testing policy activation script\"\n");
    testfile.close();
    testfile.setPermissions(testfile.permissions() | QFile::ExeOwner);

    QTimer::singleShot(200, this, SLOT(testInstall3_part2()));
    dlg->findChild<QPushButton*>("nextButton")->click();
    // now runs testInstall_part2 and fills install options dialog
    QTest::qWait(1000);
    dlg->findChild<QPushButton*>("cancelButton")->click();
    QString text = dlg->findChild<QTextBrowser*>("procLogDisplay")->toPlainText();
    QVERIFY(text.contains("Stopping background process"));
    QTest::qWait(3000);
    QVERIFY(mw->findChild<instDialog*>()->isVisible() == false); // check that dialog is closed
    QTest::qWait(1000);
}
