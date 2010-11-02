/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: instDialogInspectTest.h 2786 2010-04-01 14:05:36Z a2k $

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
#include "ObjectManipulator.h"
#include "ObjectTreeView.h"
#include "ObjectTreeViewItem.h"
#include "events.h"
#include "fwbuilder/Firewall.h"
#include "fwbuilder/Library.h"
#include "fwbuilder/Policy.h"
#include "instDialog.h"
#include "newClusterDialog.h"
#include "upgradePredicate.h"

class instDialogInspectTest : public QObject
{
    Q_OBJECT;
    bool dialogClosed;
    ObjectManipulator *om;
    ObjectTreeView *tree;
    QStringList filesToCleanup;
private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInspect_cluster();
    void testInspect_firewall();
    void testInspect_space();
    void testInspect_ascii();
    void testInspect_russian();
public slots:
    void closeInstallOptions();
    void closeContextMenu();
    void openContextMenu(ObjectManipulator *om, ObjectTreeViewItem *item,
                         ObjectTreeView *tree, const QString &actionText);
    void testInspect(QString fwname);
};

#endif // INSTDIALOGTEST_H
