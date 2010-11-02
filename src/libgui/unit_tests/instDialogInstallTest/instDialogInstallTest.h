/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: instDialogInstallTest.h 2786 2010-04-01 14:05:36Z a2k $

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

#ifndef INSTDIALOGTEST_H
#define INSTDIALOGTEST_H

#include <QTest>

#include "FWBTree.h"
#include "FWWindow.h"
#include "ObjectTreeView.h"
#include "ObjectTreeViewItem.h"
#include "events.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Policy.h"
#include "instDialog.h"
#include "newClusterDialog.h"
#include "upgradePredicate.h"

class instDialogInstallTest : public QObject
{
    Q_OBJECT;
    const char *ssh_auth_sock;
    void removeFiles();
    void verifyInstallSuccess(const QString &test_name);
    void resetDialogs();
    void fillInstOptionsDialog(const QString &user_name,
                               const QString &pwd,
                               const QString &alt_address,
                               bool verbose);
    void executeCancelAndStopTests(const QString &button_name,
                                   bool dialog_should_stay_open);
    
private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInstall2();
    void testInstall3();
    void testInstall4();
    void testInstall5();

public slots:

    void instOptionsForTest1();
    void instOptionsForTest2();
    void instOptionsForTest3();
    void instOptionsForTest4();
    void instOptionsForTest5();
    
};

#endif // INSTDIALOGTEST_H
