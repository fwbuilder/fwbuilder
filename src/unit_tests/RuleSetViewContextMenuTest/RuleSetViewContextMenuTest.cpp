/*

                          Firewall Builder

                 Copyright (C) 2010 NetCitadel, LLC

  Author:  Roman Bovsunivskiy     a2k0001@gmail.com

  $Id: RuleSetViewContextMenuTest.cpp 2786 2010-04-01 14:05:36Z a2k $

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

#include "RuleSetViewContextMenuTest.h"

#include "unistd.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDesktopWidget>
#include <QDebug>
#include <QMenuBar>
#include <QInputDialog>
#include <QApplication>
#include <QCoreApplication>

#include "FWBApplication.h"
#include "StartTipDialog.h"
#include "QMetaProperty"
#include "FWObjectClipboard.h"
#include "RuleSetModel.h"

using namespace std;
using namespace QTest;
using namespace libfwbuilder;

#define getRuleForPosition( x ) ((RuleSetModel*)view->model())->findRuleForPosition( x )
#define showContextMenu( x ) view->showContextMenu(view->pos() + x);


void RuleSetViewContextMenuTest::initTestCase()
{
    new FWObjectClipboard();
    mw = new FWWindow();
    mw->show();
    mw->move(0,0);
    if (app->desktop()->size().width() < 1024 || app->desktop()->size().height()<768)
        mw->resize(app->desktop()->size());
    else
        mw->resize(1024, 768);
    mw->startupLoad();
    StartTipDialog *d = mw->findChild<StartTipDialog*>();
    if (d!=nullptr) d->close();
    om = dynamic_cast<ObjectManipulator*>(mw->getCurrentObjectTree()->parent()->parent());
    firewall = Firewall::cast(om->createObject(FWBTree().getStandardSlotForObject(findUserLibrary(), Firewall::TYPENAME), Firewall::TYPENAME, "testFirewall"));
    firewall->setStr("platform", "iptables");
    openPolicy("testFirewall");
    view = mw->findChild<RuleSetView*>("");
    om->closeObject();
}

/*
 * Opens Policy of firewall with name fwname in RuleSetView of active project
 */
void RuleSetViewContextMenuTest::openPolicy(QString fwname)
{
    Policy *p = nullptr;
    foreach (FWObject *fw, mw->db()->getByTypeDeep(Firewall::TYPENAME))
    {
        if (fw->getName() == fwname.toStdString())
        {
            p = Firewall::cast(fw)->getPolicy();
        }
    }
    QVERIFY (p != nullptr);
    QCoreApplication::postEvent(mw, new openRulesetImmediatelyEvent(mw->activeProject()->getFileName(), p->getId()));
    QTest::qWait(100);
}

/*
 * Returns libfwbuilder::Library with name "User"
 */
Library* RuleSetViewContextMenuTest::findUserLibrary()
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

/*
 * Returns menu-relative position of topmost pixel clicking
 * which will trigger action
 */
QPoint findActionPos(QMenu *menu, QAction *action)
{
    int x = menu->width()/2;
    for (int y=0; y<menu->height(); y++)
    {
        if (menu->actionAt(QPoint(x,y)) != nullptr && menu->actionAt(QPoint(x,y))->text() == action->text())
            return QPoint(x,y);
    }
    return QPoint(-1,-1);
}

/*
 * Clicks item in topmost visible modal context menu.
 * Should be called right before opening menu (it waits 10 ms for menu to appear).
 */
void RuleSetViewContextMenuTest::clickMenuItem(QString item)
{
    clicked = false;
    itemToClick = item;
    QTimer::singleShot(100, this, SLOT(actuallyClickMenuItem()));
}

