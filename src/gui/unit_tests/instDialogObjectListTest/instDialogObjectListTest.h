/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: instDialogObjectListTest.h 2786 2010-04-01 14:05:36Z a2k $

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
#include <QTreeWidget>

#include "newClusterDialog.h"
#include "upgradePredicate.h"
#include "FWBTree.h"
#include "fwbuilder/Library.h"
#include "instDialog.h"
#include "FWWindow.h"
#include "ObjectTreeView.h"
#include "ObjectTreeViewItem.h"
#include "events.h"

#include "fwbuilder/Firewall.h"
#include "fwbuilder/Policy.h"

class instDialogObjectListTest : public QObject
{
    Q_OBJECT
    const char *ssh_auth_sock;
    void openPolicy(QString fw);

    void verifyCompileCheckboxes(QTreeWidget *table, int items = -1);
    void verifyCompileCheckboxes_2(QTreeWidget *table);
    void verifyInstallCheckboxes(QTreeWidget *table, int items = -1);

    void openContextMenu(ObjectManipulator *om, ObjectTreeViewItem *item, ObjectTreeView *tree, const QString &actionText);
    void removeFiles();

private slots:
    void initTestCase();
    void test_compile_1();
    void test_compile_2();
    void test_compile_3();
    void test_compile_4();
    void test_compile_5();
    void test_compile_6();
    void test_compile_7();
    void test_install_1();
    void test_actually_compile_1();

public slots:
    void closeContextMenu();
};

#endif // INSTDIALOGTEST_H
