/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: instDialogClusterTest.h 2786 2010-04-01 14:05:36Z a2k $

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

class instDialogClusterTest : public QObject
{
    Q_OBJECT
    const char *ssh_auth_sock;
    void openPolicy(QString fw);
    void verifyDialog(instDialog *dlg, int items = -1);
    void openContextMenu(ObjectManipulator *om, ObjectTreeViewItem *item, ObjectTreeView *tree, const QString &actionText);
    void removeFiles();

private slots:
    void initTestCase();

    void page1_8();
    void page1_9();
    void page1_10();
    void page1_11();

public slots:
    void closeContextMenu();
};

#endif // INSTDIALOGTEST_H