void RuleSetViewContextMenuTest::actuallyClickMenuItem()
{
    QMenu *menu = dynamic_cast<QMenu*>(app->activePopupWidget());
    Q_ASSERT(menu != nullptr);
    foreach(QAction *action, menu->actions())
    {
        if (action->text() == itemToClick)
        {
            //qDebug() << "clicking menu item" << action << action->text();
            clicked = true;
            QPoint pos = findActionPos(menu, action);
            //qDebug() << pos;
            //QTest::mouseMove(menu, pos);
            QTest::mouseClick(menu, Qt::LeftButton, Qt::NoModifier, pos);
            //action->trigger();
            //menu->hide();
            return;
        }
    }
    // menu item not found. Include menu items that were actually
    // present in the menu in the error message to implify debugging
    QStringList items;
    foreach(QAction *action, menu->actions())
    {
        items.push_back(QString("\"%1\"").arg(action->text()));
    }

    // need to hide the menu, otherwise test just hangs
    menu->hide();
    QFAIL(QString("Menu item %1 not found. Menu consists of: %2")
          .arg(itemToClick)
          .arg(items.join(" ")).toLatin1().constData());
}

/*
 * Returns topmost pixel which belongs to first cell
 * of rule in RuleSetView
 */
QPoint RuleSetViewContextMenuTest::findRulePosition(Rule *rule)
{
    int x = 30;
    view->scrollTo(((RuleSetModel*)view->model())->index(rule, 0));
    for (int y=view->header()->height(); y<view->height(); y+=5)
    {
        Rule *found = ((RuleSetModel*)view->model())->getRule(view->indexAt(QPoint(x,y)));
        if (found == rule)
            return QPoint(x,y);
    }
    return QPoint(-1,-1);
}

/*
 * Returns topmost pixel which belongs to first cell
 * of row #rule in RuleSetView
 */
QPoint RuleSetViewContextMenuTest::findRulePosition(int rule)
{
    return findRulePosition(getRuleForPosition(rule));
}

/*
 * Fills group name request modal dialog with group name
 * and clicks OK.
 * Should be executed right before opening dialog.
 */
void RuleSetViewContextMenuTest::createGroup(QString name)
{
    groupToCreate = name;
    // timeout should be more than clickMenuItem timeout
    QTimer::singleShot(200, this, SLOT(actuallyCreateGroup()));
}

void RuleSetViewContextMenuTest::actuallyCreateGroup()
{
    QInputDialog *dlg = dynamic_cast<QInputDialog*>(app->activeModalWidget());
    Q_ASSERT(dlg != nullptr);
    QLineEdit *name = dlg->findChild<QLineEdit*>();
    QTest::keyClicks(name, groupToCreate);
    dlg->accept();
}

QPoint RuleSetViewContextMenuTest::findCell(Rule *rule, int col)
{
    view->scrollTo(((RuleSetModel*)view->model())->index(rule, col));
    for (int x=0; x<view->width(); x++)
    {
        for(int y=view->height(); y>0; y--)
        {
            QModelIndex index = view->indexAt(QPoint(x,y));
            Rule *found = ((RuleSetModel*)view->model())->getRule(view->indexAt(QPoint(x,y)));
            if (found == rule && index.column() == col)
                return QPoint(x,y);
        }
    }
    return QPoint(-1,-1);
}

