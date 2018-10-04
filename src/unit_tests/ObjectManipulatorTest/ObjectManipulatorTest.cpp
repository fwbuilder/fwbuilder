/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id$

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

#include "ObjectManipulatorTest.h"

//#include "../../global.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QTest>
#include <iostream>

#include <QProcess>
#include <QRegExp>
#include <QDebug>

#include "FWWindow.h"
#include "ProjectPanel.h"
#include "ObjectTreeView.h"
#include "ObjectTreeViewItem.h"
#include "ObjectEditor.h"
#include "FWObjectClipboard.h"

#include "fwbuilder/RuleSet.h"

using namespace std;
using namespace libfwbuilder;


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

void ObjectManipulatorTest::editSelectedObject()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->loadFile("test.fwb", false);
    ObjectTreeView *tree = mw->getCurrentObjectTree();
    tree->expandAll();

    ObjectTreeViewItem *policy = dynamic_cast<ObjectTreeViewItem*>(
            tree->findItems("Policy", Qt::MatchRecursive | Qt::MatchExactly, 0).first());
    ObjectTreeViewItem *fw = dynamic_cast<ObjectTreeViewItem*>(
            tree->findItems("TestFirewall", Qt::MatchRecursive | Qt::MatchExactly, 0).first());

    ObjectManipulator *om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    tree->setCurrentItem(
        policy, 0, QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);

    om->editSelectedObject();
    QTest::qWait(100);
    QVERIFY(mw->getOpenedEditor() == nullptr);
    QVERIFY(mw->activeProject()->getCurrentRuleSet() == RuleSet::cast(policy->getFWObject()));

    om->editSelectedObject();
    QTest::qWait(100);
    QVERIFY(mw->getOpenedEditor() == policy->getFWObject());
    mw->closeEditor();

    tree->setCurrentItem(
        fw, 0, QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    om->editSelectedObject();
    QTest::qWait(100);
    QVERIFY(mw->getOpenedEditor() == fw->getFWObject());
}