void RuleSetViewContextMenuTest::verifyMenu(int column)
{
    names.clear();
    if (column == 0)
    {
        names << "New Group" << "Change color" << "Insert New Rule"
              << "Add New Rule Below"
              << "Remove Rule" << "Move Rule Up" << "Move Rule Down"
              << "Copy Rule"
              << "Cut Rule"
              << "Paste Rule Above" << "Paste Rule Below"
              << "Disable Rule"
              << "Compile rule";
    }
    if (column > 0 && (column < 5 || column == 7))
    {
        names << "Edit" << "Copy" << "Cut" << "Paste"
              << "Delete"<< "Where used"
              << "Reveal in tree" << "Negate" << "Compile rule";
    }
    if (column == 5)
    {
        names << "Inbound" << "Outbound" << "Both" << "Compile rule";
    }
    if (column == 6)
    {
        QMap<QString, QString> possibleItems;
        QStringList order;
        order << "Accept" << "Deny" << "Reject" << "Accounting" << "Pipe"
                << "Tag" << "Classify" << "Custom" << "Branch" << "Route" << "Continue";
        possibleItems["Accept"] = getActionNameForPlatform(firewall, PolicyRule::getActionAsString(PolicyRule::Accept));
        possibleItems["Deny"] = getActionNameForPlatform(firewall, PolicyRule::getActionAsString(PolicyRule::Deny));
        possibleItems["Reject"] = getActionNameForPlatform(firewall, PolicyRule::getActionAsString(PolicyRule::Reject));
        possibleItems["Accounting"] = getActionNameForPlatform(firewall, PolicyRule::getActionAsString(PolicyRule::Accounting));
        possibleItems["Pipe"] = getActionNameForPlatform(firewall, PolicyRule::getActionAsString(PolicyRule::Pipe));
        possibleItems["Custom"] = getActionNameForPlatform(firewall, PolicyRule::getActionAsString(PolicyRule::Custom));
        possibleItems["Branch"] = getActionNameForPlatform(firewall, PolicyRule::getActionAsString(PolicyRule::Branch));
        possibleItems["Continue"] = getActionNameForPlatform(firewall, PolicyRule::getActionAsString(PolicyRule::Continue));
        string currentPlatform = firewall->getStr("platform");
        foreach(QString item, order)
            if (Resources::isTargetActionSupported(currentPlatform, item.toStdString()))
            {
                //qDebug() << item << "is valid for" << currentPlatform.c_str() << "with name" << possibleItems[item];
                names.append(possibleItems[item]);
            }
        names << "Parameters" << "Compile rule";
    }
    if (column == 8)
    {
        names << "Rule options" << "Logging On" << "Logging Off" << "Compile Rule";
    }
    if (column == 9)
    {
        names << "Edit" << "Compile Rule";
    }
    QStringList groupCommon;
    groupCommon << "Change color" << "Insert New Rule" << "Add New Rule Below"
            << "Remove Rule" << "Move Rule up" << "Move Rule down" << "Copy Rule"
            << "Cut Rule" << "Paste Rule Above" << "Paste Rule Below" << "Disable Rule"
            << "Compile rule";
    if (column == -1) // column 0 for rule above group
    {
        names << "New group" << "Add to the group Test Group Name" << groupCommon;
    }
    if (column == -2) // column 0 for first rule in group
    {
        names << "Remove from the group" << groupCommon;
    }
    if (column == -3) // rule in the middle of the group
    {
        names << groupCommon;
    }
    if (column == -4)
    {
        names << "Remove from the group" << groupCommon;
    }
    if (column == -5)
    {
        names << "New group" << "Add to the group Test Group Name" << groupCommon;
    }
    QTimer::singleShot(150, this, SLOT(actuallyVerifyMenu()));
}

void RuleSetViewContextMenuTest::actuallyVerifyMenu()
{
    QMenu *menu = dynamic_cast<QMenu*>(app->activePopupWidget());
    menu->hide();
    int j =0;
    for (int i=0; i<menu->actions().size(); i++)
    {
        if (menu->actions()[i]->text().isEmpty() || !menu->actions()[i]->isVisible())
            continue;
        if (menu->actions()[i]->text().toLower() != names[j++].toLower())
        {
            failed = true;
            qDebug() << QString("Menu item not found or item order is wrong for item: %1").arg(names[j-1]);
        }
    }
    QVERIFY(j == names.size());
}

/*
 * Scrolls view to bottom and returns middle pixel of view's middle line.
 * There
 */
QPoint RuleSetViewContextMenuTest::getViewBottomPoint()
{
    view->scrollToBottom();
    view->viewport()->mapToParent(QPoint(view->viewport()->width()/2, view->viewport()->height()-1));
    return QPoint(view->viewport()->width()/2, view->viewport()->height()-1);
}


void RuleSetViewContextMenuTest::test_menus()
{
    failed = false;
    // Adding one new rule
    clickMenuItem("Insert New Rule");
    showContextMenu(getViewBottomPoint());
    QVERIFY(view->model()->rowCount(QModelIndex()) == 1);
    Rule *rule = ((RuleSetModel*)view->model())->findRuleForPosition(0);

    // Verify columns 1..5 and 7..10
    for (int i=0; i<10; i++)
    {
        if (i==6) continue; // it depens on platform
        qDebug() << "Verifying context menu for column" << i;
        verifyMenu(i);
        showContextMenu(findCell(rule, i));
        QVERIFY2(!failed, QString("Failed for column %1").arg(i).toLatin1().constData());
    }

    // Remove created rule
    clickMenuItem("Remove Rule");
    showContextMenu(findRulePosition(rule));
}

void RuleSetViewContextMenuTest::test_group_menus()
{
    failed = false;

    // Adding five new rules
    Rule *rules[5];
    clickMenuItem("Insert New Rule");
    showContextMenu(getViewBottomPoint());
    rules[0] = getRuleForPosition(0);
    for (int i=0; i<4; i++)
    {
        clickMenuItem("Add New Rule at the Bottom");
        showContextMenu(getViewBottomPoint());
        rules[i+1] = getRuleForPosition(i+1);
    }

    // Create new group
    view->selectRE(rules[2], 0);
    createGroup("Test Group Name");
    clickMenuItem("New Group");
    showContextMenu(findRulePosition(rules[3]));
    // Add two rules to group
    view->selectRE(rules[1], 0);
    clickMenuItem("Add To the Group Test Group Name");
    showContextMenu(findRulePosition(rules[1]));
    view->selectRE(rules[3], 0);
    clickMenuItem("Add To the Group Test Group Name");
    showContextMenu(findRulePosition(rules[3]));

    view->expandAll();

    // Check that columns 2..10 acts same for grouped and ungrouped items
    for (int j=0; j<5; j++)
    {
        // Verify columns 1..5 and 7..10
        for (int i=1; i<10; i++)
        {
            qDebug() << "Verifying context menu for column" << i;
            verifyMenu(i);
            showContextMenu(findCell(getRuleForPosition(j), i));
            QVERIFY2(!failed, QString("Failed for column %1").arg(i).toLatin1().constData());
        }
    }

    // Verify column 1 for all rules. They all should be different.
    for (int i=0; i<5; i++)
    {
        qDebug() << "Verifying rule #" <<i;
        verifyMenu(-i-1);
        view->selectRE(getRuleForPosition(i),0);
    //    QTest::qWait(1000);
        showContextMenu(findCell(getRuleForPosition(i), 0));
        QVERIFY2(!failed, QString("Failed for rule #%1 of 5").arg(i+1).toLatin1().constData());
    }

    // remove created rules
    for (int i=1; i<4; i++)
    {
        clickMenuItem("Remove From the Group");
        view->selectRE(rules[i],0);
        showContextMenu(findCell(rules[i],0))
    }
    for (int i=0; i<5; i++)
    {
        clickMenuItem("Remove Rule");
        view->selectRE(rules[i],0);
        showContextMenu(findCell(rules[i],0))
    }
}

void RuleSetViewContextMenuTest::test_platforms()
{
    failed = false;
    // Adding one new rule
    clickMenuItem("Insert New Rule");
    showContextMenu(getViewBottomPoint());
    QVERIFY(view->model()->rowCount(QModelIndex()) == 1);
    Rule *rule = ((RuleSetModel*)view->model())->findRuleForPosition(0);

    // Verify column 6 for all platforms
    QMap<QString, QString> platforms = getAllPlatforms();
    foreach(QString platform, platforms.keys())
    {
        qDebug() << "Verifying conext menu for column 6 and platform" << platform;
        firewall->setStr("platform", platform.toStdString());
        verifyMenu(6);
        showContextMenu(findCell(rule, 6));
        QVERIFY2(!failed, QString("Failed for paltform %1").arg(platform).toLatin1().constData());
    }

    // Verify that column count changes depending of firewall platform
    QVERIFY(view->model()->columnCount() == 10);
    firewall->setStr("platform", "unknown");
    openPolicy("testFirewall");
    view = mw->findChild<RuleSetView*>("");
    QVERIFY(view->model()->columnCount() == 8);
    firewall->setStr("platform", "ipf");
    openPolicy("testFirewall");
    view = mw->findChild<RuleSetView*>("");
    QVERIFY(view->model()->columnCount() == 9);
    firewall->setStr("platform", "unknown");
    openPolicy("testFirewall");
    view = mw->findChild<RuleSetView*>("");

    // Remove created rule
    clickMenuItem("Remove Rule");
    showContextMenu(findRulePosition(rule));
}
